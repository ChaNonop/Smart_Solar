import React, { useState } from 'react';
import { ToggleLeft, ToggleRight, Radio, RefreshCw, Power, Trash2, Sun, Battery, Activity } from 'lucide-react';

export default function DeviceCard({ node, onSelect, onToggleRelay, onDelete }) {
  const [loading1, setLoading1] = useState(false);
  const [loading2, setLoading2] = useState(false);

  const handleRelayClick = async (e, relayNum, currentStatus) => {
    e.stopPropagation(); // Prevent opening modal
    if (!node.is_online) return;
    
    if (relayNum === 1) setLoading1(true);
    else setLoading2(true);

    try {
      await onToggleRelay(node.mac, relayNum, currentStatus);
    } catch (err) {
      console.error("Relay toggle failed:", err);
    } finally {
      setTimeout(() => {
        setLoading1(false);
        setLoading2(false);
      }, 800);
    }
  };

  const handleDeleteClick = (e) => {
    e.stopPropagation(); // Prevent opening modal
    onDelete(node.mac);
  };

  return (
    <div 
      onClick={() => onSelect(node.mac)}
      className={`relative overflow-hidden rounded-2xl border bg-slate-900/40 p-5 cursor-pointer hover:-translate-y-1 hover:bg-slate-900/60 transition-all duration-300 backdrop-blur-md select-none group ${
        node.is_online 
          ? 'border-slate-800 hover:border-indigo-500/50' 
          : 'border-slate-800/40 opacity-75 hover:opacity-100'
      }`}
    >
      {/* Glow Effect on Hover */}
      {node.is_online && (
        <div className="absolute -inset-px bg-gradient-to-r from-indigo-500/0 via-indigo-500/10 to-indigo-500/0 opacity-0 group-hover:opacity-100 transition-opacity duration-500 pointer-events-none rounded-2xl" />
      )}

      {/* Header */}
      <div className="flex items-center justify-between border-b border-slate-800/80 pb-3 mb-4">
        <div>
          <span className="font-bold text-slate-100 text-lg tracking-wide block">
            {node.name}
          </span>
          <span className="text-[10px] font-mono text-slate-500 tracking-wider">
            MAC: {node.mac}
          </span>
        </div>
        <div className="flex items-center gap-2">
          {node.is_online ? (
            <span className="text-[10px] font-semibold text-emerald-400 bg-emerald-500/10 px-2 py-0.5 rounded-full border border-emerald-500/20 flex items-center gap-1">
              <span className="w-1.5 h-1.5 rounded-full bg-emerald-500 animate-ping" />
              ONLINE
            </span>
          ) : (
            <span className="text-[10px] font-semibold text-slate-400 bg-slate-800 px-2 py-0.5 rounded-full border border-slate-700">
              OFFLINE
            </span>
          )}
          
          <button 
            onClick={handleDeleteClick}
            className="text-slate-500 hover:text-red-400 p-1 rounded transition-colors"
            title="Remove Node"
          >
            <Trash2 className="h-3.5 w-3.5" />
          </button>
        </div>
      </div>

      {/* Grid Values */}
      <div className="grid grid-cols-2 gap-3 mb-4">
        {/* Solar Section */}
        <div className="bg-slate-950/40 rounded-xl p-3 border border-slate-800/40">
          <span className="text-[10px] font-semibold text-amber-400 flex items-center gap-1 mb-1">
            <Sun className="h-3 w-3" /> SOLAR INPUT
          </span>
          <div className="flex justify-between items-baseline">
            <span className="text-lg font-extrabold text-amber-400">
              {node.is_online ? node.solar_power.toFixed(1) : '0.0'}
            </span>
            <span className="text-[9px] font-bold text-amber-500/70">W</span>
          </div>
          <div className="mt-2 space-y-0.5 text-xs text-slate-400">
            <div className="flex justify-between">
              <span>Voltage:</span>
              <span className="font-semibold text-slate-200">
                {node.is_online ? node.v_solar.toFixed(1) : '0.0'} V
              </span>
            </div>
            <div className="flex justify-between">
              <span>Current:</span>
              <span className="font-semibold text-slate-200">
                {node.is_online ? node.i_solar.toFixed(2) : '0.00'} A
              </span>
            </div>
          </div>
        </div>

        {/* Battery Section */}
        <div className="bg-slate-950/40 rounded-xl p-3 border border-slate-800/40">
          <span className="text-[10px] font-semibold text-indigo-400 flex items-center gap-1 mb-1">
            <Battery className="h-3 w-3" /> BATTERY
          </span>
          <div className="flex justify-between items-baseline">
            <span className="text-lg font-extrabold text-indigo-400">
              {node.is_online ? node.battery_power.toFixed(1) : '0.0'}
            </span>
            <span className="text-[9px] font-bold text-indigo-500/70">W</span>
          </div>
          <div className="mt-2 space-y-0.5 text-xs text-slate-400">
            <div className="flex justify-between">
              <span>Voltage:</span>
              <span className="font-semibold text-slate-200">
                {node.is_online ? node.v_bat.toFixed(2) : '0.00'} V
              </span>
            </div>
            <div className="flex justify-between">
              <span>Charge I:</span>
              <span className="font-semibold text-slate-200">
                {node.is_online ? node.i_charge.toFixed(2) : '0.00'} A
              </span>
            </div>
          </div>
        </div>
      </div>

      {/* Load & Lux Stats */}
      <div className="bg-slate-950/20 border border-slate-800/30 rounded-xl p-3 mb-4 grid grid-cols-2 text-xs text-slate-400 gap-2">
        <div>
          <span className="block text-[10px] text-slate-500 uppercase">Load Consumption</span>
          <span className="font-bold text-slate-200 text-sm">
            {node.is_online ? (node.v_load * node.i_load).toFixed(1) : '0.0'} W
          </span>
          <span className="text-[10px] block text-slate-500">
            {node.is_online ? node.v_load.toFixed(1) : '0.0'}V | {node.is_online ? node.i_load.toFixed(2) : '0.00'}A
          </span>
        </div>
        <div>
          <span className="block text-[10px] text-slate-500 uppercase">Light Level</span>
          <span className="font-bold text-amber-500 text-sm">
            {node.is_online ? node.lux : '0'} Lux
          </span>
        </div>
      </div>

      {/* Relays Toggles */}
      <div className="border-t border-slate-800/80 pt-3 flex flex-col gap-2">
        {/* Relay 1 */}
        <div className="flex items-center justify-between text-xs">
          <span className="text-slate-400 flex items-center gap-1.5">
            <Power className="h-3 w-3" />
            <span>Relay 1 (Solar Switch)</span>
          </span>
          <button 
            onClick={(e) => handleRelayClick(e, 1, node.relay1)}
            disabled={!node.is_online || loading1}
            className={`flex items-center gap-1 rounded-lg px-2.5 py-1 text-[11px] font-semibold transition-all border ${
              !node.is_online 
                ? 'bg-slate-850 text-slate-500 border-slate-800 cursor-not-allowed'
                : loading1
                  ? 'bg-indigo-600/10 text-indigo-400 border-indigo-500/20 cursor-wait'
                  : node.relay1 === 1
                    ? 'bg-emerald-500/10 text-emerald-400 border-emerald-500/30 hover:bg-emerald-500/20 glow-green'
                    : 'bg-red-500/10 text-red-400 border-red-500/30 hover:bg-red-500/20 glow-red'
            }`}
          >
            {loading1 ? (
              <RefreshCw className="h-3 w-3 animate-spin" />
            ) : node.relay1 === 1 ? (
              <ToggleRight className="h-3.5 w-3.5 text-emerald-400" />
            ) : (
              <ToggleLeft className="h-3.5 w-3.5 text-red-400" />
            )}
            <span>{node.relay1 === 1 ? 'ON' : 'OFF'}</span>
          </button>
        </div>

        {/* Relay 2 */}
        <div className="flex items-center justify-between text-xs">
          <span className="text-slate-400 flex items-center gap-1.5">
            <Activity className="h-3 w-3" />
            <span>Relay 2 (Load Switch)</span>
          </span>
          <button 
            onClick={(e) => handleRelayClick(e, 2, node.relay2)}
            disabled={!node.is_online || loading2}
            className={`flex items-center gap-1 rounded-lg px-2.5 py-1 text-[11px] font-semibold transition-all border ${
              !node.is_online 
                ? 'bg-slate-850 text-slate-500 border-slate-800 cursor-not-allowed'
                : loading2
                  ? 'bg-indigo-600/10 text-indigo-400 border-indigo-500/20 cursor-wait'
                  : node.relay2 === 1
                    ? 'bg-emerald-500/10 text-emerald-400 border-emerald-500/30 hover:bg-emerald-500/20 glow-green'
                    : 'bg-red-500/10 text-red-400 border-red-500/30 hover:bg-red-500/20 glow-red'
            }`}
          >
            {loading2 ? (
              <RefreshCw className="h-3 w-3 animate-spin" />
            ) : node.relay2 === 1 ? (
              <ToggleRight className="h-3.5 w-3.5 text-emerald-400" />
            ) : (
              <ToggleLeft className="h-3.5 w-3.5 text-red-400" />
            )}
            <span>{node.relay2 === 1 ? 'ON' : 'OFF'}</span>
          </button>
        </div>
      </div>
    </div>
  );
}
