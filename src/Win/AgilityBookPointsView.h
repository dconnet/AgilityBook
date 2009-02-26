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

typedef tr1::shared_ptr<CAgilityBookPointsViewData> CAgilityBookPointsViewDataPtr;


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
	void OnCtrlDoubleClick(wxMouseEvent& evt);
	void OnCtrlKeyDown(wxListEvent& evt);
	void OnViewContextMenu(wxContextMenuEvent& evt);
	void OnViewUpdateCmd(wxUpdateUIEvent& evt);
	void OnViewCmd(wxCommandEvent& evt);
};
