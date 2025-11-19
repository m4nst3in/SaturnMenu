import express from 'express'
import cors from 'cors'
import fs from 'fs'
import path from 'path'
import fetch from 'node-fetch'

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
  const { email } = req.body
  const user = data.users.find(u => u.email?.toLowerCase() === String(email).toLowerCase())
  if (!user) return res.status(400).json({ error: 'Invalid credentials' })
  data.currentUserId = user.id; save()
  res.json({ user, jwt: 'dev-token' })
})

app.post('/api/auth/register', (req, res) => {
  const { username, email } = req.body
  const user = {
    id: `uid_${Date.now()}`,
    username,
    email,
    hwid: null,
    subscription: { active: false, plan: null, expiresAt: null }
  }
  data.users.push(user); data.currentUserId = user.id; save()
  res.json({ user, jwt: 'dev-token' })
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

const port = process.env.PORT || 3000
app.listen(port, () => console.log(`Noturnal Node API running on http://localhost:${port}`))