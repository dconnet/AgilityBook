@echo off
REM Generate the help file.
REM
REM Revision History
REM 2005-01-23 DRC Added History.html creation from a template.
REM 2004-05-20 DRC Created

if ("%1")==("Debug") goto doit
if ("%1")==("Release") goto doit
goto usage

:doit
if not exist "..\bin\Win32\VC7\%1\ParseResourceHeader.exe" goto usage

set OLDPATH=%PATH%
set PATH=c:\Program Files\HTML Help Workshop;%PATH%

REM Generate the context header files from the windows code
"..\bin\Win32\VC7\%1\ParseResourceHeader.exe" Win\resource.h Win\resource.hm Help\AgilityBook.h Help\contextid.h
REM Generate History.html
UpdateHistory.py -h
REM Now generate the chm file
hhc AgilityBook.hhp
REM Finally, copy the chm file into various locations.
REM /r:overwrite readonly, /q: don't show copied filename, /y:no prompt
echo Copying chm file to build output directories
xcopy /r/q/y Help\AgilityBook.chm "..\bin\Win32\VC6\Release\"
xcopy /r/q/y Help\AgilityBook.chm "..\bin\Win32\VC6\Debug\"
xcopy /r/q/y Help\AgilityBook.chm "..\bin\Win32\VC7\Release\"
xcopy /r/q/y Help\AgilityBook.chm "..\bin\Win32\VC7\Debug\"
goto done

set PATH=%OLDPATH%
set OLDPATH=

:usage
echo Usage: BuildHelp.bat [Release or Debug]

:done
