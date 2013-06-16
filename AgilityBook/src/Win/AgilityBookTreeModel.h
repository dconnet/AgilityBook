#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CAgilityBookTreeModel class
 * @author David Connet
 *
 * Revision History
 * @li 2013-04-22 DRC Converted tree+list into single control.
 */

#include "AgilityBookMenu.h"
#include "ARB/ARBBase.h"
class CAgilityBookDoc;
class CAgilityBookTreeData;


enum CTreeDataType
{
	eTreeUnknown = -1,
	eTreeDog = 0,
	eTreeTrial,
	eTreeRun,
};


class CAgilityBookTreeModel : public wxDataViewModel
{
public:
	CAgilityBookTreeModel();
	~CAgilityBookTreeModel();

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
	void LoadData();

	void Delete(const wxDataViewItem& item);
	void DeleteAllItems();
	void Expand(wxDataViewCtrl* list);

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

	CTreeDataType Type(const wxDataViewItem& item) const;
	ARBBasePtr GetARBBase(const wxDataViewItem& item) const;
	ARBDogPtr GetDog(const wxDataViewItem& item) const;
	ARBDogTrialPtr GetTrial(const wxDataViewItem& item) const;
	ARBDogRunPtr GetRun(const wxDataViewItem& item) const;
	MenuIdentityPopup GetMenuID(const wxDataViewItem& item) const;
	MenuIdentityPopup GetMenuID(wxDataViewItemArray const& items) const;

private:
	void OnDestroyControl(wxWindowDestroyEvent& evt);
	CAgilityBookTreeData* GetNode(const wxDataViewItem& item) const;

	CAgilityBookDoc* m_pDoc;
	wxDataViewCtrl* m_Ctrl;
	std::vector<long> m_Columns[3];
	mutable bool m_bInCompare;

	std::vector<CAgilityBookTreeData*> m_roots;
};
