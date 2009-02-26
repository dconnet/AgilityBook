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
 * @brief interface of the CMainFrame class
 * @author David Connet
 *
 * Revision History
 * @li 2009-01-06 DRC Ported to wxWidgets.
 * @li 2008-11-19 DRC Added context menus to status bar.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "AgilityBookMenu.h"
#include <wx/docview.h>

#define NUM_STATUS_FIELDS	5
#define STATUS_INFO			0
#define STATUS_DOG			1
#define STATUS_STATUS		2
#define STATUS_FILTERED		3
#define STATUS_FILLER		4


class CMainFrame : public wxDocParentFrame
{
	DECLARE_NO_COPY_CLASS(CMainFrame)

public:
	CMainFrame(wxDocManager* manager);
	~CMainFrame();

	void SetMessageText(
			wxString const& msg,
			bool bFiltered);
	void SetMessageText2(wxString const& msg);

private:
	void SetMessage(wxString const& msg, int index, bool bResize);

	CAgilityBookMenu m_MenuBar;
	int m_Widths[NUM_STATUS_FIELDS];

private:
	DECLARE_EVENT_TABLE()
	void OnStatusBarContextMenu(wxContextMenuEvent& evt);
	void OnClose(wxCloseEvent& evt);
	void OnUpdateCmd(wxUpdateUIEvent& evt);
	void OnFileLanguageChoose(wxCommandEvent& evt);
	void OnPrintBlankRuns(wxCommandEvent& evt);
	void OnQuit(wxCommandEvent& evt);
	void OnType(wxCommandEvent& evt);
	void OnOrient(wxCommandEvent& evt);
	void OnNextPane(wxCommandEvent& evt);
	void OnPrevPane(wxCommandEvent& evt);
	void OnHelpSysinfo(wxCommandEvent& evt);
};
