import React, { useState, useEffect } from 'react';
import { Shield, Globe, Activity, Server, Lock, AlertTriangle, Radio, MapPin } from 'lucide-react';
import { useSound } from '../hooks/useSound';

const SERVER_STATUS = [
  { id: 'EU-West', status: 'OPERATIONAL', ping: '24ms' },
  { id: 'US-East', status: 'OPERATIONAL', ping: '45ms' },
  { id: 'Asia-Pac', status: 'OPERATIONAL', ping: '89ms' },
  { id: 'SA-East', status: 'OPERATIONAL', ping: '56ms' },
];

const RECENT_BANS = [
  { game: 'CS2', count: 1420, region: 'Russia', risk: 'LOW' },
  { game: 'CS2', count: 532, region: 'China', risk: 'LOW' },
  { game: 'CS2', count: 120, region: 'USA', risk: 'LOW' },
];

export const Watchtower: React.FC = () => {
  const [riskLevel, setRiskLevel] = useState('LOW');
  const [detectionRate, setDetectionRate] = useState(0.03);
  const { playSound } = useSound();

  // Simulate live data updates
  useEffect(() => {
    const interval = setInterval(() => {
      setDetectionRate(prev => {
        const change = (Math.random() - 0.5) * 0.01;
        return Math.max(0, Math.min(0.1, prev + change));
      });
    }, 2000);
    return () => clearInterval(interval);
  }, []);

  return (
    <div className="min-h-screen bg-dark-bg pt-24 pb-12 px-4">
      <div className="max-w-7xl mx-auto">
        <div className="flex items-center gap-3 mb-8">
          <div className="w-3 h-3 bg-red-500 rounded-full animate-pulse"></div>
          <h1 className="text-2xl font-mono font-bold text-red-500 tracking-widest uppercase">Watchtower // Threat Intelligence</h1>
        </div>

        {/* Main HUD */}
        <div className="grid grid-cols-1 lg:grid-cols-3 gap-6 mb-8">
           
           {/* Global Threat Level */}
           <div className="bg-[#0c0b14] border border-red-500/30 rounded-2xl p-8 relative overflow-hidden">
              <div className="absolute top-0 right-0 p-4 opacity-20">
                 <Shield className="w-24 h-24 text-red-500" />
              </div>
              <h3 className="text-gray-500 text-sm font-mono mb-2 uppercase">Global Detection Risk</h3>
              <div className="text-6xl font-bold text-white mb-4 flex items-baseline gap-2">
                 {riskLevel}
                 <span className="text-sm text-green-500 font-normal bg-green-900/20 px-2 py-1 rounded border border-green-500/30">SAFE TO INJECT</span>
              </div>
              <div className="w-full bg-gray-800 h-2 rounded-full overflow-hidden">
                 <div className="h-full bg-green-500 w-[5%] shadow-[0_0_10px_lime]"></div>
              </div>
              <p className="text-xs text-gray-400 mt-2 font-mono">Current probability of flagging: {detectionRate.toFixed(4)}%</p>
           </div>

           {/* Map Visualization Placeholder */}
           <div className="lg:col-span-2 bg-[#0c0b14] border border-white/10 rounded-2xl p-6 relative min-h-[200px] flex items-center justify-center bg-[url('https://upload.wikimedia.org/wikipedia/commons/e/ec/World_map_blank_without_borders.svg')] bg-cover bg-fixed opacity-80">
               <div className="absolute inset-0 bg-blue-900/10 mix-blend-overlay"></div>
               <div className="relative z-10 grid grid-cols-2 md:grid-cols-4 gap-8 w-full">
                  {SERVER_STATUS.map(s => (
                    <div key={s.id} className="bg-black/60 backdrop-blur p-4 rounded border border-white/10 text-center">
                       <div className="text-xs text-gray-400 mb-1">{s.id}</div>
                       <div className="text-green-400 font-bold text-sm mb-1">{s.status}</div>
                       <div className="text-xs font-mono text-noturnal-400">{s.ping}</div>
                    </div>
                  ))}
               </div>
               
               {/* Pulsing Dots on Map */}
               <div className="absolute top-1/3 left-1/4 w-3 h-3 bg-red-500 rounded-full animate-ping"></div>
               <div className="absolute top-1/2 right-1/3 w-3 h-3 bg-noturnal-500 rounded-full animate-ping animation-delay-1000"></div>
           </div>
        </div>

        {/* Live Feed */}
        <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
           <div className="bg-[#0c0b14] border border-white/10 rounded-2xl p-6">
              <h3 className="text-white font-bold mb-4 flex items-center gap-2"><Radio className="w-4 h-4 text-noturnal-400" /> VAC Live Feed</h3>
              <div className="space-y-3 font-mono text-xs">
                 <div className="flex justify-between text-gray-400 border-b border-white/5 pb-1">
                    <span>MODULE</span>
                    <span>STATUS</span>
                 </div>
                 <div className="flex justify-between text-green-400">
                    <span>heuristic_scan.dll</span>
                    <span>BYPASSED</span>
                 </div>
                 <div className="flex justify-between text-green-400">
                    <span>dns_cache_check</span>
                    <span>BYPASSED</span>
                 </div>
                 <div className="flex justify-between text-green-400">
                    <span>input_automation_check</span>
                    <span>CLEAN</span>
                 </div>
                 <div className="flex justify-between text-yellow-400 animate-pulse">
                    <span>server_side_angles</span>
                    <span>MONITORING</span>
                 </div>
              </div>
           </div>

           <div className="bg-[#0c0b14] border border-white/10 rounded-2xl p-6">
              <h3 className="text-white font-bold mb-4 flex items-center gap-2"><AlertTriangle className="w-4 h-4 text-yellow-400" /> Recent Ban Waves</h3>
              <div className="space-y-4">
                 {RECENT_BANS.map((ban, i) => (
                    <div key={i} className="flex justify-between items-center p-2 hover:bg-white/5 rounded transition-colors">
                       <div className="flex items-center gap-3">
                          <Globe className="w-4 h-4 text-gray-500" />
                          <div>
                             <div className="text-white text-sm font-bold">{ban.region}</div>
                             <div className="text-[10px] text-gray-500">Target: {ban.game}</div>
                          </div>
                       </div>
                       <div className="text-right">
                          <div className="text-red-400 font-mono font-bold">-{ban.count} accs</div>
                          <div className="text-[10px] text-green-500">Noturnal Users Safe</div>
                       </div>
                    </div>
                 ))}
              </div>
           </div>
        </div>
      </div>
    </div>
  );
};