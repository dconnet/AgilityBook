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
 * 2021-12-02 EN/FR language descriptions changed slightly in wx3.1.6.
 * 2019-12-13 Fix issue when loading non-existing .mo file.
 * 2019-09-13 Fix initialization issue.
 * 2018-10-11 Moved to Win LibARBWin
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
#include "LibARBWin/LanguageManager.h"

#include "ARBCommon/ARBUtils.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ARBWinUtilities.h"
#include "LibARBWin/ResourceManager.h"
#include "fmt/xchar.h"
#include <wx/buffer.h>
#include <wx/config.h>
#include <wx/dir.h>
#include <wx/fileconf.h>
#include <wx/stdpaths.h>
#include <wx/translation.h>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


class wxDatTranslationsLoader : public wxTranslationsLoader
{
public:
	wxMsgCatalog* LoadCatalog(const wxString& domain, const wxString& lang) override;
	wxArrayString GetAvailableTranslations(const wxString& domain) const override;
};


wxMsgCatalog* wxDatTranslationsLoader::LoadCatalog(const wxString& domain, const wxString& lang)
{
	wxMsgCatalog* catalog = nullptr;

	auto resMgr = CResourceManager::Get();
	if (!resMgr)
		return catalog;

	std::wstring name = fmt::format(L"lang/{}/{}.mo", lang, domain);

	std::ostringstream str;
	if (resMgr->LoadFile(name, str))
	{
		auto const& s = str.str();
		catalog
			= wxMsgCatalog::CreateFromData(wxScopedCharTypeBuffer<char>::CreateNonOwned(s.c_str(), s.size()), domain);
	}
	return catalog;
}


wxArrayString wxDatTranslationsLoader::GetAvailableTranslations(const wxString& domain) const
{
	wxArrayString langs;

	auto resMgr = CResourceManager::Get();
	if (resMgr)
	{
		std::vector<wxString> directories;
		if (0 < resMgr->FindDirectories(L"lang", directories))
		{
			for (auto dir : directories)
			{
				std::wstring name = fmt::format(L"lang/{}/{}.mo", dir, domain);
				if (resMgr->Exists(name))
					langs.Add(dir);
			}
		}
	}

	return langs;
}

/////////////////////////////////////////////////////////////////////////////

CLanguageManager::CLanguageManager(ILanguageCallback* pCallback)
	: m_pCallback(pCallback)
	, m_CurLang(wxLANGUAGE_DEFAULT)
	, m_dirLoadedLang()
	, m_locale(nullptr)
{
	wxTranslations::Set(new wxTranslations);
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

	wxDatTranslationsLoader* loader = new wxDatTranslationsLoader;
	wxTranslations::Get()->SetLoader(loader);

	m_locale = std::make_unique<wxLocale>(wxLANGUAGE_DEFAULT); // Initialize wx with the default system settings

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
						bSet = SetLang(lang2);
					}
				}
			}
			// Oh well... English it is.
			if (!bSet)
				bSet = SetLang(wxLANGUAGE_ENGLISH_US);
			bInit = SelectLanguage();
			// SelectLanguage returns false if we didn't change.
			// If we set it here, we're good.
			if (!bInit && bSet)
				bInit = true;
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


size_t CLanguageManager::AvailableLanguages() const
{
	wxLanguage lang = m_CurLang;
	std::vector<wxLanguage> langId;
	wxArrayString choices;
	GetAvailableLanguages(lang, langId, choices);
	return choices.size();
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

	std::vector<wxLanguage> langId;
	wxArrayString choices;
	int idxLang = GetAvailableLanguages(lang, langId, choices);

	if (0 == choices.size())
	{
		lang = wxLANGUAGE_ENGLISH_US;
		wxMessageBox(_("No other languages are available."));
	}
	else if (1 == choices.size())
	{
		lang = langId[0];
		wxMessageBox(_("No other languages are available."));
	}
	else
	{
		// Note, the size arguments are ignored. sigh.
		wxSingleChoiceDialog dialog(parent, _("Choose a language:"), _("Language Selection"), choices);
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
		// The wx3.1.2 release had an incompatible change in AddCatalog. 3.1.3 restores it.
#if wxMAJOR_VERSION == 3 && wxMINOR_VERSION == 1 && wxRELEASE_NUMBER == 2
		if (m_CurLang == wxLANGUAGE_ENGLISH_US)
			rc = wxTranslations::Get()->AddCatalog(m_pCallback->OnGetCatalogName(), wxLANGUAGE_USER_DEFINED);
		else
			rc = wxTranslations::Get()->AddCatalog(m_pCallback->OnGetCatalogName());
#else
		rc = wxTranslations::Get()->AddCatalog(m_pCallback->OnGetCatalogName(), m_CurLang);
#endif
		if (rc)
		{
			// In wxTranslations::LoadCatalog(const wxString& domain, const wxString& lang, const wxString& msgIdLang),
			// wx assumes that if lang == msgIdLang, everything is fine. Um. No.
			// So ask if there are any available translations and fail if not.
			if (0 == wxTranslations::Get()->GetAvailableTranslations(m_pCallback->OnGetCatalogName()).size())
				rc = false;
		}
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
		wxString str = wxString::Format(L"ERROR: Unable to load '%s.mo'.", m_pCallback->OnGetCatalogName().c_str());
		m_pCallback->OnErrorMessage(str);
	}

	return rc;
}


int CLanguageManager::GetAvailableLanguages(wxLanguage& lang, std::vector<wxLanguage>& langId, wxArrayString& choices)
	const
{
	int idxLang = -1;

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
			// This changed in 3.1.6
			wchar_t* x1 = _("English (United States)");
			wchar_t* x1 = _("French (France)");
#endif
			choices.Add(wxGetTranslation(info->Description));
			langId.push_back(static_cast<wxLanguage>(info->Language));
			if (info->Language == lang)
				idxLang = static_cast<int>(langId.size()) - 1;
		}
	}

	return idxLang;
}
