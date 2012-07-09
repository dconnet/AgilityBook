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


static const struct
{
	int id;
	wchar_t const* label;
	wchar_t const* shortHelp;
	const char** bitmap;
} sc_Toolbar[] =
{
	{wxID_NEW, arbT("MenuFileNew"), arbT("DescFileNew"), toolbarNew_xpm},
	{wxID_OPEN, arbT("MenuFileOpen"), arbT("DescFileOpen"), toolbarOpen_xpm},
	{wxID_SAVE, arbT("MenuFileSave"), arbT("DescFileSave"), toolbarSave_xpm},
	{0, NULL, NULL, NULL},
	{ID_AGILITY_NEW_DOG, arbT("MenuNew"), arbT("DescDogNew"), toolbarDog_xpm},
	{ID_AGILITY_NEW_TITLE, arbT("MenuNew"), arbT("DescTitleNew"), toolbarTitle_xpm},
	{ID_AGILITY_NEW_TRIAL, arbT("MenuNew"), arbT("DescTrialNew"), toolbarTrial_xpm},
	{ID_AGILITY_NEW_RUN, arbT("MenuNew"), arbT("DescRunNew"), toolbarRun_xpm},
	{ID_AGILITY_NEW_CALENDAR, arbT("MenuNew"), arbT("DescCalendarNew"), toolbarCalendar_xpm},
	{ID_AGILITY_NEW_TRAINING, arbT("MenuNew"), arbT("DescTrainingNew"), toolbarTraining_xpm},
	{0, NULL, NULL, NULL},
	{wxID_CUT, arbT("MenuEditCut"), arbT("DescEditCut"), toolbarCut_xpm},
	{wxID_COPY, arbT("MenuEditCopy"), arbT("DescEditCopy"), toolbarCopy_xpm},
	{wxID_PASTE, arbT("MenuEditPaste"), arbT("DescEditPaste"), toolbarPaste_xpm},
	{0, NULL, NULL, NULL},
	{wxID_PREVIEW, arbT("MenuFilePrintPreview"), arbT("DescFilePrintPreview"), toolbarPreview_xpm},
	{wxID_PRINT, arbT("MenuFilePrint"), arbT("DescFilePrint"), toolbarPrint_xpm},
	{0, NULL, NULL, NULL},
	{wxID_ABOUT, arbT("MenuHelpAbout"), arbT("DescHelpAbout"), toolbarAbout_xpm},
};
static const int sc_ToolbarSize = sizeof(sc_Toolbar) / sizeof(sc_Toolbar[0]);


CAgilityBookMenu::CAgilityBookMenu()
	: m_Frame(NULL)
	, m_MenuBar(NULL)
	, m_MenuFile()
	, m_MenuEdit()
	, m_MenuAgility()
	, m_MenuNotes()
	, m_MenuView()
	, m_MenuWindow()
	, m_MenuHelp()
{
}


CAgilityBookMenu::~CAgilityBookMenu()
{
}


void CAgilityBookMenu::CreateMenu(wxDocParentFrame* pFrame, wxDocManager* manager)
{
	assert(!m_MenuBar);
	m_Frame = pFrame;

#ifdef __WXMAC__
	wxApp::s_macHelpMenuTitleName = _("MenuHelp");
#endif

	m_MenuBar = new wxMenuBar();
	m_MenuFile.Create(IdMenuFile);
	m_MenuFileRecent.Create(IdMenuRecentFiles);
	m_MenuEdit.Create(IdMenuEdit);
	m_MenuAgility.Create(IdMenuAgility);
	m_MenuNotes.Create(IdMenuNotes);
	m_MenuView.Create(IdMenuView);
	m_MenuWindow.Create(IdMenuWindow);
	m_MenuHelp.Create(IdMenuHelp);
	m_MenuAgilityDog.Create(IdMenuAgilityDog);
	m_MenuAgilityTitle.Create(IdMenuAgilityTitle);
	m_MenuAgilityTrial.Create(IdMenuAgilityTrial);
	m_MenuAgilityRun.Create(IdMenuAgilityRun);
	m_MenuAgilityCalendar.Create(IdMenuAgilityCalendar);
	m_MenuAgilityTraining.Create(IdMenuAgilityTraining);
	m_MenuViewType.Create(IdMenuViewType);
	m_MenuViewOrientation.Create(IdMenuViewOrientation);

	CreateMenu(m_MenuFile, true, manager);
	CreateMenu(m_MenuEdit, true);
	CreateMenu(m_MenuAgility, true);
	CreateMenu(m_MenuNotes, true);
	CreateMenu(m_MenuView, true);
	CreateMenu(m_MenuWindow, true);
	CreateMenu(m_MenuHelp, true);

	m_Frame->SetMenuBar(m_MenuBar);

	wxToolBar* toolbar = m_Frame->CreateToolBar(wxTB_FLAT);
	if (toolbar)
	{
		for (int i = 0; i < sc_ToolbarSize; ++i)
		{
			if (0 == sc_Toolbar[i].id)
				toolbar->AddSeparator();
			else
				toolbar->AddTool(sc_Toolbar[i].id, wxGetTranslation(sc_Toolbar[i].label), sc_Toolbar[i].bitmap, wxGetTranslation(sc_Toolbar[i].shortHelp));
		}
		toolbar->Realize();
	}
}


void CAgilityBookMenu::UpdateMenu()
{
	assert(m_MenuBar);
	CreateMenu(m_MenuFile, false);
	CreateMenu(m_MenuEdit, false);
	CreateMenu(m_MenuAgility, false);
	CreateMenu(m_MenuNotes, false);
	CreateMenu(m_MenuView, false);
	CreateMenu(m_MenuWindow, false);
	CreateMenu(m_MenuHelp, false);

	wxToolBar* toolbar = m_Frame->GetToolBar();
	if (toolbar)
	{
		for (int i = 0; i < sc_ToolbarSize; ++i)
		{
			if (0 == sc_Toolbar[i].id)
				continue;
			wxToolBarToolBase* btn = toolbar->FindById(sc_Toolbar[i].id);
			if (btn)
			{
				btn->SetLabel(wxGetTranslation(sc_Toolbar[i].label));
				btn->SetShortHelp(wxGetTranslation(sc_Toolbar[i].shortHelp));
			}
		}
	}
}


static void DoMenuItem(
		wxMenu* menu,
		int id,
		wxString const& label,
		wxString const& desc,
		wxItemKind kind,
		wxMenu* subMenu,
		const char* const* pBitmap)
{
	wxMenuItem* item = new wxMenuItem(menu, id, label, desc, kind, subMenu);
	if (pBitmap && kind == wxITEM_NORMAL)
	{
		wxBitmap bmp(pBitmap);
		item->SetBitmap(bmp);
	}
	menu->Append(item);
}


static void DoMenuItem(
		wxMenu* menu,
		int id,
		wxString const& label,
		wxMenu* subMenu,
		const char* const* pBitmap = NULL)
{
	DoMenuItem(menu, id, label, wxString(), wxITEM_NORMAL, subMenu, pBitmap);
}


static void DoMenuItem(
		wxMenu* menu,
		int id,
		wxString const& label,
		wxString const& desc,
		const char* const* pBitmap = NULL,
		wxItemKind kind = wxITEM_NORMAL)
{
	DoMenuItem(menu, id, label, desc, kind, NULL, pBitmap);
}


static void DoMenu(
		wxMenu* menu,
		bool bCreate,
		int id,
		wxString const& label,
		wxString const& desc,
		const char* const* pBitmap = NULL,
		wxItemKind kind = wxITEM_NORMAL)
{
	if (bCreate)
	{
		DoMenuItem(menu, id, label, desc, pBitmap, kind);
	}
	else
	{
		menu->SetLabel(id, label);
		menu->SetHelpString(id, desc);
	}
}


void CAgilityBookMenu::CreateMenu(
		MenuHandles& handles,
		bool bCreate,
		wxDocManager* manager)
{
	if (bCreate)
		handles.idx = static_cast<int>(m_MenuBar->GetMenuCount());

	wxString title;

	switch (handles.id)
	{
	case IdMenuFile:
		title = _("MenuFile");
		DoMenu(handles.menu, bCreate, wxID_NEW, _("MenuFileNew"), _("DescFileNew"), toolbarNew_xpm);
		DoMenu(handles.menu, bCreate, wxID_OPEN, _("MenuFileOpen"), _("DescFileOpen"), toolbarOpen_xpm);
		DoMenu(handles.menu, bCreate, wxID_SAVE, _("MenuFileSave"), _("DescFileSave"), toolbarSave_xpm);
		DoMenu(handles.menu, bCreate, wxID_SAVEAS, _("MenuFileSaveAs"), _("DescFileSaveAs"));
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_FILE_LANGUAGE_CHOOSE, _("MenuFileLanguage"), _("DescFileLanguage"));
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_FILE_EXPORT_WIZARD, _("MenuFileImportExport"), _("DescFileImportExport"));
		DoMenu(handles.menu, bCreate, ID_FILE_LINKED, _("MenuFileLinkedFiles"), _("DescFileLinkedFiles"));
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, wxID_PRINT, _("MenuFilePrint"), _("DescFilePrint"), toolbarPrint_xpm);
		DoMenu(handles.menu, bCreate, ID_FILE_PRINT_BLANK_RUNS, _("MenuFilePrintBlank"), _("DescFilePrintBlank"));
		DoMenu(handles.menu, bCreate, wxID_PREVIEW, _("MenuFilePrintPreview"), _("DescFilePrintPreview"), toolbarPreview_xpm);
		if (bCreate)
		{
			if (manager)
			{
				handles.menu->AppendSeparator();
				CreateSubMenu(handles.menu, m_MenuFileRecent, NULL, bCreate);
				manager->FileHistoryUseMenu(m_MenuFileRecent.menu);
				manager->FileHistoryAddFilesToMenu();
			}
			handles.menu->AppendSeparator();
		}
		else
		{
			CreateSubMenu(handles.menu, m_MenuFileRecent, NULL, bCreate);
		}
		DoMenu(handles.menu, bCreate, wxID_EXIT, _("MenuFileExit"), _("DescFileExit"));
		break;

	case IdMenuEdit:
		title = _("MenuEdit");
		DoMenu(handles.menu, bCreate, wxID_DUPLICATE, _("MenuEditDuplicate"), _("DescEditDuplicate"));
		DoMenu(handles.menu, bCreate, wxID_CUT, _("MenuEditCut"), _("DescEditCut"), toolbarCut_xpm);
		DoMenu(handles.menu, bCreate, wxID_COPY, _("MenuEditCopy"), _("DescEditCopy"), toolbarCopy_xpm);
		DoMenu(handles.menu, bCreate, wxID_PASTE, _("MenuEditPaste"), _("DescEditPaste"), toolbarPaste_xpm);
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, wxID_SELECTALL, _("MenuEditSelectAll"), _("DescEditSelectAll"));
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_COPY_TITLES_LIST, _("MenuEditCopyTitles"), _("DescEditCopyTitles"));
		DoMenu(handles.menu, bCreate, ID_REORDER, _("MenuEditReorder"), _("DescEditReorder"));
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, wxID_FIND, _("MenuEditFind"), _("DescEditFind"));
		DoMenu(handles.menu, bCreate, ID_EDIT_FIND_NEXT, _("MenuEditFindNext"), _("DescEditFindNext"));
		DoMenu(handles.menu, bCreate, ID_EDIT_FIND_PREVIOUS, _("MenuEditFindPrev"), _("DescEditFindPrev"));
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_EDIT_CONFIGURATION, _("MenuEditConfig"), _("DescEditConfig"));
		break;

	case IdMenuAgility:
		title = _("MenuAgility");
		CreateSubMenu(handles.menu, m_MenuAgilityDog, toolbarDog_xpm, bCreate);
		CreateSubMenu(handles.menu, m_MenuAgilityTitle, toolbarTitle_xpm, bCreate);
		CreateSubMenu(handles.menu, m_MenuAgilityTrial, toolbarTrial_xpm, bCreate);
		CreateSubMenu(handles.menu, m_MenuAgilityRun, toolbarRun_xpm, bCreate);
		if (bCreate)
			handles.menu->AppendSeparator();
		CreateSubMenu(handles.menu, m_MenuAgilityCalendar, toolbarCalendar_xpm, bCreate);
		CreateSubMenu(handles.menu, m_MenuAgilityTraining, toolbarTraining_xpm, bCreate);
		break;

	case IdMenuNotes:
		title = _("MenuNotes");
		DoMenu(handles.menu, bCreate, ID_NOTES_CLUBS, _("MenuNotesClubs"), _("DescNotesClubs"));
		DoMenu(handles.menu, bCreate, ID_NOTES_JUDGES, _("MenuNotesJudges"), _("DescNotesJudges"));
		DoMenu(handles.menu, bCreate, ID_NOTES_LOCATIONS, _("MenuNotesLocations"), _("DescNotesLocations"));
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_NOTES_SEARCH, _("MenuNotesSearch"), _("DescNotesSearch"));
		break;

	case IdMenuView:
		title = _("MenuView");
		DoMenu(handles.menu, bCreate, wxID_PREFERENCES, _("MenuViewOptions"), _("DescViewOptions"));
		DoMenu(handles.menu, bCreate, ID_VIEW_CUSTOMIZE, _("MenuViewCustomize"), _("DescViewCustomize"));
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_VIEW_SORTRUNS, _("MenuViewNewestTrials"), _("DescViewNewestTrials"), NULL, wxITEM_CHECK);
		DoMenu(handles.menu, bCreate, ID_VIEW_RUNS_BY_TRIAL, _("MenuViewRunsByTrial"), _("DescViewRunsByTrial"), NULL, wxITEM_CHECK);
		DoMenu(handles.menu, bCreate, ID_VIEW_TABLE_IN_YPS, _("MenuViewIncludeTable"), _("DescViewIncludeTable"), NULL, wxITEM_CHECK);
		DoMenu(handles.menu, bCreate, ID_VIEW_RUNTIME_IN_OPS, _("MenuViewIncludeRunTime"), _("DescViewIncludeRunTime"), NULL, wxITEM_CHECK);
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_VIEW_POINTS_VIEW_SORT, _("MenuViewSortPoints"), _("DescViewSortPoints"));
		DoMenu(handles.menu, bCreate, ID_VIEW_HIDDEN, _("MenuViewHiddenTitles"), _("DescViewHiddenTitles"), NULL, wxITEM_CHECK);
		DoMenu(handles.menu, bCreate, ID_VIEW_LIFETIME_EVENTS, _("MenuViewLifePoints"), _("DescViewLifePoints"), NULL, wxITEM_CHECK);
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_EXPAND, _("MenuViewExpand"), _("DescViewExpand"));
		DoMenu(handles.menu, bCreate, ID_COLLAPSE, _("MenuViewCollapse"), _("DescViewCollapse"));
		DoMenu(handles.menu, bCreate, ID_EXPAND_ALL, _("MenuViewExpandAll"), _("DescViewExpandAll"));
		DoMenu(handles.menu, bCreate, ID_COLLAPSE_ALL, _("MenuViewCollapseAll"), _("DescViewCollapseAll"));
		if (bCreate)
			handles.menu->AppendSeparator();
		CreateSubMenu(handles.menu, m_MenuViewType, NULL, bCreate);
		CreateSubMenu(handles.menu, m_MenuViewOrientation, NULL, bCreate);
		break;

	case IdMenuWindow:
		title = _("MenuWindow");
		DoMenu(handles.menu, bCreate, ID_WINDOW_SPLIT, _("MenuWindowSplitter"), _("DescWindowSplitter"));
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_NEXT_PANE, _("MenuWindowNext"), _("DescWindowNext"));
		DoMenu(handles.menu, bCreate, ID_PREV_PANE, _("MenuWindowPrev"), _("DescWindowPrev"));
		break;

	case IdMenuHelp:
		title = _("MenuHelp");
		DoMenu(handles.menu, bCreate, ID_HELP_UPDATE, _("MenuHelpUpdates"), _("DescHelpUpdates"));
		DoMenu(handles.menu, bCreate, ID_HELP_SYSINFO, _("MenuHelpSysInfo"), _("DescHelpSysInfo"));
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, wxID_ABOUT, _("MenuHelpAbout"), _("DescHelpAbout"), toolbarAbout_xpm);
		break;
	}

	if (bCreate)
		m_MenuBar->Append(handles.menu, title);
	else
		m_MenuBar->SetMenuLabel(handles.idx, title);
}


void CAgilityBookMenu::CreateSubMenu(
		wxMenu* parent,
		MenuHandles& handles,
		const char* const* pBitmap,
		bool bCreate)
{
	if (bCreate)
		handles.idx = static_cast<int>(parent->GetMenuItemCount());

	wxString title;

	switch (handles.id)
	{
	case IdMenuRecentFiles:
		title = _("MenuFileRecent");
		break;

	case IdMenuAgilityDog:
		title = _("MenuDog");
		DoMenu(handles.menu, bCreate, ID_AGILITY_EDIT_DOG, _("MenuDogProperties"), _("DescDogProperties"), toolbarDog_xpm);
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_AGILITY_NEW_DOG, _("MenuNew"), _("DescDogNew"), toolbarDog_xpm);
		DoMenu(handles.menu, bCreate, ID_AGILITY_DELETE_DOG, _("MenuDogDelete"), _("DescDogDelete"));
		break;

	case IdMenuAgilityTitle:
		title = _("MenuTitle");
		DoMenu(handles.menu, bCreate, ID_AGILITY_EDIT_TITLE, _("MenuTitleProperties"), _("DescTitleProperties"), toolbarTitle_xpm);
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_AGILITY_NEW_TITLE, _("MenuNew"), _("DescTitleNew"), toolbarTitle_xpm);
		DoMenu(handles.menu, bCreate, ID_AGILITY_DELETE_TITLE, _("MenuTitleDelete"), _("DescTitleDelete"));
		break;

	case IdMenuAgilityTrial:
		title = _("MenuTrial");
		DoMenu(handles.menu, bCreate, ID_AGILITY_EDIT_TRIAL, _("MenuTrialProperties"), _("DescTrialProperties"), toolbarTrial_xpm);
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_AGILITY_NEW_TRIAL, _("MenuNew"), _("DescTrialNew"), toolbarTrial_xpm);
		DoMenu(handles.menu, bCreate, ID_AGILITY_DELETE_TRIAL, _("MenuTrialDelete"), _("DescTrialDelete"));
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_AGILITY_PRINT_TRIAL, _("MenuTrialPrintRuns"), _("DescTrialPrintRuns"));
		break;

	case IdMenuAgilityRun:
		title = _("MenuRun");
		DoMenu(handles.menu, bCreate, ID_AGILITY_EDIT_RUN, _("MenuRunProperties"), _("DescRunProperties"), toolbarRun_xpm);
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_AGILITY_NEW_RUN, _("MenuNewRun"), _("DescRunNew"), toolbarRun_xpm);
		DoMenu(handles.menu, bCreate, ID_AGILITY_DELETE_RUN, _("MenuRunDelete"), _("DescRunDelete"));
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_AGILITY_PRINT_RUNS, _("MenuRunPrintRuns"), _("DescRunPrintRuns"));
		break;

	case IdMenuAgilityCalendar:
		title = _("MenuCalendar");
		DoMenu(handles.menu, bCreate, ID_AGILITY_EDIT_CALENDAR, _("MenuCalendarProperties"), _("DescCalendarProperties"), toolbarCalendar_xpm);
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_AGILITY_NEW_CALENDAR, _("MenuNew"), _("DescCalendarNew"), toolbarCalendar_xpm);
		DoMenu(handles.menu, bCreate, ID_AGILITY_DELETE_CALENDAR, _("MenuCalendarDelete"), _("DescCalendarDelete"));
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_AGILITY_UPDATE_CALENDAR, _("MenuCalendarUpdate"), _("DescCalendarUpdate"));
		DoMenu(handles.menu, bCreate, ID_AGILITY_EXPORT_CALENDAR, _("MenuCalendarExport"), _("DescCalendarExport"));
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_AGILITY_CREATEENTRY_CALENDAR, _("MenuCalendarTrialEntry"), _("DescCalendarTrialEntry"));
		break;

	case IdMenuAgilityTraining:
		title = _("MenuTraining");
		DoMenu(handles.menu, bCreate, ID_AGILITY_EDIT_TRAINING, _("MenuTrainingProperties"), _("DescTrainingProperties"), toolbarTraining_xpm);
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_AGILITY_NEW_TRAINING, _("MenuNew"), _("DescTrainingNew"), toolbarTraining_xpm);
		DoMenu(handles.menu, bCreate, ID_AGILITY_DELETE_TRAINING, _("MenuTrainingDelete"), _("DescTrainingDelete"));
		break;

	case IdMenuViewType:
		title = _("MenuViewType");
#if wxUSE_NOTEBOOK
		DoMenu(handles.menu, bCreate, ID_BOOK_NOTEBOOK, _("MenuViewNotebook"), _("DescViewNotebook"), NULL, wxITEM_RADIO);
#endif
#if wxUSE_LISTBOOK
		DoMenu(handles.menu, bCreate, ID_BOOK_LISTBOOK, _("MenuViewListbook"), _("DescViewListbook"), NULL, wxITEM_RADIO);
#endif
#if wxUSE_CHOICEBOOK
		DoMenu(handles.menu, bCreate, ID_BOOK_CHOICEBOOK, _("MenuViewChoicebook"), _("DescViewChoicebook"), NULL, wxITEM_RADIO);
#endif
#if wxUSE_TREEBOOK
		DoMenu(handles.menu, bCreate, ID_BOOK_TREEBOOK, _("MenuViewTreebook"), _("DescViewTreebook"), NULL, wxITEM_RADIO);
#endif
#if wxUSE_TOOLBOOK
		DoMenu(handles.menu, bCreate, ID_BOOK_TOOLBOOK, _("MenuViewToolbook"), _("DescViewToolbook"), NULL, wxITEM_RADIO);
#endif
		break;

	case IdMenuViewOrientation:
		title = _("MenuViewOrientation");
		DoMenu(handles.menu, bCreate, ID_ORIENT_TOP, _("MenuViewTop"), _("DescViewTop"), NULL, wxITEM_RADIO);
		DoMenu(handles.menu, bCreate, ID_ORIENT_BOTTOM, _("MenuViewBottom"), _("DescViewBottom"), NULL, wxITEM_RADIO);
		DoMenu(handles.menu, bCreate, ID_ORIENT_LEFT, _("MenuViewLeft"), _("DescViewLeft"), NULL, wxITEM_RADIO);
		DoMenu(handles.menu, bCreate, ID_ORIENT_RIGHT, _("MenuViewRight"), _("DescViewRight"), NULL, wxITEM_RADIO);
		break;
	}

	if (bCreate)
	{
		DoMenuItem(parent, wxID_ANY, title, handles.menu, pBitmap);
	}
	else
#if wxCHECK_VERSION(2, 9, 3)
		parent->FindItemByPosition(handles.idx)->SetItemLabel(title);
#else
		parent->FindItemByPosition(handles.idx)->SetText(title);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Popup menus are created each time, so we don't need the effort of above
// to fix the text on language change.

// Create New submenu with dog/title/trial/run
static wxMenu* CreateNewMenu(bool incRun)
{
	wxMenu* menu = new wxMenu;
	DoMenuItem(menu, ID_AGILITY_NEW_DOG, _("MenuDogNew"), _("DescDogNew"), toolbarDog_xpm);
	DoMenuItem(menu, ID_AGILITY_NEW_TITLE, _("MenuTitleNew"), _("DescTitleNew"), toolbarTitle_xpm);
	menu->AppendSeparator();
	DoMenuItem(menu, ID_AGILITY_NEW_TRIAL, _("MenuTrialNew"), _("DescTrialNew"), toolbarTrial_xpm);
	if (incRun)
	{
		DoMenuItem(menu, ID_AGILITY_NEW_RUN, _("MenuRunNew"), _("DescRunNew"), toolbarRun_xpm);
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
		DoMenuItem(menu, ID_AGILITY_EDIT_DOG, _("MenuDogProperties"), _("DescDogProperties"), toolbarDog_xpm);
		DoMenuItem(menu, wxID_PREFERENCES, _("MenuViewOptions"), _("DescViewOptions"));
		menu->AppendSeparator();
		menu->Append(wxID_ANY, _("MenuNew"), CreateNewMenu(false));
		DoMenuItem(menu, wxID_DUPLICATE, _("MenuEditDuplicate"), _("DescEditDuplicate"));
		DoMenuItem(menu, wxID_CUT, _("MenuEditCut"), _("DescEditCut"), toolbarCut_xpm);
		DoMenuItem(menu, wxID_COPY, _("MenuEditCopy"), _("DescEditCopy"), toolbarCopy_xpm);
		DoMenuItem(menu, wxID_PASTE, _("MenuEditPaste"), _("DescEditPaste"), toolbarPaste_xpm);
		DoMenuItem(menu, ID_AGILITY_DELETE_DOG, _("MenuDogDelete"), _("DescDogDelete"));
		menu->AppendSeparator();
		DoMenuItem(menu, ID_REORDER, _("MenuEditReorder"), _("DescEditReorder"));
		menu->AppendSeparator();
		DoMenuItem(menu, ID_EXPAND, _("MenuViewExpand"), _("DescViewExpand"));
		DoMenuItem(menu, ID_COLLAPSE, _("MenuViewCollapse"), _("DescViewCollapse"));
		DoMenuItem(menu, ID_EXPAND_ALL, _("MenuViewExpandAll"), _("DescViewExpandAll"));
		DoMenuItem(menu, ID_COLLAPSE_ALL, _("MenuViewCollapseAll"), _("DescViewCollapseAll"));
		break;

	case IdMenuTrial:
		DoMenuItem(menu, ID_AGILITY_EDIT_TRIAL, _("MenuTrialProperties"), _("DescTrialProperties"), toolbarTrial_xpm);
		DoMenuItem(menu, wxID_PREFERENCES, _("MenuViewOptions"), _("DescViewOptions"));
		menu->AppendSeparator();
		menu->Append(wxID_ANY, _("MenuNew"), CreateNewMenu(true));
		DoMenuItem(menu, wxID_DUPLICATE, _("MenuEditDuplicate"), _("DescEditDuplicate"));
		DoMenuItem(menu, wxID_CUT, _("MenuEditCut"), _("DescEditCut"), toolbarCut_xpm);
		DoMenuItem(menu, wxID_COPY, _("MenuEditCopy"), _("DescEditCopy"), toolbarCopy_xpm);
		DoMenuItem(menu, wxID_PASTE, _("MenuEditPaste"), _("DescEditPaste"), toolbarPaste_xpm);
		DoMenuItem(menu, ID_AGILITY_DELETE_TRIAL, _("MenuTrialDelete"), _("DescTrialDelete"));
		menu->AppendSeparator();
		DoMenuItem(menu, ID_REORDER, _("MenuEditReorder"), _("DescEditReorder"));
		menu->AppendSeparator();
		DoMenuItem(menu, ID_EXPAND, _("MenuViewExpand"), _("DescViewExpand"));
		DoMenuItem(menu, ID_COLLAPSE, _("MenuViewCollapse"), _("DescViewCollapse"));
		menu->AppendSeparator();
		DoMenuItem(menu, ID_AGILITY_PRINT_TRIAL, _("MenuTrialPrintRuns"), _("DescTrialPrintRuns"));
		break;

	case IdMenuRun:
		DoMenuItem(menu, ID_AGILITY_EDIT_RUN, _("MenuRunProperties"), _("DescRunProperties"), toolbarRun_xpm);
		DoMenuItem(menu, wxID_PREFERENCES, _("MenuViewOptions"), _("DescViewOptions"));
		menu->AppendSeparator();
		menu->Append(wxID_ANY, _("MenuNew"), CreateNewMenu(true));
		DoMenuItem(menu, wxID_DUPLICATE, _("MenuEditDuplicate"), _("DescEditDuplicate"));
		DoMenuItem(menu, wxID_CUT, _("MenuEditCut"), _("DescEditCut"), toolbarCut_xpm);
		DoMenuItem(menu, wxID_COPY, _("MenuEditCopy"), _("DescEditCopy"), toolbarCopy_xpm);
		DoMenuItem(menu, wxID_PASTE, _("MenuEditPaste"), _("DescEditPaste"), toolbarPaste_xpm);
		DoMenuItem(menu, ID_AGILITY_DELETE_RUN, _("MenuRunDelete"), _("DescRunDelete"));
		menu->AppendSeparator();
		DoMenuItem(menu, ID_REORDER, _("MenuEditReorder"), _("DescEditReorder"));
		menu->AppendSeparator();
		DoMenuItem(menu, ID_AGILITY_PRINT_RUNS, _("MenuRunPrintRuns"), _("DescRunPrintRuns"));
		break;

	case IdMenuPoints:
		DoMenuItem(menu, ID_DETAILS, _("MenuDetails"), _("DescDetails"));
		DoMenuItem(menu, wxID_PREFERENCES, _("MenuViewOptions"), _("DescViewOptions"));
		menu->AppendSeparator();
		DoMenuItem(menu, wxID_COPY, _("MenuEditCopySelection"), _("DescEditCopy"), toolbarCopy_xpm);
		DoMenuItem(menu, ID_COPY_TITLES_LIST, _("MenuEditCopyTitles"), _("DescEditCopyTitles"));
		menu->AppendSeparator();
		DoMenuItem(menu, wxID_SELECTALL, _("MenuEditSelectAll"), _("DescEditSelectAll"));
		break;

	case IdMenuCalendar:
		DoMenuItem(menu, ID_AGILITY_EDIT_CALENDAR, _("MenuCalendarProperties"), _("DescCalendarProperties"), toolbarCalendar_xpm);
		DoMenuItem(menu, wxID_PREFERENCES, _("MenuViewOptions"), _("DescViewOptions"));
		menu->AppendSeparator();
		DoMenuItem(menu, ID_AGILITY_NEW_CALENDAR, _("MenuNew"), _("DescCalendarNew"), toolbarCalendar_xpm);
		DoMenuItem(menu, wxID_DUPLICATE, _("MenuEditDuplicate"), _("DescEditDuplicate"));
		DoMenuItem(menu, wxID_CUT, _("MenuEditCut"), _("DescEditCut"), toolbarCut_xpm);
		DoMenuItem(menu, wxID_COPY, _("MenuEditCopy"), _("DescEditCopy"), toolbarCopy_xpm);
		DoMenuItem(menu, wxID_PASTE, _("MenuEditPaste"), _("DescEditPaste"), toolbarPaste_xpm);
		DoMenuItem(menu, ID_AGILITY_DELETE_CALENDAR, _("MenuCalendarDelete"), _("DescCalendarDelete"));
		menu->AppendSeparator();
		DoMenuItem(menu, ID_AGILITY_UPDATE_CALENDAR, _("MenuCalendarUpdate"), _("DescCalendarUpdate"));
		DoMenuItem(menu, ID_AGILITY_EXPORT_CALENDAR, _("MenuCalendarExport"), _("DescCalendarExport"));
		menu->AppendSeparator();
		DoMenuItem(menu, ID_AGILITY_CREATEENTRY_CALENDAR, _("MenuCalendarTrialEntry"), _("DescCalendarTrialEntry"));
		break;

	case IdMenuTraining:
		DoMenuItem(menu, ID_AGILITY_EDIT_TRAINING, _("MenuTrainingProperties"), _("DescTrainingProperties"), toolbarTraining_xpm);
		DoMenuItem(menu, wxID_PREFERENCES, _("MenuViewOptions"), _("DescViewOptions"));
		menu->AppendSeparator();
		DoMenuItem(menu, ID_AGILITY_NEW_TRAINING, _("MenuNew"), _("DescTrainingNew"), toolbarTraining_xpm);
		DoMenuItem(menu, wxID_DUPLICATE, _("MenuEditDuplicate"), _("DescEditDuplicate"));
		DoMenuItem(menu, wxID_CUT, _("MenuEditCut"), _("DescEditCut"), toolbarCut_xpm);
		DoMenuItem(menu, wxID_COPY, _("MenuEditCopy"), _("DescEditCopy"), toolbarCopy_xpm);
		DoMenuItem(menu, wxID_PASTE, _("MenuEditPaste"), _("DescEditPaste"), toolbarPaste_xpm);
		DoMenuItem(menu, ID_AGILITY_DELETE_TRAINING, _("MenuTrainingDelete"), _("DescTrainingDelete"));
		break;
	}

	return menu;
}
