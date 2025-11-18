import Navbar from '@/components/layout/Navbar'
import Card from '@/components/ui/Card'
import styles from './wiki.module.css'

export default function WikiPage() {
    return (
        <>
            <Navbar />
            <main className={styles.main}>
                <div className="container">
                    <h1 className={styles.title}>Documentation</h1>

                    <div className={styles.content}>
                        <aside className={styles.sidebar}>
                            <nav className={styles.nav}>
                                <div className={styles.navSection}>
                                    <h3 className={styles.navTitle}>Getting Started</h3>
                                    <a href="#installation" className={styles.navLink}>Installation</a>
                                    <a href="#first-launch" className={styles.navLink}>First Launch</a>
                                    <a href="#configuration" className={styles.navLink}>Configuration</a>
                                </div>

                                <div className={styles.navSection}>
                                    <h3 className={styles.navTitle}>Features</h3>
                                    <a href="#aimbot" className={styles.navLink}>Aimbot</a>
                                    <a href="#esp" className={styles.navLink}>ESP/Wallhack</a>
                                    <a href="#triggerbot" className={styles.navLink}>Triggerbot</a>
                                    <a href="#misc" className={styles.navLink}>Misc Features</a>
                                </div>

                                <div className={styles.navSection}>
                                    <h3 className={styles.navTitle}>Troubleshooting</h3>
                                    <a href="#common-issues" className={styles.navLink}>Common Issues</a>
                                    <a href="#faq" className={styles.navLink}>FAQ</a>
                                </div>
                            </nav>
                        </aside>

                        <div className={styles.article}>
                            <Card className={styles.section} id="installation">
                                <h2 className={styles.sectionTitle}>Installation</h2>
                                <p className={styles.text}>
                                    After purchasing a subscription, you can download the loader from your dashboard.
                                </p>
                                <ol className={styles.list}>
                                    <li>Login to your account</li>
                                    <li>Navigate to the Dashboard</li>
                                    <li>Click "Download" for your desired cheat type (External/Internal)</li>
                                    <li>Extract the downloaded file to a folder</li>
                                    <li>Run the loader as Administrator</li>
                                </ol>
                            </Card>

                            <Card className={styles.section} id="first-launch">
                                <h2 className={styles.sectionTitle}>First Launch</h2>
                                <p className={styles.text}>
                                    When you first launch the loader, you'll need to authenticate with your Saturn account credentials.
                                </p>
                                <ol className={styles.list}>
                                    <li>Enter your email and password</li>
                                    <li>Select Counter-Strike 2 from the game list</li>
                                    <li>Choose your cheat type (External or Internal)</li>
                                    <li>Click "Inject" to load the cheat</li>
                                </ol>
                            </Card>

                            <Card className={styles.section} id="configuration">
                                <h2 className={styles.sectionTitle}>Configuration</h2>
                                <p className={styles.text}>
                                    The cheat menu can be opened in-game using the <code className={styles.code}>INSERT</code> key.
                                    All settings are saved automatically.
                                </p>
                            </Card>

                            <Card className={styles.section} id="aimbot">
                                <h2 className={styles.sectionTitle}>Aimbot</h2>
                                <p className={styles.text}>
                                    Our aimbot features advanced smoothing and prediction algorithms for natural-looking aim.
                                </p>
                                <ul className={styles.list}>
                                    <li><strong>Smoothing:</strong> Adjust how smooth the aim movement is (1-100)</li>
                                    <li><strong>FOV:</strong> Field of view for target acquisition</li>
                                    <li><strong>Bone Selection:</strong> Choose which body part to target (head, chest, etc.)</li>
                                    <li><strong>Visibility Check:</strong> Only aim at visible targets</li>
                                </ul>
                            </Card>

                            <Card className={styles.section} id="esp">
                                <h2 className={styles.sectionTitle}>ESP/Wallhack</h2>
                                <p className={styles.text}>
                                    ESP (Extra Sensory Perception) displays information about players through walls.
                                </p>
                                <ul className={styles.list}>
                                    <li><strong>Player Boxes:</strong> Draw boxes around players</li>
                                    <li><strong>Health Bars:</strong> Display player health</li>
                                    <li><strong>Name ESP:</strong> Show player names</li>
                                    <li><strong>Weapon ESP:</strong> Display current weapon</li>
                                    <li><strong>Distance:</strong> Show distance to players</li>
                                </ul>
                            </Card>

                            <Card className={styles.section} id="triggerbot">
                                <h2 className={styles.sectionTitle}>Triggerbot</h2>
                                <p className={styles.text}>
                                    Automatically shoots when your crosshair is over an enemy.
                                </p>
                                <ul className={styles.list}>
                                    <li><strong>Delay:</strong> Delay before shooting (in milliseconds)</li>
                                    <li><strong>Hitbox:</strong> Which hitbox to trigger on</li>
                                    <li><strong>Team Check:</strong> Don't shoot teammates</li>
                                </ul>
                            </Card>

                            <Card className={styles.section} id="common-issues">
                                <h2 className={styles.sectionTitle}>Common Issues</h2>

                                <div className={styles.issue}>
                                    <h3 className={styles.issueTitle}>Loader won't start</h3>
                                    <p className={styles.text}>
                                        Make sure you're running the loader as Administrator. Disable your antivirus temporarily.
                                    </p>
                                </div>

                                <div className={styles.issue}>
                                    <h3 className={styles.issueTitle}>Injection failed</h3>
                                    <p className={styles.text}>
                                        Ensure Counter-Strike 2 is running before injecting. Try restarting both the game and loader.
                                    </p>
                                </div>

                                <div className={styles.issue}>
                                    <h3 className={styles.issueTitle}>Menu won't open</h3>
                                    <p className={styles.text}>
                                        Press the INSERT key. If that doesn't work, check if another program is using that key.
                                    </p>
                                </div>
                            </Card>
                        </div>
                    </div>
                </div>
            </main>
        </>
    )
}
