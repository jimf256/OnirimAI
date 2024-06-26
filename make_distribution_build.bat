@echo off

set /P include_template=include player template project? (y/n):
set /P include_python=include python player? (y/n): 
set /P include_cpp_ai=include C++ AI player? (y/n): 
echo.

echo | call clean_build_all.bat
if %ERRORLEVEL% neq 0 (
	pause
	exit /b %ERRORLEVEL%
)

set build_dir=%~dp0Build
if exist %build_dir% (
	rmdir /S /Q "%build_dir%"
)

mkdir "%build_dir%"
mkdir "%build_dir%\Binaries"
mkdir "%build_dir%\Libraries"

copy "Binaries\Onirim.exe" "%build_dir%\Binaries"
copy "Libraries\ConsolePlayer.dll" "%build_dir%\Libraries"

if "%include_template:~0,1%" == "y" (
	mkdir "%build_dir%\Source\PlayerTemplate"
	mkdir "%build_dir%\Source\GameUtils\Headers"
	copy "Source\GameUtils\Headers" "%build_dir%\Source\GameUtils\Headers\"
	copy "Source\PlayerTemplate" "%build_dir%\Source\PlayerTemplate\"
	copy "Binaries\Onirim_d.exe" "%build_dir%\Binaries\Onirim_d.exe"
	copy "Libraries\GameUtils.dll" "%build_dir%\Libraries"
	copy "Libraries\GameUtils_d.lib" "%build_dir%\Libraries"
	copy "Libraries\PlayerTemplate.dll" "%build_dir%\Libraries"
	copy "Libraries\PlayerTemplate_d.lib" "%build_dir%\Libraries"
	copy "run_cpp_AI.py" "%build_dir%\run_cpp_AI.py"
	echo PlayerTemplate > "%build_dir%\cpp_player_config.ini"
)

if "%include_python:~0,1%" == "y" (
	mkdir "%build_dir%\Source\PythonPlayer"
	copy "Source\PythonPlayer\*.py" "%build_dir%\Source\PythonPlayer\"
	copy "Libraries\PythonPlayer.dll" "%build_dir%\Libraries"
	copy "run_python_AI.py" "%build_dir%\run_python_AI.py"
	copy "python_player_config.ini" "%build_dir%\python_player_config.ini"
)

if "%include_cpp_ai:~0,1%" == "y" (
	copy "Libraries\AIPlayer.dll" "%build_dir%\Libraries"
	copy "run_cpp_AI.py" "%build_dir%\run_cpp_AI.py"
	echo AIPlayer > "%build_dir%\cpp_player_config.ini"
)

echo @echo off > "%build_dir%\run_interactive_version.bat"
echo Binaries\Onirim.exe ConsolePlayer > "%build_dir%\run_interactive_version.bat"

pause