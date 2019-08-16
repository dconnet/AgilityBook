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
#include "LibARBWin/StatusBarHelper.h"
#include <wx/dnd.h>
#include <wx/docview.h>

#define STATUS_INFO			0
#define STATUS_DOG			1
#define STATUS_STATUS		2
#define STATUS_FILTERED		3
#define NUM_STATUS_FIELDS	4


#if wxUSE_DRAG_AND_DROP

class CFileDropTarget : public wxFileDropTarget
{
public:
	CFileDropTarget(wxDocManager* docMgr)
		: m_docMgr(docMgr)
	{
	}
	bool OnDropFiles(
			wxCoord x,
			wxCoord y,
			wxArrayString const& filenames) override;
private:
	wxDocManager* m_docMgr;
};

#endif

class CMainFrame : public wxDocParentFrame
{
	DECLARE_NO_COPY_IMPLEMENTED(CMainFrame)

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
		void Notify() override;
	} m_timerStartup;
	wxDocManager* m_manager;
	CStatusBarHelper m_Widths;

private:
	DECLARE_EVENT_TABLE()
#ifdef EVT_DPI_CHANGED
	void OnDPIChanged(wxDPIChangedEvent& evt);
#endif
	void OnStatusBarContextMenu(wxContextMenuEvent& evt);
	void OnStatusBarDblClick(wxMouseEvent& evt);
	void OnClose(wxCloseEvent& evt);
	void OnUpdateCmd(wxUpdateUIEvent& evt);
	void OnFileLanguageChoose(wxCommandEvent& evt);
	void OnPrintBlankRuns(wxCommandEvent& evt);
	void OnQuit(wxCommandEvent& evt);
	void OnType(wxCommandEvent& evt);
	void OnOrient(wxCommandEvent& evt);
	void OnNextPane(wxCommandEvent& evt);
	void OnPrevPane(wxCommandEvent& evt);
	void OnViewCustomizeAccel(wxCommandEvent& evt);
	void OnHelpSysinfo(wxCommandEvent& evt);
};
