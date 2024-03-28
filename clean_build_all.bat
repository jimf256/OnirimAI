@echo off

set vscmd="C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsMSBuildCmd.bat"
if not exist %vscmd% (
	echo error: requires Visual Studio 2022 - installation not found
	pause
	exit /B 1
)

call %vscmd%

msbuild "Source\OnirimAI.sln" /t:Clean /p:Configuration=Debug;Platform=x64
msbuild "Source\OnirimAI.sln" /t:Clean /p:Configuration=Debug_NoLogging;Platform=x64
msbuild "Source\OnirimAI.sln" /t:Clean /p:Configuration=Release;Platform=x64

msbuild "Source\OnirimAI.sln" /t:Build /p:Configuration=Debug;Platform=x64
msbuild "Source\OnirimAI.sln" /t:Build /p:Configuration=Debug_NoLogging;Platform=x64
msbuild "Source\OnirimAI.sln" /t:Build /p:Configuration=Release;Platform=x64

pause