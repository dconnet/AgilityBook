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

Changes to <trunk> [2.9.1]:
-[all]- in include/wx/msw/setup.h, enable everything to compile, plus:
  - WXWIN_COMPATIBILITY_2_8 0
  - Set wxDEBUG_LEVEL to 0 on NDEBUG
-[win]- include/msvc/wx/setup.h:
  - Change the wxLIB_SUBDIR (5 lines) to:
===begin
#if _MSC_VER == 1200
	#define wxLIB_BASEDIR	vc60
#elif _MSC_VER == 1310
	#define wxLIB_BASEDIR	vc71
#elif _MSC_VER == 1400
	#define wxLIB_BASEDIR	vc80
#elif _MSC_VER == 1500
	#define wxLIB_BASEDIR	vc90
#else
	#define wxLIB_BASEDIR	vc
#endif
#if defined(_M_X64)
	#define wxLIB_ARCH	_amd64
#else
	#define wxLIB_ARCH
#endif
    #ifdef WXUSINGDLL
        #define wxLIB_SUBDIR wxCONCAT3(wxLIB_BASEDIR, wxLIB_ARCH, _dll)
    #else // !DLL
        #define wxLIB_SUBDIR wxCONCAT3(wxLIB_BASEDIR, wxLIB_ARCH, _lib)
    #endif // DLL/!DLL
===end

wx2.9.0: Not supported

Changes to 2.8.10:
-[all]- in include/wx/msw/setup.h, enable everything to compile, plus:
  - WXWIN_COMPATIBILITY_2_6 0
-[win]- include/msvc/wx/setup.h:
  Add the following lines after the first ifdef:
===begin
    #include "wx/version.h"
    #include "wx/cpp.h"
#define wxCONCAT3(x1, x2, x3) wxCONCAT(wxCONCAT(x1, x2), x3)
#define wxCONCAT4(x1, x2, x3, x4) wxCONCAT(wxCONCAT3(x1, x2, x3), x4)
#define wxCONCAT5(x1, x2, x3, x4, x5) wxCONCAT(wxCONCAT4(x1, x2, x3, x4), x5)
    // notice that wxSUFFIX_DEBUG is a string but wxSUFFIX itself must be an
    // identifier as string concatenation is not done inside #include where we
    // need it
    #ifdef _DEBUG
        #define wxSUFFIX_DEBUG "d"
        #ifdef _UNICODE
            #define wxSUFFIX ud
        #else // !_UNICODE
            #define wxSUFFIX d
        #endif // _UNICODE/!_UNICODE
    #else
        #define wxSUFFIX_DEBUG ""
        #ifdef _UNICODE
            #define wxSUFFIX u
        #else // !_UNICODE
            #define wxSUFFIX
        #endif // _UNICODE/!_UNICODE
    #endif
#if _MSC_VER == 1200
	#define wxLIB_SUFFIX	_VC60
#elif _MSC_VER == 1310
	#define wxLIB_SUFFIX	_VC71
#elif _MSC_VER == 1400
	#define wxLIB_SUFFIX	_VC80
#elif _MSC_VER == 1500
	#define wxLIB_SUFFIX	_VC90
#else
	#define wxLIB_SUFFIX
#endif
#if defined(_M_X64)
	#define wxLIB_ARCH	_amd64
#else
	#define wxLIB_ARCH
#endif
    #ifdef WXUSINGDLL
        #define wxLIB_SUBDIR wxCONCAT4(vc, wxLIB_ARCH, _dll, wxLIB_SUFFIX)
    #else // !DLL
		#define wxLIB_SUBDIR wxCONCAT4(vc, wxLIB_ARCH, _lib, wxLIB_SUFFIX)
    #endif // DLL/!DLL
    // the real setup.h header file we need is in the build-specific directory,
    // construct the path to it
    #define wxSETUPH_PATH \
        wxCONCAT5(../../../lib/, wxLIB_SUBDIR, /msw, wxSUFFIX, /wx/setup.h)
    #define wxSETUPH_PATH_STR wxSTRINGIZE(wxSETUPH_PATH)
    #include wxSETUPH_PATH_STR
===end
  Delete all other #includes
-[win]- src/msw/stdpaths.cpp
  - GetAppDir (ln 254): Delete the __WXDEBUG__ section. This strips the 'debug'
    directory from the appdir, which causes problems.
-[all]- include/wx/choicebk.h:
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
