Setting up the build environment
================================
(Note, comments/etc are very Windows-oriented since that's my main platform.
Most of the Mac equivalents are installed via MacPorts, unix notes below)
Additional software packages required (all free):
- python
- wxWidgets
- poedit (includes msgcat)
- wxFormBuilder (optional)
- Windows Installer XML toolset
- Doxygen (optional)
- GraphViz (optional)
- clang-format (optional, but helpful)
- C++14 compiler
[See "Compiler notes" at bottom of file for supported compilers]

Once the above software is unpacked, the directory structure should look like:
  - AgilityBook/
    - src/
    - Readme.txt
    - ...

--------------------

python: https://www.python.org
Currently using version 3.9.5.
- Note, also install pyWin32 from https://github.com/mhammond/pywin32

wxWidgets: http://www.wxwidgets.org/
Currently using version 3.1.5. Min supported is 3.1.5.
Make sure WXWIN is set to wxWidgets root directory.
-- Note, when changing version used during release, update fr.po (see Readme
   in src/Win/res/fr_FR) IMPORTANT: This means the directory name in fr_FR
   must match the wx version number being used.
- Mac/Unix: Use .../build/setupwx.sh to setup WX build. And ". setwxpath.sh" to
  set the ARB compile environment.

=== Changes to 3.1.5:
  (include/wx/msw/setup.h)
  - Set wxWIN_COMPATIBILITY_3_0 to 0 (currently 1)
  - Set wxUSE_UNSAFE_WXSTRING_CONV to 0 (currently 1)
  - Specifically set wxDEBUG_LEVEL (uncomment ifdef/define items) (Otherwise
    the library is compiled one way and the users do something different.
  - Set wxUSE_STD_CONTAINERS to wxUSE_STD_DEFAULT
  - Set wxUSER_PRIVATE_FONTS to 0 (currently 1)
---
diff --git a/src/common/prntbase.cpp b/src/common/prntbase.cpp
index af5a731807..fd0651f164 100644
--- a/src/common/prntbase.cpp
+++ b/src/common/prntbase.cpp
@@ -1058,7 +1058,7 @@ void wxPreviewCanvas::OnMouseWheel(wxMouseEvent& event)
             else
                 delta = 50;

-            if ( event.GetWheelRotation() > 0 )
+            if ( event.GetWheelRotation() < 0 )
                 delta = -delta;

             int newZoom = currentZoom + delta;
---

To build for VC, see ./build/CompileWX.py
To build for Mac/Unix, see ./build/setupwx.sh

--------------------

poedit: http://www.poedit.net
Cross platform editor for modifying .po files.
Currently using version 3.0.
- includes gettext
  - on Mac, probably want to include MacPorts version
Use this to keep the catalog in sync with the source code.

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

wxFormBuilder: http://sourceforge.net/projects/wxformbuilder/
Useful for figuring out how a lay a dialog out.

--------------------

Windows Installer XML toolset: http://wixtoolset.org/
Currently using version 3.11.2.
- Install votive [optional]
- (1), run WiX installer. That will set the environment variable WIX.
  (GenMSI.py looks for "WIX" and appends "\bin")
- Or (2) Unzip binaries.zip into "C:\Tools\wix3"
  (GenMSI.py assumes WiX is installed here, unless WIX env var is set)

--------------------

Doxygen: http://www.stack.nl/~dimitri/doxygen
Used to create source code documentation.
Currently using version 1.8.20.
[Install to default location]

--------------------

GraphViz: http://www.graphviz.org
Used to create source code documentation.
Currently using version 2.47.2.
Earlier versions may work. (I used 2.14.1 with no problems for a while)
[Install to default location]

--------------------
LLVM: https://releases.llvm.org/download.html
Currently using version 12.0.0 (ubuntu18: v10, ubuntu20: v11)
[Install to default location, ubuntu: apt install]
- clang-format -i *.h *.cpp
This will run the formatter and rewrite the files.
(VS2019 automatically detected the .clang-format file.
  Edit->Advanced->Format Document)


Compiler notes
==============

Compiler: requires C++14 support.

VS-2017 and before
===================================
   Not supported.

Microsoft Visual Studio 2019 (VC14.2)
===================================
   It works, no additional notes.

Unix
====
(targeting GTK3, GTK2 with wx3.1 crashes when the new-doc button is pressed)
Needs (note, some aren't strictly needed):
sudo apt install git python3 python3-pip
sudo apt install libgtk-3-dev
sudo apt install libcanberra-gtk-module
- Not strictly needed, but unit tests will fail otherwise
sudo dpkg-reconfigure locales
 - Pick fr_FR and en_GB at a minimum (I only picked the UTF8 variants)

Xcode
=====
Install xcode command line tools
Install MacPorts: http://www.macports.org
sudo port -v selfupdate
sudo port upgrade outdated
initial: sudo port install autoconf gettext git python39 py39-readline
         sudo port select --set python3 python39
Download SDK 10.11 from github.com/phracker/MacOSX-SDKs/releases/
 (this is the min SDK wx supports - use that)
 Unpack into /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/

To determine clang precessor defines (works for gcc also):
  clang -dM -E -x c /dev/null

OSX 10.9:
- Create/add to /etc/launchd.conf (replace /Users/dconnet with your HOME)
  Must reboot after modifying.
===
setenv WXBASE /Users/dconnet/devtools/wx
setenv WXWIN /Users/dconnet/devtools/wx/wxWidgets-3.1.5
====
OSX 10.10+:
- launchd.conf has been deprecated.
  The simplest fix is to put the following into ~/Library/LaunchAgents/environment.plist
(http://stackoverflow.com/questions/25385934/setting-environment-variables-via-launchd-conf-no-longer-works-in-os-x-yosemite/26311753#26311753)
(Obviously adjust paths as needed)
  Must logoff/logon after modifying.

<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>Label</key>
  <string>my.startup</string>
  <key>ProgramArguments</key>
  <array>
    <string>sh</string>
    <string>-c</string>
    <string>
    launchctl setenv WXBASE /Users/dconnet/devtools/wx
    launchctl setenv WXWIN /Users/dconnet/devtools/wx/wxWidgets-3.1.5
    </string>
  </array>
  <key>RunAtLoad</key>
  <true/>
</dict>
</plist>
