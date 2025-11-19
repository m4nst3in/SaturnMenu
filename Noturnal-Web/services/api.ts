export type PlanType = 'BASIC' | 'PREMIUM';

const BASE = (import.meta.env.VITE_API_BASE_URL as string) || 'http://localhost:3000';

const json = async (url: string, init?: RequestInit) => {
  const res = await fetch(url, { headers: { 'Content-Type': 'application/json' }, ...init });
  if (!res.ok) throw new Error(await res.text());
  return res.json();
};

export const api = {
  login: async (email: string, password: string) => json(`${BASE}/api/auth/login`, { method: 'POST', body: JSON.stringify({ email, password }) }),
  register: async (username: string, email: string, password: string) => json(`${BASE}/api/auth/register`, { method: 'POST', body: JSON.stringify({ username, email, password }) }),
  purchase: async (userId: string, plan: PlanType) => json(`${BASE}/api/subscription/purchase`, { method: 'POST', body: JSON.stringify({ plan }) }),
  resetHwid: async () => json(`${BASE}/api/subscription/reset-hwid`, { method: 'POST' }),
  downloadLoader: async (plan: PlanType | null) => {
    if (!plan) throw new Error('Subscription required');
    return json(`${BASE}/api/downloads/loader?plan=${plan}`);
  }
};