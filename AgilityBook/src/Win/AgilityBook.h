#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief main header file for the AgilityBook application
 * @author David Connet
 *
 * Revision History
 * @li 2008-12-14 DRC Ported to wxWidgets.
 * @li 2005-10-19 DRC Fixed a problem with CFile::GetStatus (see AgilityBook.cpp).
 * @li 2004-06-02 DRC Moved ShellExecute code here.
 */

#include "Localization.h"
#include "SetupApp.h"
#include "UpdateInfo.h"
class CAgilityBookDoc;
class CAgilityBookDocManager;
class CHtmlEasyPrinting;
class wxPrintDialogData;


class CAgilityBookApp : public CBaseApp
{
	DECLARE_NO_COPY_CLASS(CAgilityBookApp)
public:
	CAgilityBookApp();

	void AutoCheckConfiguration(CAgilityBookDoc* pDoc);
	void UpdateConfiguration(CAgilityBookDoc* pDoc, bool& outClose);

	void SetMessageText(
			std::wstring const& msg,
			bool bFiltered);
	void SetMessageText2(std::wstring const& msg);

	wxPrintDialogData* GetPrintData();
	void SavePrintData(wxPrintDialogData const& data);
	CHtmlEasyPrinting* GetHtmlPrinter();

protected:
	virtual bool OnInit();
	virtual int OnExit();

	virtual int OnGetLanguage() const;
	virtual wxString OnGetLangConfigName() const;

	virtual bool InitLocale();
	virtual bool SetLang(int langId);

	DECLARE_EVENT_TABLE()
	void OnQueryEndSession(wxCloseEvent& evt);
	void OnEndSession(wxCloseEvent& evt);

	CLocalization m_Localization;
	CUpdateInfo m_UpdateInfo;
	CAgilityBookDocManager* m_manager;
	wxPrintDialogData* m_printDialogData;
	CHtmlEasyPrinting* m_Prn;
};

DECLARE_APP(CAgilityBookApp)
