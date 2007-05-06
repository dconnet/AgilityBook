cd ..\..\..\bin
rd /q/s VC6
rd /q/s VC7
rd /q/s VC8
rd /q/s VC8x64
call CopyDLLs.bat
cd ..\src\Projects\VC8

devenv AgilityBook.sln /out bldWin32.txt /build "Unicode Release|Win32"
devenv AgilityBook.sln /out bldWin98.txt /build "Release|Win32"
devenv AgilityBook.sln /out bldWin64.txt /build "Unicode Release|x64"

REM Todo: Fix WiX, then trash vdproj
REM GenMSI.py
devenv AgilityBookSetup.sln /out bldWinsetup.txt /build
