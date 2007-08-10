# Microsoft Developer Studio Project File - Name="LibTidy" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=LibTidy - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LibTidy.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LibTidy.mak" CFG="LibTidy - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LibTidy - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LibTidy - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LibTidy - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../bin/VC6/Debug"
# PROP Intermediate_Dir "../../../bin/VC6/Debug/LibTidy"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /YX /FD /c
# ADD CPP /G6 /MDd /Za /W3 /Gm /GX /Zi /Od /I "..\..\tidy\include" /I "..\..\tidy\src" /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "SUPPORT_UTF16_ENCODINGS" /D "SUPPORT_ASIAN_ENCODINGS" /D "SUPPORT_ACCESSIBILITY_CHECKS" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /lib /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib advapi32.lib ws2_32.lib /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "LibTidy - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../bin/VC6/Release"
# PROP Intermediate_Dir "../../../bin/VC6/Release/LibTidy"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /MT /W3 /GX /O2 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /YX /FD /c
# ADD CPP /G6 /MD /Za /W3 /GX /O2 /Ob2 /I "..\..\tidy\include" /I "..\..\tidy\src" /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "SUPPORT_UTF16_ENCODINGS" /D "SUPPORT_ASIAN_ENCODINGS" /D "SUPPORT_ACCESSIBILITY_CHECKS" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib advapi32.lib ws2_32.lib /subsystem:windows /lib /machine:I386
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "LibTidy - Win32 Debug"
# Name "LibTidy - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter ".c,.cpp"
# Begin Source File

SOURCE="..\..\tidy\src\access.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\alloc.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\attrask.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\attrdict.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\attrget.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\attrs.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\buffio.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\clean.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\config.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\entities.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\fileio.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\istack.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\lexer.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\localize.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\mappedio.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\parser.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\pprint.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\streamio.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\tagask.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\tags.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\tidylib.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\tmbstr.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\utf8.c"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\win32tc.c"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h,.hpp"
# Begin Source File

SOURCE="..\..\tidy\src\access.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\attrdict.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\attrs.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\include\buffio.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\clean.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\config.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\entities.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\fileio.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\forward.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\lexer.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\mappedio.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\message.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\parser.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\include\platform.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\pprint.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\streamio.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\tags.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\tidy-int.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\include\tidy.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\include\tidyenum.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\tmbstr.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\utf8.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\version.h"
# End Source File
# Begin Source File

SOURCE="..\..\tidy\src\win32tc.h"
# End Source File
# End Group
# End Target
# End Project
