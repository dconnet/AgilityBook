#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CLanguageManager class
 * @author David Connet
 *
 * Revision History
 * @li 2009-01-01 DRC Ported to wxWidgets.
 * @li 2007-09-22 DRC Created
 */

#include "Localization.h"
class wxLocale;


/**
 * Wrap language management apis.
 * (originally managed resource language dlls on windows)
 */
class CLanguageManager
{
public:
	CLanguageManager();
	~CLanguageManager();

	/**
	 * Display a user interface to select a language.
	 */
	bool SelectLanguage(wxWindow* parent = NULL);

	/**
	 * Currectly selected language.
	 */
	wxString CurrentLanguage() const			{return m_dirLoadedLang;}

private:
	int SelectLang(wxWindow* parent = NULL);
	bool SetLang(int langId);

	wxString m_dirLang; /// Where the en/fr/etc directories are located
	wxString m_dirLoadedLang; /// 'en'/'fr' etc
	wxLocale* m_locale;
	int m_CurLang;
	CLocalization m_Localization;
};
