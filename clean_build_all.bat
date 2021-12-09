@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsMSBuildCmd.bat"

msbuild "Source\Onirim.sln" /t:Clean /p:Configuration=Debug;Platform=x64
msbuild "Source\Onirim.sln" /t:Clean /p:Configuration=Debug_NoLogging;Platform=x64
msbuild "Source\Onirim.sln" /t:Clean /p:Configuration=Release;Platform=x64

msbuild "Source\Onirim.sln" /t:Build /p:Configuration=Debug;Platform=x64
msbuild "Source\Onirim.sln" /t:Build /p:Configuration=Debug_NoLogging;Platform=x64
msbuild "Source\Onirim.sln" /t:Build /p:Configuration=Release;Platform=x64

pause