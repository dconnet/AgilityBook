set OLDPATH=%PATH%
set OLDINCLUDE=%INCLUDE%
set OLDLIB=%LIB%
set OLDLIBPATH=%LIBPATH%
set OLDDevEnvDir=%DevEnvDir%
set OLDFramework35Version=%Framework35Version%
set OLDFrameworkDir=%FrameworkDir%
set OLDFrameworkVersion=%FrameworkVersion%
set OLDVCINSTALLDIR=%VCINSTALLDIR%
set OLDVSINSTALLDIR=%VSINSTALLDIR%
set OLDWindowsSdkDir=%WindowsSdkDir%

cd ..\..\..\bin
rd /q/s VC6
rd /q/s VC7
rd /q/s VC8Win32
rd /q/s VC8x64
rd /q/s VC9Win32
rd /q/s VC9x64

cd ..\src\Projects\VC8
del bldWin98.txt
REM Non-unicode, must do in VC8 since VC9 no longer supports win98
call "c:\Program Files\Microsoft Visual Studio 8\VC\vcvarsall.bat" x86
devenv AgilityBook.sln /out bldWin98.txt /build "Release|Win32"

cd ..\VC9
del bldWin32.txt bldWin64.txt
REM Unicode

call "C:\Program Files\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86_amd64
REM There's a problem with x64 dependencies - they don't build.
devenv AgilityBook.sln /out bldWin64.txt /build "Release|x64" /project "LibZlib"
devenv AgilityBook.sln /out bldWin64.txt /build "Release|x64" /project "LibTidy"
devenv AgilityBook.sln /out bldWin64.txt /build "Release|x64" /project "LibXerces"
devenv AgilityBook.sln /out bldWin64.txt /build "Release|x64" /project "ARBHelp"
devenv AgilityBook.sln /out bldWin64.txt /build "Release|x64" /project "cal_usdaa"
devenv AgilityBook.sln /out bldWin64.txt /build "Release|x64" /project "AgilityBookFRA"
devenv AgilityBook.sln /out bldWin64.txt /build "Release|x64" /project "AgilityBook"

call "C:\Program Files\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86
devenv AgilityBook.sln /out bldWin32.txt /build "Release|Win32"

set PATH=%OLDPATH%
set INCLUDE=%OLDINCLUDE%
set LIB=%OLDLIB%
set LIBPATH=%OLDLIBPATH%
set DevEnvDir=%OLDDevEnvDir%
set Framework35Version=%OLDFramework35Version%
set FrameworkDir=%OLDFrameworkDir%
set FrameworkVersion=%OLDFrameworkVersion%
set VCINSTALLDIR=%OLDVCINSTALLDIR%
set VSINSTALLDIR=%OLDVSINSTALLDIR%
set WindowsSdkDir=%OLDWindowsSdkDir%
set OLDPATH=
set OLDINCLUDE=
set OLDLIB=
set OLDLIBPATH=
set OLDDevEnvDir=
set OLDFramework35Version=
set OLDFrameworkDir=
set OLDFrameworkVersion=
set OLDVCINSTALLDIR=
set OLDVSINSTALLDIR=
set OLDWindowsSdkDir=
