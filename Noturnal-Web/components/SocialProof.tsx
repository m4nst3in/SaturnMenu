
import React, { useState, useEffect } from 'react';
import { ShoppingCart, UserPlus } from 'lucide-react';

const NAMES = ['DarkSoul', 'Killa', 'Ghost', 'Viper', 'Shadow', 'Neon', 'Cyber', 'Phantom', 'Reaper', 'Spectre'];
const COUNTRIES = ['🇺🇸', '🇧🇷', '🇩🇪', '🇫🇷', '🇬🇧', '🇨🇦', '🇯🇵', '🇦🇺'];

export const SocialProof: React.FC = () => {
  const [notification, setNotification] = useState<{ text: string; type: 'buy' | 'reg'; icon: React.ReactNode } | null>(null);

  useEffect(() => {
    const triggerNotification = () => {
      const isBuy = Math.random() > 0.5;
      const name = NAMES[Math.floor(Math.random() * NAMES.length)];
      const country = COUNTRIES[Math.floor(Math.random() * COUNTRIES.length)];
      
      if (isBuy) {
        setNotification({
          text: `${name} ${country} purchased Premium Plan`,
          type: 'buy',
          icon: <ShoppingCart className="w-4 h-4 text-green-400" />
        });
      } else {
        setNotification({
          text: `New user registered: ${name} ${country}`,
          type: 'reg',
          icon: <UserPlus className="w-4 h-4 text-noturnal-400" />
        });
      }

      setTimeout(() => setNotification(null), 4000);
    };

    // Initial delay
    const timeout1 = setTimeout(triggerNotification, 5000);
    
    // Interval
    const interval = setInterval(triggerNotification, 15000 + Math.random() * 10000);

    return () => {
      clearTimeout(timeout1);
      clearInterval(interval);
    };
  }, []);

  if (!notification) return null;

  return (
    <div className="fixed bottom-4 left-4 z-50 animate-fade-in-up">
      <div className="glass px-4 py-3 rounded-lg border border-white/10 flex items-center gap-3 shadow-lg shadow-black/50">
        <div className="w-8 h-8 rounded-full bg-white/5 flex items-center justify-center shrink-0">
          {notification.icon}
        </div>
        <div>
          <p className="text-xs font-bold text-white">{notification.type === 'buy' ? 'New Purchase' : 'New Member'}</p>
          <p className="text-xs text-gray-400">{notification.text}</p>
        </div>
      </div>
    </div>
  );
};
