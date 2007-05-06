devenv AgilityBook.sln /clean "Unicode Release|Win32"
devenv AgilityBook.sln /clean "Release|Win32"
devenv AgilityBook.sln /clean "Unicode Release|x64"

devenv AgilityBook.sln /out bldWin32.txt /build "Unicode Release|Win32"
devenv AgilityBook.sln /out bldWin98.txt /build "Release|Win32"
devenv AgilityBook.sln /out bldWin64.txt /build "Unicode Release|x64"

REM Todo: Fix WiX, then trash vdproj
REM GenMSI.py
devenv AgilityBookSetup.sln /out bldWinsetup.txt /build
