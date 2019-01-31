/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CMainFrame class
 * @author David Connet
 *
 * Revision History
 * 2018-12-16 Convert to fmt.
 * 2015-09-30 Dbl-click on Filter status area invokes Preferences.
 * 2015-09-12 Clean up status bar sizing. SetStatusBarWidths was broken on Mac.
 * 2015-08-11 Status bar gripper is too small on hiDPI. Kludge.
 * 2014-05-18 Moved startup version check to a timer.
 * 2012-11-14 "Filtered" was truncated in wx2.8 status bar.
 * 2012-01-07 Fix tab type/orientation persistence.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-07-25 Fix status width on Mac.
 * 2009-01-06 Ported to wxWidgets.
 * 2008-11-19 Added context menus to status bar.
 * 2006-09-01 Added multi-monitor support.
 * 2003-05-18 Obsoleted registry settings "Calendar"/"List" (bool)
 *            "Settings"/"View" (int) changed (see TabView.cpp)
 */

#include "stdafx.h"
#include "MainFrm.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookMenu.h"
#include "CommonView.h"
#include "DlgMessage.h"
#include "Globals.h"
#include "ImageHelper.h"
#include "PointsData.h"
#include "Print.h"
#include "RegItems.h"
#include "TabView.h"
#include "VersionNumber.h"

#include "ARBCommon/ARBMisc.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ARBDebug.h"
#include "LibARBWin/DPI.h"
#include <wx/config.h>
#include <wx/dnd.h>
#include <wx/platinfo.h>
#include <wx/stdpaths.h>
#include <wx/version.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


/////////////////////////////////////////////////////////////////////////////

#if defined(__WXMAC__)
static std::wstring m_idleOpen;
#endif

bool CFileDropTarget::OnDropFiles(
		wxCoord x,
		wxCoord y,
		wxArrayString const& filenames)
{
	if (1 != filenames.size())
		return false;
#if defined(__WXMAC__)
	m_idleOpen = StringUtil::stringW(filenames[0]);
#else
	// Doing this on the Mac causes a crash. On Mac, registering this on the
	// mainframe doesn't work. If has to be registered on the topmost window.
	// When this is called, it causes the drop target to be deleted, but things
	// aren't fully cleaned up, so as this call unwinds, the object it was in
	// has been deleted, and it dies.
	m_docMgr->CreateDocument(filenames[0], wxDOC_SILENT);
#endif
	return true;
}


/////////////////////////////////////////////////////////////////////////////

// At startup, set a timer and then perform check.
// Doing the check in app OnInit caused a crash when we downloaded the program.
// A wxHTTP object was put in the pending delete queue - when that was cleaned
// up during app tear down, it crashed. Letting the app actually start and then
// handling upgrade here solved that issue.

void CMainFrame::CStartupEvent::Notify()
{
	if (wxGetApp().AutoCheckProgram())
		wxGetApp().GetTopWindow()->Close();
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(CMainFrame, wxDocParentFrame)
#ifdef EVT_DPI_CHANGED
	EVT_DPI_CHANGED(CMainFrame::OnDPIChanged)
#endif
	EVT_CLOSE(CMainFrame::OnClose)
#if defined(__WXMAC__)
	EVT_IDLE(CMainFrame::OnIdle)
#endif
	EVT_UPDATE_UI(wxID_DUPLICATE, CMainFrame::OnUpdateCmd)
	EVT_UPDATE_UI(wxID_CUT, CMainFrame::OnUpdateCmd)
	EVT_UPDATE_UI(wxID_COPY, CMainFrame::OnUpdateCmd)
	EVT_UPDATE_UI(wxID_PASTE, CMainFrame::OnUpdateCmd)
	EVT_UPDATE_UI(wxID_DELETE, CMainFrame::OnUpdateCmd)
	EVT_UPDATE_UI(wxID_SELECTALL, CMainFrame::OnUpdateCmd)
	EVT_UPDATE_UI(wxID_FIND, CMainFrame::OnUpdateCmd)
	EVT_UPDATE_UI_RANGE(ID_ARB_FIRST, ID_ARB_LAST, CMainFrame::OnUpdateCmd)
	EVT_MENU(ID_FILE_LANGUAGE_CHOOSE, CMainFrame::OnFileLanguageChoose)
	EVT_MENU(ID_FILE_PRINT_BLANK_RUNS, CMainFrame::OnPrintBlankRuns)
	EVT_MENU(wxID_EXIT, CMainFrame::OnQuit)
	EVT_MENU_RANGE(ID_BOOK_FIRST, ID_BOOK_LAST, CMainFrame::OnType)
	EVT_MENU_RANGE(ID_ORIENT_FIRST, ID_ORIENT_LAST, CMainFrame::OnOrient)
	EVT_MENU(ID_NEXT_PANE, CMainFrame::OnNextPane)
	EVT_MENU(ID_PREV_PANE, CMainFrame::OnPrevPane)
	EVT_MENU(ID_HELP_SYSINFO, CMainFrame::OnHelpSysinfo)
END_EVENT_TABLE()


CMainFrame::CMainFrame(wxDocManager* manager)
	: wxDocParentFrame(manager, nullptr, wxID_ANY, _("Agility Record Book"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE)
	, m_manager(manager)
	, m_Widths(NUM_STATUS_FIELDS)
{
	SetIcons(CImageManager::Get()->GetIconBundle(ImageMgrAppBundle));
//#if wxUSE_HELP
// Note: do not set help on the frame. Help will disable the min/max buttons.
//	SetExtraStyle(wxFRAME_EX_CONTEXTHELP | GetExtraStyle());
//#endif

	wxMenu *menuRecent = new wxMenu;
	manager->FileHistoryUseMenu(menuRecent);
	manager->FileHistoryAddFilesToMenu();
	wxGetApp().GetMenus().CreateMainMenu(this, menuRecent);

	wxStatusBar* statusbar = CreateStatusBar(NUM_STATUS_FIELDS);
	m_Widths.Initialize(statusbar);
	if (statusbar)
	{
		BIND_OR_CONNECT_CTRL(statusbar, wxEVT_CONTEXT_MENU, wxContextMenuEventHandler, CMainFrame::OnStatusBarContextMenu);
		BIND_OR_CONNECT_CTRL(statusbar, wxEVT_LEFT_DCLICK, wxMouseEvent, CMainFrame::OnStatusBarDblClick);
		for (int i = 1; i < NUM_STATUS_FIELDS; ++i)
		{
			wxString str;
			switch (i)
			{
			default:
				assert(0);
				break;
			case STATUS_DOG:
				str = _("ID_INDICATOR_DOG");
				break;
			case STATUS_STATUS:
				str = _("ID_INDICATOR_STATUS");
				break;
			case STATUS_FILTERED:
				str = _("ID_INDICATOR_FILTERED");
				break;
			}
			m_Widths.Update(this, i, str);
		}
	}
#if !defined(__WXMAC__)
	SetDropTarget(new CFileDropTarget(manager));
#endif

	m_timerStartup.Start(1000, wxTIMER_ONE_SHOT);
}


CMainFrame::~CMainFrame()
{
}


void CMainFrame::SetMessageText(
		std::wstring const& msg,
		bool bFiltered)
{
	SetMessage(msg, STATUS_STATUS, true);
	std::wstring filtered;
	if (bFiltered)
		filtered = _("ID_INDICATOR_FILTERED");
	SetMessage(filtered, STATUS_FILTERED, false);
}


void CMainFrame::SetMessageText2(std::wstring const& msg)
{
	SetMessage(msg, STATUS_DOG, true);
}


void CMainFrame::SetMessage(std::wstring const& msg, int index, bool bResize)
{
	wxStatusBar* statusbar = GetStatusBar();
	if (!statusbar)
		return;
	wxString str = StringUtil::stringWX(msg);
	if (bResize)
		m_Widths.Update(this, index, str);
	else
		SetStatusText(str, index);
}


void CMainFrame::OnStatusBarContextMenu(wxContextMenuEvent& evt)
{
	bool bSkip = true;
	wxStatusBar* statusbar = GetStatusBar();
	if (statusbar)
	{
		wxRect rect;
		wxPoint point = evt.GetPosition();
		if (wxDefaultPosition == point)
		{
			rect = statusbar->GetScreenRect();
			point = ::wxGetMousePosition();
			if (!rect.Contains(point))
			{
				point.x = rect.GetLeft() + rect.GetWidth() / 3;
				point.y = rect.GetTop() + rect.GetHeight() / 2;
			}
			point = statusbar->ScreenToClient(point);
		}
		else
			point = statusbar->ScreenToClient(point);
		static const int ids[] = {STATUS_DOG, STATUS_STATUS, STATUS_FILTERED};
		CAgilityBookDoc* pDoc = wxDynamicCast(GetDocumentManager()->GetCurrentDocument(), CAgilityBookDoc);
		assert(pDoc);
		if (pDoc)
		{
			for (int i = 0; i < _countof(ids); ++i)
			{
				if (statusbar->GetFieldRect(ids[i], rect) && rect.Contains(point))
				{
					if (pDoc->StatusBarContextMenu(statusbar, ids[i], point))
					{
						bSkip = false;
						break;
					}
				}
			}
		}
	}
	if (bSkip)
		evt.Skip();
}


void CMainFrame::OnStatusBarDblClick(wxMouseEvent& evt)
{
	bool bSkip = true;
	wxStatusBar* statusbar = GetStatusBar();
	if (statusbar)
	{
		wxRect rect;
		wxPoint point = evt.GetPosition();
		if (wxDefaultPosition == point)
		{
			rect = statusbar->GetScreenRect();
			point = ::wxGetMousePosition();
			if (!rect.Contains(point))
			{
				point.x = rect.GetLeft() + rect.GetWidth() / 3;
				point.y = rect.GetTop() + rect.GetHeight() / 2;
			}
			point = statusbar->ScreenToClient(point);
		}
		static const int ids[] = { STATUS_FILTERED };
		CAgilityBookDoc* pDoc = wxDynamicCast(GetDocumentManager()->GetCurrentDocument(), CAgilityBookDoc);
		assert(pDoc);
		if (pDoc)
		{
			for (int i = 0; i < _countof(ids); ++i)
			{
				if (statusbar->GetFieldRect(ids[i], rect) && rect.Contains(point))
				{
					wxCommandEvent evtCmd(wxEVT_MENU, wxID_PREFERENCES);
					evtCmd.SetEventObject(this);
					pDoc->ProcessEvent(evtCmd);
					bSkip = false;
					break;
				}
			}
		}
	}
	if (bSkip)
		evt.Skip();
}


#ifdef EVT_DPI_CHANGED
void CMainFrame::OnDPIChanged(wxDPIChangedEvent& evt)
{
#pragma PRAGMA_TODO(per-monitor rendering)
#if 0
	// wParam: new DPI
	// lParam: scaled rect of window
	DPI::SetScale(LOWORD(wParam));

	// Based on MS sample
	// For the new DPI: resize the window, select new
	// fonts, and re-render window content
	LPRECT lprcNewScale = (LPRECT)lParam;
	::SetWindowPos(this->m_hWnd,
		HWND_TOP,
		lprcNewScale->left,
		lprcNewScale->top,
		lprcNewScale->right - lprcNewScale->left,
		lprcNewScale->bottom - lprcNewScale->top,
		SWP_NOZORDER | SWP_NOACTIVATE);

	CreateFonts(hWnd);
	RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
#endif

#ifdef _DEBUG
	wxSize old = evt.GetOldDPI();
	wxSize newsz = evt.GetNewDPI();
	std::wstring str = fmt::format(L"DPI Old: {},{}  New: {},{}\n", old.x, old.y, newsz.x, newsz.y);
	OutputDebugString(str.c_str());
#endif

	evt.Skip();
}
#endif


void CMainFrame::OnClose(wxCloseEvent& evt)
{
	long state = 0;
	if (IsMaximized() || IsFullScreen())
	{
		state = 1;
	}
	else if (IsIconized())
	{
		state = -1;
	}
	else
	{
		wxRect r = GetScreenRect();
		wxConfig::Get()->Write(CFG_SETTINGS_LASTXPOS, DPI::UnScale(this, r.x));
		wxConfig::Get()->Write(CFG_SETTINGS_LASTYPOS, DPI::UnScale(this, r.y));
		wxConfig::Get()->Write(CFG_SETTINGS_LASTCX, DPI::UnScale(this, r.width));
		wxConfig::Get()->Write(CFG_SETTINGS_LASTCY, DPI::UnScale(this, r.height));
	}
	wxConfig::Get()->Write(CFG_SETTINGS_LASTSTATE, state);
	evt.Skip();
}


#if defined(__WXMAC__)
void CMainFrame::OnIdle(wxIdleEvent& evt)
{
	if (!m_idleOpen.empty())
	{
		wxString file(m_idleOpen.c_str());
		m_idleOpen.clear();
		m_manager->CreateDocument(file, wxDOC_SILENT);
	}
}
#endif


void CMainFrame::OnUpdateCmd(wxUpdateUIEvent& evt)
{
	bool bEnable = false;
	switch (evt.GetId())
	{
	default:
		// Eat std commands that haven't been implemented by a child view
		if ((ID_BOOK_FIRST <= evt.GetId() && evt.GetId() < ID_BOOK_LAST)
		|| (ID_ORIENT_FIRST <= evt.GetId() && evt.GetId() < ID_ORIENT_LAST))
		{
			CAgilityBookDoc* pDoc = wxDynamicCast(GetDocumentManager()->GetCurrentDocument(), CAgilityBookDoc);
			assert(pDoc);
			CTabView* pView = pDoc->GetTabView();
			assert(pView);
			bEnable = true;
			bool bCheck = false;
			if (ID_BOOK_FIRST <= evt.GetId() && evt.GetId() < ID_BOOK_LAST)
				bCheck = pView->GetType() == evt.GetId();
			else
				bCheck = pView->GetOrient() == evt.GetId();
			evt.Check(bCheck);
		}
		break;
	case ID_FILE_LANGUAGE_CHOOSE:
	case ID_FILE_PRINT_BLANK_RUNS:
		bEnable = true;
		break;
	case ID_NEXT_PANE:
		{
			CAgilityBookBaseExtraView* pView = wxDynamicCast(GetDocumentManager()->GetCurrentView(), CAgilityBookBaseExtraView);
			if (pView)
				bEnable = pView->HasNextPane();
		}
		break;
	case ID_PREV_PANE:
		{
			CAgilityBookBaseExtraView* pView = wxDynamicCast(GetDocumentManager()->GetCurrentView(), CAgilityBookBaseExtraView);
			if (pView)
				bEnable = pView->HasPrevPane();
		}
		break;
	case ID_HELP_SYSINFO:
		bEnable = true;
		break;
	}
	evt.Enable(bEnable);
}


void CMainFrame::OnFileLanguageChoose(wxCommandEvent& evt)
{
	if (wxGetApp().SelectLanguage(this))
	{
		if (GetDocumentManager()->GetCurrentDocument())
		{
			CUpdateHint hint(UPDATE_LANG_CHANGE);
			GetDocumentManager()->GetCurrentDocument()->UpdateAllViews(nullptr, &hint);
		}
		wxGetApp().GetMenus().UpdateMenu();

		CAgilityBookBaseView* pView = wxDynamicCast(GetDocumentManager()->GetCurrentView(), CAgilityBookBaseView);
		std::wstring msg;
		bool bFiltered = false;
		if (pView && pView->GetMessage(msg))
			bFiltered = pView->IsFiltered();
		else
			msg.clear();
		SetMessageText(msg, false);
		if (!pView || !pView->GetMessage2(msg))
			msg.clear();
		SetMessageText2(msg);
	}
}


void CMainFrame::OnPrintBlankRuns(wxCommandEvent& evt)
{
	std::vector<RunInfo> runs;
	PrintRuns(nullptr, runs);
}


void CMainFrame::OnQuit(wxCommandEvent& evt)
{
	Close(true);
}


void CMainFrame::OnType(wxCommandEvent& evt)
{
	CAgilityBookDoc* pDoc = wxDynamicCast(GetDocumentManager()->GetCurrentDocument(), CAgilityBookDoc);
	assert(pDoc);
	CTabView* pView = pDoc->GetTabView();
	assert(pView);
	pView->OnType(evt.GetId());
}


void CMainFrame::OnOrient(wxCommandEvent& evt)
{
	CAgilityBookDoc* pDoc = wxDynamicCast(GetDocumentManager()->GetCurrentDocument(), CAgilityBookDoc);
	assert(pDoc);
	CTabView* pView = pDoc->GetTabView();
	assert(pView);
	pView->OnOrient(evt.GetId());
}


void CMainFrame::OnNextPane(wxCommandEvent& evt)
{
	bool bHandled = false;
	CAgilityBookBaseExtraView* pView = wxDynamicCast(GetDocumentManager()->GetCurrentView(), CAgilityBookBaseExtraView);
	if (pView)
		bHandled = pView->NextPane();
	if (!bHandled)
		evt.Skip();
}


void CMainFrame::OnPrevPane(wxCommandEvent& evt)
{
	bool bHandled = false;
	CAgilityBookBaseExtraView* pView = wxDynamicCast(GetDocumentManager()->GetCurrentView(), CAgilityBookBaseExtraView);
	if (pView)
		bHandled = pView->PrevPane();
	if (!bHandled)
		evt.Skip();
}


void CMainFrame::OnHelpSysinfo(wxCommandEvent& evt)
{
	CVersionNum ver(ARB_VER_MAJOR, ARB_VER_MINOR, ARB_VER_DOT, ARB_VER_BUILD);
	std::wstring str = ARBDebug::GetSystemInfo(this, ver);

	CDlgMessage dlg(str, this);
	dlg.ShowModal();
}
