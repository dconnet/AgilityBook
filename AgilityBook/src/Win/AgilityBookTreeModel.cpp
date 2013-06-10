/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CAgilityBookTreeModel class
 * @author David Connet
 *
 * Revision History
 * @li 2013-04-22 DRC Converted tree+list into single control.
 */

#include "stdafx.h"
#include "AgilityBookTreeModel.h"

#include "AgilityBookDoc.h"
#include "AgilityBookTreeData.h"
#include "DlgAssignColumns.h"
#include "RegItems.h"

#include "ARB/ARBTypes.h"
#include "ARB/StringUtil.h"
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
	, m_bInCompare(false)
	, m_roots()
{
}


CAgilityBookTreeModel::~CAgilityBookTreeModel()
{
	DeleteAllItems();
}


void CAgilityBookTreeModel::OnDestroyControl(wxWindowDestroyEvent& evt)
{
	m_Ctrl = NULL;
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

	BIND_OR_CONNECT_CTRL(m_Ctrl, wxEVT_DESTROY, wxWindowDestroyEventHandler, CAgilityBookTreeModel::OnDestroyControl);

	UpdateColumns();
}


void CAgilityBookTreeModel::UpdateColumns()
{
	m_Ctrl->ClearColumns();

	CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_TREE_DOG, m_Columns[0]);
	CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_TREE_TRIAL, m_Columns[1]);

	size_t idxColumn = 0;
	if (CAgilityBookOptions::eViewAllRunsByList == CAgilityBookOptions::GetViewRunsStyle())
		idxColumn = IO_TYPE_VIEW_TREE_RUNS_LIST;
	else
		idxColumn = IO_TYPE_VIEW_TREE_RUNS;

	if (CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, idxColumn, m_Columns[2]))
	{
		for (unsigned int iCol = 0; iCol < static_cast<unsigned int>(m_Columns[2].size()); ++iCol)
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

	if (CAgilityBookOptions::eViewRunsByList == CAgilityBookOptions::GetViewRunsStyle())
	{
		ARBDogPtr pCurrentDog = GetDocument()->GetCurrentDog();
		if (pDog != pCurrentDog)
			return itemDog;
	}

	if (pDog)
	{
		itemDog = wxDataViewItem(0);
		if (CAgilityBookOptions::eViewRunsByTrial == CAgilityBookOptions::GetViewRunsStyle())
		{ 
			CAgilityBookTreeDataDog* nodeDog = new CAgilityBookTreeDataDog(this, pDog);
			itemDog = wxDataViewItem(nodeDog);

			m_roots.push_back(nodeDog);
			ItemAdded(wxDataViewItem(0), itemDog);
		}

		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			iterTrial != pDog->GetTrials().end();
			++iterTrial)
		{
			LoadData(pDog, *iterTrial, itemDog);
		}
	}
	return itemDog;
}


wxDataViewItem CAgilityBookTreeModel::LoadData(
		ARBDogPtr pDog,
		ARBDogTrialPtr pTrial,
		wxDataViewItem parent)
{
	wxDataViewItem itemTrial;

	CAgilityBookTreeData* pParent = NULL;
	if (CAgilityBookOptions::eViewRunsByTrial == CAgilityBookOptions::GetViewRunsStyle())
		pParent = GetNode(parent);

	if (((CAgilityBookOptions::eViewRunsByTrial == CAgilityBookOptions::GetViewRunsStyle() && parent)
	|| CAgilityBookOptions::eViewRunsByTrial != CAgilityBookOptions::GetViewRunsStyle())
	&& pTrial && !pTrial->IsFiltered())
	{
		itemTrial = wxDataViewItem(0);
		if (CAgilityBookOptions::eViewRunsByTrial == CAgilityBookOptions::GetViewRunsStyle())
		{
			CAgilityBookTreeDataTrial* nodeTrial = new CAgilityBookTreeDataTrial(this, pDog, pTrial);
			itemTrial = wxDataViewItem(nodeTrial);
			pParent->Append(nodeTrial);
		}
		for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
			iterRun != pTrial->GetRuns().end();
			++iterRun)
		{
			LoadData(pDog, pTrial, *iterRun, itemTrial);
		}
	}

	return itemTrial;
}


wxDataViewItem CAgilityBookTreeModel::LoadData(
		ARBDogPtr pDog,
		ARBDogTrialPtr pTrial,
		ARBDogRunPtr pRun,
		wxDataViewItem parent)
{
	wxDataViewItem itemRun;

	CAgilityBookTreeData* pParent = NULL;
	if (CAgilityBookOptions::eViewRunsByTrial == CAgilityBookOptions::GetViewRunsStyle())
		pParent = GetNode(parent);

	if (((CAgilityBookOptions::eViewRunsByTrial == CAgilityBookOptions::GetViewRunsStyle() && pParent)
	|| CAgilityBookOptions::eViewRunsByTrial != CAgilityBookOptions::GetViewRunsStyle())
	&& pRun && !pRun->IsFiltered())
	{
		CAgilityBookTreeDataRun* nodeRun = new CAgilityBookTreeDataRun(this, pDog, pTrial, pRun);
		itemRun = wxDataViewItem(nodeRun);
		if (CAgilityBookOptions::eViewRunsByTrial == CAgilityBookOptions::GetViewRunsStyle())
		{
			pParent->Append(nodeRun);
		}
		else
		{
			m_roots.push_back(nodeRun);
			ItemAdded(wxDataViewItem(0), itemRun);
		}
	}
	return itemRun;
}


void CAgilityBookTreeModel::LoadData()
{
	wxBusyCursor wait;

	m_Ctrl->Freeze();

	// Remember the currently selected item.
	std::list<ARBBasePtr> baseItems;
	{
		CAgilityBookTreeData* node = GetNode(m_Ctrl->GetSelection());
		if (!node)
		{
			if (m_pDoc->GetCurrentDog())
				baseItems.push_back(m_pDoc->GetCurrentDog());
		}
		if (node)
		{
			ARBDogPtr pDog = node->GetDog();
			ARBDogTrialPtr pTrial = node->GetTrial();
			ARBDogRunPtr pRun = node->GetRun();
			if (pDog)
				baseItems.push_back(pDog);
			if (pTrial && pRun)
				baseItems.push_back(pTrial);
			if (pRun)
				baseItems.push_back(pRun);
		}
	}

	DeleteAllItems();

	// Load the data
	wxDataViewItem item;
	for (ARBDogList::const_iterator iterDog = m_pDoc->Book().GetDogs().begin();
		iterDog != m_pDoc->Book().GetDogs().end();
		++iterDog)
	{
		wxDataViewItem itemDog = LoadData(*iterDog);
		if (0 < baseItems.size() && *iterDog == baseItems.front())
		{
			item = itemDog;
		}
	}
	//Resort(); Not needed - thawing will trigger a resort
	Expand(m_Ctrl);

	if (item.IsOk() && 1 < baseItems.size())
	{
		std::list<ARBBasePtr>::iterator iter = baseItems.begin();
		for (++iter; item.IsOk() && iter != baseItems.end(); ++iter)
		{
			wxDataViewItemArray kids;
			unsigned int n = GetChildren(item, kids);
			for (unsigned int i = 0; i < n; ++i)
			{
				if (GetARBBase(kids[i]) == *iter)
				{
					item = kids[i];
					break;
				}
			}
		}
	}
	if (!item.IsOk() && 0 < m_roots.size())
	{
		if (CAgilityBookOptions::eViewRunsByTrial != CAgilityBookOptions::GetViewRunsStyle()
		&& 3 == baseItems.size())
		{
			ARBBasePtr pRun = *baseItems.rbegin();
			for (std::vector<CAgilityBookTreeData*>::iterator i = m_roots.begin();
				i != m_roots.end();
				++i)
			{
				if ((*i)->GetARBBase() == pRun)
				{
					item = wxDataViewItem(*i);
					break;
				}
			}
		}
		else
			item = wxDataViewItem(m_roots.front());
	}

	if (item.IsOk())
	{
		m_Ctrl->Select(item);
		m_Ctrl->EnsureVisible(item);
		if (CAgilityBookOptions::eViewAllRunsByList != CAgilityBookOptions::GetViewRunsStyle())
		{
			// Select does not generate a change event.
			CAgilityBookTreeData* node = GetNode(item);
			if (node->GetDog() != GetDocument()->GetCurrentDog())
				GetDocument()->SetCurrentDog(node->GetDog());
		}
	}

	m_Ctrl->Thaw();
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
	if (m_Ctrl)
	{
		// When deleting everything, turn off autosizing.
		// DVC is different from list controls. Setting autosize here means the
		// column resizes itself on data changes. (really not sure I like that)
		for (unsigned int i = 0; i < m_Ctrl->GetColumnCount(); ++i)
		{
			wxDataViewColumn* pCol = m_Ctrl->GetColumn(i);
			if (pCol)
				pCol->SetWidth(pCol->GetWidth());
		}
	}

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

	if (m_Ctrl)
	{
		for (unsigned int i = 0; i < m_Ctrl->GetColumnCount(); ++i)
		{
			wxDataViewColumn* pCol = m_Ctrl->GetColumn(i);
			if (pCol)
				pCol->SetWidth(wxCOL_WIDTH_AUTOSIZE);
		}
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

	m_bInCompare = true;
	wxVariant value1, value2;
	GetValue(value1, item1, column);
	GetValue(value2, item2, column);
	m_bInCompare = false;

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
		long res = l1 - l2;
		if (res)
			return res;
	}
	else if (value1.GetType() == wxT("double"))
	{
		double d1 = value1.GetDouble();
		double d2 = value2.GetDouble();
		if (d1 < d2)
			return -1;
		if (d1 > d2)
			return 1;
	}
	else if (value1.GetType() == wxT("datetime"))
	{
		wxDateTime dt1 = value1.GetDateTime();
		wxDateTime dt2 = value2.GetDateTime();
		if (dt1.IsEarlierThan(dt2))
			return -1;
		if (dt2.IsEarlierThan(dt1))
			return 1;
	}

	// items must be different
	wxUIntPtr id1 = wxPtrToUInt(item1.GetID());
	wxUIntPtr id2 = wxPtrToUInt(item2.GetID());
	return ascending ? static_cast<int>(id1 - id2) : static_cast<int>(id2 - id1);
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
	if (!m_bInCompare && icon.IsOk())
	{
		wxDataViewIconText data(node->GetColumn(m_pDoc->Book().GetConfig(), m_Columns[node->Type()], col, m_bInCompare), icon);
		variant << data;
	}
	else
	{
		variant = node->GetColumn(m_pDoc->Book().GetConfig(), m_Columns[node->Type()], col, m_bInCompare);
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
		return static_cast<unsigned int>(m_roots.size());
	}

	return node->GetChildren(array);
}


wxString CAgilityBookTreeModel::GetPrintLine(const wxDataViewItem& item) const
{
	wxString line;
	CAgilityBookTreeData* node = GetNode(item);
	if (node)
	{
		if (node->IsContainer())
		{
			wxVariant variant = node->GetColumn(m_pDoc->Book().GetConfig(), m_Columns[node->Type()], 0, m_bInCompare);
			line << variant.GetString();
		}
		else
		{
			unsigned int n = GetColumnCount();
			for (unsigned int i = 0; i < n; ++i)
			{
				if (0 < i)
					line << wxT(" ");
				wxVariant variant = node->GetColumn(m_pDoc->Book().GetConfig(), m_Columns[node->Type()], i, m_bInCompare);
				line << variant.GetString();
			}
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


MenuIdentityPopup CAgilityBookTreeModel::GetMenuID(wxDataViewItemArray const& items) const
{
	MenuIdentityPopup id = IdMenuNone;
	for (wxDataViewItemArray::const_iterator i = items.begin(); i != items.end(); ++i)
	{
		CAgilityBookTreeData* pNode = GetNode(*i);
		if (pNode)
		{
			if (IdMenuNone == id)
				id = pNode->GetMenuID();
			else if (pNode->GetMenuID() != id)
			{
				id = IdMenuRunDifferent;
				break;
			}
		}
	}
	return id;
}
