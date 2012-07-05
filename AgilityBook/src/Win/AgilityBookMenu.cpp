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
}


enum eMenuType
{
	eMenuNormal,
	eMenuCheck,
	eMenuRadio,
};


static void DoMenu(
		wxMenu* menu,
		bool bCreate,
		int id,
		wxString const& label,
		wxString const& desc,
		eMenuType eType = eMenuNormal)
{
	if (bCreate)
	{
		switch (eType)
		{
		default:
		case eMenuNormal:
			menu->Append(id, label, desc);
			break;
		case eMenuCheck:
			menu->AppendCheckItem(id, label, desc);
			break;
		case eMenuRadio:
			menu->AppendRadioItem(id, label, desc);
			break;
		}
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
		DoMenu(handles.menu, bCreate, wxID_NEW, _("MenuFileNew"), _("DescFileNew"));
		DoMenu(handles.menu, bCreate, wxID_OPEN, _("MenuFileOpen"), _("DescFileOpen"));
		DoMenu(handles.menu, bCreate, wxID_SAVE, _("MenuFileSave"), _("DescFileSave"));
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
		DoMenu(handles.menu, bCreate, wxID_PRINT, _("MenuFilePrint"), _("DescFilePrint"));
		DoMenu(handles.menu, bCreate, ID_FILE_PRINT_BLANK_RUNS, _("MenuFilePrintBlank"), _("DescFilePrintBlank"));
		DoMenu(handles.menu, bCreate, wxID_PREVIEW, _("MenuFilePrintPreview"), _("DescFilePrintPreview"));
		if (bCreate)
		{
			if (manager)
			{
				handles.menu->AppendSeparator();
				CreateSubMenu(handles.menu, m_MenuFileRecent, bCreate);
				manager->FileHistoryUseMenu(m_MenuFileRecent.menu);
				manager->FileHistoryAddFilesToMenu();
			}
			handles.menu->AppendSeparator();
		}
		else
		{
			CreateSubMenu(handles.menu, m_MenuFileRecent, bCreate);
		}
		DoMenu(handles.menu, bCreate, wxID_EXIT, _("MenuFileExit"), _("DescFileExit"));
		break;

	case IdMenuEdit:
		title = _("MenuEdit");
		DoMenu(handles.menu, bCreate, wxID_DUPLICATE, _("MenuEditDuplicate"), _("DescEditDuplicate"));
		DoMenu(handles.menu, bCreate, wxID_CUT, _("MenuEditCut"), _("DescEditCut"));
		DoMenu(handles.menu, bCreate, wxID_COPY, _("MenuEditCopy"), _("DescEditCopy"));
		DoMenu(handles.menu, bCreate, wxID_PASTE, _("MenuEditPaste"), _("DescEditPaste"));
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
		CreateSubMenu(handles.menu, m_MenuAgilityDog, bCreate);
		CreateSubMenu(handles.menu, m_MenuAgilityTitle, bCreate);
		CreateSubMenu(handles.menu, m_MenuAgilityTrial, bCreate);
		CreateSubMenu(handles.menu, m_MenuAgilityRun, bCreate);
		if (bCreate)
			handles.menu->AppendSeparator();
		CreateSubMenu(handles.menu, m_MenuAgilityCalendar, bCreate);
		CreateSubMenu(handles.menu, m_MenuAgilityTraining, bCreate);
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
		DoMenu(handles.menu, bCreate, ID_VIEW_SORTRUNS, _("MenuViewNewestTrials"), _("DescViewNewestTrials"), eMenuCheck);
		DoMenu(handles.menu, bCreate, ID_VIEW_RUNS_BY_TRIAL, _("MenuViewRunsByTrial"), _("DescViewRunsByTrial"), eMenuCheck);
		DoMenu(handles.menu, bCreate, ID_VIEW_TABLE_IN_YPS, _("MenuViewIncludeTable"), _("DescViewIncludeTable"), eMenuCheck);
		DoMenu(handles.menu, bCreate, ID_VIEW_RUNTIME_IN_OPS, _("MenuViewIncludeRunTime"), _("DescViewIncludeRunTime"), eMenuCheck);
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_VIEW_POINTS_VIEW_SORT, _("MenuViewSortPoints"), _("DescViewSortPoints"));
		DoMenu(handles.menu, bCreate, ID_VIEW_HIDDEN, _("MenuViewHiddenTitles"), _("DescViewHiddenTitles"), eMenuCheck);
		DoMenu(handles.menu, bCreate, ID_VIEW_LIFETIME_EVENTS, _("MenuViewLifePoints"), _("DescViewLifePoints"), eMenuCheck);
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_EXPAND, _("MenuViewExpand"), _("DescViewExpand"));
		DoMenu(handles.menu, bCreate, ID_COLLAPSE, _("MenuViewCollapse"), _("DescViewCollapse"));
		DoMenu(handles.menu, bCreate, ID_EXPAND_ALL, _("MenuViewExpandAll"), _("DescViewExpandAll"));
		DoMenu(handles.menu, bCreate, ID_COLLAPSE_ALL, _("MenuViewCollapseAll"), _("DescViewCollapseAll"));
		if (bCreate)
			handles.menu->AppendSeparator();
		CreateSubMenu(handles.menu, m_MenuViewType, bCreate);
		CreateSubMenu(handles.menu, m_MenuViewOrientation, bCreate);
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
		DoMenu(handles.menu, bCreate, wxID_ABOUT, _("MenuHelpAbout"), _("DescHelpAbout"));
		break;
	}

	if (bCreate)
		m_MenuBar->Append(handles.menu, title);
	else
		m_MenuBar->SetMenuLabel(handles.idx, title);
}


void CAgilityBookMenu::CreateSubMenu(wxMenu* parent, MenuHandles& handles, bool bCreate)
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
		DoMenu(handles.menu, bCreate, ID_AGILITY_EDIT_DOG, _("MenuDogProperties"), _("DescDogProperties"));
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_AGILITY_NEW_DOG, _("MenuNew"), _("DescDogNew"));
		DoMenu(handles.menu, bCreate, ID_AGILITY_DELETE_DOG, _("MenuDogDelete"), _("DescDogDelete"));
		break;

	case IdMenuAgilityTitle:
		title = _("MenuTitle");
		DoMenu(handles.menu, bCreate, ID_AGILITY_EDIT_TITLE, _("MenuTitleProperties"), _("DescTitleProperties"));
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_AGILITY_NEW_TITLE, _("MenuNew"), _("DescTitleNew"));
		DoMenu(handles.menu, bCreate, ID_AGILITY_DELETE_TITLE, _("MenuTitleDelete"), _("DescTitleDelete"));
		break;

	case IdMenuAgilityTrial:
		title = _("MenuTrial");
		DoMenu(handles.menu, bCreate, ID_AGILITY_EDIT_TRIAL, _("MenuTrialProperties"), _("DescTrialProperties"));
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_AGILITY_NEW_TRIAL, _("MenuNew"), _("DescTrialNew"));
		DoMenu(handles.menu, bCreate, ID_AGILITY_DELETE_TRIAL, _("MenuTrialDelete"), _("DescTrialDelete"));
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_AGILITY_PRINT_TRIAL, _("MenuTrialPrintRuns"), _("DescTrialPrintRuns"));
		break;

	case IdMenuAgilityRun:
		title = _("MenuRun");
		DoMenu(handles.menu, bCreate, ID_AGILITY_EDIT_RUN, _("MenuRunProperties"), _("DescRunProperties"));
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_AGILITY_NEW_RUN, _("MenuNewRun"), _("DescRunNew"));
		DoMenu(handles.menu, bCreate, ID_AGILITY_DELETE_RUN, _("MenuRunDelete"), _("DescRunDelete"));
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_AGILITY_PRINT_RUNS, _("MenuRunPrintRuns"), _("DescRunPrintRuns"));
		break;

	case IdMenuAgilityCalendar:
		title = _("MenuCalendar");
		DoMenu(handles.menu, bCreate, ID_AGILITY_EDIT_CALENDAR, _("MenuCalendarProperties"), _("DescCalendarProperties"));
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_AGILITY_NEW_CALENDAR, _("MenuNew"), _("DescCalendarNew"));
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
		DoMenu(handles.menu, bCreate, ID_AGILITY_EDIT_TRAINING, _("MenuTrainingProperties"), _("DescTrainingProperties"));
		if (bCreate)
			handles.menu->AppendSeparator();
		DoMenu(handles.menu, bCreate, ID_AGILITY_NEW_TRAINING, _("MenuNew"), _("DescTrainingNew"));
		DoMenu(handles.menu, bCreate, ID_AGILITY_DELETE_TRAINING, _("MenuTrainingDelete"), _("DescTrainingDelete"));
		break;

	case IdMenuViewType:
		title = _("MenuViewType");
#if wxUSE_NOTEBOOK
		DoMenu(handles.menu, bCreate, ID_BOOK_NOTEBOOK, _("MenuViewNotebook"), _("DescViewNotebook"), eMenuRadio);
#endif
#if wxUSE_LISTBOOK
		DoMenu(handles.menu, bCreate, ID_BOOK_LISTBOOK, _("MenuViewListbook"), _("DescViewListbook"), eMenuRadio);
#endif
#if wxUSE_CHOICEBOOK
		DoMenu(handles.menu, bCreate, ID_BOOK_CHOICEBOOK, _("MenuViewChoicebook"), _("DescViewChoicebook"), eMenuRadio);
#endif
#if wxUSE_TREEBOOK
		DoMenu(handles.menu, bCreate, ID_BOOK_TREEBOOK, _("MenuViewTreebook"), _("DescViewTreebook"), eMenuRadio);
#endif
#if wxUSE_TOOLBOOK
		DoMenu(handles.menu, bCreate, ID_BOOK_TOOLBOOK, _("MenuViewToolbook"), _("DescViewToolbook"), eMenuRadio);
#endif
		break;

	case IdMenuViewOrientation:
		title = _("MenuViewOrientation");
		DoMenu(handles.menu, bCreate, ID_ORIENT_TOP, _("MenuViewTop"), _("DescViewTop"), eMenuRadio);
		DoMenu(handles.menu, bCreate, ID_ORIENT_BOTTOM, _("MenuViewBottom"), _("DescViewBottom"), eMenuRadio);
		DoMenu(handles.menu, bCreate, ID_ORIENT_LEFT, _("MenuViewLeft"), _("DescViewLeft"), eMenuRadio);
		DoMenu(handles.menu, bCreate, ID_ORIENT_RIGHT, _("MenuViewRight"), _("DescViewRight"), eMenuRadio);
		break;
	}

	if (bCreate)
		parent->Append(wxID_ANY, title, handles.menu);
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
	menu->Append(ID_AGILITY_NEW_DOG, _("MenuDogNew"), _("DescDogNew"));
	menu->Append(ID_AGILITY_NEW_TITLE, _("MenuTitleNew"), _("DescTitleNew"));
	menu->AppendSeparator();
	menu->Append(ID_AGILITY_NEW_TRIAL, _("MenuTrialNew"), _("DescTrialNew"));
	if (incRun)
	{
		menu->Append(ID_AGILITY_NEW_RUN, _("MenuRunNew"), _("DescRunNew"));
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
		menu->Append(ID_AGILITY_EDIT_DOG,
				_("MenuDogProperties"), _("DescDogProperties"));
		menu->Append(wxID_PREFERENCES,
				_("MenuViewOptions"), _("DescViewOptions"));
		menu->AppendSeparator();
		menu->Append(wxID_ANY, _("MenuNew"), CreateNewMenu(false));
		menu->Append(wxID_DUPLICATE,
				_("MenuEditDuplicate"), _("DescEditDuplicate"));
		menu->Append(wxID_CUT,
				_("MenuEditCut"), _("DescEditCut"));
		menu->Append(wxID_COPY,
				_("MenuEditCopy"), _("DescEditCopy"));
		menu->Append(wxID_PASTE,
				_("MenuEditPaste"), _("DescEditPaste"));
		menu->Append(ID_AGILITY_DELETE_DOG,
				_("MenuDogDelete"), _("DescDogDelete"));
		menu->AppendSeparator();
		menu->Append(ID_REORDER,
				_("MenuEditReorder"), _("DescEditReorder"));
		menu->AppendSeparator();
		menu->Append(ID_EXPAND,
				_("MenuViewExpand"), _("DescViewExpand"));
		menu->Append(ID_COLLAPSE,
				_("MenuViewCollapse"), _("DescViewCollapse"));
		menu->Append(ID_EXPAND_ALL,
				_("MenuViewExpandAll"), _("DescViewExpandAll"));
		menu->Append(ID_COLLAPSE_ALL,
				_("MenuViewCollapseAll"), _("DescViewCollapseAll"));
		break;

	case IdMenuTrial:
		menu->Append(ID_AGILITY_EDIT_TRIAL,
				_("MenuTrialProperties"), _("DescTrialProperties"));
		menu->Append(wxID_PREFERENCES,
				_("MenuViewOptions"), _("DescViewOptions"));
		menu->AppendSeparator();
		menu->Append(wxID_ANY, _("MenuNew"), CreateNewMenu(true));
		menu->Append(wxID_DUPLICATE,
				_("MenuEditDuplicate"), _("DescEditDuplicate"));
		menu->Append(wxID_CUT,
				_("MenuEditCut"), _("DescEditCut"));
		menu->Append(wxID_COPY,
				_("MenuEditCopy"), _("DescEditCopy"));
		menu->Append(wxID_PASTE,
				_("MenuEditPaste"), _("DescEditPaste"));
		menu->Append(ID_AGILITY_DELETE_TRIAL,
				_("MenuTrialDelete"), _("DescTrialDelete"));
		menu->AppendSeparator();
		menu->Append(ID_REORDER,
				_("MenuEditReorder"), _("DescEditReorder"));
		menu->AppendSeparator();
		menu->Append(ID_EXPAND,
				_("MenuViewExpand"), _("DescViewExpand"));
		menu->Append(ID_COLLAPSE,
				_("MenuViewCollapse"), _("DescViewCollapse"));
		menu->AppendSeparator();
		menu->Append(ID_AGILITY_PRINT_TRIAL,
				_("MenuTrialPrintRuns"), _("DescTrialPrintRuns"));
		break;

	case IdMenuRun:
		menu->Append(ID_AGILITY_EDIT_RUN,
				_("MenuRunProperties"), _("DescRunProperties"));
		menu->Append(wxID_PREFERENCES,
				_("MenuViewOptions"), _("DescViewOptions"));
		menu->AppendSeparator();
		menu->Append(wxID_ANY, _("MenuNew"), CreateNewMenu(true));
		menu->Append(wxID_DUPLICATE,
				_("MenuEditDuplicate"), _("DescEditDuplicate"));
		menu->Append(wxID_CUT,
				_("MenuEditCut"), _("DescEditCut"));
		menu->Append(wxID_COPY,
				_("MenuEditCopy"), _("DescEditCopy"));
		menu->Append(wxID_PASTE,
				_("MenuEditPaste"), _("DescEditPaste"));
		menu->Append(ID_AGILITY_DELETE_RUN,
				_("MenuRunDelete"), _("DescRunDelete"));
		menu->AppendSeparator();
		menu->Append(ID_REORDER,
				_("MenuEditReorder"), _("DescEditReorder"));
		menu->AppendSeparator();
		menu->Append(ID_AGILITY_PRINT_RUNS,
				_("MenuRunPrintRuns"), _("DescRunPrintRuns"));
		break;

	case IdMenuPoints:
		menu->Append(ID_DETAILS,
				_("MenuDetails"), _("DescDetails"));
		menu->Append(wxID_PREFERENCES,
				_("MenuViewOptions"), _("DescViewOptions"));
		menu->AppendSeparator();
		menu->Append(wxID_COPY,
				_("MenuEditCopySelection"), _("DescEditCopy"));
		menu->Append(ID_COPY_TITLES_LIST,
				_("MenuEditCopyTitles"), _("DescEditCopyTitles"));
		menu->AppendSeparator();
		menu->Append(wxID_SELECTALL,
				_("MenuEditSelectAll"), _("DescEditSelectAll"));
		break;

	case IdMenuCalendar:
		menu->Append(ID_AGILITY_EDIT_CALENDAR,
				_("MenuCalendarProperties"), _("DescCalendarProperties"));
		menu->Append(wxID_PREFERENCES,
				_("MenuViewOptions"), _("DescViewOptions"));
		menu->AppendSeparator();
		menu->Append(ID_AGILITY_NEW_CALENDAR,
				_("MenuNew"), _("DescCalendarNew"));
		menu->Append(wxID_DUPLICATE,
				_("MenuEditDuplicate"), _("DescEditDuplicate"));
		menu->Append(wxID_CUT,
				_("MenuEditCut"), _("DescEditCut"));
		menu->Append(wxID_COPY,
				_("MenuEditCopy"), _("DescEditCopy"));
		menu->Append(wxID_PASTE,
				_("MenuEditPaste"), _("DescEditPaste"));
		menu->Append(ID_AGILITY_DELETE_CALENDAR,
				_("MenuCalendarDelete"), _("DescCalendarDelete"));
		menu->AppendSeparator();
		menu->Append(ID_AGILITY_UPDATE_CALENDAR,
				_("MenuCalendarUpdate"), _("DescCalendarUpdate"));
		menu->Append(ID_AGILITY_EXPORT_CALENDAR,
				_("MenuCalendarExport"), _("DescCalendarExport"));
		menu->AppendSeparator();
		menu->Append(ID_AGILITY_CREATEENTRY_CALENDAR,
				_("MenuCalendarTrialEntry"), _("DescCalendarTrialEntry"));
		break;

	case IdMenuTraining:
		menu->Append(ID_AGILITY_EDIT_TRAINING,
				_("MenuTrainingProperties"), _("DescTrainingProperties"));
		menu->Append(wxID_PREFERENCES,
				_("MenuViewOptions"), _("DescViewOptions"));
		menu->AppendSeparator();
		menu->Append(ID_AGILITY_NEW_TRAINING,
				_("MenuNew"), _("DescTrainingNew"));
		menu->Append(wxID_DUPLICATE,
				_("MenuEditDuplicate"), _("DescEditDuplicate"));
		menu->Append(wxID_CUT,
				_("MenuEditCut"), _("DescEditCut"));
		menu->Append(wxID_COPY,
				_("MenuEditCopy"), _("DescEditCopy"));
		menu->Append(wxID_PASTE,
				_("MenuEditPaste"), _("DescEditPaste"));
		menu->Append(ID_AGILITY_DELETE_TRAINING,
				_("MenuTrainingDelete"), _("DescTrainingDelete"));
		break;
	}

	return menu;
}
