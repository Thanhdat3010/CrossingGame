@echo off
title Crossing Game - Auto Compiler
echo ===================================================
echo     DONG BO COMPILE VA KICH HOAT CROSSING GAME
echo ===================================================
echo.
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0run.ps1"
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Co loi xay ra trong qua trinh compile.
    pause
)
