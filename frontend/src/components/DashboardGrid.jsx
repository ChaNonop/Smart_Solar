import React from 'react';
import DeviceCard from './DeviceCard';

export default function DashboardGrid({ devices, onSelectDevice, onToggleRelay }) {
  return (
    <div>
      <div className="flex items-center justify-between mb-4">
        <h3 className="text-lg font-bold text-slate-100 tracking-wide">
          ESP32 Stations Grid
        </h3>
        <span className="text-xs text-slate-400">
          Showing 5 nodes (click a card to view detailed charts)
        </span>
      </div>
      <div className="grid grid-cols-1 gap-6 sm:grid-cols-2 lg:grid-cols-3">
        {devices.map((device) => (
          <DeviceCard
            key={device.device_id}
            device={device}
            onSelect={onSelectDevice}
            onToggleRelay={onToggleRelay}
          />
        ))}
      </div>
    </div>
  );
}
