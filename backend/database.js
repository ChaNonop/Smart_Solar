const sqlite3 = require('sqlite3').verbose();
const path = require('path');

const DB_PATH = path.join(__dirname, 'solar_iot.db');

const db = new sqlite3.Database(DB_PATH, (err) => {
  if (err) {
    console.error('Error opening database:', err.message);
  } else {
    console.log('Connected to the SQLite database.');
  }
});

// Enable foreign keys
db.run('PRAGMA foreign_keys = ON');

db.serialize(() => {
  // 1. Create nodes table
  db.run(`
    CREATE TABLE IF NOT EXISTS nodes (
      mac TEXT PRIMARY KEY,
      name TEXT NOT NULL,
      solar_v_avg REAL,
      solar_i_avg REAL,
      battery_v REAL,
      battery_capacity REAL,
      relay1_command INTEGER DEFAULT 0,
      relay2_command INTEGER DEFAULT 0,
      created_at DATETIME DEFAULT CURRENT_TIMESTAMP
    )
  `);

  // 2. Create raw telemetry table (1 month retention)
  db.run(`
    CREATE TABLE IF NOT EXISTS telemetry_raw (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      mac TEXT NOT NULL,
      v_solar REAL,
      i_solar REAL,
      i_charge REAL,
      v_bat REAL,
      v_load REAL,
      i_load REAL,
      lux REAL,
      relay1 INTEGER,
      relay2 INTEGER,
      timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
      FOREIGN KEY(mac) REFERENCES nodes(mac) ON DELETE CASCADE
    )
  `);

  // Indexing timestamp and mac for optimization
  db.run(`CREATE INDEX IF NOT EXISTS idx_telemetry_raw_mac_ts ON telemetry_raw(mac, timestamp)`);

  // 3. Create hourly telemetry table (3 months retention)
  db.run(`
    CREATE TABLE IF NOT EXISTS telemetry_hourly (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      mac TEXT NOT NULL,
      avg_v_solar REAL,
      avg_i_solar REAL,
      avg_i_charge REAL,
      avg_v_bat REAL,
      avg_v_load REAL,
      avg_i_load REAL,
      avg_lux REAL,
      timestamp DATETIME NOT NULL,
      FOREIGN KEY(mac) REFERENCES nodes(mac) ON DELETE CASCADE
    )
  `);

  db.run(`CREATE INDEX IF NOT EXISTS idx_telemetry_hourly_mac_ts ON telemetry_hourly(mac, timestamp)`);
});

module.exports = db;
