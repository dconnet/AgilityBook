Setting up the build environment
================================
Additional software packages required (all free):
- wxWidgets
- gettext
- wxFormBuilder (optional)
- UnitTest++
- HTML Help Workshop
- Boost libraries (not needed as of VC9+SP1)
- Windows Installer XML toolset
- Doxygen (optional)
- GraphViz (optional)

Once the above software is unpacked, the directory structure should look like:
  - AgilityBook/
    - src/
    - Readme.txt
    - ...
  - boost/
      - ...(Boost structure)
  - UnitTest++/
      - ... (UnitTest structure)

--------------------

wxWidgets: http://www.wxwidgets.org/
I'm currently using version 2.8.9.
Currently, this is "installed" into c:\wx. The vsprops file is coded with
this knowledge. If you put it elsewhere, update the vsprops file (but don't
check it in!) There are a couple changes I've made:
- in \wx\include\wx\msw\setup.h, enable everything to compile, plus:
  - WXWIN_COMPATIBILITY_2_6 0
  - wxUSE_STL 1
- \wx\include\msvc\wx\setup.h:
  This is hardcoded to include vc_lib\msw[u][d]/wx/setup.h,
  - Change it to include "wx/msw/setup.h"
    [unicode and nonunicode] (gets rid of all the ifdef sections)
- \wx\src\msw\stdpaths.cpp
  - GetAppDir (ln 254): Delete the __WXDEBUG__ section. This strips the 'debug'
    directory from the appdir, which causes problems.
To build (for vc9):
(i386)
nmake -f makefile.vc BUILD=release UNICODE=1 RUNTIME_LIBS=static CFG=_VC9.0s CPPFLAGS="/D_SECURE_SCL=1 /D_SECURE_SCL_THROWS=1"
nmake -f makefile.vc BUILD=debug DEBUG_INFO=1 UNICODE=1 RUNTIME_LIBS=static CFG=_VC9.0s CPPFLAGS="/D_SECURE_SCL=1 /D_SECURE_SCL_THROWS=1"
(amd64)
nmake -f makefile.vc BUILD=release UNICODE=1 RUNTIME_LIBS=static TARGET_CPU=amd64 CFG=_VC9.0s CPPFLAGS="/D_SECURE_SCL=1 /D_SECURE_SCL_THROWS=1"
nmake -f makefile.vc BUILD=debug DEBUG_INFO=1 UNICODE=1 RUNTIME_LIBS=static TARGET_CPU=amd64 CFG=_VC9.0s CPPFLAGS="/D_SECURE_SCL=1 /D_SECURE_SCL_THROWS=1"
[for vc, there's src/Projects/CompileWX.bat to easily compile for vc6 thru 9]

--------------------

gettext: http://gnuwin32.sourceforge.net/packages/gettext.htm
This is required in order to create the language stuff.
Make sure the "\Program Files\GnuWin32\bin" is added to the PATH.

--------------------

wxFormBuilder: http://www.wxformbuilder.org/
Useful for figuring out how a lay a dialog out.

--------------------

UnitTest++: http://unittest-cpp.sourceforge.net/
I'm currently using version 1.4
If you don't want to run the unit tests, this is not required.
All project files have been copied/renamed/modified such that:
 "vsnet..." to "VC..."
 OutputDirectory: $(SolutionDir)\bin\$(PlatformName)
 IntermediateDirectory: $(OutDir)\$(ConfigurationName)\$(ProjectName)
 Unicode/Non-unicode/LIB/DLL configs added to VC9 projects
 .lib names changed to UnitTest++.VC<vcversion>[S][U][D].lib
 Projects now use common vsprops files
  - common, debug dll, debug lib, debug settings, (and release versions)

--------------------

HTML Help Workshop: This is a free download from Microsoft.
I'm currently using version 4.74.8702, as reported while compiling
the help file. Add the following directories to the include paths in
the compiler options (Tools->Options, Directories tab):
    Include files: "C:\Program Files\HTML Help WorkShop\include"
    Library files: "C:\Program Files\HTML Help WorkShop\lib"
(The above is the default location, your's may vary...)

--------------------

Boost: http://www.boost.org.
- Boost is no longer required when using VC9+SP1 (or VC9FeaturePack). Note, the
  included project files now assume the Service Pack is installed with VS2008.
ARB has been built and tested using Boost version 1.38.0. There is no need
to actually build the Boost libraries. (Currently, only the smart_ptr and
weak_ptr templates are used.)
[also tested against 1.33.1, 1.34.0, 1.34.1, 1.35.1, 1.36.0, 1.37.0
however, VC9 requires some tweaks to use versions older than 1.35.1]
When the library is unpacked, it should be located according to the map
above. The default directory when unpacked is boost_1_34_0 (of course,
this will vary based on boost version). This should be renamed to 'boost'
to avoid problems in the project files.

--------------------

Windows Installer XML toolset: http://wix.sourceforge.net/releases/
Currently using Version 2.0.5805.0 (as of ARB v1.10.0.12).
- Install votive [optional]
- Unzip binaries.zip into "C:\Tools\wix2'
  (GenMSI.py assumes WiX is installed here)

During our release process of running BuildAll.bat,
GenMSI.py will be called which generates the install files.
By default, it will create installers using WiX.
(In ...\AgilityBook\src\Projects\Installer)

--------------------

Doxygen: http://www.stack.nl/~dimitri/doxygen
Used to create source code documentation. AgilityBook.dox uses v1.5.8.
[Install to default location]

--------------------

GraphViz: http://www.graphviz.org
Used to create source code documentation. AgilityBook.dox uses v2.14.1.
[Install to default location]


Compiler notes
==============

Microsoft Visual Studio 6 + Service Pack 5 (VC6)
================================================
   Make sure you have updated the STL files with the patches noted at
   http://www.dinkumware.com/vc_fixes.html. (Last updated Nov 10, 2002)

Microsoft Visual Studio .NET (VC7)
==================================
   Not supported.

Microsoft Visual Studio .NET 2003 (VC7.1)
=========================================
   It works, no additional notes.

Microsoft Visual Studio .NET 2005 (VC8)
=======================================
   It works, no additional notes.

Microsoft Visual Studio .NET 2008 (VC9)
=======================================
   Not supported.

Microsoft Visual Studio .NET 2008 (VC9) + SP1
=============================================
   It works, no additional notes.
