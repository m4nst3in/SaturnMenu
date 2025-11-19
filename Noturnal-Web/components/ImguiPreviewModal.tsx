
import React, { useState } from 'react';
import { X, Eye, Crosshair, Shield, Settings, Monitor } from 'lucide-react';

interface ImguiPreviewModalProps {
  isOpen: boolean;
  onClose: () => void;
}

export const ImguiPreviewModal: React.FC<ImguiPreviewModalProps> = ({ isOpen, onClose }) => {
  const [activeTab, setActiveTab] = useState('Legit');
  
  // Mock state for controls
  const [aimbotEnabled, setAimbotEnabled] = useState(true);
  const [fov, setFov] = useState(4.5);
  const [smooth, setSmooth] = useState(12);
  const [rcs, setRcs] = useState(true);
  const [espEnabled, setEspEnabled] = useState(true);
  const [boxType, setBoxType] = useState(0);
  const [chams, setChams] = useState(true);
  
  if (!isOpen) return null;

  const tabs = [
    { id: 'Legit', icon: <Crosshair className="w-3 h-3" /> },
    { id: 'Rage', icon: <Shield className="w-3 h-3" /> },
    { id: 'Visuals', icon: <Eye className="w-3 h-3" /> },
    { id: 'Misc', icon: <Settings className="w-3 h-3" /> },
    { id: 'Config', icon: <Monitor className="w-3 h-3" /> }
  ];

  return (
    <div className="fixed inset-0 z-[100] flex items-center justify-center bg-black/80 backdrop-blur-sm px-4" onClick={onClose}>
      {/* ImGui Window Container */}
      <div 
        className="w-[700px] h-[450px] bg-[#121212] rounded border border-[#333] shadow-[0_0_0_1px_rgba(0,0,0,0.5),0_20px_50px_rgba(139,92,246,0.2)] flex flex-col font-sans text-sm select-none overflow-hidden animate-fade-in-up"
        onClick={(e) => e.stopPropagation()}
      >
        
        {/* Title Bar */}
        <div className="h-8 bg-gradient-to-r from-noturnal-900 to-[#1a1a1a] flex items-center justify-between px-3 border-b border-[#333]">
          <span className="text-noturnal-300 font-bold text-xs tracking-wide font-mono flex items-center gap-2">
            <div className="w-2 h-2 bg-green-500 rounded-full animate-pulse"></div>
            Noturnal Menu [Stable] | cs2.exe
          </span>
          <button onClick={onClose} className="text-[#888] hover:text-white transition-colors">
            <X className="w-3 h-3" />
          </button>
        </div>

        <div className="flex flex-1 overflow-hidden">
          {/* Sidebar / Tabs */}
          <div className="w-32 bg-[#0f0f0f] border-r border-[#333] p-2 space-y-1">
            {tabs.map(tab => (
              <button
                key={tab.id}
                onClick={() => setActiveTab(tab.id)}
                className={`w-full text-left px-3 py-2 rounded text-xs font-medium transition-all flex items-center gap-2 ${
                  activeTab === tab.id 
                    ? 'bg-noturnal-700/20 text-noturnal-300 border border-noturnal-500/30' 
                    : 'text-[#888] hover:bg-[#1a1a1a] hover:text-[#ccc]'
                }`}
              >
                {tab.icon}
                {tab.id}
              </button>
            ))}
          </div>

          {/* Content Area */}
          <div className="flex-1 bg-[#141414] p-4 overflow-y-auto custom-scrollbar font-mono">
            
            {activeTab === 'Legit' && (
              <div className="grid grid-cols-2 gap-4">
                {/* Column 1 */}
                <div className="space-y-2">
                    <div className="border border-[#333] bg-[#111] rounded p-2">
                        <div className="text-[10px] text-noturnal-500 uppercase font-bold mb-2 border-b border-[#333] pb-1">Aimbot General</div>
                        
                        <label className="flex items-center gap-2 mb-2 cursor-pointer select-none group">
                            <input type="checkbox" checked={aimbotEnabled} onChange={() => setAimbotEnabled(!aimbotEnabled)} className="appearance-none w-3 h-3 border border-[#444] rounded-sm checked:bg-noturnal-500 checked:border-noturnal-500 transition-colors cursor-pointer" />
                            <span className="text-[#ccc] text-xs group-hover:text-white">Master Switch</span>
                        </label>

                        <div className="mb-3">
                            <div className="flex justify-between text-[#888] text-[10px] mb-1">
                                <span>FOV</span> <span>{fov.toFixed(1)}°</span>
                            </div>
                            <input type="range" min="0" max="20" step="0.1" value={fov} onChange={(e) => setFov(parseFloat(e.target.value))} className="w-full h-1 bg-[#333] appearance-none rounded accent-noturnal-500 cursor-pointer" />
                        </div>

                        <div className="mb-2">
                            <div className="flex justify-between text-[#888] text-[10px] mb-1">
                                <span>Smoothing</span> <span>{smooth}ms</span>
                            </div>
                            <input type="range" min="1" max="100" value={smooth} onChange={(e) => setSmooth(parseInt(e.target.value))} className="w-full h-1 bg-[#333] appearance-none rounded accent-noturnal-500 cursor-pointer" />
                        </div>
                    </div>
                </div>

                 {/* Column 2 */}
                 <div className="space-y-2">
                    <div className="border border-[#333] bg-[#111] rounded p-2">
                        <div className="text-[10px] text-noturnal-500 uppercase font-bold mb-2 border-b border-[#333] pb-1">Recoil Control</div>
                         <label className="flex items-center gap-2 mb-2 cursor-pointer select-none group">
                            <input type="checkbox" checked={rcs} onChange={() => setRcs(!rcs)} className="appearance-none w-3 h-3 border border-[#444] rounded-sm checked:bg-noturnal-500 checked:border-noturnal-500 transition-colors" />
                            <span className="text-[#ccc] text-xs group-hover:text-white">Enable RCS</span>
                        </label>
                        <div className="mb-2 opacity-80">
                            <div className="flex justify-between text-[#888] text-[10px] mb-1">
                                <span>X Axis</span> <span>100%</span>
                            </div>
                            <input type="range" value="100" readOnly className="w-full h-1 bg-[#333] appearance-none rounded accent-noturnal-500" />
                        </div>
                    </div>
                 </div>
              </div>
            )}

            {activeTab === 'Visuals' && (
               <div className="grid grid-cols-2 gap-4">
                  <div className="border border-[#333] bg-[#111] rounded p-2">
                     <div className="text-[10px] text-noturnal-500 uppercase font-bold mb-2 border-b border-[#333] pb-1">Player ESP</div>
                     <label className="flex items-center gap-2 mb-2 cursor-pointer select-none group">
                        <input type="checkbox" checked={espEnabled} onChange={() => setEspEnabled(!espEnabled)} className="appearance-none w-3 h-3 border border-[#444] rounded-sm checked:bg-noturnal-500 checked:border-noturnal-500 transition-colors" />
                        <span className="text-[#ccc] text-xs group-hover:text-white">Enabled</span>
                     </label>
                     <label className="flex items-center gap-2 mb-2 cursor-pointer select-none group">
                        <input type="checkbox" checked={true} readOnly className="appearance-none w-3 h-3 border border-[#444] rounded-sm checked:bg-noturnal-500 checked:border-noturnal-500 transition-colors" />
                        <span className="text-[#ccc] text-xs group-hover:text-white">Box</span>
                     </label>
                      <label className="flex items-center gap-2 mb-2 cursor-pointer select-none group">
                        <input type="checkbox" checked={true} readOnly className="appearance-none w-3 h-3 border border-[#444] rounded-sm checked:bg-noturnal-500 checked:border-noturnal-500 transition-colors" />
                        <span className="text-[#ccc] text-xs group-hover:text-white">Skeleton</span>
                     </label>
                  </div>
                   <div className="border border-[#333] bg-[#111] rounded p-2">
                     <div className="text-[10px] text-noturnal-500 uppercase font-bold mb-2 border-b border-[#333] pb-1">Chams</div>
                     <label className="flex items-center gap-2 mb-2 cursor-pointer select-none group">
                        <input type="checkbox" checked={chams} onChange={() => setChams(!chams)} className="appearance-none w-3 h-3 border border-[#444] rounded-sm checked:bg-noturnal-500 checked:border-noturnal-500 transition-colors" />
                        <span className="text-[#ccc] text-xs group-hover:text-white">Enable Chams</span>
                     </label>
                     <div className="flex gap-2 mt-2">
                        <div className="w-4 h-4 bg-noturnal-500 rounded border border-white/20"></div>
                        <div className="w-4 h-4 bg-blue-500 rounded border border-white/20"></div>
                     </div>
                   </div>
               </div>
            )}

            {(activeTab === 'Rage' || activeTab === 'Misc' || activeTab === 'Config') && (
                <div className="flex items-center justify-center h-full text-[#444] flex-col gap-2">
                    <Shield className="w-8 h-8 opacity-20" />
                    <span className="text-xs">Feature available in Premium</span>
                </div>
            )}
          </div>
        </div>
      </div>
    </div>
  );
};