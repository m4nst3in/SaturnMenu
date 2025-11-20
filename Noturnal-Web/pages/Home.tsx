
import React, { useEffect, useRef } from 'react';
import { Link } from 'react-router-dom';
import { ChevronRight, Shield, Target, Eye, Play, Terminal, Cpu, Zap, Lock, Ghost, Activity, Cloud, Layers } from 'lucide-react';
import { ParticleBackground } from '../components/ParticleBackground';
import { ImguiPreviewModal } from '../components/ImguiPreviewModal';
import { TextScramble } from '../components/TextScramble';
import { MatrixSidebar } from '../components/MatrixSidebar';

export const Home: React.FC = () => {
  const [showPreview, setShowPreview] = React.useState(false);
  const cardsRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    const handleMouseMove = (e: MouseEvent) => {
      if (!cardsRef.current) return;
      const cards = cardsRef.current.getElementsByClassName('spotlight-card');
      for (const card of cards) {
        const rect = (card as HTMLElement).getBoundingClientRect();
        const x = e.clientX - rect.left;
        const y = e.clientY - rect.top;
        (card as HTMLElement).style.setProperty('--mouse-x', `${x}px`);
        (card as HTMLElement).style.setProperty('--mouse-y', `${y}px`);
      }
    };

    window.addEventListener('mousemove', handleMouseMove);
    return () => window.removeEventListener('mousemove', handleMouseMove);
  }, []);

  return (
    <div className="relative overflow-hidden min-h-screen bg-dark-bg selection:bg-noturnal-500/30">

      <ImguiPreviewModal isOpen={showPreview} onClose={() => setShowPreview(false)} />
      <MatrixSidebar />

      <ParticleBackground />

      {/* Aurora Effect */}
      <div className="absolute top-[-10%] left-1/2 -translate-x-1/2 w-[800px] h-[500px] bg-noturnal-600/20 rounded-full blur-[120px] pointer-events-none animate-pulse-slow mix-blend-screen"></div>
      <div className="absolute top-[10%] left-1/4 w-[400px] h-[400px] bg-cyan-500/10 rounded-full blur-[100px] pointer-events-none mix-blend-screen"></div>

      {/* Hero Section */}
      <div className="relative pt-48 pb-32 sm:pt-64 sm:pb-40 min-h-screen flex flex-col justify-center">
        <div className="relative z-10 max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 text-center">

          {/* Animated Badge */}
          <div className="inline-flex items-center gap-2 px-3 py-1.5 rounded-full bg-white/5 border border-white/10 mb-12 animate-fade-in backdrop-blur-xl hover:bg-white/10 transition-all cursor-default shadow-[0_0_30px_rgba(0,0,0,0.5)] group">
            <span className="relative flex h-2 w-2">
              <span className="animate-ping absolute inline-flex h-full w-full rounded-full bg-green-400 opacity-75"></span>
              <span className="relative inline-flex rounded-full h-2 w-2 bg-green-500"></span>
            </span>
            <span className="text-xs font-bold tracking-widest text-gray-300 font-mono uppercase group-hover:text-white transition-colors">Undetected</span>
            <div className="w-px h-3 bg-white/10 mx-2"></div>
            <span className="text-xs font-medium text-noturnal-400 group-hover:text-noturnal-300 transition-colors">Last Update: 2h ago</span>
          </div>

          {/* Main Title with Text Scramble - Gradient Fixed (Dark Purple to White) */}
          <h1 className="text-7xl md:text-9xl font-black tracking-tighter text-white mb-8 animate-fade-in-up leading-[0.85] drop-shadow-2xl">
            <TextScramble
              text="NOTURNAL"
              speed={40}
              className="block text-transparent bg-clip-text bg-gradient-to-b from-noturnal-800 via-noturnal-400 to-white pb-2"
            />
          </h1>

          <p className="mt-10 max-w-2xl mx-auto text-xl text-gray-400 animate-fade-in-up opacity-0 [animation-delay:200ms] leading-relaxed font-light tracking-wide">
            The kernel-level solution designed for players who want to improve. Undetectable by VAC. A legit solution for those who want to cheat with zero risk of getting banned.
          </p>

          {/* Buttons - Liquid Neon Effect */}
          <div className="mt-12 flex flex-col sm:flex-row justify-center gap-5 animate-fade-in-up opacity-0 [animation-delay:400ms]">
            <Link to="/pricing" className="btn-liquid group relative px-8 py-4 text-white rounded-full font-bold text-sm tracking-wide overflow-hidden transition-transform hover:scale-105">
              <span className="relative z-10 flex items-center gap-2">
                Get Access <ChevronRight className="w-4 h-4 transition-transform group-hover:translate-x-1" />
              </span>
            </Link>
            <button onClick={() => setShowPreview(true)} className="px-8 py-4 bg-white/5 border border-white/10 text-white rounded-full font-bold text-sm tracking-wide hover:bg-white/10 hover:border-white/20 transition-all flex items-center justify-center gap-2 backdrop-blur-sm hover:shadow-[0_0_30px_rgba(118,69,253,0.15)]">
              <Terminal className="w-4 h-4 text-noturnal-400" />
              Simulate Menu
            </button>
          </div>

        </div>


        {/* Perspective Grid - Idea #2 */}
        <div className="perspective-container">
          <div className="perspective-grid"></div>
        </div>
      </div>

      {/* Stats Strip with Glass effect */}
      <div className="border-y border-white/5 bg-[#050505]/50 backdrop-blur-md py-10 relative z-10">
        <div className="max-w-7xl mx-auto px-4 flex flex-wrap justify-center md:justify-between gap-8 md:gap-0 items-center">
          <div className="text-center md:text-left">
            <div className="text-4xl font-mono font-bold text-white tracking-tighter">1.4<span className="text-sm text-gray-500 ml-1">s</span></div>
            <div className="text-[10px] text-noturnal-400 uppercase tracking-widest font-bold mt-1 flex items-center gap-2 justify-center md:justify-start">
              <Zap className="w-3 h-3" /> Injection Speed
            </div>
          </div>
          <div className="w-px h-12 bg-white/5 hidden md:block"></div>
          <div className="text-center md:text-left">
            <div className="text-4xl font-mono font-bold text-white tracking-tighter">Ring 0</div>
            <div className="text-[10px] text-noturnal-400 uppercase tracking-widest font-bold mt-1 flex items-center gap-2 justify-center md:justify-start">
              <Shield className="w-3 h-3" /> Kernel Driver
            </div>
          </div>
          <div className="w-px h-12 bg-white/5 hidden md:block"></div>
          <div className="text-center md:text-left">
            <div className="text-4xl font-mono font-bold text-white tracking-tighter">15k<span className="text-xl text-gray-500">+</span></div>
            <div className="text-[10px] text-noturnal-400 uppercase tracking-widest font-bold mt-1 flex items-center gap-2 justify-center md:justify-start">
              <Ghost className="w-3 h-3" /> Active Users
            </div>
          </div>
          <div className="w-px h-12 bg-white/5 hidden md:block"></div>
          <div className="text-center md:text-left">
            <div className="text-4xl font-mono font-bold text-green-400 tracking-tighter">99.6%</div>
            <div className="text-[10px] text-green-500/70 uppercase tracking-widest font-bold mt-1 flex items-center gap-2 justify-center md:justify-start">
              <Activity className="w-3 h-3" /> Uptime SLA
            </div>
          </div>
        </div>
      </div>

      {/* Bento Grid Features */}
      <div className="relative py-32" ref={cardsRef}>
        <div className="relative z-10 max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="mb-24 max-w-3xl">
            <h2 className="text-5xl md:text-7xl font-bold text-white mb-8 tracking-tighter leading-tight">
              Engineered for <span className="text-transparent bg-clip-text bg-gradient-to-r from-noturnal-400 to-cyan-400">Supremacy</span>.
            </h2>
            <p className="text-gray-400 text-xl font-light leading-relaxed">
              Enough of these VAC Live bans. Time to legit cheat without any ban issues.
            </p>
          </div>

          {/* The Bento Grid */}
          <div className="grid grid-cols-1 md:grid-cols-3 lg:grid-cols-4 gap-4 md:gap-6 grid-flow-dense">

            {/* Large Card: Security (Col Span 2, Row Span 2) - With Border Beam */}
            <div className="spotlight-card col-span-1 md:col-span-2 lg:col-span-2 row-span-2 rounded-[2rem] p-8 md:p-10 border border-white/5 relative group overflow-hidden flex flex-col justify-between min-h-[400px]">
              {/* Border Beam Idea #3 */}
              <div className="border-beam"></div>

              <div className="absolute inset-0 bg-gradient-to-br from-noturnal-900/20 to-transparent opacity-50 group-hover:opacity-100 transition-opacity"></div>

              <div className="relative z-10">
                <div className="w-14 h-14 bg-noturnal-500 rounded-2xl flex items-center justify-center shadow-[0_0_40px_rgba(118,69,253,0.3)] mb-6">
                  <Shield className="w-7 h-7 text-white" />
                </div>
                <h3 className="text-3xl font-bold text-white mb-4 tracking-tight">Kernel Level Driver</h3>
                <p className="text-gray-400 text-base leading-relaxed max-w-md">Operating at kernel-level, our driver sits below the anti-cheat, ensuring total invisibility against VAC, reducing detection rate by 97%. Stealth, Fast, Undetected.</p>
              </div>

              {/* Visual Tech Element */}
              <div className="mt-8 relative h-32 w-full bg-black/40 rounded-xl border border-white/5 overflow-hidden">
                <div className="absolute top-0 left-0 w-full h-px bg-noturnal-500/50 shadow-[0_0_10px_#7645fd] animate-[shimmer_3s_infinite]"></div>
                <div className="p-4 font-mono text-[10px] text-green-400/80 space-y-1">
                  <div>{'>'} noturnal-km starting...</div>
                  <div>{'>'} mapping driver on kernel-level... [OK]</div>
                  <div>{'>'} starting driver r/w...</div>
                  <div className="animate-pulse">{'>'} awaiting usermode read instructions...</div>
                </div>
              </div>
            </div>

            {/* Tall Card: Aimbot (Col Span 1, Row Span 2) - With Glitch Effect Idea #5 */}
            <div className="spotlight-card glitch-card col-span-1 lg:col-span-1 row-span-2 rounded-[2rem] p-8 border border-white/5 relative group overflow-hidden flex flex-col bg-gradient-to-b from-[#0a0510] to-[#050208]">
              <div className="absolute top-0 right-0 w-32 h-32 bg-cyan-500/10 rounded-full blur-[60px] pointer-events-none"></div>

              <div className="mb-auto relative z-10 glitch-content">
                <div className="w-12 h-12 bg-cyan-900/20 border border-cyan-500/20 rounded-xl flex items-center justify-center mb-6">
                  <Target className="w-6 h-6 text-cyan-400" />
                </div>
                <h3 className="text-2xl font-bold text-white mb-2">Aim Assist</h3>
                <p className="text-gray-400 text-sm leading-relaxed">Smooth algorithm and math that mimic a real player movement. Control your recoil like a pro, and use our optimized aimbot for ez kills.</p>
              </div>

              <div className="mt-8 flex-1 flex items-center justify-center">
                {/* Abstract Target UI */}
                <div className="relative w-32 h-32 border border-white/10 rounded-full flex items-center justify-center animate-spin-slow">
                  <div className="absolute w-full h-full border-t border-cyan-500/50 rounded-full"></div>
                  <div className="w-2 h-2 bg-cyan-400 rounded-full shadow-[0_0_15px_#22d3ee]"></div>
                </div>
              </div>
            </div>

            {/* Standard Card: Visuals */}
            <div className="spotlight-card glitch-card col-span-1 rounded-[2rem] p-8 border border-white/5 relative group">
              <div className="w-12 h-12 bg-white/5 rounded-xl flex items-center justify-center mb-4 glitch-content">
                <Eye className="w-6 h-6 text-white" />
              </div>
              <div className="glitch-content">
                <h3 className="text-xl font-bold text-white mb-2">ESP System</h3>
                <p className="text-gray-400 text-xs">Glow, Chams, and Skeletons.</p>
              </div>
            </div>

            {/* Standard Card: Cloud */}
            <div className="spotlight-card glitch-card col-span-1 rounded-[2rem] p-8 border border-white/5 relative group">
              <div className="w-12 h-12 bg-white/5 rounded-xl flex items-center justify-center mb-4 glitch-content">
                <Cloud className="w-6 h-6 text-white" />
              </div>
              <div className="glitch-content">
                <h3 className="text-xl font-bold text-white mb-2">Cloud Configs</h3>
                <p className="text-gray-400 text-xs">Sync settings across devices.</p>
              </div>
            </div>

            {/* Wide Card: Performance (Col Span 2) */}
            <div className="spotlight-card glitch-card col-span-1 md:col-span-2 rounded-[2rem] p-8 border border-white/5 relative group flex items-center gap-6">
              <div className="shrink-0 w-16 h-16 bg-orange-500/10 border border-orange-500/20 rounded-2xl flex items-center justify-center glitch-content">
                <Zap className="w-8 h-8 text-orange-400" />
              </div>
              <div className="glitch-content">
                <h3 className="text-2xl font-bold text-white mb-2">Zero FPS Impact</h3>
                <p className="text-gray-400 text-sm">Optimized C++ codebase ensures your game runs buttery smooth. No overlay lag.</p>
              </div>
            </div>

            {/* Wide Card: Stream Proof (Col Span 2) */}
            <div className="spotlight-card glitch-card col-span-1 md:col-span-2 rounded-[2rem] p-8 border border-white/5 relative group flex flex-row-reverse items-center justify-between gap-6">
              <div className="shrink-0 w-16 h-16 bg-purple-500/10 border border-purple-500/20 rounded-2xl flex items-center justify-center glitch-content">
                <Layers className="w-8 h-8 text-purple-400" />
              </div>
              <div className="glitch-content">
                <h3 className="text-2xl font-bold text-white mb-2">Stream Proof</h3>
                <p className="text-gray-400 text-sm">Invisible to OBS, Discord, and screen capture. Show your skill, hide your edge.</p>
              </div>
            </div>

          </div>
        </div>
      </div>

    </div>
  );
};
