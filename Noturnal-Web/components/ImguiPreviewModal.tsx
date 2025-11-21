import React, { useState, useRef, useEffect } from 'react';
import { X } from 'lucide-react';

interface ImguiPreviewModalProps {
  isOpen: boolean;
  onClose: () => void;
}

// Fake game coordinates for visualizer
const ENEMIES = [
  { id: 1, x: 150, y: 100, width: 40, height: 100, hp: 100, team: 'CT' },
  { id: 2, x: 300, y: 120, width: 35, height: 90, hp: 45, team: 'T' },
  { id: 3, x: 50, y: 140, width: 45, height: 110, hp: 100, team: 'CT' },
];

export const ImguiPreviewModal: React.FC<ImguiPreviewModalProps> = ({ isOpen, onClose }) => {
  const [activeTab, setActiveTab] = useState('Legit');
  const [position, setPosition] = useState({ x: 0, y: 0 });
  const [isDragging, setIsDragging] = useState(false);
  const dragStart = useRef({ x: 0, y: 0 });
  const modalRef = useRef<HTMLDivElement>(null);

  // Config State
  const [config, setConfig] = useState({
    aimbot: true,
    fov: 2.5,
    smooth: 15,
    rcs: true,
    esp: true,
    box: true,
    skeleton: false,
    snapLines: false,
    healthBar: true,
    chams: false,
    chamsVisible: false,
    radar: false,
    bhop: true
  });

  // Center modal on open
  useEffect(() => {
    if (isOpen && modalRef.current) {
      const x = (window.innerWidth - 750) / 2;
      const y = (window.innerHeight - 500) / 2;
      setPosition({ x, y });
    }
  }, [isOpen]);

  const handleMouseDown = (e: React.MouseEvent) => {
    setIsDragging(true);
    dragStart.current = {
      x: e.clientX - position.x,
      y: e.clientY - position.y
    };
  };

  const handleMouseMove = (e: React.MouseEvent) => {
    if (isDragging) {
      setPosition({
        x: e.clientX - dragStart.current.x,
        y: e.clientY - dragStart.current.y
      });
    }
  };

  const handleMouseUp = () => {
    setIsDragging(false);
  };

  const updateConfig = (key: keyof typeof config, value: any) => {
    setConfig(prev => ({ ...prev, [key]: value }));
  };

  if (!isOpen) return null;

  return (
    <div 
      className="fixed inset-0 z-[100] bg-black/60 backdrop-blur-sm flex items-center justify-center overflow-hidden"
      onMouseMove={handleMouseMove}
      onMouseUp={handleMouseUp}
    >
       {/* Visualizer Layer (Shows "Game" view behind menu) */}
       <div className="absolute inset-0 pointer-events-none flex items-center justify-center opacity-50">
          <div className="w-[800px] h-[600px] border border-white/10 relative bg-[url('https://img.freepik.com/free-photo/dark-futuristic-background-with-lines_23-2148812966.jpg')] bg-cover bg-center rounded-lg overflow-hidden">
             <div className="absolute top-4 left-4 text-green-400 font-mono text-xs bg-black/50 px-2 py-1">
               FPS: 240 | VAR: 0.1ms
             </div>
             {config.esp && ENEMIES.map(enemy => (
               <div key={enemy.id} style={{ position: 'absolute', left: enemy.x + 200, top: enemy.y + 100 }} className="transition-all duration-300">
                 {/* Player Model Representation */}
                 <div className={`w-[${enemy.width}px] h-[${enemy.height}px] opacity-50 ${config.chams ? 'bg-purple-500 mix-blend-screen' : 'bg-gray-500'}`}></div>
                 
                 {/* ESP Box */}
                 {config.box && (
                   <div 
                     style={{ width: enemy.width, height: enemy.height }} 
                     className="absolute top-0 left-0 border border-red-500 shadow-[0_0_5px_red]"
                   ></div>
                 )}

                 {/* Health Bar */}
                 {config.healthBar && (
                    <div className="absolute -left-2 top-0 bottom-0 w-1 bg-black">
                       <div style={{ height: `${enemy.hp}%` }} className="absolute bottom-0 w-full bg-green-500"></div>
                    </div>
                 )}

                 {/* Snapline */}
                 {config.snapLines && (
                    <div className="absolute top-0 left-1/2 w-0.5 h-[300px] -translate-y-full bg-white/20 origin-bottom transform rotate-12"></div>
                 )}
               </div>
             ))}
             
             {/* FOV Circle */}
             {config.aimbot && (
               <div 
                 className="absolute top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2 rounded-full border border-white/30 pointer-events-none"
                 style={{ width: config.fov * 50, height: config.fov * 50 }}
               ></div>
             )}
          </div>
       </div>

      {/* ImGui Menu */}
      <div 
        ref={modalRef}
        style={{ left: position.x, top: position.y, position: 'absolute' }}
        className="w-[700px] h-[480px] bg-[#141414] rounded shadow-[0_0_0_1px_#333,0_20px_60px_rgba(0,0,0,0.8)] flex flex-col font-sans text-[13px] select-none animate-fade-in-up overflow-hidden"
        onMouseDown={(e) => e.stopPropagation()}
      >
        
        {/* Header */}
        <div 
          className="h-8 bg-gradient-to-r from-noturnal-900 to-[#1a1a1a] flex items-center justify-between px-3 border-b border-[#333] cursor-move"
          onMouseDown={handleMouseDown}
        >
          <span className="text-noturnal-300 font-bold tracking-wide font-mono flex items-center gap-2">
            noturnal.cc | release build
          </span>
          <button onClick={onClose} className="text-[#888] hover:text-white transition-colors">
            <X className="w-3 h-3" />
          </button>
        </div>

        <div className="flex flex-1 overflow-hidden">
          {/* Tabs Sidebar */}
          <div className="w-40 bg-[#0f0f0f] border-r border-[#333] pt-4 px-2 space-y-1">
            {['Legit', 'Rage', 'Visuals', 'Misc', 'Skins', 'Config', 'Scripts'].map(tab => (
              <button
                key={tab}
                onClick={() => setActiveTab(tab)}
                className={`w-full text-left px-4 py-2.5 rounded text-xs font-bold transition-all flex items-center justify-between group ${
                  activeTab === tab 
                    ? 'bg-noturnal-600 text-white shadow-[0_2px_10px_rgba(118,69,253,0.2)]' 
                    : 'text-[#666] hover:bg-[#1a1a1a] hover:text-[#ccc]'
                }`}
              >
                {tab}
                {activeTab === tab && <div className="w-1.5 h-1.5 bg-white rounded-full shadow-[0_0_5px_white]"></div>}
              </button>
            ))}
          </div>

          {/* Content Area */}
          <div className="flex-1 bg-[#121212] p-6 grid grid-cols-2 gap-4 overflow-y-auto custom-scrollbar">
            
            {activeTab === 'Legit' && (
               <>
                 {/* GroupBox: General */}
                 <div className="border border-[#333] bg-[#161616] rounded p-3 h-fit">
                    <div className="text-noturnal-500 font-bold text-[11px] uppercase mb-3 translate-y-[-20px] bg-[#161616] px-2 w-fit">General</div>
                    <div className="space-y-3 -mt-2">
                        <Checkbox label="Enable Aimbot" checked={config.aimbot} onChange={(v) => updateConfig('aimbot', v)} />
                        <Checkbox label="Silent Aim" checked={false} onChange={() => {}} />
                        <Checkbox label="Auto Fire" checked={false} onChange={() => {}} />
                        <Slider label="FOV" value={config.fov} min={0.5} max={20} step={0.1} suffix="°" onChange={(v) => updateConfig('fov', v)} />
                        <Slider label="Smoothing" value={config.smooth} min={1} max={50} step={1} onChange={(v) => updateConfig('smooth', v)} />
                    </div>
                 </div>

                 {/* GroupBox: Accuracy */}
                 <div className="border border-[#333] bg-[#161616] rounded p-3 h-fit">
                    <div className="text-noturnal-500 font-bold text-[11px] uppercase mb-3 translate-y-[-20px] bg-[#161616] px-2 w-fit">Accuracy</div>
                    <div className="space-y-3 -mt-2">
                        <Checkbox label="Recoil Control System" checked={config.rcs} onChange={(v) => updateConfig('rcs', v)} />
                        <Slider label="RCS X" value={100} min={0} max={100} suffix="%" onChange={() => {}} />
                        <Slider label="RCS Y" value={100} min={0} max={100} suffix="%" onChange={() => {}} />
                        <Checkbox label="Backtrack" checked={true} onChange={() => {}} />
                    </div>
                 </div>
               </>
            )}

            {activeTab === 'Visuals' && (
               <>
                 {/* GroupBox: ESP */}
                 <div className="border border-[#333] bg-[#161616] rounded p-3 h-fit">
                    <div className="text-noturnal-500 font-bold text-[11px] uppercase mb-3 translate-y-[-20px] bg-[#161616] px-2 w-fit">Player ESP</div>
                    <div className="space-y-3 -mt-2">
                        <Checkbox label="Enable ESP" checked={config.esp} onChange={(v) => updateConfig('esp', v)} />
                        <Checkbox label="Box" checked={config.box} onChange={(v) => updateConfig('box', v)} />
                        <Checkbox label="Skeleton" checked={config.skeleton} onChange={(v) => updateConfig('skeleton', v)} />
                        <Checkbox label="Snaplines" checked={config.snapLines} onChange={(v) => updateConfig('snapLines', v)} />
                        <Checkbox label="Health Bar" checked={config.healthBar} onChange={(v) => updateConfig('healthBar', v)} />
                        <Checkbox label="Name" checked={true} onChange={() => {}} />
                        <Checkbox label="Weapon" checked={true} onChange={() => {}} />
                    </div>
                 </div>

                 {/* GroupBox: Chams */}
                 <div className="border border-[#333] bg-[#161616] rounded p-3 h-fit">
                    <div className="text-noturnal-500 font-bold text-[11px] uppercase mb-3 translate-y-[-20px] bg-[#161616] px-2 w-fit">Models</div>
                    <div className="space-y-3 -mt-2">
                        <Checkbox label="Enable Chams" checked={config.chams} onChange={(v) => updateConfig('chams', v)} />
                        <Checkbox label="Visible Only" checked={config.chamsVisible} onChange={(v) => updateConfig('chamsVisible', v)} />
                        <ColorPicker label="Visible Color" color="#7645fd" />
                        <ColorPicker label="Hidden Color" color="#22d3ee" />
                        <div className="flex justify-between items-center">
                          <span className="text-[#aaa]">Material</span>
                          <select className="bg-[#222] border border-[#333] text-[#ccc] text-xs rounded px-2 py-1 outline-none">
                             <option>Flat</option>
                             <option>Textured</option>
                             <option>Glow</option>
                          </select>
                        </div>
                    </div>
                 </div>
               </>
            )}

            {activeTab === 'Misc' && (
               <div className="col-span-2">
                  <div className="border border-[#333] bg-[#161616] rounded p-3 h-fit">
                    <div className="text-noturnal-500 font-bold text-[11px] uppercase mb-3 translate-y-[-20px] bg-[#161616] px-2 w-fit">Movement</div>
                    <div className="space-y-3 -mt-2">
                        <Checkbox label="Bunny Hop" checked={config.bhop} onChange={(v) => updateConfig('bhop', v)} />
                        <Checkbox label="Auto Strafe" checked={true} onChange={() => {}} />
                        <Checkbox label="Fast Stop" checked={false} onChange={() => {}} />
                        <Slider label="Hitchance" value={50} min={0} max={100} suffix="%" onChange={() => {}} />
                    </div>
                 </div>
               </div>
            )}
            
            {(activeTab === 'Rage' || activeTab === 'Skins' || activeTab === 'Config' || activeTab === 'Scripts') && (
               <div className="col-span-2 flex items-center justify-center h-full opacity-30">
                  <div className="text-center">
                     <div className="text-2xl font-bold text-white mb-2">Premium Feature</div>
                     <p className="text-[#888]">Upgrade to access {activeTab} settings.</p>
                  </div>
               </div>
            )}

          </div>
          
          {/* Footer Status */}
          <div className="bg-[#101010] border-t border-[#333] px-4 py-2 flex justify-between items-center text-[10px] text-[#555]">
             <span>User: Admin</span>
             <span>Expires: Never</span>
          </div>
        </div>
      </div>
    </div>
  );
};

// UI Helpers for ImGui Look
const Checkbox: React.FC<{ label: string; checked: boolean; onChange: (v: boolean) => void }> = ({ label, checked, onChange }) => (
  <label className="flex items-center gap-3 cursor-pointer group select-none">
    <div className={`w-4 h-4 border rounded-[3px] flex items-center justify-center transition-colors ${checked ? 'bg-noturnal-600 border-noturnal-500' : 'bg-[#222] border-[#444] group-hover:border-[#666]'}`}>
       {checked && <div className="w-2 h-2 bg-white rounded-[1px]"></div>}
    </div>
    <span className={`text-xs ${checked ? 'text-white' : 'text-[#aaa] group-hover:text-[#ccc]'}`}>{label}</span>
  </label>
);

const Slider: React.FC<{ label: string; value: number; min: number; max: number; step?: number; suffix?: string; onChange: (v: number) => void }> = ({ label, value, min, max, step=1, suffix='', onChange }) => (
  <div className="space-y-1">
    <div className="flex justify-between text-[11px]">
      <span className="text-[#aaa]">{label}</span>
      <span className="text-white font-mono">{value.toFixed(step === 1 ? 0 : 1)}{suffix}</span>
    </div>
    <input 
      type="range" 
      min={min} 
      max={max} 
      step={step} 
      value={value} 
      onChange={(e) => onChange(parseFloat(e.target.value))}
      className="w-full h-1.5 bg-[#222] rounded-full appearance-none accent-noturnal-500 cursor-pointer hover:accent-noturnal-400" 
    />
  </div>
);

const ColorPicker: React.FC<{ label: string; color: string }> = ({ label, color }) => (
  <div className="flex justify-between items-center">
    <span className="text-[#aaa]">{label}</span>
    <div className="flex gap-2 items-center">
       <div className="w-8 h-4 rounded border border-white/20" style={{ backgroundColor: color }}></div>
    </div>
  </div>
);