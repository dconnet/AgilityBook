#pragma once

/*
 * Copyright (c) 2002-2009 David Connet. All Rights Reserved.
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
