@echo off
if ("%1")==("Debug") goto doit
if ("%1")==("Release") goto doit
goto usage

:doit
if not exist "..\bin\Win32\VC7\%1\ParseResourceHeader.exe" goto usage

set OLDPATH=%PATH%
set PATH=c:\Program Files\HTML Help Workshop;%PATH%

"..\bin\Win32\VC7\%1\ParseResourceHeader.exe" Win\resource.hm >Help\AgilityBook.h
hhc AgilityBook.hhp
goto done

set PATH=%OLDPATH%
set OLDPATH=

:usage
echo Usage: BuildHelp.bat [Release or Debug]

:done
