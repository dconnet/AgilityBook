/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
 */

/**
 * @file
 *
 * @brief implementation of the CDlgConfigure class
 * @author David Connet
 *
 * Revision History
 * @li 2003-02-23 DRC Modifying the scoring methods wasn't updated in the tree.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgConfigureData.h"

#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "ARBConfigDivision.h"
#include "ARBConfigVenue.h"
#include "DlgConfigEvent.h"
#include "DlgConfigOtherPoints.h"
#include "DlgConfigure.h"
#include "DlgFixup.h"
#include "DlgName.h"
#include "DlgNameDesc.h"
#include "DlgReorder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////

void CDlgConfigureData::SetItemCurrent(HTREEITEM hItem, bool bExpand)
{
	if (NULL == hItem)
		hItem = m_hItem;
	m_pDlg->m_bReset = false;
	m_pDlg->m_ctrlTree.SelectItem(hItem);
	if (bExpand)
		m_pDlg->m_ctrlTree.Expand(hItem, TVE_EXPAND);
}

CDlgConfigureDataVenue* CDlgConfigureData::GetVenueData()
{
	CDlgConfigureData* pData = this;
	CDlgConfigureDataVenue* pVenueData = NULL;
	switch (m_Type)
	{
	default:
	case eUserData:
	case eVenues:
		break;
	case eFaultTypes:
		pVenueData = dynamic_cast<CDlgConfigureDataVenue*>(GetParentData());
		ASSERT(pVenueData);
		break;
	case eOtherPoints:
		pVenueData = dynamic_cast<CDlgConfigureDataVenue*>(GetParentData());
		ASSERT(pVenueData);
		break;
	case eDivisions:
		pVenueData = dynamic_cast<CDlgConfigureDataVenue*>(GetParentData());
		ASSERT(pVenueData);
		break;
	case eLevels:
		pData = GetParentData(); // Gives back user-Division
		ASSERT(pData->m_Type == eUserData);
		pVenueData = pData->GetVenueData();
		ASSERT(pVenueData);
		break;
	case eTitles:
		pData = GetParentData(); // Gives back user-Division
		ASSERT(pData->m_Type == eUserData);
		pVenueData = pData->GetVenueData();
		ASSERT(pVenueData);
		break;
	case eEvents:
		pData = GetParentData(); // Gives back user-Division
		ASSERT(pData->m_Type == eUserData);
		pVenueData = pData->GetVenueData();
		ASSERT(pVenueData);
		break;
	case eScoringInfo:
		pData = GetParentData(); // Gives back UserData
		ASSERT(pData->m_Type == eUserData);
		pData = pData->GetParentData(); // Gives back eScoring
		ASSERT(pData->m_Type == eScoring);
		// Fall-thru
	case eScoring:
		pData = pData->GetParentData(); // Gives back user-Event
		ASSERT(pData->m_Type == eUserData);
		pVenueData = pData->GetVenueData();
		ASSERT(pVenueData);
		break;
	}
	return pVenueData;
}

CDlgConfigureDataDivision* CDlgConfigureData::GetDivisionData()
{
	CDlgConfigureData* pData = this;
	CDlgConfigureDataDivision* pDivData = NULL;
	switch (m_Type)
	{
	default:
	case eUserData:
	case eVenues:
	case eFaultTypes:
	case eOtherPoints:
	case eDivisions:
		break;
	case eLevels:
		pDivData = dynamic_cast<CDlgConfigureDataDivision*>(GetParentData());
		ASSERT(pDivData);
		break;
	case eTitles:
		pDivData = dynamic_cast<CDlgConfigureDataDivision*>(GetParentData());
		ASSERT(pDivData);
		break;
	case eEvents:
		pDivData = dynamic_cast<CDlgConfigureDataDivision*>(GetParentData());
		ASSERT(pDivData);
		break;
	case eScoringInfo:
		pData = GetParentData(); // Gives back UserData
		ASSERT(pData->m_Type == eUserData);
		pData = pData->GetParentData(); // Gives back eScoring
		ASSERT(pData->m_Type == eScoring);
		// Fall-thru
	case eScoring:
		pData = pData->GetParentData(); // Gives back user-Event
		ASSERT(pData->m_Type == eUserData);
		pDivData = pData->GetDivisionData();
		ASSERT(pDivData);
		break;
	}
	return pDivData;
}

CDlgConfigureDataEvent* CDlgConfigureData::GetEventData()
{
	CDlgConfigureData* pData = this;
	CDlgConfigureDataEvent* pEventData = NULL;
	switch (m_Type)
	{
	default:
	case eUserData:
	case eVenues:
	case eFaultTypes:
	case eOtherPoints:
	case eDivisions:
	case eLevels:
	case eTitles:
	case eEvents:
		break;
	case eScoringInfo:
		pData = GetParentData(); // Gives back UserData
		ASSERT(pData->m_Type == eUserData);
		pData = pData->GetParentData(); // Gives back eScoring
		ASSERT(pData->m_Type == eScoring);
		// Fall-thru
	case eScoring:
		pData = pData->GetParentData(); // Gives back user-Event
		ASSERT(pData->m_Type == eUserData);
		pEventData = pData->GetEventData();
		ASSERT(pEventData);
		break;
	}
	return pEventData;
}

BOOL CDlgConfigureData::CanAdd() const
{
	BOOL bCan = FALSE;
	switch (m_Type)
	{
	default:
	case eUserData:
	case eScoring:
	case eScoringInfo:
		break;
	case eVenues:
	case eFaultTypes:
	case eOtherPoints:
	case eDivisions:
	case eLevels:
	case eTitles:
	case eEvents:
		bCan = TRUE;
	}
	return bCan;
}

void CDlgConfigureData::Add()
{
	std::string name;
	bool done = false;
	switch (m_Type)
	{
	default:
	case eUserData:
	case eScoring:
	case eScoringInfo:
		break;

	case eVenues:
		while (!done)
		{
			done = true;
			CDlgNameDesc dlg(name.c_str(), "", IDS_VENUE_NAME, m_pDlg);
			if (IDOK == dlg.DoModal())
			{
				name = dlg.GetName();
				if (GetConfig().GetVenues().FindVenue(name))
				{
					done = false;
					AfxMessageBox(IDS_NAME_IN_USE);
					continue;
				}
				ARBConfigVenue* pVenue = GetConfig().GetVenues().AddVenue(name);
				if (pVenue)
				{
					pVenue->SetDesc(dlg.GetDesc());
					HTREEITEM hItem = m_pDlg->InsertVenue(m_hItem, pVenue);
					SetItemCurrent(hItem, true);
				}
			}
		}
		break;

	case eFaultTypes:
		{
			CDlgName dlg(name.c_str(), IDS_FAULT_TYPE_NAME, m_pDlg);
			if (IDOK == dlg.DoModal())
			{
				name = dlg.GetName();
				// We could check for uniqueness, but if the user wants 2
				// strings the same, why argue! Afterall, these strings
				// are only "helper" items to fill in other data.
				if (0 < name.length())
				{
					ARBConfigFault* pFault = GetConfig().GetFaults().AddFault(name);
					if (pFault)
					{
						HTREEITEM hItem = m_pDlg->InsertFaultType(m_hItem, pFault);
						SetItemCurrent(hItem, true);
					}
				}
			}
		}
		break;

	case eOtherPoints:
		{
			ARBConfigOtherPoints* other = new ARBConfigOtherPoints;
			// The dialog will ensure uniqueness.
			CDlgConfigOtherPoints dlg(GetConfig(), other, m_pDlg);
			if (IDOK == dlg.DoModal())
			{
				ARBConfigOtherPoints* pOther = GetConfig().GetOtherPoints().AddOtherPoints(other);
				if (pOther)
				{
					HTREEITEM hItem = m_pDlg->InsertOtherPoint(m_hItem, pOther);
					SetItemCurrent(hItem, true);
				}
			}
			other->Release();
		}
		break;

	case eDivisions:
		while (!done)
		{
			done = true;
			CDlgName dlg(name.c_str(), IDS_DIVISION_NAME, m_pDlg);
			if (IDOK == dlg.DoModal())
			{
				CDlgConfigureDataVenue* pVenueData = GetVenueData();
				ASSERT(pVenueData);
				name = dlg.GetName();
				if (pVenueData->GetVenue()->GetDivisions().FindDivision(name))
				{
					done = false;
					AfxMessageBox(IDS_NAME_IN_USE);
					continue;
				}
				ARBConfigDivision* pDiv = pVenueData->GetVenue()->GetDivisions().AddDivision(name);
				if (pDiv)
				{
					HTREEITEM hItem = m_pDlg->InsertDivision(m_hItem, pDiv);
					SetItemCurrent(hItem, true);
				}
			}
		}
		break;

	case eLevels:
		while (!done)
		{
			done = true;
			CDlgName dlg(name.c_str(), IDS_LEVEL_NAME, m_pDlg);
			if (IDOK == dlg.DoModal())
			{
				CDlgConfigureDataDivision* pDivData = GetDivisionData();
				ASSERT(pDivData);
				name = dlg.GetName();
				if (pDivData->GetDivision()->GetLevels().FindTrueLevel(name))
				{
					done = false;
					AfxMessageBox(IDS_NAME_IN_USE);
					continue;
				}
				ARBConfigLevel* pLevel = pDivData->GetDivision()->GetLevels().AddLevel(name);
				if (pLevel)
				{
					HTREEITEM hItem = m_pDlg->InsertLevel(m_hItem, pLevel);
					SetItemCurrent(hItem, true);
				}
			}
		}
		break;

	case eTitles:
		while (!done)
		{
			done = true;
			CDlgNameDesc dlg(name.c_str(), "", IDS_TITLE_NAME, m_pDlg);
			if (IDOK == dlg.DoModal())
			{
				CDlgConfigureDataDivision* pDivData = GetDivisionData();
				ASSERT(pDivData);
				name = dlg.GetName();
				if (pDivData->GetDivision()->GetTitles().FindTitle(name))
				{
					done = false;
					AfxMessageBox(IDS_NAME_IN_USE);
					continue;
				}
				ARBConfigTitle* pTitle = pDivData->GetDivision()->GetTitles().AddTitle(name);
				pTitle->SetDescription(dlg.GetDesc());
				if (pTitle)
				{
					HTREEITEM hItem = m_pDlg->InsertTitle(m_hItem, pTitle);
					SetItemCurrent(hItem, true);
				}
			}
		}
		break;

	case eEvents:
		{
			// The dialog will ensure uniqueness.
			CDlgConfigureDataVenue* pVenueData = GetVenueData();
			ASSERT(pVenueData);
			ARBConfigEvent* event = new ARBConfigEvent;
			CDlgConfigEvent dlg(pVenueData->GetVenue(), event, m_pDlg);
			if (IDOK == dlg.DoModal())
			{
				ARBConfigEvent* pEvent = pVenueData->GetVenue()->GetEvents().AddEvent(event);
				if (pEvent)
				{
					HTREEITEM hItem = m_pDlg->InsertEvent(m_hItem, pEvent);
					SetItemCurrent(hItem, true);
				}
			}
			event->Release();
		}
		break;
	}
}

BOOL CDlgConfigureData::CanEdit() const
{
	BOOL bCan = FALSE;
	switch (m_Type)
	{
	case eUserData:
	case eVenues:
	case eFaultTypes:
	case eOtherPoints:
	case eDivisions:
	case eLevels:
	case eTitles:
	case eEvents:
		break;
	case eScoring:
	case eScoringInfo:
		bCan = const_cast<CDlgConfigureData*>(this)->GetEventData()->CanEdit();
		break;
	}
	return bCan;
}

void CDlgConfigureData::Edit()
{
	switch (m_Type)
	{
	case eUserData:
	case eVenues:
	case eFaultTypes:
	case eOtherPoints:
	case eDivisions:
	case eLevels:
	case eTitles:
	case eEvents:
		break;
	case eScoring:
	case eScoringInfo:
		GetEventData()->Edit();
		break;
	}
}

BOOL CDlgConfigureData::CanDelete() const
{
	BOOL bCan = FALSE;
	switch (m_Type)
	{
	default:
	case eUserData:
	case eVenues:
	case eScoringInfo:
	case eDivisions:
	case eLevels:
	case eEvents:
	case eScoring:
		break;
	case eFaultTypes:
	case eOtherPoints:
	case eTitles:
		if (m_pDlg->m_ctrlTree.ItemHasChildren(m_hItem))
		{
			HTREEITEM hItem = m_pDlg->m_ctrlTree.GetChildItem(m_hItem);
			while (!bCan && NULL != hItem)
			{
				CDlgConfigureData* pData = m_pDlg->GetItemData(hItem);
				hItem = m_pDlg->m_ctrlTree.GetNextSiblingItem(hItem);
				if (pData && pData->CanDelete())
				{
					bCan = TRUE;
				}
			}
		}
		break;
	}
	return bCan;
}

void CDlgConfigureData::Delete()
{
	UINT idDelete = 0;
	CString extraWarning;
	switch (m_Type)
	{
	default:
	case eUserData:
		// There are certain 'heading' items that we don't want to allow a
		// delete on. A user could mess up their data way too easily that way!
	case eVenues:
	case eDivisions:
	case eLevels:
	case eEvents:
	case eScoring:
	case eScoringInfo:
		break;
	case eFaultTypes:
		idDelete = IDS_FAULT_TYPES;
		break;
	case eOtherPoints:
		idDelete = IDS_OTHERPOINTS;
		extraWarning.LoadString(IDS_DELETE_ALL_OTHERPOINTS);
		break;
	case eTitles:
		idDelete = IDS_TITLES;
		extraWarning.LoadString(IDS_DELETE_ALL_TITLES);
		break;
	}
	if (0 != idDelete)
	{
		CString str, msg;
		str.LoadString(idDelete);
		msg.FormatMessage(IDS_DELETE_ALL, (LPCTSTR)str);
		if (IDYES == AfxMessageBox(msg, MB_ICONQUESTION | MB_YESNO))
		{
			if (!extraWarning.IsEmpty())
			{
				if (IDYES != AfxMessageBox(extraWarning, MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2))
					return;
			}
			HTREEITEM hItem = m_pDlg->m_ctrlTree.GetChildItem(m_hItem);
			while (NULL != hItem)
			{
				CDlgConfigureData* pData = m_pDlg->GetItemData(hItem);
				hItem = m_pDlg->m_ctrlTree.GetNextSiblingItem(hItem);
				if (pData && pData->CanDelete())
				{
					pData->Delete();
				}
			}
		}
	}
}

CDlgConfigureData* CDlgConfigureData::GetParentData() const
{
	if (!m_hItem)
		return NULL;
	HTREEITEM hParent = m_pDlg->m_ctrlTree.GetParentItem(m_hItem);
	if (!hParent)
		return NULL;
	CDlgConfigureData* pData = dynamic_cast<CDlgConfigureData*>(m_pDlg->GetItemData(hParent));
	return pData;
}

BOOL CDlgConfigureData::CanParentAdd() const
{
	BOOL bCanAdd = FALSE;
	HTREEITEM hParent = m_pDlg->m_ctrlTree.GetParentItem(m_hItem);
	if (NULL != hParent)
	{
		CDlgConfigureData* pData = m_pDlg->GetItemData(hParent);
		if (pData)
			bCanAdd = pData->CanAdd();
	}
	return bCanAdd;
}

void CDlgConfigureData::ParentAdd()
{
	HTREEITEM hParent = m_pDlg->m_ctrlTree.GetParentItem(m_hItem);
	if (NULL != hParent)
	{
		CDlgConfigureData* pData = m_pDlg->GetItemData(hParent);
		if (pData)
			pData->Add();
	}
}

CTreeCtrl& CDlgConfigureData::GetTree() const
{
	return m_pDlg->m_ctrlTree;
}

ARBAgilityRecordBook& CDlgConfigureData::GetRecordBook() const
{
	return m_pDlg->m_Book;
}

ARBConfig& CDlgConfigureData::GetConfig() const
{
	return m_pDlg->m_Config;
}

std::vector<CDlgFixup*>& CDlgConfigureData::GetFixupList() const
{
	return m_pDlg->m_DlgFixup;
}

void CDlgConfigureData::InsertScorings(HTREEITEM hParent, ARBConfigEvent* event)
{
	m_pDlg->InsertScorings(hParent, event);
}

/////////////////////////////////////////////////////////////////////////////

void CDlgConfigureDataVenue::Duplicate()
{
	CString copyOf;
	copyOf.LoadString(IDS_COPYOF);
	std::string name(m_Venue->GetName());
	while (GetConfig().GetVenues().FindVenue(name))
	{
		name = (LPCSTR)copyOf + name;
	}
	ARBConfigVenue* pVenue = GetConfig().GetVenues().AddVenue(name);
	if (pVenue)
	{
		*pVenue = *m_Venue;
		pVenue->SetName(name); // Put the name back.
		HTREEITEM hItem = m_pDlg->InsertVenue(GetParentData()->GetHTreeItem(), pVenue);
		SetItemCurrent(hItem, true);
	}
}

void CDlgConfigureDataVenue::Edit()
{
	bool done = false;
	std::string oldName = m_Venue->GetName();
	std::string name(oldName);
	while (!done)
	{
		done = true;
		CDlgNameDesc dlg(name.c_str(), m_Venue->GetDesc().c_str(), IDS_VENUE_NAME, m_pDlg);
		if (IDOK == dlg.DoModal())
		{
			name = dlg.GetName();
			if (oldName != name)
			{
				if (GetConfig().GetVenues().FindVenue(name))
				{
					done = false;
					AfxMessageBox(IDS_NAME_IN_USE);
					continue;
				}
				m_Venue->SetName(name);
			}
			m_Venue->SetDesc(dlg.GetDesc());
			if (oldName != name)
			{
				GetFixupList().push_back(new CDlgFixupRenameVenue(oldName, m_Venue->GetName()));
				GetTree().SetItem(m_hItem,
					TVIF_TEXT,
					m_Venue->GetName().c_str(),
					0, 0, 0, 0, 0);
				SetItemCurrent();
			}
		}
	}
}

void CDlgConfigureDataVenue::Delete()
{
	bool bDelete = true;
	std::string venue = m_Venue->GetName();
	int nRegNums = GetRecordBook().GetDogs().NumRegNumsInVenue(venue);
	int nTitles = GetRecordBook().GetDogs().NumTitlesInVenue(venue);
	int nTrials = GetRecordBook().GetDogs().NumTrialsInVenue(venue);
	if (0 < nRegNums || 0 < nTitles || 0 < nTrials)
	{
		CString msg;
		msg.FormatMessage(IDS_DELETE_VENUE,
			venue.c_str(),
			nRegNums,
			nTitles,
			nTrials);
		if (IDYES != AfxMessageBox(msg, MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2))
			bDelete = false;
	}
	if (bDelete)
	{
		// If we were able to delete it...
		if (GetConfig().GetVenues().DeleteVenue(venue))
		{
			// Then we commit to fixing the real data.
			if (0 < nRegNums || 0 < nTitles || 0 < nTrials)
				GetFixupList().push_back(new CDlgFixupDeleteVenue(venue));
			GetTree().DeleteItem(m_hItem);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

void CDlgConfigureDataFault::Duplicate()
{
	std::string name(m_Fault->GetName());
	ARBConfigFault* pFault = GetConfig().GetFaults().AddFault(name);
	if (pFault)
	{
		HTREEITEM hItem = m_pDlg->InsertFaultType(GetParentData()->GetHTreeItem(), pFault);
		SetItemCurrent(hItem, true);
	}
}

void CDlgConfigureDataFault::Edit()
{
	bool done = false;
	std::string oldName = m_Fault->GetName();
	std::string name(oldName);
	while (!done)
	{
		done = true;
		CDlgName dlg(name.c_str(), IDS_FAULT_TYPE_NAME, m_pDlg);
		if (IDOK == dlg.DoModal())
		{
			name = dlg.GetName();
			if (oldName != name)
			{
				if (GetConfig().GetFaults().FindFault(name))
				{
					done = false;
					AfxMessageBox(IDS_NAME_IN_USE);
					continue;
				}
				m_Fault->SetName(name);
				// No fixup necessary for faults.
				GetTree().SetItem(m_hItem,
					TVIF_TEXT,
					m_Fault->GetName().c_str(),
					0, 0, 0, 0, 0);
				SetItemCurrent();
			}
		}
	}
}

void CDlgConfigureDataFault::Delete()
{
	if (GetConfig().GetFaults().DeleteFault(m_Fault->GetName()))
	{
		// Then we commit to fixing the real data.
		GetTree().DeleteItem(m_hItem);
		// No fixup necessary for faults.
	}
}

/////////////////////////////////////////////////////////////////////////////

void CDlgConfigureDataOtherPoints::Duplicate()
{
	CString copyOf;
	copyOf.LoadString(IDS_COPYOF);
	std::string name(m_OtherPoints->GetName());
	while (GetConfig().GetOtherPoints().FindOtherPoints(name))
	{
		name = (LPCSTR)copyOf + name;
	}
	ARBConfigOtherPoints* other = new ARBConfigOtherPoints(*m_OtherPoints);
	other->SetName(name);
	CDlgConfigureDataVenue* pVenueData = GetVenueData();
	ASSERT(pVenueData);
	ARBConfigOtherPoints* pOther = GetConfig().GetOtherPoints().AddOtherPoints(other);
	if (pOther)
	{
		HTREEITEM hItem = m_pDlg->InsertOtherPoint(GetParentData()->GetHTreeItem(), pOther);
		SetItemCurrent(hItem, true);
	}
	other->Release();
}

void CDlgConfigureDataOtherPoints::Edit()
{
	std::string oldName = m_OtherPoints->GetName();
	CDlgConfigOtherPoints dlg(GetConfig(), m_OtherPoints, m_pDlg);
	if (IDOK == dlg.DoModal())
	{
		if (m_OtherPoints->GetName() != oldName)
		{
			// Other things may have changed, but we only care about the name for fixup.
			GetFixupList().push_back(new CDlgFixupRenameOtherPoints(oldName, m_OtherPoints->GetName()));
			GetTree().SetItem(m_hItem,
				TVIF_TEXT,
				m_OtherPoints->GetName().c_str(),
				0, 0, 0, 0, 0);
			SetItemCurrent();
		}
	}
}

void CDlgConfigureDataOtherPoints::Delete()
{
	bool bDelete = true;
	std::string otherPoints = m_OtherPoints->GetName();
	int nOther = GetRecordBook().GetDogs().NumOtherPointsInUse(otherPoints);
	if (0 < nOther)
	{
		CString msg;
		msg.FormatMessage(IDS_DELETE_OTHERPOINTS,
			otherPoints.c_str(),
			nOther);
		if (IDYES != AfxMessageBox(msg, MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2))
			bDelete = false;
	}
	if (bDelete)
	{
		if (GetConfig().GetOtherPoints().DeleteOtherPoints(otherPoints))
		{
			// Then we commit to fixing the real data.
			if (0 < nOther)
				GetFixupList().push_back(new CDlgFixupDeleteOtherPoints(otherPoints));
			GetTree().DeleteItem(m_hItem);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataVenue* CDlgConfigureDataDivision::GetVenueData()
{
	CDlgConfigureData* pData = GetParentData();
	ASSERT(pData->GetType() == eDivisions);
	return pData->GetVenueData();
}

void CDlgConfigureDataDivision::Edit()
{
	bool done = false;
	std::string oldName = m_Div->GetName();
	std::string name(oldName);
	CDlgConfigureDataVenue* pVenueData = GetVenueData();
	ASSERT(pVenueData);
	if (0 < GetRecordBook().GetDogs().NumMultiHostedTrialsInDivision(GetRecordBook().GetConfig(), pVenueData->GetVenue()->GetName(), name))
	{
		if (IDYES != AfxMessageBox(IDS_CHANGEDIV_ISSUES, MB_ICONQUESTION | MB_YESNO))
			return;
	}
	while (!done)
	{
		done = true;
		CDlgName dlg(name.c_str(), IDS_DIVISION_NAME, m_pDlg);
		if (IDOK == dlg.DoModal())
		{
			name = dlg.GetName();
			if (oldName != name)
			{
				if (pVenueData->GetVenue()->GetDivisions().FindDivision(name))
				{
					done = false;
					AfxMessageBox(IDS_NAME_IN_USE);
					continue;
				}
				m_Div->SetName(name);
				pVenueData->GetVenue()->GetEvents().RenameDivision(oldName, name);
				GetFixupList().push_back(new CDlgFixupRenameDivision(pVenueData->GetVenue()->GetName(), oldName, m_Div->GetName()));
				GetTree().SetItem(m_hItem,
					TVIF_TEXT,
					m_Div->GetName().c_str(),
					0, 0, 0, 0, 0);
				SetItemCurrent();
			}
		}
	}
}

void CDlgConfigureDataDivision::Delete()
{
	bool bDelete = true;
	std::string div = m_Div->GetName();
	CDlgConfigureDataVenue* pVenueData = GetVenueData();
	int nTrials = GetRecordBook().GetDogs().NumRunsInDivision(pVenueData->GetVenue(), div);
	int nTitles = GetRecordBook().GetDogs().NumTitlesInDivision(pVenueData->GetVenue(), div);
	if (0 < nTrials || 0 < nTitles)
	{
		CString msg;
		msg.FormatMessage(IDS_DELETE_DIVISION,
			div.c_str(),
			nTitles,
			nTrials);
		if (IDYES != AfxMessageBox(msg, MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2))
			bDelete = false;
	}
	if (bDelete)
	{
		// If we were able to delete it...
		if (pVenueData->GetVenue()->GetDivisions().DeleteDivision(div, pVenueData->GetVenue()->GetEvents()))
		{
			// Then we commit to fixing the real data.
			if (0 < nTrials || 0 < nTitles)
				GetFixupList().push_back(new CDlgFixupDeleteDivision(pVenueData->GetVenue()->GetName(), div));
			GetTree().DeleteItem(m_hItem);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataVenue* CDlgConfigureDataLevel::GetVenueData()
{
	CDlgConfigureData* pData = GetParentData();
	ASSERT(pData->GetType() == eLevels);
	return pData->GetVenueData();
}

CDlgConfigureDataDivision* CDlgConfigureDataLevel::GetDivisionData()
{
	CDlgConfigureData* pData = GetParentData();
	ASSERT(pData->GetType() == eLevels);
	return pData->GetDivisionData();
}

void CDlgConfigureDataLevel::Add()
{
	bool done = false;
	std::string name;
	while (!done)
	{
		done = true;
		CDlgName dlg(name.c_str(), IDS_SUBLEVEL_NAME, m_pDlg);
		if (IDOK == dlg.DoModal())
		{
			CDlgConfigureDataDivision* pDivData = GetDivisionData();
			ASSERT(pDivData);
			name = dlg.GetName();
			if (pDivData->GetDivision()->GetLevels().FindTrueLevel(name))
			{
				done = false;
				AfxMessageBox(IDS_NAME_IN_USE);
				continue;
			}
			ARBConfigSubLevel* pLevel = m_Level->GetSubLevels().AddSubLevel(name);
			if (pLevel)
			{
				HTREEITEM hItem = m_pDlg->InsertSubLevel(m_hItem, pLevel);
				SetItemCurrent(hItem, true);
			}
		}
	}
}

void CDlgConfigureDataLevel::Edit()
{
	bool done = false;
	std::string oldName = m_Level->GetName();
	std::string name(oldName);
	CDlgConfigureDataVenue* pVenueData = GetVenueData();
	ASSERT(pVenueData);
	CDlgConfigureDataDivision* pDivData = GetDivisionData();
	ASSERT(pDivData);
	// If there are sublevels, don't ask the following question. If a level has
	// sublevels, the level name isn't allowed to be used for an event.
	if (0 == m_Level->GetSubLevels().size())
	{
		if (0 < GetRecordBook().GetDogs().NumMultiHostedTrialsInDivision(GetRecordBook().GetConfig(), pVenueData->GetVenue()->GetName(), pDivData->GetDivision()->GetName()))
		{
			if (IDYES != AfxMessageBox(IDS_CHANGELEVEL_ISSUES, MB_ICONQUESTION | MB_YESNO))
				return;
		}
	}
	while (!done)
	{
		done = true;
		CDlgName dlg(name.c_str(), IDS_LEVEL_NAME, m_pDlg);
		if (IDOK == dlg.DoModal())
		{
			name = dlg.GetName();
			// If the name hasn't changed, don't do the inuse verification.
			if (oldName != name)
			{
				bool bInUse = false;
				if (0 == m_Level->GetSubLevels().size())
				{
					if (pDivData->GetDivision()->GetLevels().FindTrueLevel(name))
						bInUse = true;
				}
				else
				{
					if (pDivData->GetDivision()->GetLevels().VerifyLevel(name, false))
						bInUse = true;
				}
				if (bInUse)
				{
					done = false;
					AfxMessageBox(IDS_NAME_IN_USE);
					continue;
				}
				m_Level->SetName(name);
				if (0 == m_Level->GetSubLevels().size())
					pVenueData->GetVenue()->GetEvents().RenameLevel(oldName, name);
				GetFixupList().push_back(new CDlgFixupRenameLevel(
					pVenueData->GetVenue()->GetName(),
					pDivData->GetDivision()->GetName(),
					oldName, m_Level->GetName()));
				GetTree().SetItem(m_hItem,
					TVIF_TEXT,
					m_Level->GetName().c_str(),
					0, 0, 0, 0, 0);
				SetItemCurrent();
			}
		}
	}
}

void CDlgConfigureDataLevel::Delete()
{
	CDlgConfigureDataVenue* pVenueData = GetVenueData();
	ASSERT(pVenueData);
	CDlgConfigureDataDivision* pDivData = GetDivisionData();
	ASSERT(pDivData);
	bool bDelete = true;
	std::string level = m_Level->GetName();
	int nLevels = GetRecordBook().GetDogs().NumLevelsInUse(
		pVenueData->GetVenue()->GetName(),
		pDivData->GetDivision()->GetName(),
		level);
	if (0 < nLevels)
	{
		CString msg;
		msg.FormatMessage(IDS_DELETE_LEVEL,
			level.c_str(),
			nLevels);
		if (IDYES != AfxMessageBox(msg, MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2))
			bDelete = false;
	}
	if (bDelete)
	{
		if (pDivData->GetDivision()->GetLevels().DeleteLevel(level, pVenueData->GetVenue()->GetEvents()))
		{
			// Then we commit to fixing the real data.
			if (0 < nLevels)
				GetFixupList().push_back(new CDlgFixupDeleteLevel(
					pVenueData->GetVenue()->GetName(),
					pDivData->GetDivision()->GetName(),
					level));
			GetTree().DeleteItem(m_hItem);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataVenue* CDlgConfigureDataSubLevel::GetVenueData()
{
	CDlgConfigureDataLevel* pData = dynamic_cast<CDlgConfigureDataLevel*>(GetParentData());
	ASSERT(pData);
	return pData->GetVenueData();
}

CDlgConfigureDataDivision* CDlgConfigureDataSubLevel::GetDivisionData()
{
	CDlgConfigureDataLevel* pData = dynamic_cast<CDlgConfigureDataLevel*>(GetParentData());
	ASSERT(pData);
	return pData->GetDivisionData();
}

CDlgConfigureDataLevel* CDlgConfigureDataSubLevel::GetLevelData()
{
	CDlgConfigureDataLevel* pData = dynamic_cast<CDlgConfigureDataLevel*>(GetParentData());
	ASSERT(pData);
	return pData;
}

void CDlgConfigureDataSubLevel::Edit()
{
	bool done = false;
	std::string oldName = m_SubLevel->GetName();
	std::string name(oldName);
	CDlgConfigureDataVenue* pVenueData = GetVenueData();
	ASSERT(pVenueData);
	CDlgConfigureDataDivision* pDivData = GetDivisionData();
	ASSERT(pDivData);
	if (0 < GetRecordBook().GetDogs().NumMultiHostedTrialsInDivision(GetRecordBook().GetConfig(), pVenueData->GetVenue()->GetName(), pDivData->GetDivision()->GetName()))
	{
		if (IDYES != AfxMessageBox(IDS_CHANGESUBLEVEL_ISSUES, MB_ICONQUESTION | MB_YESNO))
			return;
	}
	while (!done)
	{
		done = true;
		CDlgName dlg(name.c_str(), IDS_LEVEL_NAME, m_pDlg);
		if (IDOK == dlg.DoModal())
		{
			name = dlg.GetName();
			if (oldName != name)
			{
				if (pDivData->GetDivision()->GetLevels().FindTrueLevel(name))
				{
					done = false;
					AfxMessageBox(IDS_NAME_IN_USE);
					continue;
				}
				m_SubLevel->SetName(name);
				// No need to fix ARBConfigEventList cause we don't do sublevel names in events.
				GetFixupList().push_back(new CDlgFixupRenameLevel(
					pVenueData->GetVenue()->GetName(),
					pDivData->GetDivision()->GetName(),
					oldName, m_SubLevel->GetName()));
				GetTree().SetItem(m_hItem,
					TVIF_TEXT,
					m_SubLevel->GetName().c_str(),
					0, 0, 0, 0, 0);
				SetItemCurrent();
			}
		}
	}
}

void CDlgConfigureDataSubLevel::Delete()
{
	CDlgConfigureDataVenue* pVenueData = GetVenueData();
	ASSERT(pVenueData);
	CDlgConfigureDataDivision* pDivData = GetDivisionData();
	ASSERT(pDivData);
	CDlgConfigureDataLevel* pLevelData = GetLevelData();
	ASSERT(pLevelData);
	std::string level = pLevelData->GetLevel()->GetName();
	bool bDelete = true;
	std::string subLevel = m_SubLevel->GetName();
	int nLevels = GetRecordBook().GetDogs().NumLevelsInUse(
		pVenueData->GetVenue()->GetName(),
		pDivData->GetDivision()->GetName(),
		subLevel);
	if (0 < nLevels)
	{
		CString msg;
		msg.FormatMessage(IDS_DELETE_LEVEL,
			subLevel.c_str(),
			nLevels);
		if (IDYES != AfxMessageBox(msg, MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2))
			bDelete = false;
	}
	if (bDelete)
	{
		bool bLevelModified = false;
		if (pDivData->GetDivision()->GetLevels().DeleteSubLevel(subLevel, bLevelModified))
		{
			// Then we commit to fixing the real data.
			if (bLevelModified)
			{
				pVenueData->GetVenue()->GetEvents().RenameLevel(level, pLevelData->GetLevel()->GetName());
				GetFixupList().push_back(new CDlgFixupRenameLevel(
					pVenueData->GetVenue()->GetName(),
					pDivData->GetDivision()->GetName(),
					level, pLevelData->GetLevel()->GetName()));
				GetTree().SetItem(pLevelData->GetHTreeItem(),
					TVIF_TEXT,
					pLevelData->GetLevel()->GetName().c_str(),
					0, 0, 0, 0, 0);
			}
			if (0 < nLevels)
				GetFixupList().push_back(new CDlgFixupDeleteLevel(
					pVenueData->GetVenue()->GetName(),
					pDivData->GetDivision()->GetName(),
					subLevel));
			GetTree().DeleteItem(m_hItem);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataVenue* CDlgConfigureDataTitle::GetVenueData()
{
	CDlgConfigureData* pData = GetParentData();
	ASSERT(pData->GetType() == eTitles);
	return pData->GetVenueData();
}

CDlgConfigureDataDivision* CDlgConfigureDataTitle::GetDivisionData()
{
	CDlgConfigureData* pData = GetParentData();
	ASSERT(pData->GetType() == eTitles);
	return pData->GetDivisionData();
}

void CDlgConfigureDataTitle::Edit()
{
	CDlgConfigureDataVenue* pVenueData = GetVenueData();
	ASSERT(pVenueData);
	bool done = false;
	std::string oldName = m_Title->GetName();
	std::string name(oldName);
	while (!done)
	{
		done = true;
		CDlgNameDesc dlg(name.c_str(), m_Title->GetDescription().c_str(), IDS_TITLE_NAME, m_pDlg);
		if (IDOK == dlg.DoModal())
		{
			name = dlg.GetName();
			if (oldName != name)
			{
				if (pVenueData->GetVenue()->GetDivisions().FindTitle(name))
				{
					done = false;
					AfxMessageBox(IDS_NAME_IN_USE);
					continue;
				}
				m_Title->SetName(name);
			}
			m_Title->SetDescription(dlg.GetDesc());
			if (name != oldName)
			{
				GetFixupList().push_back(new CDlgFixupRenameTitle(pVenueData->GetVenue()->GetName(), oldName, m_Title->GetName()));
				GetTree().SetItem(m_hItem,
					TVIF_TEXT,
					m_Title->GetName().c_str(),
					0, 0, 0, 0, 0);
				SetItemCurrent();
			}
		}
	}
}

void CDlgConfigureDataTitle::Delete()
{
	CDlgConfigureDataVenue* pVenueData = GetVenueData();
	ASSERT(pVenueData);
	bool bDelete = true;
	std::string title = m_Title->GetName();
	int nTitles = GetRecordBook().GetDogs().NumTitlesInUse(pVenueData->GetVenue()->GetName(), title);
	if (0 < nTitles)
	{
		CString msg;
		msg.FormatMessage(IDS_DELETE_TITLE,
			title.c_str(),
			nTitles);
		if (IDYES != AfxMessageBox(msg, MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2))
			bDelete = false;
	}
	if (bDelete)
	{
		CDlgConfigureDataDivision* pDivData = GetDivisionData();
		ASSERT(pDivData);
		if (pDivData->GetDivision()->GetTitles().DeleteTitle(title))
		{
			// Then we commit to fixing the real data.
			if (0 < nTitles)
				GetFixupList().push_back(new CDlgFixupDeleteTitle(pVenueData->GetVenue()->GetName(), title));
			GetTree().DeleteItem(m_hItem);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataVenue* CDlgConfigureDataEvent::GetVenueData()
{
	CDlgConfigureData* pData = GetParentData();
	ASSERT(pData->GetType() == eEvents);
	return pData->GetVenueData();
}

CDlgConfigureDataDivision* CDlgConfigureDataEvent::GetDivisionData()
{
	CDlgConfigureData* pData = GetParentData();
	ASSERT(pData->GetType() == eEvents);
	return pData->GetDivisionData();
}

void CDlgConfigureDataEvent::Duplicate()
{
	CString copyOf;
	copyOf.LoadString(IDS_COPYOF);
	std::string name(m_Event->GetName());
	CDlgConfigureDataVenue* pVenueData = GetVenueData();
	ASSERT(pVenueData);
	while (pVenueData->GetVenue()->GetEvents().FindEvent(name))
	{
		name = (LPCSTR)copyOf + name;
	}
	ARBConfigEvent* event = new ARBConfigEvent(*m_Event);
	event->SetName(name);
	ARBConfigEvent* pEvent = pVenueData->GetVenue()->GetEvents().AddEvent(event);
	if (pEvent)
	{
		HTREEITEM hItem = m_pDlg->InsertEvent(GetParentData()->GetHTreeItem(), pEvent);
		SetItemCurrent(hItem, true);
	}
	event->Release();
}

void CDlgConfigureDataEvent::Edit()
{
	CDlgConfigureDataVenue* pVenueData = GetVenueData();
	ASSERT(pVenueData);
	std::string event = m_Event->GetName();
	CDlgConfigEvent dlg(pVenueData->GetVenue(), m_Event, m_pDlg);
	if (IDOK == dlg.DoModal())
	{
		if (m_Event->GetName() != event)
		{
			GetFixupList().push_back(new CDlgFixupRenameEvent(pVenueData->GetVenue()->GetName(), event, m_Event->GetName()));
			GetTree().SetItem(m_hItem,
				TVIF_TEXT,
				m_Event->GetName().c_str(),
				0, 0, 0, 0, 0);
		}
		// Delete all scoring methods and re-add them. If we don't do this,
		// then the tree doesn't show any changes we've made.
		HTREEITEM hScoring = GetTree().GetChildItem(m_hItem);
		HTREEITEM hItem;
		while (NULL != (hItem = GetTree().GetChildItem(hScoring)))
			GetTree().DeleteItem(hItem);
		InsertScorings(hScoring, m_Event);
		SetItemCurrent();
	}
}

void CDlgConfigureDataEvent::Delete()
{
	CDlgConfigureDataVenue* pVenueData = GetVenueData();
	ASSERT(pVenueData);
	bool bDelete = true;
	std::string event = m_Event->GetName();
	int nEvents = GetRecordBook().GetDogs().NumEventsInUse(pVenueData->GetVenue()->GetName(), event);
	if (0 < nEvents)
	{
		CString msg;
		msg.FormatMessage(IDS_DELETE_EVENT,
			event.c_str(),
			nEvents);
		if (IDYES != AfxMessageBox(msg, MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2))
			bDelete = false;
	}
	if (bDelete)
	{
		if (pVenueData->GetVenue()->GetEvents().DeleteEvent(event))
		{
			// Then we commit to fixing the real data.
			if (0 < nEvents)
				GetFixupList().push_back(new CDlgFixupDeleteEvent(pVenueData->GetVenue()->GetName(), event));
			GetTree().DeleteItem(m_hItem);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataVenue* CDlgConfigureDataScoring::GetVenueData()
{
	CDlgConfigureData* pData = GetParentData();
	ASSERT(pData->GetType() == eScoring);
	return pData->GetVenueData();
}

CDlgConfigureDataDivision* CDlgConfigureDataScoring::GetDivisionData()
{
	CDlgConfigureData* pData = GetParentData();
	ASSERT(pData->GetType() == eScoring);
	return pData->GetDivisionData();
}

CDlgConfigureDataEvent* CDlgConfigureDataScoring::GetEventData()
{
	CDlgConfigureData* pData = GetParentData();
	ASSERT(pData->GetType() == eScoring);
	return pData->GetEventData();
}

void CDlgConfigureDataScoring::Edit()
{
	GetEventData()->Edit();
}
