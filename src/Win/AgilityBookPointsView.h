#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CAgilityBookPointsView class
 * @author David Connet
 *
 * Revision History
 * 2009-02-04 Ported to wxWidgets.
 * 2006-07-16 Let user specify how to sort events within a venue.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-10-14 Added a context menu.
 * 2004-12-31 Make F1 invoke context help.
 * 2004-12-03 Show all lifetime points when filtering.
 * 2004-08-12 Allow creating a new title.
 * 2004-05-27 Changed lifetime points structure to take a string.
 */

#include "CommonView.h"
#include "ListCtrl.h"
#include <wx/docview.h>
class CAgilityBookPointsViewData;

typedef std::shared_ptr<CAgilityBookPointsViewData> CAgilityBookPointsViewDataPtr;


class CAgilityBookPointsView : public CAgilityBookBaseExtraView
{
	DECLARE_CLASS(CAgilityBookPointsView)
	CAgilityBookPointsView(
			CTabView* pTabView,
			wxDocument* doc);

public:
	~CAgilityBookPointsView();

	virtual bool Create(
			CBasePanel* parentView,
			wxWindow* parentCtrl,
			wxDocument* doc,
			long flags,
			wxSizer* sizer,
			int proportion = 0,
			int sizerFlags = 0,
			int border = 0);
	virtual wxWindow* GetControl()		{return m_Ctrl;}
	virtual void DetachView();

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
			wxView *deactiveView);
	virtual void OnDraw(wxDC* dc);
	virtual void OnUpdate(
			wxView* sender,
			wxObject* inHint = nullptr);

	void GetPrintLine(long item, std::vector<std::wstring>& line) const;

private:
	CAgilityBookPointsViewDataPtr GetItemPointsData(long index) const;
	void SetupColumns();
	void LoadData();
	bool OnCmd(int id);

	CReportListCtrl* m_Ctrl;

	DECLARE_EVENT_TABLE()
	void OnCtrlItemActivated(wxListEvent& evt);
	void OnCtrlKeyDown(wxKeyEvent& evt);
	void OnViewContextMenu(wxContextMenuEvent& evt);
	void OnViewUpdateCmd(wxUpdateUIEvent& evt);
	void OnViewCmd(wxCommandEvent& evt);
	void OnPrint(wxCommandEvent& evt);
	void OnPreview(wxCommandEvent& evt);
};
