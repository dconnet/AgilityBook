#pragma once

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
			ARBDogTrialPtr pTrial,
			wxDataViewItem parent);
	wxDataViewItem LoadData(
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

private:
	CAgilityBookTreeData* GetNode(const wxDataViewItem& item) const;

	CAgilityBookDoc* m_pDoc;
	wxDataViewCtrl* m_Ctrl;
	std::vector<long> m_Columns[3];

	std::vector<CAgilityBookTreeData*> m_roots;
};
