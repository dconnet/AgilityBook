Setting up the build environment
================================
Additional software packages required (all free):
- HTML Help Workshop
- Boost libraries
- Windows Installer XML toolset
- Xerces-C
- zlib (included in src tree)

Once the above software is unpacked, the directory structure should look like:
  - AgilityBook/
    - src/
    - Readme.txt
    - ...
  - boost/
      - ...(Boost structure)
  - wix/
      - ...(WiX structure)
  - xml-xerces/
    - c/
      - ...(Xerces structure)

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
ARB has been built and tested using Boost version 1.34.0. There is no need
to actually build the Boost libraries. (Currently, only the smart_ptr and
weak_ptr templates are used.) [also tested against 1.33.1]
When the library is unpacked, it should be located according to the map
above. The default directory when unpacked in boost_1_34_0 (of course,
this will vary based on boost version). This should be renamed to 'boost'
to avoid problems in the project files.
- Note: use of Boost may change in the future. Smart Ptrs are part of the
  TR1 spec. When compilers actually implemement TR1, I will change to that.

--------------------

Windows Installer XML toolset: http://wix.sourceforge.net/
Currently using Version 2.0 (as of ARB v1.8.3.12).
- wix-2.0.4820.0-binaries.zip
- wix-2.0.4820.0-sources.zip
Unzip both of these into the 'wix' directory. In order to have the installer
work the way we want (don't display a license agreement), there are some
modifications needed.
- Copy ...\AgilityBook\Misc\Installation\*.bmp to ...\wix\Bitmaps\
- Copy ...\AgilityBook\Misc\Installation\WixUI_InstallDir.wxs to
  ...\wix\src\ui\wixui\installdir\
- Add ...\wix to your PATH, cd to wix
- "cd src\ui\wixui"
- "candle installdir\WixUI_InstallDir.wxs *.wxs"
- "lit -out ..\..\..\arbwixui.wixlib *.wixobj"
- "del *.wixobj"
Now, during our release process of running BuildAll.bat, GenMSI.py will
be called which uses WiX to generate the .msi install files.
(In ...\AgilityBook\src\Projects\VC8)

--------------------

Xerces-C: http://xml.apache.org/dist/xerces-c/.
This program has been tested with 2.2 and 2.7.
  - AgilityBook.cpp issues some warnings/comments about the version that
    is currently in use during compilation.
If you need to compile Xerces yourself, then the .dll/.lib files are in
(Xerces)/Build/Win32/<Compiler>/Release/ and should be copied to
(arb)/lib/... This works with both UNICODE and MBCS compiles.
- Changes to project files: xerceslib (only one we build)
  - On C/C++ Code Generation options:
    Change runtime library to multi-threaded NOT DLL

Note: To compile for x64, (Xerces)/src/xercesc/util/AutoSense.hpp needs
to be modified. In the '_WIN32 || WIN32' section, after:
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
I'm currently using v1.2.3. This used to be an external package, but has now
been moved into ARBs codeline as of v1.9.2.12. The zlib source package has
been pruned to just what is required.


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
