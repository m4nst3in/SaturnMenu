#!/usr/bin/env python3
"""
Script para converter o driver .sys em array de bytes para cfg.h
Uso: python convert_driver.py driver.sys
"""

import sys
import os

def convert_driver_to_array(driver_path, output_path=None):
    """Converte o arquivo .sys em um array de bytes C++"""
    
    if not os.path.exists(driver_path):
        print(f"Erro: Arquivo não encontrado: {driver_path}")
        return False
    
    with open(driver_path, 'rb') as f:
        driver_data = f.read()
    
    # Gerar array C++
    array_lines = []
    array_lines.append("static std::vector<uint8_t> image = {")
    
    # Escrever bytes em linhas de 16 bytes
    for i in range(0, len(driver_data), 16):
        line_bytes = driver_data[i:i+16]
        hex_values = ', '.join(f'0x{b:02X}' for b in line_bytes)
        if i + 16 < len(driver_data):
            array_lines.append(f"    {hex_values},")
        else:
            array_lines.append(f"    {hex_values}")
    
    array_lines.append("};")
    
    result = '\n'.join(array_lines)
    
    if output_path:
        with open(output_path, 'w') as f:
            f.write(result)
        print(f"Array gerado em: {output_path}")
    else:
        print(result)
    
    print(f"\nTotal de bytes: {len(driver_data)}")
    return True

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python convert_driver.py <driver.sys> [output.txt]")
        sys.exit(1)
    
    driver_path = sys.argv[1]
    output_path = sys.argv[2] if len(sys.argv) > 2 else None
    
    convert_driver_to_array(driver_path, output_path)

