# Microsoft Developer Studio Generated NMAKE File, Based on AgilityBook.dsp
!IF "$(CFG)" == ""
CFG=AgilityBook - Win32 Debug
!MESSAGE No configuration specified. Defaulting to AgilityBook - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "AgilityBook - Win32 Debug" && "$(CFG)" != "AgilityBook - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AgilityBook.mak" CFG="AgilityBook - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AgilityBook - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "AgilityBook - Win32 Release" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "AgilityBook - Win32 Debug"

OUTDIR=.\../../../bin/VC6/Debug
INTDIR=.\../../../bin/VC6/Debug/AgilityBook
# Begin Custom Macros
OutDir=.\../../../bin/VC6/Debug
# End Custom Macros

ALL : "$(OUTDIR)\AgilityBook.exe"


CLEAN :
	-@erase "$(INTDIR)\AgilityBook.obj"
	-@erase "$(INTDIR)\AgilityBook.pch"
	-@erase "$(INTDIR)\AgilityBookCalendarListView.obj"
	-@erase "$(INTDIR)\AgilityBookCalendarView.obj"
	-@erase "$(INTDIR)\AgilityBookDoc.obj"
	-@erase "$(INTDIR)\AgilityBookHtmlView.obj"
	-@erase "$(INTDIR)\AgilityBookMenu.obj"
	-@erase "$(INTDIR)\AgilityBookOptions.obj"
	-@erase "$(INTDIR)\AgilityBookPanels.obj"
	-@erase "$(INTDIR)\AgilityBookPointsView.obj"
	-@erase "$(INTDIR)\AgilityBookRunsView.obj"
	-@erase "$(INTDIR)\AgilityBookTrainingView.obj"
	-@erase "$(INTDIR)\AgilityBookTreeData.obj"
	-@erase "$(INTDIR)\AgilityBookTreeView.obj"
	-@erase "$(INTDIR)\ARBAgilityRecordBook.obj"
	-@erase "$(INTDIR)\ARBBase.obj"
	-@erase "$(INTDIR)\ARBBase64.obj"
	-@erase "$(INTDIR)\ARBCalendar.obj"
	-@erase "$(INTDIR)\ARBConfig.obj"
	-@erase "$(INTDIR)\ARBConfigAction.obj"
	-@erase "$(INTDIR)\ARBConfigCalSite.obj"
	-@erase "$(INTDIR)\ARBConfigDivision.obj"
	-@erase "$(INTDIR)\ARBConfigEvent.obj"
	-@erase "$(INTDIR)\ARBConfigFault.obj"
	-@erase "$(INTDIR)\ARBConfigLevel.obj"
	-@erase "$(INTDIR)\ARBConfigLifetimePoints.obj"
	-@erase "$(INTDIR)\ARBConfigMultiQ.obj"
	-@erase "$(INTDIR)\ARBConfigOtherPoints.obj"
	-@erase "$(INTDIR)\ARBConfigPlaceInfo.obj"
	-@erase "$(INTDIR)\ARBConfigScoring.obj"
	-@erase "$(INTDIR)\ARBConfigSubLevel.obj"
	-@erase "$(INTDIR)\ARBConfigTitle.obj"
	-@erase "$(INTDIR)\ARBConfigTitlePoints.obj"
	-@erase "$(INTDIR)\ARBConfigVenue.obj"
	-@erase "$(INTDIR)\ARBDate.obj"
	-@erase "$(INTDIR)\ARBDog.obj"
	-@erase "$(INTDIR)\ARBDogClub.obj"
	-@erase "$(INTDIR)\ARBDogExistingPoints.obj"
	-@erase "$(INTDIR)\ARBDogNotes.obj"
	-@erase "$(INTDIR)\ARBDogReferenceRun.obj"
	-@erase "$(INTDIR)\ARBDogRegNum.obj"
	-@erase "$(INTDIR)\ARBDogRun.obj"
	-@erase "$(INTDIR)\ARBDogRunOtherPoints.obj"
	-@erase "$(INTDIR)\ARBDogRunPartner.obj"
	-@erase "$(INTDIR)\ARBDogRunScoring.obj"
	-@erase "$(INTDIR)\ARBDogTitle.obj"
	-@erase "$(INTDIR)\ARBDogTrial.obj"
	-@erase "$(INTDIR)\ARBInfo.obj"
	-@erase "$(INTDIR)\ARBInfoItem.obj"
	-@erase "$(INTDIR)\ARBLocalization.obj"
	-@erase "$(INTDIR)\ARBString.obj"
	-@erase "$(INTDIR)\ARBTraining.obj"
	-@erase "$(INTDIR)\ARBTypes.obj"
	-@erase "$(INTDIR)\BinaryData.obj"
	-@erase "$(INTDIR)\CalendarSites.obj"
	-@erase "$(INTDIR)\CheckLink.obj"
	-@erase "$(INTDIR)\CheckTreeCtrl.obj"
	-@erase "$(INTDIR)\ClipBoard.obj"
	-@erase "$(INTDIR)\ColumnOrder.obj"
	-@erase "$(INTDIR)\ComboBoxes.obj"
	-@erase "$(INTDIR)\CommonView.obj"
	-@erase "$(INTDIR)\ConfigHandler.obj"
	-@erase "$(INTDIR)\DlgAbout.obj"
	-@erase "$(INTDIR)\DlgAssignColumns.obj"
	-@erase "$(INTDIR)\DlgAuthenticate.obj"
	-@erase "$(INTDIR)\DlgCalendar.obj"
	-@erase "$(INTDIR)\DlgCalendarQueryDetail.obj"
	-@erase "$(INTDIR)\DlgClub.obj"
	-@erase "$(INTDIR)\DlgConfigEvent.obj"
	-@erase "$(INTDIR)\DlgConfigEventMethod.obj"
	-@erase "$(INTDIR)\DlgConfigMultiQ.obj"
	-@erase "$(INTDIR)\DlgConfigOtherPoints.obj"
	-@erase "$(INTDIR)\DlgConfigPlaceInfo.obj"
	-@erase "$(INTDIR)\DlgConfigTitle.obj"
	-@erase "$(INTDIR)\DlgConfigTitlePoints.obj"
	-@erase "$(INTDIR)\DlgConfigUpdate.obj"
	-@erase "$(INTDIR)\DlgConfigure.obj"
	-@erase "$(INTDIR)\DlgConfigureData.obj"
	-@erase "$(INTDIR)\DlgConfigVenue.obj"
	-@erase "$(INTDIR)\DlgCRCDViewer.obj"
	-@erase "$(INTDIR)\DlgDog.obj"
	-@erase "$(INTDIR)\DlgDogNumbers.obj"
	-@erase "$(INTDIR)\DlgDogPoints.obj"
	-@erase "$(INTDIR)\DlgDogProp.obj"
	-@erase "$(INTDIR)\DlgDogTitles.obj"
	-@erase "$(INTDIR)\DlgEventSelect.obj"
	-@erase "$(INTDIR)\DlgExistingPoints.obj"
	-@erase "$(INTDIR)\DlgFault.obj"
	-@erase "$(INTDIR)\DlgFind.obj"
	-@erase "$(INTDIR)\DlgFindLinks.obj"
	-@erase "$(INTDIR)\DlgInfoNote.obj"
	-@erase "$(INTDIR)\DlgListCtrl.obj"
	-@erase "$(INTDIR)\DlgListViewer.obj"
	-@erase "$(INTDIR)\DlgMessage.obj"
	-@erase "$(INTDIR)\DlgMessageBox.obj"
	-@erase "$(INTDIR)\DlgName.obj"
	-@erase "$(INTDIR)\DlgOptions.obj"
	-@erase "$(INTDIR)\DlgOptionsCalendar.obj"
	-@erase "$(INTDIR)\DlgOptionsFilter.obj"
	-@erase "$(INTDIR)\DlgOptionsPrint.obj"
	-@erase "$(INTDIR)\DlgOptionsProgram.obj"
	-@erase "$(INTDIR)\DlgOtherPoint.obj"
	-@erase "$(INTDIR)\DlgPartner.obj"
	-@erase "$(INTDIR)\DlgPluginDetails.obj"
	-@erase "$(INTDIR)\DlgPointsViewSort.obj"
	-@erase "$(INTDIR)\DlgProgress.obj"
	-@erase "$(INTDIR)\DlgQueryDetail.obj"
	-@erase "$(INTDIR)\DlgReferenceRun.obj"
	-@erase "$(INTDIR)\DlgRegNum.obj"
	-@erase "$(INTDIR)\DlgReorder.obj"
	-@erase "$(INTDIR)\DlgRun.obj"
	-@erase "$(INTDIR)\DlgRunComments.obj"
	-@erase "$(INTDIR)\DlgRunCRCD.obj"
	-@erase "$(INTDIR)\DlgRunLink.obj"
	-@erase "$(INTDIR)\DlgRunReference.obj"
	-@erase "$(INTDIR)\DlgRunScore.obj"
	-@erase "$(INTDIR)\DlgSelectDog.obj"
	-@erase "$(INTDIR)\DlgSelectURL.obj"
	-@erase "$(INTDIR)\DlgTitle.obj"
	-@erase "$(INTDIR)\DlgTraining.obj"
	-@erase "$(INTDIR)\DlgTrial.obj"
	-@erase "$(INTDIR)\Element.obj"
	-@erase "$(INTDIR)\FilterOptions.obj"
	-@erase "$(INTDIR)\Globals.obj"
	-@erase "$(INTDIR)\IconList.obj"
	-@erase "$(INTDIR)\LanguageManager.obj"
	-@erase "$(INTDIR)\ListCtrl.obj"
	-@erase "$(INTDIR)\ListData.obj"
	-@erase "$(INTDIR)\Localization.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\NoteButton.obj"
	-@erase "$(INTDIR)\PointsData.obj"
	-@erase "$(INTDIR)\Print.obj"
	-@erase "$(INTDIR)\ReadHttp.obj"
	-@erase "$(INTDIR)\RichEditCtrl2.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\TabView.obj"
	-@erase "$(INTDIR)\UpdateInfo.obj"
	-@erase "$(INTDIR)\Validators.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\VersionNum.obj"
	-@erase "$(INTDIR)\Wizard.obj"
	-@erase "$(INTDIR)\WizardExcel.obj"
	-@erase "$(INTDIR)\WizardExport.obj"
	-@erase "$(INTDIR)\WizardImport.obj"
	-@erase "$(INTDIR)\WizardStart.obj"
	-@erase "$(OUTDIR)\AgilityBook.exe"
	-@erase "$(OUTDIR)\AgilityBook.ilk"
	-@erase "$(OUTDIR)\AgilityBook.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../.." /I "../../Include" /I "../../ARB" /I "../../Win" /I "../../../../boost" /I "C:/wx/include" /I "C:/wx/include/msvc" /D "WXWIDGETS" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WINDLL" /Fp"$(INTDIR)\AgilityBook.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AgilityBook.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=ws2_32.lib comctl32.lib rpcrt4.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\AgilityBook.pdb" /debug /machine:I386 /out:"$(OUTDIR)\AgilityBook.exe" /pdbtype:sept /libpath:"../../../lib/VC6" /libpath:"C:\wx\lib\vc_lib_VC6.0" 
LINK32_OBJS= \
	"$(INTDIR)\AgilityBook.obj" \
	"$(INTDIR)\AgilityBookCalendarListView.obj" \
	"$(INTDIR)\AgilityBookCalendarView.obj" \
	"$(INTDIR)\AgilityBookDoc.obj" \
	"$(INTDIR)\AgilityBookHtmlView.obj" \
	"$(INTDIR)\AgilityBookMenu.obj" \
	"$(INTDIR)\AgilityBookOptions.obj" \
	"$(INTDIR)\AgilityBookPanels.obj" \
	"$(INTDIR)\AgilityBookPointsView.obj" \
	"$(INTDIR)\AgilityBookRunsView.obj" \
	"$(INTDIR)\AgilityBookTrainingView.obj" \
	"$(INTDIR)\AgilityBookTreeData.obj" \
	"$(INTDIR)\AgilityBookTreeView.obj" \
	"$(INTDIR)\CalendarSites.obj" \
	"$(INTDIR)\CheckLink.obj" \
	"$(INTDIR)\CheckTreeCtrl.obj" \
	"$(INTDIR)\ClipBoard.obj" \
	"$(INTDIR)\ColumnOrder.obj" \
	"$(INTDIR)\ComboBoxes.obj" \
	"$(INTDIR)\CommonView.obj" \
	"$(INTDIR)\ConfigHandler.obj" \
	"$(INTDIR)\DlgAbout.obj" \
	"$(INTDIR)\DlgAssignColumns.obj" \
	"$(INTDIR)\DlgAuthenticate.obj" \
	"$(INTDIR)\DlgCalendar.obj" \
	"$(INTDIR)\DlgCalendarQueryDetail.obj" \
	"$(INTDIR)\DlgClub.obj" \
	"$(INTDIR)\DlgConfigEvent.obj" \
	"$(INTDIR)\DlgConfigEventMethod.obj" \
	"$(INTDIR)\DlgConfigMultiQ.obj" \
	"$(INTDIR)\DlgConfigOtherPoints.obj" \
	"$(INTDIR)\DlgConfigPlaceInfo.obj" \
	"$(INTDIR)\DlgConfigTitle.obj" \
	"$(INTDIR)\DlgConfigTitlePoints.obj" \
	"$(INTDIR)\DlgConfigUpdate.obj" \
	"$(INTDIR)\DlgConfigure.obj" \
	"$(INTDIR)\DlgConfigureData.obj" \
	"$(INTDIR)\DlgConfigVenue.obj" \
	"$(INTDIR)\DlgCRCDViewer.obj" \
	"$(INTDIR)\DlgDog.obj" \
	"$(INTDIR)\DlgDogNumbers.obj" \
	"$(INTDIR)\DlgDogPoints.obj" \
	"$(INTDIR)\DlgDogProp.obj" \
	"$(INTDIR)\DlgDogTitles.obj" \
	"$(INTDIR)\DlgEventSelect.obj" \
	"$(INTDIR)\DlgExistingPoints.obj" \
	"$(INTDIR)\DlgFault.obj" \
	"$(INTDIR)\DlgFind.obj" \
	"$(INTDIR)\DlgFindLinks.obj" \
	"$(INTDIR)\DlgInfoNote.obj" \
	"$(INTDIR)\DlgListCtrl.obj" \
	"$(INTDIR)\DlgListViewer.obj" \
	"$(INTDIR)\DlgMessage.obj" \
	"$(INTDIR)\DlgMessageBox.obj" \
	"$(INTDIR)\DlgName.obj" \
	"$(INTDIR)\DlgOptions.obj" \
	"$(INTDIR)\DlgOptionsCalendar.obj" \
	"$(INTDIR)\DlgOptionsFilter.obj" \
	"$(INTDIR)\DlgOptionsPrint.obj" \
	"$(INTDIR)\DlgOptionsProgram.obj" \
	"$(INTDIR)\DlgOtherPoint.obj" \
	"$(INTDIR)\DlgPartner.obj" \
	"$(INTDIR)\DlgPluginDetails.obj" \
	"$(INTDIR)\DlgPointsViewSort.obj" \
	"$(INTDIR)\DlgProgress.obj" \
	"$(INTDIR)\DlgQueryDetail.obj" \
	"$(INTDIR)\DlgReferenceRun.obj" \
	"$(INTDIR)\DlgRegNum.obj" \
	"$(INTDIR)\DlgReorder.obj" \
	"$(INTDIR)\DlgRun.obj" \
	"$(INTDIR)\DlgRunComments.obj" \
	"$(INTDIR)\DlgRunCRCD.obj" \
	"$(INTDIR)\DlgRunLink.obj" \
	"$(INTDIR)\DlgRunReference.obj" \
	"$(INTDIR)\DlgRunScore.obj" \
	"$(INTDIR)\DlgSelectDog.obj" \
	"$(INTDIR)\DlgSelectURL.obj" \
	"$(INTDIR)\DlgTitle.obj" \
	"$(INTDIR)\DlgTraining.obj" \
	"$(INTDIR)\DlgTrial.obj" \
	"$(INTDIR)\FilterOptions.obj" \
	"$(INTDIR)\Globals.obj" \
	"$(INTDIR)\IconList.obj" \
	"$(INTDIR)\LanguageManager.obj" \
	"$(INTDIR)\ListCtrl.obj" \
	"$(INTDIR)\ListData.obj" \
	"$(INTDIR)\Localization.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\NoteButton.obj" \
	"$(INTDIR)\PointsData.obj" \
	"$(INTDIR)\Print.obj" \
	"$(INTDIR)\ReadHttp.obj" \
	"$(INTDIR)\RichEditCtrl2.obj" \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\TabView.obj" \
	"$(INTDIR)\UpdateInfo.obj" \
	"$(INTDIR)\Validators.obj" \
	"$(INTDIR)\VersionNum.obj" \
	"$(INTDIR)\Wizard.obj" \
	"$(INTDIR)\WizardExcel.obj" \
	"$(INTDIR)\WizardExport.obj" \
	"$(INTDIR)\WizardImport.obj" \
	"$(INTDIR)\WizardStart.obj" \
	"$(INTDIR)\ARBAgilityRecordBook.obj" \
	"$(INTDIR)\ARBBase.obj" \
	"$(INTDIR)\ARBBase64.obj" \
	"$(INTDIR)\ARBCalendar.obj" \
	"$(INTDIR)\ARBConfig.obj" \
	"$(INTDIR)\ARBConfigAction.obj" \
	"$(INTDIR)\ARBConfigCalSite.obj" \
	"$(INTDIR)\ARBConfigDivision.obj" \
	"$(INTDIR)\ARBConfigEvent.obj" \
	"$(INTDIR)\ARBConfigFault.obj" \
	"$(INTDIR)\ARBConfigLevel.obj" \
	"$(INTDIR)\ARBConfigLifetimePoints.obj" \
	"$(INTDIR)\ARBConfigMultiQ.obj" \
	"$(INTDIR)\ARBConfigOtherPoints.obj" \
	"$(INTDIR)\ARBConfigPlaceInfo.obj" \
	"$(INTDIR)\ARBConfigScoring.obj" \
	"$(INTDIR)\ARBConfigSubLevel.obj" \
	"$(INTDIR)\ARBConfigTitle.obj" \
	"$(INTDIR)\ARBConfigTitlePoints.obj" \
	"$(INTDIR)\ARBConfigVenue.obj" \
	"$(INTDIR)\ARBDate.obj" \
	"$(INTDIR)\ARBDog.obj" \
	"$(INTDIR)\ARBDogClub.obj" \
	"$(INTDIR)\ARBDogExistingPoints.obj" \
	"$(INTDIR)\ARBDogNotes.obj" \
	"$(INTDIR)\ARBDogReferenceRun.obj" \
	"$(INTDIR)\ARBDogRegNum.obj" \
	"$(INTDIR)\ARBDogRun.obj" \
	"$(INTDIR)\ARBDogRunOtherPoints.obj" \
	"$(INTDIR)\ARBDogRunPartner.obj" \
	"$(INTDIR)\ARBDogRunScoring.obj" \
	"$(INTDIR)\ARBDogTitle.obj" \
	"$(INTDIR)\ARBDogTrial.obj" \
	"$(INTDIR)\ARBInfo.obj" \
	"$(INTDIR)\ARBInfoItem.obj" \
	"$(INTDIR)\ARBLocalization.obj" \
	"$(INTDIR)\ARBString.obj" \
	"$(INTDIR)\ARBTraining.obj" \
	"$(INTDIR)\ARBTypes.obj" \
	"$(INTDIR)\BinaryData.obj" \
	"$(INTDIR)\Element.obj"

"$(OUTDIR)\AgilityBook.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AgilityBook - Win32 Release"

OUTDIR=.\../../../bin/VC6/Release
INTDIR=.\../../../bin/VC6/Release/AgilityBook
# Begin Custom Macros
OutDir=.\../../../bin/VC6/Release
# End Custom Macros

ALL : "$(OUTDIR)\AgilityBook.exe"


CLEAN :
	-@erase "$(INTDIR)\AgilityBook.obj"
	-@erase "$(INTDIR)\AgilityBook.pch"
	-@erase "$(INTDIR)\AgilityBookCalendarListView.obj"
	-@erase "$(INTDIR)\AgilityBookCalendarView.obj"
	-@erase "$(INTDIR)\AgilityBookDoc.obj"
	-@erase "$(INTDIR)\AgilityBookHtmlView.obj"
	-@erase "$(INTDIR)\AgilityBookMenu.obj"
	-@erase "$(INTDIR)\AgilityBookOptions.obj"
	-@erase "$(INTDIR)\AgilityBookPanels.obj"
	-@erase "$(INTDIR)\AgilityBookPointsView.obj"
	-@erase "$(INTDIR)\AgilityBookRunsView.obj"
	-@erase "$(INTDIR)\AgilityBookTrainingView.obj"
	-@erase "$(INTDIR)\AgilityBookTreeData.obj"
	-@erase "$(INTDIR)\AgilityBookTreeView.obj"
	-@erase "$(INTDIR)\ARBAgilityRecordBook.obj"
	-@erase "$(INTDIR)\ARBBase.obj"
	-@erase "$(INTDIR)\ARBBase64.obj"
	-@erase "$(INTDIR)\ARBCalendar.obj"
	-@erase "$(INTDIR)\ARBConfig.obj"
	-@erase "$(INTDIR)\ARBConfigAction.obj"
	-@erase "$(INTDIR)\ARBConfigCalSite.obj"
	-@erase "$(INTDIR)\ARBConfigDivision.obj"
	-@erase "$(INTDIR)\ARBConfigEvent.obj"
	-@erase "$(INTDIR)\ARBConfigFault.obj"
	-@erase "$(INTDIR)\ARBConfigLevel.obj"
	-@erase "$(INTDIR)\ARBConfigLifetimePoints.obj"
	-@erase "$(INTDIR)\ARBConfigMultiQ.obj"
	-@erase "$(INTDIR)\ARBConfigOtherPoints.obj"
	-@erase "$(INTDIR)\ARBConfigPlaceInfo.obj"
	-@erase "$(INTDIR)\ARBConfigScoring.obj"
	-@erase "$(INTDIR)\ARBConfigSubLevel.obj"
	-@erase "$(INTDIR)\ARBConfigTitle.obj"
	-@erase "$(INTDIR)\ARBConfigTitlePoints.obj"
	-@erase "$(INTDIR)\ARBConfigVenue.obj"
	-@erase "$(INTDIR)\ARBDate.obj"
	-@erase "$(INTDIR)\ARBDog.obj"
	-@erase "$(INTDIR)\ARBDogClub.obj"
	-@erase "$(INTDIR)\ARBDogExistingPoints.obj"
	-@erase "$(INTDIR)\ARBDogNotes.obj"
	-@erase "$(INTDIR)\ARBDogReferenceRun.obj"
	-@erase "$(INTDIR)\ARBDogRegNum.obj"
	-@erase "$(INTDIR)\ARBDogRun.obj"
	-@erase "$(INTDIR)\ARBDogRunOtherPoints.obj"
	-@erase "$(INTDIR)\ARBDogRunPartner.obj"
	-@erase "$(INTDIR)\ARBDogRunScoring.obj"
	-@erase "$(INTDIR)\ARBDogTitle.obj"
	-@erase "$(INTDIR)\ARBDogTrial.obj"
	-@erase "$(INTDIR)\ARBInfo.obj"
	-@erase "$(INTDIR)\ARBInfoItem.obj"
	-@erase "$(INTDIR)\ARBLocalization.obj"
	-@erase "$(INTDIR)\ARBString.obj"
	-@erase "$(INTDIR)\ARBTraining.obj"
	-@erase "$(INTDIR)\ARBTypes.obj"
	-@erase "$(INTDIR)\BinaryData.obj"
	-@erase "$(INTDIR)\CalendarSites.obj"
	-@erase "$(INTDIR)\CheckLink.obj"
	-@erase "$(INTDIR)\CheckTreeCtrl.obj"
	-@erase "$(INTDIR)\ClipBoard.obj"
	-@erase "$(INTDIR)\ColumnOrder.obj"
	-@erase "$(INTDIR)\ComboBoxes.obj"
	-@erase "$(INTDIR)\CommonView.obj"
	-@erase "$(INTDIR)\ConfigHandler.obj"
	-@erase "$(INTDIR)\DlgAbout.obj"
	-@erase "$(INTDIR)\DlgAssignColumns.obj"
	-@erase "$(INTDIR)\DlgAuthenticate.obj"
	-@erase "$(INTDIR)\DlgCalendar.obj"
	-@erase "$(INTDIR)\DlgCalendarQueryDetail.obj"
	-@erase "$(INTDIR)\DlgClub.obj"
	-@erase "$(INTDIR)\DlgConfigEvent.obj"
	-@erase "$(INTDIR)\DlgConfigEventMethod.obj"
	-@erase "$(INTDIR)\DlgConfigMultiQ.obj"
	-@erase "$(INTDIR)\DlgConfigOtherPoints.obj"
	-@erase "$(INTDIR)\DlgConfigPlaceInfo.obj"
	-@erase "$(INTDIR)\DlgConfigTitle.obj"
	-@erase "$(INTDIR)\DlgConfigTitlePoints.obj"
	-@erase "$(INTDIR)\DlgConfigUpdate.obj"
	-@erase "$(INTDIR)\DlgConfigure.obj"
	-@erase "$(INTDIR)\DlgConfigureData.obj"
	-@erase "$(INTDIR)\DlgConfigVenue.obj"
	-@erase "$(INTDIR)\DlgCRCDViewer.obj"
	-@erase "$(INTDIR)\DlgDog.obj"
	-@erase "$(INTDIR)\DlgDogNumbers.obj"
	-@erase "$(INTDIR)\DlgDogPoints.obj"
	-@erase "$(INTDIR)\DlgDogProp.obj"
	-@erase "$(INTDIR)\DlgDogTitles.obj"
	-@erase "$(INTDIR)\DlgEventSelect.obj"
	-@erase "$(INTDIR)\DlgExistingPoints.obj"
	-@erase "$(INTDIR)\DlgFault.obj"
	-@erase "$(INTDIR)\DlgFind.obj"
	-@erase "$(INTDIR)\DlgFindLinks.obj"
	-@erase "$(INTDIR)\DlgInfoNote.obj"
	-@erase "$(INTDIR)\DlgListCtrl.obj"
	-@erase "$(INTDIR)\DlgListViewer.obj"
	-@erase "$(INTDIR)\DlgMessage.obj"
	-@erase "$(INTDIR)\DlgMessageBox.obj"
	-@erase "$(INTDIR)\DlgName.obj"
	-@erase "$(INTDIR)\DlgOptions.obj"
	-@erase "$(INTDIR)\DlgOptionsCalendar.obj"
	-@erase "$(INTDIR)\DlgOptionsFilter.obj"
	-@erase "$(INTDIR)\DlgOptionsPrint.obj"
	-@erase "$(INTDIR)\DlgOptionsProgram.obj"
	-@erase "$(INTDIR)\DlgOtherPoint.obj"
	-@erase "$(INTDIR)\DlgPartner.obj"
	-@erase "$(INTDIR)\DlgPluginDetails.obj"
	-@erase "$(INTDIR)\DlgPointsViewSort.obj"
	-@erase "$(INTDIR)\DlgProgress.obj"
	-@erase "$(INTDIR)\DlgQueryDetail.obj"
	-@erase "$(INTDIR)\DlgReferenceRun.obj"
	-@erase "$(INTDIR)\DlgRegNum.obj"
	-@erase "$(INTDIR)\DlgReorder.obj"
	-@erase "$(INTDIR)\DlgRun.obj"
	-@erase "$(INTDIR)\DlgRunComments.obj"
	-@erase "$(INTDIR)\DlgRunCRCD.obj"
	-@erase "$(INTDIR)\DlgRunLink.obj"
	-@erase "$(INTDIR)\DlgRunReference.obj"
	-@erase "$(INTDIR)\DlgRunScore.obj"
	-@erase "$(INTDIR)\DlgSelectDog.obj"
	-@erase "$(INTDIR)\DlgSelectURL.obj"
	-@erase "$(INTDIR)\DlgTitle.obj"
	-@erase "$(INTDIR)\DlgTraining.obj"
	-@erase "$(INTDIR)\DlgTrial.obj"
	-@erase "$(INTDIR)\Element.obj"
	-@erase "$(INTDIR)\FilterOptions.obj"
	-@erase "$(INTDIR)\Globals.obj"
	-@erase "$(INTDIR)\IconList.obj"
	-@erase "$(INTDIR)\LanguageManager.obj"
	-@erase "$(INTDIR)\ListCtrl.obj"
	-@erase "$(INTDIR)\ListData.obj"
	-@erase "$(INTDIR)\Localization.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\NoteButton.obj"
	-@erase "$(INTDIR)\PointsData.obj"
	-@erase "$(INTDIR)\Print.obj"
	-@erase "$(INTDIR)\ReadHttp.obj"
	-@erase "$(INTDIR)\RichEditCtrl2.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\TabView.obj"
	-@erase "$(INTDIR)\UpdateInfo.obj"
	-@erase "$(INTDIR)\Validators.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\VersionNum.obj"
	-@erase "$(INTDIR)\Wizard.obj"
	-@erase "$(INTDIR)\WizardExcel.obj"
	-@erase "$(INTDIR)\WizardExport.obj"
	-@erase "$(INTDIR)\WizardImport.obj"
	-@erase "$(INTDIR)\WizardStart.obj"
	-@erase "$(OUTDIR)\AgilityBook.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GR /GX /O2 /I "../.." /I "../../Include" /I "../../ARB" /I "../../Win" /I "../../../../boost" /I "C:/wx/include" /I "C:/wx/include/msvc" /D "WXWIDGETS" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WINDLL" /Fp"$(INTDIR)\AgilityBook.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AgilityBook.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=ws2_32.lib comctl32.lib rpcrt4.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\AgilityBook.pdb" /machine:I386 /out:"$(OUTDIR)\AgilityBook.exe" /libpath:"../../../lib/VC6" /libpath:"C:\wx\lib\vc_lib_VC6.0" 
LINK32_OBJS= \
	"$(INTDIR)\AgilityBook.obj" \
	"$(INTDIR)\AgilityBookCalendarListView.obj" \
	"$(INTDIR)\AgilityBookCalendarView.obj" \
	"$(INTDIR)\AgilityBookDoc.obj" \
	"$(INTDIR)\AgilityBookHtmlView.obj" \
	"$(INTDIR)\AgilityBookMenu.obj" \
	"$(INTDIR)\AgilityBookOptions.obj" \
	"$(INTDIR)\AgilityBookPanels.obj" \
	"$(INTDIR)\AgilityBookPointsView.obj" \
	"$(INTDIR)\AgilityBookRunsView.obj" \
	"$(INTDIR)\AgilityBookTrainingView.obj" \
	"$(INTDIR)\AgilityBookTreeData.obj" \
	"$(INTDIR)\AgilityBookTreeView.obj" \
	"$(INTDIR)\CalendarSites.obj" \
	"$(INTDIR)\CheckLink.obj" \
	"$(INTDIR)\CheckTreeCtrl.obj" \
	"$(INTDIR)\ClipBoard.obj" \
	"$(INTDIR)\ColumnOrder.obj" \
	"$(INTDIR)\ComboBoxes.obj" \
	"$(INTDIR)\CommonView.obj" \
	"$(INTDIR)\ConfigHandler.obj" \
	"$(INTDIR)\DlgAbout.obj" \
	"$(INTDIR)\DlgAssignColumns.obj" \
	"$(INTDIR)\DlgAuthenticate.obj" \
	"$(INTDIR)\DlgCalendar.obj" \
	"$(INTDIR)\DlgCalendarQueryDetail.obj" \
	"$(INTDIR)\DlgClub.obj" \
	"$(INTDIR)\DlgConfigEvent.obj" \
	"$(INTDIR)\DlgConfigEventMethod.obj" \
	"$(INTDIR)\DlgConfigMultiQ.obj" \
	"$(INTDIR)\DlgConfigOtherPoints.obj" \
	"$(INTDIR)\DlgConfigPlaceInfo.obj" \
	"$(INTDIR)\DlgConfigTitle.obj" \
	"$(INTDIR)\DlgConfigTitlePoints.obj" \
	"$(INTDIR)\DlgConfigUpdate.obj" \
	"$(INTDIR)\DlgConfigure.obj" \
	"$(INTDIR)\DlgConfigureData.obj" \
	"$(INTDIR)\DlgConfigVenue.obj" \
	"$(INTDIR)\DlgCRCDViewer.obj" \
	"$(INTDIR)\DlgDog.obj" \
	"$(INTDIR)\DlgDogNumbers.obj" \
	"$(INTDIR)\DlgDogPoints.obj" \
	"$(INTDIR)\DlgDogProp.obj" \
	"$(INTDIR)\DlgDogTitles.obj" \
	"$(INTDIR)\DlgEventSelect.obj" \
	"$(INTDIR)\DlgExistingPoints.obj" \
	"$(INTDIR)\DlgFault.obj" \
	"$(INTDIR)\DlgFind.obj" \
	"$(INTDIR)\DlgFindLinks.obj" \
	"$(INTDIR)\DlgInfoNote.obj" \
	"$(INTDIR)\DlgListCtrl.obj" \
	"$(INTDIR)\DlgListViewer.obj" \
	"$(INTDIR)\DlgMessage.obj" \
	"$(INTDIR)\DlgMessageBox.obj" \
	"$(INTDIR)\DlgName.obj" \
	"$(INTDIR)\DlgOptions.obj" \
	"$(INTDIR)\DlgOptionsCalendar.obj" \
	"$(INTDIR)\DlgOptionsFilter.obj" \
	"$(INTDIR)\DlgOptionsPrint.obj" \
	"$(INTDIR)\DlgOptionsProgram.obj" \
	"$(INTDIR)\DlgOtherPoint.obj" \
	"$(INTDIR)\DlgPartner.obj" \
	"$(INTDIR)\DlgPluginDetails.obj" \
	"$(INTDIR)\DlgPointsViewSort.obj" \
	"$(INTDIR)\DlgProgress.obj" \
	"$(INTDIR)\DlgQueryDetail.obj" \
	"$(INTDIR)\DlgReferenceRun.obj" \
	"$(INTDIR)\DlgRegNum.obj" \
	"$(INTDIR)\DlgReorder.obj" \
	"$(INTDIR)\DlgRun.obj" \
	"$(INTDIR)\DlgRunComments.obj" \
	"$(INTDIR)\DlgRunCRCD.obj" \
	"$(INTDIR)\DlgRunLink.obj" \
	"$(INTDIR)\DlgRunReference.obj" \
	"$(INTDIR)\DlgRunScore.obj" \
	"$(INTDIR)\DlgSelectDog.obj" \
	"$(INTDIR)\DlgSelectURL.obj" \
	"$(INTDIR)\DlgTitle.obj" \
	"$(INTDIR)\DlgTraining.obj" \
	"$(INTDIR)\DlgTrial.obj" \
	"$(INTDIR)\FilterOptions.obj" \
	"$(INTDIR)\Globals.obj" \
	"$(INTDIR)\IconList.obj" \
	"$(INTDIR)\LanguageManager.obj" \
	"$(INTDIR)\ListCtrl.obj" \
	"$(INTDIR)\ListData.obj" \
	"$(INTDIR)\Localization.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\NoteButton.obj" \
	"$(INTDIR)\PointsData.obj" \
	"$(INTDIR)\Print.obj" \
	"$(INTDIR)\ReadHttp.obj" \
	"$(INTDIR)\RichEditCtrl2.obj" \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\TabView.obj" \
	"$(INTDIR)\UpdateInfo.obj" \
	"$(INTDIR)\Validators.obj" \
	"$(INTDIR)\VersionNum.obj" \
	"$(INTDIR)\Wizard.obj" \
	"$(INTDIR)\WizardExcel.obj" \
	"$(INTDIR)\WizardExport.obj" \
	"$(INTDIR)\WizardImport.obj" \
	"$(INTDIR)\WizardStart.obj" \
	"$(INTDIR)\ARBAgilityRecordBook.obj" \
	"$(INTDIR)\ARBBase.obj" \
	"$(INTDIR)\ARBBase64.obj" \
	"$(INTDIR)\ARBCalendar.obj" \
	"$(INTDIR)\ARBConfig.obj" \
	"$(INTDIR)\ARBConfigAction.obj" \
	"$(INTDIR)\ARBConfigCalSite.obj" \
	"$(INTDIR)\ARBConfigDivision.obj" \
	"$(INTDIR)\ARBConfigEvent.obj" \
	"$(INTDIR)\ARBConfigFault.obj" \
	"$(INTDIR)\ARBConfigLevel.obj" \
	"$(INTDIR)\ARBConfigLifetimePoints.obj" \
	"$(INTDIR)\ARBConfigMultiQ.obj" \
	"$(INTDIR)\ARBConfigOtherPoints.obj" \
	"$(INTDIR)\ARBConfigPlaceInfo.obj" \
	"$(INTDIR)\ARBConfigScoring.obj" \
	"$(INTDIR)\ARBConfigSubLevel.obj" \
	"$(INTDIR)\ARBConfigTitle.obj" \
	"$(INTDIR)\ARBConfigTitlePoints.obj" \
	"$(INTDIR)\ARBConfigVenue.obj" \
	"$(INTDIR)\ARBDate.obj" \
	"$(INTDIR)\ARBDog.obj" \
	"$(INTDIR)\ARBDogClub.obj" \
	"$(INTDIR)\ARBDogExistingPoints.obj" \
	"$(INTDIR)\ARBDogNotes.obj" \
	"$(INTDIR)\ARBDogReferenceRun.obj" \
	"$(INTDIR)\ARBDogRegNum.obj" \
	"$(INTDIR)\ARBDogRun.obj" \
	"$(INTDIR)\ARBDogRunOtherPoints.obj" \
	"$(INTDIR)\ARBDogRunPartner.obj" \
	"$(INTDIR)\ARBDogRunScoring.obj" \
	"$(INTDIR)\ARBDogTitle.obj" \
	"$(INTDIR)\ARBDogTrial.obj" \
	"$(INTDIR)\ARBInfo.obj" \
	"$(INTDIR)\ARBInfoItem.obj" \
	"$(INTDIR)\ARBLocalization.obj" \
	"$(INTDIR)\ARBString.obj" \
	"$(INTDIR)\ARBTraining.obj" \
	"$(INTDIR)\ARBTypes.obj" \
	"$(INTDIR)\BinaryData.obj" \
	"$(INTDIR)\Element.obj"

"$(OUTDIR)\AgilityBook.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("AgilityBook.dep")
!INCLUDE "AgilityBook.dep"
!ELSE 
!MESSAGE Warning: cannot find "AgilityBook.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "AgilityBook - Win32 Debug" || "$(CFG)" == "AgilityBook - Win32 Release"
SOURCE=..\..\Win\AgilityBook.cpp

"$(INTDIR)\AgilityBook.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\AgilityBookCalendarListView.cpp

"$(INTDIR)\AgilityBookCalendarListView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\AgilityBookCalendarView.cpp

"$(INTDIR)\AgilityBookCalendarView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\AgilityBookDoc.cpp

"$(INTDIR)\AgilityBookDoc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\AgilityBookHtmlView.cpp

"$(INTDIR)\AgilityBookHtmlView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\AgilityBookMenu.cpp

"$(INTDIR)\AgilityBookMenu.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\AgilityBookOptions.cpp

"$(INTDIR)\AgilityBookOptions.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\AgilityBookPanels.cpp

"$(INTDIR)\AgilityBookPanels.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\AgilityBookPointsView.cpp

"$(INTDIR)\AgilityBookPointsView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\AgilityBookRunsView.cpp

"$(INTDIR)\AgilityBookRunsView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\AgilityBookTrainingView.cpp

"$(INTDIR)\AgilityBookTrainingView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\AgilityBookTreeData.cpp

"$(INTDIR)\AgilityBookTreeData.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\AgilityBookTreeView.cpp

"$(INTDIR)\AgilityBookTreeView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\CalendarSites.cpp

"$(INTDIR)\CalendarSites.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\CheckLink.cpp

"$(INTDIR)\CheckLink.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\CheckTreeCtrl.cpp

"$(INTDIR)\CheckTreeCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\ClipBoard.cpp

"$(INTDIR)\ClipBoard.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\ColumnOrder.cpp

"$(INTDIR)\ColumnOrder.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\ComboBoxes.cpp

"$(INTDIR)\ComboBoxes.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\CommonView.cpp

"$(INTDIR)\CommonView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\ConfigHandler.cpp

"$(INTDIR)\ConfigHandler.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgAbout.cpp

"$(INTDIR)\DlgAbout.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgAssignColumns.cpp

"$(INTDIR)\DlgAssignColumns.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgAuthenticate.cpp

"$(INTDIR)\DlgAuthenticate.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgCalendar.cpp

"$(INTDIR)\DlgCalendar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgCalendarQueryDetail.cpp

"$(INTDIR)\DlgCalendarQueryDetail.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgClub.cpp

"$(INTDIR)\DlgClub.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgConfigEvent.cpp

"$(INTDIR)\DlgConfigEvent.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgConfigEventMethod.cpp

"$(INTDIR)\DlgConfigEventMethod.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgConfigMultiQ.cpp

"$(INTDIR)\DlgConfigMultiQ.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgConfigOtherPoints.cpp

"$(INTDIR)\DlgConfigOtherPoints.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgConfigPlaceInfo.cpp

"$(INTDIR)\DlgConfigPlaceInfo.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgConfigTitle.cpp

"$(INTDIR)\DlgConfigTitle.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgConfigTitlePoints.cpp

"$(INTDIR)\DlgConfigTitlePoints.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgConfigUpdate.cpp

"$(INTDIR)\DlgConfigUpdate.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgConfigure.cpp

"$(INTDIR)\DlgConfigure.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgConfigureData.cpp

"$(INTDIR)\DlgConfigureData.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgConfigVenue.cpp

"$(INTDIR)\DlgConfigVenue.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgCRCDViewer.cpp

"$(INTDIR)\DlgCRCDViewer.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgDog.cpp

"$(INTDIR)\DlgDog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgDogNumbers.cpp

"$(INTDIR)\DlgDogNumbers.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgDogPoints.cpp

"$(INTDIR)\DlgDogPoints.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgDogProp.cpp

"$(INTDIR)\DlgDogProp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgDogTitles.cpp

"$(INTDIR)\DlgDogTitles.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgEventSelect.cpp

"$(INTDIR)\DlgEventSelect.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgExistingPoints.cpp

"$(INTDIR)\DlgExistingPoints.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgFault.cpp

"$(INTDIR)\DlgFault.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgFind.cpp

"$(INTDIR)\DlgFind.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgFindLinks.cpp

"$(INTDIR)\DlgFindLinks.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgInfoNote.cpp

"$(INTDIR)\DlgInfoNote.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgListCtrl.cpp

"$(INTDIR)\DlgListCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgListViewer.cpp

"$(INTDIR)\DlgListViewer.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgMessage.cpp

"$(INTDIR)\DlgMessage.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgMessageBox.cpp

"$(INTDIR)\DlgMessageBox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgName.cpp

"$(INTDIR)\DlgName.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgOptions.cpp

"$(INTDIR)\DlgOptions.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgOptionsCalendar.cpp

"$(INTDIR)\DlgOptionsCalendar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgOptionsFilter.cpp

"$(INTDIR)\DlgOptionsFilter.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgOptionsPrint.cpp

"$(INTDIR)\DlgOptionsPrint.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgOptionsProgram.cpp

"$(INTDIR)\DlgOptionsProgram.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgOtherPoint.cpp

"$(INTDIR)\DlgOtherPoint.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgPartner.cpp

"$(INTDIR)\DlgPartner.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgPluginDetails.cpp

"$(INTDIR)\DlgPluginDetails.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgPointsViewSort.cpp

"$(INTDIR)\DlgPointsViewSort.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgProgress.cpp

"$(INTDIR)\DlgProgress.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgQueryDetail.cpp

"$(INTDIR)\DlgQueryDetail.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgReferenceRun.cpp

"$(INTDIR)\DlgReferenceRun.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgRegNum.cpp

"$(INTDIR)\DlgRegNum.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgReorder.cpp

"$(INTDIR)\DlgReorder.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgRun.cpp

"$(INTDIR)\DlgRun.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgRunComments.cpp

"$(INTDIR)\DlgRunComments.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgRunCRCD.cpp

"$(INTDIR)\DlgRunCRCD.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgRunLink.cpp

"$(INTDIR)\DlgRunLink.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgRunReference.cpp

"$(INTDIR)\DlgRunReference.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgRunScore.cpp

"$(INTDIR)\DlgRunScore.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgSelectDog.cpp

"$(INTDIR)\DlgSelectDog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgSelectURL.cpp

"$(INTDIR)\DlgSelectURL.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgTitle.cpp

"$(INTDIR)\DlgTitle.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgTraining.cpp

"$(INTDIR)\DlgTraining.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\DlgTrial.cpp

"$(INTDIR)\DlgTrial.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\FilterOptions.cpp

"$(INTDIR)\FilterOptions.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\Globals.cpp

"$(INTDIR)\Globals.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\IconList.cpp

"$(INTDIR)\IconList.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\LanguageManager.cpp

"$(INTDIR)\LanguageManager.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\ListCtrl.cpp

"$(INTDIR)\ListCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\ListData.cpp

"$(INTDIR)\ListData.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\Localization.cpp

"$(INTDIR)\Localization.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\MainFrm.cpp

"$(INTDIR)\MainFrm.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\NoteButton.cpp

"$(INTDIR)\NoteButton.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\PointsData.cpp

"$(INTDIR)\PointsData.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\Print.cpp

"$(INTDIR)\Print.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\ReadHttp.cpp

"$(INTDIR)\ReadHttp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\RichEditCtrl2.cpp

"$(INTDIR)\RichEditCtrl2.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\stdafx.cpp

!IF  "$(CFG)" == "AgilityBook - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../.." /I "../../Include" /I "../../ARB" /I "../../Win" /I "../../../../boost" /I "C:/wx/include" /I "C:/wx/include/msvc" /D "WXWIDGETS" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WINDLL" /Fp"$(INTDIR)\AgilityBook.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\stdafx.obj"	"$(INTDIR)\AgilityBook.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AgilityBook - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GR /GX /O2 /I "../.." /I "../../Include" /I "../../ARB" /I "../../Win" /I "../../../../boost" /I "C:/wx/include" /I "C:/wx/include/msvc" /D "WXWIDGETS" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WINDLL" /Fp"$(INTDIR)\AgilityBook.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\stdafx.obj"	"$(INTDIR)\AgilityBook.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\Win\TabView.cpp

"$(INTDIR)\TabView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\UpdateInfo.cpp

"$(INTDIR)\UpdateInfo.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\Validators.cpp

"$(INTDIR)\Validators.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\VersionNum.cpp

"$(INTDIR)\VersionNum.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\Wizard.cpp

"$(INTDIR)\Wizard.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\WizardExcel.cpp

"$(INTDIR)\WizardExcel.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\WizardExport.cpp

"$(INTDIR)\WizardExport.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\WizardImport.cpp

"$(INTDIR)\WizardImport.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Win\WizardStart.cpp

"$(INTDIR)\WizardStart.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBAgilityRecordBook.cpp

"$(INTDIR)\ARBAgilityRecordBook.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBBase.cpp

"$(INTDIR)\ARBBase.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBBase64.cpp

"$(INTDIR)\ARBBase64.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBCalendar.cpp

"$(INTDIR)\ARBCalendar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBConfig.cpp

"$(INTDIR)\ARBConfig.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBConfigAction.cpp

"$(INTDIR)\ARBConfigAction.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBConfigCalSite.cpp

"$(INTDIR)\ARBConfigCalSite.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBConfigDivision.cpp

"$(INTDIR)\ARBConfigDivision.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBConfigEvent.cpp

"$(INTDIR)\ARBConfigEvent.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBConfigFault.cpp

"$(INTDIR)\ARBConfigFault.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBConfigLevel.cpp

"$(INTDIR)\ARBConfigLevel.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBConfigLifetimePoints.cpp

"$(INTDIR)\ARBConfigLifetimePoints.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBConfigMultiQ.cpp

"$(INTDIR)\ARBConfigMultiQ.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBConfigOtherPoints.cpp

"$(INTDIR)\ARBConfigOtherPoints.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBConfigPlaceInfo.cpp

"$(INTDIR)\ARBConfigPlaceInfo.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBConfigScoring.cpp

"$(INTDIR)\ARBConfigScoring.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBConfigSubLevel.cpp

"$(INTDIR)\ARBConfigSubLevel.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBConfigTitle.cpp

"$(INTDIR)\ARBConfigTitle.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBConfigTitlePoints.cpp

"$(INTDIR)\ARBConfigTitlePoints.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBConfigVenue.cpp

"$(INTDIR)\ARBConfigVenue.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBDate.cpp

"$(INTDIR)\ARBDate.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBDog.cpp

"$(INTDIR)\ARBDog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBDogClub.cpp

"$(INTDIR)\ARBDogClub.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBDogExistingPoints.cpp

"$(INTDIR)\ARBDogExistingPoints.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBDogNotes.cpp

"$(INTDIR)\ARBDogNotes.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBDogReferenceRun.cpp

"$(INTDIR)\ARBDogReferenceRun.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBDogRegNum.cpp

"$(INTDIR)\ARBDogRegNum.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBDogRun.cpp

"$(INTDIR)\ARBDogRun.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBDogRunOtherPoints.cpp

"$(INTDIR)\ARBDogRunOtherPoints.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBDogRunPartner.cpp

"$(INTDIR)\ARBDogRunPartner.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBDogRunScoring.cpp

"$(INTDIR)\ARBDogRunScoring.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBDogTitle.cpp

"$(INTDIR)\ARBDogTitle.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBDogTrial.cpp

"$(INTDIR)\ARBDogTrial.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBInfo.cpp

"$(INTDIR)\ARBInfo.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBInfoItem.cpp

"$(INTDIR)\ARBInfoItem.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBLocalization.cpp

"$(INTDIR)\ARBLocalization.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBString.cpp

"$(INTDIR)\ARBString.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBTraining.cpp

"$(INTDIR)\ARBTraining.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\ARBTypes.cpp

"$(INTDIR)\ARBTypes.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\BinaryData.cpp

"$(INTDIR)\BinaryData.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\ARB\Element.cpp

"$(INTDIR)\Element.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AgilityBook.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

