/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CLanguageManager class
 * @author David Connet
 *
 * Revision History
 * @li 2011-11-14 DRC Fix canceling language selection (in Fr, it reset to En)
 * @li 2011-11-11 DRC Make .mo name same as exe name.
 * @li 2009-09-20 DRC wxLANGUAGE is not consistent between releases.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-03-19 DRC Make sure the catalog is intialized before calling dlg.
 * @li 2009-01-01 DRC Ported to wxWidgets.
 * @li 2007-09-22 DRC Created
 */

#include "stdafx.h"
#include "LanguageManager.h"

#include "RegItems.h"

#include "ARBCommon/StringUtil.h"
#include <wx/cshelp.h>
#include <wx/dir.h>
#include <wx/stdpaths.h>
#include <stdexcept>
#include <vector>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CLanguageManager::CLanguageManager()
	: m_dirLang()
	, m_dirLoadedLang()
	, m_locale(NULL)
	, m_CurLang(wxLANGUAGE_DEFAULT)
	, m_Localization()
{
	IARBLocalization::Init(&m_Localization);

	//wxHelpProvider::Set(new wxHelpControllerHelpProvider);
	wxHelpProvider::Set(new wxSimpleHelpProvider);

	m_dirLang = wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + L"lang";

	int lang = m_CurLang;
	// Introduced in 2.1.
	wxString langStr = wxConfig::Get()->Read(CFG_SETTINGS_LANG3, wxEmptyString);
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

	if (lang == wxLANGUAGE_DEFAULT)
	{
		wxLocale* tmp = new wxLocale(wxLANGUAGE_DEFAULT);
		lang = tmp->GetLanguage();
		delete tmp;
		// Set the initial language to the system default.
		SetLang(lang);
		// If we don't support that language (lookup fails)...
		if (wxString(L"IDD_LANGUAGE") == _("IDD_LANGUAGE"))
		{
			// ... force English.
			SetLang(wxLANGUAGE_ENGLISH_US);
		}
		SelectLanguage();
	}
	else
		SetLang(lang);
}


CLanguageManager::~CLanguageManager()
{
	delete m_locale;
	m_locale = NULL;
}


bool CLanguageManager::SetLang(int langId)
{
	if (langId == m_CurLang)
		return false;

	m_CurLang = langId;
	if (m_locale)
		delete m_locale;
	m_locale = new wxLocale();
	m_locale->AddCatalogLookupPathPrefix(StringUtil::stringWX(m_dirLang));
	if (!m_locale->Init(m_CurLang, wxLOCALE_DONT_LOAD_DEFAULT))
	{
		//return false;
	}
	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	m_locale->AddCatalog(fileName.GetName(), wxLANGUAGE_USER_DEFINED, wxEmptyString);

	m_dirLoadedLang = m_locale->GetCanonicalName();
	if (2 < m_dirLoadedLang.length())
		m_dirLoadedLang = m_dirLoadedLang.substr(0, 2);

	if (!m_Localization.Load())
	{
		wxString str = wxString::Format(wxT("ERROR: Unable to load '%s.mo'."), fileName.GetName().c_str());
		wxMessageBox(str, wxMessageBoxCaptionStr, wxICON_ERROR | wxOK);
		std::string msg(str.ToAscii());
		throw std::runtime_error(msg);
	}

	return true;
}


int CLanguageManager::SelectLang(wxWindow* parent)
{
	int lang = m_CurLang;

	int idxLang = -1;
	std::vector<int> langId;
	wxArrayString choices;

	wxDir dir(StringUtil::stringWX(m_dirLang));
	if (dir.IsOpened())
	{
		wxString filename;
		bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);
		while (cont)
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
			cont = dir.GetNext(&filename);
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
	if (langInfo)
		wxConfig::Get()->Write(CFG_SETTINGS_LANG3, langInfo->CanonicalName);

	return lang;
}


bool CLanguageManager::SelectLanguage(wxWindow* parent)
{
	return SetLang(SelectLang(parent));
}
