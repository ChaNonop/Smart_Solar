import React, { useState } from 'react';
import { ToggleLeft, ToggleRight, Radio, RefreshCw, Power } from 'lucide-react';

export default function DeviceCard({ device, onSelect, onToggleRelay }) {
  const [loading, setLoading] = useState(false);

  const formatNodeName = (id) => {
    return id.replace('esp32_', 'Node ').toUpperCase();
  };

  const handleRelayClick = async (e) => {
    e.stopPropagation(); // Prevent opening modal
    if (!device.is_online) return;
    
    setLoading(true);
    try {
      await onToggleRelay(device.device_id, !device.relay_status);
    } catch (err) {
      console.error("Relay toggle failed:", err);
    } finally {
      // Small timeout to let state update propagate
      setTimeout(() => setLoading(false), 800);
    }
  };

  return (
    <div 
      onClick={() => onSelect(device.device_id)}
      className={`relative overflow-hidden rounded-2xl border bg-slate-900/40 p-5 cursor-pointer hover:-translate-y-1 hover:bg-slate-900/60 transition-all duration-300 backdrop-blur-md select-none group ${
        device.is_online 
          ? 'border-slate-800 hover:border-indigo-500/50' 
          : 'border-slate-800/40 opacity-70 hover:opacity-100'
      }`}
    >
      {/* Glow Effect on Hover */}
      {device.is_online && (
        <div className="absolute -inset-px bg-gradient-to-r from-indigo-500/0 via-indigo-500/10 to-indigo-500/0 opacity-0 group-hover:opacity-100 transition-opacity duration-500 pointer-events-none rounded-2xl" />
      )}

      {/* Header */}
      <div className="flex items-center justify-between border-b border-slate-800/80 pb-3 mb-4">
        <div className="flex items-center gap-2">
          <div className={`h-2.5 w-2.5 rounded-full ${
            device.is_online 
              ? 'bg-emerald-500 animate-ping' 
              : 'bg-slate-600'
          }`} />
          <span className="font-bold text-slate-100 text-lg tracking-wide">
            {formatNodeName(device.device_id)}
          </span>
        </div>
        <div className="flex items-center gap-1.5">
          {device.is_online ? (
            <span className="text-[10px] font-semibold text-emerald-400 bg-emerald-500/10 px-2 py-0.5 rounded-full border border-emerald-500/20">
              ONLINE
            </span>
          ) : (
            <span className="text-[10px] font-semibold text-slate-400 bg-slate-800 px-2 py-0.5 rounded-full border border-slate-700">
              OFFLINE
            </span>
          )}
        </div>
      </div>

      {/* Grid Values */}
      <div className="grid grid-cols-2 gap-4 mb-4">
        {/* Solar Section */}
        <div className="bg-slate-950/40 rounded-xl p-3 border border-slate-800/40">
          <span className="text-[11px] font-medium text-slate-400 tracking-wider block mb-1">SOLAR PANEL</span>
          <div className="flex justify-between items-baseline">
            <span className="text-xl font-extrabold text-amber-400">
              {device.is_online ? device.solar_power.toFixed(1) : '0.0'}
            </span>
            <span className="text-[10px] font-bold text-amber-500/70">W</span>
          </div>
          <div className="mt-2 space-y-0.5 text-xs text-slate-400">
            <div className="flex justify-between">
              <span>Voltage:</span>
              <span className="font-semibold text-slate-200">
                {device.is_online ? device.solar_voltage.toFixed(1) : '0.0'} V
              </span>
            </div>
            <div className="flex justify-between">
              <span>Current:</span>
              <span className="font-semibold text-slate-200">
                {device.is_online ? device.solar_current.toFixed(2) : '0.00'} A
              </span>
            </div>
          </div>
        </div>

        {/* Battery Section */}
        <div className="bg-slate-950/40 rounded-xl p-3 border border-slate-800/40">
          <span className="text-[11px] font-medium text-slate-400 tracking-wider block mb-1">BATTERY CHARGING</span>
          <div className="flex justify-between items-baseline">
            <span className="text-xl font-extrabold text-indigo-400">
              {device.is_online ? device.battery_power.toFixed(1) : '0.0'}
            </span>
            <span className="text-[10px] font-bold text-indigo-500/70">W</span>
          </div>
          <div className="mt-2 space-y-0.5 text-xs text-slate-400">
            <div className="flex justify-between">
              <span>Voltage:</span>
              <span className="font-semibold text-slate-200">
                {device.is_online ? device.battery_voltage.toFixed(2) : '0.00'} V
              </span>
            </div>
            <div className="flex justify-between">
              <span>Current:</span>
              <span className="font-semibold text-slate-200">
                {device.is_online ? device.battery_current.toFixed(2) : '0.00'} A
              </span>
            </div>
          </div>
        </div>
      </div>

      {/* Relay Toggle Footer */}
      <div className="flex items-center justify-between border-t border-slate-800/80 pt-3 mt-2">
        <div className="flex items-center gap-1.5 text-xs text-slate-400">
          <Power className="h-3.5 w-3.5" />
          <span>Relay Switch</span>
        </div>
        <button 
          onClick={handleRelayClick}
          disabled={!device.is_online || loading}
          className={`flex items-center gap-1 rounded-lg px-3 py-1.5 text-xs font-semibold transition-all border ${
            !device.is_online 
              ? 'bg-slate-850 text-slate-500 border-slate-800 cursor-not-allowed'
              : loading
                ? 'bg-indigo-600/10 text-indigo-400 border-indigo-500/20 cursor-wait'
                : device.relay_status
                  ? 'bg-emerald-500/10 text-emerald-400 border-emerald-500/30 hover:bg-emerald-500/20 glow-green'
                  : 'bg-red-500/10 text-red-400 border-red-500/30 hover:bg-red-500/20 glow-red'
          }`}
        >
          {loading ? (
            <RefreshCw className="h-3.5 w-3.5 animate-spin" />
          ) : device.relay_status ? (
            <ToggleRight className="h-4 w-4 text-emerald-400" />
          ) : (
            <ToggleLeft className="h-4 w-4 text-red-400" />
          )}
          <span>{device.relay_status ? 'ON' : 'OFF'}</span>
        </button>
      </div>
    </div>
  );
}
