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


Compiler notes
==============

Microsoft Visual Studio 6 + Service Pack 5 (VC6)
================================================
   Make sure you have updated the STL files with the patches noted at
   http://www.dinkumware.com/vc_fixes.html. (Last updated Nov 10, 2002)

   Xerces: Uses DLL.

Microsoft Visual Studio .NET (VC7)
==================================
   Not supported.

Microsoft Visual Studio .NET 2003 (VC7.1)
=========================================
   It works, no additional notes.

   Xerces: Uses DLL.

Microsoft Visual Studio .NET 2005 (VC8)
=======================================
   It works, no additional notes.

   Xerces: Uses Static lib for Release and DLL for Debug
