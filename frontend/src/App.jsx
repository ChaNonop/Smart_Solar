import React, { useState, useEffect } from 'react';
import Navbar from './components/Navbar';
import StatsOverview from './components/StatsOverview';
import DashboardGrid from './components/DashboardGrid';
import DeviceDetailsModal from './components/DeviceDetailsModal';

// Dynamically target the backend port 8000 on the same host machine (e.g., localhost or Raspberry Pi IP)
const BACKEND_URL = `http://${window.location.hostname}:8000`;

export default function App() {
  const [devices, setDevices] = useState([]);
  const [isBackendConnected, setIsBackendConnected] = useState(false);
  const [selectedDeviceId, setSelectedDeviceId] = useState(null);

  // Fetch the latest status of all 5 devices from the backend
  const fetchDevices = async () => {
    try {
      const res = await fetch(`${BACKEND_URL}/api/devices`);
      if (res.ok) {
        const data = await res.json();
        setDevices(data);
        setIsBackendConnected(true);
      } else {
        setIsBackendConnected(false);
      }
    } catch (err) {
      setIsBackendConnected(false);
    }
  };

  // Toggle the relay on a specific device
  const handleToggleRelay = async (deviceId, status) => {
    try {
      const res = await fetch(`${BACKEND_URL}/api/devices/${deviceId}/relay`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ relay: status }),
      });
      if (res.ok) {
        // Speculatively update local state immediately for instant feedback
        setDevices(prev => 
          prev.map(d => d.device_id === deviceId ? { ...d, relay_status: status } : d)
        );
        // Trigger a background refresh
        fetchDevices();
        return true;
      }
      return false;
    } catch (err) {
      console.error("Relay toggle request error:", err);
      return false;
    }
  };

  useEffect(() => {
    fetchDevices();
    // Poll the backend API every 2 seconds for live telemetry updates
    const interval = setInterval(fetchDevices, 2000);
    return () => clearInterval(interval);
  }, []);

  return (
    <div className="min-h-screen bg-[#080d1a] text-slate-100 flex flex-col font-sans antialiased">
      {/* Background Decorative Glow */}
      <div className="absolute top-0 left-1/4 w-[500px] height-[500px] bg-indigo-500/10 rounded-full blur-[120px] pointer-events-none -z-10" />
      <div className="absolute bottom-0 right-1/4 w-[600px] height-[600px] bg-emerald-500/5 rounded-full blur-[140px] pointer-events-none -z-10" />

      <Navbar isBackendConnected={isBackendConnected} />

      <main className="flex-1 mx-auto max-w-7xl w-full px-4 sm:px-6 lg:px-8 py-8">
        {!isBackendConnected && (
          <div className="mb-6 p-4 rounded-xl bg-red-500/10 border border-red-500/20 text-red-300 text-sm flex items-center gap-3 shadow-lg glow-red">
            <div className="w-2.5 h-2.5 rounded-full bg-red-500 animate-pulse" />
            <div>
              <span className="font-bold">Connection Warning:</span> Unable to connect to the FastAPI backend at <span className="font-mono">{BACKEND_URL}</span>.
              Please check if the backend server is running and accessible.
            </div>
          </div>
        )}

        {/* Aggregate statistics */}
        <StatsOverview devices={devices} />

        {/* 5-Node Monitoring Grid */}
        <div className="mt-8">
          <DashboardGrid 
            devices={devices} 
            onSelectDevice={setSelectedDeviceId} 
            onToggleRelay={handleToggleRelay}
          />
        </div>
      </main>

      {/* Footer info */}
      <footer className="border-t border-slate-800/80 bg-slate-950/40 py-6">
        <div className="mx-auto max-w-7xl px-4 sm:px-6 lg:px-8 flex flex-col md:flex-row items-center justify-between text-xs text-slate-500 gap-4">
          <div className="flex items-center gap-1.5">
            <span className="inline-block w-2 h-2 rounded-full bg-indigo-500" />
            <span>Target Broker Host: <span className="font-mono text-slate-400">broker.hivemq.com</span></span>
          </div>
          <div>
            <span>Developed with React, FastAPI, SQLite, and MQTT</span>
          </div>
          <div>
            <span>Solar Dashboard v1.0.0</span>
          </div>
        </div>
      </footer>

      {/* Detailed Modal Popup for Charts */}
      {selectedDeviceId && (
        <DeviceDetailsModal 
          deviceId={selectedDeviceId}
          onClose={() => setSelectedDeviceId(null)}
          backendUrl={BACKEND_URL}
        />
      )}
    </div>
  );
}
