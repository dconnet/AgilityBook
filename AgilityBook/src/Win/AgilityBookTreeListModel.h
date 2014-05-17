#pragma once
#if USE_TREELIST

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CAgilityBookTreeListModel class
 * @author David Connet
 *
 * Revision History
 * 2013-09-25 Remember collapsed state when reloading tree.
 * 2013-04-22 Converted tree+list into single control.
 */

#include "AgilityBookMenu.h"
#include "ARB/ARBBase.h"
#include <list>
class CAgilityBookDoc;
class CAgilityBookTreeListData;


enum CTreeListDataType
{
	eTreeUnknown = -1,
	eTreeDog = 0,
	eTreeTrial,
	eTreeRun,
};


class CAgilityBookTreeListModel : public wxDataViewModel
{
public:
	CAgilityBookTreeListModel();
	~CAgilityBookTreeListModel();

	CAgilityBookDoc* GetDocument() const	{return m_pDoc;}

	void CreateColumns(wxDataViewCtrl* ctrl, CAgilityBookDoc* pDoc);
	void UpdateColumns();

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
	void LoadData(bool bPreserveColWidths);

	void Delete(const wxDataViewItem& item);
	void DeleteAllItems();
	void Expand(
			wxDataViewCtrl* list,
			std::list<ARBBasePtr> const* pCollapsedItems = nullptr);

	virtual int Compare(
			const wxDataViewItem& item1,
			const wxDataViewItem& item2,
			unsigned int column,
			bool ascending) const;

	virtual unsigned int GetColumnCount() const;
	virtual wxString GetColumnType(unsigned int col) const;

	virtual void GetValue(
			wxVariant& variant,
			const wxDataViewItem& item,
			unsigned int col) const;
	virtual bool SetValue(
			const wxVariant& variant,
			const wxDataViewItem& item,
			unsigned int col);

	virtual wxDataViewItem GetParent(const wxDataViewItem& item) const;
	virtual bool IsContainer(const wxDataViewItem& item) const;
	virtual unsigned int GetChildren(
			const wxDataViewItem& parent,
			wxDataViewItemArray& array) const;

	wxString GetPrintLine(const wxDataViewItem& item) const;

	CTreeListDataType Type(const wxDataViewItem& item) const;
	ARBBasePtr GetARBBase(const wxDataViewItem& item) const;
	ARBDogPtr GetDog(const wxDataViewItem& item) const;
	ARBDogTrialPtr GetTrial(const wxDataViewItem& item) const;
	ARBDogRunPtr GetRun(const wxDataViewItem& item) const;
	MenuIdentityPopup GetMenuID(const wxDataViewItem& item) const;
	MenuIdentityPopup GetMenuID(wxDataViewItemArray const& items) const;

private:
	void OnDestroyControl(wxWindowDestroyEvent& evt);
	CAgilityBookTreeListData* GetNode(const wxDataViewItem& item) const;
	void GetCollapsedItems(
			std::list<ARBBasePtr>& collapsedItems,
			CAgilityBookTreeListData* pData);

	CAgilityBookDoc* m_pDoc;
	wxDataViewCtrl* m_Ctrl;
	std::vector<long> m_Columns[3];
	mutable bool m_bInCompare;

	std::vector<CAgilityBookTreeListData*> m_roots;
};

#endif // USE_TREELIST
