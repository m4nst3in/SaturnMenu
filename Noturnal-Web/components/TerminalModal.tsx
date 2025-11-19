
import React, { useState, useEffect, useRef } from 'react';
import { X, Terminal } from 'lucide-react';

interface TerminalModalProps {
  isOpen: boolean;
  onClose: () => void;
  downloadUrl: string;
}

export const TerminalModal: React.FC<TerminalModalProps> = ({ isOpen, onClose, downloadUrl }) => {
  const [lines, setLines] = useState<string[]>([]);
  const [completed, setCompleted] = useState(false);
  const scrollRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    if (isOpen) {
      setLines([]);
      setCompleted(false);
      const sequence = [
        { text: '> Initializing secure handshake...', delay: 500 },
        { text: '> Verifying user session token...', delay: 1200 },
        { text: '> Validating subscription status... [OK]', delay: 2000 },
        { text: '> Generating unique HWID signature...', delay: 2800 },
        { text: '> Obfuscating binary (Polymorphic Engine v2)...', delay: 3800 },
        { text: '> Injecting latest offsets...', delay: 4800 },
        { text: '> Build successful.', delay: 5500 },
        { text: '> Preparing file transfer...', delay: 6000 },
        { text: 'READY', delay: 6500, action: 'DONE' }
      ];

      sequence.forEach(({ text, delay, action }) => {
        setTimeout(() => {
          setLines(prev => [...prev, text]);
          if (action === 'DONE') setCompleted(true);
        }, delay);
      });
    }
  }, [isOpen]);

  useEffect(() => {
    if (scrollRef.current) {
      scrollRef.current.scrollTop = scrollRef.current.scrollHeight;
    }
  }, [lines]);

  if (!isOpen) return null;

  return (
    <div className="fixed inset-0 z-[100] flex items-center justify-center bg-black/80 backdrop-blur-sm px-4">
      <div className="w-full max-w-2xl bg-[#0f0f13] border border-noturnal-500/30 rounded-lg shadow-[0_0_50px_rgba(139,92,246,0.2)] overflow-hidden flex flex-col h-[400px]">
        {/* Header */}
        <div className="bg-[#18181b] border-b border-white/10 px-4 py-2 flex items-center justify-between">
          <div className="flex items-center gap-2">
            <Terminal className="w-4 h-4 text-noturnal-400" />
            <span className="text-xs font-mono text-gray-400">noturnal_loader_builder.exe</span>
          </div>
          <button onClick={onClose} className="text-gray-500 hover:text-white">
            <X className="w-4 h-4" />
          </button>
        </div>

        {/* Body */}
        <div ref={scrollRef} className="flex-1 p-4 font-mono text-sm overflow-y-auto space-y-1">
          {lines.map((line, i) => (
            <div key={i} className={`${line === 'READY' ? 'text-green-400 font-bold' : 'text-noturnal-300/80'}`}>
              {line}
            </div>
          ))}
          {!completed && (
            <div className="animate-pulse text-noturnal-500">_</div>
          )}
        </div>

        {/* Footer Action */}
        {completed && (
          <div className="p-4 border-t border-white/10 bg-[#18181b] flex justify-end">
             <button 
               onClick={() => {
                 if (downloadUrl) {
                   window.open(downloadUrl, '_blank');
                 }
                 onClose();
               }}
               className="bg-green-600 hover:bg-green-500 text-white px-6 py-2 rounded font-mono text-sm font-bold animate-pulse"
             >
               [ DOWNLOAD BINARY ]
             </button>
          </div>
        )}
      </div>
    </div>
  );
};