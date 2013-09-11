#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CAgilityBookCalendarListView class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-04 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-27 DRC Add color coding to calendar for entries that need attention.
 * @li 2005-01-25 DRC Remember the sort column between program invocations.
 * @li 2004-12-31 DRC Make F1 invoke context help.
 * @li 2004-06-24 DRC Added a sort header image.
 * @li 2004-04-15 DRC Added Duplicate menu item.
 * @li 2004-04-06 DRC Added simple sorting by column.
 * @li 2003-12-27 DRC Implemented Find/FindNext.
 * @li 2003-11-21 DRC Enabled copy and select all.
 * @li 2003-08-27 DRC Cleaned up selection synchronization.
 */

#include "CommonView.h"
#include "DlgFind.h"
#include "ListCtrl.h"
#include <wx/docview.h>
class CAgilityBookCalendarListView;
class CAgilityBookCalendarListViewData;
class CTabView;

typedef std::tr1::shared_ptr<CAgilityBookCalendarListViewData> CAgilityBookCalendarListViewDataPtr;


class CFindCalendar : public IFindCallback
{
public:
	CFindCalendar(CAgilityBookCalendarListView* pView)
		: m_pView(pView)
	{
	}
	virtual bool Search(CDlgFind* pDlg) const;
private:
	CAgilityBookCalendarListView* m_pView;
};


class CAgilityBookCalendarListView : public CAgilityBookBaseExtraView
{
#if wxCHECK_VERSION(3, 0, 0)
	friend int wxCALLBACK CompareCalendar(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData);
#else
	friend int wxCALLBACK CompareCalendar(long item1, long item2, long sortData);
#endif
	friend class CAgilityBookCalendarListViewData;
	friend class CFindCalendar;
	DECLARE_CLASS(CAgilityBookCalendarListView)
	CAgilityBookCalendarListView(
			CTabView* pTabView,
			wxDocument* doc);

public:
	~CAgilityBookCalendarListView();

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
	virtual bool HasPrevPane() const	{return true;}
	virtual bool PrevPane();
	virtual bool HasNextPane() const	{return true;}
	virtual bool NextPane();
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
			wxView* deactiveView);
	virtual void OnDraw(wxDC* dc);
	virtual void OnUpdate(
			wxView* sender,
			wxObject* inHint = NULL);

	void SuppressSelect(bool bSuppress)		{m_bSuppressSelect = bSuppress;}
	void GetPrintLine(long item, std::vector<std::wstring>& line) const;

private:
	CAgilityBookCalendarListViewDataPtr GetItemCalData(long index) const;
	CAgilityBookCalendarListViewDataPtr GetItemCalDataByData(long data) const;
	void SetupColumns();
	void LoadData();
	bool OnCmd(int id);

	CReportListCtrl* m_Ctrl;
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
		long GetColumn() const		{return m_iCol;}
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
	void OnPrint(wxCommandEvent& evt);
	void OnPreview(wxCommandEvent& evt);
};
