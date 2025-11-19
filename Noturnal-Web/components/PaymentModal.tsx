
import React, { useState } from 'react';
import { X, CreditCard, Bitcoin, Copy, Check } from 'lucide-react';
import { PlanType } from '../types';

interface PaymentModalProps {
  isOpen: boolean;
  onClose: () => void;
  plan: PlanType;
  onConfirm: () => void;
}

export const PaymentModal: React.FC<PaymentModalProps> = ({ isOpen, onClose, plan, onConfirm }) => {
  const [method, setMethod] = useState<'card' | 'crypto'>('card');
  const [copied, setCopied] = useState(false);
  const [loading, setLoading] = useState(false);

  if (!isOpen) return null;

  const handleCopy = () => {
    navigator.clipboard.writeText('bc1qxy2kgdygjrsqtzq2n0yrf2493p83kkfjhx0wlh');
    setCopied(true);
    setTimeout(() => setCopied(false), 2000);
  };

  const handlePay = () => {
    setLoading(true);
    setTimeout(() => {
      setLoading(false);
      onConfirm();
      onClose();
    }, 2000);
  };

  return (
    <div className="fixed inset-0 z-[100] flex items-center justify-center bg-black/90 backdrop-blur-sm px-4">
      <div className="w-full max-w-md glass border border-white/10 rounded-2xl overflow-hidden">
        <div className="p-6 border-b border-white/10 flex justify-between items-center">
          <h3 className="text-xl font-bold text-white">Secure Checkout</h3>
          <button onClick={onClose}><X className="w-5 h-5 text-gray-400 hover:text-white" /></button>
        </div>
        
        <div className="flex border-b border-white/10">
          <button 
            onClick={() => setMethod('card')}
            className={`flex-1 py-4 text-sm font-medium transition-colors flex items-center justify-center gap-2 ${method === 'card' ? 'bg-white/5 text-white border-b-2 border-noturnal-500' : 'text-gray-400 hover:text-white'}`}
          >
            <CreditCard className="w-4 h-4" /> Credit Card
          </button>
          <button 
            onClick={() => setMethod('crypto')}
            className={`flex-1 py-4 text-sm font-medium transition-colors flex items-center justify-center gap-2 ${method === 'crypto' ? 'bg-white/5 text-white border-b-2 border-noturnal-500' : 'text-gray-400 hover:text-white'}`}
          >
            <Bitcoin className="w-4 h-4" /> Crypto
          </button>
        </div>

        <div className="p-6">
          <div className="mb-6">
            <p className="text-sm text-gray-400">Selected Plan</p>
            <div className="flex justify-between items-end">
              <span className="text-2xl font-bold text-white">{plan} ACCESS</span>
              <span className="text-noturnal-400 font-mono">{plan === 'BASIC' ? '$3.00' : '$8.00'}</span>
            </div>
          </div>

          {method === 'card' ? (
            <div className="space-y-4">
              <input type="text" placeholder="Card Number" className="w-full bg-black/20 border border-white/10 rounded-lg px-4 py-3 text-white outline-none focus:border-noturnal-500" />
              <div className="flex gap-4">
                <input type="text" placeholder="MM/YY" className="w-1/2 bg-black/20 border border-white/10 rounded-lg px-4 py-3 text-white outline-none focus:border-noturnal-500" />
                <input type="text" placeholder="CVC" className="w-1/2 bg-black/20 border border-white/10 rounded-lg px-4 py-3 text-white outline-none focus:border-noturnal-500" />
              </div>
              <button onClick={handlePay} disabled={loading} className="w-full bg-noturnal-600 hover:bg-noturnal-700 text-white py-3 rounded-lg font-bold transition-all disabled:opacity-50">
                {loading ? 'Processing...' : 'Pay Securely'}
              </button>
            </div>
          ) : (
            <div className="space-y-4 text-center">
              <div className="w-32 h-32 bg-white mx-auto p-2 rounded-lg">
                 {/* Fake QR */}
                 <div className="w-full h-full bg-black opacity-80 pattern-grid-lg"></div> 
              </div>
              <p className="text-xs text-gray-400">Send exactly <span className="text-white font-bold">{plan === 'BASIC' ? '0.00005' : '0.00015'} BTC</span> to:</p>
              <div className="flex gap-2">
                <input readOnly value="bc1qxy2kgdygjrsqtzq2n0yrf2493p83kkfjhx0wlh" className="w-full bg-black/20 border border-white/10 rounded-lg px-3 py-2 text-xs text-gray-400 font-mono outline-none" />
                <button onClick={handleCopy} className="bg-white/10 hover:bg-white/20 p-2 rounded-lg text-white">
                  {copied ? <Check className="w-4 h-4 text-green-400" /> : <Copy className="w-4 h-4" />}
                </button>
              </div>
              <button onClick={handlePay} disabled={loading} className="w-full bg-orange-600 hover:bg-orange-700 text-white py-3 rounded-lg font-bold transition-all mt-2">
                {loading ? 'Verifying Blockchain...' : 'I have sent payment'}
              </button>
            </div>
          )}
        </div>
      </div>
    </div>
  );
};
