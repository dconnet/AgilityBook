# Microsoft Developer Studio Generated NMAKE File, Based on LibTidy.dsp
!IF "$(CFG)" == ""
CFG=LibTidy - Win32 Debug
!MESSAGE No configuration specified. Defaulting to LibTidy - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "LibTidy - Win32 Debug" && "$(CFG)" != "LibTidy - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LibTidy - Win32 Debug"

OUTDIR=.\../../../bin/VC6/Debug
INTDIR=.\../../../bin/VC6/Debug/LibTidy
# Begin Custom Macros
OutDir=.\../../../bin/VC6/Debug
# End Custom Macros

ALL : "$(OUTDIR)\LibTidy.lib"


CLEAN :
	-@erase "$(INTDIR)\access.obj"
	-@erase "$(INTDIR)\alloc.obj"
	-@erase "$(INTDIR)\attrask.obj"
	-@erase "$(INTDIR)\attrdict.obj"
	-@erase "$(INTDIR)\attrget.obj"
	-@erase "$(INTDIR)\attrs.obj"
	-@erase "$(INTDIR)\buffio.obj"
	-@erase "$(INTDIR)\clean.obj"
	-@erase "$(INTDIR)\config.obj"
	-@erase "$(INTDIR)\entities.obj"
	-@erase "$(INTDIR)\fileio.obj"
	-@erase "$(INTDIR)\istack.obj"
	-@erase "$(INTDIR)\lexer.obj"
	-@erase "$(INTDIR)\localize.obj"
	-@erase "$(INTDIR)\mappedio.obj"
	-@erase "$(INTDIR)\parser.obj"
	-@erase "$(INTDIR)\pprint.obj"
	-@erase "$(INTDIR)\streamio.obj"
	-@erase "$(INTDIR)\tagask.obj"
	-@erase "$(INTDIR)\tags.obj"
	-@erase "$(INTDIR)\tidylib.obj"
	-@erase "$(INTDIR)\tmbstr.obj"
	-@erase "$(INTDIR)\utf8.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\win32tc.obj"
	-@erase "$(OUTDIR)\LibTidy.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LINK32=link.exe -lib
CPP_PROJ=/nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\tidy\include" /I "..\..\tidy\src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_LIB" /D "SUPPORT_UTF16_ENCODINGS" /D "SUPPORT_ASIAN_ENCODINGS" /D "SUPPORT_ACCESSIBILITY_CHECKS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\LibTidy.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\LibTidy.lib" 
LIB32_OBJS= \
	"$(INTDIR)\access.obj" \
	"$(INTDIR)\alloc.obj" \
	"$(INTDIR)\attrask.obj" \
	"$(INTDIR)\attrdict.obj" \
	"$(INTDIR)\attrget.obj" \
	"$(INTDIR)\attrs.obj" \
	"$(INTDIR)\buffio.obj" \
	"$(INTDIR)\clean.obj" \
	"$(INTDIR)\config.obj" \
	"$(INTDIR)\entities.obj" \
	"$(INTDIR)\fileio.obj" \
	"$(INTDIR)\istack.obj" \
	"$(INTDIR)\lexer.obj" \
	"$(INTDIR)\localize.obj" \
	"$(INTDIR)\mappedio.obj" \
	"$(INTDIR)\parser.obj" \
	"$(INTDIR)\pprint.obj" \
	"$(INTDIR)\streamio.obj" \
	"$(INTDIR)\tagask.obj" \
	"$(INTDIR)\tags.obj" \
	"$(INTDIR)\tidylib.obj" \
	"$(INTDIR)\tmbstr.obj" \
	"$(INTDIR)\utf8.obj" \
	"$(INTDIR)\win32tc.obj"

"$(OUTDIR)\LibTidy.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "LibTidy - Win32 Release"

OUTDIR=.\../../../bin/VC6/Release
INTDIR=.\../../../bin/VC6/Release/LibTidy
# Begin Custom Macros
OutDir=.\../../../bin/VC6/Release
# End Custom Macros

ALL : "$(OUTDIR)\LibTidy.lib"


CLEAN :
	-@erase "$(INTDIR)\access.obj"
	-@erase "$(INTDIR)\alloc.obj"
	-@erase "$(INTDIR)\attrask.obj"
	-@erase "$(INTDIR)\attrdict.obj"
	-@erase "$(INTDIR)\attrget.obj"
	-@erase "$(INTDIR)\attrs.obj"
	-@erase "$(INTDIR)\buffio.obj"
	-@erase "$(INTDIR)\clean.obj"
	-@erase "$(INTDIR)\config.obj"
	-@erase "$(INTDIR)\entities.obj"
	-@erase "$(INTDIR)\fileio.obj"
	-@erase "$(INTDIR)\istack.obj"
	-@erase "$(INTDIR)\lexer.obj"
	-@erase "$(INTDIR)\localize.obj"
	-@erase "$(INTDIR)\mappedio.obj"
	-@erase "$(INTDIR)\parser.obj"
	-@erase "$(INTDIR)\pprint.obj"
	-@erase "$(INTDIR)\streamio.obj"
	-@erase "$(INTDIR)\tagask.obj"
	-@erase "$(INTDIR)\tags.obj"
	-@erase "$(INTDIR)\tidylib.obj"
	-@erase "$(INTDIR)\tmbstr.obj"
	-@erase "$(INTDIR)\utf8.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\win32tc.obj"
	-@erase "$(OUTDIR)\LibTidy.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LINK32=link.exe -lib
CPP_PROJ=/nologo /MD /W3 /GR /GX /O2 /I "..\..\tidy\include" /I "..\..\tidy\src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_LIB" /D "SUPPORT_UTF16_ENCODINGS" /D "SUPPORT_ASIAN_ENCODINGS" /D "SUPPORT_ACCESSIBILITY_CHECKS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\LibTidy.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\LibTidy.lib" 
LIB32_OBJS= \
	"$(INTDIR)\access.obj" \
	"$(INTDIR)\alloc.obj" \
	"$(INTDIR)\attrask.obj" \
	"$(INTDIR)\attrdict.obj" \
	"$(INTDIR)\attrget.obj" \
	"$(INTDIR)\attrs.obj" \
	"$(INTDIR)\buffio.obj" \
	"$(INTDIR)\clean.obj" \
	"$(INTDIR)\config.obj" \
	"$(INTDIR)\entities.obj" \
	"$(INTDIR)\fileio.obj" \
	"$(INTDIR)\istack.obj" \
	"$(INTDIR)\lexer.obj" \
	"$(INTDIR)\localize.obj" \
	"$(INTDIR)\mappedio.obj" \
	"$(INTDIR)\parser.obj" \
	"$(INTDIR)\pprint.obj" \
	"$(INTDIR)\streamio.obj" \
	"$(INTDIR)\tagask.obj" \
	"$(INTDIR)\tags.obj" \
	"$(INTDIR)\tidylib.obj" \
	"$(INTDIR)\tmbstr.obj" \
	"$(INTDIR)\utf8.obj" \
	"$(INTDIR)\win32tc.obj"

"$(OUTDIR)\LibTidy.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
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
!IF EXISTS("LibTidy.dep")
!INCLUDE "LibTidy.dep"
!ELSE 
!MESSAGE Warning: cannot find "LibTidy.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "LibTidy - Win32 Debug" || "$(CFG)" == "LibTidy - Win32 Release"
SOURCE="..\..\tidy\src\access.c"

"$(INTDIR)\access.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\alloc.c"

"$(INTDIR)\alloc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\attrask.c"

"$(INTDIR)\attrask.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\attrdict.c"

"$(INTDIR)\attrdict.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\attrget.c"

"$(INTDIR)\attrget.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\attrs.c"

"$(INTDIR)\attrs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\buffio.c"

"$(INTDIR)\buffio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\clean.c"

"$(INTDIR)\clean.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\config.c"

"$(INTDIR)\config.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\entities.c"

"$(INTDIR)\entities.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\fileio.c"

"$(INTDIR)\fileio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\istack.c"

"$(INTDIR)\istack.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\lexer.c"

"$(INTDIR)\lexer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\localize.c"

"$(INTDIR)\localize.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\mappedio.c"

"$(INTDIR)\mappedio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\parser.c"

"$(INTDIR)\parser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\pprint.c"

"$(INTDIR)\pprint.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\streamio.c"

"$(INTDIR)\streamio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\tagask.c"

"$(INTDIR)\tagask.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\tags.c"

"$(INTDIR)\tags.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\tidylib.c"

"$(INTDIR)\tidylib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\tmbstr.c"

"$(INTDIR)\tmbstr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\utf8.c"

"$(INTDIR)\utf8.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\tidy\src\win32tc.c"

"$(INTDIR)\win32tc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

