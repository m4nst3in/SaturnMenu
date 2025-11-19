import React, { useState, useEffect } from 'react';
import { Link, useLocation } from 'react-router-dom';
import { useAuth } from '../context/AuthContext';
import { Menu, X, LogOut, Bell, User, Sparkles, Orbit } from 'lucide-react';

export const Navbar: React.FC = () => {
  const { isAuthenticated, user, logout } = useAuth();
  const [isOpen, setIsOpen] = useState(false);
  const [scrolled, setScrolled] = useState(false);
  const [showNotifications, setShowNotifications] = useState(false);
  const location = useLocation();

  useEffect(() => {
    const handleScroll = () => {
      setScrolled(window.scrollY > 50);
    };
    window.addEventListener('scroll', handleScroll);
    return () => window.removeEventListener('scroll', handleScroll);
  }, []);

  const isActive = (path: string) => location.pathname === path;

  return (
    <div className="fixed top-0 w-full z-50 flex justify-center pointer-events-none">
      <nav 
        className={`pointer-events-auto transition-all duration-500 ease-[cubic-bezier(0.32,0.72,0,1)] border-white/5
          ${scrolled 
            ? "mt-4 w-auto min-w-[400px] bg-[#0a0a0c]/80 backdrop-blur-xl border rounded-full shadow-[0_8px_32px_rgba(0,0,0,0.4)] py-2.5 px-3" 
            : "mt-0 w-full bg-gradient-to-b from-black/50 to-transparent border-b backdrop-blur-sm py-4 px-6 md:px-8"
        }`}
      >
        <div className="flex items-center justify-between gap-4 md:gap-8">
          
          {/* Logo Section */}
          <div className="flex items-center gap-3 shrink-0">
            <Link to="/" className="flex items-center gap-2 group">
              <div className="relative flex items-center justify-center">
                 <div className="absolute inset-0 bg-noturnal-500 blur-lg opacity-0 group-hover:opacity-50 transition-opacity duration-500"></div>
                 <Orbit className={`text-noturnal-400 transition-all duration-500 ${scrolled ? 'w-5 h-5' : 'w-6 h-6'} group-hover:rotate-180`} />
              </div>
              <span className={`font-bold tracking-tight transition-all duration-300 text-white ${scrolled ? 'text-sm' : 'text-lg'}`}>
                NOTURNAL
              </span>
            </Link>
          </div>
          
          {/* Desktop Navigation */}
          <div className={`hidden md:flex items-center ${scrolled ? 'gap-1' : 'gap-8'}`}>
            {['/', '/pricing', '/wiki'].map((path) => (
              <Link 
                key={path}
                to={path} 
                className={`relative text-sm font-medium transition-colors duration-300
                  ${scrolled 
                    ? `px-4 py-1.5 rounded-full ${isActive(path) ? 'bg-white/10 text-white' : 'text-gray-400 hover:text-white hover:bg-white/5'}`
                    : `${isActive(path) ? 'text-white' : 'text-gray-400 hover:text-white'}`
                  }
                `}
              >
                {path === '/' ? 'Home' : path.slice(1).charAt(0).toUpperCase() + path.slice(2)}
                {!scrolled && isActive(path) && (
                  <span className="absolute -bottom-4 left-0 w-full h-px bg-noturnal-500 shadow-[0_0_10px_#7645fd]"></span>
                )}
              </Link>
            ))}
            {isAuthenticated && (
               <Link 
                 to="/dashboard"
                 className={`relative text-sm font-medium transition-colors duration-300
                  ${scrolled 
                    ? `px-4 py-1.5 rounded-full ${isActive('/dashboard') ? 'bg-white/10 text-white' : 'text-gray-400 hover:text-white hover:bg-white/5'}`
                    : `${isActive('/dashboard') ? 'text-white' : 'text-gray-400 hover:text-white'}`
                  }
                `}
               >
                 Dashboard
               </Link>
            )}
          </div>

          {/* Right Actions */}
          <div className="hidden md:flex items-center gap-3 shrink-0">
            {isAuthenticated ? (
               <div className="flex items-center gap-2">
                 <div className="h-4 w-px bg-white/10 mx-1"></div>
                 
                 <Link to="/profile" className="group flex items-center gap-2 pl-1 pr-3 py-1 rounded-full hover:bg-white/5 transition-all border border-transparent hover:border-white/5">
                    <div className="w-6 h-6 rounded-full bg-gradient-to-br from-noturnal-500 to-noturnal-800 p-[1px]">
                      <div className="w-full h-full rounded-full bg-dark-bg flex items-center justify-center group-hover:bg-transparent transition-colors">
                         <User className="w-3 h-3 text-white" />
                      </div>
                    </div>
                    {!scrolled && <span className="text-xs font-medium text-gray-300">{user?.username}</span>}
                 </Link>
                 
                 <button onClick={logout} className="text-gray-500 hover:text-red-400 transition-colors p-1.5 rounded-full hover:bg-red-500/10">
                   <LogOut className="w-4 h-4" />
                 </button>
               </div>
            ) : (
              <Link to="/auth" className={`group relative rounded-full font-semibold text-xs overflow-hidden transition-all
                ${scrolled 
                  ? "bg-white text-black px-4 py-1.5 hover:scale-105" 
                  : "bg-noturnal-600 text-white px-6 py-2.5 hover:bg-noturnal-500 shadow-[0_0_20px_rgba(118,69,253,0.3)]"
                }
              `}>
                <span className="relative flex items-center gap-2">
                  Login <Sparkles className="w-3 h-3" />
                </span>
              </Link>
            )}
          </div>

          {/* Mobile Toggle */}
          <div className="md:hidden flex items-center">
            <button
              onClick={() => setIsOpen(!isOpen)}
              className="text-gray-400 hover:text-white p-1"
            >
              {isOpen ? <X className="h-6 w-6" /> : <Menu className="h-6 w-6" />}
            </button>
          </div>
        </div>

        {/* Mobile Menu */}
        {isOpen && (
          <div className="md:hidden absolute top-full left-0 right-0 mt-4 mx-2 p-2 bg-[#0a0a0c]/95 border border-white/10 rounded-2xl shadow-2xl animate-fade-in-up backdrop-blur-xl z-50">
            <div className="space-y-1">
              <Link to="/" className="block px-4 py-3 rounded-xl hover:bg-white/5 text-gray-300 text-sm font-medium" onClick={() => setIsOpen(false)}>Home</Link>
              <Link to="/pricing" className="block px-4 py-3 rounded-xl hover:bg-white/5 text-gray-300 text-sm font-medium" onClick={() => setIsOpen(false)}>Pricing</Link>
              <Link to="/wiki" className="block px-4 py-3 rounded-xl hover:bg-white/5 text-gray-300 text-sm font-medium" onClick={() => setIsOpen(false)}>Wiki</Link>
              {isAuthenticated ? (
                <>
                  <div className="h-px bg-white/5 my-2 mx-4"></div>
                  <Link to="/dashboard" className="block px-4 py-3 rounded-xl hover:bg-white/5 text-gray-300 text-sm font-medium" onClick={() => setIsOpen(false)}>Dashboard</Link>
                  <button onClick={() => { logout(); setIsOpen(false); }} className="w-full text-left px-4 py-3 rounded-xl hover:bg-red-500/10 text-red-400 text-sm font-medium">Logout</button>
                </>
              ) : (
                <Link to="/auth" className="block w-full mt-2 px-4 py-3 rounded-xl bg-noturnal-600 text-white text-center font-bold text-sm shadow-lg shadow-noturnal-600/20" onClick={() => setIsOpen(false)}>Login Portal</Link>
              )}
            </div>
          </div>
        )}
      </nav>
    </div>
  );
};