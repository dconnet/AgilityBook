#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CMainFrame class
 * @author David Connet
 *
 * Revision History
 * 2009-01-06 Ported to wxWidgets.
 * 2008-11-19 Added context menus to status bar.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 */

#include "AgilityBookMenu.h"
#include "MenuHelper.h"
#include <wx/dnd.h>
#include <wx/docview.h>

#define STATUS_INFO			0
#define STATUS_DOG			1
#define STATUS_STATUS		2
#define STATUS_FILTERED		3
#if wxCHECK_VERSION(3, 0, 0)
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
#ifdef __WXMSW__
	virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
#endif

public:
	CMainFrame(wxDocManager* manager);
	~CMainFrame();

	void SetMessageText(
			std::wstring const& msg,
			bool bFiltered);
	void SetMessageText2(std::wstring const& msg);

private:
	void SetMessage(std::wstring const& msg, int index, bool bResize);

	class CStartupEvent : public wxTimer
	{
	public:
		virtual void Notify();
	} m_timerStartup;
	wxDocManager* m_manager;
	CMenuHelper m_menus;
#ifdef __WXMSW__
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
#ifdef _DEBUG
	void OnHelpDebug(wxCommandEvent& evt);
#endif
};
