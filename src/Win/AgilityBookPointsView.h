#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CAgilityBookPointsView class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-04 DRC Ported to wxWidgets.
 * @li 2006-07-16 DRC Let user specify how to sort events within a venue.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-10-14 DRC Added a context menu.
 * @li 2004-12-31 DRC Make F1 invoke context help.
 * @li 2004-12-03 DRC Show all lifetime points when filtering.
 * @li 2004-08-12 DRC Allow creating a new title.
 * @li 2004-05-27 DRC Changed lifetime points structure to take a string.
 */

#include "CommonView.h"
#include "ListCtrl.h"
#include <wx/docview.h>
class CAgilityBookPointsViewData;

typedef std::tr1::shared_ptr<CAgilityBookPointsViewData> CAgilityBookPointsViewDataPtr;


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
	virtual bool GetMessage(wxString& msg) const;
	virtual bool GetMessage2(wxString& msg) const;
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
			wxObject* inHint = NULL);

	void GetPrintLine(long item, std::vector<wxString>& line) const;

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
