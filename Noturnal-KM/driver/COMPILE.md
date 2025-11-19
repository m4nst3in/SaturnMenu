# Como Compilar o Driver Noturnal

Existem várias formas de compilar o driver. Escolha o método mais adequado para você.

## Método 1: Visual Studio com WDK (Recomendado)

### Pré-requisitos
1. **Visual Studio 2019 ou 2022** (Community, Professional ou Enterprise)
2. **Windows Driver Kit (WDK)** - Baixe do [site da Microsoft](https://learn.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk)
3. **Windows SDK** - Geralmente instalado automaticamente com o WDK

### Passos

1. **Instalar o WDK**
   - Baixe e instale o WDK compatível com sua versão do Visual Studio
   - O instalador configurará automaticamente o Visual Studio

2. **Criar Projeto no Visual Studio**
   - Abra o Visual Studio
   - File → New → Project
   - Procure por "Kernel Mode Driver, Empty (KMDF)" ou "Kernel Mode Driver, Empty (WDM)"
   - Nome: `Noturnal-kmd`
   - Localização: Escolha um diretório temporário (vamos copiar os arquivos depois)

3. **Adicionar Arquivos do Driver**
   - Delete os arquivos padrão gerados pelo template
   - Copie `driver.h` e `driver.c` para o diretório do projeto
   - No Visual Studio: Right-click no projeto → Add → Existing Item
   - Selecione `driver.h` e `driver.c`

4. **Configurar Propriedades do Projeto**
   - Right-click no projeto → Properties
   - **Configuration Properties → General:**
     - Configuration Type: `Driver`
     - Target Name: `Noturnal-kmd`
     - Target Extension: `.sys`
   - **Configuration Properties → Driver Settings:**
     - Driver Type: `WDM` (ou deixe o padrão)
   - **C/C++ → General:**
     - Additional Include Directories: Deixe padrão (WDK já configura)
   - **C/C++ → Preprocessor:**
     - Preprocessor Definitions: Adicione `DEBUG` se quiser debug prints

5. **Compilar**
   - Build → Build Solution (ou F7)
   - O driver compilado estará em: `x64\Debug\Noturnal-kmd.sys` (ou `Release`)

---

## Método 2: WDK Build Environment (Linha de Comando)

### Pré-requisitos
- Windows Driver Kit (WDK) instalado

### Passos

1. **Abrir WDK Build Environment**
   - No menu Iniciar, procure por "WDK"
   - Abra "WDK Build Environment" → "x64" (ou "x86" se necessário)
   - Isso abre um prompt de comando configurado

2. **Navegar até o Diretório do Driver**
   ```cmd
   cd C:\Users\m4\Documents\NoturnalMenu\Noturnal-kernel\driver
   ```

3. **Compilar**
   ```cmd
   build
   ```

4. **Localizar o Driver Compilado**
   - O driver estará em: `x64\Debug\Noturnal-kmd.sys`
   - Ou em: `x64\Release\Noturnal-kmd.sys` (se compilou em Release)

---

## Método 3: Usando o Arquivo `sources` (Build Manual)

Se você tem o WDK instalado mas prefere usar o arquivo `sources` diretamente:

1. **Abrir WDK Build Environment (x64)**

2. **Navegar até o Diretório**
   ```cmd
   cd C:\Users\m4\Documents\NoturnalMenu\Noturnal-kernel\driver
   ```

3. **Compilar com build.exe**
   ```cmd
   build -cZ
   ```
   - `-c` = Clean build
   - `-Z` = Verbose output

---

## Verificar Compilação Bem-Sucedida

Após compilar, verifique:

1. **Arquivo .sys gerado**
   - Deve existir: `x64\Debug\Noturnal-kmd.sys` (ou Release)
   - Tamanho típico: 20-100 KB

2. **Sem erros no output**
   - Deve mostrar: `Noturnal-kmd.sys - 0 error(s), X warning(s)`

---

## Converter para Array de Bytes

Após compilar com sucesso, converta o .sys para array de bytes:

```cmd
cd C:\Users\m4\Documents\NoturnalMenu\Noturnal-kernel\driver
python convert_driver.py x64\Debug\Noturnal-kmd.sys
```

Ou salve em arquivo:

```cmd
python convert_driver.py x64\Debug\Noturnal-kmd.sys driver_array.txt
```

Isso gerará um array C++ que você pode copiar para `cfg.h`:

```cpp
static std::vector<uint8_t> image = {
    0x4D, 0x5A, 0x90, 0x00, 0x03, 0x00, 0x00, 0x00,
    // ... mais bytes ...
};
```

---

## Troubleshooting

### Erro: "Cannot open include file: 'ntifs.h'"
- **Solução**: Instale o WDK e certifique-se de usar o WDK Build Environment ou Visual Studio com WDK instalado

### Erro: "IoCreateDriver was not declared"
- **Solução**: Adicione `extern "C"` antes da declaração ou inclua os headers corretos do WDK

### Erro: "Unresolved external symbol"
- **Solução**: Verifique se está compilando como Driver (não como aplicação) nas propriedades do projeto

### Erro: "Driver too large" ou problemas de tamanho
- **Solução**: Compile em Release mode para otimização de tamanho

### Erro ao converter para array
- **Solução**: Certifique-se de que o Python está instalado e o caminho do .sys está correto

---

## Dicas

1. **Debug vs Release**
   - **Debug**: Inclui símbolos de debug, maior tamanho, permite debug prints
   - **Release**: Otimizado, menor tamanho, sem debug prints

2. **Assinatura do Driver**
   - Para testes, você pode desabilitar a verificação de assinatura:
     ```cmd
     bcdedit /set testsigning on
     ```
     (Requer reinicialização)

3. **Testar o Driver**
   - Use o `Noturnal-km.exe` (mapper) para carregar o driver
   - Execute como Administrador

---

## Estrutura Final Esperada

Após compilar, você deve ter:

```
Noturnal-kernel/driver/
├── driver.h
├── driver.c
├── sources
├── convert_driver.py
├── README.md
├── COMPILE.md
└── x64/
    └── Debug/ (ou Release/)
        ├── Noturnal-kmd.sys  ← Driver compilado
        ├── Noturnal-kmd.pdb  ← Símbolos de debug (se Debug)
        └── ...
```

O arquivo `.sys` é o que você precisa converter para array de bytes e inserir em `cfg.h`!


