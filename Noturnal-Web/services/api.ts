export type PlanType = 'BASIC' | 'PREMIUM';

const BASE = (import.meta.env.VITE_API_BASE_URL as string) || 'http://localhost:4000';

const json = async (url: string, init?: RequestInit) => {
  const token = localStorage.getItem('noturnal_token');
  const headers: Record<string, string> = { 'Content-Type': 'application/json' };

  if (token) {
    headers['Authorization'] = `Bearer ${token}`;
  }

  const res = await fetch(url, { headers, ...init });
  if (!res.ok) throw new Error(await res.text());
  return res.json();
};

export const api = {
  // Auth endpoints
  login: async (email: string, password: string) => json(`${BASE}/api/auth/login`, { method: 'POST', body: JSON.stringify({ email, password }) }),
  register: async (username: string, email: string, password: string) => json(`${BASE}/api/auth/register`, { method: 'POST', body: JSON.stringify({ username, email, password }) }),

  // Subscription endpoints
  purchase: async (userId: string, plan: PlanType) => json(`${BASE}/api/subscription/purchase`, { method: 'POST', body: JSON.stringify({ plan }) }),
  resetHwid: async () => json(`${BASE}/api/subscription/reset-hwid`, { method: 'POST' }),

  // Download endpoints
  downloadLoader: async (plan: PlanType | null) => {
    if (!plan) throw new Error('Subscription required');
    return json(`${BASE}/api/downloads/loader?plan=${plan}`);
  },

  // User endpoints
  getUserInfo: async () => json(`${BASE}/api/user/info`),
  updateProfile: async (data: { username?: string; email?: string }) => json(`${BASE}/api/user/profile`, { method: 'PATCH', body: JSON.stringify(data) }),

  // Status endpoints
  getServerStatus: async () => json(`${BASE}/api/status`),
  getLatestVersion: async () => json(`${BASE}/api/version`),

  // Config endpoints
  getConfigs: async () => json(`${BASE}/api/configs`),
  uploadConfig: async (name: string, config: any) => json(`${BASE}/api/configs`, { method: 'POST', body: JSON.stringify({ name, config }) }),
};