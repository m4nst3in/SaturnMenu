import Navbar from '@/components/layout/Navbar'
import Button from '@/components/ui/Button'
import Card from '@/components/ui/Card'
import Link from 'next/link'
import styles from './pricing.module.css'

export default function PricingPage() {
    return (
        <>
            <Navbar />
            <main className={styles.main}>
                <div className="container">
                    <div className={styles.header}>
                        <h1 className={styles.title}>Choose Your Plan</h1>
                        <p className={styles.subtitle}>
                            Select the perfect plan for your gaming needs. All plans include lifetime updates.
                        </p>
                    </div>

                    <div className={styles.pricingGrid}>
                        {/* Basic Plan */}
                        <Card hover className={styles.pricingCard}>
                            <div className={styles.planHeader}>
                                <h3 className={styles.planName}>Basic</h3>
                                <div className={styles.planPrice}>
                                    <span className={styles.currency}>$</span>
                                    <span className={styles.amount}>3</span>
                                    <span className={styles.period}>/month</span>
                                </div>
                            </div>

                            <div className={styles.planFeatures}>
                                <div className={styles.feature}>
                                    <span className={styles.checkmark}>✓</span>
                                    <span>External Cheat Access</span>
                                </div>
                                <div className={styles.feature}>
                                    <span className={styles.checkmark}>✓</span>
                                    <span>Internal Cheat Access</span>
                                </div>
                                <div className={styles.feature}>
                                    <span className={styles.checkmark}>✓</span>
                                    <span>Aimbot & ESP</span>
                                </div>
                                <div className={styles.feature}>
                                    <span className={styles.checkmark}>✓</span>
                                    <span>Triggerbot</span>
                                </div>
                                <div className={styles.feature}>
                                    <span className={styles.checkmark}>✓</span>
                                    <span>Regular Updates</span>
                                </div>
                                <div className={styles.feature}>
                                    <span className={styles.checkmark}>✓</span>
                                    <span>Discord Support</span>
                                </div>
                                <div className={`${styles.feature} ${styles.disabled}`}>
                                    <span className={styles.crossmark}>✗</span>
                                    <span>Kernel Mode Driver</span>
                                </div>
                            </div>

                            <Link href="/register">
                                <Button variant="secondary" size="lg" className={styles.planButton}>
                                    Get Started
                                </Button>
                            </Link>
                        </Card>

                        {/* Premium Plan */}
                        <Card hover glow className={`${styles.pricingCard} ${styles.featured}`}>
                            <div className={styles.badge}>RECOMMENDED</div>
                            <div className={styles.planHeader}>
                                <h3 className={styles.planName}>Premium</h3>
                                <div className={styles.planPrice}>
                                    <span className={styles.currency}>$</span>
                                    <span className={styles.amount}>8</span>
                                    <span className={styles.period}>/month</span>
                                </div>
                            </div>

                            <div className={styles.planFeatures}>
                                <div className={styles.feature}>
                                    <span className={styles.checkmark}>✓</span>
                                    <span>External Cheat Access</span>
                                </div>
                                <div className={styles.feature}>
                                    <span className={styles.checkmark}>✓</span>
                                    <span>Internal Cheat Access</span>
                                </div>
                                <div className={styles.feature}>
                                    <span className={styles.checkmark}>✓</span>
                                    <span>Aimbot & ESP</span>
                                </div>
                                <div className={styles.feature}>
                                    <span className={styles.checkmark}>✓</span>
                                    <span>Triggerbot</span>
                                </div>
                                <div className={styles.feature}>
                                    <span className={styles.checkmark}>✓</span>
                                    <span>Regular Updates</span>
                                </div>
                                <div className={styles.feature}>
                                    <span className={styles.checkmark}>✓</span>
                                    <span>Priority Discord Support</span>
                                </div>
                                <div className={styles.feature}>
                                    <span className={styles.checkmark}>✓</span>
                                    <span><strong>Kernel Mode Driver</strong></span>
                                </div>
                                <div className={styles.feature}>
                                    <span className={styles.checkmark}>✓</span>
                                    <span><strong>Maximum Protection</strong></span>
                                </div>
                            </div>

                            <Link href="/register">
                                <Button variant="primary" size="lg" className={styles.planButton}>
                                    Get Premium
                                </Button>
                            </Link>
                        </Card>
                    </div>

                    {/* Feature Comparison */}
                    <div className={styles.comparison}>
                        <h2 className={styles.comparisonTitle}>Feature Comparison</h2>
                        <div className={styles.comparisonTable}>
                            <div className={styles.tableHeader}>
                                <div className={styles.tableCell}>Feature</div>
                                <div className={styles.tableCell}>Basic</div>
                                <div className={styles.tableCell}>Premium</div>
                            </div>

                            <div className={styles.tableRow}>
                                <div className={styles.tableCell}>External Cheat</div>
                                <div className={styles.tableCell}>✓</div>
                                <div className={styles.tableCell}>✓</div>
                            </div>

                            <div className={styles.tableRow}>
                                <div className={styles.tableCell}>Internal Cheat</div>
                                <div className={styles.tableCell}>✓</div>
                                <div className={styles.tableCell}>✓</div>
                            </div>

                            <div className={styles.tableRow}>
                                <div className={styles.tableCell}>Kernel Mode Driver</div>
                                <div className={styles.tableCell}>✗</div>
                                <div className={styles.tableCell}>✓</div>
                            </div>

                            <div className={styles.tableRow}>
                                <div className={styles.tableCell}>Aimbot</div>
                                <div className={styles.tableCell}>✓</div>
                                <div className={styles.tableCell}>✓</div>
                            </div>

                            <div className={styles.tableRow}>
                                <div className={styles.tableCell}>ESP/Wallhack</div>
                                <div className={styles.tableCell}>✓</div>
                                <div className={styles.tableCell}>✓</div>
                            </div>

                            <div className={styles.tableRow}>
                                <div className={styles.tableCell}>Triggerbot</div>
                                <div className={styles.tableCell}>✓</div>
                                <div className={styles.tableCell}>✓</div>
                            </div>

                            <div className={styles.tableRow}>
                                <div className={styles.tableCell}>Support</div>
                                <div className={styles.tableCell}>Standard</div>
                                <div className={styles.tableCell}>Priority</div>
                            </div>
                        </div>
                    </div>

                    {/* FAQ */}
                    <div className={styles.faq}>
                        <h2 className={styles.faqTitle}>Frequently Asked Questions</h2>
                        <div className={styles.faqGrid}>
                            <Card className={styles.faqCard}>
                                <h3 className={styles.faqQuestion}>What's the difference between plans?</h3>
                                <p className={styles.faqAnswer}>
                                    The Premium plan includes our Kernel Mode Driver for maximum protection against anti-cheat systems, while Basic provides external and internal cheats without kernel-level access.
                                </p>
                            </Card>

                            <Card className={styles.faqCard}>
                                <h3 className={styles.faqQuestion}>Can I upgrade later?</h3>
                                <p className={styles.faqAnswer}>
                                    Yes! You can upgrade from Basic to Premium at any time from your dashboard.
                                </p>
                            </Card>

                            <Card className={styles.faqCard}>
                                <h3 className={styles.faqQuestion}>How do I cancel?</h3>
                                <p className={styles.faqAnswer}>
                                    You can cancel your subscription at any time from your dashboard. You'll retain access until the end of your billing period.
                                </p>
                            </Card>

                            <Card className={styles.faqCard}>
                                <h3 className={styles.faqQuestion}>Is it safe to use?</h3>
                                <p className={styles.faqAnswer}>
                                    We use advanced anti-detection techniques and update regularly. However, no cheat is 100% undetectable. Use at your own risk.
                                </p>
                            </Card>
                        </div>
                    </div>
                </div>
            </main>
        </>
    )
}
