/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CLanguageManager class
 * @author David Connet
 *
 * Revision History
 * 2018-04-20 Use wxTranslations instead of wxLocale.
 * 2014-11-14 Add support for embedded MO files on Win32.
 * 2014-07-07 Fixed SetLang returning failure if the lang was the same.
 * 2013-11-26 Fixed language initialization structure.
 * 2011-11-14 Fix canceling language selection (in Fr, it reset to En)
 * 2011-11-11 Make .mo name same as exe name.
 * 2009-09-20 wxLANGUAGE is not consistent between releases.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-03-19 Make sure the catalog is intialized before calling dlg.
 * 2009-01-01 Ported to wxWidgets.
 * 2007-09-22 Created
 */

#include "stdafx.h"
#include "LanguageManager.h"

#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "Globals.h"
#include "ImageManager.h"

#include <wx/config.h>
#include <wx/dir.h>
#include <wx/fileconf.h>
#include <wx/stdpaths.h>
#include <wx/translation.h>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


CLanguageManager::CLanguageManager(ILanguageCallback* pCallback, bool bEmbedded)
	: m_pCallback(pCallback)
	, m_CurLang(wxLANGUAGE_DEFAULT)
	, m_dirLang()
	, m_dirLangDefault()
	, m_dirLoadedLang()
	, m_locale(nullptr)
#if defined(WIN32)
	, m_bEmbedded(bEmbedded)
#else
	, m_bEmbedded(false)
#endif
{
	if (!m_bEmbedded)
		m_dirLangDefault = wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + wxT("lang");
	wxTranslations::Set(new wxTranslations);
}


CLanguageManager::~CLanguageManager()
{
	delete m_locale;
}


wxLanguage CLanguageManager::GetDefaultLanguage() const
{
	if (!m_pCallback || m_pCallback->OnGetLangConfigName().empty())
		return wxLANGUAGE_ENGLISH_US;

	wxLanguage lang = m_CurLang;

	wxString langStr = wxConfig::Get()->Read(m_pCallback->OnGetLangConfigName(), wxEmptyString);
	if (!langStr.empty())
	{
		const wxLanguageInfo* langInfo = wxLocale::FindLanguageInfo(langStr);
		if (langInfo)
			lang = static_cast<wxLanguage>(langInfo->Language);
	}

	return lang;
}


wxString CLanguageManager::GetDefaultCatalogName() const
{
	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	return fileName.GetName();
}


bool CLanguageManager::InitLanguage()
{
	bool bInit = true;

	if (!m_bEmbedded && m_pCallback)
	{
		m_dirLang = m_pCallback->OnGetLanguageDir();
		wxFileTranslationsLoader* loader = new wxFileTranslationsLoader;
		loader->AddCatalogLookupPathPrefix(m_dirLang);
		wxTranslations::Get()->SetLoader(loader);
	}
#if defined(WIN32)
	else if (m_bEmbedded)
	{
		wxTranslations::Get()->SetLoader(new wxResourceTranslationsLoader);
	}
#endif

	m_locale = new wxLocale(wxLANGUAGE_DEFAULT); // Initialize wx with the default system settings

	if (m_pCallback)
	{
		wxLanguage lang = m_pCallback->OnGetLanguage();
		if (wxLANGUAGE_DEFAULT == lang)
		{
			wxLanguage lang2 = lang;
			{
				wxLocale tmp(wxLANGUAGE_DEFAULT);
				lang2 = static_cast<wxLanguage>(tmp.GetLanguage());
			}
			// See if we have this language...
			bool bSet = false;
			wxArrayString files = wxTranslations::Get()->GetAvailableTranslations(m_pCallback->OnGetCatalogName());
			for (auto it = files.begin(); !bSet && it != files.end(); ++it)
			{
				wxLanguageInfo const* info = wxLocale::FindLanguageInfo(*it);
				if (info)
				{
					if (info->Language == lang2)
					{
						bSet = true;
						SetLang(lang2);
					}
				}
			}
			// Oh well... English it is.
			if (!bSet)
				SetLang(wxLANGUAGE_ENGLISH_US);
			bInit = SelectLanguage();
		}
		else
			bInit = SetLang(lang);
	}
	else
	{
		wxTranslations::Get()->SetLanguage(wxLANGUAGE_DEFAULT);
	}
	return bInit;
}


bool CLanguageManager::SelectLanguage(wxWindow* parent)
{
	return SetLang(SelectLang(parent));
}


wxLanguage CLanguageManager::SelectLang(wxWindow* parent)
{
	if (!m_pCallback)
		return wxLANGUAGE_ENGLISH_US;

	wxLanguage lang = m_CurLang;

	int idxLang = -1;
	std::vector<wxLanguage> langId;
	wxArrayString choices;

	wxArrayString files = wxTranslations::Get()->GetAvailableTranslations(m_pCallback->OnGetCatalogName());
	for (auto it = files.begin(); it != files.end(); ++it)
	{
		wxLanguageInfo const* info = wxLocale::FindLanguageInfo(*it);
		if (info)
		{
			// Trigger poedit to capture these.
			// These are the strings wx returns.
#if 0
			wchar_t* x1 = _("English (U.S.)");
			wchar_t* x1 = _("French");
#endif
			choices.Add(wxGetTranslation(info->Description));
			langId.push_back(static_cast<wxLanguage>(info->Language));
			if (info->Language == lang)
				idxLang = static_cast<int>(langId.size()) - 1;
		}
	}

	if (0 == choices.size())
		lang = wxLANGUAGE_ENGLISH_US;
	else if (1 == choices.size())
		lang = langId[0];
	else
	{
		// Note, the size arguments are ignored. sigh.
		wxSingleChoiceDialog dialog(parent, _("Choose a language:"), _("IDD_LANGUAGE"), choices);
		dialog.SetHelpText(_("Select a language to use."));
		dialog.SetSelection(idxLang);
		if (wxID_OK == dialog.ShowModal())
			lang = langId[dialog.GetSelection()];
		else
		{
			if (wxLANGUAGE_DEFAULT == m_CurLang)
				lang = wxLANGUAGE_ENGLISH_US;
			else
				lang = m_CurLang;
		}
	}

	wxLanguageInfo const* langInfo = wxLocale::GetLanguageInfo(lang);
	if (langInfo && m_pCallback && !m_pCallback->OnGetLangConfigName().empty())
		wxConfig::Get()->Write(m_pCallback->OnGetLangConfigName(), langInfo->CanonicalName);

	return lang;
}


bool CLanguageManager::SetLang(wxLanguage langId)
{
	if (!m_pCallback)
		return false;

	// If the same, then we did succeed - but return fail so we don't do anything.
	if (langId == m_CurLang)
		return false;

	m_CurLang = langId;

	bool rc = false;
	{
		wxLocale locale(m_CurLang);
		rc = wxTranslations::Get()->AddCatalog(m_pCallback->OnGetCatalogName(), m_CurLang);
	}
	if (rc)
	{
		wxTranslations::Get()->SetLanguage(m_CurLang);
		m_dirLoadedLang = wxLocale::GetLanguageCanonicalName(m_CurLang);
		if (2 < m_dirLoadedLang.length())
			m_dirLoadedLang = m_dirLoadedLang.substr(0, 2);
		m_pCallback->OnSetLanguage(m_CurLang);
	}
	else
	{
		wxString str = wxString::Format(wxT("ERROR: Unable to load '%s.mo'."), m_pCallback->OnGetCatalogName().c_str());
		m_pCallback->OnErrorMessage(str);
	}

	return rc;
}
