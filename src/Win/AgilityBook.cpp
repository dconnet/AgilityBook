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
 * 2018-01-28 Add debug reporting.
 * 2014-07-08 Properly cleanup on early exit.
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
#include "ImageHelper.h"
#include "MainFrm.h"
#include "Print.h"
#include "RegItems.h"
#include "SetupAppARB.h"
#include "TabView.h"
#include "VersionNumber.h"

#include "ARBCommon/ARBMisc.h"
#include "ARBCommon/ARBUtils.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ARBDebug.h"
#include "LibARBWin/DPI.h"
#include "LibARBWin/ReportListCtrl.h"
#include "LibARBWin/SetupApp.h"
#include <wx/choicdlg.h>
#include <wx/cmdline.h>
#include <wx/cmndata.h>
#include <wx/config.h>
#include <wx/display.h>
#include <wx/docview.h>
#include <wx/ffile.h>
#include <wx/file.h>
#include <wx/fileconf.h>
#include <wx/msgdlg.h>
#include <wx/settings.h>
#include <wx/stdpaths.h>
#include <wx/sysopt.h>
#include <wx/url.h>
#include <wx/utils.h>
#include <stdexcept>
#include <vector>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


wxIMPLEMENT_APP(CAgilityBookApp);

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookDocManager : public wxDocManager
{
	CAgilityBookDocManager() = delete;

public:
	CAgilityBookDocManager(size_t historySize);
	wxFileHistory* OnCreateFileHistory() override;
	void ActivateView(wxView* view, bool activate = true) override;

private:
	size_t m_History;

	DECLARE_EVENT_TABLE()
	void OnPrint(wxCommandEvent& evt);
	void OnPreview(wxCommandEvent& evt);
};


wxBEGIN_EVENT_TABLE(CAgilityBookDocManager, wxDocManager)
	EVT_MENU(wxID_PRINT, CAgilityBookDocManager::OnPrint)
	EVT_MENU(wxID_PREVIEW, CAgilityBookDocManager::OnPreview)
wxEND_EVENT_TABLE()


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


void CAgilityBookDocManager::ActivateView(wxView* view, bool activate)
{
	CTabView* pTabView = wxDynamicCast(view, CTabView);
	if (!pTabView)
		wxDocManager::ActivateView(view, activate);
}


void CAgilityBookDocManager::OnPrint(wxCommandEvent& evt)
{
	// Copied wxDocManager implementation so I could put my printer data in
	wxView* view = GetCurrentView();
	if (!view)
		return;
	wxPrintout* printout = view->OnCreatePrintout();
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
	wxView* view = GetCurrentView();
	if (!view)
		return;
	wxPrintout* printout = view->OnCreatePrintout();
	if (printout)
	{
		// Pass two printout objects: for preview, and possible printing.
		wxPrintPreviewBase* preview = new CPrintPreview(printout, view->OnCreatePrintout(), wxGetApp().GetPrintData());
		if (!preview->Ok())
		{
			delete preview;
			wxMessageBox(_("Sorry, print preview needs a printer to be installed."));
			return;
		}
		wxPreviewFrame* frame = new wxPreviewFrame(
			preview,
			wxGetApp().GetTopWindow(),
			_("Print Preview"),
			wxDefaultPosition,
			wxGetApp().GetTopWindow()->GetSize());
		frame->Centre(wxBOTH);
		frame->Initialize();
		frame->Show(true);
	}
}

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookDocTemplate : public wxDocTemplate
{
public:
	using wxDocTemplate::wxDocTemplate;

	bool FileMatchesTemplate(const wxString& path) override
	{
		return wxDocTemplate::FileMatchesTemplate(path);
	}
};

/////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(CAgilityBookApp, wxApp)
	EVT_QUERY_END_SESSION(CAgilityBookApp::OnQueryEndSession)
	EVT_END_SESSION(CAgilityBookApp::OnEndSession)
wxEND_EVENT_TABLE()


// eLanguageCatalogEmbedded will fallback to eLanguageCatalogDirectory
// on systems that don't support it.

CAgilityBookApp::CAgilityBookApp()
	: CBaseApp(ARB_CONFIG_ENTRY, ARBLanguageCatalog::Embedded)
	, m_bShutdownSocket(false)
	, m_Localization()
	, m_imageHelper()
	, m_manager(nullptr)
	, m_printDialogData(nullptr)
	, m_Prn(nullptr)
	, m_menus(nullptr)
{
	m_BaseInfoName = ARB_CONFIG_INFO;
}


void CAgilityBookApp::SetMessageText(std::wstring const& msg, bool bFiltered)
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


CMenuHelper& CAgilityBookApp::GetMenus()
{
	if (!m_menus)
		m_menus = CreateMenus();
	return *(m_menus.get());
}


bool CAgilityBookApp::OnInit()
{
	if (!CBaseApp::OnInit())
		return false;
	if (!wxSocketBase::Initialize())
	{
		BaseAppCleanup(true);
		return false;
	}
	m_bShutdownSocket = true;

	wxImage::AddHandler(new wxGIFHandler);
	wxImage::AddHandler(new wxPNGHandler);
	InitFSHandlers();

	wxCmdLineParser cmdline(argc, argv);
	cmdline.AddParam(_("Agility Record Book file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);

	if (0 != cmdline.Parse(false))
	{
		cmdline.Usage();
		BaseAppCleanup(true);
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

	CReportListCtrl::EnableRowColors(CAgilityBookOptions::UseAlternateRowColor());

	m_manager = std::make_unique<CAgilityBookDocManager>(CAgilityBookOptions::GetMRUFileCount());
	m_manager->SetMaxDocsOpen(1);
	{
		CConfigPathHelper config(CFG_KEY_RECENT_FILES);
		m_manager->FileHistoryLoad(*wxConfig::Get());
	}

	// Yes. Intentional. Magic happens.
	new CAgilityBookDocTemplate(
		m_manager.get(),
		L"ARB",
		L"*.arb",
		L"",
		L"arb",
		L"ARB Doc",
		L"ARB View",
		CLASSINFO(CAgilityBookDoc),
		CLASSINFO(CTabView));
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

	int x = wxDefaultCoord, y = wxDefaultCoord;
	if (wxConfig::Get()->Read(CFG_SETTINGS_LASTXPOS, &x, x))
		x = DPI::Scale(x);
	if (wxConfig::Get()->Read(CFG_SETTINGS_LASTYPOS, &y, y))
		y = DPI::Scale(y);

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

	// Determine where window will be. At least which screen.
	wxPoint curPt;
	wxRect rWorkSpace;
	if (bCompute)
	{
		curPt = wxPoint(mouseState.GetX(), mouseState.GetY());
		int display = wxDisplay::GetFromPoint(curPt);
		if (wxNOT_FOUND == display)
			display = 0; // If the display can't be found, use the primary.
		wxDisplay monitor(display);
		rWorkSpace = monitor.GetClientArea();
	}

	CMainFrame* frame = new CMainFrame(m_manager.get());
	int width = DPI::Scale(600);
	int height = DPI::Scale(400);
	int defWidth = width;
	int defHeight = height;
	if (wxConfig::Get()->Read(CFG_SETTINGS_LASTCX, &width, width))
		width = DPI::Scale(frame, width);
	if (wxConfig::Get()->Read(CFG_SETTINGS_LASTCY, &height, height))
		height = DPI::Scale(frame, height);
	long state = wxConfig::Get()->Read(CFG_SETTINGS_LASTSTATE, 0L);
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
		// This _shouldn't_ cause a change of screens...
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
	{
		CConfigPathHelper config(CFG_KEY_RECENT_FILES);
		m_manager->FileHistorySave(*wxConfig::Get());
	}
	m_manager.reset();
	delete m_printDialogData;
	m_printDialogData = nullptr;
	delete m_Prn;
	m_Prn = nullptr;
	return CBaseApp::OnExit();
}


#if USE_DBGREPORT
bool CAgilityBookApp::OnAddFileDebugReport(wxDebugReport* report)
{
	{
		wxFileName fn(report->GetDirectory(), L"ARBInfo.txt");
		wxFFile file(fn.GetFullPath(), L"w");
		if (file.IsOpened())
		{
			CVersionNum ver(ARB_VER_MAJOR, ARB_VER_MINOR, ARB_VER_DOT, ARB_VER_BUILD);
			wxDateTime dt = wxDateTime::Now();
			file.Write(dt.FormatISODate() + L' ' + dt.FormatISOTime());
			file.Write(L"\n\nSystem Information\n");
			file.Write(ARBDebug::GetSystemInfo(GetTopWindow(), ver));
			file.Write(L"\nRegistry dump\n");
			file.Write(ARBDebug::GetRegistryInfo());
		}
		report->AddFile(fn.GetFullName(), L"System information and registry dump of this application");
	}

	if (m_manager)
	{
		wxDocument* doc = m_manager->GetCurrentDocument();
		if (doc)
		{
			wxString filename = doc->GetFilename();
			if (!filename.empty())
			{
				wxFileName fn(filename);
				if (fn.FileExists())
				{
					report->AddFile(fn.GetFullPath(), L"Current ARB File");
				}
			}
		}
	}

	return true;
}
#endif


void CAgilityBookApp::BaseAppCleanup(bool deleteConfig)
{
	if (m_bShutdownSocket)
	{
		m_bShutdownSocket = false;
		wxSocketBase::Shutdown();
	}
	CBaseApp::BaseAppCleanup(deleteConfig);
}


wxLanguage CAgilityBookApp::OnGetLanguage() const
{
	wxLanguage lang = CurrentLanguageId();

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
			lang = static_cast<wxLanguage>(langInfo->Language);
	}

	// If we haven't saved a lang, and we're running on EN, don't prompt (by returning default)
	if (wxLANGUAGE_DEFAULT == lang)
	{
		wxLocale tmp(wxLANGUAGE_DEFAULT);
		if (wxLANGUAGE_ENGLISH_US == static_cast<wxLanguage>(tmp.GetLanguage()))
			lang = wxLANGUAGE_ENGLISH_US;
	}

	return lang;
}


wxString CAgilityBookApp::OnGetLangConfigName() const
{
	return CFG_SETTINGS_LANG3;
}


void CAgilityBookApp::OnSetLanguage(wxLanguage langId)
{
	if (!m_Localization.Load())
	{
		wxString str = wxString::Format(L"ERROR: Unable to load '%s.mo'.", OnGetCatalogName().c_str());
		wxMessageBox(str, wxMessageBoxCaptionStr, wxICON_ERROR | wxOK);
		throw std::runtime_error(str.ToStdString());
	}
}


bool CAgilityBookApp::OnCreateBitmap(const wxArtID& id, const wxArtClient& client, const wxSize& size, wxBitmap& outBmp)
{
	return m_imageHelper.DoCreateBitmap(GetTopWindow(), id, client, size, outBmp);
}


bool CAgilityBookApp::OnCreateIconBundle(const wxArtID& id, const wxArtClient& client, wxIconBundle& outIcon)
{
	return m_imageHelper.DoCreateIconBundle(GetTopWindow(), id, client, outIcon);
}


bool CAgilityBookApp::InitLanguage()
{
	IARBLocalization::Init(&m_Localization);
	return CBaseApp::InitLanguage();
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
