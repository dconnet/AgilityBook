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
- C++17 compiler
[See "Compiler notes" at bottom of file for supported compilers]

Once the above software is unpacked, the directory structure should look like:
  - AgilityBook/
    - src/
    - Readme.txt
    - ...
Note: If cloning from github, use the '--recurse-submodules' option with the
initial 'git clone'. If you had already cloned the repository without this
option, run 'git submodule update --init' to also get all dependent code.

--------------------

python: https://www.python.org
Currently using version 3.11.7.
- Include pip in install
- "pip install pywin32"

wxWidgets: http://www.wxwidgets.org/
Currently using version 3.2.6. Min supported is 3.1.6.
Make sure WXWIN is set to wxWidgets root directory.
-- Note, when changing version used during release, update fr.po (see Readme
   in AgilityBookLibs/lang/res/fr_FR) IMPORTANT: This means the directory name
   in fr_FR must match the wx version number being used.
- Mac/Unix: Use .../build/BuildUnix.sh to setup WX build. And ". setwxpath.sh"
  to set the ARB compile environment.

=== Changes to 3.3.0 (trunk)
  (include/wx/msw/setup.h)
  - Set wxWIN_COMPATIBILITY_3_0 to 0 (currently 1)
  - Set wxWIN_COMPATIBILITY_3_2 to 0 (currently 1)
  - Set wxUSE_UNSAFE_WXSTRING_CONV to 0 (currently 1)
  - Specifically set wxDEBUG_LEVEL (uncomment ifdef/define items) (Otherwise
    the library is compiled one way and the users do something different.)
  - Set wxUSER_PRIVATE_FONTS to 0 (currently 1)

=== Changes to 3.2.4, 3.2.5, 3.2.6, 3.2.3, Changes to 3.2.2.1, Changes to 3.2.1, Changes to 3.2.0
  (include/wx/msw/setup.h)
  - Set wxWIN_COMPATIBILITY_3_0 to 0 (currently 1)
  - Set wxUSE_UNSAFE_WXSTRING_CONV to 0 (currently 1)
  - Specifically set wxDEBUG_LEVEL (uncomment ifdef/define items) (Otherwise
    the library is compiled one way and the users do something different.
  - Set wxUSE_STD_CONTAINERS to wxUSE_STD_DEFAULT
  - Set wxUSER_PRIVATE_FONTS to 0 (currently 1)

=== Changes to 3.2.3 only
>diff -c wxWidgets-3.2.3.orig\src\msw\thread.cpp wxWidgets-3.2.3\src\msw\thread.cpp
*** wxWidgets-3.2.3.orig\src\msw\thread.cpp     Sat Oct 07 07:28:50 2023
--- wxWidgets-3.2.3\src\msw\thread.cpp  Sun Oct 15 12:16:21 2023
***************
*** 201,207 ****
--- 201,212 ----

      static void CleanUp()
      {
+         // To avoid bogus memory leaks reports when using debug version of
+         // static MSVC CRT we need to free memory ourselves even when it would
+         // have been done by FlsAlloc() callback because it does it too late.
+ #if !defined(_MSC_VER) || !defined(_DEBUG) || defined(_DLL)
          if (!Instance().AllocCallback)
+ #endif
          {
              // FLS API was not available, which means that objects will not be freed automatically.
              delete Get();
***************
*** 1465,1470 ****
--- 1470,1478 ----

  void wxThreadModule::OnExit()
  {
+     // Delete thread-specific info object for the main thread too, if any.
+     wxThreadSpecificInfoTLS::CleanUp();
+
      if ( !::TlsFree(gs_tlsThisThread) )
      {
          wxLogLastError(wxT("TlsFree failed."));

=== Changes to 3.2.3, Changes to 3.2.2.1: (for VisualStudio 17.6)
>diff itemid.h.orig itemid.h
36c36
<     operator const Type() const { return m_pItem; }
---
>     operator Type() const { return m_pItem; }

=== Changes to 3.1.7:
  (include/wx/msw/setup.h)
  - Set wxWIN_COMPATIBILITY_3_0 to 0 (currently 1)
  - Set wxUSE_UNSAFE_WXSTRING_CONV to 0 (currently 1)
  - Specifically set wxDEBUG_LEVEL (uncomment ifdef/define items) (Otherwise
    the library is compiled one way and the users do something different.
  - Set wxUSE_STD_CONTAINERS to wxUSE_STD_DEFAULT
  - Set wxUSER_PRIVATE_FONTS to 0 (currently 1)
---The following changes are patches to trunk following the 3.1.7 release
Apply dec11e7
https://github.com/dconnet/wxWidgets/commit/dec11e76427b1811133d67af8ac5c1371a59f420
This is a patch from trunk that fixes an OSW text ctrl size issue.
--
diff --git a/src/msw/dc.cpp b/src/msw/dc.cpp
index 9c7dde05ab..412c5e081c 100644
--- a/src/msw/dc.cpp
+++ b/src/msw/dc.cpp
@@ -1231,7 +1231,7 @@ void wxMSWDCImpl::DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wx
     }

     (void)RoundRect(GetHdc(), XLOG2DEV(x), YLOG2DEV(y), XLOG2DEV(x2),
-        YLOG2DEV(y2), (int) (2*XLOG2DEV(radius)), (int)( 2*YLOG2DEV(radius)));
+        YLOG2DEV(y2), (int) (2*radius), (int)( 2*radius));

     CalcBoundingBox(x, y, x2, y2);
 }
--
diff --git a/src/generic/listctrl.cpp b/src/generic/listctrl.cpp
index d8434a6960..894db14b2d 100644
--- a/src/generic/listctrl.cpp
+++ b/src/generic/listctrl.cpp
@@ -4906,6 +4906,10 @@ void wxGenericListCtrl::Init()

 wxGenericListCtrl::~wxGenericListCtrl()
 {
+    // Don't wait until the base class does it because our subwindows expect
+    // their parent window to be a wxListCtrl, but this won't be the case any
+    // more when we get to the base class dtor (it will be only a wxWindow).
+    DestroyChildren();
 }
--

=== Changes to 3.1.6:
  (include/wx/msw/setup.h)
  - Set wxWIN_COMPATIBILITY_3_0 to 0 (currently 1)
  - Set wxUSE_UNSAFE_WXSTRING_CONV to 0 (currently 1)
  - Specifically set wxDEBUG_LEVEL (uncomment ifdef/define items) (Otherwise
    the library is compiled one way and the users do something different.
  - Set wxUSE_STD_CONTAINERS to wxUSE_STD_DEFAULT
  - Set wxUSER_PRIVATE_FONTS to 0 (currently 1)

Apply dc2ff0d
https://github.com/wxWidgets/wxWidgets/commit/dc2ff0d6b42b93dc84b886571bc6008393721f1e
This is a patch from trunk (3.1.7) that fixes a layout issue when changing a
font in a text control on OSX.


To build for VC, see ./build/CompileWX.py
To build for Mac/Unix, see ./build/BuildUnix.sh

--------------------

poedit: http://www.poedit.net
Cross platform editor for modifying .po files.
Currently using version 3.4.2.
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
Useful for figuring out how to lay a dialog out.

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
Currently using version 1.9.3. (Note: Newer versions cause issues in wx)
[Install to default location]

--------------------

GraphViz: http://www.graphviz.org
Used to create source code documentation.
Currently using version 9.0.0.
Earlier versions may work. (I used 2.14.1 with no problems for a while)
[Install to default location]

--------------------
LLVM: https://releases.llvm.org/download.html
Currently using version 17.0.6 (ubuntu18: v10, ubuntu20: v12, ubuntu22: v14)
[Install to default location, ubuntu: apt install]
- clang-format -i *.h *.cpp
  (ubuntu: clang-format-14 -i *.h *.cpp)
This will run the formatter and rewrite the files.
(VS2019/22 automatically detected the .clang-format file.
  Edit->Advanced->Format Document)


Compiler notes
==============

Compiler: requires C++17 support.

Microsoft Visual Studio 2022 (VC14.3)
===================================
   It works, no additional notes.

Microsoft Visual Studio 2019 (VC14.2)
===================================
   It works, no additional notes.

VS-2017 and before
===================================
   Not supported.

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
Ubuntu18: Install python3.8, do not do an update-alternatives (you will kill
ubuntu).

Xcode
=====
Install xcode command line tools
Install MacPorts: http://www.macports.org
sudo port -v selfupdate
sudo port upgrade outdated
initial: sudo port install autoconf gettext git python311 py311-readline
         sudo port select --set python python311
         sudo port select --set python3 python311
Note: I'm now compiling on an M2 macOS Monterey.

Note, to build using the xcode project, you need to enable xcode to use env vars
  defaults write com.apple.dt.Xcode UseSanitizedBuildSystemEnvironment -bool NO

To determine clang precessor defines (works for gcc also):
  clang -dM -E -x c /dev/null

OSX 10.9:
- Create/add to /etc/launchd.conf (replace /Users/dconnet with your HOME)
  Must reboot after modifying.
===
setenv WXBASE /Users/dconnet/devtools/wx
setenv WXWIN /Users/dconnet/devtools/wx/wxWidgets-3.2.4
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
    launchctl setenv WXWIN /Users/dconnet/devtools/wx/wxWidgets-3.2.4
    </string>
  </array>
  <key>RunAtLoad</key>
  <true/>
</dict>
</plist>
