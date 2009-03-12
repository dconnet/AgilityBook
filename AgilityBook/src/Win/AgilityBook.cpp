/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
 */

/**
 * @file
 *
 * @brief Defines the class behaviors for the application.
 * @author David Connet
 *
 * Revision History
 * @li 2008-12-14 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-10-19 DRC Fixed a problem with CFile::GetStatus (see AgilityBook.cpp).
 * @li 2005-02-10 DRC There was a problem initializing RICHED*.DLL on Win98.
 * @li 2004-07-20 DRC Changed the updating to auto-update configurations also.
 *                    Moved the user-request updates to the document.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-06-02 DRC Moved ShellExecute code here.
 * @li 2004-03-05 DRC Added check-for-updates feature.
 * @li 2003-12-07 DRC When opening the last opened file fails, open a new doc.
 */

#include "stdafx.h"
#include "AgilityBook.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "Element.h"
#include "LanguageManager.h"
#include "MainFrm.h"
#include "TabView.h"
#include <vector>

#include <wx/choicdlg.h>
#include <wx/cmdline.h>
#include <wx/config.h>
#include <wx/display.h>
#include <wx/docview.h>
#include <wx/file.h>
#include <wx/filesys.h>
#include <wx/fs_zip.h>
#include <wx/msgdlg.h>
#include <wx/settings.h>
#include <wx/stdpaths.h>
#include <wx/utils.h>
#include <wx/version.h>
#include <wx/xrc/xmlres.h>	// XRC XML resources

#if !wxCHECK_VERSION(2, 8, 9)
// ARB was developed against v2.8.9 - anything earlier is not supported.
#error Unsupported wxWidget version
#endif


IMPLEMENT_APP(CAgilityBookApp)


BEGIN_EVENT_TABLE(CAgilityBookApp, wxApp)
	EVT_MENU(wxID_HELP_CONTENTS, CAgilityBookApp::OnHelpContents)
	EVT_MENU(wxID_HELP_INDEX, CAgilityBookApp::OnHelpIndex)
END_EVENT_TABLE()

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookDocManager : public wxDocManager
{
public:
	CAgilityBookDocManager(size_t historySize);
	virtual wxFileHistory* OnCreateFileHistory();
private:
	size_t m_History;
};


CAgilityBookDocManager::CAgilityBookDocManager(size_t historySize)
	: m_History(historySize)
{
	if (wxMAX_FILE_HISTORY < m_History)
		m_History = wxMAX_FILE_HISTORY;
}


wxFileHistory* CAgilityBookDocManager::OnCreateFileHistory()
{
	return new wxFileHistory(m_History);
}

/////////////////////////////////////////////////////////////////////////////

CAgilityBookApp::CAgilityBookApp()
	: m_LangMgr(NULL)
	, m_UpdateInfo()
	, m_manager(NULL)

{
}


bool CAgilityBookApp::SelectLanguage(wxWindow* parent)
{
	assert(m_LangMgr);
	return m_LangMgr->SelectLanguage(parent);
}


void CAgilityBookApp::AutoCheckConfiguration(CAgilityBookDoc* pDoc)
{
	assert(m_LangMgr);
	m_UpdateInfo.AutoCheckConfiguration(pDoc, *m_LangMgr);
}


void CAgilityBookApp::UpdateConfiguration(CAgilityBookDoc* pDoc)
{
	assert(m_LangMgr);
	m_UpdateInfo.UpdateConfiguration(pDoc, *m_LangMgr);
}


void CAgilityBookApp::ShowHelp(wxString const& topic)
{
#pragma message PRAGMA_MESSAGE("TODO: handle help from modal dialogs - this won't work")
	if (topic.empty())
		m_LangMgr->HelpDisplayContents();
	else
		m_LangMgr->HelpDisplaySection(topic);
}


void CAgilityBookApp::SetMessageText(
		wxString const& msg,
		bool bFiltered)
{
	CMainFrame* pFrame = wxDynamicCast(GetTopWindow(), CMainFrame);
	if (pFrame)
	{
		pFrame->SetMessageText(msg, bFiltered);
	}
}


void CAgilityBookApp::SetMessageText2(wxString const& msg)
{
	CMainFrame* pFrame = wxDynamicCast(GetTopWindow(), CMainFrame);
	if (pFrame)
	{
		pFrame->SetMessageText2(msg);
	}
}


bool CAgilityBookApp::OnInit()
{
	tstring errMsg;
	if (!Element::Initialize(errMsg))
	{
		wxMessageBox(errMsg.c_str(), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_ERROR);
		return false;
	}

	// We need at least 800x600 (the event(run) dialog is big!)
	if (wxSYS_SCREEN_DESKTOP != wxSystemSettings::GetScreenType())
	{
		if (wxNO == wxMessageBox(_("IDS_MIN_RESOLUTION"), wxMessageBoxCaptionStr, wxYES_NO | wxCENTRE | wxICON_ERROR))
			return false;
	}

	SetAppName(wxT("Agility Record Book"));
	wxConfig::Set(new wxConfig(wxT("Agility Record Book"), wxT("dcon Software")));

	wxImage::AddHandler(new wxGIFHandler);
	wxFileSystem::AddHandler(new wxZipFSHandler);
	wxXmlResource::Get()->InitAllHandlers();
	m_LangMgr = new CLanguageManager();

	wxString filename;
	static const wxCmdLineEntryDesc cmdLineDesc[] =
	{
		{wxCMD_LINE_PARAM, NULL, NULL, wxT("input file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
		{wxCMD_LINE_NONE}
	};
	wxCmdLineParser cmdline(cmdLineDesc, argc, argv);
	if (0 != cmdline.Parse(false))
	{
		cmdline.Usage();
		Element::Terminate();
		delete m_LangMgr;
		m_LangMgr = NULL;
		return false;
	}
	if (0 < cmdline.GetParamCount())
	{
		filename = cmdline.GetParam(0);
		// Normalize filename - windows sometimes passes in short form
		wxFileName fName(filename);
		fName.Normalize(wxPATH_NORM_LONG | wxPATH_NORM_DOTS | wxPATH_NORM_TILDE | wxPATH_NORM_ABSOLUTE);
		filename = fName.GetFullPath();
	}

	m_manager = new CAgilityBookDocManager(CAgilityBookOptions::GetMRUFileCount());
	m_manager->SetMaxDocsOpen(1);
	wxConfig::Get()->SetPath(wxT("Recent File List")); // Named this way for compatibility with existing MFC app
	m_manager->FileHistoryLoad(*wxConfig::Get());
	wxConfig::Get()->SetPath(wxT(".."));

	(void)new wxDocTemplate(m_manager, wxT("ARB"), wxT("*.arb"), wxT(""), wxT("arb"), wxT("ARB Doc"), wxT("ARB View"),
		CLASSINFO(CAgilityBookDoc), CLASSINFO(CTabView));
#ifdef __WXMAC__
	wxFileName::MacRegisterDefaultTypeAndCreator(wxT("arb"), 'WXMB', 'WXMA');
#endif

	CMainFrame *frame = new CMainFrame(m_manager);

	int x, y, width, height;
	x = y = wxDefaultCoord;
	frame->GetSize(&width, &height);
	int defWidth = width;
	int defHeight = height;
	wxConfig::Get()->Read(wxT("Settings/lastXpos"), &x, x);
	wxConfig::Get()->Read(wxT("Settings/lastYpos"), &y, y);
	wxConfig::Get()->Read(wxT("Settings/lastCX"), &width, width);
	wxConfig::Get()->Read(wxT("Settings/lastCY"), &height, height);
	long state = wxConfig::Get()->Read(wxT("Settings/lastState"), 0L);
	bool bCompute = false;
	wxMouseState mouseState = ::wxGetMouseState();
	if (wxDefaultCoord != x)
	{
		bCompute = true;
		mouseState.SetX(x);
	}
	if (wxDefaultCoord != y)
	{
		bCompute = true;
		mouseState.SetY(y);
	}
	wxSize curSize(defWidth, defHeight);
	if (defWidth != width)
	{
		bCompute = true;
		curSize.SetWidth(width);
	}
	if (defHeight != height)
	{
		bCompute = true;
		curSize.SetHeight(height);
	}
	if (bCompute)
	{
		wxPoint curPt(mouseState.GetX(), mouseState.GetY());
		int display = wxDisplay::GetFromPoint(curPt);
		if (wxNOT_FOUND == display)
			display = 0; // If the display can't be found, use the primary.
		wxDisplay monitor(display);
		wxRect rWorkSpace = monitor.GetClientArea();
		wxRect rect(curPt, curSize);
		// Make sure window is not bigger.
		if (rect.GetWidth() > rWorkSpace.GetWidth())
			rect.SetRight(rect.GetLeft() + rWorkSpace.GetWidth());
		if (rect.GetHeight() > rWorkSpace.GetHeight())
			rect.SetBottom(rect.GetTop() + rWorkSpace.GetHeight());
		// Make sure the window is fully visible in the screen.
		if (!rWorkSpace.Contains(rect.GetTopLeft()))
		{
			if (rect.GetLeft() < rWorkSpace.GetLeft())
				rect.Offset(rWorkSpace.GetLeft() - rect.GetLeft(), 0);
			if (rect.GetTop() < rWorkSpace.GetTop())
				rect.Offset(0, rWorkSpace.GetTop() - rect.GetTop());
		}
		// Only check the bottom-right if the rect has size.
		if (!rect.IsEmpty() && !rWorkSpace.Contains(rect.GetBottomRight()))
		{
			if (rect.GetRight() > rWorkSpace.GetRight())
				rect.Offset(rWorkSpace.GetRight() - rect.GetRight(), 0);
			if (rect.GetBottom() > rWorkSpace.GetBottom())
				rect.Offset(0, rWorkSpace.GetBottom() - rect.GetBottom());
		}
		if (wxDefaultCoord != x)
			x = rect.GetLeft();
		if (wxDefaultCoord != y)
			y = rect.GetTop();
		if (defWidth != width)
			width = rect.GetWidth();
		if (defHeight != height)
			height = rect.GetHeight();
	}
	frame->SetSize(x, y, width, height);
	frame->DragAcceptFiles(true);

	SetTopWindow(frame);


	// Should we open the last open file?
	if (filename.empty())
	{
		// Don't open it if the shift key is down.
#ifdef WIN32
#pragma message PRAGMA_MESSAGE("TODO: Check how to see if shift key is down NOW")
		if (0 <= GetKeyState(VK_SHIFT))
		{
			filename = wxConfig::Get()->Read(wxT("Settings/LastFile"), wxT(""));
		}
#endif
	}
	// If a file is being opened, verify it exists first!
	// This catches both the case where the remembered last file (above)
	// doesn't exist and the case where the user types a command line where
	// it doesn't exist.
	if (!filename.empty())
	{
		if (!wxFile::Access(filename, wxFile::read))
		{
			filename.clear();
		}
	}
	if (filename.empty())
		m_manager->CreateDocument(wxEmptyString, wxDOC_NEW);
	else
		m_manager->CreateDocument(filename, wxDOC_SILENT);

	if (0 < state)
		frame->Maximize();
	frame->Show(true);

	// Check for updates every 30 days.
	if (CAgilityBookOptions::GetAutoUpdateCheck())
	{
		wxString ver = wxConfig::Get()->Read(wxT("Settings/lastVerCheck"), wxT(""));
		ARBDate date = ARBDate::FromString(ver.c_str(), ARBDate::eDashYYYYMMDD);
		if (date.IsValid())
		{
			ARBDate today = ARBDate::Today();
			date += 30;
			if (date < today)
				m_UpdateInfo.AutoUpdateProgram();
		}
	}

	return true;
}


int CAgilityBookApp::OnExit()
{
	wxConfig::Get()->SetPath(wxT("Recent File List"));
	m_manager->FileHistorySave(*wxConfig::Get());
	wxConfig::Get()->SetPath(wxT(".."));
	delete m_LangMgr;
	m_LangMgr = NULL;
	delete m_manager;
	m_manager = NULL;
	Element::Terminate();
	return wxApp::OnExit();
}


void CAgilityBookApp::OnHelpContents(wxCommandEvent& evt)
{
	m_LangMgr->HelpDisplayContents();
}


void CAgilityBookApp::OnHelpIndex(wxCommandEvent& evt)
{
	m_LangMgr->HelpDisplayIndex();
}
