Setting up the build environment
================================
Additional software packages required:
- HTML Help Workshop
- Boost libraries
- Xerces-C
- zlib

HTML Help Workshop: This is a free download from Microsoft.
I'm currently using version 4.74.8702, as reported while compiling
the help file. Add the following directories to the include paths in
the compiler options (Tools->Options, Directories tab):
    Include files: "C:\Program Files\HTML Help WorkShop\include"
    Library files: "C:\Program Files\HTML Help WorkShop\lib"
(The above is the default location, your's may vary...)

Boost: http://www.boost.org.
ARB has been built and tested using Boost version 1.33.1. There is no need
to actually build the Boost libraries. (Currently, only the smart_ptr and
weak_ptr templates are used.)
When the library is unpacked, it should be located in the same location
as xerces (above). The default directory when unpacked in boost_1_33_1
(of course, this will vary based on boost version). This should be renamed
to 'boost' (the project files will access this in the same way as xerces).
- Note: use of Boost may change in the future. Smart Ptrs are part of the
  TR1 spec. When compilers actually implemement TR1, I will change to that.

Xerces-C: http://xml.apache.org/dist/xerces-c/.
This program has been tested with 2.2 and 2.7.
  - AgilityBook.cpp issues some warnings/comments about the version that
    is currently in use.
If you need to compile Xerces yourself, then the .dll files are in
(Xerces)/Build/Win32/<Compiler>/Release/ and should be copied to
(arb)/bin/<compiler>/Release/. The lib files need to be copied into
(arb)/lib/... This works with both UNICODE and MBCS compiles.

zlib: http://www.zlib.net/
I'm currently using v1.2.3, with some makefile changes.
(Note, also fixed zlib1.rc so version really is 1.2.3)
- Change CFLAGS option '-MD' to '-MT'
- Change STATICLIB to zlib_s.lib
         SHAREDLIB to arbzlib.dll
         IMPLIB    to arbzlib.lib
  (since we're compiling our own version, this deals with the zlib request
  to keep zlib1.dll a pure product adhering to certain requirements)
- Copy win32\Makefile.msc to win64\Makefile.msc
  - Change '/dWIN32' to '/dWIN64'
-VS6:
 - Start a command shell and invoke VCVARS32.BAT.
 - Run 'nmake -f win32\Makefile.msc'.
 - Copy zdll.lib/zlib1.dll
 - Run 'nmake -f win32\Makefile.msc clean'.
-VS7
 - Start a VS2003 command shell
 - Run 'nmake -f win32\Makefile.msc'.
 - Copy zdll.lib/zlib1.dll
 - Run 'nmake -f win32\Makefile.msc clean'.
-VS8
 - Start a VS2005 command shell
 - Run 'nmake -f win32\Makefile.msc'.
 - Copy zdll.lib/zlib1.dll
 - Run 'nmake -f win32\Makefile.msc clean'.
 - Start a VS2005x64 Tools command shell
 - Run 'nmake -f win64\Makefile.msc'.
 - Copy zdll.lib/zlib1.dll
 - Run 'nmake -f win64\Makefile.msc clean'.

----------

Once the above software is unpacked, the directory structure should look like:
  - AgilityBook/
    - src/
    - Readme.txt
    - ...
  - boost/
      - ...(boost structure)
  - xml-xerces/
    - c/
      - ...(xerces structure)
  - zlib/
      - ...(zlib structure)

----------

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
   Note - xerces is checked in using VC8+SP1
