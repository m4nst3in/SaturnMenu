import React, { useEffect, useState } from 'react';

const LOGS = [
  "INITIALIZING_DRIVER_V2...",
  "MAPPING_MEMORY_REGIONS...",
  "BYPASSING_VAC_LIVE_MODULE...",
  "HOOKING_DIRECTX...",
  "RESOLVING_POINTER_CHAINS...",
  "DECRYPTING_OFFSET_TABLE...",
  "SECURE_CONNECTION_ESTABLISHED",
  "USER_492: INJECTED_SUCCESSFULLY",
  "USER_103: INJECTED_SUCCESSFULLY",
  "USER_882: CONFIG_LOADED",
  "SYSTEM: POLYMORPHIC_BUILD_GEN",
  "WATCHDOG: SLEEPING...",
  "RING0: PRIVILEGES_GRANTED",
  "OBFUSCATION: ACTIVE",
  "HEARTBEAT: SENT",
  "USER_912: PURCHASED_PREMIUM",
  "NET_GU: PACKET_ENCRYPTED",
];

export const MatrixSidebar: React.FC = () => {
  const [logs, setLogs] = useState<string[]>([]);

  useEffect(() => {
    const interval = setInterval(() => {
      const newLog = LOGS[Math.floor(Math.random() * LOGS.length)];
      const timestamp = new Date().toLocaleTimeString('en-US', { hour12: false, hour: '2-digit', minute:'2-digit', second:'2-digit' });
      const logEntry = `[${timestamp}] ${newLog} ${Math.random() > 0.8 ? '[OK]' : ''}`;
      
      setLogs(prev => {
        const updated = [...prev, logEntry];
        if (updated.length > 20) updated.shift();
        return updated;
      });
    }, 400);

    return () => clearInterval(interval);
  }, []);

  return (
    <div className="fixed left-0 top-0 bottom-0 w-64 pointer-events-none hidden 2xl:flex flex-col justify-end pb-8 pl-6 z-0 opacity-20 overflow-hidden select-none">
      <div className="space-y-1 font-mono text-[10px] text-noturnal-400">
        {logs.map((log, i) => (
          <div key={i} className="animate-fade-in" style={{ opacity: (i / logs.length) }}>
            {log}
          </div>
        ))}
        <div className="animate-pulse text-noturnal-500">_</div>
      </div>
    </div>
  );
};