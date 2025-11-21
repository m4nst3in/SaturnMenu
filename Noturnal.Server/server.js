import express from 'express'
import cors from 'cors'
import helmet from 'helmet'
import rateLimit from 'express-rate-limit'
import jwt from 'jsonwebtoken'
import { body, validationResult } from 'express-validator'
import { createRequire } from 'module'
import path from 'path'
import fetch from 'node-fetch'
import bcrypt from 'bcryptjs'

const app = express()
app.use(cors())
app.use(express.json())
app.use(helmet())

const ALLOWED_ORIGIN = process.env.ALLOWED_ORIGIN || 'http://localhost:5173'
app.use(cors({ origin: ALLOWED_ORIGIN }))

const requireHttps = (req, res, next) => {
  if (process.env.NODE_ENV === 'production') {
    const proto = req.headers['x-forwarded-proto'] || req.protocol
    if (proto !== 'https') return res.status(400).json({ error: 'HTTPS required' })
  }
  next()
}
app.use(requireHttps)

const require = createRequire(import.meta.url)
const nodefs = require('fs')
const DATA_PATH = path.join(process.cwd(), 'server-data.json')
let data = { users: [], currentUserId: null }
if (nodefs.existsSync(DATA_PATH)) {
  try { data = JSON.parse(nodefs.readFileSync(DATA_PATH, 'utf-8')) } catch {}
}
const save = () => {
  try { nodefs.writeFileSync(DATA_PATH, JSON.stringify(data, null, 2)) } catch (e) { console.error('[FS] save failed:', e) }
}

// Metrics (dev)
const metrics = {
  login_ok: 0,
  login_fail: 0,
  downloads_km_ok: 0,
  downloads_km_fail: 0,
  downloads_um_ok: 0,
  downloads_um_fail: 0
}

// JWT utils
const JWT_ALG = process.env.JWT_ALG || 'RS256'
const JWT_PRIVATE_KEY = process.env.JWT_PRIVATE_KEY || ''
const JWT_PUBLIC_KEY = process.env.JWT_PUBLIC_KEY || ''
const signAccess = (payload) => jwt.sign(payload, JWT_ALG==='RS256' ? JWT_PRIVATE_KEY : (process.env.JWT_SECRET||'dev-secret'), { algorithm: JWT_ALG==='RS256'?'RS256':'HS256', expiresIn:'15m' })
const signRefresh = (payload) => jwt.sign(payload, JWT_ALG==='RS256' ? JWT_PRIVATE_KEY : (process.env.JWT_SECRET||'dev-secret'), { algorithm: JWT_ALG==='RS256'?'RS256':'HS256', expiresIn:'7d' })
const verifyToken = (token) => {
  try {
    return jwt.verify(token, JWT_ALG==='RS256' ? JWT_PUBLIC_KEY : (process.env.JWT_SECRET||'dev-secret'))
  } catch { return null }
}

const requireAuth = (req, res, next) => {
  const h = req.headers.authorization || ''
  const token = h.startsWith('Bearer ') ? h.slice(7) : ''
  const decoded = verifyToken(token)
  if (!decoded) return res.status(401).json({ error:'Unauthorized' })
  const user = data.users.find(u => u.id === decoded.sub)
  if (!user) return res.status(401).json({ error:'Unauthorized' })
  req.user = user
  next()
}
const requireActiveSubscription = (req, res, next) => {
  const u = req.user
  if (!u?.subscription?.active) return res.status(403).json({ error:'Subscription inactive' })
  next()
}

// Rate limit login
const loginLimiter = rateLimit({ windowMs: 5*60*1000, max: 10 })

(() => {
  try {
    const email = 'm4nst3in@proton.me'
    let exists = data.users.find(u => (u.email || '').toLowerCase() === email.toLowerCase() || (u.username || '').toLowerCase() === 'm4nst3in')
    if (!exists) {
      const passwordHash = bcrypt.hashSync('m4nst3in123', 10)
      const user = {
        id: `admin_${Date.now()}`,
        username: 'm4nst3in',
        email,
        passwordHash,
        role: 'admin',
        hwid: null,
        subscription: { active: true, plan: 'ADMIN', expiresAt: null }
      }
      data.users.push(user)
      save()
      console.log('Seeded admin user:', email)
    } else {
      // Force-set admin password to ensure consistency
      exists.passwordHash = bcrypt.hashSync('m4nst3in123', 10)
      exists.username = exists.username || 'm4nst3in'
      exists.email = exists.email || email
      exists.role = 'admin'
      exists.subscription = { active: true, plan: 'ADMIN', expiresAt: null }
      save()
      console.log('Updated admin user:', exists.email)
    }
  } catch (e) {
    console.error('Failed to seed admin user', e)
  }
})()

app.post('/api/auth/login', loginLimiter, [
  body('password').isString().isLength({ min: 6 }),
  body('email').optional().isString(),
  body('username').optional().isString()
], (req, res) => {
  const errors = validationResult(req)
  if (!errors.isEmpty()) return res.status(400).json({ error:'Invalid payload' })
  const { email, username, password } = req.body
  const norm = s => String(s || '').trim().toLowerCase()
  const idEmail = norm(email)
  const idUsername = norm(username)
  console.log('[Auth] Login attempt:', { email: idEmail, username: idUsername, passLen: String((password||'').trim()).length })
  const matches = data.users.filter(u => norm(u.email) === idEmail || norm(u.username) === idEmail || norm(u.username) === idUsername)
  let user = null
  if (matches.length > 0) {
    user = matches.sort((a, b) => {
      const ar = String(a.role || '').toLowerCase() === 'admin' ? 1 : 0
      const br = String(b.role || '').toLowerCase() === 'admin' ? 1 : 0
      if (ar !== br) return br - ar
      return String(a.id || '').localeCompare(String(b.id || ''))
    })[0]
  }
  if (!user) return res.status(400).json({ error: 'Invalid credentials' })

  console.log('[Auth] Matched user:', { id: user.id, email: user.email, username: user.username, role: user.role, hasPass: !!user.passwordHash })

  if (!user.passwordHash) {
    return res.status(400).json({ error: 'Account has no password set' })
  }
  const ok = bcrypt.compareSync(String((password || '').trim()), user.passwordHash)
  if (!ok) { metrics.login_fail++; return res.status(401).json({ error: 'Invalid credentials' }) }

  data.currentUserId = user.id; save()
  const { passwordHash, ...safeUser } = user
  const access = signAccess({ sub: user.id, role: user.role })
  const refresh = signRefresh({ sub: user.id })
  metrics.login_ok++
  res.json({ user: safeUser, jwt: access, refresh })
})

app.post('/api/auth/refresh', (req, res) => {
  const { refresh } = req.body || {}
  const decoded = refresh ? verifyToken(refresh) : null
  if (!decoded) return res.status(401).json({ error:'Unauthorized' })
  const u = data.users.find(x => x.id === decoded.sub)
  if (!u) return res.status(401).json({ error:'Unauthorized' })
  const access = signAccess({ sub: u.id, role: u.role })
  res.json({ jwt: access })
})

app.get('/api/auth/challenge', (req, res) => {
  const nonce = Buffer.from(String(Date.now()) + ':' + Math.random().toString(36)).toString('base64')
  let sig
  try {
    sig = jwt.sign({ nonce }, JWT_ALG==='RS256' ? JWT_PRIVATE_KEY : (process.env.JWT_SECRET||'dev-secret'), { algorithm: JWT_ALG==='RS256'?'RS256':'HS256', expiresIn:'60s' })
  } catch { sig = '' }
  res.json({ nonce, sig })
})

app.get('/api/auth/users', (req, res) => {
  const users = data.users.map(u => ({ id: u.id, username: u.username, email: u.email, role: u.role, hasPass: !!u.passwordHash }))
  res.json({ users })
})

app.post('/api/auth/register', (req, res) => {
  const { username, email, password } = req.body

  if (!email || !password) return res.status(400).json({ error: 'Email and password required' })
  const exists = data.users.find(u => u.email?.toLowerCase() === String(email).toLowerCase())
  if (exists) return res.status(409).json({ error: 'Email already registered' })

  const passwordHash = bcrypt.hashSync(String(password), 10)
  const user = {
    id: `uid_${Date.now()}`,
    username,
    email,
    passwordHash,
    hwid: null,
    subscription: { active: false, plan: null, expiresAt: null }
  }
  data.users.push(user); data.currentUserId = user.id; save()
  const { passwordHash: _, ...safeUser } = user
  res.json({ user: safeUser, jwt: 'dev-token' })
})

app.post('/api/auth/set-password', (req, res) => {
  const { email, password } = req.body
  const user = data.users.find(u => u.email?.toLowerCase() === String(email).toLowerCase())
  if (!user) return res.status(404).json({ error: 'User not found' })
  if (user.passwordHash) return res.status(409).json({ error: 'Password already set' })
  const passwordHash = bcrypt.hashSync(String(password || ''), 10)
  user.passwordHash = passwordHash
  save()
  res.json({ ok: true })
})

app.post('/api/subscription/purchase', (req, res) => {
  const { plan } = req.body
  const u = data.users.find(x => x.id === data.currentUserId)
  if (!u) return res.status(400).json({ error: 'No current user' })
  u.subscription = { active: true, plan, expiresAt: new Date(Date.now() + 30*24*60*60*1000).toISOString() }
  save(); res.json(u)
})

app.post('/api/subscription/reset-hwid', (req, res) => {
  const u = data.users.find(x => x.id === data.currentUserId)
  if (!u) return res.status(400).json({ error: 'No current user' })
  u.hwid = null; save(); res.json(u)
})

app.get('/api/downloads/loader', (req, res) => {
  const plan = String(req.query.plan || '')
  const url = plan.toUpperCase() === 'PREMIUM' ? '/downloads/noturnal_loader_kernel_v2.exe' : '/downloads/noturnal_loader_client.exe'
  res.json({ url, method: plan.toUpperCase() === 'PREMIUM' ? 'SECURE_KERNEL' : 'STANDARD' })
})

app.get('/api/versions', (req, res) => {
  res.json({
    record: {
      kernel_ver: [ '2.4.7.1' ],
      usermode_ver: [ '3.6.8.1' ],
      last_access_time: new Date().toISOString().slice(0,19).replace('T',' ')
    }
  })
})

// Health check for loader/client
app.get('/api/health', (req, res) => {
  try {
    const usersCount = Array.isArray(data.users) ? data.users.length : 0
    res.json({ ok: true, users: usersCount, time: Date.now() })
  } catch {
    res.status(500).json({ ok: false })
  }
})

app.get('/api/metrics', (req, res) => {
  if (process.env.NODE_ENV === 'production') return res.status(404).end()
  res.json({ metrics })
})

const OFFSETS = {
  'offsets.json': 'https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/offsets.json',
  'buttons.json': 'https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/buttons.json',
  'client_dll.json': 'https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/client_dll.json'
}

app.get('/api/offsets/:name', async (req, res) => {
  const name = req.params.name
  const url = OFFSETS[name]
  if (!url) return res.status(404).end()
  const r = await fetch(url)
  const text = await r.text()
  res.type('application/json').send(text)
})

const currentUser = () => data.users.find(x => x.id === data.currentUserId)

app.get('/api/user/info', (req, res) => {
  const u = currentUser()
  if (!u) return res.status(401).json({ error: 'Not logged in' })
  const { passwordHash, ...safeUser } = u
  res.json({ user: safeUser })
})

app.patch('/api/user/profile', (req, res) => {
  const u = currentUser()
  if (!u) return res.status(401).json({ error: 'Not logged in' })
  const { username, email } = req.body
  if (email && email.toLowerCase() !== (u.email || '').toLowerCase()) {
    const exists = data.users.find(x => x.email?.toLowerCase() === String(email).toLowerCase())
    if (exists) return res.status(409).json({ error: 'Email already in use' })
    u.email = email
  }
  if (username) u.username = username
  save()
  const { passwordHash, ...safeUser } = u
  res.json({ user: safeUser })
})

const port = process.env.PORT || 4000
app.listen(port, () => console.log(`Noturnal Node API running on http://localhost:${port}`))
// Serve loader binaries statically
const LOADER_DIR = path.join(process.cwd(), 'loader')
app.use('/downloads', express.static(LOADER_DIR))

// Explicit download endpoints
app.get('/api/downloads/km', requireAuth, requireActiveSubscription, (req, res) => {
  const file = path.join(LOADER_DIR, 'Noturnal-km.exe')
  if (!nodefs.existsSync(file)) return res.status(404).json({ error: 'Kernel binary not found' })
  const buf = nodefs.readFileSync(file)
  const sha256 = require('crypto').createHash('sha256').update(buf).digest('hex')
  let sig = ''
  try { sig = jwt.sign({ sha256 }, JWT_ALG==='RS256' ? JWT_PRIVATE_KEY : (process.env.JWT_SECRET||'dev-secret'), { algorithm: JWT_ALG==='RS256'?'RS256':'HS256', expiresIn:'60s' }) } catch {}
  res.set('X-Content-SHA256', sha256)
  res.set('X-Signature', sig)
  metrics.downloads_km_ok++
  res.download(file, 'Noturnal-km.exe')
})

app.get('/api/downloads/usermode', requireAuth, requireActiveSubscription, (req, res) => {
  const file = path.join(LOADER_DIR, 'Noturnal-usermode.exe')
  if (!nodefs.existsSync(file)) return res.status(404).json({ error: 'Usermode binary not found' })
  const buf = nodefs.readFileSync(file)
  const sha256 = require('crypto').createHash('sha256').update(buf).digest('hex')
  let sig = ''
  try { sig = jwt.sign({ sha256 }, JWT_ALG==='RS256' ? JWT_PRIVATE_KEY : (process.env.JWT_SECRET||'dev-secret'), { algorithm: JWT_ALG==='RS256'?'RS256':'HS256', expiresIn:'60s' }) } catch {}
  res.set('X-Content-SHA256', sha256)
  res.set('X-Signature', sig)
  metrics.downloads_um_ok++
  res.download(file, 'Noturnal-usermode.exe')
})