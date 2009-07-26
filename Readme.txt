Setting up the build environment
================================
Additional software packages required (all free):
- wxWidgets
- gettext
- wxFormBuilder (optional)
- UnitTest++ [see note below]
- Boost libraries (not needed as of VC9+SP1)
- Windows Installer XML toolset
- Doxygen (optional)
- GraphViz (optional)
[See "Compiler notes" at bottom of file for supported compilers]

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
I'm currently using version 2.8.10.
Make sure WXWIN is set to wxWidgets root directory.
There are a couple changes I've made:
- in include/wx/msw/setup.h, enable everything to compile, plus:
  - WXWIN_COMPATIBILITY_2_6 0
- include/msvc/wx/setup.h:
  This is hardcoded to include vc_lib/msw[u][d]/wx/setup.h,
  - Change it to include "wx/msw/setup.h"
    [unicode and nonunicode] (gets rid of all the ifdef sections)
- src/msw/stdpaths.cpp
  - GetAppDir (ln 254): Delete the __WXDEBUG__ section. This strips the 'debug'
    directory from the appdir, which causes problems.
- include/wx/choicebk.h:
Line 97:
    void UpdateSelectedPage(size_t newsel)
    {
        m_selection = wx_static_cast(int, newsel);
        GetChoiceCtrl()->Select(newsel);
    }
to:
    void UpdateSelectedPage(size_t newsel)
    {
        m_selection = wx_static_cast(int, newsel);
        GetChoiceCtrl()->Select(m_selection);  <-----
    }

To build for VC, see src/Projects/CompileWX.bat

--------------------

gettext: http://gnuwin32.sourceforge.net/packages/gettext.htm
This is required in order to create the language stuff.
Make sure the "\Program Files\GnuWin32\bin" is added to the PATH. (windows)
When initially developing:
- mkdir <lang>
- cd <lang>
- generate .po file
  - xgettext -C -n -k_ -kwxPLURAL:1,2 -kwxTRANSLATE -o arb.po ../arbframe.cpp
   cp ../../locale/<language>.po ./wxstd.po
   - or -
   cp ../../locale/wxstd.po .
This will generate an initial set of strings to translate.

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
- This has now been added into the source tree. Follow the UnitTest++
directions for compiling.

--------------------

Boost: http://www.boost.org.
- Boost is no longer required when using VC9+SP1 (or VC9FeaturePack). Note, the
  included project files now assume the Service Pack is installed with VS2008.
ARB has been built and tested using Boost version 1.39.0. There is no need
to actually build the Boost libraries. (Currently, only the smart_ptr and
weak_ptr templates are used.)
[also tested against 1.33.1, 1.34.0, 1.34.1, 1.35.1, 1.36.0, 1.37.0, 1.38.0
however, VC9 requires some tweaks to use versions older than 1.35.1]
When the library is unpacked, it should be located according to the map
above. The default directory when unpacked is boost_1_34_0 (of course,
this will vary based on boost version). This should be renamed to 'boost'
to avoid problems in the project files.

--------------------

Windows Installer XML toolset: http://wix.sourceforge.net/releases/
Currently using Version 3.0.5419.0 (as of ARB v2.0.1).
- Install votive [optional]
- Unzip binaries.zip into "C:\Tools\wix3'
  (GenMSI.py assumes WiX is installed here)

During our release process of running BuildAll.bat,
GenMSI.py will be called which generates the install files.
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
   [x86: MBCS only]

Microsoft Visual Studio .NET (VC7)
==================================
   Not supported.

Microsoft Visual Studio .NET 2003 (VC7.1)
=========================================
   It works, no additional notes.
   [x86: MBCS only]
   Note: Only 'kind-of' supported now. I can compile most of the code, but 2
   files cause an internal compiler error. So I can't generate a .exe now.

Microsoft Visual Studio .NET 2005 (VC8)
=======================================
   It works, no additional notes.
   [x86: UNICODE and MBCS]

Microsoft Visual Studio .NET 2008 (VC9)
=======================================
   Not supported.

Microsoft Visual Studio .NET 2008 (VC9) + SP1
=============================================
   It works, no additional notes.
   [x86: UNICODE and MBCS]
   [x64: UNICODE only]
