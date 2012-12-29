#include "stdafx.h"
#include "AgilityBookTreeModel.h"

#include "AgilityBookDoc.h"
#include "AgilityBookTreeData.h"

#include "ARBTypes.h"
#include "DlgAssignColumns.h"
#include "RegItems.h"
#include "StringUtil.h"
#include <algorithm>
#include <list>
#include <vector>
#include <wx/config.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

CAgilityBookTreeModel::CAgilityBookTreeModel()
	: m_pDoc(NULL)
	, m_Ctrl(NULL)
	, m_Columns()
	, m_roots()
{
}


CAgilityBookTreeModel::~CAgilityBookTreeModel()
{
	DeleteAllItems();
}


CAgilityBookTreeData* CAgilityBookTreeModel::GetNode(const wxDataViewItem& item) const
{
	if (item.IsOk())
		return reinterpret_cast<CAgilityBookTreeData*>(item.GetID());
	return NULL;
}


void CAgilityBookTreeModel::CreateColumns(
		wxDataViewCtrl* ctrl,
		CAgilityBookDoc* pDoc)
{
	m_pDoc = pDoc;
	m_Ctrl = ctrl;

	UpdateColumns();
}


void CAgilityBookTreeModel::UpdateColumns()
{
	m_Ctrl->ClearColumns();

	CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_TREE_DOG, m_Columns[0]);
	CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_TREE_TRIAL, m_Columns[1]);
	CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_TREE_RUNS, m_Columns[2]);

	if (CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_TREE_RUNS, m_Columns[2]))
	{
		for (size_t iCol = 0; iCol < m_Columns[2].size(); ++iCol)
		{
			std::wstring str = CDlgAssignColumns::GetNameFromColumnID(m_Columns[2][iCol]);
			//int fmt = CDlgAssignColumns::GetFormatFromColumnID(m_Columns[2][iCol]);

			wxDataViewRenderer* renderer;
			if (0 == iCol)
				renderer = new wxDataViewIconTextRenderer("wxDataViewIconText", wxDATAVIEW_CELL_INERT);
			else
				renderer = new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_INERT);

			wxDataViewColumn* column = new wxDataViewColumn(
				StringUtil::stringWX(str),
				renderer,
				iCol,
				wxCOL_WIDTH_AUTOSIZE,
				wxALIGN_LEFT,
				wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
			m_Ctrl->AppendColumn(column);
		}
	}
}


wxDataViewItem CAgilityBookTreeModel::LoadData(
		ARBDogPtr pDog)
{
	wxDataViewItem itemDog;

	if (pDog)
	{
		CAgilityBookTreeDataDog* nodeDog = new CAgilityBookTreeDataDog(this, pDog);
		itemDog = wxDataViewItem(nodeDog);

		m_roots.push_back(nodeDog);
		ItemAdded(wxDataViewItem(0), wxDataViewItem(nodeDog));

		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			iterTrial != pDog->GetTrials().end();
			++iterTrial)
		{
			LoadData(*iterTrial, itemDog);
		}
	}
	return itemDog;
}


wxDataViewItem CAgilityBookTreeModel::LoadData(
		ARBDogTrialPtr pTrial,
		wxDataViewItem parent)
{
	wxDataViewItem itemTrial;
	CAgilityBookTreeData* pParent = GetNode(parent);
	if (pParent && pTrial && !pTrial->IsFiltered())
	{
		CAgilityBookTreeDataTrial* nodeTrial = new CAgilityBookTreeDataTrial(this, pTrial);
		itemTrial = wxDataViewItem(nodeTrial);
		pParent->Append(nodeTrial);
		for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
			iterRun != pTrial->GetRuns().end();
			++iterRun)
		{
			LoadData(*iterRun, itemTrial);
		}
	}
	return itemTrial;
}


wxDataViewItem CAgilityBookTreeModel::LoadData(
		ARBDogRunPtr pRun,
		wxDataViewItem parent)
{
	wxDataViewItem itemRun;
	CAgilityBookTreeData* pParent = GetNode(parent);
	if (pParent && pRun && !pRun->IsFiltered())
	{
		CAgilityBookTreeDataRun* nodeRun = new CAgilityBookTreeDataRun(this, pRun);
		itemRun = wxDataViewItem(nodeRun);
		pParent->Append(nodeRun);
	}
	return itemRun;
}


void CAgilityBookTreeModel::LoadData()
{
	wxBusyCursor wait;

	m_Ctrl->Freeze();

	// Remember the currently selected item.
	std::wstring strCallName;
	CAgilityBookTreeData* node = GetNode(m_Ctrl->GetSelection());
	if (!node)
	{
		strCallName = wxConfig::Get()->Read(CFG_SETTINGS_LASTDOG, wxString());
	}
	std::list<ARBBasePtr> baseItems;
	while (node)
	{
		baseItems.push_front(node->GetARBBase());
		node = node->GetParent();
	}

	DeleteAllItems();

	// Load the data
	wxDataViewItem item;
	for (ARBDogList::const_iterator iterDog = m_pDoc->Book().GetDogs().begin();
		iterDog != m_pDoc->Book().GetDogs().end();
		++iterDog)
	{
		wxDataViewItem itemDog = LoadData(*iterDog);
		if (0 < strCallName.length() && (*iterDog)->GetCallName() == strCallName)
		{
			item = itemDog;
		}
	}
	Resort();
	Expand(m_Ctrl);

	if (!item.IsOk())
	{
#pragma PRAGMA_TODO("Fix me")
#if 0
		// Work thru all the base items...
		// Otherwise, after a reorder, the wrong item is selected.
		hItem = m_Ctrl->GetRootItem();
		for (std::list<ARBBasePtr>::iterator iter = baseItems.begin();
			iter != baseItems.end();
			++iter)
		{
			pData = FindData(hItem, (*iter));
			if (pData)
				hItem = pData->GetId();
		}
		if (hItem == m_Ctrl->GetRootItem())
			hItem = wxTreeItemId();
#endif
	}
	if (!item.IsOk())
	{
		//wxTreeItemIdValue cookie;
		//hItem = m_Ctrl->GetFirstChild(m_Ctrl->GetRootItem(), cookie);
	}

	if (item.IsOk())
	{
		m_Ctrl->Select(item);
		m_Ctrl->EnsureVisible(item);
	}

	// Reset the context menu reset.
	// We may have reloaded during a context menu operation.
	//m_bReset = false;

	//if (m_Ctrl->IsShownOnScreen())
	//	UpdateMessages();

	m_Ctrl->Thaw();
	//m_bSuppressSelect = false;
}


void CAgilityBookTreeModel::Delete(const wxDataViewItem& item)
{
	CAgilityBookTreeData* node = GetNode(item);
	if (!node) // happens if item.IsOk()==false
		return;

	wxDataViewItem parent(node->GetParent());
	if (!parent.IsOk())
	{
		std::vector<CAgilityBookTreeData*>::iterator i = std::find(m_roots.begin(), m_roots.end(), node);
		wxASSERT(i != m_roots.end());
		m_roots.erase(i);
		delete node;
	}
	else
	{
		node->GetParent()->Remove(node);
	}

	// notify control
	ItemDeleted(parent, item);
}


void CAgilityBookTreeModel::DeleteAllItems()
{
	while (m_roots.size() > 0)
	{
		std::vector<CAgilityBookTreeData*>::iterator i = m_roots.begin();
		CAgilityBookTreeData* node = *i;
		m_roots.erase(i);

		wxDataViewItem parent(0);
		wxDataViewItem item(node);
		delete node;

		ItemDeleted(parent, item);
	}
}


void CAgilityBookTreeModel::Expand(wxDataViewCtrl* list)
{
	for (std::vector<CAgilityBookTreeData*>::iterator i = m_roots.begin();
		i != m_roots.end();
		++i)
	{
		list->Expand(wxDataViewItem(*i));
		if ((*i)->IsContainer())
		{
			wxDataViewItemArray array;
			(*i)->GetChildren(array);
			for (wxDataViewItemArray::iterator iArray = array.begin();
				iArray != array.end();
				++iArray)
			{
				list->Expand(*iArray);
			}
		}
	}
}


int CAgilityBookTreeModel::Compare(
		const wxDataViewItem& item1,
		const wxDataViewItem& item2,
		unsigned int column,
		bool ascending) const
{
	wxASSERT(item1.IsOk() && item2.IsOk()); // should never happen
	CAgilityBookTreeData* node1 = GetNode(item1);
	CAgilityBookTreeData* node2 = GetNode(item2);

	if (node1->IsContainer() && node2->IsContainer())
		column = 0;

	wxVariant value1, value2;
	GetValue(value1, item1, column);
	GetValue(value2, item2, column);

	if (!ascending)
	{
		wxVariant temp = value1;
		value1 = value2;
		value2 = temp;
	}

	if (value1.GetType() == wxT("string"))
	{
		wxString str1 = value1.GetString();
		wxString str2 = value2.GetString();
		int res = str1.CmpNoCase(str2);
		if (res)
			return res;
	}
	else if (value1.GetType() == wxT("long"))
	{
		long l1 = value1.GetLong();
		long l2 = value2.GetLong();
		long res = l1-l2;
		if (res)
			return res;
	}
	else if (value1.GetType() == wxT("double"))
	{
		double d1 = value1.GetDouble();
		double d2 = value2.GetDouble();
		if (d1 < d2)
			return 1;
		if (d1 > d2)
			return -1;
	}
	else if (value1.GetType() == wxT("datetime"))
	{
		wxDateTime dt1 = value1.GetDateTime();
		wxDateTime dt2 = value2.GetDateTime();
		if (dt1.IsEarlierThan(dt2))
			return 1;
		if (dt2.IsEarlierThan(dt1))
			return -1;
	}

	// items must be different
	wxUIntPtr id1 = wxPtrToUInt(item1.GetID());
	wxUIntPtr id2 = wxPtrToUInt(item2.GetID());
	return ascending ? id1 - id2 : id2 - id1;
}


unsigned int CAgilityBookTreeModel::GetColumnCount() const
{
	return static_cast<unsigned int>(m_Columns[2].size());
}


wxString CAgilityBookTreeModel::GetColumnType(unsigned int col) const
{
	return wxT("string");
}


void CAgilityBookTreeModel::GetValue(
		wxVariant& variant,
		const wxDataViewItem& item,
		unsigned int col) const
{
	CAgilityBookTreeData* node = GetNode(item);
	wxASSERT(node);

	wxIcon icon = node->GetIcon(col);
	if (icon.IsOk())
	{
		wxDataViewIconText data(node->GetColumn(m_pDoc->Book().GetConfig(), m_Columns[node->Type()], col), icon);
		variant << data;
	}
	else
	{
		variant = node->GetColumn(m_pDoc->Book().GetConfig(), m_Columns[node->Type()], col);
	}
}


bool CAgilityBookTreeModel::SetValue(
		const wxVariant& variant,
		const wxDataViewItem& item,
		unsigned int col)
{
	return false;
}


wxDataViewItem CAgilityBookTreeModel::GetParent(const wxDataViewItem& item) const
{
	// the invisible root node has no parent
	if (!item.IsOk())
		return wxDataViewItem(0);

	CAgilityBookTreeData* node = GetNode(item);
	return wxDataViewItem(node->GetParent());
}


bool CAgilityBookTreeModel::IsContainer(const wxDataViewItem& item) const
{
	// the invisble root node can have children
	if (!item.IsOk())
		return true;
	return GetNode(item)->IsContainer();
}


unsigned int CAgilityBookTreeModel::GetChildren(
		const wxDataViewItem& parent,
		wxDataViewItemArray& array) const
{
	CAgilityBookTreeData* node = GetNode(parent);
	if (!node)
	{
		/*
		if (m_roots.size() == 0 && m_emptyRoot)
		{
			array.Add(wxDataViewItem(m_emptyRoot));
			return 1;
		}
		*/
		for (std::vector<CAgilityBookTreeData*>::const_iterator i = m_roots.begin();
			i != m_roots.end();
			++i)
		{
			array.Add(wxDataViewItem(*i));
		}
		return m_roots.size();
	}

	return node->GetChildren(array);
}


wxString CAgilityBookTreeModel::GetPrintLine(const wxDataViewItem& item) const
{
	wxString line;
	CAgilityBookTreeData* node = GetNode(item);
	if (node)
	{
		unsigned int n = GetColumnCount();
		for (unsigned int i = 0; i < n; ++i)
		{
			if (0 < i)
				line << wxT(" ");
			wxVariant variant;
			GetValue(variant, item, i);
			line << variant.GetString();
		}
	}
	return line;
}


CTreeDataType CAgilityBookTreeModel::Type(const wxDataViewItem& item) const
{
	CAgilityBookTreeData* pNode = GetNode(item);
	if (pNode)
		return pNode->Type();
	return eTreeUnknown;
}


ARBBasePtr CAgilityBookTreeModel::GetARBBase(const wxDataViewItem& item) const
{
	CAgilityBookTreeData* pNode = GetNode(item);
	if (pNode)
		return pNode->GetARBBase();
	return ARBBasePtr();
}


ARBDogPtr CAgilityBookTreeModel::GetDog(const wxDataViewItem& item) const
{
	CAgilityBookTreeData* pNode = GetNode(item);
	if (pNode)
		return pNode->GetDog();
	return ARBDogPtr();
}


ARBDogTrialPtr CAgilityBookTreeModel::GetTrial(const wxDataViewItem& item) const
{
	CAgilityBookTreeData* pNode = GetNode(item);
	if (pNode)
		return pNode->GetTrial();
	return ARBDogTrialPtr();
}


ARBDogRunPtr CAgilityBookTreeModel::GetRun(const wxDataViewItem& item) const
{
	CAgilityBookTreeData* pNode = GetNode(item);
	if (pNode)
		return pNode->GetRun();
	return ARBDogRunPtr();
}


MenuIdentityPopup CAgilityBookTreeModel::GetMenuID(const wxDataViewItem& item) const
{
	CAgilityBookTreeData* pNode = GetNode(item);
	if (pNode)
		return pNode->GetMenuID();
	return IdMenuNone;
}
