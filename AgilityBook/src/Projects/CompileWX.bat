@echo off
rem This is my quick-and-easy way to compile wxWidgets on Windows.
rem It assumes ARB is in "\AgilityBook\src\AgilityBook"

set _DO_UNICODE=0
set _DO_MBCS=0

if ("%1")==("vc6") goto vc6
if ("%1")==("vc7") goto vc7
if ("%1")==("vc8") goto vc8
if ("%1")==("vc9") goto vc9
if ("%1")==("vc9x64") goto vc9x64
goto usage

:vc6
title VC6
call "C:\Program Files\Microsoft Visual Studio\VC98\bin\vcvars32.bat"
echo on
set _DO_MBCS=1
set _CFG=_VC6.0
set _RUNTIME_LIBS=dynamic
set _CPPFLAGS=
REM vc_lib_VC6.0
goto doit

:vc7
title VC7
call "C:\Program Files\Microsoft Visual Studio .NET 2003\Common7\Tools\vsvars32.bat"
set _DO_MBCS=1
set _CFG=_VC7.1s
set _RUNTIME_LIBS=static
set _CPPFLAGS=
REM vc_lib_VC7.1s
goto doit

:vc8
title VC8
call "C:\Program Files\Microsoft Visual Studio 8.0\VC\vcvarsall.bat" x86
set _DO_UNICODE=1
set _DO_MBCS=1
set _CFG=_VC8.0s
set _RUNTIME_LIBS=static
set _CPPFLAGS=
REM vc_lib_VC8.0s
goto doit

:vc9
title VC9
call "C:\Program Files\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86
set _DO_UNICODE=1
set _DO_MBCS=1
set _CFG=_VC9.0s
set _RUNTIME_LIBS=static
set _CPPFLAGS=/D_SECURE_SCL=1 /D_SECURE_SCL_THROWS=1
REM vc_lib_VC9.0s
goto doit

:vc9x64
title VC9x64
call "C:\Program Files\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86_amd64
set _DO_UNICODE=1
set _TARGET_CPU=TARGET_CPU=amd64
set _CFG=_VC9.0s
set _RUNTIME_LIBS=static
set _CPPFLAGS=/D_SECURE_SCL=1 /D_SECURE_SCL_THROWS=1
REM vc_amd64_lib_VC9.0s
goto doit


:doit
cd %WXWIN%\build\msw

if ("%_DO_UNICODE%")==("0") goto next
nmake -f makefile.vc BUILD=release            UNICODE=1 RUNTIME_LIBS=%_RUNTIME_LIBS% %_TARGET_CPU% CFG=%_CFG% CPPFLAGS="%_CPPFLAGS%"
nmake -f makefile.vc BUILD=debug DEBUG_INFO=1 UNICODE=1 RUNTIME_LIBS=%_RUNTIME_LIBS% %_TARGET_CPU% CFG=%_CFG% CPPFLAGS="%_CPPFLAGS%"

:next
if ("%_DO_MBCS%")==("0") goto next2
nmake -f makefile.vc BUILD=release            RUNTIME_LIBS=%_RUNTIME_LIBS% CFG=%_CFG% CPPFLAGS="%_CPPFLAGS%"
nmake -f makefile.vc BUILD=debug DEBUG_INFO=1 RUNTIME_LIBS=%_RUNTIME_LIBS% CFG=%_CFG% CPPFLAGS="%_CPPFLAGS%"

:next2
cd \AgilityBook\src\AgilityBook\src\Projects
goto done

:usage
echo Usage: %0 vc6 or vc7 or vc8 or vc9 or vc9x64

:done
set _DO_UNICODE=
set _DO_MBCS=
set _TARGET_CPU=
set _CFG=
set _RUNTIME_LIBS=

