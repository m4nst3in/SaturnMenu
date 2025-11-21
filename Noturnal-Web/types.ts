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
  gamification?: {
    level: number;
    xp: number;
    nextLevelXp: number;
    rank: string;
    badges: string[];
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

export type ConfigType = 'LEGIT' | 'RAGE' | 'SEMI' | 'HVH';

export interface ConfigItem {
  id: string;
  name: string;
  author: string;
  authorId: string;
  type: ConfigType;
  price: number; // 0 = Free
  downloads: number;
  rating: number;
  shareCode: string;
  description: string;
  lastUpdated: string;
  verified: boolean;
}

// Support System Types
export type TicketStatus = 'OPEN' | 'ANSWERED' | 'CLOSED';
export type TicketPriority = 'LOW' | 'MEDIUM' | 'HIGH' | 'CRITICAL';
export type TicketCategory = 'GENERAL' | 'TECHNICAL' | 'BILLING' | 'BUG_REPORT';

export interface TicketMessage {
  id: string;
  sender: 'user' | 'admin' | 'system';
  text: string;
  timestamp: string;
}

export interface Ticket {
  id: string;
  userId: string;
  subject: string;
  category: TicketCategory;
  priority: TicketPriority;
  status: TicketStatus;
  createdAt: string;
  lastUpdated: string;
  messages: TicketMessage[];
}