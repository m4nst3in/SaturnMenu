import { NextRequest, NextResponse } from 'next/server'
import { jwtVerify } from 'jose'
import { prisma } from '@/lib/db'

const JWT_SECRET = new TextEncoder().encode(
    process.env.NEXTAUTH_SECRET || 'saturn-secret-key-change-in-production-2024'
)

export async function GET(request: NextRequest) {
    try {
        const token = request.cookies.get('saturn-token')?.value

        console.log('Session check - Token present:', !!token)

        if (!token) {
            return NextResponse.json(
                { authenticated: false, message: 'No token provided' },
                { status: 401 }
            )
        }

        // Verify token
        const { payload } = await jwtVerify(token, JWT_SECRET)

        // Get user with subscription
        const user = await prisma.user.findUnique({
            where: { id: payload.userId as string },
            include: {
                subscriptions: {
                    where: { status: 'active' },
                    orderBy: { createdAt: 'desc' },
                    take: 1
                }
            }
        })

        if (!user) {
            return NextResponse.json(
                { authenticated: false, message: 'User not found' },
                { status: 401 }
            )
        }

        return NextResponse.json({
            authenticated: true,
            user: {
                id: user.id,
                email: user.email,
                username: user.username,
                createdAt: user.createdAt,
                subscription: user.subscriptions[0] || null
            }
        })

    } catch (error) {
        console.error('Session verification error:', error)
        return NextResponse.json(
            { authenticated: false, message: 'Invalid or expired token' },
            { status: 401 }
        )
    }
}
