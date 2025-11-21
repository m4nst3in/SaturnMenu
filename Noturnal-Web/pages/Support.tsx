
import React, { useState, useEffect, useRef } from 'react';
import { useAuth } from '../context/AuthContext';
import { api } from '../services/api';
import { Ticket, TicketMessage, TicketCategory, TicketPriority } from '../types';
import { 
  MessageSquare, 
  LifeBuoy, 
  AlertTriangle, 
  Send, 
  Paperclip, 
  Plus, 
  Clock, 
  CheckCircle, 
  X, 
  ChevronDown, 
  ChevronUp, 
  Terminal, 
  Cpu, 
  Activity, 
  Lock, 
  ShieldCheck 
} from 'lucide-react';
import { Link } from 'react-router-dom';

const FAQS = [
  { q: "My loader closes instantly upon opening.", a: "This is usually caused by Windows Defender or other AVs. Please disable Real-time Protection and add the folder to exclusions." },
  { q: "How do I renew my subscription?", a: "Go to the Pricing page, select your plan, and checkout again. The time will be added to your existing subscription." },
  { q: "Is this compatible with Windows 11?", a: "Yes, Noturnal is fully compatible with Windows 10 (20H2+) and Windows 11 (all versions)." },
  { q: "I got a BSOD (Blue Screen).", a: "Ensure Virtualization is enabled in BIOS (VT-x / SVM) and that Secure Boot is disabled if you are on Premium." }
];

export const Support: React.FC = () => {
  const { user } = useAuth();
  const [activeTab, setActiveTab] = useState<'TICKETS' | 'FAQ' | 'STATUS'>('TICKETS');
  
  // Ticket State
  const [tickets, setTickets] = useState<Ticket[]>([]);
  const [selectedTicket, setSelectedTicket] = useState<Ticket | null>(null);
  const [isLoading, setIsLoading] = useState(false);
  const [messageInput, setMessageInput] = useState('');
  
  // Create Ticket Modal
  const [showCreateModal, setShowCreateModal] = useState(false);
  const [newSubject, setNewSubject] = useState('');
  const [newCategory, setNewCategory] = useState<TicketCategory>('TECHNICAL');
  const [newPriority, setNewPriority] = useState<TicketPriority>('MEDIUM');
  const [newInitialMessage, setNewInitialMessage] = useState('');

  // FAQ State
  const [openFaqIndex, setOpenFaqIndex] = useState<number | null>(null);

  const messagesEndRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    if (user) {
      loadTickets();
    }
  }, [user]);

  useEffect(() => {
    scrollToBottom();
  }, [selectedTicket?.messages]);

  const scrollToBottom = () => {
    messagesEndRef.current?.scrollIntoView({ behavior: "smooth" });
  };

  const loadTickets = async () => {
    if (!user) return;
    setIsLoading(true);
    const data = await api.getTickets(user.id);
    setTickets(data);
    setIsLoading(false);
  };

  const handleCreateTicket = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!user) return;
    
    const ticket = await api.createTicket(user.id, newSubject, newCategory, newPriority, newInitialMessage);
    setTickets([ticket, ...tickets]);
    setShowCreateModal(false);
    setSelectedTicket(ticket);
    // Reset form
    setNewSubject('');
    setNewInitialMessage('');
  };

  const handleSendMessage = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!selectedTicket || !messageInput.trim()) return;

    // Optimistic UI update
    const tempMsg: TicketMessage = {
        id: 'temp',
        sender: 'user',
        text: messageInput,
        timestamp: new Date().toISOString()
    };
    
    const updatedTicket = { ...selectedTicket, messages: [...selectedTicket.messages, tempMsg] };
    setSelectedTicket(updatedTicket);
    setMessageInput('');

    // Actual API call
    await api.sendMessage(selectedTicket.id, tempMsg.text, 'user');
    
    // Refresh to get valid ID and potentially admin response simulation
    const refreshedTickets = await api.getTickets(user!.id);
    setTickets(refreshedTickets);
    const refreshedSelected = refreshedTickets.find(t => t.id === selectedTicket.id);
    if (refreshedSelected) setSelectedTicket(refreshedSelected);
  };

  const getStatusColor = (status: string) => {
    switch(status) {
        case 'OPEN': return 'text-green-400 bg-green-500/10 border-green-500/30';
        case 'ANSWERED': return 'text-yellow-400 bg-yellow-500/10 border-yellow-500/30';
        case 'CLOSED': return 'text-gray-400 bg-gray-500/10 border-gray-500/30';
        default: return 'text-white';
    }
  };

  return (
    <div className="min-h-screen bg-dark-bg pt-24 pb-12 px-4">
      
      {/* Creation Modal */}
      {showCreateModal && (
        <div className="fixed inset-0 z-[100] flex items-center justify-center bg-black/80 backdrop-blur-sm px-4">
          <div className="w-full max-w-lg bg-[#0f0f13] border border-white/10 rounded-2xl p-8 relative animate-fade-in-up">
            <button onClick={() => setShowCreateModal(false)} className="absolute top-4 right-4 text-gray-500 hover:text-white">
              <X className="w-5 h-5" />
            </button>
            <h2 className="text-2xl font-bold text-white mb-6 flex items-center gap-2">
                <Terminal className="w-6 h-6 text-noturnal-400" /> Open Secure Channel
            </h2>
            <form onSubmit={handleCreateTicket} className="space-y-4">
              <div>
                <label className="block text-xs font-bold text-gray-500 uppercase mb-1">Subject</label>
                <input required type="text" value={newSubject} onChange={e => setNewSubject(e.target.value)} className="w-full bg-black/30 border border-white/10 rounded-lg p-3 text-white focus:border-noturnal-500 outline-none" placeholder="Brief description of the issue" />
              </div>
              <div className="grid grid-cols-2 gap-4">
                <div>
                  <label className="block text-xs font-bold text-gray-500 uppercase mb-1">Category</label>
                  <select value={newCategory} onChange={e => setNewCategory(e.target.value as any)} className="w-full bg-black/30 border border-white/10 rounded-lg p-3 text-white focus:border-noturnal-500 outline-none">
                    <option value="TECHNICAL">Technical Issue</option>
                    <option value="BILLING">Billing / Payments</option>
                    <option value="BUG_REPORT">Bug Report</option>
                    <option value="GENERAL">General Question</option>
                  </select>
                </div>
                <div>
                  <label className="block text-xs font-bold text-gray-500 uppercase mb-1">Priority</label>
                  <select value={newPriority} onChange={e => setNewPriority(e.target.value as any)} className="w-full bg-black/30 border border-white/10 rounded-lg p-3 text-white focus:border-noturnal-500 outline-none">
                    <option value="LOW">Low</option>
                    <option value="MEDIUM">Medium</option>
                    <option value="HIGH">High</option>
                    <option value="CRITICAL">Critical</option>
                  </select>
                </div>
              </div>
              <div>
                <label className="block text-xs font-bold text-gray-500 uppercase mb-1">Initial Message</label>
                <textarea required value={newInitialMessage} onChange={e => setNewInitialMessage(e.target.value)} className="w-full bg-black/30 border border-white/10 rounded-lg p-3 text-white focus:border-noturnal-500 outline-none h-32 resize-none" placeholder="Provide detailed steps to reproduce or explain your problem..." />
              </div>
              <button type="submit" className="w-full bg-noturnal-600 hover:bg-noturnal-500 text-white py-3 rounded-lg font-bold flex items-center justify-center gap-2">
                <Send className="w-4 h-4" /> Transmit Ticket
              </button>
            </form>
          </div>
        </div>
      )}

      <div className="max-w-7xl mx-auto">
        
        {/* Header */}
        <div className="flex flex-col md:flex-row justify-between items-end mb-8 gap-6">
           <div>
             <div className="inline-flex items-center gap-2 px-4 py-1.5 rounded-full border border-white/10 bg-white/5 backdrop-blur-md mb-4">
               <LifeBuoy className="w-4 h-4 text-noturnal-400" />
               <span className="text-xs font-mono font-bold text-gray-300 tracking-widest uppercase">Support Center</span>
             </div>
             <h1 className="text-4xl md:text-6xl font-black text-white tracking-tighter">
               System <span className="text-transparent bg-clip-text bg-gradient-to-r from-noturnal-400 to-cyan-400">Status & Help</span>
             </h1>
           </div>

           {/* Status Indicators */}
           <div className="flex gap-4 bg-[#0a0a0c] p-4 rounded-xl border border-white/5">
              <div className="flex items-center gap-2">
                 <div className="w-2 h-2 rounded-full bg-green-500 animate-pulse"></div>
                 <div className="flex flex-col">
                   <span className="text-[10px] text-gray-500 font-bold uppercase">Auth Server</span>
                   <span className="text-xs text-white font-mono">ONLINE</span>
                 </div>
              </div>
              <div className="w-px h-8 bg-white/10"></div>
              <div className="flex items-center gap-2">
                 <div className="w-2 h-2 rounded-full bg-green-500 animate-pulse"></div>
                 <div className="flex flex-col">
                   <span className="text-[10px] text-gray-500 font-bold uppercase">Injection</span>
                   <span className="text-xs text-white font-mono">OPERATIONAL</span>
                 </div>
              </div>
              <div className="w-px h-8 bg-white/10"></div>
              <div className="flex items-center gap-2">
                 <div className="w-2 h-2 rounded-full bg-yellow-500"></div>
                 <div className="flex flex-col">
                   <span className="text-[10px] text-gray-500 font-bold uppercase">Support Agents</span>
                   <span className="text-xs text-white font-mono">HIGH LOAD</span>
                 </div>
              </div>
           </div>
        </div>

        {/* Main Layout */}
        <div className="grid grid-cols-1 lg:grid-cols-12 gap-6 h-[700px]">
          
          {/* Left Sidebar (Navigation & List) */}
          <div className="lg:col-span-4 flex flex-col gap-4 h-full">
             
             {/* Tabs */}
             <div className="flex bg-[#0c0b14] p-1 rounded-xl border border-white/5">
               <button onClick={() => setActiveTab('TICKETS')} className={`flex-1 py-2 text-sm font-bold rounded-lg transition-all ${activeTab === 'TICKETS' ? 'bg-white/10 text-white' : 'text-gray-500 hover:text-gray-300'}`}>Tickets</button>
               <button onClick={() => setActiveTab('FAQ')} className={`flex-1 py-2 text-sm font-bold rounded-lg transition-all ${activeTab === 'FAQ' ? 'bg-white/10 text-white' : 'text-gray-500 hover:text-gray-300'}`}>FAQ</button>
             </div>

             {/* Ticket List Content */}
             {activeTab === 'TICKETS' && (
               <div className="flex-1 bg-glass border border-white/5 rounded-2xl overflow-hidden flex flex-col bg-[#0a0a0c]">
                 <div className="p-4 border-b border-white/5 flex justify-between items-center">
                   <span className="text-xs font-bold text-gray-400 uppercase tracking-widest">Active Tickets</span>
                   <button onClick={() => setShowCreateModal(true)} className="bg-noturnal-600 hover:bg-noturnal-500 text-white p-2 rounded-lg transition-colors">
                     <Plus className="w-4 h-4" />
                   </button>
                 </div>
                 
                 <div className="flex-1 overflow-y-auto p-2 space-y-2 custom-scrollbar">
                   {!user ? (
                     <div className="text-center p-8 text-gray-500 text-sm">
                       Please <Link to="/auth" className="text-noturnal-400 underline">login</Link> to view tickets.
                     </div>
                   ) : tickets.length === 0 ? (
                     <div className="text-center p-8 text-gray-500 text-sm flex flex-col items-center gap-2">
                       <MessageSquare className="w-8 h-8 opacity-50" />
                       No active tickets. Start a new transmission.
                     </div>
                   ) : (
                     tickets.map(ticket => (
                       <div 
                         key={ticket.id} 
                         onClick={() => setSelectedTicket(ticket)}
                         className={`p-4 rounded-xl border cursor-pointer transition-all hover:bg-white/5 ${selectedTicket?.id === ticket.id ? 'bg-white/5 border-noturnal-500/50' : 'bg-transparent border-white/5'}`}
                       >
                         <div className="flex justify-between items-start mb-2">
                           <div className={`px-2 py-0.5 rounded text-[10px] font-bold border uppercase ${getStatusColor(ticket.status)}`}>
                             {ticket.status}
                           </div>
                           <span className="text-[10px] text-gray-500 font-mono">{new Date(ticket.lastUpdated).toLocaleDateString()}</span>
                         </div>
                         <h4 className="font-bold text-white text-sm truncate mb-1">{ticket.subject}</h4>
                         <p className="text-xs text-gray-500 truncate">{ticket.messages[ticket.messages.length - 1].text}</p>
                       </div>
                     ))
                   )}
                 </div>
               </div>
             )}

             {/* FAQ Content */}
             {activeTab === 'FAQ' && (
               <div className="flex-1 bg-glass border border-white/5 rounded-2xl overflow-hidden bg-[#0a0a0c] p-4 overflow-y-auto">
                 <div className="space-y-4">
                   {FAQS.map((item, idx) => (
                     <div key={idx} className="border border-white/5 rounded-xl overflow-hidden bg-white/5">
                       <button 
                         onClick={() => setOpenFaqIndex(openFaqIndex === idx ? null : idx)}
                         className="w-full flex justify-between items-center p-4 text-left text-sm font-bold text-gray-200 hover:text-white transition-colors"
                       >
                         {item.q}
                         {openFaqIndex === idx ? <ChevronUp className="w-4 h-4" /> : <ChevronDown className="w-4 h-4" />}
                       </button>
                       {openFaqIndex === idx && (
                         <div className="px-4 pb-4 text-xs text-gray-400 leading-relaxed border-t border-white/5 pt-4 bg-black/20">
                           {item.a}
                         </div>
                       )}
                     </div>
                   ))}
                 </div>
               </div>
             )}
          </div>

          {/* Right Content (Chat Window) */}
          <div className="lg:col-span-8 bg-[#0c0b14] border border-white/5 rounded-2xl overflow-hidden flex flex-col relative">
            {/* Grid Background */}
            <div className="absolute inset-0 bg-grid-white opacity-[0.02] pointer-events-none"></div>
            
            {selectedTicket ? (
              <>
                {/* Chat Header */}
                <div className="p-4 border-b border-white/5 bg-[#121216] flex justify-between items-center relative z-10">
                  <div>
                    <div className="flex items-center gap-3 mb-1">
                       <h3 className="text-lg font-bold text-white">{selectedTicket.subject}</h3>
                       <span className="text-xs font-mono text-gray-500">#{selectedTicket.id}</span>
                    </div>
                    <div className="flex gap-2 text-[10px] uppercase font-bold text-gray-400">
                       <span>{selectedTicket.category}</span>
                       <span className="text-noturnal-400">•</span>
                       <span>{selectedTicket.priority} Priority</span>
                    </div>
                  </div>
                  <div className="flex gap-2">
                    <button className="p-2 hover:bg-white/5 rounded-lg text-gray-400 hover:text-white transition-colors" title="Close Ticket">
                        <CheckCircle className="w-5 h-5" />
                    </button>
                  </div>
                </div>

                {/* Chat Messages */}
                <div className="flex-1 overflow-y-auto p-6 space-y-6 relative z-10">
                   {selectedTicket.messages.map((msg) => (
                     <div key={msg.id} className={`flex ${msg.sender === 'user' ? 'justify-end' : 'justify-start'}`}>
                        <div className={`max-w-[80%] ${msg.sender === 'user' ? 'order-1' : 'order-2'}`}>
                           <div className={`flex items-end gap-2 mb-1 ${msg.sender === 'user' ? 'flex-row-reverse' : 'flex-row'}`}>
                              <span className={`text-xs font-bold ${msg.sender === 'user' ? 'text-noturnal-400' : msg.sender === 'system' ? 'text-blue-400' : 'text-green-400'}`}>
                                {msg.sender === 'user' ? user?.username : msg.sender === 'system' ? 'SYSTEM' : 'SUPPORT AGENT'}
                              </span>
                              <span className="text-[10px] text-gray-600 font-mono">{new Date(msg.timestamp).toLocaleTimeString()}</span>
                           </div>
                           <div className={`p-4 rounded-2xl text-sm leading-relaxed border ${
                             msg.sender === 'user' 
                               ? 'bg-noturnal-600/10 border-noturnal-500/20 text-gray-200 rounded-tr-none' 
                               : msg.sender === 'system'
                               ? 'bg-blue-900/10 border-blue-500/20 text-gray-400 italic rounded-tl-none'
                               : 'bg-[#1a1a20] border-white/10 text-gray-300 rounded-tl-none'
                           }`}>
                             {msg.text}
                           </div>
                        </div>
                     </div>
                   ))}
                   <div ref={messagesEndRef} />
                </div>

                {/* Input Area */}
                <div className="p-4 bg-[#121216] border-t border-white/5 relative z-10">
                  {selectedTicket.status === 'CLOSED' ? (
                    <div className="text-center text-gray-500 text-sm py-2 bg-white/5 rounded-lg border border-white/5">
                        This ticket has been closed. <button className="text-noturnal-400 hover:underline">Re-open?</button>
                    </div>
                  ) : (
                    <form onSubmit={handleSendMessage} className="relative">
                        <input 
                          type="text" 
                          value={messageInput}
                          onChange={(e) => setMessageInput(e.target.value)}
                          className="w-full bg-[#0a0a0c] border border-white/10 rounded-xl pl-4 pr-32 py-4 text-white placeholder-gray-600 focus:border-noturnal-500 focus:ring-1 focus:ring-noturnal-500 outline-none transition-all"
                          placeholder="Type your secure message..."
                        />
                        <div className="absolute right-2 top-1/2 -translate-y-1/2 flex gap-1">
                           <button type="button" className="p-2 text-gray-500 hover:text-white transition-colors rounded-lg hover:bg-white/5">
                              <Paperclip className="w-4 h-4" />
                           </button>
                           <button type="submit" className="bg-noturnal-600 hover:bg-noturnal-500 text-white p-2 rounded-lg transition-all shadow-lg shadow-noturnal-600/20">
                              <Send className="w-4 h-4" />
                           </button>
                        </div>
                    </form>
                  )}
                </div>
              </>
            ) : (
               /* Empty State */
               <div className="flex-1 flex flex-col items-center justify-center text-center p-8 opacity-50">
                  <div className="w-24 h-24 bg-white/5 rounded-full flex items-center justify-center mb-6 border border-white/10">
                    <Cpu className="w-10 h-10 text-gray-400" />
                  </div>
                  <h3 className="text-xl font-bold text-white mb-2">Secure Uplink Standby</h3>
                  <p className="text-gray-400 max-w-md">Select an active ticket from the list to view the secure channel or initialize a new transmission.</p>
               </div>
            )}
          </div>

        </div>

        {/* Bottom Info */}
        <div className="mt-8 grid grid-cols-1 md:grid-cols-3 gap-6">
           <div className="glass p-6 rounded-xl border border-white/5 flex items-start gap-4">
              <div className="p-3 bg-blue-500/10 rounded-lg border border-blue-500/20"><ShieldCheck className="w-6 h-6 text-blue-400" /></div>
              <div>
                 <h4 className="text-white font-bold text-sm">Encrypted Comms</h4>
                 <p className="text-xs text-gray-500 mt-1">All tickets are end-to-end encrypted. Your HWID and logs are safe.</p>
              </div>
           </div>
           <div className="glass p-6 rounded-xl border border-white/5 flex items-start gap-4">
              <div className="p-3 bg-purple-500/10 rounded-lg border border-purple-500/20"><Activity className="w-6 h-6 text-purple-400" /></div>
              <div>
                 <h4 className="text-white font-bold text-sm">Average Response</h4>
                 <p className="text-xs text-gray-500 mt-1">Current queue time is approximately <span className="text-white">45 minutes</span>.</p>
              </div>
           </div>
           <div className="glass p-6 rounded-xl border border-white/5 flex items-start gap-4">
              <div className="p-3 bg-orange-500/10 rounded-lg border border-orange-500/20"><Lock className="w-6 h-6 text-orange-400" /></div>
              <div>
                 <h4 className="text-white font-bold text-sm">Bug Bounty</h4>
                 <p className="text-xs text-gray-500 mt-1">Found a exploit? Report it with "High Priority" for a reward.</p>
              </div>
           </div>
        </div>
      </div>
    </div>
  );
};
