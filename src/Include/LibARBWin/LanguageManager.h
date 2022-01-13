#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CLanguageManager class
 * @author David Connet
 *
 * Revision History
 * 2018-10-11 Moved to Win LibARBWin
 * 2018-04-20 Use wxTranslations instead of wxLocale.
 * 2013-11-26 Fixed language initialization structure.
 * 2013-08-18 Reworked for easier reuse.
 * 2009-01-01 Ported to wxWidgets.
 * 2007-09-22 Created
 */

#include "LibwxARBWin.h"
class wxLocale;


class ARBWIN_API ILanguageCallback
{
public:
	virtual ~ILanguageCallback() = default;
	virtual wxLanguage OnGetLanguage() const = 0;
	virtual wxString OnGetCatalogName() const = 0;
	virtual wxString OnGetLangConfigName() const = 0;
	virtual void OnSetLanguage(wxLanguage langId) = 0;
	virtual void OnErrorMessage(wxString const& msg) const = 0;
};


class ARBWIN_API CLanguageManager
{
	DECLARE_NO_COPY_IMPLEMENTED(CLanguageManager)
public:
	// Must use callback to enable catalogs.
	// MO files are assumed to be embedded in DAT file in 'lang' directory structure.
	explicit CLanguageManager(ILanguageCallback* pCallback);

	bool InitLanguage();

	std::wstring CurrentLanguage() const
	{
		return m_dirLoadedLang;
	}
	wxLanguage CurrentLanguageId() const
	{
		return m_CurLang;
	}
	wxLanguage GetDefaultLanguage() const;
	wxString GetDefaultCatalogName() const;

	size_t AvailableLanguages() const;

	bool SelectLanguage(wxWindow* parent = nullptr);
	wxLanguage SelectLang(wxWindow* parent = nullptr);
	bool SetLang(wxLanguage langId);

private:
	int GetAvailableLanguages(wxLanguage& lang, std::vector<wxLanguage>& langId, wxArrayString& choices) const;

	ILanguageCallback* m_pCallback;
	wxLanguage m_CurLang;
	std::wstring m_dirLoadedLang; /// 'en'/'fr' etc
	std::unique_ptr<wxLocale> m_locale;
};
