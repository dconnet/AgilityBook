/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CMainFrame class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-07-25 DRC Fix status width on Mac.
 * @li 2009-01-06 DRC Ported to wxWidgets.
 * @li 2008-11-19 DRC Added context menus to status bar.
 * @li 2006-09-01 DRC Added multi-monitor support.
 * @li 2003-05-18 DRC Obsoleted registry settings "Calendar"/"List" (bool)
 *                    "Settings"/"View" (int) changed (see TabView.cpp)
 */

#include "stdafx.h"
#include "MainFrm.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookMenu.h"
#include "CommonView.h"
#include "DlgMessage.h"
#include "Globals.h"
#include "PointsData.h"
#include "Print.h"
#include "RegItems.h"
#include "TabView.h"
#include <wx/config.h>
#include <wx/platinfo.h>
#include <wx/stdpaths.h>
#include <wx/version.h>

#include "res/AgilityBook16.xpm"
#include "res/AgilityBook32.xpm"
#include "res/AgilityBook48.xpm"
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


BEGIN_EVENT_TABLE(CMainFrame, wxDocParentFrame)
	EVT_CLOSE(CMainFrame::OnClose)
	EVT_UPDATE_UI(wxID_DUPLICATE, CMainFrame::OnUpdateCmd)
	EVT_UPDATE_UI(wxID_CUT, CMainFrame::OnUpdateCmd)
	EVT_UPDATE_UI(wxID_COPY, CMainFrame::OnUpdateCmd)
	EVT_UPDATE_UI(wxID_PASTE, CMainFrame::OnUpdateCmd)
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


static const struct
{
	int id;
	wxChar const* label;
	wxChar const* shortHelp;
	const char** bitmap;
} sc_Toolbar[] =
{
	{wxID_NEW, wxT("MenuFileNew"), wxT("DescFileNew"), toolbarNew_xpm},
	{wxID_OPEN, wxT("MenuFileOpen"), wxT("DescFileOpen"), toolbarOpen_xpm},
	{wxID_SAVE, wxT("MenuFileSave"), wxT("DescFileSave"), toolbarSave_xpm},
	{0, NULL, NULL, NULL},
	{ID_AGILITY_NEW_DOG, wxT("MenuNew"), wxT("DescDogNew"), toolbarDog_xpm},
	{ID_AGILITY_NEW_TITLE, wxT("MenuNew"), wxT("DescTitleNew"), toolbarTitle_xpm},
	{ID_AGILITY_NEW_TRIAL, wxT("MenuNew"), wxT("DescTrialNew"), toolbarTrial_xpm},
	{ID_AGILITY_NEW_RUN, wxT("MenuNew"), wxT("DescRunNew"), toolbarRun_xpm},
	{ID_AGILITY_NEW_CALENDAR, wxT("MenuNew"), wxT("DescCalendarNew"), toolbarCalendar_xpm},
	{ID_AGILITY_NEW_TRAINING, wxT("MenuNew"), wxT("DescTrainingNew"), toolbarTraining_xpm},
	{0, NULL, NULL, NULL},
	{wxID_CUT, wxT("MenuEditCut"), wxT("DescEditCut"), toolbarCut_xpm},
	{wxID_COPY, wxT("MenuEditCopy"), wxT("DescEditCopy"), toolbarCopy_xpm},
	{wxID_PASTE, wxT("MenuEditPaste"), wxT("DescEditPaste"), toolbarPaste_xpm},
	{0, NULL, NULL, NULL},
	{wxID_PREVIEW, wxT("MenuFilePrintPreview"), wxT("DescFilePrintPreview"), toolbarPreview_xpm},
	{wxID_PRINT, wxT("MenuFilePrint"), wxT("DescFilePrint"), toolbarPrint_xpm},
	{0, NULL, NULL, NULL},
	{wxID_ABOUT, wxT("MenuHelpAbout"), wxT("DescHelpAbout"), toolbarAbout_xpm},
};
static const int sc_ToolbarSize = sizeof(sc_Toolbar) / sizeof(sc_Toolbar[0]);

/////////////////////////////////////////////////////////////////////////////

static void SetStatusBarWidths(
		wxStatusBar* statusbar,
		int col,
		int* widths)
{
	statusbar->SetStatusWidths(NUM_STATUS_FIELDS, widths);
#if defined(__WXMAC__)
	// On the Mac, setting the width is always a bit small.
	// For instance, we want 36, but it gets set to 32.
	// So kludge it and force it larger.
	for (int i = 0; i < NUM_STATUS_FIELDS; ++i)
	{
		if ((0 > col || i == col) && 0 < widths[i])
		{
			wxRect r;
			statusbar->GetFieldRect(i, r);
			if (r.width < widths[i])
			{
				widths[i] += 2 * (widths[i] - r.width);
				statusbar->SetStatusWidths(NUM_STATUS_FIELDS, widths);
			}
		}
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////

CMainFrame::CMainFrame(wxDocManager* manager)
	: wxDocParentFrame(manager, NULL, wxID_ANY, _("Agility Record Book"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE)
	, m_MenuBar()
{
	wxIconBundle icons;
	icons.AddIcon(wxIcon(AgilityBook16_xpm));
	icons.AddIcon(wxIcon(AgilityBook32_xpm));
	icons.AddIcon(wxIcon(AgilityBook48_xpm));
	SetIcons(icons);
//#if wxUSE_HELP
// Note: do not set help on the frame. Help will disable the min/max buttons.
//	SetExtraStyle(wxFRAME_EX_CONTEXTHELP);
//#endif

	m_MenuBar.CreateMenu(this, manager);

	wxToolBar* toolbar = CreateToolBar(wxTB_FLAT);
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

	wxStatusBar* statusbar = CreateStatusBar(NUM_STATUS_FIELDS);
	if (statusbar)
	{
#ifdef WIN32
		m_fontStatusBar = statusbar->GetFont();
		statusbar->SetFont(m_fontStatusBar);
#endif
		statusbar->Connect(wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(CMainFrame::OnStatusBarContextMenu), NULL, this);
		wxClientDC dc(statusbar);
		dc.SetFont(statusbar->GetFont());
		int style[NUM_STATUS_FIELDS];
		m_Widths[0] = -1;
		style[0] = wxSB_FLAT;
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
#if !wxCHECK_VERSION(2, 9, 0)
			case STATUS_FILLER:
				str = wxT("   "); // Filler for where the grabber is
				break;
#endif
			}
			m_Widths[i] = dc.GetTextExtent(str).x;
#if !wxCHECK_VERSION(2, 9, 0)
			m_Widths[i] += 4;
#endif
			style[i] = wxSB_NORMAL;
			SetStatusText(str, i);
		}
		SetStatusBarWidths(statusbar, -1, m_Widths);
		statusbar->SetStatusStyles(NUM_STATUS_FIELDS, style);
	}
}


CMainFrame::~CMainFrame()
{
}


void CMainFrame::SetMessageText(
		wxString const& msg,
		bool bFiltered)
{
	SetMessage(msg, STATUS_STATUS, true);
	wxString filtered;
	if (bFiltered)
		filtered = _("ID_INDICATOR_FILTERED");
	SetMessage(filtered, STATUS_FILTERED, false);
}


void CMainFrame::SetMessageText2(wxString const& msg)
{
	SetMessage(msg, STATUS_DOG, true);
}


void CMainFrame::SetMessage(wxString const& msg, int index, bool bResize)
{
	wxStatusBar* statusbar = GetStatusBar();
	if (!statusbar)
		return;
	if (bResize)
	{
		wxClientDC dc(statusbar);
		dc.SetFont(statusbar->GetFont());
		m_Widths[index] = dc.GetTextExtent(msg).x;
#if !wxCHECK_VERSION(2, 9, 0)
		m_Widths[index] += 4;
#endif
		SetStatusBarWidths(statusbar, index, m_Widths);
	}
	SetStatusText(msg, index);
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
		static const int ids[NUM_STATUS_FIELDS-1] = {STATUS_DOG, STATUS_STATUS, STATUS_FILTERED};
		CAgilityBookDoc* pDoc = wxDynamicCast(GetDocumentManager()->GetCurrentDocument(), CAgilityBookDoc);
		assert(pDoc);
		if (pDoc)
		{
			for (int i = 0; i < NUM_STATUS_FIELDS - 1; ++i)
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
		wxConfig::Get()->Write(CFG_SETTINGS_LASTXPOS, r.x);
		wxConfig::Get()->Write(CFG_SETTINGS_LASTYPOS, r.y);
		wxConfig::Get()->Write(CFG_SETTINGS_LASTCX, r.width);
		wxConfig::Get()->Write(CFG_SETTINGS_LASTCY, r.height);
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
			bEnable = true;
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
			GetDocumentManager()->GetCurrentDocument()->UpdateAllViews(NULL, &hint);
		}
		m_MenuBar.UpdateMenu();
		wxToolBar* toolbar = GetToolBar();
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

		CAgilityBookBaseView* pView = wxDynamicCast(GetDocumentManager()->GetCurrentView(), CAgilityBookBaseView);
		wxString msg;
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
	PrintRuns(NULL, ARBDogPtr(), runs);
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
	wxString str;

	// OS version
	wxPlatformInfo info;
	str << wxT("OS: ")
		<< info.GetOperatingSystemIdName()
		<< wxT(" ")
		<< info.GetOSMajorVersion()
		<< wxT(".")
		<< info.GetOSMinorVersion()
		<< wxT("\n");
	if (wxPORT_BASE != info.GetPortId())
	{
		str << wxT("Port: ")
			<< info.GetPortIdName()
			<< wxT(" ")
			<< info.GetToolkitMajorVersion()
			<< wxT(".")
			<< info.GetToolkitMinorVersion()
			<< wxT("\n");
	}
	str << wxT("Architecture: ")
		<< info.GetArchName()
		<< wxT(", ")
		<< info.GetEndiannessName()
		<< wxT("\n");

	// Me.
	{
		CVersionNum ver(true);
		str << wxStandardPaths::Get().GetExecutablePath() << wxT(": ");
		if (ver.Valid())
			str << ver.GetVersionString();
		else
			str << _("IDS_BAD_VERSION");
		str << wxT("\n");
	}

	// wxWidgets
	str << wxVERSION_STRING << wxT("\n");

	wxString msg(str);
	CDlgMessage dlg(msg, this);
	dlg.ShowModal();
}
