cd ..\..\..\bin
rd /q/s VC6
rd /q/s VC7
rd /q/s VC8Win32
rd /q/s VC8x64
cd ..\src\Projects\VC8

del bldWin32.txt bldWin98.txt bldWin64.txt bldWinsetup.txt

devenv AgilityBook.sln /out bldWin32.txt /build "Unicode Release|Win32"
devenv AgilityBook.sln /out bldWin98.txt /build "Release|Win32"
devenv AgilityBook.sln /out bldWin64.txt /build "Unicode Release|x64"

REM Todo: Fix WiX, then trash vdproj
REM GenMSI.py
devenv AgilityBookSetup.sln /out bldWinsetup.txt /build
