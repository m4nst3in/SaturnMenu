import React, { useState, useEffect } from 'react';

interface TextScrambleProps {
  text: string;
  className?: string;
  duration?: number;
  speed?: number;
}

const CHARS = "!@#$%^&*()_+-=[]{}|;:,.<>?/~AZ0129";

export const TextScramble: React.FC<TextScrambleProps> = ({ 
  text, 
  className = "", 
  duration = 2000,
  speed = 50 
}) => {
  const [displayText, setDisplayText] = useState("");
  const [isHovered, setIsHovered] = useState(false);

  const scramble = () => {
    let iterations = 0;
    
    const interval = setInterval(() => {
      setDisplayText(prev => 
        text.split("").map((char, index) => {
          if (index < iterations) {
            return text[index];
          }
          return CHARS[Math.floor(Math.random() * CHARS.length)];
        }).join("")
      );

      if (iterations >= text.length) {
        clearInterval(interval);
      }

      iterations += 1/3; 
    }, speed);

    return interval;
  };

  useEffect(() => {
    const interval = scramble();
    return () => clearInterval(interval);
  }, [text]);

  // Re-trigger on hover if desired
  const handleMouseEnter = () => {
    if (!isHovered) {
      setIsHovered(true);
      scramble();
      setTimeout(() => setIsHovered(false), duration);
    }
  };

  return (
    <span className={`font-mono ${className}`} onMouseEnter={handleMouseEnter}>
      {displayText}
    </span>
  );
};