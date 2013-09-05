Setting up the build environment
================================
Additional software packages required (all free):
- wxWidgets
- gettext (MinGW)
- poedit
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
  - <some_path>/boost_<ver>/ [BOOST_ROOT points to this]
      - ...(Boost structure)

(note, comments/etc are very Windows-oriented since that's my main platform.)

--------------------

wxWidgets: http://www.wxwidgets.org/
I'm currently using version 2.8.12. Previously released with 2.8.10/.11.
Also, kept current with development trunk of 2.9.x.
Make sure WXWIN is set to wxWidgets root directory.
-- Note, when changing version used during release, update fr.po (see Readme
   in src/Win/res/fr_FR)

=== Changes to <trunk> [2.9.5]:
-[all]- in include/wx/msw/setup.h, enable everything to compile, plus:
  - Set WXWIN_COMPATIBILITY_2_8 to 0 (currently 1)
  - Specifically set wxDEBUG_LEVEL (uncomment ifdef/define items) (Otherwise
    the library is compiled one way and the users do something different.
  - Set wxUSE_STD_CONTAINERS to wxUSE_STD_DEFAULT (currently 0)
- src/msw/bmpcbox.cpp
====
 --- src/msw/bmpcbox.cpp (revision 74692)
 +++ src/msw/bmpcbox.cpp (working copy)
 @@ -175,7 +175,7 @@
          if ( !objectClientData.empty() )
              SetClientObject(i, objectClientData[i]);
          else if ( !voidClientData.empty() )
 -            SetClientData(i, objectClientData[i]);
 +            SetClientData(i, voidClientData[i]);
      }

      // and make sure it has the same attributes as before
==== Note, this diff is actually against a post-2.9.5 version in trunk
==== Once the wx version is bumped, 2.9.5 will not be supported

=== pre wx2.9.5: Not supported
- There were API changes in .3, so I simply don't support .0, .1, or .2.

=== Changes to 2.8.12:
-[win]- in src/msw/windows.cpp, the include for "pbt.h" needs to be deleted
        when using VS2012. This compiler stopped including this header.
-[all]- in include/wx/msw/setup.h, enable everything to compile, plus:
  - WXWIN_COMPATIBILITY_2_6 0
-[win]- include/msvc/wx/setup.h:
  [same as 2.8..10]
-[win]- src/msw/stdpaths.cpp
  - GetAppDir (ln 254): Delete the __WXDEBUG__ section. This strips the 'debug'
    directory from the appdir, which causes problems.
-[all]- src/generic/odcombo.cpp
  - In wxVListBoxComboPopup::Insert, add the following after m_strings.Insert
    m_clientDatas.Insert(0,pos);
  [This probably applies to all prior versions of wx, just found in 2.8.12]
-[mac]- src/common/wxchar.cpp
  - This fixes positional parameter formatting on Mac. Now possible to use unicode.
Output of "diff -c" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
*** wxchar.cpp.orig	Tue Mar 22 04:59:42 2011
--- wxchar.cpp	Mon Aug 08 18:58:26 2011
***************
*** 1499,1513 ****
      {
          if ( CopyFmtChar(*format++) == _T('%') )
          {
!             // skip any flags
!             while ( IsFlagChar(*format) )
!                 CopyFmtChar(*format++);
  
!             // and possible width
!             if ( *format == _T('*') )
!                 CopyFmtChar(*format++);
!             else
!                 SkipDigits(&format);
  
              // precision?
              if ( *format == _T('.') )
--- 1499,1542 ----
      {
          if ( CopyFmtChar(*format++) == _T('%') )
          {
! #if wxUSE_PRINTF_POS_PARAMS 
!             if( *format >= '0' && *format <= '9' ) 
!             { 
!                 SkipDigits(&format); 
!                 if( *format == '$' ) 
!                 { 
!                     // It was a positional printf argument 
!  
!                     CopyFmtChar(*format++); 
!  
!                     // skip any flags 
!                     while ( IsFlagChar(*format) ) 
!                         CopyFmtChar(*format++); 
!  
!                     // and possible width 
!                     if ( *format == _T('*') ) 
!                         CopyFmtChar(*format++); 
!                     else 
!                         SkipDigits(&format); 
!                 } 
!                 else 
!                 { 
!                     // Must have been width 
!                 } 
!             } 
!             else 
! #endif 
!             {
!                 // skip any flags
!                 while ( IsFlagChar(*format) )
!                     CopyFmtChar(*format++);
  
!                 // and possible width
!                 if ( *format == _T('*') )
!                     CopyFmtChar(*format++);
!                 else
!                     SkipDigits(&format);
!             }
  
              // precision?
              if ( *format == _T('.') )
end Output of "diff -c" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


=== Changes to 2.8.11:
-[all]- in include/wx/msw/setup.h, enable everything to compile, plus:
  - WXWIN_COMPATIBILITY_2_6 0
-[win]- include/msvc/wx/setup.h:
  [same as 2.8..10]
-[win]- src/msw/stdpaths.cpp
  - GetAppDir (ln 254): Delete the __WXDEBUG__ section. This strips the 'debug'
    directory from the appdir, which causes problems.
-[win]- src/msw/ole/automtn.cpp:
  - Line 716: Move to after date.Set call.
  [This probably applies to all prior versions of wx, just found in 2.8.11]

=== Changes to 2.8.10:
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
#if _MSC_VER == 1310
    #define wxLIB_SUFFIX _VC71
#elif _MSC_VER == 1400
    #define wxLIB_SUFFIX _VC80
#elif _MSC_VER == 1500
    #define wxLIB_SUFFIX _VC90
#elif _MSC_VER == 1600
    #define wxLIB_SUFFIX _VC100
#elif _MSC_VER == 1700
    #define wxLIB_SUFFIX _VC110
#else
    #define wxLIB_SUFFIX
#endif
#if defined(_M_X64)
    #define wxLIB_ARCH _amd64
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

To build for VC, see src/Projects/CompileWX.py

--------------------

gettext: [windows] http://sourceforge.net/projects/mingw/files/Installer
         [mac] Part of MacPorts
This is required in order to create the language stuff.
Make sure "C:\MinGW\bin" is added to the PATH. (windows)
When initially developing:
- mkdir <lang>
- cd <lang>
- generate .po file [update: see poedit]
  - xgettext -C -n -k_ -kwxPLURAL:1,2 -kwxTRANSLATE -o arb.po ../arbframe.cpp
   cp ../../locale/<language>.po ./wxstd.po
   - or -
   cp ../../locale/wxstd.po .
This will generate an initial set of strings to translate.

MinGW: After installing the Installation Manager (currently 0.6.0 beta),
select the mingw32-gettext/bin package. Update. That's all. (Plus anything
else you might want) Currently using Gettext v0.18.3.1.

--------------------

poedit: http://www.poedit.net
Cross platform editor for modifying .po files. Currently using 1.5.7.
(makes use of gettext)
Use this to keep the catalog in sync with the source code.

--------------------

wxFormBuilder: http://sourceforge.net/projects/wxformbuilder/
Useful for figuring out how a lay a dialog out.

--------------------

UnitTest++: http://unittest-cpp.sourceforge.net/
I'm currently using version 1.4
If you don't want to run the unit tests, this is not required.
All VC project files have been copied/renamed/modified such that:
 "vsnet..." to "VC..."
 OutputDirectory: $(SolutionDir)\bin\$(PlatformName)
 IntermediateDirectory: $(OutDir)\$(ConfigurationName)\$(ProjectName)
 Unicode/Non-unicode/LIB/DLL configs added to VC9 projects
 .lib names changed to UnitTest++.VC<vcversion>[S][U][D].lib
 Projects now use common vsprops files
  - common, debug dll, debug lib, debug settings, (and release versions)
- The original makefile has been renamed to Makefile.orig and Makefile.in
  has been added so it can easily be compiled on Mac.
- In addition, the .cpp files in Posix/ have been copied into the root
  src directory as posix_*.cpp to avoid subdirectory make issues.
- This has now been added into the source tree. Follow the UnitTest++
  directions for compiling. [On Windows, you must specifically compile this.
  On Mac, it is included in the current make process]

--------------------

Boost: http://www.boost.org.
- Boost is no longer required when using VC9+SP1 (or VC9FeaturePack). Note, the
  included project files now assume the Service Pack is installed with VS2008.
ARB has been built and tested using Boost version 1.54.0. There is no need
to actually build the Boost libraries. (Currently, only the smart_ptr,
weak_ptr and make_shared templates are used.)
[Minimum Boost version supported (for TR1): 1.38.0]
When the library is unpacked, it should be located according to the map
above. The default directory when unpacked is boost_1_52_0 (of course,
this will vary based on boost version). Set BOOST_ROOT to point to this
directory. The projects use this environment variable.

--------------------

Windows Installer XML toolset: http://wixtoolset.org/
Currently using Version 3.5.2519.0 (as of ARB v2.2.6).
- Install votive [optional]
- (1), run WiX installer. That will set the environment variable WIX.
  (GenMSI.py looks for "WIX" and appends "\bin")
- Or (2) Unzip binaries.zip into "C:\Tools\wix3"
  (GenMSI.py assumes WiX is installed here, unless WIX env var is set)
  [I actually have c:\wix\wix<version>\ so I can test multiple versions]

--------------------

Doxygen: http://www.stack.nl/~dimitri/doxygen
Used to create source code documentation. AgilityBook.dox uses v1.8.3.1.
[Install to default location]

--------------------

GraphViz: http://www.graphviz.org
Used to create source code documentation. AgilityBook.dox uses v2.32.0
Earlier versions may work. (I used 2.14.1 with no problems for a while)
[Install to default location]


Compiler notes
==============

Microsoft Visual Studio 6
=========================
   Not supported.
   It just can't handle namespaces well. I'm not going
   to keep mangling the source code to support this.

Microsoft Visual Studio .NET (VC7)
==================================
   Not supported.

Microsoft Visual Studio 2003 (VC7.1)
====================================
   Not supported.
   (dropped after moving to a new machine, no longer have compiler)

Microsoft Visual Studio 2005 (VC8)
==================================
   Not supported.
   (dropped after moving to a new machine, no longer have compiler)

Microsoft Visual Studio 2008 (VC9)
==================================
   Not supported.

Microsoft Visual Studio 2008 (VC9) + SP1
========================================
   Not supported. (no support for std::make_shared)

Microsoft Visual Studio 2010 (VC10)
===================================
   It works, no additional notes.
   [x86: UNICODE and MBCS] [note: MBCS support has been dropped]
   [x64: UNICODE only]
-- Old Express notes (ARB has moved to VC10Pro)
   - Must install Win7-.Net4 SDK first, in order to get x64 compiler
     (project won't open otherwise)
   - Turn on Expert Settings (Tools->Settings->Expert Settings)
   - Modify \Program Files\Microsoft Visual Studio 10.0\Common7\Tools\VCVarsQueryRegistry.bat:
     In the function GetWindowsSdkDirHelper, remove the "\7.0A" from the key
     path and change the value name to "CurrentInstallFolder". This will
     properly bring in the current SDK version.
   - Make sure you set the SDK in Platform Toolset (project properties)
     [this is done for ARB]

Microsoft Visual Studio 2012 (VC11)
===================================
   It works, no additional notes. But only on Vista+.
   If 'vc110_xp' is set as the target platform, it appears to work, but the
   compilation of WX is not targeted at that platform, so it's not supported.


Xcode
=====
Install MacPorts: http://www.macports.org
sudo port -v selfupdate
sudo port upgrade outdated
initial: sudo port install autoconf
         sudo port install subversion

The xcode projects were used as follows:
.../xcode3: Xcode 3.1 on OSX10.5
            AgilityBook.xcodeproj
                wx2.8 (production), carbon, OSX10.4 sdk
                trunk, carbon, OSX10.5 sdk
            AgilityBookCocoa.xcodeproj
                trunk, cocoa, OSX10.5 sdk

OSX 10.8:
- Create/add to /etc/launchd.conf (replace /Users/dconnet with your HOME)
  Must reboot after modifying.
===
setenv BOOST_ROOT /Users/dconnet/src/boost_1_54_0
setenv WXBASE /Users/dconnet/wx
setenv WXWIN /Users/dconnet/wx/trunk
====
