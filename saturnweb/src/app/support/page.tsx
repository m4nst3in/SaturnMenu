import Navbar from '@/components/layout/Navbar'
import Card from '@/components/ui/Card'
import Input from '@/components/ui/Input'
import Button from '@/components/ui/Button'
import styles from './support.module.css'

export default function SupportPage() {
    return (
        <>
            <Navbar />
            <main className={styles.main}>
                <div className="container">
                    <h1 className={styles.title}>Support</h1>
                    <p className={styles.subtitle}>
                        Need help? We're here for you 24/7.
                    </p>

                    <div className={styles.grid}>
                        {/* Contact Form */}
                        <Card className={styles.contactCard}>
                            <h2 className={styles.cardTitle}>Send us a message</h2>
                            <form className={styles.form}>
                                <Input
                                    label="Name"
                                    type="text"
                                    placeholder="Your name"
                                    required
                                />
                                <Input
                                    label="Email"
                                    type="email"
                                    placeholder="your@email.com"
                                    required
                                />
                                <div className={styles.inputWrapper}>
                                    <label className={styles.label}>Message</label>
                                    <textarea
                                        className={styles.textarea}
                                        placeholder="Describe your issue..."
                                        rows={6}
                                        required
                                    />
                                </div>
                                <Button variant="primary" size="lg" type="submit">
                                    Send Message
                                </Button>
                            </form>
                        </Card>

                        {/* Quick Links */}
                        <div className={styles.quickLinks}>
                            <Card className={styles.linkCard}>
                                <div className={styles.icon}>💬</div>
                                <h3 className={styles.linkTitle}>Discord</h3>
                                <p className={styles.linkDescription}>
                                    Join our Discord server for instant support and community discussion.
                                </p>
                                <Button variant="secondary" size="sm">
                                    Join Discord
                                </Button>
                            </Card>

                            <Card className={styles.linkCard}>
                                <div className={styles.icon}>📚</div>
                                <h3 className={styles.linkTitle}>Documentation</h3>
                                <p className={styles.linkDescription}>
                                    Check our comprehensive wiki for guides and troubleshooting.
                                </p>
                                <a href="/wiki">
                                    <Button variant="secondary" size="sm">
                                        View Wiki
                                    </Button>
                                </a>
                            </Card>

                            <Card className={styles.linkCard}>
                                <div className={styles.icon}>📧</div>
                                <h3 className={styles.linkTitle}>Email</h3>
                                <p className={styles.linkDescription}>
                                    Prefer email? Reach us at support@saturn.gg
                                </p>
                                <Button variant="secondary" size="sm">
                                    Send Email
                                </Button>
                            </Card>
                        </div>
                    </div>

                    {/* FAQ Section */}
                    <div className={styles.faqSection}>
                        <h2 className={styles.faqTitle}>Frequently Asked Questions</h2>
                        <div className={styles.faqGrid}>
                            <Card>
                                <h3 className={styles.faqQuestion}>How long does support take?</h3>
                                <p className={styles.faqAnswer}>
                                    We typically respond within 1-2 hours on Discord, and within 24 hours via email.
                                </p>
                            </Card>

                            <Card>
                                <h3 className={styles.faqQuestion}>What if I get banned?</h3>
                                <p className={styles.faqAnswer}>
                                    While we use advanced anti-detection, no cheat is 100% safe. We are not responsible for bans.
                                </p>
                            </Card>

                            <Card>
                                <h3 className={styles.faqQuestion}>Can I get a refund?</h3>
                                <p className={styles.faqAnswer}>
                                    We offer refunds within 24 hours of purchase if you haven't downloaded the cheat.
                                </p>
                            </Card>

                            <Card>
                                <h3 className={styles.faqQuestion}>How do I report a bug?</h3>
                                <p className={styles.faqAnswer}>
                                    Report bugs on our Discord server in the #bug-reports channel with detailed information.
                                </p>
                            </Card>
                        </div>
                    </div>
                </div>
            </main>
        </>
    )
}
