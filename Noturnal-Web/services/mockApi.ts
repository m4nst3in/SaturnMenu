import { User, PlanType } from '../types';

// Simulates network latency
const delay = (ms: number) => new Promise(resolve => setTimeout(resolve, ms));

export const mockApi = {
  login: async (email: string, password: string): Promise<User> => {
    await delay(800);
    // Simple mock - accepts any email/password for demo unless email contains 'error'
    if (email.includes('error')) throw new Error('Invalid credentials');
    
    return {
      id: 'uid_12345',
      username: email.split('@')[0],
      email,
      hwid: 'HWID-MOCKED-ABC-123',
      subscription: {
        active: false,
        plan: null,
        expiresAt: null
      }
    };
  },

  register: async (username: string, email: string, password: string): Promise<User> => {
    await delay(1000);
    return {
      id: `uid_${Date.now()}`,
      username,
      email,
      hwid: null,
      subscription: {
        active: false,
        plan: null,
        expiresAt: null
      }
    };
  },

  // Simulates purchase
  purchase: async (userId: string, plan: PlanType): Promise<User> => {
    await delay(1500);
    return {
      id: userId,
      username: 'User',
      email: 'user@example.com',
      hwid: 'HWID-MOCKED-ABC-123',
      subscription: {
        active: true,
        plan: plan,
        expiresAt: new Date(Date.now() + 30 * 24 * 60 * 60 * 1000).toISOString() // +30 days
      }
    };
  },

  resetHwid: async (user: User): Promise<User> => {
    await delay(1000);
    return {
      ...user,
      hwid: null
    };
  },

  downloadLoader: async (plan: PlanType | null) => {
    await delay(1000);
    if (!plan) throw new Error("Subscription required.");
    
    // Returns mocked URL
    if (plan === PlanType.PREMIUM) {
      return { url: 'noturnal_loader_kernel_v2.exe', method: 'SECURE_KERNEL' };
    }
    return { url: 'noturnal_loader_client.exe', method: 'STANDARD' };
  }
};