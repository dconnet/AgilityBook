@echo off
REM This script is called in the post-build step to run the unittest program
if (%1)==() goto usage
if (%2)==() goto usage

if ("%3")==("wx") goto genFiles
goto testArg2
:genFiles
REM Create "TestARB.exe.dat"
"c:\Program Files\WinZip\WZZIP.EXE" -a %1.dat ..\..\Win\res\DefaultConfig.xml ..\..\TestARB\res\*.xml

:testArg2
if ("%2")==("Win32") goto w32
if ("%2")==("x64") goto x64

echo Error: Unknown target: %2
goto done

:w32
REM 32bit on 32bit
if ("%PROCESSOR_ARCHITECTURE%")==("x86") goto run
goto nope

:x64
REM 64bit on 64bit
if ("%PROCESSOR_ARCHITECTURE%")==("AMD64") goto run
REM 64bit on Wow64 (32bit cmd shell spawned from msdev)
if ("%PROCESSOR_ARCHITEW6432%")==("AMD64") goto run
goto nope

:nope
echo WARNING: Unable to run %2 binary on %PROCESSOR_ARCHITECTURE%
goto done

:run
%1
goto done

:usage
echo Usage: RunTests.bat Executable PlatformName
echo PlatformName = Win32 x64
echo Ex (in devenv postbuild): RunTests.bat "$(TargetPath)" $(PlatformName) [wx]

:done
