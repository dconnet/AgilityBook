/*
 * Copyright © 2004-2005 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgExistingPoints class
 * @author David Connet
 *
 * Revision History
 * @li 2005-07-31 DRC Changed how QQs are done.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-02-03 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgExistingPoints.h"

#include "AgilityBookDoc.h"
#include "ARBConfig.h"
#include "ARBDogExistingPoints.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

class CDlgPointsDataLevel
{
public:
	CDlgPointsDataLevel(ARBConfigLevel const* pLevel)
		: m_pLevel(pLevel)
		, m_pSubLevel(NULL)
	{
	}
	CDlgPointsDataLevel(
			ARBConfigLevel const* pLevel,
			ARBConfigSubLevel const* pSubLevel)
		: m_pLevel(pLevel)
		, m_pSubLevel(pSubLevel)
	{
	}
	ARBConfigLevel const* m_pLevel;
	ARBConfigSubLevel const* m_pSubLevel;
};

static void GetEnableLists(
		ARBDogExistingPoints::PointType inType,
		BOOL& outOther,
		BOOL& outVenue,
		BOOL& outDiv,
		BOOL& outLevel,
		BOOL& outEvent,
		BOOL& outSubName)
{
	outOther = FALSE;
	outVenue = FALSE;
	outDiv = FALSE;
	outLevel = FALSE;
	outEvent = FALSE;
	outSubName = FALSE;
	switch (inType)
	{
	default:
		ASSERT(0);
		break;
	case ARBDogExistingPoints::eOtherPoints:
		outOther = TRUE;
		outVenue = TRUE;
		outDiv = TRUE;
		outLevel = TRUE;
		outEvent = TRUE;
		break;
	case ARBDogExistingPoints::eRuns:
		outVenue = TRUE;
		outDiv = TRUE;
		outLevel = TRUE;
		outEvent = TRUE;
		outSubName = TRUE;
		break;
	case ARBDogExistingPoints::eSpeed:
		outVenue = TRUE;
		outDiv = TRUE;
		outLevel = TRUE;
		break;
	case ARBDogExistingPoints::eQQ:
		outVenue = TRUE;
		break;
	case ARBDogExistingPoints::eSQ:
		outVenue = TRUE;
		outDiv = TRUE;
		outLevel = TRUE;
		outEvent = TRUE;
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgExistingPoints dialog

// If pExistingPoints is NULL, we're creating a new entry. Otherwise, we're editing an existing.
CDlgExistingPoints::CDlgExistingPoints(
		CAgilityBookDoc* pDoc,
		ARBDogExistingPointsList& points,
		ARBDogExistingPoints* pExistingPoints,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgExistingPoints::IDD, pParent)
	, m_pDoc(pDoc)
	, m_PointsList(points)
	, m_pExistingPoints(pExistingPoints)
{
	//{{AFX_DATA_INIT(CDlgExistingPoints)
	m_Points = 0;
	m_Comments = _T("");
	//}}AFX_DATA_INIT
	if (m_pExistingPoints)
	{
		m_Date = m_pExistingPoints->GetDate();
		m_Points = m_pExistingPoints->GetPoints();
		m_Comments = m_pExistingPoints->GetComment().c_str();
		m_Comments.Replace("\n", "\r\n");
	}
}

void CDlgExistingPoints::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExistingPoints)
	DDX_Control(pDX, IDC_EXISTING_DATE, m_ctrlDate);
	DDX_Control(pDX, IDC_EXISTING_TYPE, m_ctrlType);
	DDX_Control(pDX, IDC_EXISTING_OTHERPOINTS, m_ctrlOther);
	DDX_Control(pDX, IDC_EXISTING_VENUES, m_ctrlVenues);
	DDX_Control(pDX, IDC_EXISTING_DIVISION, m_ctrlDivisions);
	DDX_Control(pDX, IDC_EXISTING_LEVEL, m_ctrlLevels);
	DDX_Control(pDX, IDC_EXISTING_EVENT, m_ctrlEvents);
	DDX_Control(pDX, IDC_EXISTING_SUBNAME, m_ctrlSubNames);
	DDX_CBString(pDX, IDC_EXISTING_SUBNAME, m_SubName);
	DDX_Text(pDX, IDC_EXISTING_POINTS, m_Points);
	DDX_Text(pDX, IDC_EXISTING_COMMENTS, m_Comments);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		CTime time;
		m_ctrlDate.GetTime(time);
		m_Date = ARBDate(time.GetYear(), time.GetMonth(), time.GetDay());
	}
}

BEGIN_MESSAGE_MAP(CDlgExistingPoints, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgExistingPoints)
	ON_CBN_SELCHANGE(IDC_EXISTING_TYPE, OnSelchangeType)
	ON_CBN_SELCHANGE(IDC_EXISTING_VENUES, OnSelchangeVenues)
	ON_CBN_SELCHANGE(IDC_EXISTING_DIVISION, OnSelchangeDivision)
	ON_CBN_SELCHANGE(IDC_EXISTING_LEVEL, OnSelchangeLevel)
	ON_CBN_SELCHANGE(IDC_EXISTING_EVENT, OnSelchangeEvent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgExistingPoints::ClearLevels()
{
	for (int index = m_ctrlLevels.GetCount() - 1; 0 <= index; --index)
	{
		CDlgPointsDataLevel* pData = reinterpret_cast<CDlgPointsDataLevel*>(m_ctrlLevels.GetItemDataPtr(index));
		delete pData;
		m_ctrlLevels.SetItemDataPtr(index, NULL);
	}
	m_ctrlLevels.ResetContent();
}

void CDlgExistingPoints::UpdateControls()
{
	int index = m_ctrlType.GetCurSel();
	BOOL bOther = FALSE;
	BOOL bVenue = FALSE;
	BOOL bDiv = FALSE;
	BOOL bLevel = FALSE;
	BOOL bEvent = FALSE;
	BOOL bSubName = FALSE;
	if (CB_ERR == index)
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		ARBDogExistingPoints::PointType type = static_cast<ARBDogExistingPoints::PointType>(m_ctrlType.GetItemData(index));
		GetEnableLists(type, bOther, bVenue, bDiv, bLevel, bEvent, bSubName);
	}
	m_ctrlOther.EnableWindow(bOther);
	m_ctrlVenues.EnableWindow(bVenue);
	m_ctrlDivisions.EnableWindow(bDiv);
	m_ctrlLevels.EnableWindow(bLevel);
	m_ctrlEvents.EnableWindow(bEvent);
	m_ctrlSubNames.EnableWindow(bSubName);
}

void CDlgExistingPoints::FillDivisions()
{
	CString str;
	std::string div;
	int index = m_ctrlDivisions.GetCurSel();
	if (CB_ERR != index)
	{
		m_ctrlDivisions.GetLBText(index, str);
		div = (LPCSTR)str;
	}
	else if (m_pExistingPoints)
		div = m_pExistingPoints->GetDivision();

	m_ctrlDivisions.ResetContent();
	int idxVenue = m_ctrlVenues.GetCurSel();
	if (CB_ERR != idxVenue)
	{
		ARBConfigVenue const* pVenue = reinterpret_cast<ARBConfigVenue const*>(m_ctrlVenues.GetItemDataPtr(idxVenue));
		for (ARBConfigDivisionList::const_iterator iterDiv = pVenue->GetDivisions().begin();
			iterDiv != pVenue->GetDivisions().end();
			++iterDiv)
		{
			ARBConfigDivision* pDiv = (*iterDiv);
			int index = m_ctrlDivisions.AddString(pDiv->GetName().c_str());
			m_ctrlDivisions.SetItemDataPtr(index, pDiv);
			if (m_pExistingPoints && div == pDiv->GetName())
				m_ctrlDivisions.SetCurSel(index);
		}
	}
	FillLevels();
}

void CDlgExistingPoints::FillLevels()
{
	CString str;
	std::string level;
	int index = m_ctrlLevels.GetCurSel();
	if (CB_ERR != index)
	{
		m_ctrlLevels.GetLBText(index, str);
		level = (LPCSTR)str;
	}
	else if (m_pExistingPoints)
		level = m_pExistingPoints->GetLevel();

	ClearLevels();
	int idxDiv = m_ctrlDivisions.GetCurSel();
	if (CB_ERR != idxDiv)
	{
		ARBConfigDivision const* pDiv = reinterpret_cast<ARBConfigDivision const*>(m_ctrlDivisions.GetItemDataPtr(idxDiv));
		for (ARBConfigLevelList::const_iterator iter = pDiv->GetLevels().begin();
			iter != pDiv->GetLevels().end();
			++iter)
		{
			ARBConfigLevel* pLevel = (*iter);
			if (0 < pLevel->GetSubLevels().size())
			{
				for (ARBConfigSubLevelList::const_iterator iterSub = pLevel->GetSubLevels().begin();
					iterSub != pLevel->GetSubLevels().end();
					++iterSub)
				{
					ARBConfigSubLevel* pSubLevel = (*iterSub);
					CDlgPointsDataLevel* pData = new CDlgPointsDataLevel(pLevel, pSubLevel);
					int index = m_ctrlLevels.AddString(pSubLevel->GetName().c_str());
					m_ctrlLevels.SetItemDataPtr(index, pData);
					if (level == pSubLevel->GetName())
						m_ctrlLevels.SetCurSel(index);
				}
			}
			else
			{
				CDlgPointsDataLevel *pData = new CDlgPointsDataLevel(pLevel);
				int index = m_ctrlLevels.AddString(pLevel->GetName().c_str());
				m_ctrlLevels.SetItemDataPtr(index, pData);
				if (level == pLevel->GetName())
					m_ctrlLevels.SetCurSel(index);
			}
		}
	}
	FillEvents();
}

void CDlgExistingPoints::FillEvents()
{
	CString str;
	std::string event;
	int index = m_ctrlEvents.GetCurSel();
	if (CB_ERR != index)
	{
		m_ctrlEvents.GetLBText(index, str);
		event = (LPCSTR)str;
	}
	else if (m_pExistingPoints)
		event = m_pExistingPoints->GetEvent();
	m_ctrlEvents.ResetContent();

	int idxVenue = m_ctrlVenues.GetCurSel();
	if (CB_ERR != idxVenue)
	{
		ARBConfigVenue const* pVenue = reinterpret_cast<ARBConfigVenue const*>(m_ctrlVenues.GetItemDataPtr(idxVenue));
		int idxDiv = m_ctrlDivisions.GetCurSel();
		if (CB_ERR != idxDiv)
		{
			ARBConfigDivision const* pDiv = reinterpret_cast<ARBConfigDivision const*>(m_ctrlDivisions.GetItemDataPtr(idxDiv));
			int idxLevel = m_ctrlLevels.GetCurSel();
			if (CB_ERR != idxLevel)
			{
				CDlgPointsDataLevel const* pData = reinterpret_cast<CDlgPointsDataLevel const*>(m_ctrlLevels.GetItemDataPtr(idxLevel));
				for (ARBConfigEventList::const_iterator iter = pVenue->GetEvents().begin();
					iter != pVenue->GetEvents().end();
					++iter)
				{
					ARBConfigEvent* pEvent = (*iter);
					if (pEvent->FindEvent(pDiv->GetName(), pData->m_pLevel->GetName(), m_Date))
					{
						int index = m_ctrlEvents.AddString(pEvent->GetName().c_str());
						m_ctrlEvents.SetItemDataPtr(index, pEvent);
						if (event == pEvent->GetName())
							m_ctrlEvents.SetCurSel(index);
					}
				}
			}
		}
	}
	FillSubNames();
	UpdateControls();
}

void CDlgExistingPoints::FillSubNames()
{
	m_ctrlSubNames.ResetContent();

	int index = m_ctrlVenues.GetCurSel();
	if (CB_ERR == index)
		return;
	ARBConfigVenue const* pVenue = reinterpret_cast<ARBConfigVenue const*>(m_ctrlVenues.GetItemDataPtr(index));

	index = m_ctrlEvents.GetCurSel();
	if (CB_ERR == index)
		return;
	ARBConfigEvent const* pEvent = reinterpret_cast<ARBConfigEvent const*>(m_ctrlEvents.GetItemDataPtr(index));

	std::set<std::string> names;
	m_pDoc->GetAllEventSubNames(pVenue->GetName(), pEvent, names);
	for (std::set<std::string>::const_iterator iter = names.begin();
		iter != names.end();
		++iter)
	{
		index = m_ctrlSubNames.AddString(iter->c_str());
		if (m_pExistingPoints && *iter == m_pExistingPoints->GetSubName())
			m_ctrlSubNames.SetCurSel(index);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgExistingPoints message handlers

BOOL CDlgExistingPoints::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();

	ARBDogExistingPoints::PointType types[] =
	{
		ARBDogExistingPoints::eOtherPoints,
		ARBDogExistingPoints::eRuns,
		ARBDogExistingPoints::eSpeed,
		ARBDogExistingPoints::eQQ,
		ARBDogExistingPoints::eSQ
	};
	int nTypes = sizeof(types) / sizeof(types[0]);
	for (int i = 0; i < nTypes; ++i)
	{
		int index = m_ctrlType.AddString(ARBDogExistingPoints::GetPointTypeName(types[i]).c_str());
		m_ctrlType.SetItemData(index, types[i]);
	}

	for (ARBConfigOtherPointsList::const_iterator iterOther = m_pDoc->GetConfig().GetOtherPoints().begin();
		iterOther != m_pDoc->GetConfig().GetOtherPoints().end();
		++iterOther)
	{
		ARBConfigOtherPoints* pOther = (*iterOther);
		int index = m_ctrlOther.AddString(pOther->GetName().c_str());
		m_ctrlOther.SetItemDataPtr(index, pOther);
	}

	for (ARBConfigVenueList::const_iterator iterVenue = m_pDoc->GetConfig().GetVenues().begin();
		iterVenue != m_pDoc->GetConfig().GetVenues().end();
		++iterVenue)
	{
		ARBConfigVenue* pVenue = (*iterVenue);
		int index = m_ctrlVenues.AddString(pVenue->GetName().c_str());
		m_ctrlVenues.SetItemDataPtr(index, pVenue);
	}

	if (m_pExistingPoints)
	{
		if (m_Date.IsValid())
		{
			CTime t(m_Date.GetYear(), m_Date.GetMonth(), m_Date.GetDay(), 0, 0, 0);
			m_ctrlDate.SetTime(&t);
		}
		int index = m_ctrlType.FindStringExact(0, ARBDogExistingPoints::GetPointTypeName(m_pExistingPoints->GetType()).c_str());
		if (0 <= index)
			m_ctrlType.SetCurSel(index);
		index = m_ctrlOther.FindStringExact(0, m_pExistingPoints->GetOtherPoints().c_str());
		if (0 <= index)
			m_ctrlOther.SetCurSel(index);
		index = m_ctrlVenues.FindStringExact(0, m_pExistingPoints->GetVenue().c_str());
		if (0 <= index)
			m_ctrlVenues.SetCurSel(index);
	}

	FillDivisions();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgExistingPoints::OnSelchangeType() 
{
	UpdateControls();
}

void CDlgExistingPoints::OnSelchangeVenues() 
{
	FillDivisions();
}

void CDlgExistingPoints::OnSelchangeDivision() 
{
	FillLevels();
}

void CDlgExistingPoints::OnSelchangeLevel() 
{
	FillEvents();
}

void CDlgExistingPoints::OnSelchangeEvent() 
{
	FillSubNames();
}

void CDlgExistingPoints::OnOK()
{
	if (!UpdateData(TRUE))
		return;

	int index = m_ctrlType.GetCurSel();
	if (CB_ERR == index)
	{
		GotoDlgCtrl(&m_ctrlType);
		return;
	}
	ARBDogExistingPoints::PointType type = static_cast<ARBDogExistingPoints::PointType>(m_ctrlType.GetItemData(index));

	BOOL bOther, bVenue, bDiv, bLevel, bEvent, bSubName;
	GetEnableLists(type, bOther, bVenue, bDiv, bLevel, bEvent, bSubName);

	std::string other;
	if (bOther)
	{
		index = m_ctrlOther.GetCurSel();
		if (CB_ERR == index)
		{
			GotoDlgCtrl(&m_ctrlOther);
			return;
		}
		ARBConfigOtherPoints const* pOther = reinterpret_cast<ARBConfigOtherPoints const*>(m_ctrlOther.GetItemDataPtr(index));
		other = pOther->GetName();
	}

	std::string venue;
	if (bVenue)
	{
		index = m_ctrlVenues.GetCurSel();
		if (CB_ERR == index)
		{
			GotoDlgCtrl(&m_ctrlVenues);
			return;
		}
		ARBConfigVenue const* pVenue = reinterpret_cast<ARBConfigVenue const*>(m_ctrlVenues.GetItemDataPtr(index));
		venue = pVenue->GetName();
	}

	std::string div;
	if (bDiv)
	{
		index = m_ctrlDivisions.GetCurSel();
		if (CB_ERR == index)
		{
			GotoDlgCtrl(&m_ctrlDivisions);
			return;
		}
		ARBConfigDivision const* pDiv = reinterpret_cast<ARBConfigDivision const*>(m_ctrlDivisions.GetItemDataPtr(index));
		div = pDiv->GetName();
	}

	std::string level;
	if (bLevel)
	{
		index = m_ctrlLevels.GetCurSel();
		if (CB_ERR == index)
		{
			GotoDlgCtrl(&m_ctrlLevels);
			return;
		}
		CDlgPointsDataLevel const* pData = reinterpret_cast<CDlgPointsDataLevel const*>(m_ctrlLevels.GetItemDataPtr(index));
		level = pData->m_pLevel->GetName();
		if (pData->m_pSubLevel)
			level = pData->m_pSubLevel->GetName();
	}

	std::string event;
	if (bEvent)
	{
		index = m_ctrlEvents.GetCurSel();
		if (CB_ERR == index)
		{
			GotoDlgCtrl(&m_ctrlEvents);
			return;
		}
		ARBConfigEvent const* pEvent = reinterpret_cast<ARBConfigEvent const*>(m_ctrlEvents.GetItemDataPtr(index));
		event = pEvent->GetName();
	}

	std::string subName;
	if (bSubName)
	{
		m_SubName.TrimRight();
		m_SubName.TrimLeft();
		subName = (LPCTSTR)m_SubName;
	}

	m_Comments.Replace("\r\n", "\n");

	if (m_pExistingPoints)
	{
		m_pExistingPoints->SetDate(m_Date);
		m_pExistingPoints->SetType(type);
		m_pExistingPoints->SetOtherPoints(other);
		m_pExistingPoints->SetVenue(venue);
		m_pExistingPoints->SetDivision(div);
		m_pExistingPoints->SetLevel(level);
		m_pExistingPoints->SetEvent(event);
		m_pExistingPoints->SetSubName(subName);
		m_pExistingPoints->SetComment((LPCSTR)m_Comments);
		m_pExistingPoints->SetPoints(m_Points);
	}
	else
	{
		ARBDogExistingPoints* pPoints = new ARBDogExistingPoints();
		if (m_PointsList.AddExistingPoints(pPoints))
		{
			pPoints->SetDate(m_Date);
			pPoints->SetType(type);
			pPoints->SetOtherPoints(other);
			pPoints->SetVenue(venue);
			pPoints->SetDivision(div);
			pPoints->SetLevel(level);
			pPoints->SetEvent(event);
			pPoints->SetSubName(subName);
			pPoints->SetComment((LPCSTR)m_Comments);
			pPoints->SetPoints(m_Points);
		}
		pPoints->Release();
	}

	ClearLevels();
	CDlgBaseDialog::OnOK();
}

void CDlgExistingPoints::OnCancel()
{
	ClearLevels();
	CDlgBaseDialog::OnCancel();
}
