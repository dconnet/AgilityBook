#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CAgilityBookTree class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-08 DRC Ported to wxWidgets.
 * @li 2008-11-19 DRC Added SelectDog()
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-12-31 DRC Make F1 invoke context help.
 * @li 2003-12-30 DRC Implemented customized text in tree.
 * @li 2003-12-28 DRC Implemented Find/FindNext.
 * @li 2003-08-30 DRC Added GetPrintLine to allow future differences between
 *                    printing and viewing (already in the listctrl)
 * @li 2003-08-28 DRC Added printing.
 * @li 2003-08-27 DRC Cleaned up selection synchronization.
 * @li 2003-08-24 DRC Optimized filtering by adding boolean into ARBBase to
 *                    prevent constant re-evaluation.
 */

#include "AgilityBookTreeModel.h"
#include "CommonView.h"
#include "DlgFind.h"
#include "IconList.h"
#include <vector>
#include <wx/docview.h>
class CAgilityBookTreeData;
class CAgilityBookTreeView;


class CFindTree : public IFindCallback
{
public:
	CFindTree(CAgilityBookTreeView* pView)
		: m_pView(pView)
	{
	}
	virtual bool Search(CDlgFind* pDlg) const;
private:
	void FillTree(wxDataViewItem item) const;
	wxDataViewItem GetNextItem() const;
	CAgilityBookTreeView* m_pView;
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
			long style = wxDV_ROW_LINES,
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

	CAgilityBookTreeModel* GetStore();
	const CAgilityBookTreeModel* GetStore() const;
};


class CAgilityBookTreeView : public CAgilityBookBaseExtraView
{
	friend class CAgilityBookTreeData;
	friend class CFindTree;
	DECLARE_CLASS(CAgilityBookTreeView)
	CAgilityBookTreeView(
			CTabView* pTabView,
			wxDocument* doc);

public:
	~CAgilityBookTreeView();

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
	wxDataViewItem GetSelection() const			{return m_Ctrl->GetSelection();}
	void Select(wxDataViewItem const& item)		{m_Ctrl->Select(item);}
	void RefreshItem(wxDataViewItem const& item);
	CAgilityBookTreeModel* GetStore()			{return m_Ctrl->GetStore();}
	const CAgilityBookTreeModel* GetStore() const	{return m_Ctrl->GetStore();}
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
	void ChangeSelection(
			wxDataViewItem const& item,
			bool bEnsureVisible = true);
	void DoSelectionChange(wxDataViewItem const& item);
	bool DoEdit(
			wxDataViewItem const& item,
			CTreeDataType type);
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
	void OnUpdateNewRun(wxUpdateUIEvent& evt);
	void OnNewRun(wxCommandEvent& evt);
	void OnUpdateEditRun(wxUpdateUIEvent& evt);
	void OnEditRun(wxCommandEvent& evt);
	void OnViewCustomize(wxCommandEvent& evt);
	void OnUpdateUnsort(wxUpdateUIEvent& evt);
	void OnUnsort(wxCommandEvent& evt);
	void OnViewSortRuns(wxCommandEvent& evt);
	void OnViewRunsByTrial(wxCommandEvent& evt);
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
