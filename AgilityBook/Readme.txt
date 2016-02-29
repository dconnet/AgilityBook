Setting up the build environment
================================
(note, comments/etc are very Windows-oriented since that's my main platform.)
Additional software packages required (all free):
- wxWidgets
- poedit (includes msgcat)
- wxFormBuilder (optional)
- Boost libraries (not needed as of VC9+SP1)
- Windows Installer XML toolset
- Doxygen (optional)
- GraphViz (optional)
[See "Compiler notes" at bottom of file for supported compilers]

Once the above software is unpacked, the directory structure should look like:
  - AgilityBook/
    - src/
    - Readme.txt
    - ...
  - <some_path>/boost_<ver>/ [BOOST_ROOT points to this]
      - ...(Boost structure)
    I typically use \devtools\boost\boost_<ver>. Then my other 3rdparty tools
    also reside in devtools, for instance: \devtools\wx\wxWidgets-<ver>.

--------------------

wxWidgets: http://www.wxwidgets.org/
I'm currently using version 3.1.0.
Make sure WXWIN is set to wxWidgets root directory.
-- Note, when changing version used during release, update fr.po (see Readme
   in src/Win/res/fr_FR)

=== Changes to 3.1.0:
-[all]- in include/wx/msw/setup.h, enable everything to compile, plus:
  - Set WXWIN_COMPATIBILITY_2_8 to 0 (currently 1)
  - Specifically set wxDEBUG_LEVEL (uncomment ifdef/define items) (Otherwise
    the library is compiled one way and the users do something different.
  - Set wxUSE_STD_CONTAINERS to wxUSE_STD_DEFAULT
  - Set wxUSE_MEDIACTRL to 0 (currently 1)
  - Set wxUSE_INKEDIT to 1 (currently 0)
>diff -c textctrl.cpp.orig textctrl.cpp
*** textctrl.cpp.orig   Mon Feb 29 13:45:04 2016
--- textctrl.cpp        Mon Feb 29 13:25:44 2016
***************
*** 329,334 ****
--- 329,339 ----
      }
  #endif // wxUSE_DRAG_AND_DROP && wxUSE_RICHEDIT

+ #if wxUSE_INKEDIT && wxUSE_RICHEDIT
+     if (m_isInkEdit)
+         DissociateHandle();
+ #endif
+
      delete m_privateContextMenu;
  }

=== Changes to 3.0.2:
-[all]- in include/wx/msw/setup.h, enable everything to compile, plus:
  - Set WXWIN_COMPATIBILITY_2_8 to 0 (currently 1)
  - Specifically set wxDEBUG_LEVEL (uncomment ifdef/define items) (Otherwise
    the library is compiled one way and the users do something different.
  - Set wxUSE_STD_CONTAINERS to wxUSE_STD_DEFAULT
  - Set wxUSE_MEDIACTRL to 0 (currently 1)
  - Set wxUSE_INKEDIT to 1 (currently 0)
-[win]- src/msw/bmpcbox.cpp
    cross ported changes from trunk to fix hi-dpi issue with combo
c:\devtools\wx\wxWidgets-3.0.2\src\msw>diff -c bmpcbox.cpp.orig bmpcbox.cpp
*** bmpcbox.cpp.orig    Mon Oct 06 13:34:04 2014
--- bmpcbox.cpp Thu Jan 01 19:47:56 2015
***************
*** 409,415 ****

      // Draw default for item -1, which means 'focus rect only'
      if ( pos == -1 )
!         return FALSE;

      int flags = 0;
      if ( lpDrawItem->itemState & ODS_COMBOBOXEDIT )
--- 409,415 ----

      // Draw default for item -1, which means 'focus rect only'
      if ( pos == -1 )
!         return false;

      int flags = 0;
      if ( lpDrawItem->itemState & ODS_COMBOBOXEDIT )
***************
*** 449,457 ****
      LPMEASUREITEMSTRUCT lpMeasureItem = (LPMEASUREITEMSTRUCT) item;
      int pos = lpMeasureItem->itemID;

!     // Measure item height if item list is not empty,
      // otherwise leave default system value.
!     if ( pos >= 0 )
      {
          lpMeasureItem->itemHeight = wxBitmapComboBoxBase::MeasureItem(pos);
      }
--- 449,457 ----
      LPMEASUREITEMSTRUCT lpMeasureItem = (LPMEASUREITEMSTRUCT) item;
      int pos = lpMeasureItem->itemID;

!     // Measure edit field height if item list is not empty,
      // otherwise leave default system value.
!     if ( m_usedImgSize.y >= 0 || pos >= 0 )
      {
          lpMeasureItem->itemHeight = wxBitmapComboBoxBase::MeasureItem(pos);
      }

=== Changes for support VC14: https://forums.wxwidgets.org/viewtopic.php?t=40491
1) <wxdir>\src\zlib\gzguts.h - line 102
change:
#ifdef _MSC_VER
#  define snprintf _snprintf
#endif

to:
#if (defined(_MSC_VER) && (_MSC_VER < 1900))
  #define snprintf _snprintf
#endif

2) <wxdir>\src\tiff\libtiff\tif_config.h - line 367
change:
#define snprintf _snprintf

to:
#if (defined(_MSC_VER) && (_MSC_VER < 1900))
  #define snprintf _snprintf
#endif

3) <wxdir>\include\wx\propgrid\advprops.h - line 453
change:
wxDateTime GetDateValue() const
{
    //return m_valueDateTime;
    return m_value;
}

to:
wxDateTime GetDateValue() const
{
    //return m_valueDateTime;
    return m_value.GetDateTime();
}

=== Changes for support 64bit xcode: http://goharsha.com/blog/compiling-wxwidgets-3-0-2-mac-os-x-yosemite/
-[mac]- src/osx/webview_webkit.mm
(note, fixed in wx3.0.3)
src/osx/webview_webkit.mm: - line 31
change:
#include <WebKit/WebKit.h>
to:
#include <WebKit/WebKitLegacy.h>

=== Changes to support ink in richedit
-[win]- src/msw/textctrl.cpp
>diff -c textctrl.cpp.orig textctrl.cpp
*** textctrl.cpp.orig   Mon Oct 06 14:34:04 2014
--- textctrl.cpp        Thu Oct 22 14:35:14 2015
***************
*** 76,81 ****
--- 76,85 ----

  #endif // wxUSE_RICHEDIT

+ #if wxUSE_INKEDIT
+     #include <wx/dynlib.h>
+ #endif
+
  #include "wx/msw/missing.h"

  // FIXME-VC6: This seems to be only missing from VC6 headers.
***************
*** 339,344 ****
--- 343,353 ----
          m_dropTarget = NULL;
      }
  #endif // wxUSE_DRAG_AND_DROP && wxUSE_RICHEDIT
+
+ #if wxUSE_INKEDIT && wxUSE_RICHEDIT
+     if (m_isInkEdit)
+         DissociateHandle();
+ #endif

      delete m_privateContextMenu;
  }


=== Changes to 3.0.1
-[all]- in include/wx/msw/setup.h, enable everything to compile, plus:
  - Set WXWIN_COMPATIBILITY_2_8 to 0 (currently 1)
  - Specifically set wxDEBUG_LEVEL (uncomment ifdef/define items) (Otherwise
    the library is compiled one way and the users do something different.
  - Set wxUSE_STD_CONTAINERS to wxUSE_STD_DEFAULT
  - Set wxUSE_MEDIACTRL to 0 (currently 1)

=== Changes to 3.0.0:
-[all]- in include/wx/msw/setup.h, enable everything to compile, plus:
  - Set WXWIN_COMPATIBILITY_2_8 to 0 (currently 1)
  - Specifically set wxDEBUG_LEVEL (uncomment ifdef/define items) (Otherwise
    the library is compiled one way and the users do something different.
  - Set wxUSE_STD_CONTAINERS to wxUSE_STD_DEFAULT
  - Set wxUSE_MEDIACTRL to 0 (currently 1)
-[win]- src/msw/combobox.cpp
    http://trac.wxwidgets.org/changeset/75196
c:\devtools\wx\wxWidgets-3.0.0\src\msw>diff -c combobox.cpp.orig combobox.cpp
*** combobox.cpp.orig   Fri Nov 15 08:20:58 2013
--- combobox.cpp        Fri Nov 15 08:21:19 2013
***************
*** 683,689 ****
      // our own one. So we must explicitly check the HWND value too here and
      // avoid eating the events from the listbox as otherwise it is rendered
      // inoperative, see #15647.
!     if ( id == GetId() && hWnd != GetHWND() )
      {
          // Must be the case described above.
          return NULL;
--- 683,689 ----
      // our own one. So we must explicitly check the HWND value too here and
      // avoid eating the events from the listbox as otherwise it is rendered
      // inoperative, see #15647.
!     if ( id == GetId() && hWnd && hWnd != GetHWND() )
      {
          // Must be the case described above.
          return NULL;

=== wx2.9.x: Not supported
-  ARBv3 does not support pre 2.9. It uses new features.

To build for VC, see ./src/Projects/CompileWX.py
To build for Mac, see ./build/setupwx

--------------------

poedit: http://www.poedit.net
Cross platform editor for modifying .po files. Currently using 1.6.10.
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

Boost: http://www.boost.org.
- Boost is no longer required when using VC9+SP1 (or VC9FeaturePack). Note, the
  included project files now assume the Service Pack is installed with VS2008.
ARB has been built and tested using Boost version 1.56.0. There is no need
to actually build the Boost libraries. (Currently, only the smart_ptr,
weak_ptr and make_shared templates are used.)
[Minimum Boost version supported (for TR1): 1.38.0]
When the library is unpacked, it should be located according to the map
above. The default directory when unpacked is boost_1_56_0 (of course,
this will vary based on boost version). Set BOOST_ROOT to point to this
directory. The projects use this environment variable.

--------------------

Windows Installer XML toolset: http://wixtoolset.org/
Currently using Version 3.9.1006.0 (as of ARB v2.4.5).
- Install votive [optional]
- (1), run WiX installer. That will set the environment variable WIX.
  (GenMSI.py looks for "WIX" and appends "\bin")
- Or (2) Unzip binaries.zip into "C:\Tools\wix3"
  (GenMSI.py assumes WiX is installed here, unless WIX env var is set)

--------------------

Doxygen: http://www.stack.nl/~dimitri/doxygen
Used to create source code documentation. AgilityBook.dox uses v1.8.8.
[Install to default location]

--------------------

GraphViz: http://www.graphviz.org
Used to create source code documentation. AgilityBook.dox uses v2.38.0
Earlier versions may work. (I used 2.14.1 with no problems for a while)
[Install to default location]


Compiler notes
==============

Microsoft Visual Studio 6
=========================
   Not supported.
   It just can't handle namespaces well. I'm not going
   to keep mangling the source code to support this.

Microsoft Visual Studio .NET (VC7)
==================================
   Not supported.

Microsoft Visual Studio 2003 (VC7.1)
====================================
   Not supported.
   (dropped after moving to a new machine, no longer have compiler)

Microsoft Visual Studio 2005 (VC8)
==================================
   Not supported.
   (dropped after moving to a new machine, no longer have compiler)

Microsoft Visual Studio 2008 (VC9)
==================================
   Not supported.

Microsoft Visual Studio 2008 (VC9) + SP1
========================================
   Not supported. (no support for std::make_shared)

Microsoft Visual Studio 2010 (VC10)
===================================
   It works, no additional notes.
   [x86: UNICODE and MBCS] [note: MBCS support has been dropped]
   [x64: UNICODE only]
-- Old Express notes (ARB has moved to VC10Pro)
   - Must install Win7-.Net4 SDK first, in order to get x64 compiler
     (project won't open otherwise)
   - Turn on Expert Settings (Tools->Settings->Expert Settings)
   - Modify \Program Files\Microsoft Visual Studio 10.0\Common7\Tools\VCVarsQueryRegistry.bat:
     In the function GetWindowsSdkDirHelper, remove the "\7.0A" from the key
     path and change the value name to "CurrentInstallFolder". This will
     properly bring in the current SDK version.
   - Make sure you set the SDK in Platform Toolset (project properties)
     [this is done for ARB]

Microsoft Visual Studio 2012 (VC11)
===================================
   It works, no additional notes. But only targets Vista+.
   If 'vc110_xp' is set as the target platform, it appears to work, but the
   compilation of WX is not targeted at that platform, so it's not supported.

Microsoft Visual Studio 2013 (VC12)
===================================
   It works, no additional notes. But only targets Vista+.
   If 'vc120_xp' is set as the target platform, it appears to work, but the
   compilation of WX is not targeted at that platform, so it's not supported.

Microsoft Visual Studio 2015 (VC14)
===================================
   It works, no additional notes. But only targets Vista+.
   If 'vc140_xp' is set as the target platform, it appears to work, but the
   compilation of WX is not targeted at that platform, so it's not supported.


Xcode
=====
Install xcode command line tools
Install MacPorts: http://www.macports.org
sudo port -v selfupdate
sudo port upgrade outdated
initial: sudo port install autoconf
         sudo port install subversion
         sudo port install git

The xcode projects were used as follows:
.../xcode7: Xcode 7.x on OSX10.11
            From release notes: "The Xcode build system no longer automatically
            inherits the environment used to launch the app when running in the
            IDE" This means my use of $WXBASE is now broken. To fix, run this
            in bash:
              defaults write com.apple.dt.Xcode UseSanitizedBuildSystemEnvironment -bool NO
            Recommended (so projects work right):
            - Set DerivedData to Relative, 'build'
              - Advanced: Unique
            AgilityBook.xcworkspace
                wx3.0, cocoa, Current sdk, OSX10.7 deployment target
                x64 (dropped 32bit)
                C++ Language Dialect: c++11 (-std=c++11)
                C++ Std Lib: libc++

To determine clang precessor defines (works for gcc also):
  clang -dM -E -x c /dev/null

OSX 10.9:
- Create/add to /etc/launchd.conf (replace /Users/dconnet with your HOME)
  Must reboot after modifying.
===
setenv BOOST_ROOT /Users/dconnet/devtools/boost/boost_1_56_0
setenv WXBASE /Users/dconnet/devtools/wx
setenv WXWIN /Users/dconnet/devtools/wx/wxWidgets-3.0.2
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
    launchctl setenv BOOST_ROOT /Users/dconnet/devtools/boost/boost_1_56_0
    launchctl setenv WXBASE /Users/dconnet/devtools/wx
    launchctl setenv WXWIN /Users/dconnet/devtools/wx/wxWidgets-3.0.2
    </string>
  </array>
  <key>RunAtLoad</key>
  <true/>
</dict>
</plist>
