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
#include "AgilityBookOptions.h"
#include "CommonView.h"
#include "DlgAbout.h"
#include "DlgMessage.h"
#include "ImageHelper.h"
#include "PointsData.h"
#include "Print.h"
#include "RegItems.h"
#include "TabView.h"
#include "UpdateInfo.h"
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

#if wxUSE_DRAG_AND_DROP

bool CFileDropTarget::OnDropFiles(wxCoord x, wxCoord y, wxArrayString const& filenames)
{
	if (1 != filenames.size())
		return false;
	m_docMgr->CreateDocument(filenames[0], wxDOC_SILENT);
	return true;
}

#endif

/////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(CMainFrame, wxDocParentFrame)
#ifdef EVT_DPI_CHANGED
	EVT_DPI_CHANGED(CMainFrame::OnDPIChanged)
#endif
	EVT_CLOSE(CMainFrame::OnClose)
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
	EVT_MENU(ID_VIEW_CUSTOMIZE_ACCEL, CMainFrame::OnViewCustomizeAccel)
	EVT_MENU(ID_HELP_UPDATE, CMainFrame::OnHelpCheckUpdates)
	EVT_MENU(ID_HELP_SYSINFO, CMainFrame::OnHelpSysinfo)
	EVT_MENU(wxID_ABOUT, CMainFrame::OnHelpAbout)
wxEND_EVENT_TABLE()


CMainFrame::CMainFrame(wxDocManager* manager)
	: wxDocParentFrame(
		manager,
		nullptr,
		wxID_ANY,
		_("Agility Record Book"),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_FRAME_STYLE)
	, m_manager(manager)
	, m_statusBar(this, NUM_STATUS_FIELDS)
	, m_UpdateInfo(this)
{
	SetIcons(CImageManager::Get()->GetIconBundle(ImageMgrAppBundle));
	//#if wxUSE_HELP
	// Note: do not set help on the frame. Help will disable the min/max buttons.
	//	SetExtraStyle(wxFRAME_EX_CONTEXTHELP | GetExtraStyle());
	//#endif

	wxMenu* menuRecent = new wxMenu;
	manager->FileHistoryUseMenu(menuRecent);
	manager->FileHistoryAddFilesToMenu();
	wxGetApp().GetMenus().CreateMenu(this, menuRecent);

	wxStatusBar* statusbar = m_statusBar.Initialize();
	if (statusbar)
	{
		statusbar->Bind(wxEVT_CONTEXT_MENU, &CMainFrame::OnStatusBarContextMenu, this);
		statusbar->Bind(wxEVT_LEFT_DCLICK, &CMainFrame::OnStatusBarDblClick, this);
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
			m_statusBar.Update(i, str);
		}
	}
#if wxUSE_DRAG_AND_DROP
	SetDropTarget(new CFileDropTarget(manager));
#endif
	m_UpdateInfo.Initialize();
}


CMainFrame::~CMainFrame()
{
}


bool CMainFrame::DownloadInProgress()
{
	return m_UpdateInfo.DownloadInProgress();
}


bool CMainFrame::CanClose()
{
	return m_UpdateInfo.CanClose(this, true);
}


void CMainFrame::SetMessageText(std::wstring const& msg, bool bFiltered)
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


// Note: This is only called from CAgilityBookDoc::OnOpenDocument
void CMainFrame::AutoCheckConfiguration(CAgilityBookDoc* pDoc)
{
	m_UpdateInfo.AutoCheckConfiguration(pDoc);
}


// Note: Called from OnHelpCheckUpdates and CDlgAbout::OnCheckForUpdates
void CMainFrame::UpdateConfiguration(CAgilityBookDoc* pDoc, bool& outDownloadStarted)
{
	m_UpdateInfo.UpdateConfiguration(pDoc, outDownloadStarted);
}


void CMainFrame::SetMessage(std::wstring const& msg, int index, bool bResize)
{
	wxStatusBar* statusbar = GetStatusBar();
	if (!statusbar)
		return;
	wxString str = StringUtil::stringWX(msg);
	if (bResize)
		m_statusBar.Update(index, str);
	else
		SetStatusText(str, index);
}


void CMainFrame::OnStatusBarContextMenu(wxContextMenuEvent& evt)
{
	CAgilityBookDoc* pDoc = wxDynamicCast(GetDocumentManager()->GetCurrentDocument(), CAgilityBookDoc);
	assert(pDoc);
	wxStatusBar* statusbar = nullptr;
	wxPoint point;
	auto id = m_statusBar.GetContextMenuFieldId(evt, statusbar, point);
	switch (id)
	{
	default:
		evt.Skip();
		break;
	case STATUS_DOG:
	case STATUS_STATUS:
	case STATUS_FILTERED:
		if (!pDoc->StatusBarContextMenu(statusbar, id, point))
			evt.Skip();
		break;
	}
}


void CMainFrame::OnStatusBarDblClick(wxMouseEvent& evt)
{
	CAgilityBookDoc* pDoc = wxDynamicCast(GetDocumentManager()->GetCurrentDocument(), CAgilityBookDoc);
	assert(pDoc);
	wxCommandEvent evtCmd(wxEVT_MENU, wxID_PREFERENCES);
	switch (m_statusBar.GetDoubleClickFieldId(evt))
	{
	default:
		evt.Skip();
		break;
	case STATUS_FILTERED:
		evtCmd.SetEventObject(this);
		pDoc->ProcessEvent(evtCmd);
		break;
	}
}


#ifdef EVT_DPI_CHANGED
void CMainFrame::OnDPIChanged(wxDPIChangedEvent& evt)
{
#pragma PRAGMA_TODO(per - monitor rendering)
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
	// Note: CancelDownload will return false is no request is active.
	if (evt.CanVeto() && m_UpdateInfo.CancelDownload())
	{
		// When m_UpdateInfo is notified, it will call wxApp().GetTopWindow()->Close()
		SetStatusText(_("Canceling file download..."), 0);
		evt.Veto();
		return;
	}
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
		CAgilityBookBaseExtraView* pView
			= wxDynamicCast(GetDocumentManager()->GetCurrentView(), CAgilityBookBaseExtraView);
		if (pView)
			bEnable = pView->HasNextPane();
	}
	break;
	case ID_PREV_PANE:
	{
		CAgilityBookBaseExtraView* pView
			= wxDynamicCast(GetDocumentManager()->GetCurrentView(), CAgilityBookBaseExtraView);
		if (pView)
			bEnable = pView->HasPrevPane();
	}
	break;
	case ID_VIEW_CUSTOMIZE_ACCEL:
	case ID_HELP_SYSINFO:
		bEnable = true;
		break;
	case ID_HELP_UPDATE:
		bEnable = !m_UpdateInfo.DownloadInProgress();
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


void CMainFrame::OnViewCustomizeAccel(wxCommandEvent& evt)
{
	wxGetApp().GetMenus().ConfigureAccelerators(this);
}


void CMainFrame::OnHelpCheckUpdates(wxCommandEvent& evt)
{
	bool close = false;
	CAgilityBookDoc* pDoc = wxDynamicCast(GetDocumentManager()->GetCurrentDocument(), CAgilityBookDoc);
	UpdateConfiguration(pDoc, close);
	// App will auto-close when download is done.
}


void CMainFrame::OnHelpSysinfo(wxCommandEvent& evt)
{
	CVersionNum ver(ARB_VER_MAJOR, ARB_VER_MINOR, ARB_VER_DOT, ARB_VER_BUILD);
	std::wstring str = ARBDebug::GetSystemInfo(this, ver);
	CDlgMessage dlg(str, this);
	dlg.ShowModal();
}


void CMainFrame::OnHelpAbout(wxCommandEvent& evt)
{
	CAgilityBookDoc* pDoc = wxDynamicCast(GetDocumentManager()->GetCurrentDocument(), CAgilityBookDoc);
	CDlgAbout dlg(pDoc, this);
	dlg.ShowModal();
}
