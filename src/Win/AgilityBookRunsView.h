#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CAgilityBookViewRuns class
 * @author David Connet
 *
 * Revision History
 * 2009-02-04 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-01-25 Remember the sort column between program invocations.
 * 2004-12-31 Make F1 invoke context help.
 * 2004-06-24 Added a sort header image.
 * 2004-04-06 Added simple sorting by column.
 * 2003-12-27 Implemented Find/FindNext.
 * 2003-08-30 Added the ability to copy entries to the clipboard.
 * 2003-08-27 Cleaned up selection synchronization.
 */

#include "CommonView.h"
#include "DlgFind.h"
#include "ListCtrl.h"
#include <wx/docview.h>
class CAgilityBookRunsView;
class CAgilityBookRunsViewData;

typedef std::shared_ptr<CAgilityBookRunsViewData> CAgilityBookRunsViewDataPtr;


class CFindRuns : public IFindCallback
{
public:
	CFindRuns(CAgilityBookRunsView* pView)
		: m_pView(pView)
	{
	}
	virtual bool Search(CDlgFind* pDlg) const;
private:
	CAgilityBookRunsView* m_pView;
};


class CAgilityBookRunsView : public CAgilityBookBaseExtraView
{
#if wxCHECK_VERSION(3, 0, 0)
	friend int wxCALLBACK CompareRuns(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData);
#else
	friend int wxCALLBACK CompareRuns(long item1, long item2, long sortData);
#endif
	friend class CAgilityBookRunsViewData;
	friend class CFindRuns;
	DECLARE_CLASS(CAgilityBookRunsView)
	CAgilityBookRunsView(
			CTabView* pTabView,
			wxDocument* doc);

public:
	~CAgilityBookRunsView();

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

	bool IsTrial(ARBDogTrialPtr pTrial) const;

	void SuppressSelect(bool bSuppress)		{m_bSuppressSelect = bSuppress;}
	void GetPrintLine(long item, std::vector<std::wstring>& line) const;

private:
	CAgilityBookRunsViewDataPtr GetItemRunData(long index) const;
#if wxCHECK_VERSION(3, 0, 0)
	CAgilityBookRunsViewDataPtr GetItemRunDataByData(wxUIntPtr data) const;
#else
	CAgilityBookRunsViewDataPtr GetItemRunDataByData(long data) const;
#endif
	void SetupColumns();
	void LoadData();
	bool OnCmd(int id);

	CReportListCtrl* m_Ctrl;
	int m_imgCourse;
	int m_imgMap;
	bool m_bSuppressSelect;
	std::vector<long> m_Columns;
	CFindRuns m_Callback;

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
