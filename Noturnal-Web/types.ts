import React from 'react';

export enum PlanType {
  BASIC = 'BASIC',
  PREMIUM = 'PREMIUM'
}

export interface User {
  id: string;
  username: string;
  email: string;
  hwid: string | null;
  subscription: {
    active: boolean;
    plan: PlanType | null;
    expiresAt: string | null;
  };
}

export interface AuthState {
  user: User | null;
  isAuthenticated: boolean;
  isLoading: boolean;
}

export interface Feature {
  name: string;
  description: string;
  icon: React.ReactNode;
}

export interface ChatMessage {
  role: 'user' | 'model';
  text: string;
  timestamp: number;
}