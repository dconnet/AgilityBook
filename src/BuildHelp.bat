@echo off
if ("%1")==("Debug") goto doit
if ("%1")==("Release") goto doit
goto usage

:doit
if not exist "..\bin\Win32\VC7\%1\ParseResourceHeader.exe" goto usage

set OLDPATH=%PATH%
set PATH=c:\Program Files\HTML Help Workshop;%PATH%

"..\bin\Win32\VC7\%1\ParseResourceHeader.exe" Win\resource.h Win\resource.hm Help\AgilityBook.h Help\contextid.h
hhc AgilityBook.hhp
REM /r:overwrite readonly, /q: don't show copied filename, /y:no prompt
echo Copying chm file to build output directories
xcopy /r/q/y Help\AgilityBook.chm "..\bin\Win32\VC6\%1\"
xcopy /r/q/y Help\AgilityBook.chm "..\bin\Win32\VC7\%1\"
goto done

set PATH=%OLDPATH%
set OLDPATH=

:usage
echo Usage: BuildHelp.bat [Release or Debug]

:done
