import { NextRequest, NextResponse } from 'next/server'
import { prisma } from '@/lib/db'
import { verifyPassword } from '@/lib/auth-utils'
import { SignJWT } from 'jose'

const JWT_SECRET = new TextEncoder().encode(
    process.env.NEXTAUTH_SECRET || 'saturn-secret-key-change-in-production-2024'
)

export async function POST(request: NextRequest) {
    try {
        const body = await request.json()
        const { email, password, game } = body

        // Validate input
        if (!email || !password) {
            return NextResponse.json(
                { success: false, message: 'Missing email or password' },
                { status: 400 }
            )
        }

        // Find user
        const user = await prisma.user.findUnique({
            where: { email: email.toLowerCase() },
            include: {
                subscriptions: {
                    where: {
                        status: 'active',
                        game: game || 'cs2',
                        expiresAt: { gte: new Date() }
                    },
                    orderBy: { createdAt: 'desc' },
                    take: 1
                }
            }
        })

        if (!user) {
            return NextResponse.json(
                { success: false, message: 'Invalid credentials' },
                { status: 401 }
            )
        }

        // Verify password
        const isValid = await verifyPassword(password, user.password)
        if (!isValid) {
            return NextResponse.json(
                { success: false, message: 'Invalid credentials' },
                { status: 401 }
            )
        }

        // Check subscription
        const subscription = user.subscriptions[0]
        if (!subscription) {
            return NextResponse.json(
                { success: false, message: 'No active subscription found' },
                { status: 403 }
            )
        }

        // Create JWT token for loader
        const token = await new SignJWT({
            userId: user.id,
            email: user.email,
            username: user.username,
            plan: subscription.plan,
            game: subscription.game
        })
            .setProtectedHeader({ alg: 'HS256' })
            .setIssuedAt()
            .setExpirationTime('24h')
            .sign(JWT_SECRET)

        return NextResponse.json({
            success: true,
            message: 'Authentication successful',
            token,
            user: {
                id: user.id,
                email: user.email,
                username: user.username
            },
            subscription: {
                plan: subscription.plan,
                game: subscription.game,
                expiresAt: subscription.expiresAt,
                status: subscription.status
            }
        })

    } catch (error) {
        console.error('Loader login error:', error)
        return NextResponse.json(
            { success: false, message: 'Internal server error' },
            { status: 500 }
        )
    }
}
