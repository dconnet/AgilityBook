# Microsoft Developer Studio Project File - Name="LibZlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=LibZlib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LibZlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LibZlib.mak" CFG="LibZlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LibZlib - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LibZlib - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LibZlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../bin/VC6/Debug"
# PROP Intermediate_Dir "../../../bin/VC6/Debug/LibZlib"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /YX /FD /c
# ADD CPP /G6 /MDd /Za /W3 /Gm /GX /Zi /Od /I "..\..\zlib" /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /FD /c
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

!ELSEIF  "$(CFG)" == "LibZlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../bin/VC6/Release"
# PROP Intermediate_Dir "../../../bin/VC6/Release/LibZlib"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /MT /W3 /GX /O2 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /YX /FD /c
# ADD CPP /G6 /MD /Za /W3 /GX /O2 /Ob2 /I "..\..\zlib" /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /FD /c
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

# Name "LibZlib - Win32 Debug"
# Name "LibZlib - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter ".c,.cpp"
# Begin Source File

SOURCE=..\..\zlib\adler32.c
# End Source File
# Begin Source File

SOURCE=..\..\zlib\compress.c
# End Source File
# Begin Source File

SOURCE=..\..\zlib\crc32.c
# End Source File
# Begin Source File

SOURCE=..\..\zlib\deflate.c
# End Source File
# Begin Source File

SOURCE=..\..\zlib\gzio.c
# End Source File
# Begin Source File

SOURCE=..\..\zlib\infback.c
# End Source File
# Begin Source File

SOURCE=..\..\zlib\inffast.c
# End Source File
# Begin Source File

SOURCE=..\..\zlib\inflate.c
# End Source File
# Begin Source File

SOURCE=..\..\zlib\inftrees.c
# End Source File
# Begin Source File

SOURCE=..\..\zlib\trees.c
# End Source File
# Begin Source File

SOURCE=..\..\zlib\uncompr.c
# End Source File
# Begin Source File

SOURCE=..\..\zlib\zutil.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h,.hpp"
# Begin Source File

SOURCE=..\..\zlib\crc32.h
# End Source File
# Begin Source File

SOURCE=..\..\zlib\deflate.h
# End Source File
# Begin Source File

SOURCE=..\..\zlib\inffast.h
# End Source File
# Begin Source File

SOURCE=..\..\zlib\inffixed.h
# End Source File
# Begin Source File

SOURCE=..\..\zlib\inflate.h
# End Source File
# Begin Source File

SOURCE=..\..\zlib\inftrees.h
# End Source File
# Begin Source File

SOURCE=..\..\zlib\trees.h
# End Source File
# Begin Source File

SOURCE=..\..\zlib\zconf.h
# End Source File
# Begin Source File

SOURCE=..\..\zlib\zconf.in.h
# End Source File
# Begin Source File

SOURCE=..\..\zlib\zlib.h
# End Source File
# Begin Source File

SOURCE=..\..\zlib\zutil.h
# End Source File
# End Group
# End Target
# End Project
