import { GoogleGenAI } from "@google/genai";

const SYSTEM_INSTRUCTION = `
You are the official support assistant and wiki for "Noturnal Menu", a cheat software for CS2.
Your persona is technical, helpful, and slightly "hacker/cyberpunk".
Answer questions about the cheat.
Information about Noturnal Menu:
- Supports only Counter-Strike 2 currently.
- Basic Plan ($3): External and simple Internal access. No Kernel Driver.
- Premium Plan ($8): Full access, Kernel Mode Driver (Ring 0) for maximum security, HWID Spoofer.
- Features: Aimbot (Legit/Rage), ESP (Box, Skeleton, Health), Triggerbot, RCS (Recoil Control System), Skin Changer.
- Installation: Download the loader, log in, select CS2, and click Inject. For Premium, restart PC to load the driver.
- Menu key: Insert or Delete.
- The cheat is "Undetected".

If the user asks about other games, say the current focus is dominating CS2, but news will come.
Respond in English.
`;

export const generateWikiResponse = async (userQuestion: string): Promise<string> => {
  try {
    const apiKey = process.env.API_KEY;
    if (!apiKey) {
      return "Error: API Key not configured in environment. Contact administrator.";
    }

    const ai = new GoogleGenAI({ apiKey });
    
    const response = await ai.models.generateContent({
      model: 'gemini-2.5-flash',
      contents: userQuestion,
      config: {
        systemInstruction: SYSTEM_INSTRUCTION,
        temperature: 0.7,
      }
    });

    return response.text || "No response from server.";
  } catch (error) {
    console.error("Gemini API Error:", error);
    return "Sorry, my systems are temporarily offline. Try again later.";
  }
};