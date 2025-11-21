import React, { createContext, useContext, useState, useEffect } from 'react';
import { User, AuthState, PlanType } from '../types';
import { api } from '../services/api';

interface AuthContextType extends AuthState {
  login: (email: string, pass: string) => Promise<void>;
  register: (user: string, email: string, pass: string) => Promise<void>;
  logout: () => void;
  upgradePlan: (plan: PlanType) => Promise<void>;
  resetHwid: () => Promise<void>;
}

const AuthContext = createContext<AuthContextType | undefined>(undefined);

export const AuthProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [state, setState] = useState<AuthState>({
    user: null,
    isAuthenticated: false,
    isLoading: true
  });

  useEffect(() => {
    // Check local storage for persistence
    const storedUser = localStorage.getItem('noturnal_user');
    if (storedUser) {
      setState({
        user: JSON.parse(storedUser),
        isAuthenticated: true,
        isLoading: false
      });
    } else {
      setState(s => ({ ...s, isLoading: false }));
    }
  }, []);

  const login = async (email: string, pass: string) => {
    const { user, jwt } = await api.login(email, pass);
    localStorage.setItem('noturnal_user', JSON.stringify(user));
    localStorage.setItem('noturnal_token', jwt);
    setState({ user, isAuthenticated: true, isLoading: false });
  };

  const register = async (username: string, email: string, pass: string) => {
    const { user, jwt } = await api.register(username, email, pass);
    localStorage.setItem('noturnal_user', JSON.stringify(user));
    localStorage.setItem('noturnal_token', jwt);
    setState({ user, isAuthenticated: true, isLoading: false });
  };

  const logout = () => {
    localStorage.removeItem('noturnal_user');
    setState({ user: null, isAuthenticated: false, isLoading: false });
  };

  const upgradePlan = async (plan: PlanType) => {
    if (!state.user) return;
    const updatedUser = await api.purchase(state.user.id, plan);
    localStorage.setItem('noturnal_user', JSON.stringify(updatedUser));
    setState({ ...state, user: updatedUser });
  };

  const resetHwid = async () => {
    if (!state.user) return;
    const updatedUser = await api.resetHwid(state.user);
    localStorage.setItem('noturnal_user', JSON.stringify(updatedUser));
    setState({ ...state, user: updatedUser });
  };

  return (
    <AuthContext.Provider value={{ ...state, login, register, logout, upgradePlan, resetHwid }}>
      {children}
    </AuthContext.Provider>
  );
};

export const useAuth = () => {
  const context = useContext(AuthContext);
  if (!context) throw new Error('useAuth must be used within AuthProvider');
  return context;
};