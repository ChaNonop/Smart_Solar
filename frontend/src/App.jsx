import React, { useState, useEffect } from 'react';
import Navbar from './components/Navbar';
import StatsOverview from './components/StatsOverview';
import DashboardGrid from './components/DashboardGrid';
import DeviceDetailsModal from './components/DeviceDetailsModal';
import { Plus, X, Cpu } from 'lucide-react';

const BACKEND_URL = `http://${window.location.hostname}:5000`;

export default function App() {
  const [nodes, setNodes] = useState([]);
  const [isBackendConnected, setIsBackendConnected] = useState(false);
  const [selectedMac, setSelectedMac] = useState(null);
  const [showAddModal, setShowAddModal] = useState(false);

  // Form states for adding a node
  const [mac, setMac] = useState('');
  const [name, setName] = useState('');
  const [solarVAvg, setSolarVAvg] = useState('');
  const [solarIAvg, setSolarIAvg] = useState('');
  const [batteryV, setBatteryV] = useState('');
  const [batteryCapacity, setBatteryCapacity] = useState('');
  const [formError, setFormError] = useState('');

  // Fetch registered nodes (with their latest telemetry)
  const fetchNodes = async () => {
    try {
      const res = await fetch(`${BACKEND_URL}/api/nodes`);
      if (res.ok) {
        const data = await res.json();
        
        // Transform backend fields to standard frontend attributes
        const processedNodes = data.map(node => {
          const isOnline = node.telemetry_timestamp && 
            (Date.now() - new Date(node.telemetry_timestamp).getTime() < 15000); // 15 seconds timeout
          
          return {
            mac: node.mac,
            name: node.name,
            solar_v_avg: node.solar_v_avg,
            solar_i_avg: node.solar_i_avg,
            battery_v: node.battery_v,
            battery_capacity: node.battery_capacity,
            relay1_command: node.relay1_command,
            relay2_command: node.relay2_command,
            is_online: isOnline,
            
            // Latest Telemetry values
            v_solar: node.v_solar || 0.0,
            i_solar: node.i_solar || 0.0,
            i_charge: node.i_charge || 0.0,
            v_bat: node.v_bat || 0.0,
            v_load: node.v_load || 0.0,
            i_load: node.i_load || 0.0,
            lux: node.lux || 0,
            relay1: node.relay1 || 0,
            relay2: node.relay2 || 0,
            
            // Calculations
            solar_power: (node.v_solar || 0.0) * (node.i_solar || 0.0),
            battery_power: (node.v_bat || 0.0) * (node.i_charge || 0.0),
            load_power: (node.v_load || 0.0) * (node.i_load || 0.0)
          };
        });

        setNodes(processedNodes);
        setIsBackendConnected(true);
      } else {
        setIsBackendConnected(false);
      }
    } catch (err) {
      setIsBackendConnected(false);
    }
  };

  // Toggle relay1 or relay2 on a specific node
  const handleToggleRelay = async (macAddress, relayNum, currentStatus) => {
    try {
      const targetState = currentStatus === 1 ? 0 : 1;
      const body = {};
      if (relayNum === 1) body.relay1 = targetState;
      if (relayNum === 2) body.relay2 = targetState;

      const res = await fetch(`${BACKEND_URL}/api/nodes/${macAddress}/control`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(body),
      });

      if (res.ok) {
        // Optimistic local state update
        setNodes(prev => 
          prev.map(n => {
            if (n.mac === macAddress) {
              return {
                ...n,
                relay1: relayNum === 1 ? targetState : n.relay1,
                relay2: relayNum === 2 ? targetState : n.relay2
              };
            }
            return n;
          })
        );
        fetchNodes();
        return true;
      }
      return false;
    } catch (err) {
      console.error("Relay toggle request error:", err);
      return false;
    }
  };

  // Add new node submission
  const handleAddNode = async (e) => {
    e.preventDefault();
    setFormError('');
    if (!mac || !name) {
      setFormError('MAC Address and Node Name are required.');
      return;
    }

    // Basic MAC address format check (AA:BB:CC:DD:EE:FF or simple 12 hex chars)
    const macRegex = /^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$/;
    if (!macRegex.test(mac)) {
      setFormError('Please enter a valid MAC address format (e.g. 00:1A:2B:3C:4D:01)');
      return;
    }

    try {
      const res = await fetch(`${BACKEND_URL}/api/nodes`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          mac: mac.toUpperCase().trim(),
          name: name.trim(),
          solar_v_avg: parseFloat(solarVAvg) || null,
          solar_i_avg: parseFloat(solarIAvg) || null,
          battery_v: parseFloat(batteryV) || null,
          battery_capacity: parseFloat(batteryCapacity) || null
        })
      });

      if (res.ok) {
        // Reset form states
        setMac('');
        setName('');
        setSolarVAvg('');
        setSolarIAvg('');
        setBatteryV('');
        setBatteryCapacity('');
        setShowAddModal(false);
        fetchNodes();
      } else {
        const errData = await res.json();
        setFormError(errData.error || 'Failed to register node.');
      }
    } catch (err) {
      setFormError('Network error registering node.');
    }
  };

  // Delete node
  const handleDeleteNode = async (macAddress) => {
    if (!window.confirm(`Are you sure you want to remove node ${macAddress}?`)) return;
    try {
      const res = await fetch(`${BACKEND_URL}/api/nodes/${macAddress}`, {
        method: 'DELETE'
      });
      if (res.ok) {
        fetchNodes();
      }
    } catch (err) {
      console.error('Delete error:', err);
    }
  };

  useEffect(() => {
    fetchNodes();
    const interval = setInterval(fetchNodes, 5000);
    return () => clearInterval(interval);
  }, []);

  return (
    <div className="min-h-screen bg-[#080d1a] text-slate-100 flex flex-col font-sans antialiased relative">
      {/* Background Decorative Glow */}
      <div className="absolute top-0 left-1/4 w-[500px] h-[500px] bg-indigo-500/10 rounded-full blur-[120px] pointer-events-none -z-10" />
      <div className="absolute bottom-0 right-1/4 w-[600px] h-[600px] bg-emerald-500/5 rounded-full blur-[140px] pointer-events-none -z-10" />

      <Navbar isBackendConnected={isBackendConnected} />

      <main className="flex-1 mx-auto max-w-7xl w-full px-4 sm:px-6 lg:px-8 py-8">
        {!isBackendConnected && (
          <div className="mb-6 p-4 rounded-xl bg-red-500/10 border border-red-500/20 text-red-300 text-sm flex items-center gap-3 shadow-lg glow-red">
            <div className="w-2.5 h-2.5 rounded-full bg-red-500 animate-pulse" />
            <div>
              <span className="font-bold">Connection Warning:</span> Unable to connect to the Express backend at <span className="font-mono">{BACKEND_URL}</span>.
              Please check if the backend server is running and accessible.
            </div>
          </div>
        )}

        {/* Aggregate statistics */}
        <StatsOverview nodes={nodes} />

        {/* 5-Node Monitoring Grid */}
        <div className="mt-8">
          <div className="flex items-center justify-between mb-6">
            <div>
              <h3 className="text-xl font-bold text-slate-100 tracking-wide flex items-center gap-2">
                <Cpu className="h-5 w-5 text-indigo-400" />
                Active Solar Energy Stations
              </h3>
              <p className="text-xs text-slate-400 mt-1">
                Click a card to view detailed historical charts and logs
              </p>
            </div>
            
            <button
              onClick={() => setShowAddModal(true)}
              className="flex items-center gap-2 bg-indigo-600 hover:bg-indigo-500 text-white rounded-xl px-4 py-2 text-sm font-bold shadow-lg shadow-indigo-600/20 transition-all active:scale-95"
            >
              <Plus className="h-4 w-4" />
              Add Solar Node
            </button>
          </div>

          <DashboardGrid 
            nodes={nodes} 
            onSelectNode={setSelectedMac} 
            onToggleRelay={handleToggleRelay}
            onDeleteNode={handleDeleteNode}
          />
        </div>
      </main>

      {/* Add Node Modal */}
      {showAddModal && (
        <div className="fixed inset-0 z-50 flex items-center justify-center bg-slate-950/80 p-4 backdrop-blur-sm">
          <div className="w-full max-w-md rounded-2xl border border-slate-800 bg-slate-900 shadow-2xl overflow-hidden p-6">
            <div className="flex items-center justify-between mb-4 pb-3 border-b border-slate-800">
              <h3 className="text-lg font-bold text-slate-100">Register New Solar Node</h3>
              <button onClick={() => setShowAddModal(false)} className="text-slate-400 hover:text-white">
                <X className="h-5 w-5" />
              </button>
            </div>

            {formError && (
              <div className="mb-4 p-3 bg-red-500/10 border border-red-500/20 rounded-xl text-xs text-red-400">
                {formError}
              </div>
            )}

            <form onSubmit={handleAddNode} className="space-y-4 text-sm">
              <div>
                <label className="block text-xs font-semibold text-slate-400 mb-1">ESP MAC ADDRESS (Required)</label>
                <input
                  type="text"
                  placeholder="e.g. 00:1A:2B:3C:4D:01"
                  value={mac}
                  onChange={(e) => setMac(e.target.value)}
                  className="w-full bg-slate-950 border border-slate-800 rounded-xl px-3 py-2 text-slate-100 focus:outline-none focus:border-indigo-500"
                />
              </div>

              <div>
                <label className="block text-xs font-semibold text-slate-400 mb-1">NODE NAME / LOCATION (Required)</label>
                <input
                  type="text"
                  placeholder="e.g. Rooftop Station #1"
                  value={name}
                  onChange={(e) => setName(e.target.value)}
                  className="w-full bg-slate-950 border border-slate-800 rounded-xl px-3 py-2 text-slate-100 focus:outline-none focus:border-indigo-500"
                />
              </div>

              <div className="grid grid-cols-2 gap-4">
                <div>
                  <label className="block text-xs font-semibold text-slate-400 mb-1">Solar V Avg (V)</label>
                  <input
                    type="number"
                    step="0.1"
                    placeholder="e.g. 18.0"
                    value={solarVAvg}
                    onChange={(e) => setSolarVAvg(e.target.value)}
                    className="w-full bg-slate-950 border border-slate-800 rounded-xl px-3 py-2 text-slate-100 focus:outline-none focus:border-indigo-500"
                  />
                </div>
                <div>
                  <label className="block text-xs font-semibold text-slate-400 mb-1">Solar I Avg (A)</label>
                  <input
                    type="number"
                    step="0.1"
                    placeholder="e.g. 3.0"
                    value={solarIAvg}
                    onChange={(e) => setSolarIAvg(e.target.value)}
                    className="w-full bg-slate-950 border border-slate-800 rounded-xl px-3 py-2 text-slate-100 focus:outline-none focus:border-indigo-500"
                  />
                </div>
              </div>

              <div className="grid grid-cols-2 gap-4">
                <div>
                  <label className="block text-xs font-semibold text-slate-400 mb-1">Battery V (V)</label>
                  <input
                    type="number"
                    step="0.1"
                    placeholder="e.g. 12.8"
                    value={batteryV}
                    onChange={(e) => setBatteryV(e.target.value)}
                    className="w-full bg-slate-950 border border-slate-800 rounded-xl px-3 py-2 text-slate-100 focus:outline-none focus:border-indigo-500"
                  />
                </div>
                <div>
                  <label className="block text-xs font-semibold text-slate-400 mb-1">Battery Cap (Ah)</label>
                  <input
                    type="number"
                    step="0.1"
                    placeholder="e.g. 100"
                    value={batteryCapacity}
                    onChange={(e) => setBatteryCapacity(e.target.value)}
                    className="w-full bg-slate-950 border border-slate-800 rounded-xl px-3 py-2 text-slate-100 focus:outline-none focus:border-indigo-500"
                  />
                </div>
              </div>

              <div className="pt-4 flex gap-3 justify-end border-t border-slate-800">
                <button
                  type="button"
                  onClick={() => setShowAddModal(false)}
                  className="bg-slate-800 hover:bg-slate-700 text-slate-300 rounded-xl px-4 py-2 font-semibold"
                >
                  Cancel
                </button>
                <button
                  type="submit"
                  className="bg-indigo-600 hover:bg-indigo-500 text-white rounded-xl px-4 py-2 font-bold shadow-lg shadow-indigo-600/20"
                >
                  Enroll Node
                </button>
              </div>
            </form>
          </div>
        </div>
      )}

      {/* Footer info */}
      <footer className="border-t border-slate-800/80 bg-slate-950/40 py-6 mt-12">
        <div className="mx-auto max-w-7xl px-4 sm:px-6 lg:px-8 flex flex-col md:flex-row items-center justify-between text-xs text-slate-500 gap-4">
          <div className="flex items-center gap-1.5">
            <span className="inline-block w-2 h-2 rounded-full bg-indigo-500" />
            <span>Target Broker Host: <span className="font-mono text-slate-400">broker.hivemq.com</span></span>
          </div>
          <div>
            <span>Developed with React, Express, SQLite, and MQTT</span>
          </div>
          <div>
            <span>Solar Dashboard v2.0.0 (Dynamic Grid)</span>
          </div>
        </div>
      </footer>

      {/* Detailed Modal Popup for Charts */}
      {selectedMac && (
        <DeviceDetailsModal 
          mac={selectedMac}
          onClose={() => setSelectedMac(null)}
          backendUrl={BACKEND_URL}
        />
      )}
    </div>
  );
}
