Setting up the build environment
================================
Additional software packages required (all free):
- wxWidgets
- gettext
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
  - boost_<ver>/ [BOOST_ROOT points to this]
      - ...(Boost structure)

--------------------

wxWidgets: http://www.wxwidgets.org/
I'm currently using version 2.9.x (trunk).
Make sure WXWIN is set to wxWidgets root directory.
-- Note, when changing version used during release, update fr.po (see Readme
   in src/Win/res/fr_FR)

=== Changes to <trunk> [2.9.4]:
-[all]- in include/wx/msw/setup.h, enable everything to compile, plus:
  - WXWIN_COMPATIBILITY_2_8 0
  - Specifically set wxDEBUG_LEVEL (uncomment ifdef/define items) (Otherwise
    the library is compiled one way and the users do something different.
- Note: as of 8/8/2011, unicode doesn't work on Mac due to positional parameters.

=== wx2.9.x pre.3: Not supported
-  ARBv3 does not support pre 2.9. It uses new features.

To build for VC, see src/Projects/CompileWX.py

--------------------

gettext: http://gnuwin32.sourceforge.net/packages/gettext.htm
This is required in order to create the language stuff.
Make sure the "\Program Files\GnuWin32\bin" is added to the PATH. (windows)
When initially developing:
- mkdir <lang>
- cd <lang>
- generate .po file [update: see poedit]
  - xgettext -C -n -k_ -kwxPLURAL:1,2 -kwxTRANSLATE -o arb.po ../arbframe.cpp
   cp ../../locale/<language>.po ./wxstd.po
   - or -
   cp ../../locale/wxstd.po .
This will generate an initial set of strings to translate.

--------------------

poedit: http://www.poedit.net
Cross platform editor for modifying .po files. Currently using 1.5.4.
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
ARB has been built and tested using Boost version 1.52.0. There is no need
to actually build the Boost libraries. (Currently, only the smart_ptr and
weak_ptr templates are used.)
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
- Or (2) Unzip binaries.zip into "C:\Tools\wix3'
  (GenMSI.py assumes WiX is installed here, unless WIX env var is set)
  [I actually have c:\wix\wix<version>\ so I can test multiple versions]

--------------------

Doxygen: http://www.stack.nl/~dimitri/doxygen
Used to create source code documentation. AgilityBook.dox uses v1.8.2.
This is imcompatible with earlier versions.
[Install to default location]

--------------------

GraphViz: http://www.graphviz.org
Used to create source code documentation. AgilityBook.dox uses v2.28.0
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
   It works, no additional notes.
   [x86: UNICODE and MBCS] [note: MBCS support has been dropped]
   [x64: UNICODE only]

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
   It works, no additional notes.

XCode
=====
The xcode projects were used as follows:
.../macos: Xcode 3.1 on OSX10.5
           Targets trunk, carbon
.../mac-cocoa: Xcode 4.5 on OSX10.8
			   Targets trunk, cocoa
