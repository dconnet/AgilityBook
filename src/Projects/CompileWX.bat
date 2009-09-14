@echo off
rem This is my quick-and-easy way to compile wxWidgets on Windows.
rem
rem 09/12/2009 DRC Fix dll creation

rem Where is 'Program Files'?
rem default: 32bit on 32bit
set _PFILES=c:\Program Files
rem 64bit on 64bit
if ("%PROCESSOR_ARCHITECTURE%")==("AMD64") set _PFILES=c:\Program Files (x86)
rem 64bit on Wow64 (32bit cmd shell spawned from msdev)
if ("%PROCESSOR_ARCHITEW6432%")==("AMD64") set _PFILES=c:\Program Files (x86)

if not exist "%WXWIN%" echo WXWIN doesn't exist && goto done

set _PROGNAME=%0
set _COMMENT=
rem For wxWidgets 2.9+, set this to 1
set _HAS_COMPILER_PREFIX=0

if not ("%1")==("all") goto :args
call :args dynamic mbcs vc6
call :args static mbcs vc7
call :args static mbcs vc8
call :args static unicode vc9
call :args static unicode vc9x64
goto done

:args
set _PATH=%PATH%
set _LIBPATH=%LIBPATH%

rem Just set the compiler environment
if ("%1")==("env") set _COMMENT=rem&& shift

set _DO_SHIFT=0
REM Lib (or Dll)
set _SHARED=0
REM Runtime library (usually always same as shared)
set _RUNTIME_LIBS=static
set _DO_UNICODE=1

if ("%1")==("hasprefix") set _HAS_COMPILER_PREFIX=1&& shift

if ("%1")==("static") set _DO_SHIFT=1
if ("%1")==("dynamic") set _RUNTIME_LIBS=dynamic&& set _SHARED=1&& set _DO_SHIFT=1
if ("%_DO_SHIFT%")==("1") shift && set _DO_SHIFT=0

if ("%1")==("mbcs") set _DO_UNICODE=0&& set _DO_SHIFT=1
if ("%1")==("unicode") set _DO_UNICODE=1&& set _DO_SHIFT=1
if ("%_DO_SHIFT%")==("1") shift && set _DO_SHIFT=0

if ("%_RUNTIME_LIBS%")==("dynamic") set _VENDOR=VENDOR=dconsoft

set INCLUDE=
set LIB=

if ("%1")==("vc6") goto vc6
if ("%1")==("vc7") goto vc7
if ("%1")==("vc8") goto vc8
if ("%1")==("vc9") goto vc9
if ("%1")==("vc9x64") goto vc9x64
goto usage

:vc6
if not exist "%_PFILES%\Microsoft Visual Studio\VC98\bin\vcvars32.bat" echo VC6 not installed && goto done
if ("%_DO_UNICODE%")==("1") echo Error: VC6 doesn't do unicode && goto usage
if ("%_SHARED%")==("0") echo Error: VC6 doesn't do static && goto usage
title VC6
call "%_PFILES%\Microsoft Visual Studio\VC98\bin\vcvars32.bat"
rem if ERRORLEVEL 1 goto error
if ("%_HAS_COMPILER_PREFIX%")==("1") set _CFG=COMPILER_PREFIX=vc60
if ("%_HAS_COMPILER_PREFIX%")==("0") set _CFG=CFG=_VC60
set _CPPFLAGS=
goto :doit

:vc7
if not exist "%_PFILES%\Microsoft Visual Studio .NET 2003\Common7\Tools\vsvars32.bat" echo VC7.1 not installed && goto done
title VC7
call "%_PFILES%\Microsoft Visual Studio .NET 2003\Common7\Tools\vsvars32.bat"
rem if ERRORLEVEL 1 goto error
if ("%_DO_UNICODE%")==("1") echo Error: VC7 doesn't do unicode && goto usage
if ("%_HAS_COMPILER_PREFIX%")==("1") set _CFG=COMPILER_PREFIX=vc71
if ("%_HAS_COMPILER_PREFIX%")==("0") set _CFG=CFG=_VC71
set _CPPFLAGS=
goto :doit

:vc8
if not exist "%_PFILES%\Microsoft Visual Studio 8\VC\vcvarsall.bat" echo VC8 not installed && goto done
title VC8
call "%_PFILES%\Microsoft Visual Studio 8\VC\vcvarsall.bat" x86
rem vc8 seems to always return 1
rem if ERRORLEVEL 1 goto error
if ("%_HAS_COMPILER_PREFIX%")==("1") set _CFG=COMPILER_PREFIX=vc80
if ("%_HAS_COMPILER_PREFIX%")==("0") set _CFG=CFG=_VC80
set _CPPFLAGS=
goto :doit

:vc9
if not exist "%_PFILES%\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" echo VC9 not installed && goto done
title VC9
call "%_PFILES%\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86
if ERRORLEVEL 1 goto error
if ("%_HAS_COMPILER_PREFIX%")==("1") set _CFG=COMPILER_PREFIX=vc90
if ("%_HAS_COMPILER_PREFIX%")==("0") set _CFG=CFG=_VC90
set _CPPFLAGS=/D_SECURE_SCL=1 /D_SECURE_SCL_THROWS=1 /D_BIND_TO_CURRENT_VCLIBS_VERSION=1
goto :doit

:vc9x64
if not exist "%_PFILES%\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" echo VC9 not installed && goto done
if ("%_DO_UNICODE%")==("0") echo Error: VC9x64 doesn't do mbcs && goto usage
set _ARCHTYPE=x86_amd64
if ("%PROCESSOR_ARCHITECTURE%")==("AMD64") set _ARCHTYPE=amd64
title VC9 %_ARCHTYPE%
call "%_PFILES%\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" %_ARCHTYPE%
if ERRORLEVEL 1 goto error
set _TARGET_CPU=TARGET_CPU=amd64
if ("%_HAS_COMPILER_PREFIX%")==("1") set _CFG=COMPILER_PREFIX=vc90
if ("%_HAS_COMPILER_PREFIX%")==("0") set _CFG=CFG=_VC90
set _CPPFLAGS=/D_SECURE_SCL=1 /D_SECURE_SCL_THROWS=1 /D_BIND_TO_CURRENT_VCLIBS_VERSION=1
set _ARCHTYPE=
goto :doit


:doit
cd %WXWIN%\build\msw

%_COMMENT% nmake -f makefile.vc BUILD=release            UNICODE=%_DO_UNICODE% SHARED=%_SHARED% RUNTIME_LIBS=%_RUNTIME_LIBS% %_TARGET_CPU% %_CFG% CPPFLAGS="%_CPPFLAGS%" %_VENDOR%
%_COMMENT% nmake -f makefile.vc BUILD=debug DEBUG_INFO=1 UNICODE=%_DO_UNICODE% SHARED=%_SHARED% RUNTIME_LIBS=%_RUNTIME_LIBS% %_TARGET_CPU% %_CFG% CPPFLAGS="%_CPPFLAGS%" %_VENDOR%

cd \AgilityBook\src\AgilityBook\src\Projects
goto done

:usage
echo Usage: %_PROGNAME% all
echo Usage: %_PROGNAME% env vc6/vc7/vc8/vc9/vc9x64
echo Usage: %_PROGNAME% [hasprefix] [dynamic/static] [mbcs/unicode] vc6/vc7/vc8/vc9/vc9x64
goto done

:error
echo %_PROGNAME%: ERROR!!!

:done
%_COMMENT% if not ("%_PATH%")==("") set PATH=%_PATH%&& set _PATH=
%_COMMENT% if not ("%_LIBPATH%")==("") set LIBPATH=%_LIBPATH%&& set _LIBPATH=

set _CFG=
set _COMMENT=
set _CPPFLAGS=
set _DO_SHIFT=
set _DO_UNICODE=
set _HAS_COMPILER_PREFIX=
set _PFILES=
set _PROGNAME=
set _RUNTIME_LIBS=
set _SHARED=
set _TARGET_CPU=
set _VENDOR=
