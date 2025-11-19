@echo off
REM Script de compilação do driver Noturnal-kmd
REM Requer WDK Build Environment

echo ========================================
echo Compilando Driver Noturnal-kmd
echo ========================================
echo.

REM Verificar se estamos no diretório correto
if not exist "driver.c" (
    echo ERRO: Arquivo driver.c nao encontrado!
    echo Execute este script no diretório do driver.
    pause
    exit /b 1
)

REM Verificar se o build.exe está disponível
where build.exe >nul 2>&1
if %errorlevel% neq 0 (
    echo.
    echo ERRO: build.exe nao encontrado!
    echo.
    echo Por favor, abra o "WDK Build Environment (x64)" e execute:
    echo   cd %CD%
    echo   build
    echo.
    echo Ou use o Visual Studio com WDK instalado.
    echo.
    pause
    exit /b 1
)

echo Compilando...
echo.

REM Compilar o driver
build -cZ

if %errorlevel% equ 0 (
    echo.
    echo ========================================
    echo Compilacao bem-sucedida!
    echo ========================================
    echo.
    
    REM Verificar se o arquivo .sys foi gerado
    if exist "x64\Debug\Noturnal-kmd.sys" (
        echo Driver compilado: x64\Debug\Noturnal-kmd.sys
        dir /b x64\Debug\Noturnal-kmd.sys
        echo.
        echo Para converter para array de bytes, execute:
        echo   python convert_driver.py x64\Debug\Noturnal-kmd.sys
    ) else if exist "x64\Release\Noturnal-kmd.sys" (
        echo Driver compilado: x64\Release\Noturnal-kmd.sys
        dir /b x64\Release\Noturnal-kmd.sys
        echo.
        echo Para converter para array de bytes, execute:
        echo   python convert_driver.py x64\Release\Noturnal-kmd.sys
    ) else (
        echo AVISO: Arquivo .sys nao encontrado apos compilacao!
    )
) else (
    echo.
    echo ========================================
    echo ERRO na compilacao!
    echo ========================================
    echo.
    echo Verifique os erros acima.
)

echo.
pause


