'use client'

import Link from 'next/link'
import { usePathname } from 'next/navigation'
import styles from './Navbar.module.css'
import Button from '../ui/Button'

export default function Navbar() {
    const pathname = usePathname()

    const isActive = (path: string) => pathname === path

    return (
        <nav className={styles.navbar}>
            <div className={styles.container}>
                <Link href="/" className={styles.logo}>
                    <span className={styles.logoText}>SATURN</span>
                </Link>

                <div className={styles.links}>
                    <Link
                        href="/"
                        className={`${styles.link} ${isActive('/') ? styles.active : ''}`}
                    >
                        Home
                    </Link>
                    <Link
                        href="/pricing"
                        className={`${styles.link} ${isActive('/pricing') ? styles.active : ''}`}
                    >
                        Pricing
                    </Link>
                    <Link
                        href="/wiki"
                        className={`${styles.link} ${isActive('/wiki') ? styles.active : ''}`}
                    >
                        Wiki
                    </Link>
                    <Link
                        href="/support"
                        className={`${styles.link} ${isActive('/support') ? styles.active : ''}`}
                    >
                        Support
                    </Link>
                </div>

                <div className={styles.actions}>
                    <Link href="/login">
                        <Button variant="ghost" size="sm">Login</Button>
                    </Link>
                    <Link href="/register">
                        <Button variant="primary" size="sm">Get Started</Button>
                    </Link>
                </div>
            </div>
        </nav>
    )
}
