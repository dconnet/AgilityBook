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
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-02-03 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgExistingPoints.h"

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
	CDlgPointsDataLevel(ARBConfigLevel const* pLevel, ARBConfigSubLevel const* pSubLevel)
		: m_pLevel(pLevel)
		, m_pSubLevel(pSubLevel)
	{
	}
	ARBConfigLevel const* m_pLevel;
	ARBConfigSubLevel const* m_pSubLevel;
};

/////////////////////////////////////////////////////////////////////////////
// CDlgExistingPoints dialog

// If pExistingPoints is NULL, we're creating a new entry. Otherwise, we're editing an existing.
CDlgExistingPoints::CDlgExistingPoints(ARBConfig const& config,
	ARBDogExistingPointsList& points,
	ARBDogExistingPoints* pExistingPoints, CWnd* pParent)
	: CDlgBaseDialog(CDlgExistingPoints::IDD, pParent)
	, m_Config(config)
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
	BOOL bEnableOther = FALSE;
	BOOL bEnableEvent = FALSE;
	if (CB_ERR == index)
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
	else
	{
		bEnableOther = TRUE;
		bEnableEvent = TRUE;
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		ARBDogExistingPoints::PointType type = static_cast<ARBDogExistingPoints::PointType>(m_ctrlType.GetItemData(index));
		if (ARBDogExistingPoints::eSpeed == type
		|| ARBDogExistingPoints::eQQ == type)
		{
			bEnableEvent = FALSE;
		}
		if (ARBDogExistingPoints::eOtherPoints != type)
			bEnableOther = FALSE;
	}
	m_ctrlOther.EnableWindow(bEnableOther);
	m_ctrlEvents.EnableWindow(bEnableEvent);
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
	UpdateControls();
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

	for (ARBConfigOtherPointsList::const_iterator iterOther = m_Config.GetOtherPoints().begin();
		iterOther != m_Config.GetOtherPoints().end();
		++iterOther)
	{
		ARBConfigOtherPoints* pOther = (*iterOther);
		int index = m_ctrlOther.AddString(pOther->GetName().c_str());
		m_ctrlOther.SetItemDataPtr(index, pOther);
	}

	for (ARBConfigVenueList::const_iterator iterVenue = m_Config.GetVenues().begin();
		iterVenue != m_Config.GetVenues().end();
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

	index = m_ctrlOther.GetCurSel();
	std::string other;
	if (CB_ERR != index)
	{
		ARBConfigOtherPoints const* pOther = reinterpret_cast<ARBConfigOtherPoints const*>(m_ctrlOther.GetItemDataPtr(index));
		other = pOther->GetName();
	}

	index = m_ctrlVenues.GetCurSel();
	if (CB_ERR == index)
	{
		GotoDlgCtrl(&m_ctrlVenues);
		return;
	}
	ARBConfigVenue const* pVenue = reinterpret_cast<ARBConfigVenue const*>(m_ctrlVenues.GetItemDataPtr(index));
	std::string venue = pVenue->GetName();

	index = m_ctrlDivisions.GetCurSel();
	if (CB_ERR == index)
	{
		GotoDlgCtrl(&m_ctrlDivisions);
		return;
	}
	ARBConfigDivision const* pDiv = reinterpret_cast<ARBConfigDivision const*>(m_ctrlDivisions.GetItemDataPtr(index));
	std::string div = pDiv->GetName();

	index = m_ctrlLevels.GetCurSel();
	if (CB_ERR == index)
	{
		GotoDlgCtrl(&m_ctrlLevels);
		return;
	}
	CDlgPointsDataLevel const* pData = reinterpret_cast<CDlgPointsDataLevel const*>(m_ctrlLevels.GetItemDataPtr(index));
	std::string level = pData->m_pLevel->GetName();
	if (pData->m_pSubLevel)
		level = pData->m_pSubLevel->GetName();

	index = m_ctrlEvents.GetCurSel();
	std::string event;
	if (CB_ERR != index)
	{
		ARBConfigEvent const* pEvent = reinterpret_cast<ARBConfigEvent const*>(m_ctrlEvents.GetItemDataPtr(index));
		event = pEvent->GetName();
	}

	m_Comments.Replace("\r\n", "\n");

	if (ARBDogExistingPoints::eSpeed == type
	|| ARBDogExistingPoints::eQQ == type)
	{
		event.erase();
	}
	if (ARBDogExistingPoints::eOtherPoints != type)
		other.erase();

	if (m_pExistingPoints)
	{
		m_pExistingPoints->SetDate(m_Date);
		m_pExistingPoints->SetType(type);
		m_pExistingPoints->SetOtherPoints(other);
		m_pExistingPoints->SetVenue(venue);
		m_pExistingPoints->SetDivision(div);
		m_pExistingPoints->SetLevel(level);
		m_pExistingPoints->SetEvent(event);
		m_pExistingPoints->SetComment((LPCSTR)m_Comments);
		m_pExistingPoints->SetPoints(m_Points);
	}
	else
	{
		ARBDogExistingPoints* pPoints = new ARBDogExistingPoints();
		ARBDogExistingPoints* pNewPoints = m_PointsList.AddExistingPoints(pPoints);
		pPoints->Release();
		pNewPoints->SetDate(m_Date);
		pNewPoints->SetType(type);
		pNewPoints->SetOtherPoints(other);
		pNewPoints->SetVenue(venue);
		pNewPoints->SetDivision(div);
		pNewPoints->SetLevel(level);
		pNewPoints->SetEvent(event);
		pNewPoints->SetComment((LPCSTR)m_Comments);
		pNewPoints->SetPoints(m_Points);
	}

	ClearLevels();
	CDlgBaseDialog::OnOK();
}

void CDlgExistingPoints::OnCancel()
{
	ClearLevels();
	CDlgBaseDialog::OnCancel();
}
