import React from 'react';
import { Shield, Zap, Cpu, Ghost, Target, Eye } from 'lucide-react';

export const PLANS = [
  {
    id: 'basic',
    name: 'Basic',
    price: 3.00,
    features: [
      'Legit Aimbot',
      'Visuals (ESP/Chams)',
      'Skin Changer',
      'Cloud Configs',
      'External Access'
    ],
    driver: 'User Mode',
    recommended: false
  },
  {
    id: 'premium',
    name: 'Premium',
    price: 8.00,
    features: [
      'Rage & Legit Aimbot',
      'Advanced Visuals',
      'Inventory Changer',
      'Kernel Mode Driver (Ring 0)',
      'HWID Spoofer included',
      'Stream Proof'
    ],
    driver: 'Kernel Mode',
    recommended: true
  }
];

export const FEATURES = [
  {
    title: "Kernel Level Security",
    desc: "Our driver operates at Ring 0, ensuring total invisibility against VAC and VAC Live.",
    icon: <Shield className="w-6 h-6 text-noturnal-400" />
  },
  {
    title: "High Performance",
    desc: "Optimized to not affect your FPS. Play smoothly while you dominate.",
    icon: <Zap className="w-6 h-6 text-noturnal-400" />
  },
  {
    title: "Humanized Aimbot",
    desc: "Advanced smoothing algorithms that mimic professional human movement.",
    icon: <Target className="w-6 h-6 text-noturnal-400" />
  },
  {
    title: "Customizable Visuals",
    desc: "ESP Glow, Chams, 2D Radar, and much more, fully configurable.",
    icon: <Eye className="w-6 h-6 text-noturnal-400" />
  }
];