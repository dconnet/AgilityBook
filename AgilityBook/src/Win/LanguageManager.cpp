/*
 * Copyright (c) 2007-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CLanguageManager class
 * @author David Connet
 *
 * Revision History
 * @li 2009-03-19 DRC Make sure the catalog is intialized before calling dlg.
 * @li 2009-01-01 DRC Ported to wxWidgets.
 * @li 2007-09-22 DRC Created
 */

#include "stdafx.h"
#include "LanguageManager.h"

#include <wx/cshelp.h>
#include <wx/dir.h>
#include <wx/html/helpctrl.h>
#include <wx/stdpaths.h>


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

	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	m_dirLang = fileName.GetPath() + wxFileName::GetPathSeparator() + wxT("lang");

	int lang = m_CurLang;
	long lastLang;
	if (wxConfig::Get()->Read(wxT("Settings/Lang2"), &lastLang, 0) && 0 != lastLang)
	{
		lang = lastLang;
	}
	else if (wxConfig::Get()->Read(wxT("Settings/Lang"), &lastLang, 0) && 0 != lastLang)
	{
		// Translates v1.10 registry
		if (0x0409 == lastLang)
			lang = wxLANGUAGE_ENGLISH_US;
		else if (0x040c == lastLang)
			lang = wxLANGUAGE_FRENCH;
	}

	if (lang == wxLANGUAGE_DEFAULT)
	{
		wxLocale* tmp = new wxLocale(wxLANGUAGE_DEFAULT);
		lang = tmp->GetLanguage();
		delete tmp;
		// Set the initial language to the system default.
		SetLang(lang);
		// If we don't support that language (lookup fails)...
		if (wxString(wxT("IDD_LANGUAGE")) == _("IDD_LANGUAGE"))
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
	m_locale->AddCatalogLookupPathPrefix(m_dirLang);
	if (!m_locale->Init(m_CurLang, wxLOCALE_CONV_ENCODING))
	{
		//return false;
	}
	m_locale->AddCatalog(wxT("arb"));

	m_dirLoadedLang = m_locale->GetCanonicalName();
	if (2 < m_dirLoadedLang.length())
		m_dirLoadedLang = m_dirLoadedLang.Left(2);

	m_Localization.Load();

	return true;
}


int CLanguageManager::SelectLang(wxWindow* parent)
{
	int lang = m_CurLang;

	int idxLang = -1;
	std::vector<int> langId;
	wxArrayString choices;

	wxDir dir(m_dirLang);
	if (dir.IsOpened())
	{
		wxString filename;
		bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);
		while (cont)
		{
			wxLanguageInfo const* info = wxLocale::FindLanguageInfo(filename);
			if (info)
			{
				choices.Add(info->Description);
				langId.push_back(info->Language);
				if (info->Language == lang)
					idxLang = static_cast<int>(langId.size()) - 1;
			}
			cont = dir.GetNext(&filename);
		}
	}

	if (1 == choices.size())
		lang = langId[0];
	else
	{
		// Note, the size arguments are ignored. sigh.
		wxSingleChoiceDialog dialog(parent, _("IDC_LANG_CHOOSE"), _("IDD_LANGUAGE"), choices);
		dialog.SetHelpText(_("HIDC_LANGUAGE"));
		dialog.SetSelection(idxLang);
		if (wxID_OK == dialog.ShowModal())
			lang = langId[dialog.GetSelection()];
		else
			lang = wxLANGUAGE_ENGLISH_US;
	}
	wxConfig::Get()->Write(wxT("Settings/Lang2"), lang);

	return lang;
}


bool CLanguageManager::SelectLanguage(wxWindow* parent)
{
	return SetLang(SelectLang(parent));
}
