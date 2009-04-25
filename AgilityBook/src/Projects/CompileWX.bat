@echo off
rem This is my quick-and-easy way to compile wxWidgets on Windows.

set _PROGNAME=%0

if not ("%1")==("all") goto :args
call :args dynamic mbcs vc6
call :args static mbcs vc7
call :args static mbcs vc8
call :args static unicode vc9
call :args static unicode vc9x64
goto done

:args
set _DO_SHIFT=0
set _RUNTIME_LIBS=static
if ("%1")==("static") set _RUNTIME_LIBS=static&& set _DO_SHIFT=1
if ("%1")==("dynamic") set _RUNTIME_LIBS=dynamic&& set _DO_SHIFT=1
if ("%_DO_SHIFT%")==("1") shift && set _DO_SHIFT=0

set _DO_UNICODE=1
if ("%1")==("mbcs") set _DO_UNICODE=0&& set _DO_SHIFT=1
if ("%1")==("unicode") set _DO_UNICODE=1&& set _DO_SHIFT=1
if ("%_DO_SHIFT%")==("1") shift && set _DO_SHIFT=0

if ("%_RUNTIME_LIBS%")==("static") set _CFGEND=s

if ("%1")==("vc6") goto vc6
if ("%1")==("vc7") goto vc7
if ("%1")==("vc8") goto vc8
if ("%1")==("vc9") goto vc9
if ("%1")==("vc9x64") goto vc9x64
goto usage

:vc6
if ("%_DO_UNICODE%")==("1") echo Error: VC6 doesn't do unicode && goto usage
if ("%_RUNTIME_LIBS%")==("static") echo Error: VC6 doesn't do static && goto usage
title VC6
call "C:\Program Files\Microsoft Visual Studio\VC98\bin\vcvars32.bat"
set _CFG=_VC6.0
set _CPPFLAGS=
goto :doit

:vc7
title VC7
call "C:\Program Files\Microsoft Visual Studio .NET 2003\Common7\Tools\vsvars32.bat"
if ("%_DO_UNICODE%")==("1") echo Error: VC7 doesn't do unicode && goto usage
set _CFG=_VC7.1
set _CPPFLAGS=
goto :doit

:vc8
title VC8
call "C:\Program Files\Microsoft Visual Studio 8\VC\vcvarsall.bat" x86
set _CFG=_VC8.0
set _CPPFLAGS=
goto :doit

:vc9
title VC9
call "C:\Program Files\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86
set _CFG=_VC9.0
set _CPPFLAGS=/D_SECURE_SCL=1 /D_SECURE_SCL_THROWS=1
goto :doit

:vc9x64
if ("%_DO_UNICODE%")==("0") echo Error: VC9x64 doesn't do mbcs && goto usage
title VC9x64
call "C:\Program Files\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86_amd64
set _TARGET_CPU=TARGET_CPU=amd64
set _CFG=_VC9.0
set _CPPFLAGS=/D_SECURE_SCL=1 /D_SECURE_SCL_THROWS=1
goto :doit


:doit
cd %WXWIN%\build\msw

nmake -f makefile.vc BUILD=release            UNICODE=%_DO_UNICODE% RUNTIME_LIBS=%_RUNTIME_LIBS% %_TARGET_CPU% CFG=%_CFG%%_CFGEND% CPPFLAGS="%_CPPFLAGS%"
nmake -f makefile.vc BUILD=debug DEBUG_INFO=1 UNICODE=%_DO_UNICODE% RUNTIME_LIBS=%_RUNTIME_LIBS% %_TARGET_CPU% CFG=%_CFG%%_CFGEND% CPPFLAGS="%_CPPFLAGS%"

cd \AgilityBook\src\AgilityBook\src\Projects
goto done

:usage
echo Usage: %_PROGNAME% all
echo Usage: %_PROGNAME% [dynamic/static] [mbcs/unicode] vc6/vc7/vc8/vc9/vc9x64

:done
set _CFG=
set _CFGEND=
set _CPPFLAGS=
set _DO_SHIFT=
set _DO_UNICODE=
set _PROGNAME=
set _RUNTIME_LIBS=
set _TARGET_CPU=
