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
 * 2018-10-11 Moved to Win LibARBWin
 * 2018-04-20 Use wxTranslations instead of wxLocale.
 * 2018-01-28 Add debug reporting.
 * 2013-11-26 Fixed language initialization structure.
 * 2013-08-22 Fixed issue with ctor auto-cast.
 * 2013-08-18 Merged language management in.
 * 2013-08-17 Added local config file support.
 * 2013-06-26 Changed ctors.
 * 2013-04-10 Add wxLocale default initialization.
 * 2012-12-30 Created base app to derive from.
 * 2012-08-13 Moved dconSetupApp out of Globals.cpp
 */

#include "ImageManager.h"
#include "LanguageManager.h"
#include "LibwxARBWin.h"

#if wxUSE_DEBUGREPORT && wxUSE_ON_FATAL_EXCEPTION
#define USE_DBGREPORT 1
#else
#define USE_DBGREPORT 0
#endif
#if USE_DBGREPORT
#include <wx/debugrpt.h>
#endif


enum class ARBLanguageCatalog
{
	None,      // Not using MO files.
	Directory, // Files are in 'lang' directory
	Embedded,  // Files are embedded in rc file (win only)
};


class ARBWIN_API CBaseApp : public wxApp
	, public ILanguageCallback
	, public IImageManagerCallback
{
	DECLARE_NO_COPY_IMPLEMENTED(CBaseApp)
protected:
	// If appRegKey is empty, no config will be created.
	CBaseApp(
			wxString const& appName,
			wxString const& appRegKey = wxEmptyString,
			ARBLanguageCatalog useLangCatalog = ARBLanguageCatalog::None);

#if USE_DBGREPORT
	void GenerateReport(wxDebugReport::Context ctx);
#endif

	virtual wxString GetReportName() const { return GetAppName(); }

	// wxApp virtual
	// Will init Element, ImageManager, wxApp::SetAppName and wxConfig::Set.
	// If OnInit fails, BaseAppCleanup will be called before returning.
	bool OnInit() override;
	int OnExit() override;
#if USE_DBGREPORT
	void OnFatalException() override;
#endif

#if USE_DBGREPORT
	// Allow derived program ability to add files
	virtual bool OnAddFileDebugReport(wxDebugReport* report) { return false; }
#endif

	// CBaseApp virtual
	// Note: If called before creating a frame, config needs to be deleted.
	virtual void BaseAppCleanup(bool deleteConfig = false); // So dlg apps can cleanup.

	// CBaseApp virtual
	// Initialize typical wxFileSystem handlers that may be used.
	virtual void InitFSHandlers();

public:

	// Language control

	/**
	 * Display a user interface to select a language.
	 */
	virtual bool SelectLanguage(wxWindow* parent = nullptr);
	/**
	 * Currectly selected language.
	 */
	std::wstring CurrentLanguage() const;
	wxLanguage CurrentLanguageId() const;

	// Are we running in standalone mode? (wxConfig is using .info file)

	bool IsStandloneMode() const { return m_bStandalone; }

protected:
	// ILanguageCallback interface
	wxLanguage OnGetLanguage() const override;
	wxString OnGetCatalogName() const override;
	wxString OnGetLangConfigName() const override;
	wxString OnGetLanguageDir() const override;
	void OnSetLanguage(wxLanguage langId) override;
	void OnErrorMessage(wxString const& msg) const override;

	// IImageManagerCallback interface
	bool OnCreateBitmap(
			const wxArtID& id,
			const wxArtClient& client,
			const wxSize& size,
			wxBitmap& outBmp) override;
	bool OnCreateIconBundle(
			const wxArtID& id,
			const wxArtClient& client,
			wxIconBundle& outIcon) override;

	virtual bool InitLanguage();
	virtual int SelectLang(wxWindow* parent = nullptr);

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
	// .info is in use for wxConfig
	bool m_bStandalone;

private:
	std::unique_ptr<CLanguageManager> m_langMgr;
};
