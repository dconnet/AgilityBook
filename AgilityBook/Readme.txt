Compiler notes:

Microsoft Visual Studio 6 + Service Pack 5
==========================================
Make sure you have updated the STL files with the patches noted at
http://www.dinkumware.com/vc_fixes.html. (Last updated Nov 10, 2002)

Microsoft Visual Studio .NET 2003
=================================
It works, no additional notes.

Setting up the build environment
================================
In addition to the source tree here, you also need Xerces-C 2.2.0.
This can be obtained from http://xml.apache.org/dist/xerces-c/
Once unpacked, the directory structure should look like:
  - AgilityBook/
    - src/
    - Readme.txt
    - ...
  - xml-xerces/
    - c/
      - ...(xerces structure)
