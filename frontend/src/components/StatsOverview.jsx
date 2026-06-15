import React from 'react';
import { Sun, BatteryCharging, Power, Wifi } from 'lucide-react';

export default function StatsOverview({ devices }) {
  const onlineCount = devices.filter(d => d.is_online).length;
  
  const totalSolarPower = devices.reduce((sum, d) => sum + (d.is_online ? d.solar_power : 0), 0);
  const totalBatteryPower = devices.reduce((sum, d) => sum + (d.is_online ? d.battery_power : 0), 0);
  
  const activeRelays = devices.filter(d => d.relay_status && d.is_online).length;

  return (
    <div className="grid grid-cols-1 gap-4 sm:grid-cols-2 lg:grid-cols-4 mb-6">
      {/* Active Nodes */}
      <div className="relative overflow-hidden rounded-2xl border border-slate-800 bg-slate-900/40 p-5 backdrop-blur-md">
        <div className="flex items-center justify-between">
          <div>
            <p className="text-sm font-medium text-slate-400">Active Nodes</p>
            <h4 className="mt-2 text-3xl font-extrabold text-white">
              {onlineCount} <span className="text-sm font-normal text-slate-500">/ 5</span>
            </h4>
          </div>
          <div className={`rounded-xl p-3 border ${
            onlineCount > 0 
              ? 'bg-emerald-500/10 text-emerald-400 border-emerald-500/20' 
              : 'bg-slate-800 text-slate-400 border-slate-700'
          }`}>
            <Wifi className={`h-6 w-6 ${onlineCount > 0 ? 'animate-pulse' : ''}`} />
          </div>
        </div>
        <div className="mt-4 flex items-center text-xs text-slate-400">
          <span className="font-semibold text-emerald-400 mr-1.5">{onlineCount * 20}%</span>
          <span>online and publishing data</span>
        </div>
      </div>

      {/* Solar Generation */}
      <div className="relative overflow-hidden rounded-2xl border border-slate-800 bg-slate-900/40 p-5 backdrop-blur-md">
        <div className="flex items-center justify-between">
          <div>
            <p className="text-sm font-medium text-slate-400">Total Solar Input</p>
            <h4 className="mt-2 text-3xl font-extrabold text-white">
              {totalSolarPower.toFixed(1)} <span className="text-sm font-normal text-slate-500">W</span>
            </h4>
          </div>
          <div className="rounded-xl bg-amber-500/10 text-amber-400 border border-amber-500/20 p-3">
            <Sun className="h-6 w-6" />
          </div>
        </div>
        <div className="mt-4 flex items-center text-xs text-slate-400">
          <span className="font-semibold text-amber-400 mr-1.5">Live</span>
          <span>from all solar panel arrays</span>
        </div>
      </div>

      {/* Battery Power */}
      <div className="relative overflow-hidden rounded-2xl border border-slate-800 bg-slate-900/40 p-5 backdrop-blur-md">
        <div className="flex items-center justify-between">
          <div>
            <p className="text-sm font-medium text-slate-400">Total Charging Load</p>
            <h4 className="mt-2 text-3xl font-extrabold text-white">
              {totalBatteryPower.toFixed(1)} <span className="text-sm font-normal text-slate-500">W</span>
            </h4>
          </div>
          <div className="rounded-xl bg-indigo-500/10 text-indigo-400 border border-indigo-500/20 p-3">
            <BatteryCharging className="h-6 w-6" />
          </div>
        </div>
        <div className="mt-4 flex items-center text-xs text-slate-400">
          <span className="font-semibold text-indigo-400 mr-1.5">
            {totalSolarPower > 0 ? ((totalBatteryPower / totalSolarPower) * 100).toFixed(0) : 0}%
          </span>
          <span>charging transfer efficiency</span>
        </div>
      </div>

      {/* Active Relays */}
      <div className="relative overflow-hidden rounded-2xl border border-slate-800 bg-slate-900/40 p-5 backdrop-blur-md">
        <div className="flex items-center justify-between">
          <div>
            <p className="text-sm font-medium text-slate-400">Active Relays</p>
            <h4 className="mt-2 text-3xl font-extrabold text-white">
              {activeRelays} <span className="text-sm font-normal text-slate-500">/ 5</span>
            </h4>
          </div>
          <div className={`rounded-xl p-3 border ${
            activeRelays > 0 
              ? 'bg-sky-500/10 text-sky-400 border-sky-500/20' 
              : 'bg-slate-800 text-slate-400 border-slate-700'
          }`}>
            <Power className="h-6 w-6" />
          </div>
        </div>
        <div className="mt-4 flex items-center text-xs text-slate-400">
          <span>Switched on to power loads/charge lines</span>
        </div>
      </div>
    </div>
  );
}
