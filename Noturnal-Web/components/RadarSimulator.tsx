import React, { useEffect, useRef } from 'react';

export const RadarSimulator: React.FC = () => {
  const canvasRef = useRef<HTMLCanvasElement>(null);

  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;
    const ctx = canvas.getContext('2d');
    if (!ctx) return;

    let animationFrameId: number;

    // Radar settings
    const centerX = canvas.width / 2;
    const centerY = canvas.height / 2;
    const radius = canvas.width / 2 - 10;
    let angle = 0;

    // Entities
    const enemies: { x: number; y: number; opacity: number }[] = [];
    // Initial random enemies
    for (let i = 0; i < 4; i++) {
      enemies.push({
        x: centerX + (Math.random() - 0.5) * radius * 1.4,
        y: centerY + (Math.random() - 0.5) * radius * 1.4,
        opacity: 0.2 + Math.random() * 0.8
      });
    }

    const draw = () => {
      // Clear
      ctx.clearRect(0, 0, canvas.width, canvas.height);

      // Draw Radar Background
      ctx.fillStyle = 'rgba(15, 15, 20, 0.9)';
      ctx.strokeStyle = 'rgba(139, 92, 246, 0.4)'; // Noturnal purple
      ctx.lineWidth = 2;
      
      ctx.beginPath();
      ctx.arc(centerX, centerY, radius, 0, Math.PI * 2);
      ctx.fill();
      ctx.stroke();

      // Concentric circles
      ctx.strokeStyle = 'rgba(139, 92, 246, 0.2)';
      ctx.beginPath();
      ctx.arc(centerX, centerY, radius * 0.66, 0, Math.PI * 2);
      ctx.stroke();
      
      ctx.beginPath();
      ctx.arc(centerX, centerY, radius * 0.33, 0, Math.PI * 2);
      ctx.stroke();

      // Crosshair
      ctx.beginPath();
      ctx.moveTo(centerX - radius, centerY);
      ctx.lineTo(centerX + radius, centerY);
      ctx.moveTo(centerX, centerY - radius);
      ctx.lineTo(centerX, centerY + radius);
      ctx.stroke();

      // Draw Sweep
      angle += 0.04;
      ctx.save();
      ctx.translate(centerX, centerY);
      ctx.rotate(angle);
      
      const gradient = ctx.createLinearGradient(0, 0, radius, 0);
      gradient.addColorStop(0, 'rgba(139, 92, 246, 0)');
      gradient.addColorStop(1, 'rgba(139, 92, 246, 0.6)');
      
      ctx.fillStyle = gradient;
      ctx.beginPath();
      ctx.moveTo(0, 0);
      ctx.arc(0, 0, radius, 0, 0.5); // Slice size
      ctx.lineTo(0, 0);
      ctx.fill();
      ctx.restore();

      // Player Arrow
      ctx.save();
      ctx.translate(centerX, centerY);
      ctx.fillStyle = '#fff';
      ctx.shadowBlur = 10;
      ctx.shadowColor = '#fff';
      ctx.beginPath();
      ctx.moveTo(0, -6);
      ctx.lineTo(5, 6);
      ctx.lineTo(0, 4);
      ctx.lineTo(-5, 6);
      ctx.closePath();
      ctx.fill();
      ctx.restore();

      // Draw Enemies
      enemies.forEach(enemy => {
        // Draw enemy dot
        ctx.fillStyle = `rgba(239, 68, 68, ${enemy.opacity})`; // Red
        ctx.shadowBlur = 5;
        ctx.shadowColor = 'rgba(239, 68, 68, 0.8)';
        ctx.beginPath();
        ctx.arc(enemy.x, enemy.y, 3, 0, Math.PI * 2);
        ctx.fill();
        ctx.shadowBlur = 0;

        // ESP Box effect simulation (faint box around dot)
        ctx.strokeStyle = `rgba(239, 68, 68, ${enemy.opacity * 0.5})`;
        ctx.lineWidth = 1;
        ctx.strokeRect(enemy.x - 5, enemy.y - 8, 10, 16);
        
        // Random subtle movement
        enemy.x += (Math.random() - 0.5) * 0.2;
        enemy.y += (Math.random() - 0.5) * 0.2;

        // Constrain to circle
        const dx = enemy.x - centerX;
        const dy = enemy.y - centerY;
        if (Math.sqrt(dx*dx + dy*dy) > radius - 5) {
             enemy.x = centerX; 
             enemy.y = centerY; 
        }
      });

      // Text Overlay
      ctx.font = '10px JetBrains Mono';
      ctx.fillStyle = 'rgba(139, 92, 246, 0.8)';
      ctx.fillText('RADAR_NET: ONLINE', centerX - 40, centerY + radius + 15);

      animationFrameId = requestAnimationFrame(draw);
    };

    draw();

    return () => cancelAnimationFrame(animationFrameId);
  }, []);

  return (
    <div className="relative inline-block p-1 rounded-full bg-gradient-to-b from-noturnal-500/20 to-transparent backdrop-blur-md">
       <canvas ref={canvasRef} width={240} height={240} className="rounded-full" />
    </div>
  );
};