/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * Revision History
 * 2019-08-16 Suppress PNG warning message.
 * 2019-01-01 Fix standalone detection.
 * 2018-10-11 Moved to Win LibARBWin
 * 2018-04-20 Use wxTranslations instead of wxLocale.
 * 2018-01-28 Add debug reporting.
 * 2014-11-16 Add support to language initialization for embedding MO files.
 * 2014-07-08 Cleanup config if intialization fails.
 * 2013-11-26 Fixed language initialization structure.
 * 2013-08-22 Fixed issue with ctor auto-cast.
 * 2013-08-17 Added local config file support.
 * 2013-04-10 Add wxLocale default initialization.
 * 2012-08-13 Moved dconSetupApp out of Globals.cpp
 */

#include "stdafx.h"
#include "LibARBWin/SetupApp.h"

#include "ARBCommon/ARBUtils.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ARBWinUtilities.h"
#include "LibARBWin/ImageManager.h"
#include "LibARBWin/LanguageManager.h"

#include <wx/config.h>
#include <wx/dir.h>
#include <wx/ffile.h>
#include <wx/fileconf.h>
#include <wx/fs_arc.h>
#include <wx/fs_mem.h>
#include <wx/stdpaths.h>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


CBaseApp::CBaseApp(
		wxString const& appName,
		wxString const& appRegKey,
		ARBLanguageCatalog useLangCatalog)
	: m_VendorName(L"dcon Software")
	, m_BaseAppName(appName)
	, m_BaseRegName(appRegKey)
	, m_BaseInfoName()
	, m_bReadOnlyInfo(false)
	, m_ConfigTest(L"Settings/isLocal")
	, m_bFallback(true)
	, m_bStandalone(false)
	, m_langMgr(nullptr)
{
	wxStandardPaths::Get().SetFileLayout(wxStandardPaths::FileLayout_XDG);

	if (m_BaseRegName.empty())
		m_BaseRegName = m_BaseAppName;
#if USE_DBGREPORT
	wxHandleFatalExceptions();
#endif
	m_langMgr = std::make_unique<CLanguageManager>(
			ARBLanguageCatalog::None != useLangCatalog ? this : nullptr,
			ARBLanguageCatalog::Embedded == useLangCatalog);
}


#if USE_DBGREPORT
class BaseAppDebugReport : public wxDebugReportCompress
{
	wxString m_reportName;
public:
	BaseAppDebugReport(wxString reportName) : m_reportName(reportName)
	{
	}

	wxString GetReportName() const override { return m_reportName; }
};


void CBaseApp::GenerateReport(wxDebugReport::Context ctx)
{
	BaseAppDebugReport report(GetReportName());

	// Add all standard files: currently this means just a minidump and an
	// XML file with system info and stack trace
	report.AddAll(ctx);

	// Allow derived apps to add additional files.
	if (!OnAddFileDebugReport(&report))
	{
		// Add a test file containing the date of the crash
		wxFileName fn(report.GetDirectory(), L"timestamp.my");
		wxFFile file(fn.GetFullPath(), L"w");
		if (file.IsOpened())
		{
			wxDateTime dt = wxDateTime::Now();
			file.Write(dt.FormatISODate() + L' ' + dt.FormatISOTime());
			file.Close();
		}
		report.AddFile(fn.GetFullName(), L"Timestamp of this report");
	}

	// Calling Show() is not mandatory, but is more polite
	if (wxDebugReportPreviewStd().Show(report))
	{
		if (report.Process())
		{
			wxLogMessage(_("Report generated in \"%s\"."), report.GetCompressedFileName().c_str());
			report.Reset();
		}
	}
	//else: user cancelled the report
}
#endif


bool CBaseApp::OnInit()
{
#if defined(__WXMSW__)
	// Remove current directory from DLL load path.
	SetDllDirectory(L"");

	// By default, the path directories are tweaked to remove debug/release.
	// I assume my files are in the same location as the binary.
	// Now I don't need to tweak the wx source!
	wxStandardPaths::Get().DontIgnoreAppSubDir();
#endif

    // Suppress PNG loading messages.
    // Specifically: "iCCP: known incorrect sRGB profile"
	wxImage::SetDefaultLoadFlags(wxImage::GetDefaultLoadFlags() & ~wxImage::Load_Verbose);

	std::wstring errMsg;
	if (!Element::Initialize(errMsg))
	{
		wxMessageBox(errMsg.c_str(), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_ERROR);
		return false;
	}

	CImageManager::Get()->SetCallback(this);

	bool bConfigSet = false;
	if (!m_BaseAppName.empty())
	{
		SetAppName(m_BaseAppName);
		if (!m_BaseRegName.empty())
		{
			if (m_BaseInfoName.empty())
			{
				wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
				m_BaseInfoName = fileName.GetName() + L".info";
			}
			wxString inifile = GetARBResourceDir() + wxFileName::GetPathSeparator() + m_BaseInfoName;

			// Important: If this is a dialog-based app, you must delete
			// the config before returning from OnInit for this will leak.
			wxConfigBase* pBaseConfig = nullptr;

			// Determine if this is a stand-alone execution.
			if (wxFile::Exists(inifile))
			{
				wxLogNull suppress;
				wxFileConfig* pConfig = new wxFileConfig(m_BaseRegName, m_VendorName, inifile, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
				if (!m_bReadOnlyInfo)
				{
					pConfig->Write(m_ConfigTest, true); // Write-only value to test info file viability
					if (pConfig->Flush())
					{
						pBaseConfig = pConfig;
						m_bStandalone = true;
					}
					else
					{
						// If Flush failed, we can't write the file. Probably readonly.
						// Why? (attrib/ProgramFiles/etc) Don't care. Just fall back.
						delete pConfig;
						if (!m_bFallback)
							return false;
					}
				}
			}

			if (!pBaseConfig)
				pBaseConfig = new wxConfig(m_BaseRegName, m_VendorName);
			wxConfig::Set(pBaseConfig);
			bConfigSet = true;
		}
	}

	bool rc = InitLanguage();
	if (!rc && bConfigSet)
		BaseAppCleanup(true);
	return rc;
}


int CBaseApp::OnExit()
{
	BaseAppCleanup();
	return wxApp::OnExit();
}


#if USE_DBGREPORT
void CBaseApp::OnFatalException()
{
	GenerateReport(wxDebugReport::Context_Exception);
}
#endif


void CBaseApp::BaseAppCleanup(bool deleteConfig)
{
	if (deleteConfig)
	{
		wxConfigBase* pOld = wxConfig::Set(nullptr);
		if (pOld)
			delete pOld;
	}
	m_langMgr.reset();

	CImageManager::Get()->SetCallback(nullptr);
	Element::Terminate();
}


void CBaseApp::InitFSHandlers()
{
	wxFileSystem::AddHandler(new wxArchiveFSHandler);
	wxFileSystem::AddHandler(new wxMemoryFSHandler);
}


bool CBaseApp::SelectLanguage(wxWindow* parent)
{
	return m_langMgr->SelectLanguage(parent);
}


std::wstring CBaseApp::CurrentLanguage() const
{
	return m_langMgr->CurrentLanguage();
}


wxLanguage CBaseApp::CurrentLanguageId() const
{
	return m_langMgr->CurrentLanguageId();
}


wxLanguage CBaseApp::OnGetLanguage() const
{
	return m_langMgr->GetDefaultLanguage();
}


wxString CBaseApp::OnGetCatalogName() const
{
	return m_langMgr->GetDefaultCatalogName();
}


wxString CBaseApp::OnGetLangConfigName() const
{
	return wxEmptyString;
}


wxString CBaseApp::OnGetLanguageDir() const
{
	return m_langMgr->GetDefaultLanguageDir();
}


void CBaseApp::OnSetLanguage(wxLanguage langId)
{
}


void CBaseApp::OnErrorMessage(wxString const& msg) const
{
	wxMessageBox(msg, wxMessageBoxCaptionStr, wxICON_ERROR | wxOK);
}


bool CBaseApp::OnCreateBitmap(
		const wxArtID& id,
		const wxArtClient& client,
		const wxSize& size,
		wxBitmap& outBmp)
{
	return false;
}


bool CBaseApp::OnCreateIconBundle(
		const wxArtID& id,
		const wxArtClient& client,
		wxIconBundle& outIcon)
{
	return false;
}


bool CBaseApp::InitLanguage()
{
	return m_langMgr->InitLanguage();
}


int CBaseApp::SelectLang(wxWindow* parent)
{
	return m_langMgr->SelectLang(parent);
}
