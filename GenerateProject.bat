@echo off
echo.
echo Choose Visual Studio version:
echo   1 - VS2017
echo   2 - VS2019
echo   3 - VS2022
set /p choice="Enter choice: "

if "%choice%"=="1" (
    premake5 vs2017
) else if "%choice%"=="2" (
    premake5 vs2019
) else if "%choice%"=="3" (
    premake5 vs2022
) else (
    echo Invalid choice.
    pause
    exit /b
)

echo.
echo ✅ Project generated successfully!
pause
