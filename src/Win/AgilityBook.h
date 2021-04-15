#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief main header file for the AgilityBook application
 * @author David Connet
 *
 * Revision History
 * 2018-01-28 Add debug reporting.
 * 2014-05-18 Added AutoCheckProgram
 * 2013-11-26 Fixed language initialization structure.
 * 2008-12-14 Ported to wxWidgets.
 * 2005-10-19 Fixed a problem with CFile::GetStatus (see AgilityBook.cpp).
 * 2004-06-02 Moved ShellExecute code here.
 */

#include "AgilityBookMenu.h"
#include "ImageHelper.h"
#include "Localization.h"

#include "ARBCommon/ARBUtils.h"
#include "LibARBWin/SetupApp.h"
class CAgilityBookDoc;
class CAgilityBookDocManager;
class CHtmlEasyPrinting;
class wxPrintDialogData;


class CAgilityBookApp : public CBaseApp
{
	DECLARE_NO_COPY_IMPLEMENTED(CAgilityBookApp)
public:
	CAgilityBookApp();

	void SetMessageText(std::wstring const& msg, bool bFiltered);
	void SetMessageText2(std::wstring const& msg);

	wxPrintDialogData* GetPrintData();
	void SavePrintData(wxPrintDialogData const& data);
	CHtmlEasyPrinting* GetHtmlPrinter();
	CMenuHelper& GetMenus();

protected:
	static std::unique_ptr<CMenuHelper> CreateMenus();
	wxString GetReportName() const override
	{
		return L"AgilityBook";
	}

	bool OnInit() override;
	int OnExit() override;

#if USE_DBGREPORT
	bool OnAddFileDebugReport(wxDebugReport* report) override;
#endif

	void BaseAppCleanup(bool deleteConfig = false) override;

	wxLanguage OnGetLanguage() const override;
	wxString OnGetLangConfigName() const override;
	void OnSetLanguage(wxLanguage langId) override;

	// IImageManagerCallback interface
	bool OnCreateBitmap(const wxArtID& id, const wxArtClient& client, const wxSize& size, wxBitmap& outBmp) override;
	bool OnCreateIconBundle(const wxArtID& id, const wxArtClient& client, wxIconBundle& outIcon) override;

	bool InitLanguage() override;

	DECLARE_EVENT_TABLE()
	void OnQueryEndSession(wxCloseEvent& evt);
	void OnEndSession(wxCloseEvent& evt);

	bool m_bShutdownSocket;
	CLocalization m_Localization;
	CImageHelper m_imageHelper;
	std::unique_ptr<CAgilityBookDocManager> m_manager;
	wxPrintDialogData* m_printDialogData;
	CHtmlEasyPrinting* m_Prn;
	std::unique_ptr<CMenuHelper> m_menus;
};

wxDECLARE_APP(CAgilityBookApp);
