@echo off
REM This script is called in the post-build step to run the unittest program
if (%1)==() goto usage
if (%2)==() goto usage
if (%3)==() goto usage

set _DATFILE=%1\%2.dat
set _EXE=%1\%2.exe

REM Create "TestARB.exe.dat"
"c:\Program Files\WinZip\WZZIP.EXE" -a %_DATFILE% ..\..\Win\res\DefaultConfig.xml ..\..\Win\res\AgilityRecordBook.dtd ..\..\TestARB\res\*.xml

:testArg2
if ("%3")==("Win32") goto w32
if ("%3")==("x64") goto x64

echo Error: Unknown target: %3
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
echo WARNING: Unable to run %3 binary on %PROCESSOR_ARCHITECTURE%
goto done

:run
%_EXE%
goto done

:usage
echo Usage: RunTests.bat Executable PlatformName
echo PlatformName = Win32 x64
echo Ex (in devenv postbuild): RunTests.bat "$(TargetDir)" "$(TargetName)" $(PlatformName)

:done
set _DATFILE=
set _EXE=
