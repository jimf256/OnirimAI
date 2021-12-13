@echo off

set /P include_template=include player template project? (y/n): 
set /P include_python=include python player? (y/n): 
set /P include_cpp_ai=include C++ AI player? (y/n): 
echo.

echo | call clean_build_all.bat

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
	copy "Binaries\Onirim_d_nologging.exe" "%build_dir%\Binaries\Onirim_d.exe"
	copy "Libraries\GameUtils.lib" "%build_dir%\Libraries"
	copy "Libraries\GameUtils_d.lib" "%build_dir%\Libraries"

	echo @echo off > "%build_dir%\run_debug.bat"
	echo Binaries\Onirim_d.exe PlayerTemplate > "%build_dir%\run_debug.bat"

	echo @echo off > "%build_dir%\run_release.bat"
	echo Binaries\Onirim.exe PlayerTemplate > "%build_dir%\run_release.bat"
)

if "%include_python:~0,1%" == "y" (
	mkdir "%build_dir%\Source\PythonAI"
	copy "Source\PythonAI\*.py" "%build_dir%\Source\PythonAI\"
	copy "Libraries\PythonPlayer.dll" "%build_dir%\Libraries"
	copy "run_python_player.py" "%build_dir%\run_python_player.py"
)

if "%include_cpp_ai:~0,1%" == "y" (
	copy "Libraries\AIPlayer.dll" "%build_dir%\Libraries"
	copy "run_ai_player.py" "%build_dir%\run_ai_player.py"
)

echo @echo off > "%build_dir%\run_interactive_version.bat"
echo Binaries\Onirim.exe ConsolePlayer > "%build_dir%\run_interactive_version.bat"

pause