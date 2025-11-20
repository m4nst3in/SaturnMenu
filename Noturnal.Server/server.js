import express from 'express'
import cors from 'cors'
import fs from 'fs'
import path from 'path'
import fetch from 'node-fetch'
import bcrypt from 'bcryptjs'

const app = express()
app.use(cors())
app.use(express.json())

const DATA_PATH = path.join(process.cwd(), 'server-data.json')
let data = { users: [], currentUserId: null }
if (fs.existsSync(DATA_PATH)) {
  try { data = JSON.parse(fs.readFileSync(DATA_PATH, 'utf-8')) } catch {}
}
const save = () => fs.writeFileSync(DATA_PATH, JSON.stringify(data, null, 2))

app.post('/api/auth/login', (req, res) => {
  const { email, password } = req.body
  const user = data.users.find(u => u.email?.toLowerCase() === String(email).toLowerCase())
  if (!user) return res.status(400).json({ error: 'Invalid credentials' })

  if (!user.passwordHash) {
    return res.status(400).json({ error: 'Account has no password set' })
  }
  const ok = bcrypt.compareSync(String(password || ''), user.passwordHash)
  if (!ok) return res.status(401).json({ error: 'Invalid credentials' })

  data.currentUserId = user.id; save()
  const { passwordHash, ...safeUser } = user
  res.json({ user: safeUser, jwt: 'dev-token' })
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
app.get('/api/downloads/km', (req, res) => {
  const file = path.join(LOADER_DIR, 'Noturnal-km.exe')
  if (!fs.existsSync(file)) return res.status(404).json({ error: 'Kernel binary not found' })
  res.download(file, 'Noturnal-km.exe')
})

app.get('/api/downloads/usermode', (req, res) => {
  const file = path.join(LOADER_DIR, 'Noturnal-usermode.exe')
  if (!fs.existsSync(file)) return res.status(404).json({ error: 'Usermode binary not found' })
  res.download(file, 'Noturnal-usermode.exe')
})