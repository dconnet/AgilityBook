# Microsoft Developer Studio Generated NMAKE File, Based on cal_usdaa.dsp
!IF "$(CFG)" == ""
CFG=cal_usdaa - Win32 Debug
!MESSAGE No configuration specified. Defaulting to cal_usdaa - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "cal_usdaa - Win32 Debug" && "$(CFG)" != "cal_usdaa - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cal_usdaa.mak" CFG="cal_usdaa - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cal_usdaa - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cal_usdaa - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cal_usdaa - Win32 Debug"

OUTDIR=.\../../../bin/VC6/Debug
INTDIR=.\../../../bin/VC6/Debug/cal_usdaa
# Begin Custom Macros
OutDir=.\../../../bin/VC6/Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\cal_usdaa.dll"

!ELSE 

ALL : "LibTidy - Win32 Debug" "$(OUTDIR)\cal_usdaa.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"LibTidy - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ARBDate.obj"
	-@erase "$(INTDIR)\ARBString.obj"
	-@erase "$(INTDIR)\ARBTypes.obj"
	-@erase "$(INTDIR)\cal_usdaa.obj"
	-@erase "$(INTDIR)\cal_usdaa.pch"
	-@erase "$(INTDIR)\cal_usdaa.res"
	-@erase "$(INTDIR)\CalendarSite.obj"
	-@erase "$(INTDIR)\Element.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\cal_usdaa.dll"
	-@erase "$(OUTDIR)\cal_usdaa.exp"
	-@erase "$(OUTDIR)\cal_usdaa.ilk"
	-@erase "$(OUTDIR)\cal_usdaa.lib"
	-@erase "$(OUTDIR)\cal_usdaa.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../.." /I "../../Include" /I "../../ARB" /I "../../cal_usdaa" /I "../../../../boost" /I "C:/wx/include" /I "C:/wx/include/msvc" /D "WXWIDGETS" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WINDLL" /Fp"$(INTDIR)\cal_usdaa.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\cal_usdaa.res" /i "../../Include" /i "C:/wx/include" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cal_usdaa.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=ws2_32.lib comctl32.lib rpcrt4.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\cal_usdaa.pdb" /debug /machine:I386 /def:"..\..\cal_usdaa\cal_usdaa.def" /out:"$(OUTDIR)\cal_usdaa.dll" /implib:"$(OUTDIR)\cal_usdaa.lib" /pdbtype:sept /libpath:"C:\wx\lib\vc_lib_VC6.0" 
DEF_FILE= \
	"..\..\cal_usdaa\cal_usdaa.def"
LINK32_OBJS= \
	"$(INTDIR)\cal_usdaa.obj" \
	"$(INTDIR)\CalendarSite.obj" \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\ARBDate.obj" \
	"$(INTDIR)\ARBString.obj" \
	"$(INTDIR)\ARBTypes.obj" \
	"$(INTDIR)\Element.obj" \
	"$(INTDIR)\cal_usdaa.res" \
	"$(OUTDIR)\LibTidy.lib"

"$(OUTDIR)\cal_usdaa.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cal_usdaa - Win32 Release"

OUTDIR=.\../../../bin/VC6/Release
INTDIR=.\../../../bin/VC6/Release/cal_usdaa
# Begin Custom Macros
OutDir=.\../../../bin/VC6/Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\cal_usdaa.dll"

!ELSE 

ALL : "LibTidy - Win32 Release" "$(OUTDIR)\cal_usdaa.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"LibTidy - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ARBDate.obj"
	-@erase "$(INTDIR)\ARBString.obj"
	-@erase "$(INTDIR)\ARBTypes.obj"
	-@erase "$(INTDIR)\cal_usdaa.obj"
	-@erase "$(INTDIR)\cal_usdaa.pch"
	-@erase "$(INTDIR)\cal_usdaa.res"
	-@erase "$(INTDIR)\CalendarSite.obj"
	-@erase "$(INTDIR)\Element.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\cal_usdaa.dll"
	-@erase "$(OUTDIR)\cal_usdaa.exp"
	-@erase "$(OUTDIR)\cal_usdaa.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GR /GX /O2 /I "../.." /I "../../Include" /I "../../ARB" /I "../../cal_usdaa" /I "../../../../boost" /I "C:/wx/include" /I "C:/wx/include/msvc" /D "WXWIDGETS" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WINDLL" /Fp"$(INTDIR)\cal_usdaa.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\cal_usdaa.res" /i "../../Include" /i "C:/wx/include" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cal_usdaa.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=ws2_32.lib comctl32.lib rpcrt4.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\cal_usdaa.pdb" /machine:I386 /def:"..\..\cal_usdaa\cal_usdaa.def" /out:"$(OUTDIR)\cal_usdaa.dll" /implib:"$(OUTDIR)\cal_usdaa.lib" /libpath:"C:\wx\lib\vc_lib_VC6.0" 
DEF_FILE= \
	"..\..\cal_usdaa\cal_usdaa.def"
LINK32_OBJS= \
	"$(INTDIR)\cal_usdaa.obj" \
	"$(INTDIR)\CalendarSite.obj" \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\ARBDate.obj" \
	"$(INTDIR)\ARBString.obj" \
	"$(INTDIR)\ARBTypes.obj" \
	"$(INTDIR)\Element.obj" \
	"$(INTDIR)\cal_usdaa.res" \
	"$(OUTDIR)\LibTidy.lib"

"$(OUTDIR)\cal_usdaa.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("cal_usdaa.dep")
!INCLUDE "cal_usdaa.dep"
!ELSE 
!MESSAGE Warning: cannot find "cal_usdaa.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "cal_usdaa - Win32 Debug" || "$(CFG)" == "cal_usdaa - Win32 Release"
SOURCE=..\..\cal_usdaa\cal_usdaa.cpp

"$(INTDIR)\cal_usdaa.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cal_usdaa.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\cal_usdaa\cal_usdaa.rc

!IF  "$(CFG)" == "cal_usdaa - Win32 Debug"


"$(INTDIR)\cal_usdaa.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\cal_usdaa.res" /i "../../Include" /i "C:/wx/include" /i "\AgilityBook\src\AgilityBook\src\cal_usdaa" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "cal_usdaa - Win32 Release"


"$(INTDIR)\cal_usdaa.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\cal_usdaa.res" /i "../../Include" /i "C:/wx/include" /i "\AgilityBook\src\AgilityBook\src\cal_usdaa" /d "NDEBUG" $(SOURCE)


!ENDIF 

SOURCE=..\..\cal_usdaa\CalendarSite.cpp

"$(INTDIR)\CalendarSite.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cal_usdaa.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\stdafx.cpp

!IF  "$(CFG)" == "cal_usdaa - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../.." /I "../../Include" /I "../../ARB" /I "../../cal_usdaa" /I "../../../../boost" /I "C:/wx/include" /I "C:/wx/include/msvc" /D "WXWIDGETS" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WINDLL" /Fp"$(INTDIR)\cal_usdaa.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\stdafx.obj"	"$(INTDIR)\cal_usdaa.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "cal_usdaa - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GR /GX /O2 /I "../.." /I "../../Include" /I "../../ARB" /I "../../cal_usdaa" /I "../../../../boost" /I "C:/wx/include" /I "C:/wx/include/msvc" /D "WXWIDGETS" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WINDLL" /Fp"$(INTDIR)\cal_usdaa.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\stdafx.obj"	"$(INTDIR)\cal_usdaa.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\ARB\ARBDate.cpp

"$(INTDIR)\ARBDate.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cal_usdaa.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBString.cpp

"$(INTDIR)\ARBString.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cal_usdaa.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBTypes.cpp

"$(INTDIR)\ARBTypes.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cal_usdaa.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\Element.cpp

"$(INTDIR)\Element.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\cal_usdaa.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!IF  "$(CFG)" == "cal_usdaa - Win32 Debug"

"LibTidy - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\LibTidy.mak CFG="LibTidy - Win32 Debug" 
   cd "."

"LibTidy - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\LibTidy.mak CFG="LibTidy - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "cal_usdaa - Win32 Release"

"LibTidy - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\LibTidy.mak CFG="LibTidy - Win32 Release" 
   cd "."

"LibTidy - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\LibTidy.mak CFG="LibTidy - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ENDIF 


!ENDIF 

