import Link from 'next/link'
import Navbar from '@/components/layout/Navbar'
import Button from '@/components/ui/Button'
import Card from '@/components/ui/Card'
import styles from './home.module.css'

export default function Home() {
  return (
    <>
      <Navbar />
      <main className={styles.main}>
        {/* Hero Section */}
        <section className={styles.hero}>
          <div className={styles.heroBackground}></div>
          <div className={styles.heroContent}>
            <h1 className={styles.heroTitle}>
              DOMINATE THE GAME
            </h1>
            <p className={styles.heroSubtitle}>
              Professional-grade cheats for Counter-Strike 2. External, Internal, and Kernel Mode options.
            </p>
            <div className={styles.heroButtons}>
              <Link href="/pricing">
                <Button size="lg">Get Started</Button>
              </Link>
              <Link href="/wiki">
                <Button variant="secondary" size="lg">Learn More</Button>
              </Link>
            </div>
          </div>
        </section>

        {/* Features Section */}
        <section className={styles.features}>
          <div className="container">
            <h2 className={styles.sectionTitle}>Why Choose Saturn?</h2>
            <div className={styles.featuresGrid}>
              <Card hover glow>
                <div className={styles.featureIcon}>🛡️</div>
                <h3 className={styles.featureTitle}>Undetectable</h3>
                <p className={styles.featureDescription}>
                  Advanced anti-cheat bypass with kernel mode driver for maximum security.
                </p>
              </Card>

              <Card hover glow>
                <div className={styles.featureIcon}>⚡</div>
                <h3 className={styles.featureTitle}>High Performance</h3>
                <p className={styles.featureDescription}>
                  Optimized code ensures zero FPS drops and smooth gameplay experience.
                </p>
              </Card>

              <Card hover glow>
                <div className={styles.featureIcon}>🎯</div>
                <h3 className={styles.featureTitle}>Precision Aimbot</h3>
                <p className={styles.featureDescription}>
                  Advanced aimbot with customizable smoothing, FOV, and bone selection.
                </p>
              </Card>

              <Card hover glow>
                <div className={styles.featureIcon}>👁️</div>
                <h3 className={styles.featureTitle}>ESP & Visuals</h3>
                <p className={styles.featureDescription}>
                  Full ESP with player boxes, health bars, weapon info, and more.
                </p>
              </Card>

              <Card hover glow>
                <div className={styles.featureIcon}>🔄</div>
                <h3 className={styles.featureTitle}>Regular Updates</h3>
                <p className={styles.featureDescription}>
                  Frequent updates to stay ahead of anti-cheat and game patches.
                </p>
              </Card>

              <Card hover glow>
                <div className={styles.featureIcon}>💬</div>
                <h3 className={styles.featureTitle}>24/7 Support</h3>
                <p className={styles.featureDescription}>
                  Dedicated support team available around the clock via Discord.
                </p>
              </Card>
            </div>
          </div>
        </section>

        {/* Game Section */}
        <section className={styles.gameSection}>
          <div className="container">
            <h2 className={styles.sectionTitle}>Supported Games</h2>
            <div className={styles.gameGrid}>
              <Card hover className={styles.gameCard}>
                <div className={styles.gameImage}>
                  <div className={styles.gamePlaceholder}>CS2</div>
                </div>
                <h3 className={styles.gameTitle}>Counter-Strike 2</h3>
                <p className={styles.gameDescription}>
                  Full feature set including aimbot, ESP, triggerbot, and more.
                </p>
                <Link href="/pricing">
                  <Button variant="primary" size="sm">View Plans</Button>
                </Link>
              </Card>
            </div>
          </div>
        </section>

        {/* CTA Section */}
        <section className={styles.cta}>
          <div className="container">
            <div className={styles.ctaContent}>
              <h2 className={styles.ctaTitle}>Ready to Dominate?</h2>
              <p className={styles.ctaDescription}>
                Join thousands of players using Saturn to elevate their gameplay.
              </p>
              <Link href="/register">
                <Button size="lg">Get Started Now</Button>
              </Link>
            </div>
          </div>
        </section>
      </main>
    </>
  )
}
