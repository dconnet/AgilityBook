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

#include "LanguageManager.h"
#include "LibwxARBWin.h"
#include "ResourceManager.h"

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
	None,     // Not using MO files.
	Embedded, // 'mo' files are embedded in dat file
};


class ARBWIN_API CBaseApp
	: public wxApp
	, public ILanguageCallback
	, public IResourceManagerCallback
{
	DECLARE_NO_COPY_IMPLEMENTED(CBaseApp)
protected:
	explicit CBaseApp(wxString const& appName, ARBLanguageCatalog useLangCatalog = ARBLanguageCatalog::None);
	// If appRegKey is empty, no local config will be created.
	CBaseApp(
		wxString const& appName,
		wxString const& appRegKey,
		ARBLanguageCatalog useLangCatalog = ARBLanguageCatalog::None);

#if USE_DBGREPORT
	void GenerateReport(wxDebugReport::Context ctx);
#endif

	virtual wxString GetReportName() const
	{
		return GetAppName();
	}

	// wxApp virtual
	// Will init Element, ResourceManager, wxApp::SetAppName and wxConfig::Set.
	// If OnInit fails, BaseAppCleanup will be called before returning.
	bool OnInit() override;
	int OnExit() override;
#if USE_DBGREPORT
	void OnFatalException() override;
#endif

#if USE_DBGREPORT
	// Allow derived program ability to add files
	virtual bool OnAddFileDebugReport(wxDebugReport* report)
	{
		return false;
	}
#endif

	// CBaseApp virtual
	// Note: If called before creating a frame, config needs to be deleted.
	virtual void BaseAppCleanup(bool deleteConfig = false); // So dlg apps can cleanup.

public:
	/**
	 * Key to use in UpdateInfo (default registry value)
	 */
	virtual std::wstring GetUpdateInfoKey() const;

	/**
	 * Called after setting config path. Derived app should create settings here.
	 */
	virtual void OnConfigInitialized();

	// IResourceManagerCallback
	// Derived classes need to override these to use the resource manager.
	wxWindow* GetResourceWindow() override;
	bool GetResImageName(wxArtID const& id, wxArtClient const& client, wxString& outName, bool& outSvg) const override;

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
	size_t AvailableLanguages() const;

	// Are we running in standalone mode? (wxConfig is using .info file)

	bool IsStandloneMode() const
	{
		return m_bStandalone;
	}

protected:
	// ILanguageCallback interface
	wxLanguage OnGetLanguage() const override;
	wxString OnGetCatalogName() const override;
	wxString OnGetLangConfigName() const override;
	void OnSetLanguage(wxLanguage langId) override;
	void OnErrorMessage(wxString const& msg) const override;

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
	ARBLanguageCatalog m_useLangCatalog;
	std::unique_ptr<CLanguageManager> m_langMgr;
};
