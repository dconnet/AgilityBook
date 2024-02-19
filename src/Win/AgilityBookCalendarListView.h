#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CAgilityBookCalendarListView class
 * @author David Connet
 *
 * Revision History
 * 2009-02-04 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-27 Add color coding to calendar for entries that need attention.
 * 2005-01-25 Remember the sort column between program invocations.
 * 2004-12-31 Make F1 invoke context help.
 * 2004-06-24 Added a sort header image.
 * 2004-04-15 Added Duplicate menu item.
 * 2004-04-06 Added simple sorting by column.
 * 2003-12-27 Implemented Find/FindNext.
 * 2003-11-21 Enabled copy and select all.
 * 2003-08-27 Cleaned up selection synchronization.
 */

#include "CommonView.h"

#include "LibARBWin/DlgFind.h"
#include "LibARBWin/ListData.h"
#include "LibARBWin/ReportListCtrl.h"
#include <wx/docview.h>


namespace dconSoft
{
class CAgilityBookCalendarListView;
class CAgilityBookCalendarListViewData;
class CTabView;

typedef std::shared_ptr<CAgilityBookCalendarListViewData> CAgilityBookCalendarListViewDataPtr;


class CFindCalendar : public ARBWin::CFindCallback
{
public:
	CFindCalendar(CAgilityBookCalendarListView* pView)
		: m_pView(pView)
	{
	}
	bool Search(ARBWin::CDlgFind* pDlg) override;

private:
	CAgilityBookCalendarListView* m_pView;
};


class CAgilityBookCalendarListView : public CAgilityBookBaseExtraView
{
	friend int wxCALLBACK CompareCalendar(
		ARBWin::CListDataPtr const& item1,
		ARBWin::CListDataPtr const& item2,
		ARBWin::SortInfo const* pSortInfo);
	friend class CAgilityBookCalendarListViewData;
	friend class CFindCalendar;
	DECLARE_CLASS(CAgilityBookCalendarListView)
	DECLARE_NO_COPY_IMPLEMENTED(CAgilityBookCalendarListView)

public:
	CAgilityBookCalendarListView(CTabView* pTabView, wxDocument* doc);
	~CAgilityBookCalendarListView();

	bool Create(
		CBasePanel* parentView,
		wxWindow* parentCtrl,
		wxDocument* doc,
		long flags,
		wxSizer* sizer,
		int proportion = 0,
		int sizerFlags = 0,
		int border = 0) override;
	wxWindow* GetControl() override
	{
		return m_Ctrl;
	}
	bool HasPrevPane() const override
	{
		return true;
	}
	bool PrevPane() override;
	bool HasNextPane() const override
	{
		return true;
	}
	bool NextPane() override;
	void DetachView() override;

	bool IsFiltered() const override;
	bool GetMessage(std::wstring& msg) const override;
	bool GetMessage2(std::wstring& msg) const override;
	bool AllowStatusContext(int field) const override;

	bool OnCreate(wxDocument* doc, long flags) override;
	void DoActivateView(bool activate, wxView* activeView, wxView* deactiveView) override;
	void OnDraw(wxDC* dc) override;
	void OnUpdate(wxView* sender, wxObject* inHint = nullptr) override;

	void SuppressSelect(bool bSuppress)
	{
		m_bSuppressSelect = bSuppress;
	}
	void GetPrintLine(long item, std::vector<std::wstring>& line) const;

private:
	CAgilityBookCalendarListViewDataPtr GetItemCalData(long index) const;
	void SetupColumns();
	void LoadData();
	bool OnCmd(int id);

	ARBWin::CReportListCtrl* m_Ctrl;
	int m_imgTentative;
	int m_imgPlan;
	int m_imgPlanTentative;
	int m_imgPending;
	int m_imgPendingTentative;
	int m_imgEntered;
	int m_imgEnteredTentative;
	int m_imgAccomNone;
	int m_imgAccomTodo;
	int m_imgAccomConfirm;
	bool m_bSuppressSelect;
	std::vector<long> m_Columns;
	CFindCalendar m_Callback;

	class CSortColumn
	{
		DECLARE_NO_COPY_IMPLEMENTED(CSortColumn)
	public:
		CSortColumn(std::vector<long>& inColumns);
		void Initialize();
		long GetColumn() const
		{
			return m_iCol;
		}
		void SetColumn(long iCol);

	private:
		long LookupColumn(long iCol) const;
		std::vector<long>& m_Columns;
		long m_iCol;
	} m_SortColumn;

	DECLARE_EVENT_TABLE()
	void OnCtrlSetFocus(wxFocusEvent& evt);
	void OnCtrlColumnClick(wxListEvent& evt);
	void OnCtrlItemSelected(wxListEvent& evt);
	void OnCtrlItemActivated(wxListEvent& evt);
	void OnCtrlKeyDown(wxKeyEvent& evt);
	void OnViewContextMenu(wxContextMenuEvent& evt);
	void OnViewUpdateCmd(wxUpdateUIEvent& evt);
	void OnViewCmd(wxCommandEvent& evt);
	void OnPrintView(wxCommandEvent& evt);
	void OnPreview(wxCommandEvent& evt);
};

} // namespace dconSoft
