REM Note, the setup projects do not get cleaned. Doesn't appear possible...
devenv AgilityBook.sln /clean "Unicode Release|Win32"
devenv AgilityBook.sln /clean "Release|Win32"
devenv AgilityBook.sln /clean "Unicode Release|x64"

devenv AgilityBook.sln /build "Unicode Release|Win32" /project AgilityBookSetup.vdproj /out bldWin.txt

devenv AgilityBook.sln /build "Release|Win32" /project AgilityBookSetup98.vdproj /out bldWin98.txt

devenv AgilityBook.sln /build "Unicode Release|x64" /project AgilityBookSetup64.vdproj /out bldWin64.txt
