#pragma once

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

#include "CommonView.h"
#include "DlgFind.h"
#include "IconList.h"
#include "Widgets.h"

#include "ARB/ARBBase.h"
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
	void FillTree(wxTreeItemId hItem) const;
	wxTreeItemId GetNextItem() const;
	CAgilityBookTreeView* m_pView;
	mutable std::vector<wxTreeItemId> m_Items;
	mutable std::vector<wxTreeItemId>::const_iterator m_Iter;
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
	virtual wxWindow* GetControl()		{return m_Ctrl;}
	virtual bool HasNextPane() const	{return true;}
	virtual bool NextPane();
	virtual bool HasPrevPane() const	{return true;}
	virtual bool PrevPane();
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
			wxObject* inHint = nullptr);

	// Allow access to tree control
	void Freeze()								{m_Ctrl->Freeze();}
	void Thaw()									{m_Ctrl->Thaw();}
	void Refresh()								{m_Ctrl->Refresh();}
	void RefreshItem(wxTreeItemId item, bool bRecurse = false);
	bool IsExpanded(wxTreeItemId item) const	{return m_Ctrl->IsExpanded(item);}
	void Expand(wxTreeItemId item)				{m_Ctrl->Expand(item);}
	void ExpandAllChildren(wxTreeItemId item)	{m_Ctrl->ExpandAllChildren(item);}
	void Collapse(wxTreeItemId item)			{m_Ctrl->Collapse(item);}
	void CollapseAllChildren(wxTreeItemId item)	{m_Ctrl->CollapseAllChildren(item);}
	void SelectItem(wxTreeItemId item)			{ChangeSelection(item, true);}
	void EnsureVisible(wxTreeItemId item)		{m_Ctrl->EnsureVisible(item);}
	bool ItemHasChildren(wxTreeItemId item) const
		{return m_Ctrl->ItemHasChildren(item);}
	void Delete(wxTreeItemId item)				{m_Ctrl->Delete(item);}
	wxTreeItemId GetItemParent(wxTreeItemId item) const
		{return m_Ctrl->GetItemParent(item);}

	CIconList const& GetImageList() const	{return m_ImageList;}
	CAgilityBookTreeData* GetCurrentTreeItem() const;
	CAgilityBookTreeData* GetTreeItem(wxTreeItemId hItem) const;
	bool SelectDog(ARBDogPtr);

	CAgilityBookTreeData* FindData(ARBBasePtr pBase) const
	{
		return FindData(m_Ctrl->GetRootItem(), pBase);
	}
	CAgilityBookTreeData* FindData(
			wxTreeItemId hItem,
			ARBBasePtr pBase) const;
	CAgilityBookTreeData* FindData(ARBDogPtr pDog) const
	{
		return FindData(m_Ctrl->GetRootItem(), pDog);
	}
	CAgilityBookTreeData* FindData(
			wxTreeItemId hItem,
			ARBDogPtr pDog) const;
	CAgilityBookTreeData* FindData(ARBDogTrialPtr pTrial) const
	{
		return FindData(m_Ctrl->GetRootItem(), pTrial);
	}
	CAgilityBookTreeData* FindData(
			wxTreeItemId hItem,
			ARBDogTrialPtr pTrial) const;
	CAgilityBookTreeData* FindData(ARBDogRunPtr pRun) const
	{
		return FindData(m_Ctrl->GetRootItem(), pRun);
	}
	CAgilityBookTreeData* FindData(
			wxTreeItemId hItem,
			ARBDogRunPtr pRun) const;

	wxTreeItemId InsertDog(
			ARBDogPtr pDog,
			bool bSelect = false);
	wxTreeItemId InsertTrial(
			ARBDogTrialPtr pTrial,
			wxTreeItemId hParent);
	wxTreeItemId InsertRun(
			ARBDogTrialPtr pTrial,
			ARBDogRunPtr pRun,
			wxTreeItemId hParent);
	bool PasteDog(bool& bLoaded);
	bool PasteRuns(ARBDogPtr pDog, ARBDogTrialPtr pTrial, bool& bLoaded, bool* bTreeSelectionSet = nullptr);
	void SuppressSelect(bool bSuppress)		{m_bSuppressSelect = bSuppress;}

	std::wstring GetPrintLine(wxTreeItemId hItem) const;

private:
	void UpdateData(wxTreeItemId hItem);
	void ChangeSelection(
			wxTreeItemId hItem,
			bool bEnsureVisible = true);
	void DoSelectionChange(wxTreeItemId hItem);
	void LoadData();
	void PrintLine(
			std::wostringstream& data,
			wxTreeItemId id,
			int indent) const;
	std::wstring GetPrintDataAsHtmlTable() const;
	bool OnCmd(int id);
	//void PrintLine(
	//		CDC* pDC,
	//		CTreePrintData *pData,
	//		wxTreeItemId hItem,
	//		int indent) const;

	CTreeCtrl* m_Ctrl;
	CIconList m_ImageList;
#ifdef WX_TREE_HAS_STATE
	wxImageList m_ImageListStates;
	int m_idxEmpty;
	int m_idxChecked;
#endif
	std::vector<long> m_Columns[3];
	bool m_bReset;
	bool m_bSuppressSelect;
	bool m_bInPopup;
	bool m_bInDelete;
	bool m_bDeleteChanged; // Used with m_itemPopup
	wxTreeItemId m_itemPopup; // Tried to select item during menu - delay
	CFindTree m_Callback;

	DECLARE_EVENT_TABLE()
	void OnCtrlSetFocus(wxFocusEvent& evt);
	void OnCtrlContextMenu(wxTreeEvent& evt);
	void OnCtrlSelectionChanged(wxTreeEvent& evt);
	void OnCtrlItemActivated(wxTreeEvent& evt);
	void OnCtrlKeyDown(wxKeyEvent& evt);
	void OnViewContextMenu(wxContextMenuEvent& evt);
	void OnViewUpdateCmd(wxUpdateUIEvent& evt);
	void OnViewCmd(wxCommandEvent& evt);
	void OnPrint(wxCommandEvent& evt);
	void OnPreview(wxCommandEvent& evt);
};
