#pragma once
#if USE_TREELIST

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CAgilityBookTree class
 * @author David Connet
 *
 * Revision History
 * 2013-04-22 Converted tree+list into single control.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-08 Ported to wxWidgets.
 * 2008-11-19 Added SelectDog()
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-12-31 Make F1 invoke context help.
 * 2003-12-30 Implemented customized text in tree.
 * 2003-12-28 Implemented Find/FindNext.
 * 2003-08-30 Added GetPrintLine to allow future differences between
 *            printing and viewing (already in the listctrl)
 * 2003-08-28 Added printing.
 * 2003-08-27 Cleaned up selection synchronization.
 * 2003-08-24 Optimized filtering by adding boolean into ARBBase to
 *            prevent constant re-evaluation.
 */

#include "AgilityBookTreeListModel.h"
#include "CommonView.h"
#include "DlgFind.h"
#include "IconList.h"
#include <vector>
#include <wx/docview.h>
class CAgilityBookTreeListData;
class CAgilityBookTreeListView;


class CFindTree : public IFindCallback
{
public:
	CFindTree(CAgilityBookTreeListView* pView)
		: m_pView(pView)
	{
	}
	virtual bool Search(CDlgFind* pDlg) const;
private:
	void FillTree(wxDataViewItem item) const;
	wxDataViewItem GetNextItem() const;
	CAgilityBookTreeListView* m_pView;
	mutable std::vector<wxDataViewItem> m_Items;
	mutable std::vector<wxDataViewItem>::const_iterator m_Iter;
};


class CAgilityBookTreeCtrl : public wxDataViewCtrl
{
	DECLARE_DYNAMIC_CLASS_NO_ASSIGN(CAgilityBookTreeCtrl)
public:
	CAgilityBookTreeCtrl();
	CAgilityBookTreeCtrl(
			wxWindow* parent,
			wxWindowID id,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxDV_MULTIPLE | wxDV_ROW_LINES,
			const wxValidator& validator = wxDefaultValidator);
	~CAgilityBookTreeCtrl();

	bool Create(
			wxWindow *parent,
			wxWindowID id,
			const wxPoint& pos,
			const wxSize& size,
			long style,
			const wxValidator& validator);

	bool Unsort();

	void CollapseAllChildren(wxDataViewItem const& item);
	void ExpandAllChildren(wxDataViewItem const& item);

	CAgilityBookTreeListModel* GetStore();
	const CAgilityBookTreeListModel* GetStore() const;
};


class CAgilityBookTreeListView : public CAgilityBookBaseExtraView
{
	friend class CAgilityBookTreeListData;
	friend class CFindTree;
	DECLARE_CLASS(CAgilityBookTreeListView)
	CAgilityBookTreeListView(
			CTabView* pTabView,
			wxDocument* doc);

public:
	~CAgilityBookTreeListView();

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

	// Allow access to tree control
	void Freeze()								{m_Ctrl->Freeze();}
	void Thaw()									{m_Ctrl->Thaw();}
	void Refresh()								{m_Ctrl->Refresh();}
	wxDataViewItem GetSelection() const;
	void RefreshItem(wxDataViewItem const& item);
	CAgilityBookTreeListModel* GetStore()			{return m_Ctrl->GetStore();}
	const CAgilityBookTreeListModel* GetStore() const	{return m_Ctrl->GetStore();}
	wxEvtHandler* GetEventHandler() const		{return m_Ctrl;}

	wxDataViewItem FindData(ARBBasePtr pBase) const;
	wxDataViewItem FindData(
			wxDataViewItem const& inItem,
			ARBBasePtr pBase) const;
	wxDataViewItem FindData(ARBDogPtr pDog) const;
	wxDataViewItem FindData(
			wxDataViewItem const& inItem,
			ARBDogPtr pDog) const;
	wxDataViewItem FindData(ARBDogTrialPtr pTrial) const;
	wxDataViewItem FindData(
			wxDataViewItem const& inItem,
			ARBDogTrialPtr pTrial) const;
	wxDataViewItem FindData(ARBDogRunPtr pRun) const;
	wxDataViewItem FindData(
			wxDataViewItem const& inItem,
			ARBDogRunPtr pRun) const;

	bool SelectDog(ARBDogPtr);
	wxDataViewItem InsertDog(
			ARBDogPtr pDog,
			bool bSelect = false);
	wxDataViewItem InsertTrial(
			ARBDogPtr pDog,
			ARBDogTrialPtr pTrial,
			wxDataViewItem parent);
	wxDataViewItem InsertRun(
			ARBDogPtr pDog,
			ARBDogTrialPtr pTrial,
			ARBDogRunPtr pRun,
			wxDataViewItem parent);
	std::wstring GetPrintLine(wxDataViewItem const& item) const;

private:
	void GetQCount(
			int& ioCount,
			int& ioTotal) const;
	bool GetUnifiedDog(
			wxDataViewItemArray const& sel,
			ARBDogPtr& pDog) const;
	bool GetUnifiedTrial(
			wxDataViewItemArray const& sel,
			ARBDogPtr& pDog,
			ARBDogTrialPtr& pTrial) const;
	ARBDogRunPtr GetFirstRun(wxDataViewItemArray const& sel);
	void ChangeSelection(
			wxDataViewItem const& item,
			bool bEnsureVisible = true);
	void DoSelectionChange(wxDataViewItem const& item);
	bool EditDog(ARBDogPtr pDog);
	bool EditTrial(
			ARBDogPtr pDog,
			ARBDogTrialPtr pTrial);
	bool EditRun(
			ARBDogPtr pDog,
			ARBDogTrialPtr pTrial,
			ARBDogRunPtr pRun);
	bool DoEdit(
			wxDataViewItem const& item,
			CTreeListDataType type);
	wxDataViewItem LoadData(
			ARBDogPtr pDog);
	wxDataViewItem LoadData(
			ARBDogPtr pDog,
			ARBDogTrialPtr pTrial,
			wxDataViewItem parent);
	wxDataViewItem LoadData(
			ARBDogPtr pDog,
			ARBDogTrialPtr pTrial,
			ARBDogRunPtr pRun,
			wxDataViewItem parent);
	void LoadData(bool bColumns);
	void PrintLine(
			std::wostringstream& data,
			wxDataViewItem item,
			int indent) const;
	std::wstring GetPrintDataAsHtmlTable() const;

	CAgilityBookTreeCtrl* m_Ctrl;
	CFindTree m_Callback;
	bool m_bSuppressPrompt;

	void OnContextMenu(wxDataViewEvent& evt);
	void OnSelectionChanged(wxDataViewEvent& evt);
	void OnItemActivated(wxDataViewEvent& evt);
	void OnKeyDown(wxKeyEvent& evt);
	void OnPrint(wxCommandEvent& evt);
	void OnPreview(wxCommandEvent& evt);
	void OnUpdateEnable(wxUpdateUIEvent& evt);
	void OnUpdateDisable(wxUpdateUIEvent& evt);
	void OnUpdateDuplicate(wxUpdateUIEvent& evt);
	void OnDuplicate(wxCommandEvent& evt);
	void OnUpdateCut(wxUpdateUIEvent& evt);
	void OnCut(wxCommandEvent& evt);
	void OnUpdateCopy(wxUpdateUIEvent& evt);
	void OnCopy(wxCommandEvent& evt);
	void OnUpdatePaste(wxUpdateUIEvent& evt);
	void OnPaste(wxCommandEvent& evt);
	void OnUpdateDelete(wxUpdateUIEvent& evt);
	void OnDelete(wxCommandEvent& evt);
	void OnUpdateReorder(wxUpdateUIEvent& evt);
	void OnReorder(wxCommandEvent& evt);
	void OnFind(wxCommandEvent& evt);
	void OnFindNext(wxCommandEvent& evt);
	void OnFindPrevious(wxCommandEvent& evt);
	void OnUpdateEditDog(wxUpdateUIEvent& evt);
	void OnEditDog(wxCommandEvent& evt);
	void OnUpdateNewTitle(wxUpdateUIEvent& evt);
	void OnNewTitle(wxCommandEvent& evt);
	void OnUpdateNewTrial(wxUpdateUIEvent& evt);
	void OnNewTrial(wxCommandEvent& evt);
	void OnUpdateEditTrial(wxUpdateUIEvent& evt);
	void OnEditTrial(wxCommandEvent& evt);
	void OnUpdatePrintTrial(wxUpdateUIEvent& evt);
	void OnPrintTrial(wxCommandEvent& evt);
	void OnUpdatePrintRuns(wxUpdateUIEvent& evt);
	void OnPrintRuns(wxCommandEvent& evt);
	void OnUpdateNewRun(wxUpdateUIEvent& evt);
	void OnNewRun(wxCommandEvent& evt);
	void OnUpdateEditRun(wxUpdateUIEvent& evt);
	void OnEditRun(wxCommandEvent& evt);
	void OnViewCustomize(wxCommandEvent& evt);
	void OnUpdateUnsort(wxUpdateUIEvent& evt);
	void OnUnsort(wxCommandEvent& evt);
	void OnViewSortRuns(wxCommandEvent& evt);
	void OnViewRunsByTrial(wxCommandEvent& evt);
	void OnViewRunsByList(wxCommandEvent& evt);
	void OnViewAllRunsByList(wxCommandEvent& evt);
	void OnViewTableInYPS(wxCommandEvent& evt);
	void OnViewRuntimeInOPS(wxCommandEvent& evt);
	void OnUpdateExpand(wxUpdateUIEvent& evt);
	void OnExpand(wxCommandEvent& evt);
	void OnUpdateCollapse(wxUpdateUIEvent& evt);
	void OnCollapse(wxCommandEvent& evt);
	void OnUpdateExpandAll(wxUpdateUIEvent& evt);
	void OnExpandAll(wxCommandEvent& evt);
	void OnUpdateCollapseAll(wxUpdateUIEvent& evt);
	void OnCollapseAll(wxCommandEvent& evt);
};

#endif // USE_TREELIST
