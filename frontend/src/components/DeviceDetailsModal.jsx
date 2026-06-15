import React, { useState, useEffect } from 'react';
import { X, Calendar, Database, RefreshCw, Sun, BatteryCharging, Power } from 'lucide-react';
import { 
  ResponsiveContainer, 
  LineChart, 
  Line, 
  XAxis, 
  YAxis, 
  CartesianGrid, 
  Tooltip, 
  Legend 
} from 'recharts';

export default function DeviceDetailsModal({ deviceId, onClose, backendUrl }) {
  const [history, setHistory] = useState([]);
  const [loading, setLoading] = useState(true);
  const [activeTab, setActiveTab] = useState('power'); // 'power', 'battery', 'solar', 'raw'

  const formatNodeName = (id) => {
    return id.replace('esp32_', 'Node ').toUpperCase();
  };

  const fetchHistory = async () => {
    try {
      const res = await fetch(`${backendUrl}/api/devices/${deviceId}/history?limit=30`);
      if (res.ok) {
        const data = await res.json();
        
        // Calculate power for each history entry
        const processed = data.map(item => ({
          ...item,
          time: new Date(item.timestamp).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' }),
          solar_power: roundVal(item.solar_voltage * item.solar_current),
          battery_power: roundVal(item.battery_voltage * item.battery_current),
        }));
        
        setHistory(processed);
      }
    } catch (err) {
      console.error("Failed to fetch historical telemetry:", err);
    } finally {
      setLoading(false);
    }
  };

  const roundVal = (v) => Math.round(v * 100) / 100;

  useEffect(() => {
    fetchHistory();
    // Poll for historical updates in real-time
    const interval = setInterval(fetchHistory, 3000);
    return () => clearInterval(interval);
  }, [deviceId]);

  return (
    <div className="fixed inset-0 z-50 flex items-center justify-center bg-slate-950/80 p-4 backdrop-blur-sm animate-fadeIn">
      <div 
        className="w-full max-w-4xl rounded-2xl border border-slate-800 bg-slate-900/90 shadow-2xl overflow-hidden flex flex-col max-h-[90vh] glow-card"
        onClick={(e) => e.stopPropagation()}
      >
        {/* Modal Header */}
        <div className="flex items-center justify-between border-b border-slate-800 p-5 bg-slate-900/40">
          <div className="flex items-center gap-3">
            <div className="flex h-10 w-10 items-center justify-center rounded-xl bg-indigo-600/10 text-indigo-400 border border-indigo-500/20">
              <Database className="h-5 w-5" />
            </div>
            <div>
              <h3 className="text-xl font-bold text-white tracking-wide">
                {formatNodeName(deviceId)} Telemetry Details
              </h3>
              <p className="text-xs text-slate-400">
                Real-time chart and log history (polling every 3s)
              </p>
            </div>
          </div>
          <button 
            onClick={onClose}
            className="rounded-lg p-2 text-slate-400 hover:bg-slate-800 hover:text-white transition-colors"
          >
            <X className="h-5 w-5" />
          </button>
        </div>

        {/* Tabs selector */}
        <div className="flex border-b border-slate-800 px-5 bg-slate-900/20 text-sm overflow-x-auto">
          <button
            onClick={() => setActiveTab('power')}
            className={`py-3 px-4 border-b-2 font-semibold transition-all flex items-center gap-1.5 whitespace-nowrap ${
              activeTab === 'power' 
                ? 'border-indigo-500 text-indigo-400 bg-indigo-500/5' 
                : 'border-transparent text-slate-400 hover:text-slate-200'
            }`}
          >
            <Power className="h-4 w-4" />
            Solar vs Battery Power (W)
          </button>
          <button
            onClick={() => setActiveTab('battery')}
            className={`py-3 px-4 border-b-2 font-semibold transition-all flex items-center gap-1.5 whitespace-nowrap ${
              activeTab === 'battery' 
                ? 'border-indigo-500 text-indigo-400 bg-indigo-500/5' 
                : 'border-transparent text-slate-400 hover:text-slate-200'
            }`}
          >
            <BatteryCharging className="h-4 w-4" />
            Battery Voltage & Current
          </button>
          <button
            onClick={() => setActiveTab('solar')}
            className={`py-3 px-4 border-b-2 font-semibold transition-all flex items-center gap-1.5 whitespace-nowrap ${
              activeTab === 'solar' 
                ? 'border-indigo-500 text-indigo-400 bg-indigo-500/5' 
                : 'border-transparent text-slate-400 hover:text-slate-200'
            }`}
          >
            <Sun className="h-4 w-4" />
            Solar Voltage & Current
          </button>
          <button
            onClick={() => setActiveTab('raw')}
            className={`py-3 px-4 border-b-2 font-semibold transition-all flex items-center gap-1.5 whitespace-nowrap ${
              activeTab === 'raw' 
                ? 'border-indigo-500 text-indigo-400 bg-indigo-500/5' 
                : 'border-transparent text-slate-400 hover:text-slate-200'
            }`}
          >
            <Calendar className="h-4 w-4" />
            Telemetry Logs
          </button>
        </div>

        {/* Modal Body */}
        <div className="flex-1 overflow-y-auto p-6 bg-slate-950/20 min-h-[350px]">
          {loading ? (
            <div className="flex flex-col items-center justify-center h-64 gap-3 text-slate-400">
              <RefreshCw className="h-8 w-8 animate-spin text-indigo-500" />
              <span>Fetching telemetry history...</span>
            </div>
          ) : history.length === 0 ? (
            <div className="flex flex-col items-center justify-center h-64 text-slate-500">
              <span>No telemetry records found. Check if the simulator is running.</span>
            </div>
          ) : (
            <>
              {/* Tab: Power Chart */}
              {activeTab === 'power' && (
                <div className="h-72 w-full">
                  <ResponsiveContainer width="100%" height="100%">
                    <LineChart data={history} margin={{ top: 5, right: 10, left: -10, bottom: 5 }}>
                      <CartesianGrid strokeDasharray="3 3" stroke="#1e293b" />
                      <XAxis dataKey="time" stroke="#64748b" tick={{ fontSize: 10 }} />
                      <YAxis stroke="#64748b" tick={{ fontSize: 10 }} label={{ value: 'Power (W)', angle: -90, position: 'insideLeft', fill: '#64748b', fontSize: 10 }} />
                      <Tooltip contentStyle={{ backgroundColor: '#0f172a', borderColor: '#334155', borderRadius: '8px' }} labelStyle={{ color: '#94a3b8' }} />
                      <Legend verticalAlign="top" height={36} wrapperStyle={{ fontSize: 12 }} />
                      <Line type="monotone" dataKey="solar_power" name="Solar Power (W)" stroke="#f59e0b" strokeWidth={2.5} dot={false} activeDot={{ r: 6 }} />
                      <Line type="monotone" dataKey="battery_power" name="Battery Power (W)" stroke="#6366f1" strokeWidth={2.5} dot={false} activeDot={{ r: 6 }} />
                    </LineChart>
                  </ResponsiveContainer>
                </div>
              )}

              {/* Tab: Battery Chart */}
              {activeTab === 'battery' && (
                <div className="h-72 w-full">
                  <ResponsiveContainer width="100%" height="100%">
                    <LineChart data={history} margin={{ top: 5, right: -10, left: -10, bottom: 5 }}>
                      <CartesianGrid strokeDasharray="3 3" stroke="#1e293b" />
                      <XAxis dataKey="time" stroke="#64748b" tick={{ fontSize: 10 }} />
                      <YAxis yAxisId="left" stroke="#6366f1" tick={{ fontSize: 10 }} label={{ value: 'Voltage (V)', angle: -90, position: 'insideLeft', fill: '#6366f1', fontSize: 10 }} />
                      <YAxis yAxisId="right" orientation="right" stroke="#10b981" tick={{ fontSize: 10 }} label={{ value: 'Current (A)', angle: 90, position: 'insideRight', fill: '#10b981', fontSize: 10 }} />
                      <Tooltip contentStyle={{ backgroundColor: '#0f172a', borderColor: '#334155', borderRadius: '8px' }} labelStyle={{ color: '#94a3b8' }} />
                      <Legend verticalAlign="top" height={36} wrapperStyle={{ fontSize: 12 }} />
                      <Line yAxisId="left" type="monotone" dataKey="battery_voltage" name="Battery Voltage (V)" stroke="#6366f1" strokeWidth={2.5} dot={false} />
                      <Line yAxisId="right" type="monotone" dataKey="battery_current" name="Battery Current (A)" stroke="#10b981" strokeWidth={2.5} dot={false} />
                    </LineChart>
                  </ResponsiveContainer>
                </div>
              )}

              {/* Tab: Solar Chart */}
              {activeTab === 'solar' && (
                <div className="h-72 w-full">
                  <ResponsiveContainer width="100%" height="100%">
                    <LineChart data={history} margin={{ top: 5, right: -10, left: -10, bottom: 5 }}>
                      <CartesianGrid strokeDasharray="3 3" stroke="#1e293b" />
                      <XAxis dataKey="time" stroke="#64748b" tick={{ fontSize: 10 }} />
                      <YAxis yAxisId="left" stroke="#f59e0b" tick={{ fontSize: 10 }} label={{ value: 'Voltage (V)', angle: -90, position: 'insideLeft', fill: '#f59e0b', fontSize: 10 }} />
                      <YAxis yAxisId="right" orientation="right" stroke="#10b981" tick={{ fontSize: 10 }} label={{ value: 'Current (A)', angle: 90, position: 'insideRight', fill: '#10b981', fontSize: 10 }} />
                      <Tooltip contentStyle={{ backgroundColor: '#0f172a', borderColor: '#334155', borderRadius: '8px' }} labelStyle={{ color: '#94a3b8' }} />
                      <Legend verticalAlign="top" height={36} wrapperStyle={{ fontSize: 12 }} />
                      <Line yAxisId="left" type="monotone" dataKey="solar_voltage" name="Solar Voltage (V)" stroke="#f59e0b" strokeWidth={2.5} dot={false} />
                      <Line yAxisId="right" type="monotone" dataKey="solar_current" name="Solar Current (A)" stroke="#10b981" strokeWidth={2.5} dot={false} />
                    </LineChart>
                  </ResponsiveContainer>
                </div>
              )}

              {/* Tab: Telemetry Logs */}
              {activeTab === 'raw' && (
                <div className="border border-slate-800 rounded-xl overflow-hidden bg-slate-900/30">
                  <div className="overflow-x-auto">
                    <table className="w-full text-left border-collapse text-xs">
                      <thead>
                        <tr className="bg-slate-900 border-b border-slate-800 text-slate-400 font-semibold uppercase tracking-wider">
                          <th className="p-3">Time</th>
                          <th className="p-3">Solar Volt</th>
                          <th className="p-3">Solar Amp</th>
                          <th className="p-3">Solar Power</th>
                          <th className="p-3">Battery Volt</th>
                          <th className="p-3">Battery Amp</th>
                          <th className="p-3">Battery Power</th>
                          <th className="p-3 text-center">Relay</th>
                        </tr>
                      </thead>
                      <tbody className="divide-y divide-slate-800 text-slate-300">
                        {history.slice(0).reverse().map((log) => (
                          <tr key={log.id} className="hover:bg-slate-800/20">
                            <td className="p-3 text-slate-400 font-mono">{log.time}</td>
                            <td className="p-3">{log.solar_voltage.toFixed(2)} V</td>
                            <td className="p-3">{log.solar_current.toFixed(2)} A</td>
                            <td className="p-3 font-semibold text-amber-500">{log.solar_power.toFixed(1)} W</td>
                            <td className="p-3">{log.battery_voltage.toFixed(2)} V</td>
                            <td className="p-3">{log.battery_current.toFixed(2)} A</td>
                            <td className="p-3 font-semibold text-indigo-400">{log.battery_power.toFixed(1)} W</td>
                            <td className="p-3 text-center">
                              <span className={`inline-block w-2.5 h-2.5 rounded-full ${
                                log.relay_status ? 'bg-emerald-500 glow-green' : 'bg-red-500 glow-red'
                              }`} />
                            </td>
                          </tr>
                        ))}
                      </tbody>
                    </table>
                  </div>
                </div>
              )}
            </>
          )}
        </div>

        {/* Modal Footer */}
        <div className="border-t border-slate-800 bg-slate-900/40 p-4 flex justify-between items-center text-xs text-slate-400">
          <span>Target device ID: <span className="font-mono text-indigo-400">{deviceId}</span></span>
          <button 
            onClick={onClose}
            className="bg-indigo-600 hover:bg-indigo-500 text-white rounded-lg px-4 py-2 font-semibold transition-colors"
          >
            Close Dialog
          </button>
        </div>
      </div>
    </div>
  );
}
