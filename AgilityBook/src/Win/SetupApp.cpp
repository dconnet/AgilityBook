/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * Revision History
 * @li 2013-08-17 DRC Added local config file support.
 * @li 2013-04-10 DRC Add wxLocale default initialization.
 * @li 2012-08-13 DRC Moved dconSetupApp out of Globals.cpp
 */

#include "stdafx.h"
#include "SetupApp.h"

#include "ARBCommon/Element.h"
#include "Globals.h"
#include "ImageManager.h"

#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/stdpaths.h>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


CBaseApp::CBaseApp(wxString const& appName)
	: m_VendorName(wxT("dcon Software"))
	, m_BaseAppName(appName)
	, m_BaseRegName(appName)
	, m_BaseInfoName()
	, m_bReadOnlyInfo(false)
	, m_ConfigTest(wxT("Settings/isLocal"))
	, m_bFallback(true)
	, m_locale(NULL)
{
}


CBaseApp::CBaseApp(
		wxString const& appName,
		wxString const& appRegKey)
	: m_VendorName(wxT("dcon Software"))
	, m_BaseAppName(appName)
	, m_BaseRegName(appRegKey)
	, m_BaseInfoName()
	, m_bReadOnlyInfo(false)
	, m_ConfigTest(wxT("Settings/isLocal"))
	, m_bFallback(true)
	, m_locale(NULL)
{
}


CBaseApp::~CBaseApp()
{
	delete m_locale;
}


void CBaseApp::BaseAppCleanup(bool deleteConfig)
{
	if (deleteConfig)
	{
		wxConfigBase* pOld = wxConfig::Set(NULL);
		if (pOld)
			delete pOld;
	}
	CImageManager::Delete();
	Element::Terminate();
}


bool CBaseApp::OnInit()
{
#if defined(__WXMSW__) && wxCHECK_VERSION(2, 9, 5)
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
			wxConfigBase* pBaseConfig = NULL;

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
		}
	}

	return InitLocale();
}


int CBaseApp::OnExit()
{
	BaseAppCleanup();
	return wxApp::OnExit();
}


bool CBaseApp::InitDefaultLocale()
{
	m_locale = new wxLocale();
	m_locale->Init();
	return true;
}


bool CBaseApp::InitCatalogLocale()
{
	wxString dirLang = wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + wxT("lang");

	m_locale = new wxLocale();
	m_locale->AddCatalogLookupPathPrefix(dirLang);
#if wxCHECK_VERSION(2, 9, 5)
	m_locale->Init(wxLANGUAGE_ENGLISH_US, wxLOCALE_DONT_LOAD_DEFAULT);
#else
	m_locale->Init(wxLANGUAGE_ENGLISH_US, wxLOCALE_CONV_ENCODING);
#endif

	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	bool rc = m_locale->AddCatalog(fileName.GetName(), wxLANGUAGE_USER_DEFINED, wxEmptyString);
	if (!rc)
	{
		wxString str = wxString::Format(wxT("ERROR: Unable to load '%s.mo'."), fileName.GetName().c_str());
		wxMessageBox(str, wxMessageBoxCaptionStr, wxICON_ERROR | wxOK);
	}
	return rc;
}
