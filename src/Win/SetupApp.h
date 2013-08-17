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
 * @li 2013-08-17 DRC Added local config file support.
 * @li 2013-06-26 DRC Changed ctors.
 * @li 2013-04-10 DRC Add wxLocale default initialization.
 * @li 2012-12-30 DRC Created base app to derive from.
 * @li 2012-08-13 DRC Moved dconSetupApp out of Globals.cpp
 */

class wxLocale;


class CBaseApp : public wxApp
{
	DECLARE_NO_COPY_CLASS(CBaseApp)
protected:
	// Caption and registry names are the same.
	CBaseApp(wxString const& appName);
	// If appRegKey is empty, no config will be created.
	CBaseApp(
			wxString const& appName,
			wxString const& appRegKey);
	~CBaseApp();

	// CBaseApp virtual
	virtual bool InitLocale()	{return InitDefaultLocale();}
	virtual void BaseAppCleanup(bool deleteConfig = false); // So dlg apps can cleanup.

	// wxApp virtual
	// Will init Element, ImageManager, wxApp::SetAppName and wxConfig::Set.
	virtual bool OnInit();
	virtual int OnExit();

	// InitLocale() should call one of these 2 functions to init m_locale.
	bool InitDefaultLocale();
	bool InitCatalogLocale();

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
	wxLocale* m_locale;
};
