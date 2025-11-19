import React, { useState } from 'react';
import { PLANS } from '../constants';
import { Check, X, Sparkles, Crown } from 'lucide-react';
import { useAuth } from '../context/AuthContext';
import { useNavigate } from 'react-router-dom';
import { PlanType } from '../types';
import { PaymentModal } from '../components/PaymentModal';
import { TextScramble } from '../components/TextScramble';

export const Pricing: React.FC = () => {
  const { user, upgradePlan } = useAuth();
  const navigate = useNavigate();
  const [selectedPlan, setSelectedPlan] = useState<{ id: string, type: PlanType } | null>(null);

  const handlePurchaseClick = (planId: string, planType: PlanType) => {
    if (!user) {
      navigate('/auth');
      return;
    }
    setSelectedPlan({ id: planId, type: planType });
  };

  const handlePaymentConfirm = async () => {
    if (selectedPlan && user) {
      try {
        await upgradePlan(selectedPlan.type);
        navigate('/dashboard');
      } catch (e) {
        console.error(e);
      } finally {
        setSelectedPlan(null);
      }
    }
  };

  return (
    <div className="pt-40 pb-32 min-h-screen bg-dark-bg relative overflow-hidden">
      <PaymentModal 
        isOpen={!!selectedPlan}
        plan={selectedPlan?.type || PlanType.BASIC}
        onClose={() => setSelectedPlan(null)}
        onConfirm={handlePaymentConfirm}
      />

      {/* Ambient Glows */}
      <div className="absolute top-0 left-1/2 -translate-x-1/2 w-[1000px] h-[600px] bg-noturnal-900/20 rounded-full blur-[120px] pointer-events-none"></div>
      <div className="absolute bottom-0 right-0 w-[800px] h-[600px] bg-cyan-900/10 rounded-full blur-[120px] pointer-events-none"></div>

      <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 relative z-10">
        <div className="text-center mb-24">
           <div className="inline-block mb-6 px-4 py-1.5 rounded-full border border-noturnal-500/30 bg-noturnal-500/10 backdrop-blur-md">
             <span className="text-noturnal-300 font-mono text-xs tracking-[0.2em] uppercase font-bold">Access Granted</span>
           </div>
          <h2 className="text-6xl md:text-8xl font-black text-white mb-8 tracking-tighter">
            Choose Your <span className="text-transparent bg-clip-text bg-gradient-to-r from-noturnal-400 to-cyan-400"><TextScramble text="ARSENAL" speed={60} /></span>
          </h2>
          <p className="text-gray-400 max-w-xl mx-auto text-lg font-light leading-relaxed">
            Instant delivery via secure crypto or card. All plans include 24/7 live support and HWID spoofer protection.
          </p>
        </div>

        <div className="grid grid-cols-1 md:grid-cols-2 gap-10 max-w-5xl mx-auto items-stretch">
          {PLANS.map((plan, index) => (
            <div 
              key={plan.id}
              className="group relative"
              style={{ animationDelay: `${index * 150}ms` }}
            >
              {/* Moving Border Beam for Premium (Idea #3) */}
              {plan.recommended && (
                 <div className="border-beam"></div>
              )}
              
              {/* Holographic Back Glow */}
              {plan.recommended && (
                <div className="absolute -inset-1 bg-gradient-to-b from-noturnal-500 via-cyan-500 to-noturnal-500 rounded-[2.1rem] opacity-20 blur-xl group-hover:opacity-40 transition duration-1000"></div>
              )}

              <div 
                className={`relative h-full flex flex-col rounded-[2rem] overflow-hidden transition-all duration-500 backdrop-blur-xl
                  ${plan.recommended 
                    ? 'bg-[#0c0b14] border-0' 
                    : 'bg-[#0a0a0c]/80 border border-white/10 hover:border-white/20'
                  }`}
              >
                 {/* Shimmer Effect */}
                 <div className="absolute inset-0 -translate-x-full group-hover:animate-shimmer bg-gradient-to-r from-transparent via-white/5 to-transparent z-20 pointer-events-none"></div>

                 {/* Header */}
                 <div className="p-10 pb-0 relative z-10">
                    <div className="flex justify-between items-start mb-6">
                       <div className="p-3 rounded-2xl bg-white/5 border border-white/10">
                         {plan.recommended ? <Crown className="w-6 h-6 text-noturnal-400" /> : <Sparkles className="w-6 h-6 text-gray-400" />}
                       </div>
                       {plan.recommended && (
                         <div className="px-3 py-1 rounded-full bg-gradient-to-r from-noturnal-600 to-noturnal-500 text-white text-[10px] font-bold uppercase tracking-widest shadow-lg shadow-noturnal-500/30">
                           Recommended
                         </div>
                       )}
                    </div>
                    
                    <h3 className="text-3xl font-bold text-white mb-2">{plan.name}</h3>
                    <p className="text-gray-400 text-sm mb-6 h-10">{plan.driver === 'Kernel Mode' ? 'Full Kernel-Level access (Ring 0) with maximum security.' : 'Standard external access for casual play.'}</p>
                    
                    <div className="flex items-baseline gap-1 pb-8 border-b border-white/5">
                        <span className="text-6xl font-black text-white tracking-tighter">${plan.price}</span>
                        <span className="text-sm text-gray-500 font-mono">/MONTH</span>
                    </div>
                 </div>

                 {/* Features */}
                 <div className="p-10 pt-8 flex-1 relative z-10">
                   <ul className="space-y-5 mb-10">
                     {plan.features.map((feat, idx) => (
                       <li key={idx} className="flex items-start gap-4 group/item">
                         <div className={`mt-0.5 w-5 h-5 rounded-full flex items-center justify-center shrink-0 transition-colors ${plan.recommended ? 'bg-noturnal-500 text-white shadow-[0_0_10px_rgba(118,69,253,0.4)]' : 'bg-white/10 text-gray-400 group-hover/item:bg-white/20'}`}>
                           <Check className="w-3 h-3" />
                         </div>
                         <span className="text-gray-300 text-sm font-medium group-hover/item:text-white transition-colors">{feat}</span>
                       </li>
                     ))}
                     {!plan.recommended && (
                        <li className="flex items-start gap-4 opacity-40 grayscale">
                           <div className="mt-0.5 w-5 h-5 rounded-full bg-white/5 flex items-center justify-center shrink-0">
                             <X className="w-3 h-3" />
                           </div>
                           <span className="text-gray-500 text-sm">Kernel Mode Driver (Ring 0)</span>
                        </li>
                      )}
                   </ul>
                 </div>

                 {/* Action Button - Liquid Neon for Recommended (Idea #7) */}
                 <div className="p-10 pt-0 relative z-10 mt-auto">
                   <button
                     onClick={() => handlePurchaseClick(plan.id, plan.id === 'basic' ? PlanType.BASIC : PlanType.PREMIUM)}
                     className={`w-full py-5 rounded-xl font-bold text-sm uppercase tracking-widest transition-all duration-300 transform hover:-translate-y-1
                       ${plan.recommended 
                         ? 'btn-liquid text-white' 
                         : 'bg-white/5 text-white border border-white/10 hover:bg-white/10'
                       }`}
                   >
                     {plan.recommended ? 'Get Premium Access' : 'Get Basic Access'}
                   </button>
                 </div>

              </div>
            </div>
          ))}
        </div>

        {/* Trust Indicators */}
        <div className="mt-24 grid grid-cols-2 md:grid-cols-4 gap-8 text-center opacity-60">
           <div><h4 className="text-white font-bold text-xl mb-1">24/7</h4><p className="text-xs text-gray-500 uppercase tracking-widest">Support</p></div>
           <div><h4 className="text-white font-bold text-xl mb-1">Instant</h4><p className="text-xs text-gray-500 uppercase tracking-widest">Delivery</p></div>
           <div><h4 className="text-white font-bold text-xl mb-1">Secure</h4><p className="text-xs text-gray-500 uppercase tracking-widest">Payments</p></div>
           <div><h4 className="text-white font-bold text-xl mb-1">15k+</h4><p className="text-xs text-gray-500 uppercase tracking-widest">Users</p></div>
        </div>
      </div>
    </div>
  );
};