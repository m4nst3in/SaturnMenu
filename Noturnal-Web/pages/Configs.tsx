import React, { useState, useEffect, useRef } from 'react';
import { api } from '../services/api';
import { ConfigItem, ConfigType } from '../types';
import { Search, Filter, Star, Download, Copy, Check, Shield, Zap, Crosshair, Skull, ShoppingCart } from 'lucide-react';
import { Link } from 'react-router-dom';

const ConfigCard: React.FC<{ config: ConfigItem }> = ({ config }) => {
  const [copied, setCopied] = useState(false);

  const handleCopy = () => {
    navigator.clipboard.writeText(config.shareCode);
    setCopied(true);
    setTimeout(() => setCopied(false), 2000);
  };

  const getTypeColor = (type: string) => {
    switch (type) {
      case 'LEGIT': return 'text-green-400 border-green-500/30 bg-green-500/10';
      case 'RAGE': return 'text-red-400 border-red-500/30 bg-red-500/10';
      case 'SEMI': return 'text-orange-400 border-orange-500/30 bg-orange-500/10';
      case 'HVH': return 'text-purple-400 border-purple-500/30 bg-purple-500/10';
      default: return 'text-gray-400';
    }
  };

  return (
    <div className="spotlight-card group relative bg-[#0c0b14] border border-white/5 rounded-2xl overflow-hidden transition-all duration-300 hover:border-noturnal-500/40 hover:-translate-y-1">
      {/* Header Gradient */}
      <div className={`absolute top-0 left-0 w-full h-1 ${
         config.type === 'LEGIT' ? 'bg-green-500' : 
         config.type === 'RAGE' ? 'bg-red-500' : 
         config.type === 'SEMI' ? 'bg-orange-500' : 'bg-purple-500'
      } shadow-[0_0_15px_currentColor]`}></div>

      <div className="p-5 relative z-10">
        {/* Top Row */}
        <div className="flex justify-between items-start mb-4">
          <div className={`px-2.5 py-1 rounded text-[10px] font-bold uppercase tracking-widest border ${getTypeColor(config.type)}`}>
            {config.type}
          </div>
          <div className="flex items-center gap-1 text-yellow-400 text-xs font-bold">
            <Star className="w-3 h-3 fill-yellow-400" />
            {config.rating}
          </div>
        </div>

        {/* Info */}
        <h3 className="text-lg font-bold text-white mb-1 group-hover:text-transparent group-hover:bg-clip-text group-hover:bg-gradient-to-r group-hover:from-white group-hover:to-gray-400 transition-all">
          {config.name}
        </h3>
        <div className="flex items-center gap-2 mb-4">
          <span className="text-xs text-gray-500">by</span>
          <span className="text-xs font-medium text-noturnal-400">{config.author}</span>
          {config.verified && <Shield className="w-3 h-3 text-blue-400" />}
        </div>

        <p className="text-xs text-gray-500 leading-relaxed mb-6 h-10 line-clamp-2">
          {config.description}
        </p>

        {/* Stats */}
        <div className="flex items-center gap-4 mb-6 text-xs text-gray-400 font-mono">
          <div className="flex items-center gap-1.5">
            <Download className="w-3 h-3" /> {config.downloads.toLocaleString()}
          </div>
          <div className="flex items-center gap-1.5">
             Updated: {new Date(config.lastUpdated).toLocaleDateString()}
          </div>
        </div>

        {/* Actions */}
        <div className="flex items-center gap-2">
          {config.price === 0 ? (
            <button 
              onClick={handleCopy}
              className="flex-1 bg-white/5 hover:bg-white/10 border border-white/10 hover:border-white/20 text-gray-300 py-2.5 rounded-lg text-xs font-bold transition-all flex items-center justify-center gap-2"
            >
              {copied ? <Check className="w-3 h-3 text-green-400" /> : <Copy className="w-3 h-3" />}
              {copied ? 'COPIED' : 'COPY CODE'}
            </button>
          ) : (
            <button className="flex-1 bg-gradient-to-r from-noturnal-600 to-noturnal-500 hover:from-noturnal-500 hover:to-noturnal-400 text-white py-2.5 rounded-lg text-xs font-bold transition-all flex items-center justify-center gap-2 shadow-lg shadow-noturnal-500/20">
              <ShoppingCart className="w-3 h-3" />
              BUY ${config.price}
            </button>
          )}
        </div>
      </div>
    </div>
  );
};

export const Configs: React.FC = () => {
  const [configs, setConfigs] = useState<ConfigItem[]>([]);
  const [filter, setFilter] = useState<ConfigType | 'ALL'>('ALL');
  const [search, setSearch] = useState('');
  const [loading, setLoading] = useState(true);
  const cardsRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    const load = async () => {
      setLoading(true);
      const data = await api.getConfigs();
      setConfigs(data);
      setLoading(false);
    };
    load();
  }, []);

  // Spotlight effect logic
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
  }, [configs]);

  const filtered = configs.filter(c => {
    const matchFilter = filter === 'ALL' || c.type === filter;
    const matchSearch = c.name.toLowerCase().includes(search.toLowerCase()) || c.author.toLowerCase().includes(search.toLowerCase());
    return matchFilter && matchSearch;
  });

  return (
    <div className="min-h-screen bg-dark-bg pt-24 pb-12 relative overflow-hidden">
      
      {/* Hero Section */}
      <div className="relative z-10 max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 mb-12">
        <div className="text-center mb-12">
           <div className="inline-flex items-center gap-2 px-4 py-1.5 rounded-full border border-noturnal-500/30 bg-noturnal-500/10 backdrop-blur-md mb-6 animate-fade-in">
             <Zap className="w-4 h-4 text-noturnal-400" />
             <span className="text-xs font-mono font-bold text-noturnal-200 tracking-widest uppercase">Cloud Market</span>
           </div>
           <h1 className="text-5xl md:text-7xl font-black text-white mb-6 tracking-tighter animate-fade-in-up">
             Find Your <span className="text-transparent bg-clip-text bg-gradient-to-r from-noturnal-400 to-cyan-400">Playstyle</span>
           </h1>
           <p className="text-gray-400 max-w-2xl mx-auto text-lg leading-relaxed animate-fade-in-up [animation-delay:200ms]">
             Browse thousands of community-created configs. From legit-bot presets to rage-hacking masterpieces. Monetize your own configs securely.
           </p>
        </div>

        {/* Search & Filter Bar */}
        <div className="flex flex-col md:flex-row gap-4 items-center justify-between bg-[#0a0a0c]/60 backdrop-blur-xl border border-white/10 p-2 rounded-2xl mb-8 animate-fade-in-up [animation-delay:300ms]">
          {/* Search */}
          <div className="relative flex-1 w-full md:w-auto">
             <Search className="absolute left-4 top-1/2 -translate-y-1/2 w-5 h-5 text-gray-500" />
             <input 
               type="text" 
               placeholder="Search configs, authors, or tags..." 
               value={search}
               onChange={(e) => setSearch(e.target.value)}
               className="w-full bg-transparent border-none outline-none text-white pl-12 pr-4 py-3 placeholder-gray-600"
             />
          </div>

          {/* Divider */}
          <div className="hidden md:block w-px h-8 bg-white/10"></div>

          {/* Filters */}
          <div className="flex gap-2 p-1 overflow-x-auto w-full md:w-auto no-scrollbar">
             {[
               { id: 'ALL', icon: <Filter className="w-3 h-3" /> },
               { id: 'LEGIT', icon: <Crosshair className="w-3 h-3" /> },
               { id: 'SEMI', icon: <Shield className="w-3 h-3" /> },
               { id: 'RAGE', icon: <Skull className="w-3 h-3" /> },
               { id: 'HVH', icon: <Zap className="w-3 h-3" /> }
             ].map(f => (
               <button
                 key={f.id}
                 onClick={() => setFilter(f.id as any)}
                 className={`flex items-center gap-2 px-4 py-2 rounded-xl text-xs font-bold uppercase tracking-wider transition-all whitespace-nowrap
                   ${filter === f.id 
                     ? 'bg-noturnal-600 text-white shadow-[0_0_15px_rgba(118,69,253,0.4)]' 
                     : 'bg-white/5 text-gray-400 hover:bg-white/10 hover:text-white'
                   }`}
               >
                 {f.icon} {f.id}
               </button>
             ))}
          </div>
        </div>

        {/* Content Grid */}
        {loading ? (
          <div className="text-center py-20">
            <div className="animate-spin w-8 h-8 border-2 border-noturnal-500 border-t-transparent rounded-full mx-auto mb-4"></div>
            <p className="text-gray-500 font-mono text-xs">Fetching marketplace data...</p>
          </div>
        ) : (
          <div ref={cardsRef} className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 xl:grid-cols-4 gap-6">
             {filtered.map((cfg, idx) => (
               <div key={cfg.id} className="animate-fade-in-up" style={{ animationDelay: `${idx * 50}ms` }}>
                 <ConfigCard config={cfg} />
               </div>
             ))}
          </div>
        )}

        {!loading && filtered.length === 0 && (
          <div className="text-center py-20 opacity-50">
             <Filter className="w-12 h-12 text-gray-600 mx-auto mb-4" />
             <h3 className="text-xl font-bold text-white">No configs found</h3>
             <p className="text-gray-400">Try adjusting your search filters.</p>
          </div>
        )}

      </div>
    </div>
  );
};