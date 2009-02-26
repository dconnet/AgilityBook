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
 * @brief interface of the CAgilityBookViewCalendar class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-10 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-12-31 DRC Make F1 invoke context help.
 * @li 2003-11-21 DRC Enabled copy and select all.
 * @li 2003-10-29 DRC Support the mouse wheel for scrolling.
 * @li 2003-10-22 DRC Right click sets the current date.
 * @li 2003-08-27 DRC Cleaned up selection synchronization.
 */

#include "ARBDate.h"
#include "CommonView.h"
#include <wx/docview.h>


class CAgilityBookCalendarView : public CAgilityBookBaseExtraView
{
	friend class CAgilityBookCalendar;
	DECLARE_CLASS(CAgilityBookCalendarView)
	CAgilityBookCalendarView(
			CTabView* pTabView,
			wxDocument* doc);

public:
	~CAgilityBookCalendarView();

	virtual bool Create(
			CBasePanel* parentView,
			wxWindow* parentCtrl,
			wxDocument* doc,
			long flags,
			wxSizer* sizer,
			int proportion = 0,
			int sizerFlags = 0,
			int border = 0);
	virtual wxWindow* GetControl();
	virtual bool HasPrevPane() const	{return true;}
	virtual bool PrevPane();
	virtual bool HasNextPane() const	{return true;}
	virtual bool NextPane();
	virtual void DetachView();

	bool SetCurrentDate(
			ARBDate const& date,
			bool bEnsureVisible);

	virtual bool IsFiltered() const;
	virtual bool GetMessage(wxString& msg) const;
	virtual bool GetMessage2(wxString& msg) const;

	virtual bool OnCreate(
			wxDocument* doc,
			long flags);
	virtual void DoActivateView(
			bool activate,
			wxView* activeView,
			wxView* deactiveView);
	virtual void OnDraw(wxDC* dc);
	virtual void OnUpdate(
			wxView* sender,
			wxObject* inHint = NULL);

private:
	void SetupColumns();
	void LoadData();

	CAgilityBookCalendar* m_Ctrl;

	DECLARE_EVENT_TABLE()
	void OnContextMenu(wxContextMenuEvent& evt);
	void OnCtrlMouseEvent(wxMouseEvent& evt);
#if wxUSE_MOUSEWHEEL
	void OnCtrlMouseWheel(wxMouseEvent& evt);
#endif
	void OnCtrlKeyDown(wxKeyEvent& evt);
	void OnViewUpdateCmd(wxUpdateUIEvent& evt);
	void OnViewCmd(wxCommandEvent& evt);
};
