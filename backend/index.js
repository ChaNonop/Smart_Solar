require('dotenv').config();
const express = require('express');
const cors = require('cors');
const path = require('path');
const mqtt = require('mqtt');
const cron = require('node-cron');
const db = require('./database');

const app = express();
const PORT = process.env.PORT || 5000;
const MQTT_BROKER = process.env.MQTT_BROKER || 'mqtt://broker.hivemq.com:1883';

app.use(cors());
app.use(express.json());

// Serve static files from React frontend build
app.use(express.static(path.join(__dirname, '../frontend/dist')));

// ----------------------------------------------------
// MQTT Client Configuration & Logic
// ----------------------------------------------------
const mqttClient = mqtt.connect(MQTT_BROKER);

mqttClient.on('connect', () => {
  console.log(`Connected to MQTT Broker at ${MQTT_BROKER}`);
  // Subscribe to all incoming sensor telemetry
  mqttClient.subscribe('solarcharger/sensors/+', (err) => {
    if (err) console.error('Subscription error:', err);
    else console.log('Subscribed to solarcharger/sensors/+');
  });
});

mqttClient.on('message', (topic, message) => {
  try {
    const parts = topic.split('/');
    if (parts.length < 3) return;
    
    const mac = parts[2];
    const payload = JSON.parse(message.toString());
    
    // Check if the node is registered
    db.get('SELECT * FROM nodes WHERE mac = ?', [mac], (err, node) => {
      if (err) {
        console.error('Database query error:', err.message);
        return;
      }
      
      if (!node) {
        console.log(`Received data from unregistered node MAC: ${mac}. Discarding.`);
        return;
      }
      
      // Save raw telemetry data
      const query = `
        INSERT INTO telemetry_raw (mac, v_solar, i_solar, i_charge, v_bat, v_load, i_load, lux, relay1, relay2)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
      `;
      const params = [
        mac,
        payload.v_solar || 0.0,
        payload.i_solar || 0.0,
        payload.i_charge || 0.0,
        payload.v_bat || 0.0,
        payload.v_load || 0.0,
        payload.i_load || 0.0,
        payload.lux || 0,
        payload.relay1 !== undefined ? payload.relay1 : 0,
        payload.relay2 !== undefined ? payload.relay2 : 0
      ];
      
      db.run(query, params, function(insertErr) {
        if (insertErr) {
          console.error('Error inserting raw telemetry:', insertErr.message);
          return;
        }
        
        // ADR-0001 implementation: Check for relay state mismatch and auto republish if needed
        const currentRelay1 = payload.relay1 || 0;
        const currentRelay2 = payload.relay2 || 0;
        
        if (currentRelay1 !== node.relay1_command || currentRelay2 !== node.relay2_command) {
          console.log(`Relay mismatch detected for ${mac}. Current [R1:${currentRelay1}, R2:${currentRelay2}] vs Target [R1:${node.relay1_command}, R2:${node.relay2_command}]. Re-publishing command...`);
          
          mqttClient.publish(`solarcharger/control/${mac}`, JSON.stringify({
            relay1: node.relay1_command,
            relay2: node.relay2_command
          }));
        }
      });
    });
  } catch (e) {
    console.error('Failed to process MQTT message:', e.message);
  }
});

// ----------------------------------------------------
// REST API Endpoints
// ----------------------------------------------------

// 1. Get all registered nodes with their latest telemetry
app.get('/api/nodes', (req, res) => {
  const query = `
    SELECT n.*, t.v_solar, t.i_solar, t.i_charge, t.v_bat, t.v_load, t.i_load, t.lux, t.relay1, t.relay2, t.timestamp as telemetry_timestamp
    FROM nodes n
    LEFT JOIN (
      SELECT * FROM telemetry_raw WHERE id IN (SELECT MAX(id) FROM telemetry_raw GROUP BY mac)
    ) t ON n.mac = t.mac
    ORDER BY n.created_at DESC
  `;
  db.all(query, [], (err, rows) => {
    if (err) return res.status(500).json({ error: err.message });
    res.json(rows);
  });
});

// 2. Register a new node
app.post('/api/nodes', (req, res) => {
  const { mac, name, solar_v_avg, solar_i_avg, battery_v, battery_capacity } = req.body;
  if (!mac || !name) {
    return res.status(400).json({ error: 'MAC address and Name are required' });
  }

  const query = `
    INSERT INTO nodes (mac, name, solar_v_avg, solar_i_avg, battery_v, battery_capacity)
    VALUES (?, ?, ?, ?, ?, ?)
  `;
  db.run(query, [mac.trim(), name.trim(), solar_v_avg || null, solar_i_avg || null, battery_v || null, battery_capacity || null], function(err) {
    if (err) {
      if (err.message.includes('UNIQUE constraint failed')) {
        return res.status(400).json({ error: 'A node with this MAC address already exists' });
      }
      return res.status(500).json({ error: err.message });
    }
    res.status(201).json({ message: 'Node registered successfully', mac });
  });
});

// 3. Delete a node
app.delete('/api/nodes/:mac', (req, res) => {
  const { mac } = req.params;
  db.run('DELETE FROM nodes WHERE mac = ?', [mac], function(err) {
    if (err) return res.status(500).json({ error: err.message });
    res.json({ message: 'Node deleted successfully' });
  });
});

// 4. Get latest telemetry for a node
app.get('/api/nodes/:mac/telemetry/realtime', (req, res) => {
  const { mac } = req.params;
  db.get('SELECT * FROM telemetry_raw WHERE mac = ? ORDER BY timestamp DESC LIMIT 1', [mac], (err, row) => {
    if (err) return res.status(500).json({ error: err.message });
    if (!row) return res.status(404).json({ error: 'No telemetry data found for this node' });
    res.json(row);
  });
});

// 5. Get historical telemetry (1-day, 7-day, 30-day views)
app.get('/api/nodes/:mac/telemetry/history', (req, res) => {
  const { mac } = req.params;
  const { range } = req.query; // '1d', '7d', '30d', or '90d'
  
  let query = '';
  let interval = '1 day';
  
  if (range === '7d') interval = '7 days';
  else if (range === '30d') interval = '30 days';
  else if (range === '90d') interval = '90 days';

  // For queries <= 30 days, we pull from telemetry_raw.
  // For queries > 30 days, we pull from telemetry_hourly to optimize performance.
  if (range === '90d') {
    query = `
      SELECT avg_v_solar AS v_solar, avg_i_solar AS i_solar, avg_i_charge AS i_charge, 
             avg_v_bat AS v_bat, avg_v_load AS v_load, avg_i_load AS i_load, avg_lux AS lux,
             timestamp
      FROM telemetry_hourly
      WHERE mac = ? AND timestamp >= datetime('now', '-90 days')
      ORDER BY timestamp ASC
    `;
  } else {
    query = `
      SELECT v_solar, i_solar, i_charge, v_bat, v_load, i_load, lux, relay1, relay2, timestamp
      FROM telemetry_raw
      WHERE mac = ? AND timestamp >= datetime('now', '-${interval}')
      ORDER BY timestamp ASC
    `;
  }

  db.all(query, [mac], (err, rows) => {
    if (err) return res.status(500).json({ error: err.message });
    res.json(rows);
  });
});

// 6. Control relays
app.post('/api/nodes/:mac/control', (req, res) => {
  const { mac } = req.params;
  const { relay1, relay2 } = req.body; // Expects 0 or 1 for each

  db.get('SELECT * FROM nodes WHERE mac = ?', [mac], (err, node) => {
    if (err) return res.status(500).json({ error: err.message });
    if (!node) return res.status(404).json({ error: 'Node not found' });

    const newR1 = relay1 !== undefined ? relay1 : node.relay1_command;
    const newR2 = relay2 !== undefined ? relay2 : node.relay2_command;

    db.run(
      'UPDATE nodes SET relay1_command = ?, relay2_command = ? WHERE mac = ?',
      [newR1, newR2, mac],
      function(updateErr) {
        if (updateErr) return res.status(500).json({ error: updateErr.message });
        
        // Publish target states to MQTT broker for the ESP to catch
        mqttClient.publish(`solarcharger/control/${mac}`, JSON.stringify({
          relay1: newR1,
          relay2: newR2
        }));

        res.json({ message: 'Relay commands updated and sent', relay1: newR1, relay2: newR2 });
      }
    );
  });
});

// Serve React index.html for non-API routes (SPA Routing support)
app.use((req, res) => {
  res.sendFile(path.join(__dirname, '../frontend/dist/index.html'));
});

// ----------------------------------------------------
// node-cron Background Scheduler (Data Retention)
// ----------------------------------------------------
// Runs daily at 00:00 (Midnight)
cron.schedule('0 0 * * *', () => {
  console.log('[Cron] Running daily database aggregation and cleanup...');
  
  // 1. Move raw data older than 1 month into hourly averages inside telemetry_hourly
  // We compute the average of every hour for each node
  const aggregationQuery = `
    INSERT INTO telemetry_hourly (mac, avg_v_solar, avg_i_solar, avg_i_charge, avg_v_bat, avg_v_load, avg_i_load, avg_lux, timestamp)
    SELECT 
      mac,
      AVG(v_solar) as avg_v_solar,
      AVG(i_solar) as avg_i_solar,
      AVG(i_charge) as avg_i_charge,
      AVG(v_bat) as avg_v_bat,
      AVG(v_load) as avg_v_load,
      AVG(i_load) as avg_i_load,
      AVG(lux) as avg_lux,
      strftime('%Y-%m-%d %H:00:00', timestamp) as hour_timestamp
    FROM telemetry_raw
    WHERE timestamp < datetime('now', '-30 days')
    GROUP BY mac, hour_timestamp
  `;

  db.run(aggregationQuery, [], function(err) {
    if (err) {
      console.error('[Cron Error] Failed to aggregate telemetry:', err.message);
      return;
    }
    console.log(`[Cron] Aggregated raw records into telemetry_hourly: ${this.changes} rows created.`);

    // 2. Delete raw records older than 30 days
    db.run("DELETE FROM telemetry_raw WHERE timestamp < datetime('now', '-30 days')", [], function(delRawErr) {
      if (delRawErr) console.error('[Cron Error] Failed to delete old raw telemetry:', delRawErr.message);
      else console.log(`[Cron] Deleted ${this.changes} expired raw rows.`);
    });

    // 3. Delete hourly aggregated data older than 3 months (90 days)
    db.run("DELETE FROM telemetry_hourly WHERE timestamp < datetime('now', '-90 days')", [], function(delHourlyErr) {
      if (delHourlyErr) console.error('[Cron Error] Failed to delete expired hourly telemetry:', delHourlyErr.message);
      else console.log(`[Cron] Deleted ${this.changes} expired aggregated rows.`);
    });
  });
});

// Start the server
app.listen(PORT, () => {
  console.log(`Backend server is running on port ${PORT}`);
});
