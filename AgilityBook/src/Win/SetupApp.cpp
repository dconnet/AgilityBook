/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * Revision History
 * 2014-11-16 Add support to language initialization for embedding MO files.
 * 2014-07-08 Cleanup config if intialization fails.
 * 2013-11-26 Fixed language initialization structure.
 * 2013-08-22 Fixed issue with ctor auto-cast.
 * 2013-08-17 Added local config file support.
 * 2013-04-10 Add wxLocale default initialization.
 * 2012-08-13 Moved dconSetupApp out of Globals.cpp
 */

#include "stdafx.h"
#include "SetupApp.h"

#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "Globals.h"
#include "ImageManager.h"
#include "LanguageManager.h"

#include <wx/config.h>
#include <wx/dir.h>
#include <wx/fileconf.h>
#include <wx/stdpaths.h>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


CBaseApp::CBaseApp(
		wxString const& appName,
		wxString const& appRegKey,
		LanguageCatalog useLangCatalog)
	: m_VendorName(wxT("dcon Software"))
	, m_BaseAppName(appName)
	, m_BaseRegName(appRegKey)
	, m_BaseInfoName()
	, m_bReadOnlyInfo(false)
	, m_ConfigTest(wxT("Settings/isLocal"))
	, m_bFallback(true)
	, m_langMgr(nullptr)
{
	if (m_BaseRegName.empty())
		m_BaseRegName = m_BaseAppName;
	m_langMgr = new CLanguageManager(
			eLanguageCatalogNone != useLangCatalog ? this : nullptr,
			eLanguageCatalogEmbedded == useLangCatalog);
}


CBaseApp::~CBaseApp()
{
	delete m_langMgr;
}


bool CBaseApp::OnInit()
{
#if defined(__WXMSW__) && wxCHECK_VERSION(3, 0, 0)
	// By default, the path directories are tweaked to remove debug/release.
	// I assume my files are in the same location as the binary.
	// Now I don't need to tweak the wx source!
	wxStandardPaths::Get().DontIgnoreAppSubDir();
#endif

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
				m_BaseInfoName = fileName.GetName() + wxT(".info");
			}
			wxString inifile = wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + m_BaseInfoName;

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
						pBaseConfig = pConfig;
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

	bool rc = InitLocale();
	if (!rc && bConfigSet)
		BaseAppCleanup(true);
	return rc;
}


int CBaseApp::OnExit()
{
	BaseAppCleanup();
	return wxApp::OnExit();
}


void CBaseApp::BaseAppCleanup(bool deleteConfig)
{
	if (deleteConfig)
	{
		wxConfigBase* pOld = wxConfig::Set(nullptr);
		if (pOld)
			delete pOld;
	}
	delete m_langMgr;
	m_langMgr = nullptr;

	CImageManager::Get()->SetCallback(nullptr);
	Element::Terminate();
}


bool CBaseApp::SelectLanguage(wxWindow* parent)
{
	return m_langMgr->SelectLanguage(parent);
}


std::wstring CBaseApp::CurrentLanguage() const
{
	return m_langMgr->CurrentLanguage();
}


int CBaseApp::CurrentLanguageId() const
{
	return m_langMgr->CurrentLanguageId();
}


int CBaseApp::OnGetLanguage() const
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


void CBaseApp::OnSetLanguage(int langId)
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


bool CBaseApp::InitLocale()
{
	return m_langMgr->InitLocale();
}


int CBaseApp::SelectLang(wxWindow* parent)
{
	return m_langMgr->SelectLang(parent);
}
