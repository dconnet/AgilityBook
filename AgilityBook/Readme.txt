Setting up the build environment
================================
Additional software packages required (all free):
- HTML Help Workshop
- Boost libraries
- Windows Installer XML toolset (or Inno Setup)
- Xerces-C (included in src tree)
- zlib (included in src tree)

Once the above software is unpacked, the directory structure should look like:
  - AgilityBook/
    - src/
    - Readme.txt
    - ...
  - boost/
      - ...(Boost structure)

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
ARB has been built and tested using Boost version 1.34.1. There is no need
to actually build the Boost libraries. (Currently, only the smart_ptr and
weak_ptr templates are used.) [also tested against 1.33.1, and 1.34.0]
When the library is unpacked, it should be located according to the map
above. The default directory when unpacked in boost_1_34_0 (of course,
this will vary based on boost version). This should be renamed to 'boost'
to avoid problems in the project files.
- Note: use of Boost may change in the future. Smart Ptrs are part of the
  TR1 spec. When compilers actually implemement TR1, I will change to that.

--------------------

Windows Installer XML toolset: http://wix.sourceforge.net/releases/
Currently using Version 2.0.5805.0 (as of ARB v1.10.0.12).
- Install votive
- Unzip binaries.zip into installed directory as 'wix2'
  - This will create "C:\Program Files\Windows Installer XML\wix2"

Inno Setup: http://www.jrsoftware.org/isdl.php
Currently using Version 5.2.2, also ISTool addon.

During our release process of running BuildAll.bat,
GenMSI.py will be called which generates the install files.
By default, it will create installers using WiX.
(In ...\AgilityBook\src\Projects\VC8)

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
   Note - zlib/xerces are checked in using VC8+SP1

Microsoft Visual Studio .NET 2008 (VC9)
=======================================
   Boost: boost/config/compilers/visualc.hpp: Change 1400 to 1500 on last
   known version check (bottom of file). Also create a BOOST_COMPILER_VERSION
   macro above that.
