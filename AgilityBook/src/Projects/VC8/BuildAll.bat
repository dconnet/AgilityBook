REM Note, the setup projects do not get cleaned. Doesn't appear possible...
rem devenv AgilityBook.sln /clean "Unicode Release|Win32"
rem devenv AgilityBook.sln /clean "Release|Win32"
rem devenv AgilityBook.sln /clean "Unicode Release|x64"

rem devenv AgilityBook.sln /build "Unicode Release|Win32" /project AgilityBookSetup.vdproj /out bldWin.txt

rem devenv AgilityBook.sln /build "Release|Win32" /project AgilityBookSetup98.vdproj /out bldWin98.txt

REM Note: Ignore errors from ParseResourceHeader during x64 build.
REM It obviously won't run on a 32bit system. And the 1st build make the chm.
REM (It's left in vcproj file for future use on a 64bit system)
devenv AgilityBook.sln /build "Unicode Release|x64" /project AgilityBookSetup64.vdproj /out bldWin64.txt
