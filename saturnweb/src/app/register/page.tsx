'use client'

import { useState } from 'react'
import Link from 'next/link'
import { useRouter } from 'next/navigation'
import Card from '@/components/ui/Card'
import Input from '@/components/ui/Input'
import Button from '@/components/ui/Button'
import { validateEmail, validatePassword, validateUsername } from '@/lib/auth-utils'
import styles from './register.module.css'

export default function RegisterPage() {
    const router = useRouter()
    const [email, setEmail] = useState('')
    const [username, setUsername] = useState('')
    const [password, setPassword] = useState('')
    const [confirmPassword, setConfirmPassword] = useState('')
    const [errors, setErrors] = useState<Record<string, string>>({})
    const [loading, setLoading] = useState(false)

    const validate = () => {
        const newErrors: Record<string, string> = {}

        if (!validateEmail(email)) {
            newErrors.email = 'Invalid email address'
        }

        const usernameValidation = validateUsername(username)
        if (!usernameValidation.valid) {
            newErrors.username = usernameValidation.message!
        }

        const passwordValidation = validatePassword(password)
        if (!passwordValidation.valid) {
            newErrors.password = passwordValidation.message!
        }

        if (password !== confirmPassword) {
            newErrors.confirmPassword = 'Passwords do not match'
        }

        setErrors(newErrors)
        return Object.keys(newErrors).length === 0
    }

    const handleSubmit = async (e: React.FormEvent) => {
        e.preventDefault()

        if (!validate()) return

        setLoading(true)

        try {
            const res = await fetch('/api/auth/register', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ email, username, password }),
            })

            const data = await res.json()

            if (!res.ok) {
                throw new Error(data.message || 'Registration failed')
            }

            // Redirect to login
            router.push('/login?registered=true')
        } catch (err: any) {
            setErrors({ general: err.message })
        } finally {
            setLoading(false)
        }
    }

    return (
        <div className={styles.container}>
            <div className={styles.background}></div>

            <Card className={styles.card}>
                <div className={styles.header}>
                    <Link href="/" className={styles.logo}>
                        SATURN
                    </Link>
                    <h1 className={styles.title}>Create Account</h1>
                    <p className={styles.subtitle}>Join thousands of players using Saturn</p>
                </div>

                <form onSubmit={handleSubmit} className={styles.form}>
                    {errors.general && (
                        <div className={styles.error}>
                            {errors.general}
                        </div>
                    )}

                    <Input
                        label="Email"
                        type="email"
                        placeholder="your@email.com"
                        value={email}
                        onChange={(e) => setEmail(e.target.value)}
                        error={errors.email}
                        required
                    />

                    <Input
                        label="Username"
                        type="text"
                        placeholder="username"
                        value={username}
                        onChange={(e) => setUsername(e.target.value)}
                        error={errors.username}
                        required
                    />

                    <Input
                        label="Password"
                        type="password"
                        placeholder="••••••••"
                        value={password}
                        onChange={(e) => setPassword(e.target.value)}
                        error={errors.password}
                        required
                    />

                    <Input
                        label="Confirm Password"
                        type="password"
                        placeholder="••••••••"
                        value={confirmPassword}
                        onChange={(e) => setConfirmPassword(e.target.value)}
                        error={errors.confirmPassword}
                        required
                    />

                    <div className={styles.passwordHints}>
                        <p className={styles.hintTitle}>Password must contain:</p>
                        <ul className={styles.hintList}>
                            <li className={password.length >= 8 ? styles.valid : ''}>
                                At least 8 characters
                            </li>
                            <li className={/[A-Z]/.test(password) ? styles.valid : ''}>
                                One uppercase letter
                            </li>
                            <li className={/[a-z]/.test(password) ? styles.valid : ''}>
                                One lowercase letter
                            </li>
                            <li className={/[0-9]/.test(password) ? styles.valid : ''}>
                                One number
                            </li>
                        </ul>
                    </div>

                    <Button
                        type="submit"
                        variant="primary"
                        size="lg"
                        disabled={loading}
                        className={styles.submitButton}
                    >
                        {loading ? 'Creating account...' : 'Create Account'}
                    </Button>
                </form>

                <div className={styles.footer}>
                    <p className={styles.footerText}>
                        Already have an account?{' '}
                        <Link href="/login" className={styles.link}>
                            Login here
                        </Link>
                    </p>
                </div>
            </Card>
        </div>
    )
}
