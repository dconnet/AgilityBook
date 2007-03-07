devenv AgilityBook.sln /clean "Unicode Release|Win32"
devenv AgilityBook.sln /clean "Release|Win32"
devenv AgilityBook.sln /clean "Unicode Release|x64"

devenv AgilityBook.sln /build "Unicode Release|Win32" /out bldWin.txt
devenv AgilityBook.sln /build "Release|Win32" /out bldWin98.txt
devenv AgilityBook.sln /build "Unicode Release|x64" /out bldWin64.txt

GenMSI.py
