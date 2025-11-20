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
  },

  // New endpoints
  getUserInfo: async (): Promise<User> => {
    await delay(500);
    const storedUser = localStorage.getItem('noturnal_user');
    if (storedUser) return JSON.parse(storedUser);
    throw new Error('Not authenticated');
  },

  updateProfile: async (data: { username?: string; email?: string }): Promise<User> => {
    await delay(800);
    const storedUser = localStorage.getItem('noturnal_user');
    if (!storedUser) throw new Error('Not authenticated');

    const user = JSON.parse(storedUser);
    const updated = { ...user, ...data };
    localStorage.setItem('noturnal_user', JSON.stringify(updated));
    return updated;
  },

  getServerStatus: async () => {
    await delay(300);
    return {
      status: 'online',
      uptime: 99.6,
      activeUsers: 15234,
      lastUpdate: new Date().toISOString(),
      services: {
        auth: 'operational',
        downloads: 'operational',
        api: 'operational'
      }
    };
  },

  getLatestVersion: async () => {
    await delay(200);
    return {
      version: 'v2.4.1',
      releaseDate: new Date(Date.now() - 2 * 60 * 60 * 1000).toISOString(), // 2 hours ago
      changelog: [
        'Security update - Offsets updated for latest CS2 patch',
        'Performance improvements',
        'Bug fixes'
      ],
      downloadUrl: '/api/downloads/loader'
    };
  },

  getConfigs: async () => {
    await delay(600);
    return [
      {
        id: 'cfg_1',
        name: 'Legit Pro V2',
        author: 'S1mpleFan',
        type: 'LEGIT',
        downloads: 1523,
        rating: 4.8,
        createdAt: new Date(Date.now() - 7 * 24 * 60 * 60 * 1000).toISOString()
      },
      {
        id: 'cfg_2',
        name: 'HvH Destroyer',
        author: 'SpinBotter',
        type: 'RAGE',
        downloads: 892,
        rating: 4.5,
        createdAt: new Date(Date.now() - 3 * 24 * 60 * 60 * 1000).toISOString()
      },
      {
        id: 'cfg_3',
        name: 'Semi-Rage Pub',
        author: 'CasualGamer',
        type: 'SEMI',
        downloads: 2341,
        rating: 4.9,
        createdAt: new Date(Date.now() - 14 * 24 * 60 * 60 * 1000).toISOString()
      }
    ];
  },

  uploadConfig: async (name: string, config: any) => {
    await delay(1000);
    return {
      id: `cfg_${Date.now()}`,
      name,
      config,
      success: true
    };
  }
};