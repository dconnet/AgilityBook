/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Wrapper for menus to ease i18n and menu ids
 * @author David Connet
 *
 * Important: For accelerators to work, they must be in the main menu.
 * If they are only in a context menu, the accelerator will not work.
 *
 * Revision History
 * 2012-07-28 Fixed extra separator on Mac, removed Update menu icon.
 * 2012-07-13 Reworked menu creation to use a generic reusable helper.
 * 2012-07-08 Put images into menu.
 * 2012-07-04 Add option to use run time or opening time in gamble OPS.
 * 2009-09-19 Fix IdMenuRecentFiles on language switch.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-09-09 Fix Ctrl+R. See note above.
 * 2008-12-14 Created
 */

#include "stdafx.h"
#include "AgilityBookMenu.h"

#include "AgilityBook.h"
#include "ImageHelper.h"

#include "LibARBWin/MenuHelper.h"
#include <wx/artprov.h>
#include <wx/config.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


static const std::vector<CMenuHelper::ItemData> sc_items =
{
	{IdMenuNone, MENU_ITEM, 0,                        wxITEM_NORMAL, 0, nullptr, arbT("MenuFile"), nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, wxID_NEW,                 wxITEM_NORMAL, 1, arbT("MenuFileNew"), arbT("MenuFileNew"), arbT("DescFileNew"), ImageMgrNew},
	{IdMenuNone, MENU_ITEM, wxID_OPEN,                wxITEM_NORMAL, 1, arbT("MenuFileOpen"), arbT("MenuFileOpen"), arbT("DescFileOpen"), ImageMgrOpen},
	{IdMenuNone, MENU_ITEM, wxID_SAVE,                wxITEM_NORMAL, 1, arbT("MenuFileOpen"), arbT("MenuFileSave"), arbT("DescFileSave"), ImageMgrSave},
	{IdMenuNone, MENU_ITEM, wxID_SAVEAS,              wxITEM_NORMAL, 1, nullptr, arbT("MenuFileSaveAs"), arbT("DescFileSaveAs"), wxART_FILE_SAVE_AS},
	{IdMenuNone, MENU_SEP,  0,                        wxITEM_NORMAL, 1, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_FILE_LANGUAGE_CHOOSE,  wxITEM_NORMAL, 1, nullptr, arbT("MenuFileLanguage"), arbT("DescFileLanguage"), nullptr},
	{IdMenuNone, MENU_SEP,  0,                        wxITEM_NORMAL, 1, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_FILE_EXPORT_WIZARD,    wxITEM_NORMAL, 1, nullptr, arbT("MenuFileImportExport"), arbT("DescFileImportExport"), nullptr},
	{IdMenuNone, MENU_ITEM, ID_FILE_LINKED,           wxITEM_NORMAL, 1, nullptr, arbT("MenuFileLinkedFiles"), arbT("DescFileLinkedFiles"), nullptr},
	{IdMenuNone, MENU_SEP,  0,                        wxITEM_NORMAL, 1, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, wxID_PRINT,               wxITEM_NORMAL, 1, arbT("MenuFilePrint"), arbT("MenuFilePrint"), arbT("DescFilePrint"), ImageMgrPrint},
	{IdMenuNone, MENU_ITEM, ID_FILE_PRINT_BLANK_RUNS, wxITEM_NORMAL, 1, nullptr, arbT("MenuFilePrintBlank"), arbT("DescFilePrintBlank"), nullptr},
	{IdMenuNone, MENU_ITEM, wxID_PREVIEW,             wxITEM_NORMAL, 1, arbT("MenuFilePrintPreview"), arbT("MenuFilePrintPreview"), arbT("DescFilePrintPreview"), ImageMgrPreview},
	{IdMenuNone, MENU_MRU,  0,                        wxITEM_NORMAL, 1, nullptr, arbT("MenuFileRecent"), nullptr, nullptr},
	{IdMenuNone, MENU_SEP,  0,                        wxITEM_NORMAL, 1, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_FILE_PROPERTIES,       wxITEM_NORMAL, 1, nullptr, arbT("MenuFileProperties"), arbT("DescFileProperties"), nullptr},
	{IdMenuNone, MENU_SEP,  0,                        wxITEM_NORMAL, 1, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, wxID_EXIT,                wxITEM_NORMAL, 1, nullptr, arbT("MenuFileExit"), arbT("DescFileExit"), nullptr},

	{IdMenuNone, MENU_ITEM, 0,                        wxITEM_NORMAL, 0, nullptr, arbT("MenuEdit"), nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, wxID_CUT,                 wxITEM_NORMAL, 1, arbT("MenuEditCut"), arbT("MenuEditCut"), arbT("DescEditCut"), ImageMgrCut},
	{IdMenuNone, MENU_ITEM, wxID_COPY,                wxITEM_NORMAL, 1, arbT("MenuEditCopy"), arbT("MenuEditCopy"), arbT("DescEditCopy"), ImageMgrCopy},
	{IdMenuNone, MENU_ITEM, wxID_PASTE,               wxITEM_NORMAL, 1, arbT("MenuEditPaste"), arbT("MenuEditPaste"), arbT("DescEditPaste"), ImageMgrPaste},
	{IdMenuNone, MENU_ITEM, wxID_DUPLICATE,           wxITEM_NORMAL, 1, nullptr, arbT("MenuEditDuplicate"), arbT("DescEditDuplicate"), nullptr},
	{IdMenuNone, MENU_ITEM, wxID_DELETE,              wxITEM_NORMAL, 1, arbT("MenuEditDelete"), arbT("MenuEditDelete"), arbT("DescEditDelete"), ImageMgrDelete},
	{IdMenuNone, MENU_SEP,  0,                        wxITEM_NORMAL, 1, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, wxID_SELECTALL,           wxITEM_NORMAL, 1, nullptr, arbT("MenuEditSelectAll"), arbT("DescEditSelectAll"), nullptr},
	{IdMenuNone, MENU_SEP,  0,                        wxITEM_NORMAL, 1, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_COPY_TITLES_LIST,      wxITEM_NORMAL, 1, nullptr, arbT("MenuEditCopyTitles"), arbT("DescEditCopyTitles"), nullptr},
	{IdMenuNone, MENU_ITEM, ID_REORDER,               wxITEM_NORMAL, 1, nullptr, arbT("MenuEditReorder"), arbT("DescEditReorder"), nullptr},
	{IdMenuNone, MENU_SEP,  0,                        wxITEM_NORMAL, 1, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, wxID_FIND,                wxITEM_NORMAL, 1, nullptr, arbT("MenuEditFind"), arbT("DescEditFind"), nullptr},
	{IdMenuNone, MENU_ITEM, ID_EDIT_FIND_NEXT,        wxITEM_NORMAL, 1, nullptr, arbT("MenuEditFindNext"), arbT("DescEditFindNext"), nullptr},
	{IdMenuNone, MENU_ITEM, ID_EDIT_FIND_PREVIOUS,    wxITEM_NORMAL, 1, nullptr, arbT("MenuEditFindPrev"), arbT("DescEditFindPrev"), nullptr},
	{IdMenuNone, MENU_SEP,  0,                        wxITEM_NORMAL, 1, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_EDIT_CONFIGURATION,    wxITEM_NORMAL, 1, nullptr, arbT("MenuEditConfig"), arbT("DescEditConfig"), nullptr},

	{IdMenuNone, MENU_ITEM, 0,                               wxITEM_NORMAL, 0, nullptr, arbT("MenuAgility"), nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, 0,                               wxITEM_NORMAL, 1, nullptr, arbT("MenuDog"), nullptr, ImageMgrDog},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_EDIT_DOG,             wxITEM_NORMAL, 2, nullptr, arbT("MenuDogProperties"), arbT("DescDogProperties"), ImageMgrDog},
	{IdMenuNone, MENU_SEP,  0,                               wxITEM_NORMAL, 2, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_NEW_DOG,              wxITEM_NORMAL, 2, arbT("MenuNew"), arbT("MenuNew"), arbT("DescDogNew"), ImageMgrDog},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_DELETE_DOG,           wxITEM_NORMAL, 2, nullptr, arbT("MenuDogDelete"), arbT("DescDogDelete"), nullptr},
	{IdMenuNone, MENU_ITEM, 0,                               wxITEM_NORMAL, 1, nullptr, arbT("MenuTitle"), nullptr, ImageMgrTitle},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_EDIT_TITLE,           wxITEM_NORMAL, 2, nullptr, arbT("MenuTitleProperties"), arbT("DescTitleProperties"), ImageMgrTitle},
	{IdMenuNone, MENU_SEP,  0,                               wxITEM_NORMAL, 2, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_NEW_TITLE,            wxITEM_NORMAL, 2, arbT("MenuNew"), arbT("MenuNew"), arbT("DescTitleNew"), ImageMgrTitle},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_DELETE_TITLE,         wxITEM_NORMAL, 2, nullptr, arbT("MenuTitleDelete"), arbT("DescTitleDelete"), nullptr},
	{IdMenuNone, MENU_ITEM, 0,                               wxITEM_NORMAL, 1, nullptr, arbT("MenuTrial"), nullptr, ImageMgrTrial},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_EDIT_TRIAL,           wxITEM_NORMAL, 2, nullptr, arbT("MenuTrialProperties"), arbT("DescTrialProperties"), ImageMgrTrial},
	{IdMenuNone, MENU_SEP,  0,                               wxITEM_NORMAL, 2, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_NEW_TRIAL,            wxITEM_NORMAL, 2, arbT("MenuNew"), arbT("MenuNew"), arbT("DescTrialNew"), ImageMgrTrial},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_DELETE_TRIAL,         wxITEM_NORMAL, 2, nullptr, arbT("MenuTrialDelete"), arbT("DescTrialDelete"), nullptr},
	{IdMenuNone, MENU_SEP,  0,                               wxITEM_NORMAL, 2, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_PRINT_TRIAL,          wxITEM_NORMAL, 2, nullptr, arbT("MenuTrialPrintRuns"), arbT("DescTrialPrintRuns"), nullptr},
	{IdMenuNone, MENU_ITEM, 0,                               wxITEM_NORMAL, 1, nullptr, arbT("MenuRun"), nullptr, ImageMgrRuns},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_EDIT_RUN,             wxITEM_NORMAL, 2, nullptr, arbT("MenuRunProperties"), arbT("DescRunProperties"), ImageMgrRuns},
	{IdMenuNone, MENU_SEP,  0,                               wxITEM_NORMAL, 2, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_NEW_RUN,              wxITEM_NORMAL, 2, arbT("MenuNewRun"), arbT("MenuNewRun"), arbT("DescRunNew"), ImageMgrRuns},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_DELETE_RUN,           wxITEM_NORMAL, 2, nullptr, arbT("MenuRunDelete"), arbT("DescRunDelete"), nullptr},
	{IdMenuNone, MENU_SEP,  0,                               wxITEM_NORMAL, 2, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_PRINT_RUNS,           wxITEM_NORMAL, 2, nullptr, arbT("MenuRunPrintRuns"), arbT("DescRunPrintRuns"), nullptr},
	{IdMenuNone, MENU_ITEM, 0,                               wxITEM_NORMAL, 1, nullptr, arbT("MenuCalendar"), nullptr, ImageMgrCalendar},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_EDIT_CALENDAR,        wxITEM_NORMAL, 2, nullptr, arbT("MenuCalendarProperties"), arbT("DescCalendarProperties"), ImageMgrCalendar},
	{IdMenuNone, MENU_SEP,  0,                               wxITEM_NORMAL, 2, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_NEW_CALENDAR,         wxITEM_NORMAL, 2, arbT("MenuNew"), arbT("MenuNew"), arbT("DescCalendarNew"), ImageMgrCalendar},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_DELETE_CALENDAR,      wxITEM_NORMAL, 2, nullptr, arbT("MenuCalendarDelete"), arbT("DescCalendarDelete"), nullptr},
	{IdMenuNone, MENU_SEP,  0,                               wxITEM_NORMAL, 2, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_UPDATE_CALENDAR,      wxITEM_NORMAL, 2, nullptr, arbT("MenuCalendarUpdate"), arbT("DescCalendarUpdate"), nullptr},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_EXPORT_CALENDAR,      wxITEM_NORMAL, 2, nullptr, arbT("MenuCalendarExport"), arbT("DescCalendarExport"), nullptr},
	{IdMenuNone, MENU_SEP,  0,                               wxITEM_NORMAL, 2, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_CREATEENTRY_CALENDAR, wxITEM_NORMAL, 2, nullptr, arbT("MenuCalendarTrialEntry"), arbT("DescCalendarTrialEntry"), nullptr},
	{IdMenuNone, MENU_ITEM, 0,                               wxITEM_NORMAL, 1, nullptr, arbT("MenuTraining"), nullptr, ImageMgrTraining},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_EDIT_TRAINING,        wxITEM_NORMAL, 2, nullptr, arbT("MenuTrainingProperties"), arbT("DescTrainingProperties"), ImageMgrTraining},
	{IdMenuNone, MENU_SEP,  0,                               wxITEM_NORMAL, 2, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_NEW_TRAINING,         wxITEM_NORMAL, 2, arbT("MenuNew"), arbT("MenuNew"), arbT("DescTrainingNew"), ImageMgrTraining},
	{IdMenuNone, MENU_ITEM, ID_AGILITY_DELETE_TRAINING,      wxITEM_NORMAL, 2, nullptr, arbT("MenuTrainingDelete"), arbT("DescTrainingDelete"), nullptr},

	{IdMenuNone, MENU_ITEM, 0,                        wxITEM_NORMAL, 0, nullptr, arbT("MenuNotes"), nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_NOTES_CLUBS,           wxITEM_NORMAL, 1, nullptr, arbT("MenuNotesClubs"), arbT("DescNotesClubs"), nullptr},
	{IdMenuNone, MENU_ITEM, ID_NOTES_JUDGES,          wxITEM_NORMAL, 1, nullptr, arbT("MenuNotesJudges"), arbT("DescNotesJudges"), nullptr},
	{IdMenuNone, MENU_ITEM, ID_NOTES_LOCATIONS,       wxITEM_NORMAL, 1, nullptr, arbT("MenuNotesLocations"), arbT("DescNotesLocations"), nullptr},
	{IdMenuNone, MENU_SEP,  0,                        wxITEM_NORMAL, 1, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_NOTES_SEARCH,          wxITEM_NORMAL, 1, nullptr, arbT("MenuNotesSearch"), arbT("DescNotesSearch"), nullptr},

	{IdMenuNone, MENU_ITEM, 0,                        wxITEM_NORMAL, 0, nullptr, arbT("MenuView"), nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, wxID_PREFERENCES,         wxITEM_NORMAL, 1, nullptr, arbT("MenuViewOptions"), arbT("DescViewOptions"), nullptr},
	{IdMenuNone, MENU_ITEM, ID_VIEW_CUSTOMIZE,        wxITEM_NORMAL, 1, nullptr, arbT("MenuViewCustomize"), arbT("DescViewCustomize"), nullptr},
	{IdMenuNone, MENU_ITEM, ID_VIEW_CUSTOMIZE_ACCEL,  wxITEM_NORMAL, 1, nullptr, arbT("MenuViewCustomizeAccel"), arbT("DescViewCustomizeAccel"), nullptr},
	{IdMenuNone, MENU_SEP,  0,                        wxITEM_NORMAL, 1, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_UNSORT,                wxITEM_NORMAL, 1, nullptr, arbT("MenuUnsort"), arbT("DescUnsort"), nullptr},
	{IdMenuNone, MENU_SEP,  0,                        wxITEM_NORMAL, 1, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_VIEW_SORTRUNS,         wxITEM_CHECK,  1, nullptr, arbT("MenuViewNewestTrials"), arbT("DescViewNewestTrials"), nullptr},
	{IdMenuNone, MENU_ITEM, ID_VIEW_RUNS_BY_TRIAL,    wxITEM_CHECK,  1, nullptr, arbT("MenuViewRunsByTrial"), arbT("DescViewRunsByTrial"), nullptr},
	{IdMenuNone, MENU_ITEM, ID_VIEW_TABLE_IN_YPS,     wxITEM_CHECK,  1, nullptr, arbT("MenuViewIncludeTable"), arbT("DescViewIncludeTable"), nullptr},
	{IdMenuNone, MENU_ITEM, ID_VIEW_RUNTIME_IN_OPS,   wxITEM_CHECK,  1, nullptr, arbT("MenuViewIncludeRunTime"), arbT("DescViewIncludeRunTime"), nullptr},
	{IdMenuNone, MENU_SEP,  0,                        wxITEM_NORMAL, 1, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_VIEW_POINTS_VIEW_SORT, wxITEM_NORMAL, 1, nullptr, arbT("MenuViewSortPoints"), arbT("DescViewSortPoints"), nullptr},
	{IdMenuNone, MENU_ITEM, ID_VIEW_HIDDEN,           wxITEM_CHECK,  1, nullptr, arbT("MenuViewHiddenTitles"), arbT("DescViewHiddenTitles"), nullptr},
	{IdMenuNone, MENU_ITEM, ID_VIEW_LIFETIME_EVENTS,  wxITEM_CHECK,  1, nullptr, arbT("MenuViewLifePoints"), arbT("DescViewLifePoints"), nullptr},
	{IdMenuNone, MENU_SEP,  0,                        wxITEM_NORMAL, 1, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_EXPAND,                wxITEM_NORMAL, 1, nullptr, arbT("MenuViewExpand"), arbT("DescViewExpand"), nullptr},
	{IdMenuNone, MENU_ITEM, ID_COLLAPSE,              wxITEM_NORMAL, 1, nullptr, arbT("MenuViewCollapse"), arbT("DescViewCollapse"), nullptr},
	{IdMenuNone, MENU_ITEM, ID_EXPAND_ALL,            wxITEM_NORMAL, 1, nullptr, arbT("MenuViewExpandAll"), arbT("DescViewExpandAll"), nullptr},
	{IdMenuNone, MENU_ITEM, ID_COLLAPSE_ALL,          wxITEM_NORMAL, 1, nullptr, arbT("MenuViewCollapseAll"), arbT("DescViewCollapseAll"), nullptr},
	{IdMenuNone, MENU_SEP,  0,                        wxITEM_NORMAL, 1, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, 0,                        wxITEM_NORMAL, 1, nullptr, arbT("MenuViewType"), nullptr, nullptr},
#if wxUSE_NOTEBOOK
	{IdMenuNone, MENU_ITEM, ID_BOOK_NOTEBOOK,         wxITEM_RADIO,  2, nullptr, arbT("MenuViewNotebook"), arbT("DescViewNotebook"), nullptr},
#endif
#if wxUSE_LISTBOOK
	{IdMenuNone, MENU_ITEM, ID_BOOK_LISTBOOK,         wxITEM_RADIO,  2, nullptr, arbT("MenuViewListbook"), arbT("DescViewListbook"), nullptr},
#endif
#if wxUSE_CHOICEBOOK
	{IdMenuNone, MENU_ITEM, ID_BOOK_CHOICEBOOK,       wxITEM_RADIO,  2, nullptr, arbT("MenuViewChoicebook"), arbT("DescViewChoicebook"), nullptr},
#endif
#if wxUSE_TREEBOOK
	{IdMenuNone, MENU_ITEM, ID_BOOK_TREEBOOK,         wxITEM_RADIO,  2, nullptr, arbT("MenuViewTreebook"), arbT("DescViewTreebook"), nullptr},
#endif
#if wxUSE_TOOLBOOK
	{IdMenuNone, MENU_ITEM, ID_BOOK_TOOLBOOK,         wxITEM_RADIO,  2, nullptr, arbT("MenuViewToolbook"), arbT("DescViewToolbook"), nullptr},
#endif
	{IdMenuNone, MENU_ITEM, 0,                        wxITEM_NORMAL, 1, nullptr, arbT("MenuViewOrientation"), nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_ORIENT_TOP,            wxITEM_RADIO,  2, nullptr, arbT("MenuViewTop"), arbT("DescViewTop"), nullptr},
	{IdMenuNone, MENU_ITEM, ID_ORIENT_BOTTOM,         wxITEM_RADIO,  2, nullptr, arbT("MenuViewBottom"), arbT("DescViewBottom"), nullptr},
	{IdMenuNone, MENU_ITEM, ID_ORIENT_LEFT,           wxITEM_RADIO,  2, nullptr, arbT("MenuViewLeft"), arbT("DescViewLeft"), nullptr},
	{IdMenuNone, MENU_ITEM, ID_ORIENT_RIGHT,          wxITEM_RADIO,  2, nullptr, arbT("MenuViewRight"), arbT("DescViewRight"), nullptr},

	{IdMenuNone, MENU_ITEM, 0,                        wxITEM_NORMAL, 0, nullptr, arbT("MenuWindow"), nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_WINDOW_SPLIT,          wxITEM_NORMAL, 1, nullptr, arbT("MenuWindowSplitter"), arbT("DescWindowSplitter"), nullptr},
	{IdMenuNone, MENU_SEP,  0,                        wxITEM_NORMAL, 1, nullptr, nullptr, nullptr, nullptr},
	{IdMenuNone, MENU_ITEM, ID_NEXT_PANE,             wxITEM_NORMAL, 1, nullptr, arbT("MenuWindowNext"), arbT("DescWindowNext"), nullptr},
	{IdMenuNone, MENU_ITEM, ID_PREV_PANE,             wxITEM_NORMAL, 1, nullptr, arbT("MenuWindowPrev"), arbT("DescWindowPrev"), nullptr},

	{IdMenuNone, MENU_ITEM, 0,                        wxITEM_NORMAL, 0, nullptr, arbT("MenuHelp"), nullptr, nullptr},
#ifdef _DEBUG
	{IdMenuNone, MENU_ITEM, ID_HELP_DEBUG,            wxITEM_NORMAL, 1, nullptr, L"Debug: Test Feature", L"Debug Helper", nullptr},
#endif
	{IdMenuNone, MENU_ITEM, ID_HELP_UPDATE,           wxITEM_NORMAL, 1, nullptr, arbT("MenuHelpUpdates"), arbT("DescHelpUpdates"), nullptr},
	{IdMenuNone, MENU_ITEM, ID_HELP_SYSINFO,          wxITEM_NORMAL, 1, nullptr, arbT("MenuHelpSysInfo"), arbT("DescHelpSysInfo"), nullptr},
#ifndef __WXMAC__
	{IdMenuNone, MENU_SEP,  0,                        wxITEM_NORMAL, 1, nullptr, nullptr, nullptr, nullptr},
#endif
	{IdMenuNone, MENU_HELP, wxID_ABOUT,               wxITEM_NORMAL, 1, arbT("MenuHelpAbout"), arbT("MenuHelpAbout"), arbT("DescHelpAbout"), ImageMgrAbout},

	{IdMenuDog, MENU_ITEM, ID_AGILITY_EDIT_DOG,   wxITEM_NORMAL, 0, nullptr, _("MenuDogProperties"), _("DescDogProperties"), ImageMgrDog},
	{IdMenuDog, MENU_ITEM, wxID_PREFERENCES,      wxITEM_NORMAL, 0, nullptr, _("MenuViewOptions"), _("DescViewOptions"), nullptr},
	{IdMenuDog, MENU_SEP,  0,                     wxITEM_NORMAL, 0, nullptr, nullptr, nullptr, nullptr},
	{IdMenuDog, MENU_ITEM, 0,                     wxITEM_NORMAL, 0, nullptr, arbT("MenuNew"), nullptr, nullptr},
	{IdMenuDog, MENU_ITEM, ID_AGILITY_NEW_DOG,    wxITEM_NORMAL, 1, nullptr, arbT("MenuDogNew"), arbT("DescDogNew"), ImageMgrDog},
	{IdMenuDog, MENU_ITEM, ID_AGILITY_NEW_TITLE,  wxITEM_NORMAL, 1, nullptr, arbT("MenuTitleNew"), arbT("DescTitleNew"), ImageMgrTitle},
	{IdMenuDog, MENU_SEP,  0,                     wxITEM_NORMAL, 1, nullptr, nullptr, nullptr, nullptr},
	{IdMenuDog, MENU_ITEM, ID_AGILITY_NEW_TRIAL,  wxITEM_NORMAL, 1, nullptr, arbT("MenuTrialNew"), arbT("DescTrialNew"), ImageMgrTrial},
	{IdMenuDog, MENU_ITEM, wxID_CUT,              wxITEM_NORMAL, 0, nullptr, _("MenuEditCut"), _("DescEditCut"), ImageMgrCut},
	{IdMenuDog, MENU_ITEM, wxID_COPY,             wxITEM_NORMAL, 0, nullptr, _("MenuEditCopy"), _("DescEditCopy"), ImageMgrCopy},
	{IdMenuDog, MENU_ITEM, wxID_PASTE,            wxITEM_NORMAL, 0, nullptr, _("MenuEditPaste"), _("DescEditPaste"), ImageMgrPaste},
	{IdMenuDog, MENU_ITEM, wxID_DUPLICATE,        wxITEM_NORMAL, 0, nullptr, _("MenuEditDuplicate"), _("DescEditDuplicate"), nullptr},
	{IdMenuDog, MENU_ITEM, ID_AGILITY_DELETE_DOG, wxITEM_NORMAL, 0, nullptr, _("MenuDogDelete"), _("DescDogDelete"), nullptr},
	{IdMenuDog, MENU_SEP,  0,                     wxITEM_NORMAL, 0, nullptr, nullptr, nullptr, nullptr},
	{IdMenuDog, MENU_ITEM, ID_REORDER,            wxITEM_NORMAL, 0, nullptr, _("MenuEditReorder"), _("DescEditReorder"), nullptr},
	{IdMenuDog, MENU_SEP,  0,                     wxITEM_NORMAL, 0, nullptr, nullptr, nullptr, nullptr},
	{IdMenuDog, MENU_ITEM, ID_EXPAND,             wxITEM_NORMAL, 0, nullptr, _("MenuViewExpand"), _("DescViewExpand"), nullptr},
	{IdMenuDog, MENU_ITEM, ID_COLLAPSE,           wxITEM_NORMAL, 0, nullptr, _("MenuViewCollapse"), _("DescViewCollapse"), nullptr},
	{IdMenuDog, MENU_ITEM, ID_EXPAND_ALL,         wxITEM_NORMAL, 0, nullptr, _("MenuViewExpandAll"), _("DescViewExpandAll"), nullptr},
	{IdMenuDog, MENU_ITEM, ID_COLLAPSE_ALL,       wxITEM_NORMAL, 0, nullptr, _("MenuViewCollapseAll"), _("DescViewCollapseAll"), nullptr},

	{IdMenuTrial, MENU_ITEM, ID_AGILITY_EDIT_TRIAL,   wxITEM_NORMAL, 0, nullptr, _("MenuTrialProperties"), _("DescTrialProperties"), ImageMgrTrial},
	{IdMenuTrial, MENU_ITEM, wxID_PREFERENCES,        wxITEM_NORMAL, 0, nullptr, _("MenuViewOptions"), _("DescViewOptions"), nullptr},
	{IdMenuTrial, MENU_SEP,  0,                       wxITEM_NORMAL, 0, nullptr, nullptr, nullptr, nullptr},
	{IdMenuTrial, MENU_ITEM, 0,                       wxITEM_NORMAL, 0, nullptr, arbT("MenuNew"), nullptr, nullptr},
	{IdMenuTrial, MENU_ITEM, ID_AGILITY_NEW_DOG,      wxITEM_NORMAL, 1, nullptr, arbT("MenuDogNew"), arbT("DescDogNew"), ImageMgrDog},
	{IdMenuTrial, MENU_ITEM, ID_AGILITY_NEW_TITLE,    wxITEM_NORMAL, 1, nullptr, arbT("MenuTitleNew"), arbT("DescTitleNew"), ImageMgrTitle},
	{IdMenuTrial, MENU_SEP,  0,                       wxITEM_NORMAL, 1, nullptr, nullptr, nullptr, nullptr},
	{IdMenuTrial, MENU_ITEM, ID_AGILITY_NEW_TRIAL,    wxITEM_NORMAL, 1, nullptr, arbT("MenuTrialNew"), arbT("DescTrialNew"), ImageMgrTrial},
	{IdMenuTrial, MENU_ITEM, ID_AGILITY_NEW_RUN,      wxITEM_NORMAL, 1, nullptr, arbT("MenuRunNew"), arbT("DescRunNew"), ImageMgrRuns},
	{IdMenuTrial, MENU_ITEM, wxID_CUT,                wxITEM_NORMAL, 0, nullptr, _("MenuEditCut"), _("DescEditCut"), ImageMgrCut},
	{IdMenuTrial, MENU_ITEM, wxID_COPY,               wxITEM_NORMAL, 0, nullptr, _("MenuEditCopy"), _("DescEditCopy"), ImageMgrCopy},
	{IdMenuTrial, MENU_ITEM, wxID_PASTE,              wxITEM_NORMAL, 0, nullptr, _("MenuEditPaste"), _("DescEditPaste"), ImageMgrPaste},
	{IdMenuTrial, MENU_ITEM, wxID_DUPLICATE,          wxITEM_NORMAL, 0, nullptr, _("MenuEditDuplicate"), _("DescEditDuplicate"), nullptr},
	{IdMenuTrial, MENU_ITEM, ID_AGILITY_DELETE_TRIAL, wxITEM_NORMAL, 0, nullptr, arbT("MenuTrialDelete"), arbT("DescTrialDelete"), nullptr},
	{IdMenuTrial, MENU_SEP,  0,                       wxITEM_NORMAL, 0, nullptr, nullptr, nullptr, nullptr},
	{IdMenuTrial, MENU_ITEM, ID_REORDER,              wxITEM_NORMAL, 0, nullptr, _("MenuEditReorder"), _("DescEditReorder"), nullptr},
	{IdMenuTrial, MENU_SEP,  0,                       wxITEM_NORMAL, 0, nullptr, nullptr, nullptr, nullptr},
	{IdMenuTrial, MENU_ITEM, ID_EXPAND,               wxITEM_NORMAL, 0, nullptr, _("MenuViewExpand"), _("DescViewExpand"), nullptr},
	{IdMenuTrial, MENU_ITEM, ID_COLLAPSE,             wxITEM_NORMAL, 0, nullptr, _("MenuViewCollapse"), _("DescViewCollapse"), nullptr},
	{IdMenuTrial, MENU_SEP,  0,                       wxITEM_NORMAL, 0, nullptr, nullptr, nullptr, nullptr},
	{IdMenuTrial, MENU_ITEM, ID_AGILITY_PRINT_TRIAL,  wxITEM_NORMAL, 0, nullptr, _("MenuTrialPrintRuns"), _("DescTrialPrintRuns"), nullptr},

	{IdMenuRun, MENU_ITEM, ID_AGILITY_EDIT_RUN,    wxITEM_NORMAL, 0, nullptr, _("MenuRunProperties"), _("DescRunProperties"), ImageMgrRuns},
	{IdMenuRun, MENU_ITEM, wxID_PREFERENCES,       wxITEM_NORMAL, 0, nullptr, _("MenuViewOptions"), _("DescViewOptions"), nullptr},
	{IdMenuRun, MENU_SEP,  0,                      wxITEM_NORMAL, 0, nullptr, nullptr, nullptr, nullptr},
	{IdMenuRun, MENU_ITEM, 0,                      wxITEM_NORMAL, 0, nullptr, arbT("MenuNew"), nullptr, nullptr},
	{IdMenuRun, MENU_ITEM, ID_AGILITY_NEW_DOG,     wxITEM_NORMAL, 1, nullptr, arbT("MenuDogNew"), arbT("DescDogNew"), ImageMgrDog},
	{IdMenuRun, MENU_ITEM, ID_AGILITY_NEW_TITLE,   wxITEM_NORMAL, 1, nullptr, arbT("MenuTitleNew"), arbT("DescTitleNew"), ImageMgrTitle},
	{IdMenuRun, MENU_SEP,  0,                      wxITEM_NORMAL, 1, nullptr, nullptr, nullptr, nullptr},
	{IdMenuRun, MENU_ITEM, ID_AGILITY_NEW_TRIAL,   wxITEM_NORMAL, 1, nullptr, arbT("MenuTrialNew"), arbT("DescTrialNew"), ImageMgrTrial},
	{IdMenuRun, MENU_ITEM, ID_AGILITY_NEW_RUN,     wxITEM_NORMAL, 1, nullptr, arbT("MenuRunNew"), arbT("DescRunNew"), ImageMgrRuns},
	{IdMenuRun, MENU_ITEM, wxID_CUT,               wxITEM_NORMAL, 0, nullptr, _("MenuEditCut"), _("DescEditCut"), ImageMgrCut},
	{IdMenuRun, MENU_ITEM, wxID_COPY,              wxITEM_NORMAL, 0, nullptr, _("MenuEditCopy"), _("DescEditCopy"), ImageMgrCopy},
	{IdMenuRun, MENU_ITEM, wxID_PASTE,             wxITEM_NORMAL, 0, nullptr, _("MenuEditPaste"), _("DescEditPaste"), ImageMgrPaste},
	{IdMenuRun, MENU_ITEM, wxID_DUPLICATE,         wxITEM_NORMAL, 0, nullptr, _("MenuEditDuplicate"), _("DescEditDuplicate"), nullptr},
	{IdMenuRun, MENU_ITEM, ID_AGILITY_DELETE_RUN,  wxITEM_NORMAL, 0, nullptr, arbT("MenuRunDelete"), arbT("DescRunDelete"), nullptr},
	{IdMenuRun, MENU_SEP,  0,                      wxITEM_NORMAL, 0, nullptr, nullptr, nullptr, nullptr},
	{IdMenuRun, MENU_ITEM, ID_REORDER,             wxITEM_NORMAL, 0, nullptr, _("MenuEditReorder"), _("DescEditReorder"), nullptr},
	{IdMenuRun, MENU_SEP,  0,                      wxITEM_NORMAL, 0, nullptr, nullptr, nullptr, nullptr},
	{IdMenuRun, MENU_ITEM, ID_AGILITY_PRINT_TRIAL, wxITEM_NORMAL, 0, nullptr, _("MenuTrialPrintRuns"), _("DescTrialPrintRuns"), nullptr},
	{IdMenuRun, MENU_ITEM, ID_AGILITY_PRINT_RUNS,  wxITEM_NORMAL, 0, nullptr, _("MenuRunPrintRuns"), _("DescRunPrintRuns"), nullptr},

	{IdMenuPoints, MENU_ITEM, ID_DETAILS,          wxITEM_NORMAL, 0, nullptr, _("MenuDetails"), _("DescDetails"), nullptr},
	{IdMenuPoints, MENU_ITEM, wxID_PREFERENCES,    wxITEM_NORMAL, 0, nullptr, _("MenuViewOptions"), _("DescViewOptions"), nullptr},
	{IdMenuPoints, MENU_SEP,  0,                   wxITEM_NORMAL, 0, nullptr, nullptr, nullptr, nullptr},
	{IdMenuPoints, MENU_ITEM, wxID_COPY,           wxITEM_NORMAL, 0, nullptr, _("MenuEditCopy"), _("DescEditCopy"), ImageMgrCopy},
	{IdMenuPoints, MENU_ITEM, ID_COPY_TITLES_LIST, wxITEM_NORMAL, 0, nullptr, _("MenuEditCopyTitles"), _("DescEditCopyTitles"), nullptr},
	{IdMenuPoints, MENU_SEP,  0,                   wxITEM_NORMAL, 0, nullptr, nullptr, nullptr, nullptr},
	{IdMenuPoints, MENU_ITEM, wxID_SELECTALL,      wxITEM_NORMAL, 0, nullptr, _("MenuEditSelectAll"), _("DescEditSelectAll"), nullptr},

	{IdMenuCalendar, MENU_ITEM, ID_AGILITY_EDIT_CALENDAR,        wxITEM_NORMAL, 0, nullptr, _("MenuCalendarProperties"), _("DescCalendarProperties"), ImageMgrCalendar},
	{IdMenuCalendar, MENU_ITEM, wxID_PREFERENCES,                wxITEM_NORMAL, 0, nullptr, _("MenuViewOptions"), _("DescViewOptions"), nullptr},
	{IdMenuCalendar, MENU_SEP,  0,                               wxITEM_NORMAL, 0, nullptr, nullptr, nullptr, nullptr},
	{IdMenuCalendar, MENU_ITEM, ID_AGILITY_NEW_CALENDAR,         wxITEM_NORMAL, 0, nullptr, arbT("MenuNew"), arbT("DescCalendarNew"), ImageMgrCalendar},
	{IdMenuCalendar, MENU_ITEM, wxID_CUT,                        wxITEM_NORMAL, 0, nullptr, _("MenuEditCut"), _("DescEditCut"), ImageMgrCut},
	{IdMenuCalendar, MENU_ITEM, wxID_COPY,                       wxITEM_NORMAL, 0, nullptr, _("MenuEditCopy"), _("DescEditCopy"), ImageMgrCopy},
	{IdMenuCalendar, MENU_ITEM, wxID_PASTE,                      wxITEM_NORMAL, 0, nullptr, _("MenuEditPaste"), _("DescEditPaste"), ImageMgrPaste},
	{IdMenuCalendar, MENU_ITEM, wxID_DUPLICATE,                  wxITEM_NORMAL, 0, nullptr, _("MenuEditDuplicate"), _("DescEditDuplicate"), nullptr},
	{IdMenuCalendar, MENU_ITEM, ID_AGILITY_DELETE_CALENDAR,      wxITEM_NORMAL, 0, nullptr, arbT("MenuCalendarDelete"), arbT("DescCalendarDelete"), nullptr},
	{IdMenuCalendar, MENU_SEP,  0,                               wxITEM_NORMAL, 0, nullptr, nullptr, nullptr, nullptr},
	{IdMenuCalendar, MENU_ITEM, ID_AGILITY_UPDATE_CALENDAR,      wxITEM_NORMAL, 0, nullptr, arbT("MenuCalendarUpdate"), arbT("DescCalendarUpdate"), nullptr},
	{IdMenuCalendar, MENU_ITEM, ID_AGILITY_EXPORT_CALENDAR,      wxITEM_NORMAL, 0, nullptr, arbT("MenuCalendarExport"), arbT("DescCalendarExport"), nullptr},
	{IdMenuCalendar, MENU_SEP,  0,                               wxITEM_NORMAL, 0, nullptr, nullptr, nullptr, nullptr},
	{IdMenuCalendar, MENU_ITEM, ID_AGILITY_CREATEENTRY_CALENDAR, wxITEM_NORMAL, 0, nullptr, arbT("MenuCalendarTrialEntry"), arbT("DescCalendarTrialEntry"), nullptr},

	{IdMenuTraining, MENU_ITEM, ID_AGILITY_EDIT_TRAINING,   wxITEM_NORMAL, 0, nullptr, _("MenuTrainingProperties"), _("DescTrainingProperties"), ImageMgrTraining},
	{IdMenuTraining, MENU_ITEM, wxID_PREFERENCES,           wxITEM_NORMAL, 0, nullptr, _("MenuViewOptions"), _("DescViewOptions"), nullptr},
	{IdMenuTraining, MENU_SEP,  0,                          wxITEM_NORMAL, 0, nullptr, nullptr, nullptr, nullptr},
	{IdMenuTraining, MENU_ITEM, ID_AGILITY_NEW_TRAINING,    wxITEM_NORMAL, 0, nullptr, arbT("MenuNew"), arbT("DescTrainingNew"), ImageMgrTraining},
	{IdMenuTraining, MENU_ITEM, wxID_CUT,                   wxITEM_NORMAL, 0, nullptr, _("MenuEditCut"), _("DescEditCut"), ImageMgrCut},
	{IdMenuTraining, MENU_ITEM, wxID_COPY,                  wxITEM_NORMAL, 0, nullptr, _("MenuEditCopy"), _("DescEditCopy"), ImageMgrCopy},
	{IdMenuTraining, MENU_ITEM, wxID_PASTE,                 wxITEM_NORMAL, 0, nullptr, _("MenuEditPaste"), _("DescEditPaste"), ImageMgrPaste},
	{IdMenuTraining, MENU_ITEM, wxID_DUPLICATE,             wxITEM_NORMAL, 0, nullptr, _("MenuEditDuplicate"), _("DescEditDuplicate"), nullptr},
	{IdMenuTraining, MENU_ITEM, ID_AGILITY_DELETE_TRAINING, wxITEM_NORMAL, 0, nullptr, arbT("MenuTrainingDelete"), arbT("DescTrainingDelete"), nullptr},
};

// Note: The first number cannot change. It is stored to the registry.
// Future changes all have to add new numbers. If old entries go away,
// just comment them out (as a reserved spot).
static const std::vector<CMenuHelper::ItemAccel> sc_accels =
{
	{1,  wxID_NEW,                    true, false, false, 'N'},
	{2,  wxID_OPEN,                   true, false, false, 'O'},
	{3,  wxID_SAVE,                   true, false, false, 'S'},
	{4,  wxID_SAVEAS,                false, false, false, WXK_F12},
	{5,  ID_FILE_LANGUAGE_CHOOSE,    false, false, false, 0},
	{6,  ID_FILE_EXPORT_WIZARD,       true, false, false, 'E'},
	{7,  ID_FILE_LINKED,             false, false, false, 0},
	{8,  wxID_PRINT,                  true, false, false, 'P'},
	{9,  ID_FILE_PRINT_BLANK_RUNS,   false, false, false, 0},
	{10, wxID_PREVIEW,                true,  true, false, 'P'},
	{11, ID_FILE_PROPERTIES,         false, false, false, 0},
	{12, wxID_EXIT,                  false, false, false, 0},
	{13, wxID_CUT,                    true, false, false, 'X'},
	{14, wxID_COPY,                   true, false, false, 'C'},
	{15, wxID_PASTE,                  true, false, false, 'V'},
	{16, wxID_DUPLICATE,             false, false, false, 0},
	{17, wxID_DELETE,                false, false, false, WXK_DELETE},
	{18, wxID_SELECTALL,              true, false, false, 'A'},
	{19, ID_COPY_TITLES_LIST,        false, false, false, 0},
	{20, ID_REORDER,                 false, false, false, 0},
	{21, wxID_FIND,                   true, false, false, 'F'},
	{22, ID_EDIT_FIND_NEXT,          false, false, false, WXK_F3},
	{23, ID_EDIT_FIND_PREVIOUS,      false, false,  true, WXK_F3},
	{24, ID_EDIT_CONFIGURATION,      false, false, false, 0},
	{25, ID_AGILITY_EDIT_DOG,        false, false, false, 0},
	{26, ID_AGILITY_NEW_DOG,         false, false, false, 0},
	{27, ID_AGILITY_DELETE_DOG,      false, false, false, 0},
	{28, ID_AGILITY_EDIT_TITLE,      false, false, false, 0},
	{29, ID_AGILITY_NEW_TITLE,       false, false, false, 0},
	{30, ID_AGILITY_DELETE_TITLE,    false, false, false, 0},
	{31, ID_AGILITY_EDIT_TRIAL,      false, false, false, 0},
	{32, ID_AGILITY_NEW_TRIAL,       false, false, false, 0},
	{33, ID_AGILITY_DELETE_TRIAL,    false, false, false, 0},
	{34, ID_AGILITY_PRINT_TRIAL,     false, false, false, 0},
	{35, ID_AGILITY_EDIT_RUN,        false, false, false, 0},
	{36, ID_AGILITY_NEW_RUN,          true, false, false, 'R'},
	{37, ID_AGILITY_DELETE_RUN,      false, false, false, 0},
	{38, ID_AGILITY_PRINT_RUNS,      false, false, false, 0},
	{39, ID_AGILITY_EDIT_CALENDAR,   false, false, false, 0},
	{40, ID_AGILITY_NEW_CALENDAR,    false, false, false, 0},
	{41, ID_AGILITY_DELETE_CALENDAR, false, false, false, 0},
	{42, ID_AGILITY_UPDATE_CALENDAR, false, false, false, 0},
	{43, ID_AGILITY_EXPORT_CALENDAR, false, false, false, 0},
	{44, ID_AGILITY_CREATEENTRY_CALENDAR, false, false, false, 0},
	{45, ID_AGILITY_EDIT_TRAINING,   false, false, false, 0},
	{46, ID_AGILITY_NEW_TRAINING,    false, false, false, 0},
	{47, ID_AGILITY_DELETE_TRAINING, false, false, false, 0},
	{48, ID_NOTES_CLUBS,             false, false, false, 0},
	{49, ID_NOTES_JUDGES,             true, false, false, 'J'},
	{50, ID_NOTES_LOCATIONS,         false, false, false, 0},
	{51, ID_NOTES_SEARCH,            false, false, false, 0},
	{52, wxID_PREFERENCES,           false, false, false, WXK_F7},
	{53, ID_VIEW_CUSTOMIZE,          false, false, false, 0},
	{54, ID_VIEW_CUSTOMIZE_ACCEL,    false, false, false, 0},
	{55, ID_UNSORT,                  false, false, false, 0},
	{56, ID_VIEW_SORTRUNS,           false, false, false, 0},
	{57, ID_VIEW_RUNS_BY_TRIAL,      false, false, false, 0},
	{58, ID_VIEW_TABLE_IN_YPS,       false, false, false, 0},
	{59, ID_VIEW_RUNTIME_IN_OPS,     false, false, false, 0},
	{60, ID_VIEW_POINTS_VIEW_SORT,   false, false, false, 0},
	{61, ID_VIEW_HIDDEN,             false, false, false, 0},
	{62, ID_VIEW_LIFETIME_EVENTS,    false, false, false, 0},
	{63, ID_EXPAND,                  false, false, false, 0},
	{64, ID_COLLAPSE,                false, false, false, 0},
	{65, ID_EXPAND_ALL,              false, false, false, 0},
	{66, ID_COLLAPSE_ALL,            false, false, false, 0},
#if wxUSE_NOTEBOOK
	{67, ID_BOOK_NOTEBOOK,            true, false, false, '1'},
#endif
#if wxUSE_LISTBOOK
	{68, ID_BOOK_LISTBOOK,            true, false, false, '2'},
#endif
#if wxUSE_CHOICEBOOK
	{69, ID_BOOK_CHOICEBOOK,          true, false, false, '3'},
#endif
#if wxUSE_TREEBOOK
	{70, ID_BOOK_TREEBOOK,            true, false, false, '4'},
#endif
#if wxUSE_TOOLBOOK
	{71, ID_BOOK_TOOLBOOK,            true, false, false, '5'},
#endif
	{72, ID_ORIENT_TOP,              false, false, false, 0},
	{73, ID_ORIENT_BOTTOM,           false, false, false, 0},
	{74, ID_ORIENT_LEFT,             false, false, false, 0},
	{75, ID_ORIENT_RIGHT,            false, false, false, 0},
	{76, ID_WINDOW_SPLIT,            false, false, false, 0},
	{77, ID_NEXT_PANE,               false, false, false, WXK_F6},
	{78, ID_PREV_PANE,               false, false,  true, WXK_F6},
#ifdef _DEBUG
	{79, ID_HELP_DEBUG,              false, false, false, 0},
#endif
	{80, ID_HELP_UPDATE,             false, false, false, 0},
	{81, ID_HELP_SYSINFO,            false, false, false, 0},
	{82, wxID_ABOUT,                 false, false, false, 0},
	{83, ID_DETAILS,                 false, false, false, 0},
};

static const std::vector<int> sc_toolbarItems =
{
	wxID_NEW,
	wxID_OPEN,
	wxID_SAVE,
	0,
	ID_AGILITY_NEW_DOG,
	ID_AGILITY_NEW_TITLE,
	ID_AGILITY_NEW_TRIAL,
	ID_AGILITY_NEW_RUN,
	ID_AGILITY_NEW_CALENDAR,
	ID_AGILITY_NEW_TRAINING,
	0,
	wxID_CUT,
	wxID_COPY,
	wxID_PASTE,
	wxID_DELETE,
	0,
	wxID_PREVIEW,
	wxID_PRINT,
	0,
	wxID_ABOUT
};


std::unique_ptr<CMenuHelper> CAgilityBookApp::CreateMenus()
{
	std::unordered_map<int, std::wstring> menuIds;
	menuIds[IdMenuNone] = wxGetTranslation(arbT("IDS_MENU_MAIN"));
	menuIds[IdMenuDog] = wxGetTranslation(arbT("IDS_MENU_DOG"));
	menuIds[IdMenuTrial] = wxGetTranslation(arbT("IDS_MENU_TRIAL"));
	menuIds[IdMenuRun] = wxGetTranslation(arbT("IDS_MENU_RUN"));
	menuIds[IdMenuPoints] = wxGetTranslation(arbT("IDS_MENU_POINTS"));
	menuIds[IdMenuCalendar] = wxGetTranslation(arbT("IDS_MENU_CALENDAR"));
	menuIds[IdMenuTraining] = wxGetTranslation(arbT("IDS_MENU_TRAINING"));
	return std::make_unique<CMenuHelper>(sc_items, sc_accels, sc_toolbarItems, menuIds, true);
}
