
import React from 'react';
import { Github, Twitter, Disc } from 'lucide-react';

export const Footer: React.FC = () => {
  return (
    <footer className="border-t border-white/5 bg-dark-bg pt-16 pb-8">
      <div className="max-w-7xl mx-auto px-4">
        <div className="grid grid-cols-1 md:grid-cols-4 gap-12 mb-12">
          <div className="space-y-4">
            <h3 className="font-bold text-white text-xl tracking-wider">NOTURNAL</h3>
            <p className="text-gray-500 text-sm leading-relaxed">
              Premium cheats for selected games. Stealth, Undetectable, Performance.
            </p>
            <div className="flex gap-4">
              <a href="#" className="text-gray-400 hover:text-white transition-colors"><Twitter className="w-5 h-5" /></a>
              <a href="#" className="text-gray-400 hover:text-white transition-colors"><Github className="w-5 h-5" /></a>
              <a href="#" className="text-gray-400 hover:text-noturnal-400 transition-colors"><Disc className="w-5 h-5" /></a>
            </div>
          </div>
          
          <div>
            <h4 className="font-bold text-white mb-4">Product</h4>
            <ul className="space-y-2 text-sm text-gray-500">
              <li><a href="#" className="hover:text-noturnal-400 transition-colors">Features</a></li>
              <li><a href="#" className="hover:text-noturnal-400 transition-colors">Pricing</a></li>
              <li><a href="#" className="hover:text-noturnal-400 transition-colors">Changelog</a></li>
              <li><a href="#" className="hover:text-noturnal-400 transition-colors">Status</a></li>
            </ul>
          </div>

          <div>
            <h4 className="font-bold text-white mb-4">Support</h4>
            <ul className="space-y-2 text-sm text-gray-500">
              <li><a href="#" className="hover:text-noturnal-400 transition-colors">Documentation</a></li>
              <li><a href="#" className="hover:text-noturnal-400 transition-colors">Contact Us</a></li>
              <li><a href="#" className="hover:text-noturnal-400 transition-colors">Reseller Program</a></li>
              <li><a href="#" className="hover:text-noturnal-400 transition-colors">Report Bug</a></li>
            </ul>
          </div>

          <div>
            <h4 className="font-bold text-white mb-4">Live Stats</h4>
            <div className="space-y-3">
               <div className="flex justify-between text-sm">
                 <span className="text-gray-500">Active Users</span>
                 <span className="text-green-400 font-mono font-bold">15,402</span>
               </div>
               <div className="flex justify-between text-sm">
                 <span className="text-gray-500">Supported Build</span>
                 <span className="text-white font-mono">v1.39.8.2</span>
               </div>
               <div className="flex justify-between text-sm">
                 <span className="text-gray-500">Uptime</span>
                 <span className="text-noturnal-400 font-mono">99.6%</span>
               </div>
            </div>
          </div>
        </div>

        <div className="pt-8 border-t border-white/5 flex flex-col md:flex-row justify-between items-center text-xs text-gray-600">
          <div>&copy; {new Date().getFullYear()} Noturnal Menu. All rights reserved.</div>
          <div className="flex gap-6 mt-4 md:mt-0">
             <a href="#" className="hover:text-gray-400">Privacy Policy</a>
             <a href="#" className="hover:text-gray-400">Terms of Service</a>
             <a href="#" className="hover:text-gray-400">Refund Policy</a>
          </div>
        </div>
      </div>
    </footer>
  );
};
