import React from 'react';
import { Sun, Cpu, ShieldAlert, CheckCircle } from 'lucide-react';

export default function Navbar({ isBackendConnected }) {
  return (
    <nav className="border-b border-slate-800 bg-slate-900/60 backdrop-blur-md sticky top-0 z-50">
      <div className="mx-auto max-w-7xl px-4 sm:px-6 lg:px-8">
        <div className="flex h-16 items-center justify-between">
          <div className="flex items-center gap-3">
            <div className="flex h-10 w-10 items-center justify-center rounded-xl bg-indigo-600/20 text-indigo-400 border border-indigo-500/20 glow-card">
              <Sun className="h-6 w-6 animate-pulse" />
            </div>
            <div>
              <span className="font-extrabold text-xl tracking-wider bg-gradient-to-r from-white via-indigo-200 to-indigo-400 bg-clip-text text-transparent">
                SOLAR IOT
              </span>
              <span className="text-xs block text-slate-400 font-medium tracking-wide">
                COMMAND CENTER (5 NODES)
              </span>
            </div>
          </div>

          <div className="flex items-center gap-4">
            <div className={`flex items-center gap-2 rounded-full px-3 py-1 text-xs font-semibold border ${
              isBackendConnected 
                ? 'bg-emerald-500/10 text-emerald-400 border-emerald-500/20 glow-green' 
                : 'bg-red-500/10 text-red-400 border-red-500/20 glow-red animate-pulse'
            }`}>
              {isBackendConnected ? (
                <>
                  <CheckCircle className="h-3.5 w-3.5" />
                  <span>BACKEND: ONLINE</span>
                </>
              ) : (
                <>
                  <ShieldAlert className="h-3.5 w-3.5" />
                  <span>BACKEND: OFFLINE</span>
                </>
              )}
            </div>
            <div className="flex items-center gap-2 text-slate-400 text-xs bg-slate-800/50 rounded-lg px-3 py-1 border border-slate-700/50">
              <Cpu className="h-3.5 w-3.5" />
              <span>RPi Broker</span>
            </div>
          </div>
        </div>
      </div>
    </nav>
  );
}
