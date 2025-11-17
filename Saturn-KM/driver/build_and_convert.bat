@echo off
REM Script para compilar e converter o driver automaticamente

echo ========================================
echo Compilando e Convertendo Driver
echo ========================================
echo.

REM Compilar
call build.bat
if %errorlevel% neq 0 (
    echo Compilacao falhou. Abortando conversao.
    pause
    exit /b 1
)

echo.
echo ========================================
echo Convertendo para array de bytes...
echo ========================================
echo.

REM Verificar Python
where python >nul 2>&1
if %errorlevel% neq 0 (
    echo ERRO: Python nao encontrado!
    echo Instale Python para converter o driver.
    pause
    exit /b 1
)

REM Encontrar o arquivo .sys
set DRIVER_SYS=
if exist "x64\Debug\Saturn-kmd.sys" (
    set DRIVER_SYS=x64\Debug\Saturn-kmd.sys
) else if exist "x64\Release\Saturn-kmd.sys" (
    set DRIVER_SYS=x64\Release\Saturn-kmd.sys
) else (
    echo ERRO: Arquivo .sys nao encontrado!
    pause
    exit /b 1
)

echo Convertendo %DRIVER_SYS%...
python convert_driver.py %DRIVER_SYS% driver_array.txt

if %errorlevel% equ 0 (
    echo.
    echo ========================================
    echo Conversao concluida!
    echo ========================================
    echo.
    echo Array de bytes salvo em: driver_array.txt
    echo.
    echo Copie o conteudo do arquivo para cfg.h:
    echo   static std::vector^<uint8_t^> image = { ... };
    echo.
) else (
    echo.
    echo ERRO na conversao!
)

pause


