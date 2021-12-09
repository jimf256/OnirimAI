@echo off

set build_dir=%~dp0Build
if exist %build_dir% (
	rmdir /S /Q "%build_dir%"
)

mkdir "%build_dir%"
mkdir "%build_dir%\Binaries"
mkdir "%build_dir%\Libraries"
mkdir "%build_dir%\Source\PlayerTemplate"
mkdir "%build_dir%\Source\GameUtils\Headers"

copy "Source\GameUtils\Headers" "%build_dir%\Source\GameUtils\Headers\"
copy "Source\PlayerTemplate" "%build_dir%\Source\PlayerTemplate\"
copy "Binaries\Onirim.exe" "%build_dir%\Binaries"
copy "Binaries\Onirim_d_nologging.exe" "%build_dir%\Binaries\Onirim_d.exe"
copy "Libraries\GameUtils.lib" "%build_dir%\Libraries"
copy "Libraries\GameUtils_d.lib" "%build_dir%\Libraries"
copy "Libraries\ConsolePlayer.dll" "%build_dir%\Libraries"
copy "Libraries\ConsolePlayer_d.dll" "%build_dir%\Libraries"

echo @echo off > "%build_dir%\run_release_interactive_version.bat"
echo Binaries\Onirim.exe ConsolePlayer > "%build_dir%\run_release_interactive_version.bat"

echo @echo off > "%build_dir%\run_debug.bat"
echo Binaries\Onirim_d.exe PlayerTemplate > "%build_dir%\run_debug.bat"

echo @echo off > "%build_dir%\run_release.bat"
echo Binaries\Onirim.exe PlayerTemplate > "%build_dir%\run_release.bat"

pause