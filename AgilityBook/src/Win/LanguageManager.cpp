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


CLanguageManager::CLanguageManager(ILanguageCallback* pCallback)
	: m_pCallback(pCallback)
	, m_CurLang(wxLANGUAGE_DEFAULT)
	, m_dirLang()
	, m_dirLoadedLang()
	, m_locale(nullptr)
	, m_bEmbedded(false)
{
#ifdef WIN32
	m_bEmbedded = true;
#endif
}


CLanguageManager::~CLanguageManager()
{
	delete m_locale;
}


int CLanguageManager::GetDefaultLanguage() const
{
	if (!m_pCallback || m_pCallback->OnGetLangConfigName().empty())
		return wxLANGUAGE_ENGLISH_US;

	int lang = m_CurLang;

	wxString langStr = wxConfig::Get()->Read(m_pCallback->OnGetLangConfigName(), wxEmptyString);
	if (!langStr.empty())
	{
		const wxLanguageInfo* langInfo = wxLocale::FindLanguageInfo(langStr);
		if (langInfo)
			lang = langInfo->Language;
	}

	return lang;
}


wxString CLanguageManager::GetDefaultCatalogName() const
{
	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	return fileName.GetName();
}


wxString CLanguageManager::GetDefaultLanguageDir() const
{
	if (m_bEmbedded)
		return wxString();
	return wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + wxT("lang");
}


bool CLanguageManager::InitLocale()
{
	bool bInit = true;
	if (m_pCallback)
	{
		m_dirLang = m_pCallback->OnGetLanguageDir();

		int lang = m_pCallback->OnGetLanguage();
		if (wxLANGUAGE_DEFAULT == lang)
		{
			wxLocale* tmp = new wxLocale(wxLANGUAGE_DEFAULT);
			int lang2 = tmp->GetLanguage();
			delete tmp;
			// Set the initial language to the system default.
			SetLang(lang2);
			// If we don't support that language (lookup fails)...
			if (wxString(L"IDD_LANGUAGE") == _("IDD_LANGUAGE"))
			{
				// ... force English.
				SetLang(wxLANGUAGE_ENGLISH_US);
			}
			bInit = SelectLanguage();
		}
		else
			bInit = SetLang(lang);
	}
	else
	{
		m_locale = new wxLocale();
		m_locale->Init();
	}
	return bInit;
}


bool CLanguageManager::SelectLanguage(wxWindow* parent)
{
	return SetLang(SelectLang(parent));
}


#ifdef WIN32
BOOL CALLBACK EnumResourceProc(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam)
{
	std::vector<wxString>* pFiles = reinterpret_cast<std::vector<wxString>*>(lParam);
	pFiles->push_back(lpszName);
	return TRUE;
}
#endif


int CLanguageManager::SelectLang(wxWindow* parent)
{
	if (!m_pCallback)
		return wxLANGUAGE_ENGLISH_US;

	int lang = m_CurLang;

	int idxLang = -1;
	std::vector<int> langId;
	wxArrayString choices;

	std::vector<wxString> files;
#ifdef WIN32
	if (m_bEmbedded)
	{
		// Note: Resource names are the basename+langid.
		// See wxResourceTranslationsLoader for more information.
		std::vector<wxString> resourceNames;
		::EnumResourceNames(NULL, L"MOFILE", EnumResourceProc, reinterpret_cast<LONG_PTR>(&resourceNames));
		wxString base = GetDefaultCatalogName();
		base.MakeUpper();
		for each (wxString resName in resourceNames)
		{
			if (0 == resName.Find(base))
				files.push_back(resName.Mid(base.length() + 1));
			else
				assert(0);
		}
	}
	else
#endif
	{
		wxDir dir(m_dirLang);
		if (dir.IsOpened())
		{
			wxString filename;
			bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);
			while (cont)
			{
				files.push_back(filename);
				cont = dir.GetNext(&filename);
			}
		}
	}

	for each (wxString filename in files)
	{
		wxLanguageInfo const* info = wxLocale::FindLanguageInfo(filename);
		if (info)
		{
			// Trigger poedit to capture these.
			// These are the strings wx returns.
#if 0
			wchar_t* x1 = _("English (U.S.)");
			wchar_t* x1 = _("French");
#endif
			choices.Add(wxGetTranslation(info->Description));
			langId.push_back(info->Language);
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


bool CLanguageManager::SetLang(int langId)
{
	if (!m_pCallback)
		return false;

	// If the same, then we did succeed.
	if (langId == m_CurLang)
		return true;

	m_CurLang = langId;
	if (m_locale)
		delete m_locale;
	m_locale = new wxLocale();

	if (!m_bEmbedded)
	{
		m_locale->AddCatalogLookupPathPrefix(m_dirLang);
	}

#if wxCHECK_VERSION(3, 0, 0)
	m_locale->Init(m_CurLang, wxLOCALE_DONT_LOAD_DEFAULT);
#else
	m_locale->Init(m_CurLang, wxLOCALE_CONV_ENCODING);
#endif

#ifdef WIN32
	if (m_bEmbedded)
		wxTranslations::Get()->SetLoader(new wxResourceTranslationsLoader);
#endif

	bool rc = m_locale->AddCatalog(m_pCallback->OnGetCatalogName(), wxLANGUAGE_USER_DEFINED, wxEmptyString);
	if (rc)
	{
		m_dirLoadedLang = m_locale->GetCanonicalName();
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
