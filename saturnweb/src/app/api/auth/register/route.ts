import { NextRequest, NextResponse } from 'next/server'
import { prisma } from '@/lib/db'
import { hashPassword, validateEmail, validatePassword, validateUsername } from '@/lib/auth-utils'

export async function POST(request: NextRequest) {
    try {
        const body = await request.json()
        const { email, username, password } = body

        // Validate input
        if (!email || !username || !password) {
            return NextResponse.json(
                { success: false, message: 'Missing required fields' },
                { status: 400 }
            )
        }

        // Validate email
        if (!validateEmail(email)) {
            return NextResponse.json(
                { success: false, message: 'Invalid email address' },
                { status: 400 }
            )
        }

        // Validate username
        const usernameValidation = validateUsername(username)
        if (!usernameValidation.valid) {
            return NextResponse.json(
                { success: false, message: usernameValidation.message },
                { status: 400 }
            )
        }

        // Validate password
        const passwordValidation = validatePassword(password)
        if (!passwordValidation.valid) {
            return NextResponse.json(
                { success: false, message: passwordValidation.message },
                { status: 400 }
            )
        }

        // Check if user already exists
        const existingUser = await prisma.user.findFirst({
            where: {
                OR: [
                    { email: email.toLowerCase() },
                    { username: username.toLowerCase() }
                ]
            }
        })

        if (existingUser) {
            return NextResponse.json(
                { success: false, message: 'Email or username already exists' },
                { status: 409 }
            )
        }

        // Hash password
        const hashedPassword = await hashPassword(password)

        // Create user
        const user = await prisma.user.create({
            data: {
                email: email.toLowerCase(),
                username: username.toLowerCase(),
                password: hashedPassword,
            },
            select: {
                id: true,
                email: true,
                username: true,
                createdAt: true,
            }
        })

        return NextResponse.json({
            success: true,
            message: 'Account created successfully',
            user
        }, { status: 201 })

    } catch (error) {
        console.error('Registration error:', error)
        return NextResponse.json(
            { success: false, message: 'Internal server error' },
            { status: 500 }
        )
    }
}
