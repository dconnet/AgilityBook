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
 * 2016-09-02 Add support for scrolling on touch (or mouse drag).
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
	DECLARE_NO_COPY_IMPLEMENTED(CAgilityBookCalendarView)

public:
	CAgilityBookCalendarView(
			CTabView* pTabView,
			wxDocument* doc);
	~CAgilityBookCalendarView();

	bool Create(
			CBasePanel* parentView,
			wxWindow* parentCtrl,
			wxDocument* doc,
			long flags,
			wxSizer* sizer,
			int proportion = 0,
			int sizerFlags = 0,
			int border = 0) override;
	wxWindow* GetControl() override;
	bool HasPrevPane() const override	{return true;}
	bool PrevPane() override;
	bool HasNextPane() const override	{return true;}
	bool NextPane() override;
	void DetachView() override;

	ARBDate GetCurrentDate() const;
	bool SetCurrentDate(
			ARBDate const& date,
			bool bEnsureVisible);

	bool IsFiltered() const override;
	bool GetMessage(std::wstring& msg) const override;
	bool GetMessage2(std::wstring& msg) const override;
	bool AllowStatusContext(int field) const override;

	bool OnCreate(
			wxDocument* doc,
			long flags) override;
	void DoActivateView(
			bool activate,
			wxView* activeView,
			wxView* deactiveView) override;
	void OnDraw(wxDC* dc) override;
	void OnUpdate(
			wxView* sender,
			wxObject* inHint = nullptr) override;

private:
	void SetupColumns();
	void LoadData();

	CAgilityBookCalendar* m_Ctrl;
	bool m_bTracking;
	wxPoint m_lastPt;
	int m_motionDelta;

	DECLARE_EVENT_TABLE()
	void OnContextMenu(wxContextMenuEvent& evt);
	void OnCtrlMouseEvent(wxMouseEvent& evt);
	void OnCtrlMouseMove(wxMouseEvent& evt);
#if wxUSE_MOUSEWHEEL
	void OnCtrlMouseWheel(wxMouseEvent& evt);
#endif
	void OnCtrlChar(wxKeyEvent& evt);
	void OnViewUpdateCmd(wxUpdateUIEvent& evt);
	void OnViewCmd(wxCommandEvent& evt);
};
