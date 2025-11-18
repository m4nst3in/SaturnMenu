import { NextRequest, NextResponse } from 'next/server'
import { jwtVerify } from 'jose'
import { prisma } from '@/lib/db'
import path from 'path'
import fs from 'fs'

const JWT_SECRET = new TextEncoder().encode(
    process.env.NEXTAUTH_SECRET || 'saturn-secret-key-change-in-production-2024'
)

export async function GET(request: NextRequest) {
    try {
        // Get token from header or cookie
        const authHeader = request.headers.get('authorization')
        const token = authHeader?.replace('Bearer ', '') || request.cookies.get('saturn-token')?.value

        if (!token) {
            return NextResponse.json(
                { success: false, message: 'Unauthorized' },
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
                    where: {
                        status: 'active',
                        expiresAt: { gte: new Date() }
                    },
                    orderBy: { createdAt: 'desc' },
                    take: 1
                }
            }
        })

        if (!user || !user.subscriptions[0]) {
            return NextResponse.json(
                { success: false, message: 'No active subscription' },
                { status: 403 }
            )
        }

        const subscription = user.subscriptions[0]

        // Log download
        await prisma.downloadLog.create({
            data: {
                userId: user.id,
                downloadType: 'usermode',
                game: subscription.game,
                ipAddress: request.headers.get('x-forwarded-for') || request.ip || 'unknown'
            }
        })

        // In production, serve the actual file
        // For now, return a placeholder response
        return NextResponse.json({
            success: true,
            message: 'Download authorized',
            downloadUrl: '/downloads/usermode.exe',
            subscription: {
                plan: subscription.plan,
                expiresAt: subscription.expiresAt
            }
        })

    } catch (error) {
        console.error('Download error:', error)
        return NextResponse.json(
            { success: false, message: 'Download failed' },
            { status: 500 }
        )
    }
}
