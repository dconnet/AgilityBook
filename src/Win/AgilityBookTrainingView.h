#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CAgilityBookTrainingView class
 * @author David Connet
 *
 * Revision History
 * 2009-02-04 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-01-25 Remember the sort column between program invocations.
 * 2004-12-31 Make F1 invoke context help.
 * 2004-06-24 Added a sort header image.
 * 2004-04-15 Added Duplicate menu item.
 * 2004-04-06 Added simple sorting by column.
 * 2003-12-27 Implemented Find/FindNext.
 * 2003-09-21 Created
 */

#include "CommonView.h"
#include "DlgFind.h"

#include "LibARBWin/ReportListCtrl.h"
#include <wx/docview.h>
class CAgilityBookTrainingView;
class CAgilityBookTrainingViewData;

typedef std::shared_ptr<CAgilityBookTrainingViewData> CAgilityBookTrainingViewDataPtr;


class CFindTraining : public IFindCallback
{
public:
	CFindTraining(CAgilityBookTrainingView* pView)
		: m_pView(pView)
	{
	}
	bool Search(CDlgFind* pDlg) const override;

private:
	CAgilityBookTrainingView* m_pView;
};


class CAgilityBookTrainingView : public CAgilityBookBaseExtraView
{
	friend class CAgilityBookTrainingViewData;
	friend class CFindTraining;
	friend int wxCALLBACK
	CompareTraining(CListDataPtr const& item1, CListDataPtr const& item2, SortInfo const* pSortInfo);
	DECLARE_CLASS(CAgilityBookTrainingView)
	DECLARE_NO_COPY_IMPLEMENTED(CAgilityBookTrainingView)

public:
	CAgilityBookTrainingView(CTabView* pTabView, wxDocument* doc);
	~CAgilityBookTrainingView();

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
	void DetachView() override;

	void SetCurrentDate(ARBDate const& inDate);

	bool IsFiltered() const override;
	bool GetMessage(std::wstring& msg) const override;
	bool GetMessage2(std::wstring& msg) const override;
	bool AllowStatusContext(int field) const override;

	bool OnCreate(wxDocument* doc, long flags) override;
	void DoActivateView(bool activate, wxView* activeView, wxView* deactiveView) override;
	void OnDraw(wxDC* dc) override;
	void OnUpdate(wxView* sender, wxObject* inHint = nullptr) override;

	void GetPrintLine(long item, std::vector<std::wstring>& line) const;

private:
	CAgilityBookTrainingViewDataPtr GetItemTrainingData(long index) const;
	void SetupColumns();
	void LoadData();
	bool OnCmd(int id);

	CReportListCtrl* m_Ctrl;
	std::vector<long> m_Columns;
	CFindTraining m_Callback;

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
	void OnCtrlContextMenu(wxListEvent& evt);
	void OnCtrlColumnClick(wxListEvent& evt);
	void OnCtrlItemActivated(wxListEvent& evt);
	void OnCtrlKeyDown(wxKeyEvent& evt);
	void OnViewContextMenu(wxContextMenuEvent& evt);
	void OnViewUpdateCmd(wxUpdateUIEvent& evt);
	void OnViewCmd(wxCommandEvent& evt);
	void OnPrintView(wxCommandEvent& evt);
	void OnPreview(wxCommandEvent& evt);
};
