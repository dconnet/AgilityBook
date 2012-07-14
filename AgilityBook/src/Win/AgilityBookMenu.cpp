/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Wrapper for menus to ease i18n and menu ids
 * @author David Connet
 *
 * Important: For accelerators to work, they must be in the main menu.
 * If they are only in a context menu, the accelerator will not work.
 *
 * Revision History
 * @li 2012-07-13 DRC Reworked menu creation to use a generic reusable helper.
 * @li 2012-07-08 DRC Put images into menu.
 * @li 2012-07-04 DRC Add option to use run time or opening time in gamble OPS.
 * @li 2009-09-19 DRC Fix IdMenuRecentFiles on language switch.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-09-09 DRC Fix Ctrl+R. See note above.
 * @li 2008-12-14 DRC Created
 */

#include "stdafx.h"
#include "AgilityBookMenu.h"

#include "MainFrm.h"
#include <wx/config.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

#include "res/toolbarNew.xpm"
#include "res/toolbarOpen.xpm"
#include "res/toolbarSave.xpm"
#include "res/toolbarDog.xpm"
#include "res/toolbarTitle.xpm"
#include "res/toolbarTrial.xpm"
#include "res/toolbarRun.xpm"
#include "res/toolbarCalendar.xpm"
#include "res/toolbarTraining.xpm"
#include "res/toolbarCut.xpm"
#include "res/toolbarCopy.xpm"
#include "res/toolbarPaste.xpm"
#include "res/toolbarPreview.xpm"
#include "res/toolbarPrint.xpm"
#include "res/toolbarAbout.xpm"


static const CMenuHelper::ItemData sc_Items[] =
{
	{MENU_ITEM, 0,                        wxITEM_NORMAL, 0, NULL, arbT("MenuFile"), NULL, NULL},
	{MENU_ITEM, wxID_NEW,                 wxITEM_NORMAL, 1, arbT("MenuFileNew"), arbT("MenuFileNew"), arbT("DescFileNew"), toolbarNew_xpm},
	{MENU_ITEM, wxID_OPEN,                wxITEM_NORMAL, 1, arbT("MenuFileOpen"), arbT("MenuFileOpen"), arbT("DescFileOpen"), toolbarOpen_xpm},
	{MENU_ITEM, wxID_SAVE,                wxITEM_NORMAL, 1, arbT("MenuFileOpen"), arbT("MenuFileSave"), arbT("DescFileSave"), toolbarSave_xpm},
	{MENU_ITEM, wxID_SAVEAS,              wxITEM_NORMAL, 1, NULL, arbT("MenuFileSaveAs"), arbT("DescFileSaveAs"), NULL},
	{MENU_SEP,  0,                        wxITEM_NORMAL, 1, NULL, NULL, NULL, NULL},
	{MENU_ITEM, ID_FILE_LANGUAGE_CHOOSE,  wxITEM_NORMAL, 1, NULL, arbT("MenuFileLanguage"), arbT("DescFileLanguage"), NULL},
	{MENU_SEP,  0,                        wxITEM_NORMAL, 1, NULL, NULL, NULL, NULL},
	{MENU_ITEM, ID_FILE_EXPORT_WIZARD,    wxITEM_NORMAL, 1, NULL, arbT("MenuFileImportExport"), arbT("DescFileImportExport"), NULL},
	{MENU_ITEM, ID_FILE_LINKED,           wxITEM_NORMAL, 1, NULL, arbT("MenuFileLinkedFiles"), arbT("DescFileLinkedFiles"), NULL},
	{MENU_SEP,  0,                        wxITEM_NORMAL, 1, NULL, NULL, NULL, NULL},
	{MENU_ITEM, wxID_PRINT,               wxITEM_NORMAL, 1, arbT("MenuFilePrint"), arbT("MenuFilePrint"), arbT("DescFilePrint"), toolbarPrint_xpm},
	{MENU_ITEM, ID_FILE_PRINT_BLANK_RUNS, wxITEM_NORMAL, 1, NULL, arbT("MenuFilePrintBlank"), arbT("DescFilePrintBlank"), NULL},
	{MENU_ITEM, wxID_PREVIEW,             wxITEM_NORMAL, 1, arbT("MenuFilePrintPreview"), arbT("MenuFilePrintPreview"), arbT("DescFilePrintPreview"), toolbarPreview_xpm},
	{MENU_MRU,  0,                        wxITEM_NORMAL, 1, NULL, arbT("MenuFileRecent"), NULL, NULL},
	{MENU_SEP,  0,                        wxITEM_NORMAL, 1, NULL, NULL, NULL, NULL},
	{MENU_ITEM, wxID_EXIT,                wxITEM_NORMAL, 1, NULL, arbT("MenuFileExit"), arbT("DescFileExit"), NULL},

	{MENU_ITEM, 0,                        wxITEM_NORMAL, 0, NULL, arbT("MenuEdit"), NULL, NULL},
	{MENU_ITEM, wxID_DUPLICATE,           wxITEM_NORMAL, 1, NULL, arbT("MenuEditDuplicate"), arbT("DescEditDuplicate"), NULL},
	{MENU_ITEM, wxID_CUT,                 wxITEM_NORMAL, 1, arbT("MenuEditCut"), arbT("MenuEditCut"), arbT("DescEditCut"), toolbarCut_xpm},
	{MENU_ITEM, wxID_COPY,                wxITEM_NORMAL, 1, arbT("MenuEditCopy"), arbT("MenuEditCopy"), arbT("DescEditCopy"), toolbarCopy_xpm},
	{MENU_ITEM, wxID_PASTE,               wxITEM_NORMAL, 1, arbT("MenuEditPaste"), arbT("MenuEditPaste"), arbT("DescEditPaste"), toolbarPaste_xpm},
	{MENU_SEP,  0,                        wxITEM_NORMAL, 1, NULL, NULL, NULL, NULL},
	{MENU_ITEM, wxID_SELECTALL,           wxITEM_NORMAL, 1, NULL, arbT("MenuEditSelectAll"), arbT("DescEditSelectAll"), NULL},
	{MENU_SEP,  0,                        wxITEM_NORMAL, 1, NULL, NULL, NULL, NULL},
	{MENU_ITEM, ID_COPY_TITLES_LIST,      wxITEM_NORMAL, 1, NULL, arbT("MenuEditCopyTitles"), arbT("DescEditCopyTitles"), NULL},
	{MENU_ITEM, ID_REORDER,               wxITEM_NORMAL, 1, NULL, arbT("MenuEditReorder"), arbT("DescEditReorder"), NULL},
	{MENU_SEP,  0,                        wxITEM_NORMAL, 1, NULL, NULL, NULL, NULL},
	{MENU_ITEM, wxID_FIND,                wxITEM_NORMAL, 1, NULL, arbT("MenuEditFind"), arbT("DescEditFind"), NULL},
	{MENU_ITEM, ID_EDIT_FIND_NEXT,        wxITEM_NORMAL, 1, NULL, arbT("MenuEditFindNext"), arbT("DescEditFindNext"), NULL},
	{MENU_ITEM, ID_EDIT_FIND_PREVIOUS,    wxITEM_NORMAL, 1, NULL, arbT("MenuEditFindPrev"), arbT("DescEditFindPrev"), NULL},
	{MENU_SEP,  0,                        wxITEM_NORMAL, 1, NULL, NULL, NULL, NULL},
	{MENU_ITEM, ID_EDIT_CONFIGURATION,    wxITEM_NORMAL, 1, NULL, arbT("MenuEditConfig"), arbT("DescEditConfig"), NULL},

	{MENU_ITEM, 0,                               wxITEM_NORMAL, 0, NULL, arbT("MenuAgility"), NULL, NULL},
	{MENU_ITEM, 0,                               wxITEM_NORMAL, 1, NULL, arbT("MenuDog"), NULL, toolbarDog_xpm},
	{MENU_ITEM, ID_AGILITY_EDIT_DOG,             wxITEM_NORMAL, 2, NULL, arbT("MenuDogProperties"), arbT("DescDogProperties"), toolbarDog_xpm},
	{MENU_SEP,  0,                               wxITEM_NORMAL, 2, NULL, NULL, NULL, NULL},
	{MENU_ITEM, ID_AGILITY_NEW_DOG,              wxITEM_NORMAL, 2, arbT("MenuNew"), arbT("MenuNew"), arbT("DescDogProperties"), toolbarDog_xpm},
	{MENU_ITEM, ID_AGILITY_DELETE_DOG,           wxITEM_NORMAL, 2, NULL, arbT("MenuDogDelete"), arbT("DescDogDelete"), NULL},
	{MENU_ITEM, 0,                               wxITEM_NORMAL, 1, NULL, arbT("MenuTitle"), NULL, toolbarTitle_xpm},
	{MENU_ITEM, ID_AGILITY_EDIT_TITLE,           wxITEM_NORMAL, 2, NULL, arbT("MenuTitleProperties"), arbT("DescTitleProperties"), toolbarTitle_xpm},
	{MENU_SEP,  0,                               wxITEM_NORMAL, 2, NULL, NULL, NULL, NULL},
	{MENU_ITEM, ID_AGILITY_NEW_TITLE,            wxITEM_NORMAL, 2, arbT("MenuNew"), arbT("MenuNew"), arbT("DescTitleNew"), toolbarTitle_xpm},
	{MENU_ITEM, ID_AGILITY_DELETE_TITLE,         wxITEM_NORMAL, 2, NULL, arbT("MenuTitleDelete"), arbT("DescTitleDelete"), NULL},
	{MENU_ITEM, 0,                               wxITEM_NORMAL, 1, NULL, arbT("MenuTrial"), NULL, toolbarTrial_xpm},
	{MENU_ITEM, ID_AGILITY_EDIT_TRIAL,           wxITEM_NORMAL, 2, NULL, arbT("MenuTrialProperties"), arbT("DescTrialProperties"), toolbarTrial_xpm},
	{MENU_SEP,  0,                               wxITEM_NORMAL, 2, NULL, NULL, NULL, NULL},
	{MENU_ITEM, ID_AGILITY_NEW_TRIAL,            wxITEM_NORMAL, 2, arbT("MenuNew"), arbT("MenuNew"), arbT("DescTrialNew"), toolbarTrial_xpm},
	{MENU_ITEM, ID_AGILITY_DELETE_TRIAL,         wxITEM_NORMAL, 2, NULL, arbT("MenuTrialDelete"), arbT("DescTrialDelete"), NULL},
	{MENU_SEP,  0,                               wxITEM_NORMAL, 2, NULL, NULL, NULL, NULL},
	{MENU_ITEM, ID_AGILITY_PRINT_TRIAL,          wxITEM_NORMAL, 2, NULL, arbT("MenuTrialPrintRuns"), arbT("DescTrialPrintRuns"), NULL},
	{MENU_ITEM, 0,                               wxITEM_NORMAL, 1, NULL, arbT("MenuRun"), NULL, toolbarRun_xpm},
	{MENU_ITEM, ID_AGILITY_EDIT_RUN,             wxITEM_NORMAL, 2, NULL, arbT("MenuRunProperties"), arbT("DescRunProperties"), toolbarRun_xpm},
	{MENU_SEP,  0,                               wxITEM_NORMAL, 2, NULL, NULL, NULL, NULL},
	{MENU_ITEM, ID_AGILITY_NEW_RUN,              wxITEM_NORMAL, 2, arbT("MenuNewRun"), arbT("MenuNewRun"), arbT("DescRunNew"), toolbarRun_xpm},
	{MENU_ITEM, ID_AGILITY_DELETE_RUN,           wxITEM_NORMAL, 2, NULL, arbT("MenuRunDelete"), arbT("DescRunDelete"), NULL},
	{MENU_SEP,  0,                               wxITEM_NORMAL, 2, NULL, NULL, NULL, NULL},
	{MENU_ITEM, ID_AGILITY_PRINT_RUNS,           wxITEM_NORMAL, 2, NULL, arbT("MenuRunPrintRuns"), arbT("DescRunPrintRuns"), NULL},
	{MENU_ITEM, 0,                               wxITEM_NORMAL, 1, NULL, arbT("MenuCalendar"), NULL, toolbarCalendar_xpm},
	{MENU_ITEM, ID_AGILITY_EDIT_CALENDAR,        wxITEM_NORMAL, 2, NULL, arbT("MenuCalendarProperties"), arbT("DescCalendarProperties"), toolbarCalendar_xpm},
	{MENU_SEP,  0,                               wxITEM_NORMAL, 2, NULL, NULL, NULL, NULL},
	{MENU_ITEM, ID_AGILITY_NEW_CALENDAR,         wxITEM_NORMAL, 2, arbT("MenuNew"), arbT("MenuNew"), arbT("DescCalendarNew"), toolbarCalendar_xpm},
	{MENU_ITEM, ID_AGILITY_DELETE_CALENDAR,      wxITEM_NORMAL, 2, NULL, arbT("MenuCalendarDelete"), arbT("DescCalendarDelete"), NULL},
	{MENU_SEP,  0,                               wxITEM_NORMAL, 2, NULL, NULL, NULL, NULL},
	{MENU_ITEM, ID_AGILITY_UPDATE_CALENDAR,      wxITEM_NORMAL, 2, NULL, arbT("MenuCalendarUpdate"), arbT("DescCalendarUpdate"), NULL},
	{MENU_ITEM, ID_AGILITY_EXPORT_CALENDAR,      wxITEM_NORMAL, 2, NULL, arbT("MenuCalendarExport"), arbT("DescCalendarExport"), NULL},
	{MENU_SEP,  0,                               wxITEM_NORMAL, 2, NULL, NULL, NULL, NULL},
	{MENU_ITEM, ID_AGILITY_CREATEENTRY_CALENDAR, wxITEM_NORMAL, 2, NULL, arbT("MenuCalendarTrialEntry"), arbT("DescCalendarTrialEntry"), NULL},
	{MENU_ITEM, 0,                               wxITEM_NORMAL, 1, NULL, arbT("MenuTraining"), NULL, toolbarTraining_xpm},
	{MENU_ITEM, ID_AGILITY_EDIT_TRAINING,        wxITEM_NORMAL, 2, NULL, arbT("MenuTrainingProperties"), arbT("DescTrainingProperties"), toolbarTraining_xpm},
	{MENU_SEP,  0,                               wxITEM_NORMAL, 2, NULL, NULL, NULL, NULL},
	{MENU_ITEM, ID_AGILITY_NEW_TRAINING,         wxITEM_NORMAL, 2, arbT("MenuNew"), arbT("MenuNew"), arbT("DescTrainingNew"), toolbarTraining_xpm},
	{MENU_ITEM, ID_AGILITY_DELETE_TRAINING,      wxITEM_NORMAL, 2, NULL, arbT("MenuTrainingDelete"), arbT("DescTrainingDelete"), NULL},

	{MENU_ITEM, 0,                        wxITEM_NORMAL, 0, NULL, arbT("MenuNotes"), NULL, NULL},
	{MENU_ITEM, ID_NOTES_CLUBS,           wxITEM_NORMAL, 1, NULL, arbT("MenuNotesClubs"), arbT("DescNotesClubs"), NULL},
	{MENU_ITEM, ID_NOTES_JUDGES,          wxITEM_NORMAL, 1, NULL, arbT("MenuNotesJudges"), arbT("DescNotesJudges"), NULL},
	{MENU_ITEM, ID_NOTES_LOCATIONS,       wxITEM_NORMAL, 1, NULL, arbT("MenuNotesLocations"), arbT("DescNotesLocations"), NULL},
	{MENU_SEP,  0,                        wxITEM_NORMAL, 1, NULL, NULL, NULL, NULL},
	{MENU_ITEM, ID_NOTES_SEARCH,          wxITEM_NORMAL, 1, NULL, arbT("MenuNotesSearch"), arbT("DescNotesSearch"), NULL},

	{MENU_ITEM, 0,                        wxITEM_NORMAL, 0, NULL, arbT("MenuView"), NULL, NULL},
	{MENU_ITEM, wxID_PREFERENCES,         wxITEM_NORMAL, 1, NULL, arbT("MenuViewOptions"), arbT("DescViewOptions"), NULL},
	{MENU_ITEM, ID_VIEW_CUSTOMIZE,        wxITEM_NORMAL, 1, NULL, arbT("MenuViewCustomize"), arbT("DescViewCustomize"), NULL},
	{MENU_SEP,  0,                        wxITEM_NORMAL, 1, NULL, NULL, NULL, NULL},
	{MENU_ITEM, ID_VIEW_SORTRUNS,         wxITEM_CHECK,  1, NULL, arbT("MenuViewNewestTrials"), arbT("DescViewNewestTrials"), NULL},
	{MENU_ITEM, ID_VIEW_RUNS_BY_TRIAL,    wxITEM_CHECK,  1, NULL, arbT("MenuViewRunsByTrial"), arbT("DescViewRunsByTrial"), NULL},
	{MENU_ITEM, ID_VIEW_TABLE_IN_YPS,     wxITEM_CHECK,  1, NULL, arbT("MenuViewIncludeTable"), arbT("DescViewIncludeTable"), NULL},
	{MENU_ITEM, ID_VIEW_RUNTIME_IN_OPS,   wxITEM_CHECK,  1, NULL, arbT("MenuViewIncludeRunTime"), arbT("DescViewIncludeRunTime"), NULL},
	{MENU_SEP,  0,                        wxITEM_NORMAL, 1, NULL, NULL, NULL, NULL},
	{MENU_ITEM, ID_VIEW_POINTS_VIEW_SORT, wxITEM_NORMAL, 1, NULL, arbT("MenuViewSortPoints"), arbT("DescViewSortPoints"), NULL},
	{MENU_ITEM, ID_VIEW_HIDDEN,           wxITEM_CHECK,  1, NULL, arbT("MenuViewHiddenTitles"), arbT("DescViewHiddenTitles"), NULL},
	{MENU_ITEM, ID_VIEW_LIFETIME_EVENTS,  wxITEM_CHECK,  1, NULL, arbT("MenuViewLifePoints"), arbT("DescViewLifePoints"), NULL},
	{MENU_SEP,  0,                        wxITEM_NORMAL, 1, NULL, NULL, NULL, NULL},
	{MENU_ITEM, ID_EXPAND,                wxITEM_NORMAL, 1, NULL, arbT("MenuViewExpand"), arbT("DescViewExpand"), NULL},
	{MENU_ITEM, ID_COLLAPSE,              wxITEM_NORMAL, 1, NULL, arbT("MenuViewCollapse"), arbT("DescViewCollapse"), NULL},
	{MENU_ITEM, ID_EXPAND_ALL,            wxITEM_NORMAL, 1, NULL, arbT("MenuViewExpandAll"), arbT("DescViewExpandAll"), NULL},
	{MENU_ITEM, ID_COLLAPSE_ALL,          wxITEM_NORMAL, 1, NULL, arbT("MenuViewCollapseAll"), arbT("DescViewCollapseAll"), NULL},
	{MENU_SEP,  0,                        wxITEM_NORMAL, 1, NULL, NULL, NULL, NULL},
	{MENU_ITEM, 0,                        wxITEM_NORMAL, 1, NULL, arbT("MenuViewType"), NULL, NULL},
#if wxUSE_NOTEBOOK
	{MENU_ITEM, ID_BOOK_NOTEBOOK,         wxITEM_RADIO,  2, NULL, arbT("MenuViewNotebook"), arbT("DescViewNotebook"), NULL},
#endif
#if wxUSE_LISTBOOK
	{MENU_ITEM, ID_BOOK_LISTBOOK,         wxITEM_RADIO,  2, NULL, arbT("MenuViewListbook"), arbT("DescViewListbook"), NULL},
#endif
#if wxUSE_CHOICEBOOK
	{MENU_ITEM, ID_BOOK_CHOICEBOOK,       wxITEM_RADIO,  2, NULL, arbT("MenuViewChoicebook"), arbT("DescViewChoicebook"), NULL},
#endif
#if wxUSE_TREEBOOK
	{MENU_ITEM, ID_BOOK_TREEBOOK,         wxITEM_RADIO,  2, NULL, arbT("MenuViewTreebook"), arbT("DescViewTreebook"), NULL},
#endif
#if wxUSE_TOOLBOOK
	{MENU_ITEM, ID_BOOK_TOOLBOOK,         wxITEM_RADIO,  2, NULL, arbT("MenuViewToolbook"), arbT("DescViewToolbook"), NULL},
#endif
	{MENU_ITEM, 0,                        wxITEM_NORMAL, 1, NULL, arbT("MenuViewOrientation"), NULL, NULL},
	{MENU_ITEM, ID_ORIENT_TOP,            wxITEM_RADIO,  2, NULL, arbT("MenuViewTop"), arbT("DescViewTop"), NULL},
	{MENU_ITEM, ID_ORIENT_BOTTOM,         wxITEM_RADIO,  2, NULL, arbT("MenuViewBottom"), arbT("DescViewBottom"), NULL},
	{MENU_ITEM, ID_ORIENT_LEFT,           wxITEM_RADIO,  2, NULL, arbT("MenuViewLeft"), arbT("DescViewLeft"), NULL},
	{MENU_ITEM, ID_ORIENT_RIGHT,          wxITEM_RADIO,  2, NULL, arbT("MenuViewRight"), arbT("DescViewRight"), NULL},

	{MENU_ITEM, 0,                        wxITEM_NORMAL, 0, NULL, arbT("MenuWindow"), NULL, NULL},
	{MENU_ITEM, ID_WINDOW_SPLIT,          wxITEM_NORMAL, 1, NULL, arbT("MenuWindowSplitter"), arbT("DescWindowSplitter"), NULL},
	{MENU_SEP,  0,                        wxITEM_NORMAL, 1, NULL, NULL, NULL, NULL},
	{MENU_ITEM, ID_NEXT_PANE,             wxITEM_NORMAL, 1, NULL, arbT("MenuWindowNext"), arbT("DescWindowNext"), NULL},
	{MENU_ITEM, ID_PREV_PANE,             wxITEM_NORMAL, 1, NULL, arbT("MenuWindowPrev"), arbT("DescWindowPrev"), NULL},

	{MENU_ITEM, 0,                        wxITEM_NORMAL, 0, NULL, arbT("MenuHelp"), NULL, NULL},
	{MENU_ITEM, ID_HELP_UPDATE,           wxITEM_NORMAL, 1, NULL, arbT("MenuHelpUpdates"), arbT("DescHelpUpdates"), toolbarNew_xpm},
	{MENU_ITEM, ID_HELP_SYSINFO,          wxITEM_NORMAL, 1, NULL, arbT("MenuHelpSysInfo"), arbT("DescHelpSysInfo"), NULL},
	{MENU_SEP,  0,                        wxITEM_NORMAL, 1, NULL, NULL, NULL, NULL},
	{MENU_HELP, wxID_ABOUT,               wxITEM_NORMAL, 1, arbT("MenuHelpAbout"), arbT("MenuHelpAbout"), arbT("DescHelpAbout"), toolbarAbout_xpm},
};
static const size_t sc_ItemsCount = sizeof(sc_Items) / sizeof(sc_Items[0]);

static const int sc_toolbarItems[] =
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
	0,
	wxID_PREVIEW,
	wxID_PRINT,
	0,
	wxID_ABOUT
};
static const size_t sc_toolbarItemsCount = sizeof(sc_toolbarItems) / sizeof(sc_toolbarItems[0]);


void CreateMainMenu(
		CMenuHelper& menus,
		wxFrame* pFrame,
		wxMenu* pRecent)
{
	menus.CreateMenu(pFrame, sc_Items, sc_ItemsCount, sc_toolbarItems, sc_toolbarItemsCount, true, pRecent);
}


/////////////////////////////////////////////////////////////////////////////
// Popup menus are created each time, so we don't need the effort of above
// to fix the text on language change.

// Create New submenu with dog/title/trial/run
static wxMenu* CreateNewMenu(bool incRun)
{
	wxMenu* menu = new wxMenu;
	CMenuHelper::DoMenuItem(menu, ID_AGILITY_NEW_DOG, _("MenuDogNew"), _("DescDogNew"), toolbarDog_xpm);
	CMenuHelper::DoMenuItem(menu, ID_AGILITY_NEW_TITLE, _("MenuTitleNew"), _("DescTitleNew"), toolbarTitle_xpm);
	menu->AppendSeparator();
	CMenuHelper::DoMenuItem(menu, ID_AGILITY_NEW_TRIAL, _("MenuTrialNew"), _("DescTrialNew"), toolbarTrial_xpm);
	if (incRun)
	{
		CMenuHelper::DoMenuItem(menu, ID_AGILITY_NEW_RUN, _("MenuRunNew"), _("DescRunNew"), toolbarRun_xpm);
	}
	return menu;
}


wxMenu* CreatePopup(MenuIdentityPopup idMenu)
{
	wxMenu* menu = new wxMenu;

	switch (idMenu)
	{
	default:
		// all items in 'enum MenuIdentityPopup' should be listed
		assert(0);
		break;

	case IdMenuNone:
		break;

	case IdMenuDog:
		CMenuHelper::DoMenuItem(menu, ID_AGILITY_EDIT_DOG, _("MenuDogProperties"), _("DescDogProperties"), toolbarDog_xpm);
		CMenuHelper::DoMenuItem(menu, wxID_PREFERENCES, _("MenuViewOptions"), _("DescViewOptions"));
		menu->AppendSeparator();
		menu->Append(wxID_ANY, _("MenuNew"), CreateNewMenu(false));
		CMenuHelper::DoMenuItem(menu, wxID_DUPLICATE, _("MenuEditDuplicate"), _("DescEditDuplicate"));
		CMenuHelper::DoMenuItem(menu, wxID_CUT, _("MenuEditCut"), _("DescEditCut"), toolbarCut_xpm);
		CMenuHelper::DoMenuItem(menu, wxID_COPY, _("MenuEditCopy"), _("DescEditCopy"), toolbarCopy_xpm);
		CMenuHelper::DoMenuItem(menu, wxID_PASTE, _("MenuEditPaste"), _("DescEditPaste"), toolbarPaste_xpm);
		CMenuHelper::DoMenuItem(menu, ID_AGILITY_DELETE_DOG, _("MenuDogDelete"), _("DescDogDelete"));
		menu->AppendSeparator();
		CMenuHelper::DoMenuItem(menu, ID_REORDER, _("MenuEditReorder"), _("DescEditReorder"));
		menu->AppendSeparator();
		CMenuHelper::DoMenuItem(menu, ID_EXPAND, _("MenuViewExpand"), _("DescViewExpand"));
		CMenuHelper::DoMenuItem(menu, ID_COLLAPSE, _("MenuViewCollapse"), _("DescViewCollapse"));
		CMenuHelper::DoMenuItem(menu, ID_EXPAND_ALL, _("MenuViewExpandAll"), _("DescViewExpandAll"));
		CMenuHelper::DoMenuItem(menu, ID_COLLAPSE_ALL, _("MenuViewCollapseAll"), _("DescViewCollapseAll"));
		break;

	case IdMenuTrial:
		CMenuHelper::DoMenuItem(menu, ID_AGILITY_EDIT_TRIAL, _("MenuTrialProperties"), _("DescTrialProperties"), toolbarTrial_xpm);
		CMenuHelper::DoMenuItem(menu, wxID_PREFERENCES, _("MenuViewOptions"), _("DescViewOptions"));
		menu->AppendSeparator();
		menu->Append(wxID_ANY, _("MenuNew"), CreateNewMenu(true));
		CMenuHelper::DoMenuItem(menu, wxID_DUPLICATE, _("MenuEditDuplicate"), _("DescEditDuplicate"));
		CMenuHelper::DoMenuItem(menu, wxID_CUT, _("MenuEditCut"), _("DescEditCut"), toolbarCut_xpm);
		CMenuHelper::DoMenuItem(menu, wxID_COPY, _("MenuEditCopy"), _("DescEditCopy"), toolbarCopy_xpm);
		CMenuHelper::DoMenuItem(menu, wxID_PASTE, _("MenuEditPaste"), _("DescEditPaste"), toolbarPaste_xpm);
		CMenuHelper::DoMenuItem(menu, ID_AGILITY_DELETE_TRIAL, _("MenuTrialDelete"), _("DescTrialDelete"));
		menu->AppendSeparator();
		CMenuHelper::DoMenuItem(menu, ID_REORDER, _("MenuEditReorder"), _("DescEditReorder"));
		menu->AppendSeparator();
		CMenuHelper::DoMenuItem(menu, ID_EXPAND, _("MenuViewExpand"), _("DescViewExpand"));
		CMenuHelper::DoMenuItem(menu, ID_COLLAPSE, _("MenuViewCollapse"), _("DescViewCollapse"));
		menu->AppendSeparator();
		CMenuHelper::DoMenuItem(menu, ID_AGILITY_PRINT_TRIAL, _("MenuTrialPrintRuns"), _("DescTrialPrintRuns"));
		break;

	case IdMenuRun:
		CMenuHelper::DoMenuItem(menu, ID_AGILITY_EDIT_RUN, _("MenuRunProperties"), _("DescRunProperties"), toolbarRun_xpm);
		CMenuHelper::DoMenuItem(menu, wxID_PREFERENCES, _("MenuViewOptions"), _("DescViewOptions"));
		menu->AppendSeparator();
		menu->Append(wxID_ANY, _("MenuNew"), CreateNewMenu(true));
		CMenuHelper::DoMenuItem(menu, wxID_DUPLICATE, _("MenuEditDuplicate"), _("DescEditDuplicate"));
		CMenuHelper::DoMenuItem(menu, wxID_CUT, _("MenuEditCut"), _("DescEditCut"), toolbarCut_xpm);
		CMenuHelper::DoMenuItem(menu, wxID_COPY, _("MenuEditCopy"), _("DescEditCopy"), toolbarCopy_xpm);
		CMenuHelper::DoMenuItem(menu, wxID_PASTE, _("MenuEditPaste"), _("DescEditPaste"), toolbarPaste_xpm);
		CMenuHelper::DoMenuItem(menu, ID_AGILITY_DELETE_RUN, _("MenuRunDelete"), _("DescRunDelete"));
		menu->AppendSeparator();
		CMenuHelper::DoMenuItem(menu, ID_REORDER, _("MenuEditReorder"), _("DescEditReorder"));
		menu->AppendSeparator();
		CMenuHelper::DoMenuItem(menu, ID_AGILITY_PRINT_RUNS, _("MenuRunPrintRuns"), _("DescRunPrintRuns"));
		break;

	case IdMenuPoints:
		CMenuHelper::DoMenuItem(menu, ID_DETAILS, _("MenuDetails"), _("DescDetails"));
		CMenuHelper::DoMenuItem(menu, wxID_PREFERENCES, _("MenuViewOptions"), _("DescViewOptions"));
		menu->AppendSeparator();
		CMenuHelper::DoMenuItem(menu, wxID_COPY, _("MenuEditCopySelection"), _("DescEditCopy"), toolbarCopy_xpm);
		CMenuHelper::DoMenuItem(menu, ID_COPY_TITLES_LIST, _("MenuEditCopyTitles"), _("DescEditCopyTitles"));
		menu->AppendSeparator();
		CMenuHelper::DoMenuItem(menu, wxID_SELECTALL, _("MenuEditSelectAll"), _("DescEditSelectAll"));
		break;

	case IdMenuCalendar:
		CMenuHelper::DoMenuItem(menu, ID_AGILITY_EDIT_CALENDAR, _("MenuCalendarProperties"), _("DescCalendarProperties"), toolbarCalendar_xpm);
		CMenuHelper::DoMenuItem(menu, wxID_PREFERENCES, _("MenuViewOptions"), _("DescViewOptions"));
		menu->AppendSeparator();
		CMenuHelper::DoMenuItem(menu, ID_AGILITY_NEW_CALENDAR, _("MenuNew"), _("DescCalendarNew"), toolbarCalendar_xpm);
		CMenuHelper::DoMenuItem(menu, wxID_DUPLICATE, _("MenuEditDuplicate"), _("DescEditDuplicate"));
		CMenuHelper::DoMenuItem(menu, wxID_CUT, _("MenuEditCut"), _("DescEditCut"), toolbarCut_xpm);
		CMenuHelper::DoMenuItem(menu, wxID_COPY, _("MenuEditCopy"), _("DescEditCopy"), toolbarCopy_xpm);
		CMenuHelper::DoMenuItem(menu, wxID_PASTE, _("MenuEditPaste"), _("DescEditPaste"), toolbarPaste_xpm);
		CMenuHelper::DoMenuItem(menu, ID_AGILITY_DELETE_CALENDAR, _("MenuCalendarDelete"), _("DescCalendarDelete"));
		menu->AppendSeparator();
		CMenuHelper::DoMenuItem(menu, ID_AGILITY_UPDATE_CALENDAR, _("MenuCalendarUpdate"), _("DescCalendarUpdate"));
		CMenuHelper::DoMenuItem(menu, ID_AGILITY_EXPORT_CALENDAR, _("MenuCalendarExport"), _("DescCalendarExport"));
		menu->AppendSeparator();
		CMenuHelper::DoMenuItem(menu, ID_AGILITY_CREATEENTRY_CALENDAR, _("MenuCalendarTrialEntry"), _("DescCalendarTrialEntry"));
		break;

	case IdMenuTraining:
		CMenuHelper::DoMenuItem(menu, ID_AGILITY_EDIT_TRAINING, _("MenuTrainingProperties"), _("DescTrainingProperties"), toolbarTraining_xpm);
		CMenuHelper::DoMenuItem(menu, wxID_PREFERENCES, _("MenuViewOptions"), _("DescViewOptions"));
		menu->AppendSeparator();
		CMenuHelper::DoMenuItem(menu, ID_AGILITY_NEW_TRAINING, _("MenuNew"), _("DescTrainingNew"), toolbarTraining_xpm);
		CMenuHelper::DoMenuItem(menu, wxID_DUPLICATE, _("MenuEditDuplicate"), _("DescEditDuplicate"));
		CMenuHelper::DoMenuItem(menu, wxID_CUT, _("MenuEditCut"), _("DescEditCut"), toolbarCut_xpm);
		CMenuHelper::DoMenuItem(menu, wxID_COPY, _("MenuEditCopy"), _("DescEditCopy"), toolbarCopy_xpm);
		CMenuHelper::DoMenuItem(menu, wxID_PASTE, _("MenuEditPaste"), _("DescEditPaste"), toolbarPaste_xpm);
		CMenuHelper::DoMenuItem(menu, ID_AGILITY_DELETE_TRAINING, _("MenuTrainingDelete"), _("DescTrainingDelete"));
		break;
	}

	return menu;
}
