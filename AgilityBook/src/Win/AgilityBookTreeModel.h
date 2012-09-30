#pragma once

#include <wx/dataview.h>
class CAgilityBookDoc;
class CAgilityBookTreeModelNode;


class CAgilityBookTreeModel : public wxDataViewModel
{
public:
	CAgilityBookTreeModel();
	~CAgilityBookTreeModel();

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

private:
	CAgilityBookTreeModelNode* GetNode(const wxDataViewItem& item) const;
	void FakeRoot();

	CAgilityBookDoc* m_pDoc;
	wxDataViewCtrl* m_Ctrl;
	std::vector<long> m_Columns[3];

	std::vector<CAgilityBookTreeModelNode*> m_roots;
	// Deleting the last item does really bad things. Something in the base
	// class is messed up and newly added items don't show up.
	// By adding/removing a fake node, the control stays usable.
	CAgilityBookTreeModelNode* m_emptyRoot;
};
