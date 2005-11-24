Setting up the build environment
================================
In addition to the source tree here, you also need Xerces-C.
The version required is listed by compiler. This can be obtained from
http://xml.apache.org/dist/xerces-c/. Once unpacked, the directory
structure should look like:
  - AgilityBook/
    - src/
    - Readme.txt
    - ...
  - xml-xerces/
    - c/
      - ...(xerces structure)

HTML Help Workshop is required. This is a free download from Microsoft.
I'm currently using version 4.74.8702, as reported while compiling
the help file. Add the following directories to the include paths in
the compiler options (Tools->Options, Directories tab):
	Include files: "C:\Program Files\HTML Help WorkShop\include"
	Library files: "C:\Program Files\HTML Help WorkShop\lib"
(The above is the default location, your's may vary...)


Compiler notes
==============

Microsoft Visual Studio 6 + Service Pack 5 (VC6)
================================================
   Make sure you have updated the STL files with the patches noted at
   http://www.dinkumware.com/vc_fixes.html. (Last updated Nov 10, 2002)

   Xerces: The binaries (lib/dll) that are currently checked in the
   source control are for Xerces 2.2. (Linked to dll)

Microsoft Visual Studio .NET (VC7)
==================================
   Not supported.

Microsoft Visual Studio .NET 2003 (VC7.1)
=========================================
   It works, no additional notes.

   Xerces: The binaries (lib/dll) that are currently checked in the
   source control are for Xerces 2.2. (Linked to dll)

Microsoft Visual Studio .NET 2005 (VC8)
=======================================
   It works, no additional notes.

   Xerces: The binaries (lib) that are currently checked in the
   source control are for Xerces 2.7. (Linked to static library)
