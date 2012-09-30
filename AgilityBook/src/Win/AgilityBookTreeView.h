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

#include "CommonView.h"
#include "DlgFind.h"
#include "IconList.h"
#include <vector>
#include <wx/dataview.h>
#include <wx/docview.h>
class CAgilityBookTreeCtrl;
class CAgilityBookTreeData;
class CAgilityBookTreeModel;
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
	CAgilityBookTreeView* m_pView;
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

	/*
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
			*/

	bool SelectDog(ARBDogPtr);
	bool PasteDog(bool& bLoaded);
	std::wstring GetPrintLine(long item) const;

private:
	void UpdateData(long item);
	//void ChangeSelection(
	//		wxTreeItemId hItem,
	//		bool bEnsureVisible = true);
	//void DoSelectionChange(wxTreeItemId hItem);
	void LoadData();
	void PrintLine(
			std::wostringstream& data,
			long id,
			int indent) const;
	std::wstring GetPrintDataAsHtmlTable() const;
	bool OnCmd(int id);

	CAgilityBookTreeCtrl* m_Ctrl;
	//CIconList m_ImageList;
//#ifdef WX_TREE_HAS_STATE
	//wxImageList m_ImageListStates;
	//int m_idxEmpty;
	//int m_idxChecked;
//#endif
	CFindTree m_Callback;
	ARBDogPtr m_pDog;

	DECLARE_EVENT_TABLE()
	void OnCtrlContextMenu(wxDataViewEvent& evt);
	void OnCtrlSelectionChanged(wxDataViewEvent& evt);
	void OnCtrlItemActivated(wxDataViewEvent& evt);
	void OnCtrlKeyDown(wxKeyEvent& evt);
	void OnViewContextMenu(wxContextMenuEvent& evt);
	void OnViewUpdateCmd(wxUpdateUIEvent& evt);
	void OnViewCmd(wxCommandEvent& evt);
	void OnPrint(wxCommandEvent& evt);
	void OnPreview(wxCommandEvent& evt);
};
