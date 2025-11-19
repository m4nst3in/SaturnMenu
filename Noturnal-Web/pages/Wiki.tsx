
import React, { useState, useEffect, useRef } from 'react';
import { Bot, Send, User, Sparkles, Zap } from 'lucide-react';
import { generateWikiResponse } from '../services/geminiService';
import { ChatMessage } from '../types';

export const Wiki: React.FC = () => {
  const [messages, setMessages] = useState<ChatMessage[]>([
    {
      role: 'model',
      text: 'Hello, operator. I am the Noturnal Menu support AI. I can help with configurations, Kernel Driver questions, or installation. What do you need to know?',
      timestamp: Date.now()
    }
  ]);
  const [input, setInput] = useState('');
  const [isThinking, setIsThinking] = useState(false);
  const messagesEndRef = useRef<HTMLDivElement>(null);

  const scrollToBottom = () => {
    messagesEndRef.current?.scrollIntoView({ behavior: "smooth" });
  };

  useEffect(scrollToBottom, [messages]);

  const handleSend = async (text = input) => {
    if (!text.trim() || isThinking) return;

    const userMsg: ChatMessage = { role: 'user', text: text, timestamp: Date.now() };
    setMessages(prev => [...prev, userMsg]);
    setInput('');
    setIsThinking(true);

    try {
      const responseText = await generateWikiResponse(userMsg.text);
      const modelMsg: ChatMessage = { role: 'model', text: responseText, timestamp: Date.now() };
      setMessages(prev => [...prev, modelMsg]);
    } catch (error) {
      console.error(error);
    } finally {
      setIsThinking(false);
    }
  };

  const quickQuestions = [
    "How to install?",
    "Is it streamer proof?",
    "Best Legit settings?",
    "Status?"
  ];

  return (
    <div className="pt-24 min-h-screen bg-dark-bg flex flex-col">
      <div className="max-w-4xl mx-auto w-full px-4 flex-1 flex flex-col pb-8">
        
        <div className="mb-6 text-center">
          <div className="inline-flex items-center justify-center p-3 bg-noturnal-500/20 rounded-full mb-4 ring-1 ring-noturnal-500/50">
            <Bot className="w-8 h-8 text-noturnal-400" />
          </div>
          <h1 className="text-3xl font-bold text-white">Noturnal AI Support</h1>
          <p className="text-gray-400">Neural knowledge base. Ask anything.</p>
        </div>

        <div className="flex-1 glass rounded-2xl border border-white/10 flex flex-col overflow-hidden shadow-2xl min-h-[500px]">
          
          {/* Chat Area */}
          <div className="flex-1 overflow-y-auto p-6 space-y-6">
            {messages.map((msg, idx) => (
              <div key={idx} className={`flex gap-4 ${msg.role === 'user' ? 'flex-row-reverse' : ''}`}>
                <div className={`w-10 h-10 rounded-full flex items-center justify-center shrink-0 ${msg.role === 'user' ? 'bg-gray-700' : 'bg-noturnal-700'}`}> 
                  {msg.role === 'user' ? <User className="w-5 h-5" /> : <Sparkles className="w-5 h-5" />}
                </div>
                <div className={`max-w-[80%] p-4 rounded-2xl text-sm leading-relaxed ${msg.role === 'user' ? 'bg-gray-800 text-white rounded-tr-none' : 'bg-noturnal-900/50 border border-noturnal-500/20 text-gray-200 rounded-tl-none'}`}>
                  {msg.text.split('\n').map((line, i) => (
                    <p key={i} className={line.startsWith('-') ? 'ml-4' : ''}>{line}</p>
                  ))}
                </div>
              </div>
            ))}
            {isThinking && (
              <div className="flex gap-4">
                 <div className="w-10 h-10 rounded-full bg-noturnal-700 flex items-center justify-center shrink-0 animate-pulse">
                   <Sparkles className="w-5 h-5" />
                 </div>
                 <div className="flex items-center gap-1 h-10">
                   <span className="w-2 h-2 bg-gray-500 rounded-full animate-bounce"></span>
                   <span className="w-2 h-2 bg-gray-500 rounded-full animate-bounce [animation-delay:0.2s]"></span>
                   <span className="w-2 h-2 bg-gray-500 rounded-full animate-bounce [animation-delay:0.4s]"></span>
                 </div>
              </div>
            )}
            <div ref={messagesEndRef} />
          </div>

          {/* Input Area */}
          <div className="p-4 bg-black/20 border-t border-white/5">
            {/* Quick Chips */}
            <div className="flex gap-2 mb-3 overflow-x-auto pb-2">
              {quickQuestions.map((q, i) => (
                <button
                  key={i}
                  onClick={() => handleSend(q)}
                  className="whitespace-nowrap px-3 py-1 rounded-full bg-white/5 border border-white/10 text-xs text-gray-300 hover:bg-noturnal-600 hover:text-white hover:border-noturnal-500 transition-colors flex items-center gap-1"
                >
                  <Zap className="w-3 h-3" /> {q}
                </button>
              ))}
            </div>

            <div className="flex gap-2">
              <input 
                type="text"
                value={input}
                onChange={(e) => setInput(e.target.value)}
                onKeyDown={(e) => e.key === 'Enter' && handleSend()}
                placeholder="Ex: How do I configure aimbot to look legit?"
                className="flex-1 bg-dark-bg border border-white/10 rounded-xl px-4 py-3 text-white focus:outline-none focus:border-noturnal-500 focus:ring-1 focus:ring-noturnal-500 transition-all"
              />
              <button 
                onClick={() => handleSend()}
                disabled={isThinking || !input.trim()}
                className="bg-noturnal-600 hover:bg-noturnal-700 text-white p-3 rounded-xl transition-colors disabled:opacity-50 disabled:cursor-not-allowed"
              >
                <Send className="w-5 h-5" />
              </button>
            </div>
          </div>

        </div>
      </div>
    </div>
  );
};