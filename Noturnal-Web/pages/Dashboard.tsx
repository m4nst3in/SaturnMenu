
import React, { useState } from 'react';
import { useAuth } from '../context/AuthContext';
import { api } from '../services/api';
import { Download, AlertTriangle, CheckCircle, Clock, ShieldAlert, FileJson, Copy } from 'lucide-react';
import { Link } from 'react-router-dom';
import { TerminalModal } from '../components/TerminalModal';

export const Dashboard: React.FC = () => {
  const { user, resetHwid } = useAuth();
  const [resettingHwid, setResettingHwid] = useState(false);
  const [showTerminal, setShowTerminal] = useState(false);
  const [msg, setMsg] = useState('');
  const [copiedConfig, setCopiedConfig] = useState<string | null>(null);

  const [downloadUrl, setDownloadUrl] = useState<string>("");
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

  const handleCopyConfig = (id: string) => {
    setCopiedConfig(id);
    setTimeout(() => setCopiedConfig(null), 2000);
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

                 <div className="bg-black/30 p-4 rounded-lg text-left text-xs text-gray-500 space-y-2">
                   <p>1. Disable Windows Defender/Antivirus.</p>
                   <p>2. Run loader as Administrator.</p>
                   <p>3. {isPremium ? 'Premium: PC will restart to load the driver.' : 'Basic: Inject when game is in menu.'}</p>
                 </div>
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
               
               <div className="relative pl-6">
                 <div className="absolute left-0 top-1.5 w-4 h-4 rounded-full bg-dark-bg border-2 border-gray-600"></div>
                 <div className="text-xs text-gray-500 font-mono mb-1">1 WEEK AGO</div>
                 <p className="text-gray-400 text-sm">Kernel Driver improvement. 2% CPU usage reduction.</p>
               </div>
            </div>
          </div>
        </div>

        {/* Cloud Configs Section */}
        <div className="mt-12">
           <div className="flex items-center justify-between mb-6">
             <h2 className="text-2xl font-bold text-white">Community Configs</h2>
             <span className="text-sm text-noturnal-400 font-medium cursor-pointer hover:text-white">View All &rarr;</span>
           </div>
           
           <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
             {[
               { id: 'cfg_1', name: 'Legit Pro V2', author: 'S1mpleFan', type: 'LEGIT' },
               { id: 'cfg_2', name: 'HvH Destroyer', author: 'SpinBotter', type: 'RAGE' },
               { id: 'cfg_3', name: 'Semi-Rage Pub', author: 'CasualGamer', type: 'SEMI' }
             ].map((cfg) => (
              <div key={cfg.id} className="glass p-4 rounded-xl border border-white/5 hover:border-noturnal-500/30 transition-colors group">
                 <div className="flex justify-between items-start mb-2">
                   <div className="flex items-center gap-2">
                     <FileJson className="w-5 h-5 text-gray-400" />
                     <span className="font-bold text-white">{cfg.name}</span>
                   </div>
                   <span className={`text-[10px] px-2 py-1 rounded border ${cfg.type === 'LEGIT' ? 'border-green-500/30 text-green-400' : 'border-red-500/30 text-red-400'}`}>
                     {cfg.type}
                   </span>
                 </div>
                 <p className="text-xs text-gray-500 mb-4">by {cfg.author}</p>
                 <button 
                   onClick={() => handleCopyConfig(cfg.id)}
                   className="w-full bg-white/5 hover:bg-white/10 py-2 rounded text-sm text-gray-300 flex items-center justify-center gap-2 transition-colors"
                 >
                   {copiedConfig === cfg.id ? <CheckCircle className="w-4 h-4 text-green-500" /> : <Copy className="w-4 h-4" />}
                   {copiedConfig === cfg.id ? 'Copied to Clipboard' : 'Copy Share Code'}
                 </button>
               </div>
             ))}
           </div>
        </div>

      </div>
    </div>
  );
};