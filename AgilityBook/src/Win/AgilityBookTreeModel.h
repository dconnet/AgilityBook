#pragma once

#include "IconList.h"
#include <wx/dataview.h>
class CAgilityBookDoc;
class CAgilityBookTreeData;


class CAgilityBookTreeModel : public wxDataViewModel
{
public:
	CAgilityBookTreeModel();
	~CAgilityBookTreeModel();

	CAgilityBookDoc* GetDocument() const	{return m_pDoc;}
	CIconList const& IconList() const		{return m_icons;}

	void CreateColumns(wxDataViewCtrl* ctrl, CAgilityBookDoc* pDoc);
	void UpdateColumns();
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
	CAgilityBookTreeData* GetNode(const wxDataViewItem& item) const;

private:
	CAgilityBookDoc* m_pDoc;
	wxDataViewCtrl* m_Ctrl;
	std::vector<long> m_Columns[3];

	CIconList m_icons;
	std::vector<CAgilityBookTreeData*> m_roots;
};
