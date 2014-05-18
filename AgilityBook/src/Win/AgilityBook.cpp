/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Defines the class behaviors for the application.
 * @author David Connet
 *
 * Revision History
 * 2014-05-18 Moved startup check to AutoCheckProgram
 * 2013-11-26 Fixed language initialization structure.
 * 2013-08-16 Add support for standalone app (.info file) on Windows.
 * 2012-12-15 Prevent TabView from getting activation.
 * 2012-08-28 Rework how wxCmdLineParser is initialized.
 * 2012-07-27 Disable spell checking on OSX 10.8. It crashes.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-08-26 Fix file autoload failure so it opens new document.
 * 2008-12-14 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-10-19 Fixed a problem with CFile::GetStatus (see AgilityBook.cpp).
 * 2005-02-10 There was a problem initializing RICHED*.DLL on Win98.
 * 2004-07-20 Changed the updating to auto-update configurations also.
 *            Moved the user-request updates to the document.
 * 2004-06-16 Changed ARBDate::GetString to put leadingzero into format.
 * 2004-06-02 Moved ShellExecute code here.
 * 2004-03-05 Added check-for-updates feature.
 * 2003-12-07 When opening the last opened file fails, open a new doc.
 */

#include "stdafx.h"
#include "AgilityBook.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "MainFrm.h"
#include "Print.h"
#include "RegItems.h"
#include "SetupApp.h"
#include "SetupAppARB.h"
#include "TabView.h"

#include "ARBCommon/ARBMisc.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include <stdexcept>
#include <vector>
#include <wx/choicdlg.h>
#include <wx/cmdline.h>
#include <wx/cmndata.h>
#include <wx/config.h>
#include <wx/display.h>
#include <wx/docview.h>
#include <wx/file.h>
#include <wx/fileconf.h>
#include <wx/fs_arc.h>
#include <wx/msgdlg.h>
#include <wx/settings.h>
#include <wx/stdpaths.h>
#include <wx/sysopt.h>
#include <wx/url.h>
#include <wx/utils.h>

#include "res/AgilityBook48.xpm"
#include "res/AgilityBook256.xpm"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


IMPLEMENT_APP(CAgilityBookApp)

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookDocManager : public wxDocManager
{
	CAgilityBookDocManager(); // not implemented
public:
	CAgilityBookDocManager(size_t historySize);
	virtual wxFileHistory* OnCreateFileHistory();
	virtual void ActivateView(wxView *view, bool activate = true);
private:
	size_t m_History;

	DECLARE_EVENT_TABLE()
	void OnPrint(wxCommandEvent& evt);
	void OnPreview(wxCommandEvent& evt);
};


BEGIN_EVENT_TABLE(CAgilityBookDocManager, wxDocManager)
	EVT_MENU(wxID_PRINT, CAgilityBookDocManager::OnPrint)
	EVT_MENU(wxID_PREVIEW, CAgilityBookDocManager::OnPreview)
END_EVENT_TABLE()


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


void CAgilityBookDocManager::ActivateView(wxView *view, bool activate)
{
	CTabView* pTabView = wxDynamicCast(view, CTabView);
	if (!pTabView)
		wxDocManager::ActivateView(view, activate);
}


void CAgilityBookDocManager::OnPrint(wxCommandEvent& evt)
{
	// Copied wxDocManager implementation so I could put my printer data in
	wxView *view = GetCurrentView();
	if (!view)
		return;
	wxPrintout *printout = view->OnCreatePrintout();
	if (printout)
	{
		wxPrinter printer(wxGetApp().GetPrintData());
		printer.Print(view->GetFrame(), printout, true);
		wxGetApp().SavePrintData(printer.GetPrintDialogData());
		delete printout;
	}
}


void CAgilityBookDocManager::OnPreview(wxCommandEvent& evt)
{
	// Copied wxDocManager implementation so I could put my printer data in
	wxView *view = GetCurrentView();
	if (!view)
		return;
	wxPrintout *printout = view->OnCreatePrintout();
	if (printout)
	{
		// Pass two printout objects: for preview, and possible printing.
		wxPrintPreviewBase *preview = new CPrintPreview(printout, view->OnCreatePrintout(), wxGetApp().GetPrintData());
		if (!preview->Ok())
		{
			delete preview;
			wxMessageBox(_("Sorry, print preview needs a printer to be installed."));
			return;
		}
		wxPreviewFrame *frame = new wxPreviewFrame(preview, wxGetApp().GetTopWindow(), _("Print Preview"), wxDefaultPosition, wxGetApp().GetTopWindow()->GetSize());
		frame->Centre(wxBOTH);
		frame->Initialize();
		frame->Show(true);
	}
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(CAgilityBookApp, wxApp)
	EVT_QUERY_END_SESSION(CAgilityBookApp::OnQueryEndSession)
	EVT_END_SESSION(CAgilityBookApp::OnEndSession)
END_EVENT_TABLE()


CAgilityBookApp::CAgilityBookApp()
	: CBaseApp(ARB_CONFIG_ENTRY, wxEmptyString, true)
	, m_bShutdownSocket(false)
	, m_Localization()
	, m_UpdateInfo()
	, m_manager(nullptr)
	, m_printDialogData(nullptr)
	, m_Prn(nullptr)
{
	m_BaseInfoName = ARB_CONFIG_INFO;
}


bool CAgilityBookApp::AutoCheckProgram()
{
	bool bClose = false;
	// Check for updates every 30 days.
	if (CAgilityBookOptions::GetAutoUpdateCheck())
	{
		std::wstring ver = StringUtil::stringW(wxConfig::Get()->Read(CFG_SETTINGS_LASTVERCHECK, L""));
		ARBDate date = ARBDate::FromString(ver, ARBDate::eISO);
		if (date.IsValid())
		{
			ARBDate today = ARBDate::Today();
			date += 30;
			if (date < today)
			{
				m_UpdateInfo.AutoUpdateProgram(nullptr, bClose);
			}
		}
	}
	return bClose;
}


void CAgilityBookApp::AutoCheckConfiguration(CAgilityBookDoc* pDoc)
{
	m_UpdateInfo.AutoCheckConfiguration(pDoc);
}


void CAgilityBookApp::UpdateConfiguration(
		CAgilityBookDoc* pDoc,
		bool& outClose)
{
	m_UpdateInfo.UpdateConfiguration(pDoc, outClose);
}


void CAgilityBookApp::SetMessageText(
		std::wstring const& msg,
		bool bFiltered)
{
	CMainFrame* pFrame = wxDynamicCast(GetTopWindow(), CMainFrame);
	if (pFrame)
	{
		pFrame->SetMessageText(msg, bFiltered);
	}
}


void CAgilityBookApp::SetMessageText2(std::wstring const& msg)
{
	CMainFrame* pFrame = wxDynamicCast(GetTopWindow(), CMainFrame);
	if (pFrame)
	{
		pFrame->SetMessageText2(msg);
	}
}


wxPrintDialogData* CAgilityBookApp::GetPrintData()
{
	if (!m_printDialogData)
	{
		m_printDialogData = new wxPrintDialogData();
		bool val = true;
		wxConfig::Get()->Read(CFG_SETTINGS_PRINTLAND, &val);
		m_printDialogData->GetPrintData().SetOrientation(val ? wxLANDSCAPE : wxPORTRAIT);
	}
	return m_printDialogData;
}


void CAgilityBookApp::SavePrintData(wxPrintDialogData const& data)
{
	assert(m_printDialogData);
	*m_printDialogData = data;
	bool val = (m_printDialogData->GetPrintData().GetOrientation() == wxLANDSCAPE);
	wxConfig::Get()->Write(CFG_SETTINGS_PRINTLAND, val);
}


CHtmlEasyPrinting* CAgilityBookApp::GetHtmlPrinter()
{
	if (!m_Prn)
		m_Prn = new CHtmlEasyPrinting(GetTopWindow());
	return m_Prn;
}


bool CAgilityBookApp::OnInit()
{
	if (!CBaseApp::OnInit())
		return false;
	if (!wxSocketBase::Initialize())
		return false;
	m_bShutdownSocket = true;

	// We need at least 800x600 (the event(run) dialog is big!)
	if (wxSYS_SCREEN_DESKTOP != wxSystemSettings::GetScreenType())
	{
		if (wxNO == wxMessageBox(_("IDS_MIN_RESOLUTION"), wxMessageBoxCaptionStr, wxYES_NO | wxCENTRE | wxICON_ERROR))
			return false;
	}

	wxImage::AddHandler(new wxGIFHandler);
	wxFileSystem::AddHandler(new wxArchiveFSHandler);

	wxCmdLineParser cmdline(argc, argv);
	cmdline.AddParam(_("Agility Record Book file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);

	if (0 != cmdline.Parse(false))
	{
		cmdline.Usage();
		BaseAppCleanup();
		return false;
	}

	wxString filename;
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
	wxConfig::Get()->SetPath(L"Recent File List"); // Named this way for compatibility with existing MFC app
	m_manager->FileHistoryLoad(*wxConfig::Get());
	wxConfig::Get()->SetPath(L"..");

	(void)new wxDocTemplate(m_manager, L"ARB", L"*.arb", L"", L"arb", L"ARB Doc", L"ARB View",
		CLASSINFO(CAgilityBookDoc), CLASSINFO(CTabView));
#ifdef __WXMAC__
#ifndef __WXOSX_COCOA__
	wxFileName::MacRegisterDefaultTypeAndCreator(L"arb", 'ARBB', 'ARBA');
#endif
	{
		bool useSpellChecker = true;
#ifndef __WXOSX_COCOA__
		int majVer;
		int minVer;
		GetOSInfo(majVer, minVer);
		if (majVer == 10 && minVer < 8)
		{
			useSpellChecker = false;
		}
#endif
		if (useSpellChecker)
		{
			wxSystemOptions::SetOption(wxMAC_TEXTCONTROL_USE_SPELL_CHECKER, 1);
		}
	}
	// Sorting is broken in the native sorting in wx 2.8.10 and earlier
	wxSystemOptions::SetOption(L"mac.listctrl.always_use_generic", 1);
#endif

	CMainFrame *frame = new CMainFrame(m_manager);

	int x, y, width, height;
	x = y = wxDefaultCoord;
	frame->GetSize(&width, &height);
	int defWidth = width;
	int defHeight = height;
	wxConfig::Get()->Read(CFG_SETTINGS_LASTXPOS, &x, x);
	wxConfig::Get()->Read(CFG_SETTINGS_LASTYPOS, &y, y);
	wxConfig::Get()->Read(CFG_SETTINGS_LASTCX, &width, width);
	wxConfig::Get()->Read(CFG_SETTINGS_LASTCY, &height, height);
	long state = wxConfig::Get()->Read(CFG_SETTINGS_LASTSTATE, 0L);
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

	// Should we open the last open file?
	if (filename.empty())
	{
		// Don't open it if the shift key is down.
		if (!::wxGetKeyState(WXK_SHIFT))
		{
			filename = wxConfig::Get()->Read(CFG_SETTINGS_LASTFILE, L"");
		}
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
	bool bNew = true;
	if (!filename.empty())
	{
		if (m_manager->CreateDocument(filename, wxDOC_SILENT))
			bNew = false;
	}
	if (bNew)
		m_manager->CreateDocument(wxEmptyString, wxDOC_NEW);

	if (0 < state)
		frame->Maximize();
	frame->Show(true);

	std::wstring proxy = CAgilityBookOptions::GetProxy();
	if (!proxy.empty())
		wxURL::SetDefaultProxy(proxy.c_str());

	// Cleanup ARBUpdater.exe from temp directory from previous auto-update.
	CUpdateInfo::CleanupUpdate();

	return true;
}


int CAgilityBookApp::OnExit()
{
	wxConfig::Get()->SetPath(L"Recent File List");
	m_manager->FileHistorySave(*wxConfig::Get());
	wxConfig::Get()->SetPath(L"..");
	delete m_manager;
	m_manager = nullptr;
	delete m_printDialogData;
	m_printDialogData = nullptr;
	delete m_Prn;
	m_Prn = nullptr;
	return CBaseApp::OnExit();
}


void CAgilityBookApp::BaseAppCleanup(bool deleteConfig)
{
	if (m_bShutdownSocket)
	{
		m_bShutdownSocket = false;
		wxSocketBase::Shutdown();
	}
	CBaseApp::BaseAppCleanup(deleteConfig);
}


int CAgilityBookApp::OnGetLanguage() const
{
	int lang = CurrentLanguageId();

	// Introduced in 2.1.
	wxString langStr = wxConfig::Get()->Read(OnGetLangConfigName(), wxEmptyString);
	if (langStr.empty())
	{
		// Introduced in 2.0 - turns out the wxLANGUAGE enum may change between releases.
		// (and did between 2.8 and 2.9)
		long lastLang;
		if (wxConfig::Get()->Read(CFG_SETTINGS_LANG2, &lastLang, 0) && 0 != lastLang)
		{
			// As of 2.0, we only supported 2 languages, so remapping is easy (whew!)
			if (58 == lastLang)
				langStr = L"en_US";
			else if (78 == lastLang)
				langStr = L"fr_FR";
		}
		else if (wxConfig::Get()->Read(CFG_SETTINGS_LANG, &lastLang, 0) && 0 != lastLang)
		{
			// Translates v1.10 registry
			if (0x0409 == lastLang)
				langStr = L"en_US";
			else if (0x040c == lastLang)
				langStr = L"fr_FR";
		}
	}
	if (!langStr.empty())
	{
		const wxLanguageInfo* langInfo = wxLocale::FindLanguageInfo(langStr);
		if (langInfo)
			lang = langInfo->Language;
	}

	return lang;
}


wxString CAgilityBookApp::OnGetLangConfigName() const
{
	return CFG_SETTINGS_LANG3;
}


void CAgilityBookApp::OnSetLanguage(int langId)
{
	if (!m_Localization.Load())
	{
		wxString str = wxString::Format(wxT("ERROR: Unable to load '%s.mo'."), OnGetCatalogName().c_str());
		wxMessageBox(str, wxMessageBoxCaptionStr, wxICON_ERROR | wxOK);
		std::string msg(str.ToAscii());
		throw std::runtime_error(msg);
	}
}


bool CAgilityBookApp::OnCreateBitmap(
		const wxArtID& id,
		const wxArtClient& client,
		const wxSize& size,
		wxBitmap& outBmp)
{
	return false;
}


bool CAgilityBookApp::OnCreateIconBundle(
		const wxArtID& id,
		const wxArtClient& client,
		wxIconBundle& outIcon)
{
	if (id == ImageMgrAppBundle)
	{
		outIcon.AddIcon(CImageManager::Get()->GetIcon(ImageMgrApp));
		outIcon.AddIcon(CImageManager::Get()->GetIcon(ImageMgrApp, wxART_MESSAGE_BOX));
		outIcon.AddIcon(wxIcon(AgilityBook48_xpm));
		outIcon.AddIcon(wxIcon(AgilityBook256_xpm));
		return true;
	}
	return false;
}


bool CAgilityBookApp::InitLocale()
{
	IARBLocalization::Init(&m_Localization);
	return CBaseApp::InitLocale();
}


void CAgilityBookApp::OnQueryEndSession(wxCloseEvent& evt)
{
	if (m_manager)
	{
		wxDocument* pDoc = m_manager->GetCurrentDocument();
		if (pDoc && pDoc->IsModified())
		{
			wxString name = pDoc->GetFilename();
			// If the doc has never been saved (hence has no filename),
			// let normal processing pop up a save dlg to block shutdown.
			if (name.empty() || !pDoc->GetDocumentSaved())
			{
				evt.Skip();
			}
		}
	}
}


void CAgilityBookApp::OnEndSession(wxCloseEvent& evt)
{
	if (m_manager)
	{
		wxDocument* pDoc = m_manager->GetCurrentDocument();
		if (pDoc && pDoc->IsModified())
		{
			wxString name = pDoc->GetFilename();
			if (!name.empty() && pDoc->GetDocumentSaved())
			{
				pDoc->OnSaveDocument(name);
			}
		}
	}
	// Pass control to wx so it shuts things down normally.
	evt.Skip();
}
