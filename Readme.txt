Setting up the build environment
================================
HTML Help Workshop is required. This is a free download from Microsoft.
I'm currently using version 4.74.8702, as reported while compiling
the help file. Add the following directories to the include paths in
the compiler options (Tools->Options, Directories tab):
    Include files: "C:\Program Files\HTML Help WorkShop\include"
    Library files: "C:\Program Files\HTML Help WorkShop\lib"
(The above is the default location, your's may vary...)

In addition to the source tree here, you also need Xerces-C.
This can be obtained from http://xml.apache.org/dist/xerces-c/.
Once unpacked, the directory structure should look like:
  - AgilityBook/
    - src/
    - Readme.txt
    - ...
  - xml-xerces/
    - c/
      - ...(xerces structure)
This program has been tested with 2.2 and 2.7.
  - AgilityBook.cpp issues some warnings/comments about the version that
    is currently in use.
If you need to compile Xerces yourself, then the .dll files with are in
(Xerces)/Build/Win32/<Compiler>/Release/ should be copied to to
(arb)/bin/<compiler>/Release/. The project files will access
"../../../../xml-xerces/c/Build/Win32/<vc>/Release", so there's no need to
copy the .lib files.

Finally, the Boost libraries are needed: http://www.boost.org.
ARB has been built and tested using Boost version 1.33.1. There is no need
to actually build the Boost libraries. (Currently, only the smart_ptr and
weak_ptr templates are used.)
When the library is unpacked, it should be located in the same location
as xerces (above). The default directory when unpacked in boost_1_33_1
(of course, this will vary based on boost version). This should be renamed
to 'boost' (the project files will access this in the same way as xerces).
- Note: use of Boost may change in the future. Smart Ptrs are part of the
  TR1 spec. When compilers actually implemement TR1, I will change to that.


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
