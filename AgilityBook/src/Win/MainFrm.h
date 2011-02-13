#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
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
#include <wx/dnd.h>
#include <wx/docview.h>

#define STATUS_INFO			0
#define STATUS_DOG			1
#define STATUS_STATUS		2
#define STATUS_FILTERED		3
#if wxCHECK_VERSION(2, 9, 0)
#define NUM_STATUS_FIELDS	4
#else
#define STATUS_FILLER		4
#define NUM_STATUS_FIELDS	5
#endif


class CFileDropTarget : public wxFileDropTarget
{
public:
	CFileDropTarget(wxDocManager* docMgr)
		: m_docMgr(docMgr)
	{
	}
	virtual bool OnDropFiles(
			wxCoord x,
			wxCoord y,
			wxArrayString const& filenames);
private:
	wxDocManager* m_docMgr;
};


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

	wxDocManager* m_manager;
	CAgilityBookMenu m_MenuBar;
#ifdef WIN32
	wxFont m_fontStatusBar;
#endif
	int m_Widths[NUM_STATUS_FIELDS];

private:
	DECLARE_EVENT_TABLE()
	void OnStatusBarContextMenu(wxContextMenuEvent& evt);
	void OnClose(wxCloseEvent& evt);
#if defined(__WXMAC__)
	void OnIdle(wxIdleEvent& evt);
#endif
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
