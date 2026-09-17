@echo off
setlocal
echo [INFO] Installing Lucy Package Manager (LPM) for Windows...

:: Create target directories
set "INSTALL_DIR=C:\Program Files\Lucy\bin"
set "STDLIB_DIR=C:\Program Files\Lucy\stdlib"
set "PACKAGES_DIR=C:\Program Files\Lucy\packages"
set "TMP_DIR=C:\Program Files\Lucy\tmp"

if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"
if not exist "%STDLIB_DIR%" mkdir "%STDLIB_DIR%"
if not exist "%PACKAGES_DIR%" mkdir "%PACKAGES_DIR%"
if not exist "%TMP_DIR%" mkdir "%TMP_DIR%"

:: Check if lpm.exe exists in current directory or build folder
if exist "build\Release\lpm.exe" (
    copy /Y "build\Release\lpm.exe" "%INSTALL_DIR%\lpm.exe"
) else if exist "lpm.exe" (
    copy /Y "lpm.exe" "%INSTALL_DIR%\lpm.exe"
) else (
    echo [ERROR] lpm.exe not found! Please compile the project first using CMake.
    pause
    exit /b 1
)

:: Add to System PATH permanently
echo [INFO] Adding %INSTALL_DIR% to System PATH...
powershell -Command "[Environment]::SetEnvironmentVariable('Path', [Environment]::GetEnvironmentVariable('Path', [EnvironmentVariableTarget]::Machine) + ';%INSTALL_DIR%', [EnvironmentVariableTarget]::Machine)"

echo [SUCCESS] LPM successfully installed and added to PATH!
echo [NOTE] Please restart your terminal/cmd to apply changes.
pause
