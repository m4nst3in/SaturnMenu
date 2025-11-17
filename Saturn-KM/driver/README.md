# Driver de Kernel Saturn

Este diretório contém o código fonte do driver de kernel necessário para o Saturn funcionar.

## Características

Este driver foi otimizado com:
- **Cache de processo PEPROCESS** - Reduz lookups desnecessários
- **MmCopyVirtualMemory** - Método seguro e eficiente para leitura de memória
- **Validação otimizada** - Checks ordenados por probabilidade de falha
- **Leitura em lote otimizada** - Processa múltiplas leituras de forma eficiente

## Estrutura

- `driver.h` - Cabeçalho com definições e estruturas
- `driver.c` - Implementação do driver otimizada
- `sources` - Arquivo de configuração para compilação com WDK
- `convert_driver.py` - Script para converter o .sys em array de bytes

## Requisitos

Para compilar o driver, você precisa:

1. **Windows Driver Kit (WDK)** - Baixe do site da Microsoft
2. **Visual Studio** - Compatível com a versão do WDK
3. **Windows SDK** - Geralmente incluído com o WDK

## Como Compilar

### Método 1: Usando Visual Studio com WDK

1. Abra o Visual Studio
2. Crie um novo projeto "Kernel Mode Driver, Empty (KMDF)"
3. Adicione os arquivos `driver.h` e `driver.c`
4. Configure as propriedades do projeto:
   - **Configuration Type**: Driver
   - **Target Name**: Saturn-kmd
   - **Target Extension**: .sys
5. Compile o projeto (Build Solution)

### Método 2: Usando linha de comando (WDK Build Environment)

1. Abra o "WDK Build Environment" (x64)
2. Navegue até o diretório do driver:
   ```cmd
   cd Saturn-kernel\driver
   ```
3. Compile:
   ```cmd
   build
   ```

O driver compilado estará em: `x64\Debug\Saturn-kmd.sys` (ou Release)

## Converter para Array de Bytes

Após compilar o driver, você precisa convertê-lo em um array de bytes para inserir no `cfg.h`:

```bash
python convert_driver.py x64\Debug\Saturn-kmd.sys output.txt
```

Ou simplesmente:

```bash
python convert_driver.py Saturn-kmd.sys
```

Isso gerará um array C++ que você pode copiar para `cfg.h`:

```cpp
static std::vector<uint8_t> image = {
    0x4D, 0x5A, 0x90, 0x00, ...
};
```

## Funcionalidades do Driver

O driver implementa os seguintes IOCTLs:

- **IOCTL_ATTACH** (0x4452) - Anexa a um processo
- **IOCTL_READ** (0x4453) - Lê memória do processo
- **IOCTL_GET_MODULE_BASE** (0x4454) - Obtém endereço base de módulo
- **IOCTL_GET_PID** (0x4455) - Obtém PID por nome de processo
- **IOCTL_BATCH_READ** (0x4456) - Leitura em lote de memória

## Notas Importantes

⚠️ **AVISO**: Este driver é apenas para leitura de memória (read-only). Ele não modifica memória de processos.

⚠️ **SEGURANÇA**: 
- O driver precisa ser assinado ou você precisa desabilitar a verificação de assinatura (modo de teste)
- Em sistemas modernos, você pode precisar desabilitar o Driver Signature Enforcement

⚠️ **COMPATIBILIDADE**: 
- Testado no Windows 10/11
- Requer privilégios de administrador para carregar

## Troubleshooting

### Erro: "Driver image is empty"
- Certifique-se de que compilou o driver corretamente
- Verifique se o array de bytes foi copiado corretamente para `cfg.h`
- O array não deve estar vazio: `image = {}`

### Erro ao mapear o driver
- Execute o mapper como administrador
- Verifique se o driver vulnerável da Intel está disponível
- Desabilite antivírus temporariamente

