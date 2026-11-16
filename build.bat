@echo off
cd /d "%~dp0"
gcc -std=c99 -Wall -Wextra -pedantic -O2 src\main.c src\utils.c src\room.c src\guest.c src\booking.c src\report.c src\fileio.c -o hotel.exe
if errorlevel 1 (
    echo Bien dich THAT BAI.
    exit /b 1
)
echo Bien dich thanh cong: hotel.exe
