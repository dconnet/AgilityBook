#pragma once

/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
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
 * @brief main header file for the AgilityBook application
 * @author David Connet
 *
 * Revision History
 * @li 2008-12-14 DRC Ported to wxWidgets.
 * @li 2005-10-19 DRC Fixed a problem with CFile::GetStatus (see AgilityBook.cpp).
 * @li 2004-06-02 DRC Moved ShellExecute code here.
 */

#include "UpdateInfo.h"
class CAgilityBookDoc;
class CAgilityBookDocManager;
class CLanguageManager;
class wxPrintDialogData;


class CAgilityBookApp : public wxApp
{
	DECLARE_NO_COPY_CLASS(CAgilityBookApp)
public:
	CAgilityBookApp();

	bool SelectLanguage(wxWindow* parent = NULL);
	void AutoCheckConfiguration(CAgilityBookDoc* pDoc);
	void UpdateConfiguration(CAgilityBookDoc* pDoc);

	void SetMessageText(
			wxString const& msg,
			bool bFiltered);
	void SetMessageText2(wxString const& msg);

	wxPrintDialogData* GetPrintData();
	void SavePrintData(wxPrintDialogData const& data);

protected:
	virtual bool OnInit();
	virtual int OnExit();

	CLanguageManager* m_LangMgr;
	CUpdateInfo m_UpdateInfo;
	CAgilityBookDocManager* m_manager;
	wxPrintDialogData* m_printDialogData;
};

DECLARE_APP(CAgilityBookApp)
