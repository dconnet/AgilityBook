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
#include "UpdateInfo.h"

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

	bool AutoCheckProgram();
	void AutoCheckConfiguration(CAgilityBookDoc* pDoc);
	void UpdateConfiguration(CAgilityBookDoc* pDoc, bool& outClose);

	void SetMessageText(
			std::wstring const& msg,
			bool bFiltered);
	void SetMessageText2(std::wstring const& msg);

	wxPrintDialogData* GetPrintData();
	void SavePrintData(wxPrintDialogData const& data);
	CHtmlEasyPrinting* GetHtmlPrinter();
	CAgilityBookMenu& GetMenus() { return m_menus; }

protected:
	virtual wxString GetReportName() const { return wxT("AgilityBook"); }

	virtual bool OnInit();
	virtual int OnExit();

#if USE_DBGREPORT
	virtual bool OnAddFileDebugReport(wxDebugReport* report);
#endif

	virtual void BaseAppCleanup(bool deleteConfig = false);

	virtual wxLanguage OnGetLanguage() const;
	virtual wxString OnGetLangConfigName() const;
	virtual void OnSetLanguage(wxLanguage langId);

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

	virtual bool InitLanguage();

	DECLARE_EVENT_TABLE()
	void OnQueryEndSession(wxCloseEvent& evt);
	void OnEndSession(wxCloseEvent& evt);

	bool m_bShutdownSocket;
	CLocalization m_Localization;
	CImageHelper m_imageHelper;
	CUpdateInfo m_UpdateInfo;
	CAgilityBookDocManager* m_manager;
	wxPrintDialogData* m_printDialogData;
	CHtmlEasyPrinting* m_Prn;
	CAgilityBookMenu m_menus;
};

DECLARE_APP(CAgilityBookApp)
