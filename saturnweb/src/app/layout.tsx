import type { Metadata } from "next";
import { Inter, Rajdhani } from "next/font/google";
import "./globals.css";

const inter = Inter({
  variable: "--font-sans",
  subsets: ["latin"],
});

const rajdhani = Rajdhani({
  variable: "--font-display",
  subsets: ["latin"],
  weight: ["400", "500", "600", "700"],
});

export const metadata: Metadata = {
  title: "Saturn - Premium Game Cheats",
  description: "Professional cheat provider for Counter-Strike 2 with external, internal, and kernel mode options.",
  keywords: ["CS2", "Counter-Strike 2", "cheats", "hacks", "Saturn"],
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="en">
      <body className={`${inter.variable} ${rajdhani.variable}`}>
        {children}
      </body>
    </html>
  );
}
