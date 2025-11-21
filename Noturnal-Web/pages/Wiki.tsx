
import React, { useState } from 'react';
import { 
  Search, 
  Crosshair, 
  Eye, 
  Shield, 
  Settings, 
  Cpu, 
  Zap, 
  AlertTriangle, 
  CheckCircle,
  MousePointer,
  Box,
  Layers,
  Ghost,
  Download,
  Power,
  HardDrive,
  Terminal,
  Activity
} from 'lucide-react';

// --- Data Structure for Wiki ---

type FeatureCategory = 'SETUP' | 'AIMBOT' | 'VISUALS' | 'MISC' | 'SKINS' | 'CONFIGS';

interface FeatureDoc {
  id: string;
  name: string;
  icon: React.ReactNode;
  description: string;
  category: FeatureCategory;
  riskLevel: 'SAFE' | 'MEDIUM' | 'RISKY' | 'REQUIRED';
  values: {
    legit: string;
    rage: string;
  };
  tip: string;
}

// Helper Icons
const SunIcon = (props: any) => (
  <svg {...props} xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round"><circle cx="12" cy="12" r="4"/><path d="M12 2v2"/><path d="M12 20v2"/><path d="m4.93 4.93 1.41 1.41"/><path d="m17.66 17.66 1.41 1.41"/><path d="M2 12h2"/><path d="M20 12h2"/><path d="m6.34 17.66-1.41 1.41"/><path d="m19.07 4.93-1.41 1.41"/></svg>
);
const TrophyIcon = (props: any) => (
  <svg {...props} xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round"><path d="M6 9H4.5a2.5 2.5 0 0 1 0-5H6"/><path d="M18 9h1.5a2.5 2.5 0 0 0 0-5H18"/><path d="M4 22h16"/><path d="M10 14.66V17c0 .55-.47.98-.97 1.21C7.85 18.75 7 20.24 7 22"/><path d="M14 14.66V17c0 .55.47.98.97 1.21C16.15 18.75 17 20.24 17 22"/><path d="M18 2H6v7a6 6 0 0 0 12 0V2Z"/></svg>
);

const WIKI_DATA: FeatureDoc[] = [
  // --- SETUP / INSTALLATION ---
  {
    id: 'setup-av',
    name: '1. System Preparation',
    icon: <Shield className="w-5 h-5" />,
    category: 'SETUP',
    description: 'Before downloading, you must disable interference. Anti-cheats and Antiviruses flag our encryption as a false positive.',
    riskLevel: 'REQUIRED',
    values: { legit: 'Real-time Protection: OFF', rage: 'SmartScreen: OFF' },
    tip: 'Uninstall third-party AVs (Avast, Norton, McAfee). They block the kernel driver driver access.'
  },
  {
    id: 'setup-download',
    name: '2. Download & Install',
    icon: <Download className="w-5 h-5" />,
    category: 'SETUP',
    description: 'Access the Dashboard to download the unique loader build generated for your HWID.',
    riskLevel: 'SAFE',
    values: { legit: 'Browser: Allow Download', rage: 'Run as: Administrator' },
    tip: 'The loader is portable. Put it on a USB drive for extra security if you are paranoid.'
  },
  {
    id: 'setup-inject-basic',
    name: '3a. Injection (Basic)',
    icon: <Terminal className="w-5 h-5" />,
    category: 'SETUP',
    description: 'For Basic Plan users (External). Provides visual features without kernel driver.',
    riskLevel: 'SAFE',
    values: { legit: 'Game Status: Main Menu', rage: 'Window Mode: Borderless' },
    tip: 'Open CS2 first, then run the loader and click Inject. Simple as that.'
  },
  {
    id: 'setup-inject-premium',
    name: '3b. Injection (Premium)',
    icon: <HardDrive className="w-5 h-5" />,
    category: 'SETUP',
    description: 'For Premium users. Loads the Kernel Driver (Ring 0) before Windows boots fully for maximum stealth.',
    riskLevel: 'REQUIRED',
    values: { legit: 'Step 1: Run Loader', rage: 'Step 2: Restart PC' },
    tip: 'The loader will ask to restart. After restart, the driver is loaded. Open CS2 and press INSERT.'
  },

  // --- AIMBOT ---
  {
    id: 'aim-fov',
    name: 'Field of View (FOV)',
    icon: <Crosshair className="w-5 h-5" />,
    category: 'AIMBOT',
    description: 'Determines the circular area around your crosshair where the aimbot will activate. A lower FOV looks more legitimate.',
    riskLevel: 'SAFE',
    values: { legit: '1.5° - 3.0°', rage: '180.0° (Max)' },
    tip: 'For Prime Matchmaking, keep FOV under 2.5° to avoid Overwatch suspicion.'
  },
  {
    id: 'aim-smooth',
    name: 'Smoothing',
    icon: <MousePointer className="w-5 h-5" />,
    category: 'AIMBOT',
    description: 'Controls how "human" the aim movement looks. Higher values mean slower, more natural movement. Lower values are robotic and instant.',
    riskLevel: 'SAFE',
    values: { legit: '15 - 25', rage: '1 (Instant)' },
    tip: 'Combine High Smooth with Low FOV for the ultimate "Ghost" config.'
  },
  {
    id: 'aim-rcs',
    name: 'Recoil Control (RCS)',
    icon: <Zap className="w-5 h-5" />,
    category: 'AIMBOT',
    description: 'Automatically compensates for weapon recoil patterns while shooting.',
    riskLevel: 'SAFE',
    values: { legit: 'X: 80% / Y: 85%', rage: 'X: 100% / Y: 100%' },
    tip: 'Do not use 100% RCS on legit configs. It looks too perfect. 85% is the sweet spot.'
  },
  {
    id: 'aim-trigger',
    name: 'Triggerbot',
    icon: <Zap className="w-5 h-5" />,
    category: 'AIMBOT',
    description: 'Automatically fires when your crosshair is over an enemy.',
    riskLevel: 'MEDIUM',
    values: { legit: 'Delay: 80-120ms', rage: 'Delay: 0ms' },
    tip: 'Always add a delay. 0ms reaction time is inhuman and easily detectable by spectators.'
  },

  // --- VISUALS ---
  {
    id: 'vis-esp-box',
    name: 'ESP Box',
    icon: <Box className="w-5 h-5" />,
    category: 'VISUALS',
    description: 'Draws a 2D box around enemies. Modes include Corners (stealthy) or Full Box (spatial info).',
    riskLevel: 'SAFE',
    values: { legit: 'Corners Only', rage: '3D Box' },
    tip: 'Use "Visible Only" mode to prevent accidental pre-fires on non-visible enemies.'
  },
  {
    id: 'vis-chams',
    name: 'Chams (Models)',
    icon: <Ghost className="w-5 h-5" />,
    category: 'VISUALS',
    description: 'Colors player models. "XQZ" mode renders enemies through walls, while "Visible Only" respects line-of-sight.',
    riskLevel: 'SAFE',
    values: { legit: 'Visible Only (Flat)', rage: 'XQZ (Wireframe)' },
    tip: 'XQZ (Through Walls) is powerful but risky. If you trace someone through a wall, you will be banned.'
  },
  {
    id: 'vis-glow',
    name: 'Glow ESP',
    icon: <SunIcon className="w-5 h-5" />,
    category: 'VISUALS',
    description: 'Renders a neon outline around players. High visibility in dark areas.',
    riskLevel: 'SAFE',
    values: { legit: 'Low Opacity', rage: 'High Brightness' },
    tip: 'Performance heavy. Disable on low-end PCs to maintain FPS.'
  },
  {
    id: 'vis-vischeck',
    name: 'Visible Only Filter',
    icon: <Eye className="w-5 h-5" />,
    category: 'VISUALS',
    description: 'Master safety switch. When enabled, visual features are only drawn when the enemy is directly visible.',
    riskLevel: 'SAFE',
    values: { legit: 'REQUIRED (ON)', rage: 'OFF' },
    tip: 'The #1 setting for avoiding Overwatch bans. Always keep this ON for legit play.'
  },
  {
    id: 'vis-skeleton',
    name: 'Skeleton ESP',
    icon: <Activity className="w-5 h-5" />,
    category: 'VISUALS',
    description: 'Draws player bone structure. Useful for seeing stance (crouch/stand) behind cover.',
    riskLevel: 'SAFE',
    values: { legit: 'OFF', rage: 'ON' },
    tip: 'Combine with XQZ for maximum information in HvH scenarios.'
  },

  // --- MISC ---
  {
    id: 'misc-bhop',
    name: 'Bunnyhop',
    icon: <Layers className="w-5 h-5" />,
    category: 'MISC',
    description: 'Automates jumping to gain speed and movement momentum.',
    riskLevel: 'RISKY',
    values: { legit: 'Humanized (80%)', rage: 'Perfect (100%)' },
    tip: 'Do not hold spacebar for the entire map. Use it in short bursts.'
  },
  {
    id: 'misc-rank',
    name: 'Rank Reveal',
    icon: <TrophyIcon className="w-5 h-5" />,
    category: 'MISC',
    description: 'Shows competitive ranks of all players in the scoreboard (Tab).',
    riskLevel: 'SAFE',
    values: { legit: 'On', rage: 'On' },
    tip: 'Client-side only feature. Totally safe to use.'
  },
];

const CATEGORIES: { id: FeatureCategory; label: string; icon: React.ReactNode }[] = [
  { id: 'SETUP', label: 'Installation Guide', icon: <Power className="w-4 h-4" /> },
  { id: 'AIMBOT', label: 'Aimbot & Combat', icon: <Crosshair className="w-4 h-4" /> },
  { id: 'VISUALS', label: 'ESP & Visuals', icon: <Eye className="w-4 h-4" /> },
  { id: 'MISC', label: 'Movement & Misc', icon: <Settings className="w-4 h-4" /> },
];

export const Wiki: React.FC = () => {
  const [activeCategory, setActiveCategory] = useState<FeatureCategory>('SETUP');
  const [searchTerm, setSearchTerm] = useState('');
  const [hoveredCard, setHoveredCard] = useState<string | null>(null);

  const filteredData = WIKI_DATA.filter(item => {
    const matchesSearch = item.name.toLowerCase().includes(searchTerm.toLowerCase()) || 
                          item.description.toLowerCase().includes(searchTerm.toLowerCase());
    const matchesCategory = searchTerm ? true : item.category === activeCategory;
    return matchesSearch && matchesCategory;
  });

  const getLegitLabel = (category: FeatureCategory) => {
    return category === 'SETUP' ? 'Requirement' : 'Legit Value';
  };

  const getRageLabel = (category: FeatureCategory) => {
    return category === 'SETUP' ? 'Action' : 'Rage Value';
  };

  return (
    <div className="min-h-screen bg-dark-bg pt-24 pb-12 relative overflow-hidden">
      {/* Background Elements */}
      <div className="absolute top-0 left-0 w-full h-[500px] bg-gradient-to-b from-noturnal-900/20 to-transparent pointer-events-none" />
      <div className="absolute top-40 right-0 w-[600px] h-[600px] bg-noturnal-600/5 rounded-full blur-[120px] pointer-events-none" />

      <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 relative z-10">
        
        {/* Header */}
        <div className="text-center mb-16 animate-fade-in-up">
           <div className="inline-flex items-center gap-2 px-4 py-1.5 rounded-full border border-white/10 bg-white/5 backdrop-blur-md mb-6">
             <Cpu className="w-4 h-4 text-noturnal-400" />
             <span className="text-xs font-mono font-bold text-gray-300 tracking-widest uppercase">Official Documentation</span>
           </div>
           <h1 className="text-5xl md:text-7xl font-bold text-white mb-6 tracking-tighter">
             Master The <span className="text-transparent bg-clip-text bg-gradient-to-r from-noturnal-400 to-cyan-400">Software</span>
           </h1>
           <p className="text-gray-400 max-w-2xl mx-auto text-lg leading-relaxed">
             Comprehensive guide to every feature. Learn how to configure the cheat for maximum legitimacy or total domination.
           </p>
        </div>

        {/* Main Layout */}
        <div className="grid grid-cols-1 lg:grid-cols-12 gap-8">
          
          {/* Left Sidebar / Navigation */}
          <div className="lg:col-span-3 space-y-8 animate-fade-in-up [animation-delay:100ms]">
            
            {/* Search */}
            <div className="relative group">
              <div className="absolute inset-0 bg-noturnal-500/20 rounded-xl blur opacity-0 group-hover:opacity-100 transition-opacity"></div>
              <div className="relative bg-[#0c0b14] border border-white/10 rounded-xl flex items-center px-4 py-3 focus-within:border-noturnal-500 transition-colors">
                <Search className="w-5 h-5 text-gray-500 mr-3" />
                <input 
                  type="text" 
                  placeholder="Search features..." 
                  value={searchTerm}
                  onChange={(e) => setSearchTerm(e.target.value)}
                  className="bg-transparent border-none outline-none text-white w-full placeholder-gray-600 text-sm"
                />
              </div>
            </div>

            {/* Categories */}
            <div className="space-y-2">
              <h3 className="text-xs font-bold text-gray-500 uppercase tracking-widest mb-4 pl-2">Modules</h3>
              {CATEGORIES.map((cat) => (
                <button
                  key={cat.id}
                  onClick={() => { setActiveCategory(cat.id); setSearchTerm(''); }}
                  className={`w-full flex items-center gap-3 px-4 py-3 rounded-xl transition-all duration-300 group relative overflow-hidden
                    ${activeCategory === cat.id && !searchTerm 
                      ? 'bg-noturnal-600 text-white shadow-[0_0_20px_rgba(118,69,253,0.3)]' 
                      : 'bg-white/5 text-gray-400 hover:bg-white/10 hover:text-white'
                    }`}
                >
                  <div className={`relative z-10`}>{cat.icon}</div>
                  <span className="relative z-10 text-sm font-bold tracking-wide">{cat.label}</span>
                  {activeCategory === cat.id && !searchTerm && (
                    <div className="absolute inset-0 bg-gradient-to-r from-noturnal-600 to-purple-600 opacity-100"></div>
                  )}
                </button>
              ))}
            </div>

            {/* Quick Tip Card */}
            <div className="p-6 rounded-2xl border border-noturnal-500/30 bg-gradient-to-br from-noturnal-900/50 to-transparent backdrop-blur-sm">
               <div className="w-10 h-10 bg-noturnal-500 rounded-lg flex items-center justify-center mb-4 shadow-lg shadow-noturnal-500/30">
                 <Zap className="w-6 h-6 text-white" />
               </div>
               <h4 className="text-white font-bold mb-2">Pro Tip</h4>
               <p className="text-xs text-gray-400 leading-relaxed">
                 For "Legit" play, always keep <span className="text-white font-mono">Smoothing</span> above 15 and <span className="text-white font-mono">FOV</span> below 2.5. This makes you invisible to Overwatch.
               </p>
            </div>
          </div>

          {/* Right Content Grid */}
          <div className="lg:col-span-9">
             <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
               {filteredData.map((feature, idx) => (
                 <div 
                   key={feature.id}
                   onMouseEnter={() => setHoveredCard(feature.id)}
                   onMouseLeave={() => setHoveredCard(null)}
                   className="group relative bg-[#0a0a0c] border border-white/5 rounded-2xl p-6 hover:border-noturnal-500/50 transition-all duration-500 animate-fade-in-up"
                   style={{ animationDelay: `${idx * 50}ms` }}
                 >
                   {/* Hover Glow */}
                   <div className="absolute inset-0 bg-gradient-to-br from-noturnal-500/5 to-transparent opacity-0 group-hover:opacity-100 transition-opacity rounded-2xl pointer-events-none"></div>
                   
                   <div className="relative z-10">
                     <div className="flex justify-between items-start mb-4">
                       <div className="p-3 rounded-xl bg-white/5 border border-white/10 group-hover:bg-noturnal-500/20 group-hover:border-noturnal-500/30 transition-colors text-noturnal-400">
                         {feature.icon}
                       </div>
                       <div className={`px-2 py-1 rounded text-[10px] font-bold uppercase tracking-wider border ${
                         feature.riskLevel === 'SAFE' ? 'border-green-500/30 text-green-400 bg-green-500/10' : 
                         feature.riskLevel === 'MEDIUM' ? 'border-yellow-500/30 text-yellow-400 bg-yellow-500/10' : 
                         feature.riskLevel === 'REQUIRED' ? 'border-blue-500/30 text-blue-400 bg-blue-500/10' :
                         'border-red-500/30 text-red-400 bg-red-500/10'
                       }`}>
                         {feature.riskLevel}
                       </div>
                     </div>

                     <h3 className="text-xl font-bold text-white mb-2 group-hover:text-noturnal-400 transition-colors">{feature.name}</h3>
                     <p className="text-gray-400 text-sm mb-6 h-10 leading-relaxed">{feature.description}</p>

                     {/* Values Grid */}
                     <div className="bg-black/40 rounded-xl p-4 border border-white/5 space-y-3">
                       <div className="flex justify-between items-center pb-2 border-b border-white/5">
                         <span className="text-xs text-gray-500 uppercase font-bold flex items-center gap-2">
                           {feature.category === 'SETUP' ? <CheckCircle className="w-3 h-3 text-blue-500" /> : <CheckCircle className="w-3 h-3 text-green-500" />}
                           {getLegitLabel(feature.category)}
                         </span>
                         <span className="text-sm font-mono text-white bg-white/5 px-2 py-0.5 rounded border border-white/10">
                           {feature.values.legit}
                         </span>
                       </div>
                       <div className="flex justify-between items-center">
                         <span className="text-xs text-gray-500 uppercase font-bold flex items-center gap-2">
                           {feature.category === 'SETUP' ? <Terminal className="w-3 h-3 text-blue-500" /> : <AlertTriangle className="w-3 h-3 text-red-500" />}
                           {getRageLabel(feature.category)}
                         </span>
                         <span className="text-sm font-mono text-white bg-white/5 px-2 py-0.5 rounded border border-white/10">
                           {feature.values.rage}
                         </span>
                       </div>
                     </div>

                     {/* Dynamic Tip on Hover */}
                     <div className={`mt-4 overflow-hidden transition-all duration-500 ease-out ${hoveredCard === feature.id ? 'max-h-20 opacity-100' : 'max-h-0 opacity-0'}`}>
                        <p className="text-xs text-noturnal-300 italic border-l-2 border-noturnal-500 pl-3">
                          "{feature.tip}"
                        </p>
                     </div>
                   </div>
                 </div>
               ))}
             </div>

             {filteredData.length === 0 && (
               <div className="text-center py-20 opacity-50">
                 <Search className="w-12 h-12 text-gray-500 mx-auto mb-4" />
                 <h3 className="text-xl font-bold text-white">No functions found</h3>
                 <p className="text-gray-400">Try searching for "Aimbot", "ESP", or "Recoil".</p>
               </div>
             )}
          </div>

        </div>
      </div>
    </div>
  );
};
