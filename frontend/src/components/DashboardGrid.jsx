import React from 'react';
import DeviceCard from './DeviceCard';

export default function DashboardGrid({ nodes, onSelectNode, onToggleRelay, onDeleteNode }) {
  if (nodes.length === 0) {
    return (
      <div className="flex flex-col items-center justify-center p-12 rounded-2xl border border-dashed border-slate-800 bg-slate-900/10 text-slate-500">
        <p className="text-sm">No solar nodes registered yet.</p>
        <p className="text-xs mt-1">Click the "Add Solar Node" button above to register a station.</p>
      </div>
    );
  }

  return (
    <div className="grid grid-cols-1 gap-6 sm:grid-cols-2 lg:grid-cols-3">
      {nodes.map((node) => (
        <DeviceCard
          key={node.mac}
          node={node}
          onSelect={onSelectNode}
          onToggleRelay={onToggleRelay}
          onDelete={onDeleteNode}
        />
      ))}
    </div>
  );
}
