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
cd ..\src\Projects

REM Targets:
REM VC6
REM  Configuration: 'Release'/'Debug' (non-unicode)
REM  Platform: Win32
REM  Targets: AgilityBook, cal_usdaa, LibTidy
REM VC7.1
REM  Configuration: 'Release'/'Debug' (non-unicode)
REM  Platform: Win32
REM  Targets: AgilityBook, cal_usdaa, LibTidy
REM VC8
REM  Configuration: 'Release'/'Debug'/'Unicode Release'/'Unicode Debug'
REM  Platform: Win32, x64
REM  ARBHelp: 'Release'/'Debug'
REM  Targets:
REM   AgilityBook: all
REM   ARBHelp: no unicode
REM   cal_usdaa: all
REM   LibTidy: no unicode
REM VC9
REM  Configuration: 'Release'/'Debug'/'Release - No Unicode'/'Debug - No Unicode'
REM  Platform: Win32, x64
REM  Targets:
REM   AgilityBook: unicode only
REM   ARBHelp: unicode only
REM   cal_usdaa: unicode only
REM   LibTidy: no unicode [debug/release=non unicode]
REM   TestARB: all

cd VC9
del bldWin32.txt bldWin64.txt
REM Unicode (Release == Unicode)
call "C:\Program Files\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86_amd64
devenv AgilityBook.sln /out bldWin64.txt /build "Release|x64"
call "C:\Program Files\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86
devenv AgilityBook.sln /out bldWin32.txt /build "Release|Win32"
cd ..

REM We no longer support win98!!! (we could, but why?)
REM cd VC8
REM del bldWin98.txt
REM Non-unicode, must do in VC8 since VC9 no longer supports win98
REM (Release == non-unicode)
REM call "c:\Program Files\Microsoft Visual Studio 8\VC\vcvarsall.bat" x86
REM devenv AgilityBook.sln /out bldWin98.txt /build "Release|Win32"
REM cd ..

REM Go back to where we started
cd Installer

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
