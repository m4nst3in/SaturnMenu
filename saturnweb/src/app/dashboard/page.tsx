'use client'

import { useEffect, useState } from 'react'
import { useRouter } from 'next/navigation'
import Navbar from '@/components/layout/Navbar'
import Card from '@/components/ui/Card'
import Button from '@/components/ui/Button'
import { formatDate, daysUntil } from '@/lib/utils'
import styles from './dashboard.module.css'

interface User {
    id: string
    email: string
    username: string
    createdAt: string
    subscription: {
        plan: string
        game: string
        status: string
        expiresAt: string
    } | null
}

export default function DashboardPage() {
    const router = useRouter()
    const [user, setUser] = useState<User | null>(null)
    const [loading, setLoading] = useState(true)

    useEffect(() => {
        checkAuth()
    }, [])

    const checkAuth = async () => {
        try {
            const res = await fetch('/api/auth/session')
            const data = await res.json()

            if (!data.authenticated) {
                router.push('/login')
                return
            }

            setUser(data.user)
        } catch (error) {
            console.error('Auth check failed:', error)
            router.push('/login')
        } finally {
            setLoading(false)
        }
    }

    const handleDownload = async (type: 'usermode' | 'kernelmode') => {
        try {
            const res = await fetch(`/api/downloads/${type}`)
            const data = await res.json()

            if (!res.ok) {
                alert(data.message)
                return
            }

            alert(`Download authorized! URL: ${data.downloadUrl}`)
        } catch (error) {
            alert('Download failed')
        }
    }

    const handleLogout = async () => {
        await fetch('/api/auth/logout', { method: 'POST' })
        router.push('/')
    }

    if (loading) {
        return (
            <div className={styles.loading}>
                <div className={styles.spinner}></div>
            </div>
        )
    }

    if (!user) return null

    const subscription = user.subscription
    const daysLeft = subscription ? daysUntil(subscription.expiresAt) : 0

    return (
        <>
            <Navbar />
            <main className={styles.main}>
                <div className="container">
                    <div className={styles.header}>
                        <div>
                            <h1 className={styles.title}>Welcome back, {user.username}!</h1>
                            <p className={styles.subtitle}>Manage your subscription and downloads</p>
                        </div>
                        <Button variant="ghost" onClick={handleLogout}>
                            Logout
                        </Button>
                    </div>

                    {/* Subscription Status */}
                    <div className={styles.grid}>
                        <Card className={styles.statusCard}>
                            <h2 className={styles.cardTitle}>Subscription Status</h2>
                            {subscription ? (
                                <>
                                    <div className={styles.planBadge}>
                                        {subscription.plan.toUpperCase()}
                                    </div>
                                    <div className={styles.statusInfo}>
                                        <div className={styles.infoRow}>
                                            <span className={styles.label}>Game:</span>
                                            <span className={styles.value}>Counter-Strike 2</span>
                                        </div>
                                        <div className={styles.infoRow}>
                                            <span className={styles.label}>Status:</span>
                                            <span className={`${styles.value} ${styles.active}`}>
                                                {subscription.status}
                                            </span>
                                        </div>
                                        <div className={styles.infoRow}>
                                            <span className={styles.label}>Expires:</span>
                                            <span className={styles.value}>
                                                {formatDate(subscription.expiresAt)}
                                            </span>
                                        </div>
                                        <div className={styles.infoRow}>
                                            <span className={styles.label}>Days Left:</span>
                                            <span className={styles.value}>{daysLeft} days</span>
                                        </div>
                                    </div>
                                </>
                            ) : (
                                <div className={styles.noSubscription}>
                                    <p>No active subscription</p>
                                    <Button variant="primary" onClick={() => router.push('/pricing')}>
                                        View Plans
                                    </Button>
                                </div>
                            )}
                        </Card>

                        {/* Quick Stats */}
                        <div className={styles.statsGrid}>
                            <Card className={styles.statCard}>
                                <div className={styles.statIcon}>👤</div>
                                <div className={styles.statLabel}>Account</div>
                                <div className={styles.statValue}>{user.username}</div>
                            </Card>

                            <Card className={styles.statCard}>
                                <div className={styles.statIcon}>📅</div>
                                <div className={styles.statLabel}>Member Since</div>
                                <div className={styles.statValue}>
                                    {new Date(user.createdAt).toLocaleDateString('en-US', { month: 'short', year: 'numeric' })}
                                </div>
                            </Card>
                        </div>
                    </div>

                    {/* Downloads */}
                    {subscription && (
                        <div className={styles.downloads}>
                            <h2 className={styles.sectionTitle}>Downloads</h2>
                            <div className={styles.downloadGrid}>
                                <Card hover className={styles.downloadCard}>
                                    <div className={styles.downloadIcon}>📦</div>
                                    <h3 className={styles.downloadTitle}>User Mode</h3>
                                    <p className={styles.downloadDescription}>
                                        External and Internal cheats without kernel driver
                                    </p>
                                    <Button
                                        variant="primary"
                                        onClick={() => handleDownload('usermode')}
                                    >
                                        Download UM
                                    </Button>
                                </Card>

                                <Card hover className={styles.downloadCard}>
                                    <div className={styles.downloadIcon}>🛡️</div>
                                    <h3 className={styles.downloadTitle}>Kernel Mode</h3>
                                    <p className={styles.downloadDescription}>
                                        Maximum protection with kernel driver
                                    </p>
                                    <Button
                                        variant={subscription.plan === 'premium' ? 'primary' : 'secondary'}
                                        onClick={() => handleDownload('kernelmode')}
                                        disabled={subscription.plan !== 'premium'}
                                    >
                                        {subscription.plan === 'premium' ? 'Download KM' : 'Premium Only'}
                                    </Button>
                                </Card>
                            </div>
                        </div>
                    )}
                </div>
            </main>
        </>
    )
}
