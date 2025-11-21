import React, { useState, useEffect } from 'react';
import { useAuth } from '../context/AuthContext';
import { api } from '../services/api';
import { Download, AlertTriangle, CheckCircle, Clock, ShieldAlert, FileJson, Copy, Plus, Settings, BarChart, DollarSign, Edit } from 'lucide-react';
import { Link } from 'react-router-dom';
import { TerminalModal } from '../components/TerminalModal';
import { ConfigItem, ConfigType } from '../types';

export const Dashboard: React.FC = () => {
  const { user, resetHwid } = useAuth();
  const [resettingHwid, setResettingHwid] = useState(false);
  const [showTerminal, setShowTerminal] = useState(false);
  const [msg, setMsg] = useState('');
  const [copiedConfig, setCopiedConfig] = useState<string | null>(null);
  const [myConfigs, setMyConfigs] = useState<ConfigItem[]>([]);
  const [isCreating, setIsCreating] = useState(false);
  
  // New Config Form State
  const [newConfigName, setNewConfigName] = useState('');
  const [newConfigType, setNewConfigType] = useState<ConfigType>('LEGIT');
  const [newConfigPrice, setNewConfigPrice] = useState(0);
  const [newConfigDesc, setNewConfigDesc] = useState('');

  const [downloadUrl, setDownloadUrl] = useState<string>("");

  useEffect(() => {
    if (user) {
      api.getMyConfigs(user.id).then(setMyConfigs);
    }
  }, [user]);

  const handleDownload = async () => {
    if (!user?.subscription.active) return;
    const r = await api.downloadLoader(user.subscription.plan as any);
    setDownloadUrl(r.url);
    setShowTerminal(true);
  };

  const handleResetHwid = async () => {
    if (!user) return;
    if (!confirm("Are you sure you want to reset your HWID? This action cannot be undone immediately.")) return;

    setResettingHwid(true);
    try {
      await resetHwid();
      setMsg("HWID reset successfully. Launch loader to lock new HWID.");
    } catch (e) {
      setMsg("Failed to reset HWID.");
    } finally {
      setResettingHwid(false);
    }
  };

  const handleCopyConfig = (code: string) => {
    navigator.clipboard.writeText(code);
    setCopiedConfig(code);
    setTimeout(() => setCopiedConfig(null), 2000);
  };

  const handleCreateConfig = async (e: React.FormEvent) => {
    e.preventDefault();
    const created = await api.publishConfig({
      name: newConfigName,
      type: newConfigType,
      price: newConfigPrice,
      description: newConfigDesc
    });
    setMyConfigs([created, ...myConfigs]);
    setIsCreating(false);
    setNewConfigName('');
    setNewConfigDesc('');
  };

  if (!user) return null;

  const isPremium = user.subscription.plan === 'PREMIUM';

  return (
    <div className="pt-24 pb-12 min-h-screen bg-dark-bg px-4">
      <TerminalModal 
        isOpen={showTerminal} 
        onClose={() => setShowTerminal(false)} 
        downloadUrl={downloadUrl}
      />

      {/* Creation Modal */}
      {isCreating && (
        <div className="fixed inset-0 z-[100] flex items-center justify-center bg-black/80 backdrop-blur-sm px-4">
          <div className="w-full max-w-lg bg-[#0f0f13] border border-white/10 rounded-2xl p-8 relative">
            <button onClick={() => setIsCreating(false)} className="absolute top-4 right-4 text-gray-500 hover:text-white">
              <Settings className="w-5 h-5" />
            </button>
            <h2 className="text-2xl font-bold text-white mb-6">Publish New Config</h2>
            <form onSubmit={handleCreateConfig} className="space-y-4">
              <div>
                <label className="block text-xs font-bold text-gray-500 uppercase mb-1">Config Name</label>
                <input required type="text" value={newConfigName} onChange={e => setNewConfigName(e.target.value)} className="w-full bg-black/30 border border-white/10 rounded-lg p-3 text-white focus:border-noturnal-500 outline-none" placeholder="e.g., Global Elite Legit" />
              </div>
              <div className="grid grid-cols-2 gap-4">
                <div>
                  <label className="block text-xs font-bold text-gray-500 uppercase mb-1">Type</label>
                  <select value={newConfigType} onChange={e => setNewConfigType(e.target.value as any)} className="w-full bg-black/30 border border-white/10 rounded-lg p-3 text-white focus:border-noturnal-500 outline-none">
                    <option value="LEGIT">Legit</option>
                    <option value="RAGE">Rage</option>
                    <option value="SEMI">Semi-Rage</option>
                    <option value="HVH">HvH</option>
                  </select>
                </div>
                <div>
                  <label className="block text-xs font-bold text-gray-500 uppercase mb-1">Price ($)</label>
                  <input type="number" min="0" max="50" step="0.01" value={newConfigPrice} onChange={e => setNewConfigPrice(parseFloat(e.target.value))} className="w-full bg-black/30 border border-white/10 rounded-lg p-3 text-white focus:border-noturnal-500 outline-none" />
                </div>
              </div>
              <div>
                <label className="block text-xs font-bold text-gray-500 uppercase mb-1">Description</label>
                <textarea required value={newConfigDesc} onChange={e => setNewConfigDesc(e.target.value)} className="w-full bg-black/30 border border-white/10 rounded-lg p-3 text-white focus:border-noturnal-500 outline-none h-24" placeholder="Describe your playstyle..." />
              </div>
              <button type="submit" className="w-full bg-noturnal-600 hover:bg-noturnal-500 text-white py-3 rounded-lg font-bold">
                Publish to Marketplace
              </button>
            </form>
          </div>
        </div>
      )}

      <div className="max-w-6xl mx-auto space-y-6">
        
        {/* Header Stats */}
        <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
           {/* User Profile */}
           <div className="glass p-6 rounded-xl border border-white/5">
              <h3 className="text-gray-400 text-sm font-mono">USER</h3>
              <div className="mt-2 text-2xl font-bold text-white">{user.username}</div>
              <div className="text-xs text-gray-500 font-mono mt-1">UID: {user.id}</div>
           </div>
           
           {/* Subscription Status */}
           <div className="glass p-6 rounded-xl border border-white/5 relative overflow-hidden">
              <h3 className="text-gray-400 text-sm font-mono">SUBSCRIPTION STATUS</h3>
              <div className="mt-2 flex items-center gap-2">
                 {user.subscription.active ? (
                   <>
                    <CheckCircle className="text-green-500 w-6 h-6" />
                    <span className="text-2xl font-bold text-white">{user.subscription.plan}</span>
                   </>
                 ) : (
                   <>
                    <AlertTriangle className="text-red-500 w-6 h-6" />
                    <span className="text-2xl font-bold text-gray-300">INACTIVE</span>
                   </>
                 )}
              </div>
              {user.subscription.expiresAt && (
                <div className="text-xs text-gray-500 font-mono mt-1 flex items-center gap-1">
                  <Clock className="w-3 h-3" /> Expires in: {new Date(user.subscription.expiresAt).toLocaleDateString()}
                </div>
              )}
              {user.subscription.active && (
                 <div className="absolute right-0 top-0 p-4">
                    <div className="w-24 h-24 bg-green-500/10 rounded-full blur-xl"></div>
                 </div>
              )}
           </div>

           {/* HWID Status */}
           <div className="glass p-6 rounded-xl border border-white/5">
              <h3 className="text-gray-400 text-sm font-mono">HWID</h3>
              <div className="mt-2 font-mono text-sm text-gray-300 break-all">
                {user.hwid || "Not registered"}
              </div>
              <button 
                onClick={handleResetHwid}
                disabled={!user.hwid || resettingHwid}
                className="mt-4 text-xs text-noturnal-400 hover:text-noturnal-300 underline disabled:opacity-50 disabled:cursor-not-allowed"
              >
                {resettingHwid ? "Resetting..." : "Reset HWID"}
              </button>
           </div>
        </div>

        {/* Main Action Area */}
        <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
          
          {/* Download Center */}
          <div className="lg:col-span-2 glass p-8 rounded-xl border border-white/5 flex flex-col justify-center items-center text-center relative">
             {!user.subscription.active ? (
               <div className="space-y-4">
                 <ShieldAlert className="w-16 h-16 text-red-500 mx-auto opacity-80" />
                 <h2 className="text-2xl font-bold text-white">Access Blocked</h2>
                 <p className="text-gray-400">You need an active subscription to access the loader.</p>
                 <Link to="/pricing" className="inline-block bg-noturnal-600 hover:bg-noturnal-700 text-white px-6 py-3 rounded-lg font-bold mt-4">
                   View Plans
                 </Link>
               </div>
             ) : (
               <div className="space-y-6 w-full max-w-md">
                 <div className={`w-20 h-20 mx-auto rounded-full flex items-center justify-center ${isPremium ? 'bg-noturnal-500/20 shadow-[0_0_30px_rgba(139,92,246,0.3)]' : 'bg-blue-500/20'}`}>
                    <Download className={`w-10 h-10 ${isPremium ? 'text-noturnal-400' : 'text-blue-400'}`} />
                 </div>
                 <div>
                   <h2 className="text-3xl font-bold text-white">Download Loader</h2>
                   <p className="text-gray-400 mt-2">Version: <span className="font-mono text-white">v2.4.1 (Stable)</span></p>
                   {isPremium && <p className="text-noturnal-400 text-xs font-bold tracking-widest mt-1">KERNEL MODE ENABLED</p>}
                 </div>
                 
                 <button 
                   onClick={handleDownload}
                   className={`w-full py-4 rounded-lg font-bold text-lg transition-all ${
                     isPremium 
                    ? 'bg-gradient-to-r from-noturnal-600 to-purple-600 hover:from-noturnal-500 hover:to-purple-500 text-white shadow-lg'
                     : 'bg-blue-600 hover:bg-blue-500 text-white'
                   } disabled:opacity-50 disabled:cursor-not-allowed`}
                 >
                   LAUNCH SECURE DOWNLOADER
                 </button>
                 
                 {msg && <div className="text-xs font-mono text-green-400">{msg}</div>}
               </div>
             )}
          </div>

          {/* Updates Feed */}
          <div className="glass p-6 rounded-xl border border-white/5">
            <h3 className="text-white font-bold mb-4 flex items-center gap-2">
              <div className="w-2 h-2 bg-green-500 rounded-full animate-pulse"></div>
              CHANGELOG
            </h3>
            <div className="space-y-4 relative">
               <div className="absolute left-2 top-2 bottom-2 w-0.5 bg-white/10"></div>
               <div className="relative pl-6">
                 <div className="absolute left-0 top-1.5 w-4 h-4 rounded-full bg-dark-bg border-2 border-noturnal-500"></div>
                 <div className="text-xs text-noturnal-400 font-mono mb-1">TODAY</div>
                 <p className="text-gray-300 text-sm">Security update v2.4.1. Offsets updated for latest CS2 patch.</p>
               </div>
               <div className="relative pl-6">
                 <div className="absolute left-0 top-1.5 w-4 h-4 rounded-full bg-dark-bg border-2 border-gray-600"></div>
                 <div className="text-xs text-gray-500 font-mono mb-1">2 DAYS AGO</div>
                 <p className="text-gray-400 text-sm">Added new Skin Changer for knives (Kukri).</p>
               </div>
            </div>
          </div>
        </div>

        {/* Config Studio Section */}
        <div className="mt-12">
           <div className="flex items-center justify-between mb-6">
             <div>
                <h2 className="text-2xl font-bold text-white flex items-center gap-2">
                    Config Studio <div className="text-[10px] bg-noturnal-500/20 text-noturnal-400 border border-noturnal-500/30 px-2 py-0.5 rounded font-mono">BETA</div>
                </h2>
                <p className="text-sm text-gray-400">Manage, monetize, and share your configuration files.</p>
             </div>
             <button 
               onClick={() => setIsCreating(true)}
               className="bg-white/5 hover:bg-white/10 border border-white/10 text-white px-4 py-2 rounded-lg flex items-center gap-2 font-medium text-sm transition-colors"
             >
                <Plus className="w-4 h-4" /> Create New
             </button>
           </div>

           {/* Stats Row */}
           <div className="grid grid-cols-1 md:grid-cols-3 gap-4 mb-8">
              <div className="bg-[#0c0b14] border border-white/5 p-4 rounded-xl flex items-center gap-4">
                  <div className="w-10 h-10 rounded-full bg-blue-500/10 flex items-center justify-center"><Download className="w-5 h-5 text-blue-400" /></div>
                  <div>
                      <div className="text-2xl font-bold text-white">{myConfigs.reduce((acc, c) => acc + c.downloads, 0)}</div>
                      <div className="text-xs text-gray-500 uppercase">Total Downloads</div>
                  </div>
              </div>
              <div className="bg-[#0c0b14] border border-white/5 p-4 rounded-xl flex items-center gap-4">
                  <div className="w-10 h-10 rounded-full bg-green-500/10 flex items-center justify-center"><DollarSign className="w-5 h-5 text-green-400" /></div>
                  <div>
                      <div className="text-2xl font-bold text-white">$0.00</div>
                      <div className="text-xs text-gray-500 uppercase">Total Earnings</div>
                  </div>
              </div>
              <div className="bg-[#0c0b14] border border-white/5 p-4 rounded-xl flex items-center gap-4">
                  <div className="w-10 h-10 rounded-full bg-purple-500/10 flex items-center justify-center"><FileJson className="w-5 h-5 text-purple-400" /></div>
                  <div>
                      <div className="text-2xl font-bold text-white">{myConfigs.length}</div>
                      <div className="text-xs text-gray-500 uppercase">Published Configs</div>
                  </div>
              </div>
           </div>
           
           {/* User Configs List */}
           <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
             {myConfigs.map((cfg) => (
              <div key={cfg.id} className="glass p-5 rounded-xl border border-white/5 hover:border-noturnal-500/30 transition-all group">
                 <div className="flex justify-between items-start mb-3">
                   <div className="flex items-center gap-2">
                     <div className="p-2 bg-white/5 rounded-lg"><FileJson className="w-5 h-5 text-noturnal-400" /></div>
                     <div>
                        <span className="font-bold text-white block">{cfg.name}</span>
                        <span className="text-xs text-gray-500">{cfg.shareCode}</span>
                     </div>
                   </div>
                   <span className={`text-[10px] px-2 py-1 rounded border font-bold ${cfg.type === 'LEGIT' ? 'border-green-500/30 text-green-400' : 'border-red-500/30 text-red-400'}`}>
                     {cfg.type}
                   </span>
                 </div>
                 
                 <div className="flex items-center justify-between text-xs text-gray-400 mb-4 bg-black/20 p-2 rounded">
                    <span className="flex items-center gap-1"><Download className="w-3 h-3" /> {cfg.downloads}</span>
                    <span className="flex items-center gap-1"><BarChart className="w-3 h-3" /> {cfg.rating}</span>
                    <span className="flex items-center gap-1 text-green-400"><DollarSign className="w-3 h-3" /> {cfg.price}</span>
                 </div>

                 <div className="flex gap-2">
                    <button 
                        onClick={() => handleCopyConfig(cfg.shareCode)}
                        className="flex-1 bg-white/5 hover:bg-white/10 py-2 rounded text-xs text-gray-300 flex items-center justify-center gap-2 transition-colors"
                    >
                        {copiedConfig === cfg.shareCode ? <CheckCircle className="w-3 h-3 text-green-500" /> : <Copy className="w-3 h-3" />}
                        {copiedConfig === cfg.shareCode ? 'Copied' : 'Copy Code'}
                    </button>
                    <button className="px-3 bg-white/5 hover:bg-white/10 rounded text-gray-300 transition-colors">
                        <Edit className="w-3 h-3" />
                    </button>
                 </div>
               </div>
             ))}
             
             {/* Add New Placeholder Card */}
             <button 
               onClick={() => setIsCreating(true)}
               className="border border-dashed border-white/10 rounded-xl flex flex-col items-center justify-center p-6 text-gray-600 hover:text-noturnal-400 hover:border-noturnal-500/30 hover:bg-noturnal-500/5 transition-all"
             >
                 <Plus className="w-8 h-8 mb-2" />
                 <span className="font-bold text-sm">Upload New Config</span>
             </button>
           </div>
        </div>

      </div>
    </div>
  );
};