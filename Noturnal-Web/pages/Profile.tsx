import React from 'react';
import { useAuth } from '../context/AuthContext';
import { User as UserIcon, Mail, Shield, Cpu, Hash, Award, Star, Zap } from 'lucide-react';

export const Profile: React.FC = () => {
  const { user } = useAuth();

  if (!user) return null;

  const formatDate = (dateString: string | null) => {
    if (!dateString) return 'Never';
    return new Date(dateString).toLocaleDateString('en-US', {
      year: 'numeric',
      month: 'long',
      day: 'numeric'
    });
  };

  // Calculate XP progress percentage
  const xpProgress = user.gamification 
    ? (user.gamification.xp / user.gamification.nextLevelXp) * 100 
    : 0;

  return (
    <div className="pt-32 pb-24 min-h-screen bg-dark-bg relative overflow-hidden">
      {/* Background effects */}
      <div className="absolute inset-0 z-0 pointer-events-none bg-grid-white bg-grid-pattern opacity-10"></div>
      <div className="absolute top-20 left-1/2 -translate-x-1/2 w-[600px] h-[600px] bg-noturnal-900/20 rounded-full blur-[120px] pointer-events-none"></div>

      <div className="max-w-4xl mx-auto px-4 relative z-10">
        <h1 className="text-4xl font-bold text-white mb-8">User Profile</h1>

        <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
          {/* Main Profile Card */}
          <div className="md:col-span-3 glass p-8 rounded-2xl border border-white/5 flex flex-col sm:flex-row items-center gap-8">
            <div className="relative">
               <div className="w-24 h-24 rounded-full bg-gradient-to-br from-noturnal-400 to-noturnal-700 flex items-center justify-center shadow-[0_0_30px_rgba(139,92,246,0.3)] shrink-0">
                  <UserIcon className="w-12 h-12 text-white" />
               </div>
               {user.gamification && (
                 <div className="absolute -bottom-2 -right-2 bg-[#0a0a0c] border border-noturnal-500 text-noturnal-400 text-xs font-bold w-8 h-8 rounded-full flex items-center justify-center">
                    {user.gamification.level}
                 </div>
               )}
            </div>
            
            <div className="text-center sm:text-left flex-1 w-full">
              <h2 className="text-3xl font-bold text-white flex items-center justify-center sm:justify-start gap-2">
                 {user.username}
                 {user.gamification?.rank && (
                    <span className="px-2 py-0.5 rounded bg-yellow-500/10 border border-yellow-500/20 text-yellow-400 text-[10px] uppercase tracking-wider font-bold">
                       {user.gamification.rank}
                    </span>
                 )}
              </h2>
              <div className="flex items-center justify-center sm:justify-start gap-2 mt-2 text-gray-400">
                <Mail className="w-4 h-4" />
                <span>{user.email}</span>
              </div>
              
              {/* XP Bar */}
              {user.gamification && (
                <div className="mt-4 max-w-md">
                   <div className="flex justify-between text-xs text-gray-500 mb-1">
                      <span>XP Progress</span>
                      <span>{user.gamification.xp} / {user.gamification.nextLevelXp} XP</span>
                   </div>
                   <div className="h-2 bg-black/50 rounded-full overflow-hidden border border-white/5">
                      <div style={{ width: `${xpProgress}%` }} className="h-full bg-gradient-to-r from-noturnal-600 to-cyan-500 shadow-[0_0_10px_#7645fd]"></div>
                   </div>
                </div>
              )}
            </div>

            <div className="mt-4 sm:mt-0">
               <span className={`px-4 py-2 rounded-full text-sm font-bold border ${user.subscription.active ? 'bg-green-500/10 text-green-400 border-green-500/20' : 'bg-red-500/10 text-red-400 border-red-500/20'}`}>
                 {user.subscription.active ? 'Active Subscription' : 'Inactive'}
               </span>
            </div>
          </div>
          
          {/* Gamification Badges (Feature 7) */}
          <div className="md:col-span-3 glass p-6 rounded-2xl border border-white/5">
             <h3 className="text-lg font-bold text-white mb-4 flex items-center gap-2">
               <Award className="w-5 h-5 text-yellow-500" />
               Achievements & Badges
             </h3>
             <div className="flex flex-wrap gap-4">
                {user.gamification?.badges.map((badge, i) => (
                   <div key={i} className="flex items-center gap-2 bg-white/5 border border-white/10 px-4 py-2 rounded-xl hover:bg-white/10 transition-colors">
                      <Star className="w-4 h-4 text-yellow-400 fill-yellow-400" />
                      <span className="text-sm text-gray-200 font-bold">{badge}</span>
                   </div>
                ))}
                {(!user.gamification?.badges || user.gamification.badges.length === 0) && (
                   <div className="text-gray-500 text-sm italic">No badges earned yet. Complete tasks to rank up.</div>
                )}
             </div>
          </div>

          {/* Subscription Details */}
          <div className="md:col-span-2 glass p-6 rounded-2xl border border-white/5">
             <h3 className="text-lg font-bold text-white mb-6 flex items-center gap-2">
               <Shield className="w-5 h-5 text-noturnal-400" />
               Subscription Details
             </h3>
             
             <div className="space-y-6">
                <div className="flex justify-between items-center border-b border-white/5 pb-4">
                   <span className="text-gray-400">Current Plan</span>
                   <span className="text-white font-bold text-lg">{user.subscription.plan || 'None'}</span>
                </div>
                <div className="flex justify-between items-center border-b border-white/5 pb-4">
                   <span className="text-gray-400">Status</span>
                   <span className="text-white">{user.subscription.active ? 'Active' : 'Expired'}</span>
                </div>
                <div className="flex justify-between items-center">
                   <span className="text-gray-400">Expires At</span>
                   <span className="text-white font-mono">{formatDate(user.subscription.expiresAt)}</span>
                </div>
             </div>
          </div>

          {/* Security / HWID */}
          <div className="glass p-6 rounded-2xl border border-white/5">
             <h3 className="text-lg font-bold text-white mb-6 flex items-center gap-2">
               <Cpu className="w-5 h-5 text-noturnal-400" />
               Hardware ID
             </h3>
             <div className="bg-black/30 p-4 rounded-lg border border-white/5 mb-4">
                <code className="text-xs text-gray-400 break-all block">
                  {user.hwid || 'NO HWID LOCKED'}
                </code>
             </div>
             <p className="text-xs text-gray-500 leading-relaxed">
               Your subscription is locked to this hardware ID. If you change PC parts, you can reset this in the Dashboard.
             </p>
          </div>
        </div>
      </div>
    </div>
  );
};