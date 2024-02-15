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
 * 2014-05-21 Enable paste of copied runs.
 * 2014-04-12 Implement unsorting.
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

#include "LibARBWin/ReportListCtrl.h"
#include <wx/docview.h>


namespace dconSoft
{
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
	bool Search(CDlgFind* pDlg) override;

private:
	CAgilityBookRunsView* m_pView;
};


class CAgilityBookRunsView : public CAgilityBookBaseExtraView
{
	friend int wxCALLBACK CompareRuns(
		ARBWin::CListDataPtr const& item1,
		ARBWin::CListDataPtr const& item2,
		ARBWin::SortInfo const* pSortInfo);
	friend class CAgilityBookRunsViewData;
	friend class CFindRuns;
	DECLARE_CLASS(CAgilityBookRunsView)
	DECLARE_NO_COPY_IMPLEMENTED(CAgilityBookRunsView)

public:
	CAgilityBookRunsView(CTabView* pTabView, wxDocument* doc);
	~CAgilityBookRunsView();

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

	bool IsTrial(ARB::ARBDogTrialPtr const& inTrial) const;

	void SuppressSelect(bool bSuppress)
	{
		m_bSuppressSelect = bSuppress;
	}
	void GetPrintLine(long item, std::vector<std::wstring>& line) const;

private:
	CAgilityBookRunsViewDataPtr GetItemRunData(long index) const;
	bool GetUnifiedTrial(ARB::ARBDogPtr& pDog, ARB::ARBDogTrialPtr& pTrial, bool bSelectionOnly = true) const;
	void SetupColumns();
	void LoadData();
	void SetSelectedRun(ARB::ARBDogRunPtr pRun);
	bool OnCmd(int id, bool bSilent = false);

	ARBWin::CReportListCtrl* m_Ctrl;
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
		bool IsSorted() const
		{
			return m_bIsSorted;
		}
		long GetColumn() const
		{
			return m_iCol;
		}
		void SetSorted(bool bSorted);
		void SetColumn(long iCol);

	private:
		long LookupColumn(long iCol) const;
		std::vector<long>& m_Columns;
		long m_iCol;
		bool m_bIsSorted;
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
