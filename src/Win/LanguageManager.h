#pragma once

/*
 * Copyright © 2002-2007 David Connet. All Rights Reserved.
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
 * @li 2007-09-22 DRC Created
 */

#include <map>
#include "Localization.h"

/**
 * Class to manage resource-only language DLLs.
 * This class is meant to work closely with CWinApp.
 */
class CLanguageManager
{
public:
	CLanguageManager();
	~CLanguageManager();

	/**
	 * Set the initial language in the app based on system settings.
	 * This is normally called once as the first thing in InitInstance
	 */
	void SetInitialLanguage(LPCTSTR* pszHelpFilePath);

	/**
	 * Set the default language. When run the first time, this will call
	 * SelectLanguage(). Subsequent calls will load the last selected language.
	 * @note Do not call until after SetRegistryKey as it needs to access
	 * the registry to retrieve the last selected language.
	 */
	void SetDefaultLanguage();

	/**
	 * How many languages are we managing - Number of AgilityBook*.dll files
	 * found in the executables directory that have the same version number as
	 * the executable.
	 */
	size_t NumLanguages();

	/**
	 * Display a user interface to select a language.
	 */
	bool SelectLanguage();

	/**
	 * Executables language (embedded resources)
	 */
	LANGID ExeLanguage() const				{return m_LangID;}

	typedef std::map<LANGID, HINSTANCE> LangResources;
	/**
	 * Mapping of language ids to loaded resource instance.
	 */
	LangResources const& Languages() const	{return m_Langs;}

	/**
	 * Currectly selected language.
	 */
	LANGID CurrentLanguage() const			{return m_CurLang;}

private:
	LANGID DetectLanguage() const;
	bool SetLanguage(LANGID langId);
	void SetHelpFile();

	HINSTANCE m_hInstance;
	CString m_InitHelpFilePath;
	LPCTSTR* m_pszHelpFilePath;
	LANGID m_LangID;
	LangResources m_Langs;
	LANGID m_CurLang;
	CLocalization m_Localization;
};
