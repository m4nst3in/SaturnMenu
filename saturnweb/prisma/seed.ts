import { PrismaClient } from '@prisma/client'
import { hashPassword } from '../src/lib/auth-utils'

const prisma = new PrismaClient()

async function main() {
    console.log('Seeding database...')

    // Create test user with Basic subscription
    const basicUser = await prisma.user.create({
        data: {
            email: 'basic@test.com',
            username: 'basicuser',
            password: await hashPassword('Test123!'),
            subscriptions: {
                create: {
                    plan: 'basic',
                    game: 'cs2',
                    status: 'active',
                    expiresAt: new Date(Date.now() + 30 * 24 * 60 * 60 * 1000), // 30 days from now
                }
            }
        }
    })

    // Create test user with Premium subscription
    const premiumUser = await prisma.user.create({
        data: {
            email: 'premium@test.com',
            username: 'premiumuser',
            password: await hashPassword('Test123!'),
            subscriptions: {
                create: {
                    plan: 'premium',
                    game: 'cs2',
                    status: 'active',
                    expiresAt: new Date(Date.now() + 30 * 24 * 60 * 60 * 1000), // 30 days from now
                }
            }
        }
    })

    // Create test user without subscription
    const noSubUser = await prisma.user.create({
        data: {
            email: 'nosub@test.com',
            username: 'nosubuser',
            password: await hashPassword('Test123!'),
        }
    })

    console.log('✓ Created test users:')
    console.log('  Basic User: basic@test.com / Test123!')
    console.log('  Premium User: premium@test.com / Test123!')
    console.log('  No Sub User: nosub@test.com / Test123!')
    console.log('')
    console.log('Database seeded successfully!')
}

main()
    .catch((e) => {
        console.error('Error seeding database:', e)
        process.exit(1)
    })
    .finally(async () => {
        await prisma.$disconnect()
    })
