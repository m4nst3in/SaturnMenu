import { User, PlanType, ConfigItem, ConfigType, Ticket, TicketMessage, TicketCategory, TicketPriority } from '../types';

// Simulates network latency
const delay = (ms: number) => new Promise(resolve => setTimeout(resolve, ms));

const MOCK_CONFIGS: ConfigItem[] = [
  {
    id: 'cfg_1',
    name: 'S1mple Logic V3',
    author: 'ProPlayerX',
    authorId: 'uid_99',
    type: 'LEGIT',
    price: 0,
    downloads: 15420,
    rating: 4.9,
    shareCode: 'CS2-X7Z9-MM22',
    description: 'Perfect for Prime Matchmaking. Low FOV, smooth aim, visible-only chams.',
    lastUpdated: new Date(Date.now() - 2 * 24 * 60 * 60 * 1000).toISOString(),
    verified: true
  },
  {
    id: 'cfg_2',
    name: 'HvH Dominator',
    author: 'SpinBotter',
    authorId: 'uid_88',
    type: 'RAGE',
    price: 5.00,
    downloads: 892,
    rating: 4.5,
    shareCode: 'CS2-RAGE-9999',
    description: 'Aggressive auto-wall, anti-aim presets, and instant trigger.',
    lastUpdated: new Date(Date.now() - 5 * 24 * 60 * 60 * 1000).toISOString(),
    verified: true
  },
  {
    id: 'cfg_3',
    name: 'Semi-Rage Public',
    author: 'CasualGamer',
    authorId: 'uid_77',
    type: 'SEMI',
    price: 0,
    downloads: 2341,
    rating: 4.7,
    shareCode: 'CS2-SEMI-PUB1',
    description: 'Best of both worlds. Use with caution in matchmaking.',
    lastUpdated: new Date(Date.now() - 14 * 24 * 60 * 60 * 1000).toISOString(),
    verified: false
  },
  {
    id: 'cfg_4',
    name: 'Scout Magic',
    author: 'AWP_God',
    authorId: 'uid_66',
    type: 'HVH',
    price: 2.50,
    downloads: 560,
    rating: 4.8,
    shareCode: 'CS2-SCOUT-X1',
    description: 'Optimized specifically for SSG-08. Jump scout ready.',
    lastUpdated: new Date(Date.now() - 1 * 24 * 60 * 60 * 1000).toISOString(),
    verified: false
  },
  {
    id: 'cfg_5',
    name: 'Ghost Legit',
    author: 'UndetectedUser',
    authorId: 'uid_55',
    type: 'LEGIT',
    price: 0,
    downloads: 5432,
    rating: 4.6,
    shareCode: 'CS2-GST-001',
    description: 'Very low FOV, intended for high-tier faceit (use at own risk).',
    lastUpdated: new Date(Date.now() - 10 * 24 * 60 * 60 * 1000).toISOString(),
    verified: true
  }
];

// Mock Tickets
let MOCK_TICKETS: Ticket[] = [
  {
    id: 'tkt_1023',
    userId: 'uid_12345',
    subject: 'Loader crashing on inject',
    category: 'TECHNICAL',
    priority: 'HIGH',
    status: 'ANSWERED',
    createdAt: new Date(Date.now() - 1000 * 60 * 60 * 2).toISOString(),
    lastUpdated: new Date(Date.now() - 1000 * 60 * 10).toISOString(),
    messages: [
      { id: 'm1', sender: 'user', text: 'Hi, whenever I press inject, my game crashes instantly.', timestamp: new Date(Date.now() - 1000 * 60 * 60 * 2).toISOString() },
      { id: 'm2', sender: 'admin', text: 'Hello. Please ensure you have disabled Secure Boot in BIOS and uninstalled any third-party antivirus.', timestamp: new Date(Date.now() - 1000 * 60 * 10).toISOString() }
    ]
  }
];

export const mockApi = {
  login: async (email: string, password: string): Promise<{ user: User; jwt: string }> => {
    await delay(800);
    if (email.includes('error')) throw new Error('Invalid credentials');

    const user: User = {
      id: 'uid_12345',
      username: email.split('@')[0],
      email,
      hwid: 'HWID-MOCKED-ABC-123',
      subscription: {
        active: true,
        plan: PlanType.PREMIUM,
        expiresAt: new Date(Date.now() + 30 * 24 * 60 * 60 * 1000).toISOString()
      },
      gamification: {
        level: 4,
        xp: 2450,
        nextLevelXp: 3000,
        rank: "Script Kiddie",
        badges: ["Early Adopter", "Beta Tester"]
      }
    };

    return { user, jwt: 'mock-jwt-token-login' };
  },

  register: async (username: string, email: string, password: string): Promise<{ user: User; jwt: string }> => {
    await delay(1000);
    const user: User = {
      id: `uid_${Date.now()}`,
      username,
      email,
      hwid: null,
      subscription: {
        active: false,
        plan: null,
        expiresAt: null
      },
      gamification: {
        level: 1,
        xp: 0,
        nextLevelXp: 500,
        rank: "Newbie",
        badges: []
      }
    };

    return { user, jwt: 'mock-jwt-token-register' };
  },

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
        expiresAt: new Date(Date.now() + 30 * 24 * 60 * 60 * 1000).toISOString()
      },
      gamification: {
        level: 5, // Level up on purchase
        xp: 2800,
        nextLevelXp: 3000,
        rank: "Supporter",
        badges: ["Early Adopter", "Beta Tester", "Premium"]
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
    if (plan === PlanType.PREMIUM) {
      return { url: 'noturnal_loader_kernel_v2.exe', method: 'SECURE_KERNEL' };
    }
    return { url: 'noturnal_loader_client.exe', method: 'STANDARD' };
  },

  getUserInfo: async (): Promise<User> => {
    await delay(500);
    const storedUser = localStorage.getItem('noturnal_user');
    if (storedUser) return JSON.parse(storedUser);
    throw new Error('Not authenticated');
  },

  getConfigs: async (filter?: ConfigType): Promise<ConfigItem[]> => {
    await delay(600);
    if (filter) {
      return MOCK_CONFIGS.filter(c => c.type === filter);
    }
    return MOCK_CONFIGS;
  },

  getMyConfigs: async (userId: string): Promise<ConfigItem[]> => {
    await delay(400);
    return [
      {
        id: 'cfg_user_1',
        name: 'My Personal Legit',
        author: 'Me',
        authorId: userId,
        type: 'LEGIT',
        price: 0,
        downloads: 12,
        rating: 5.0,
        shareCode: 'CS2-MY-TEST',
        description: 'My private settings.',
        lastUpdated: new Date().toISOString(),
        verified: false
      }
    ];
  },

  publishConfig: async (data: Partial<ConfigItem>): Promise<ConfigItem> => {
    await delay(1200);
    const newConfig: ConfigItem = {
      id: `cfg_${Date.now()}`,
      name: data.name || 'Untitled Config',
      author: 'User', 
      authorId: 'uid_12345',
      type: data.type || 'LEGIT',
      price: data.price || 0,
      downloads: 0,
      rating: 0,
      shareCode: `CS2-${Math.floor(Math.random()*9999)}-GEN`,
      description: data.description || '',
      lastUpdated: new Date().toISOString(),
      verified: false
    };
    MOCK_CONFIGS.unshift(newConfig);
    return newConfig;
  },

  // Ticket System Methods
  getTickets: async (userId: string): Promise<Ticket[]> => {
    await delay(500);
    return MOCK_TICKETS.filter(t => t.userId === userId);
  },

  createTicket: async (userId: string, subject: string, category: TicketCategory, priority: TicketPriority, initialMessage: string): Promise<Ticket> => {
    await delay(1000);
    const newTicket: Ticket = {
      id: `tkt_${Date.now()}`,
      userId,
      subject,
      category,
      priority,
      status: 'OPEN',
      createdAt: new Date().toISOString(),
      lastUpdated: new Date().toISOString(),
      messages: [
        { id: `msg_${Date.now()}`, sender: 'user', text: initialMessage, timestamp: new Date().toISOString() },
        { id: `msg_${Date.now()}_auto`, sender: 'system', text: 'Support ticket created. An agent will be with you shortly.', timestamp: new Date().toISOString() }
      ]
    };
    MOCK_TICKETS.unshift(newTicket);
    return newTicket;
  },

  sendMessage: async (ticketId: string, text: string, sender: 'user' | 'admin'): Promise<TicketMessage> => {
    await delay(300);
    const ticket = MOCK_TICKETS.find(t => t.id === ticketId);
    if (!ticket) throw new Error("Ticket not found");
    
    const newMessage: TicketMessage = {
      id: `msg_${Date.now()}`,
      sender,
      text,
      timestamp: new Date().toISOString()
    };
    ticket.messages.push(newMessage);
    ticket.lastUpdated = new Date().toISOString();
    
    // Simulate admin reply if user sent message
    if (sender === 'user') {
        ticket.status = 'OPEN'; // Re-open if it was answered
        setTimeout(() => {
            const reply: TicketMessage = {
                id: `msg_${Date.now()}_rep`,
                sender: 'admin',
                text: "Thank you for the update. We are investigating the logs.",
                timestamp: new Date().toISOString()
            };
            ticket.messages.push(reply);
            ticket.status = 'ANSWERED';
        }, 5000);
    }

    return newMessage;
  }
};