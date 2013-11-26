#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * Revision History
 * @li 2013-11-26 DRC Fixed language initialization structure.
 * @li 2013-08-22 DRC Fixed issue with ctor auto-cast.
 * @li 2013-08-18 DRC Merged language management in.
 * @li 2013-08-17 DRC Added local config file support.
 * @li 2013-06-26 DRC Changed ctors.
 * @li 2013-04-10 DRC Add wxLocale default initialization.
 * @li 2012-12-30 DRC Created base app to derive from.
 * @li 2012-08-13 DRC Moved dconSetupApp out of Globals.cpp
 */

#include "ImageManager.h"
#include "LanguageManager.h"


class CBaseApp : public wxApp
				, public ILanguageCallback
				, public IImageManagerCallback
{
	DECLARE_NO_COPY_CLASS(CBaseApp)
protected:
	// If appRegKey is empty, no config will be created.
	CBaseApp(
			wxString const& appName,
			wxString const& appRegKey = wxEmptyString,
			bool bUseLangCatalog = false);
	~CBaseApp();

	// wxApp virtual
	// Will init Element, ImageManager, wxApp::SetAppName and wxConfig::Set.
	virtual bool OnInit();
	virtual int OnExit();

	// CBaseApp virtual
	virtual void BaseAppCleanup(bool deleteConfig = false); // So dlg apps can cleanup.

	// Language control

public:
	/**
	 * Display a user interface to select a language.
	 */
	virtual bool SelectLanguage(wxWindow* parent = NULL);
	/**
	 * Currectly selected language.
	 */
	std::wstring CurrentLanguage() const;
	int CurrentLanguageId() const;

protected:
	// ILanguageCallback interface
	virtual int OnGetLanguage() const;
	virtual wxString OnGetCatalogName() const;
	virtual wxString OnGetLangConfigName() const;
	virtual wxString OnGetLanguageDir() const;
	virtual void OnSetLanguage(int langId);
	virtual void OnErrorMessage(wxString const& msg) const;

	// IImageManagerCallback interface
	virtual bool OnCreateBitmap(
			const wxArtID& id,
			const wxArtClient& client,
			const wxSize& size,
			wxBitmap& outBmp);
	virtual bool OnCreateIconBundle(
			const wxArtID& id,
			const wxArtClient& client,
			wxIconBundle& outIcon);

	virtual bool InitLocale();
	virtual int SelectLang(wxWindow* parent = NULL);

	// Vendor name (default: 'dcon Software')
	wxString m_VendorName;
	// Name displayed in caption.
	wxString m_BaseAppName;
	// Name used as registry key.
	wxString m_BaseRegName;

	// Filename to use for '.info' files. (Default: <filename.info>)
	// Always located in <wxResourceDir>.
	// If it does not exist, fall back to system default wxConfig.
	wxString m_BaseInfoName;
	// '.info' will be used readonly. (default: false)
	bool m_bReadOnlyInfo;
	wxString m_ConfigTest; // For testing readonly (default: 'Settings/isLocal')
	// If '.info' isn't writable, fall back to system default. (default: true)
	bool m_bFallback;

private:
	CLanguageManager* m_langMgr;
};
