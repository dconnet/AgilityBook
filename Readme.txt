Setting up the build environment
================================
Additional software packages required (all free):
- UnitTest++
- HTML Help Workshop
- Boost libraries (not needed as of VC9+SP1)
- Windows Installer XML toolset
- Xerces-C (included in src tree)
- zlib (included in src tree)
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

UnitTest++: http://unittest-cpp.sourceforge.net/
I'm currently using the SVN head (as of 2/19/2008, subject to change)
If you don't want to run the unit tests, this is not required.
All project files have been copied/renamed/modified such that:
 "vsnet..." to "VC..."
 OutputDirectory: $(SolutionDir)\bin\$(PlatformName)
 IntermediateDirectory: $(OutDir)\$(ConfigurationName)\$(ProjectName)
 Unicode/Non-unicode/LIB/DLL configs added to all projects (except vc7)
 .lib names changed to UnitTest++.VC<vcversion>[S][U][D].lib
 VC8/9 projects now use common vsprops files
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
ARB has been built and tested using Boost version 1.36.0. There is no need
to actually build the Boost libraries. (Currently, only the smart_ptr and
weak_ptr templates are used.)
[also tested against 1.33.1, 1.34.0, 1.34.1, 1.35.1
however, VC9 requires some tweaks to use versions older than 1.35.1]
When the library is unpacked, it should be located according to the map
above. The default directory when unpacked in boost_1_34_0 (of course,
this will vary based on boost version). This should be renamed to 'boost'
to avoid problems in the project files.
- Boost is no longer required when using VC9+SP1 (or FeaturePack). Note, the
  included project files now assume the Service Pack is installed with VS2008.

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

Xerces-C: http://xml.apache.org/dist/xerces-c/.
This used to be an external package, but has now been moved into ARBs
codeline as of v1.9.2.12. Xerces will now be upgraded as needed. [current 2.8]

[old comments]
This program has been tested with 2.2 and 2.7 dlls.
  - AgilityBook.cpp issues some warnings/comments about the version that
    is currently in use during compilation.
Note: To compile for x64, (Xerces)/src/xercesc/util/AutoSense.hpp needs
to be modified. In the '_WIN32 || WIN32' section, after:
[Note: this is currently checked in - if using xerces separately, do this]
    #ifndef WIN32
      #define WIN32
    #endif
add
	#if defined(_WIN64) && !defined(WIN64)
      #define WIN64
    #endif
Also, the Win32/VC8/... project files should be copied to x64/VC8/...
About the only changes were to modify the output paths, change the
DebugInformationFormat to '3' (VCCLCompilerTool), set the TargetMachine to 17
(VCLinkerTool) and set TargetEnvironment to 3 (VCMIDLTool). That was it.
ARB deviates from xerces structure here - instead of creating an x64 directory,
ARB uses VC8x64. I just didn't feel like creating another directory layer.
[This assumes you've installed the x64 libraries with VC8]

--------------------

zlib: http://www.zlib.net/
This used to be an external package, but has now been moved into ARBs
codeline as of v1.9.2.12.
I'm currently using v1.2.3.
The zlib source package has been pruned to just what is required.

--------------------

Doxygen: http://www.stack.nl/~dimitri/doxygen
Used to create source code documentation. AgilityBook.dox uses v1.5.6.
[Install to default location]

--------------------

GraphViz: http://www.graphviz.org
Used to create source code documentation. AgilityBook.dox uses v2.14.1.
Newer versions just don't seem to work right.
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
