import { NextRequest, NextResponse } from 'next/server'
import { jwtVerify } from 'jose'
import { prisma } from '@/lib/db'

const JWT_SECRET = new TextEncoder().encode(
    process.env.NEXTAUTH_SECRET || 'saturn-secret-key-change-in-production-2024'
)

export async function POST(request: NextRequest) {
    try {
        const body = await request.json()
        const { token, game } = body

        if (!token) {
            return NextResponse.json(
                { valid: false, message: 'No token provided' },
                { status: 401 }
            )
        }

        // Verify token
        const { payload } = await jwtVerify(token, JWT_SECRET)

        // Get user with active subscription
        const user = await prisma.user.findUnique({
            where: { id: payload.userId as string },
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

        if (!user || !user.subscriptions[0]) {
            return NextResponse.json(
                { valid: false, message: 'Invalid token or no active subscription' },
                { status: 401 }
            )
        }

        const subscription = user.subscriptions[0]

        // Determine permissions based on plan
        const permissions = {
            external: true,
            internal: true,
            kernelMode: subscription.plan === 'premium'
        }

        return NextResponse.json({
            valid: true,
            user: {
                id: user.id,
                username: user.username,
                email: user.email
            },
            subscription: {
                plan: subscription.plan,
                game: subscription.game,
                status: subscription.status,
                expiresAt: subscription.expiresAt
            },
            permissions
        })

    } catch (error) {
        console.error('Token verification error:', error)
        return NextResponse.json(
            { valid: false, message: 'Invalid or expired token' },
            { status: 401 }
        )
    }
}
