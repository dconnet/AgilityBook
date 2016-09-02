#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CAgilityBookViewCalendar class
 * @author David Connet
 *
 * Revision History
 * 2009-02-10 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-12-31 Make F1 invoke context help.
 * 2003-11-21 Enabled copy and select all.
 * 2003-10-29 Support the mouse wheel for scrolling.
 * 2003-10-22 Right click sets the current date.
 * 2003-08-27 Cleaned up selection synchronization.
 */

#include "CommonView.h"

#include "ARBCommon/ARBDate.h"
#include <wx/docview.h>
class CAgilityBookCalendar;


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

	ARBDate GetCurrentDate() const;
	bool SetCurrentDate(
			ARBDate const& date,
			bool bEnsureVisible);

	virtual bool IsFiltered() const;
	virtual bool GetMessage(std::wstring& msg) const;
	virtual bool GetMessage2(std::wstring& msg) const;
	virtual bool AllowStatusContext(int field) const;

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
			wxObject* inHint = nullptr);

private:
	void SetupColumns();
	void LoadData();

	CAgilityBookCalendar* m_Ctrl;

	DECLARE_EVENT_TABLE()
	void OnContextMenu(wxContextMenuEvent& evt);
	void OnCtrlMouseEvent(wxMouseEvent& evt);
	void OnCtrlMouseDrag(wxMouseEvent& evt);
#if wxUSE_MOUSEWHEEL
	void OnCtrlMouseWheel(wxMouseEvent& evt);
#endif
	void OnCtrlChar(wxKeyEvent& evt);
	void OnViewUpdateCmd(wxUpdateUIEvent& evt);
	void OnViewCmd(wxCommandEvent& evt);
};
