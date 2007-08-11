# Microsoft Developer Studio Project File - Name="LibTidy" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

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
!MESSAGE "LibTidy - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "LibTidy - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
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
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\tidy\include" /I "..\..\tidy\src" /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "SUPPORT_UTF16_ENCODINGS" /D "SUPPORT_ASIAN_ENCODINGS" /D "SUPPORT_ACCESSIBILITY_CHECKS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

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
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\..\tidy\include" /I "..\..\tidy\src" /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "SUPPORT_UTF16_ENCODINGS" /D "SUPPORT_ASIAN_ENCODINGS" /D "SUPPORT_ACCESSIBILITY_CHECKS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

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
