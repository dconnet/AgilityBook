@echo off
REM This script is called in the post-build step of ParseResourceHeader
REM ProjectDir (quoted)
if ("%1")==("") goto usage
REM Build Configuration (quoted)
if ("%2")==("") goto usage
REM PlatformName (not quoted)
if ("%3")==("") goto usage

if ("%3")==("Win32") goto w32
if ("%3")==("x64") goto x64

echo $0: Error: Unknown target: %3
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
cd %1..\..
python BuildHelp.py %2 VC9%3 %4

goto done

:usage
echo Usage: %0 ProjectDirectory Configuration PlatformName [wx]
echo PlatformName = Win32 x64
echo Ex (in devenv postbuild): %0 "$(ProjectDir)" "$(ConfigurationName)" $(PlatformName)

:done
