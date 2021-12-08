@echo off
Binaries\Onirim_d.exe AIPlayer
if %ERRORLEVEL% == 1 (
	echo result: win
)
if %ERRORLEVEL% == 2 (
	echo result: loss - ran out of cards
)
if %ERRORLEVEL% == 3 (
	echo result: loss - discarded a door
)
pause