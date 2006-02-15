/*
 * Copyright © 2004-2006 David Connet. All Rights Reserved.
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
#include "ListData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

class CDlgPointsDataLevel
{
public:
	CDlgPointsDataLevel(ARBConfigLevelPtr pLevel)
		: m_pLevel(pLevel)
		, m_pSubLevel()
	{
	}
	CDlgPointsDataLevel(
			ARBConfigLevelPtr pLevel,
			ARBConfigSubLevelPtr pSubLevel)
		: m_pLevel(pLevel)
		, m_pSubLevel(pSubLevel)
	{
	}
	ARBConfigLevelPtr m_pLevel;
	ARBConfigSubLevelPtr m_pSubLevel;
};

/////////////////////////////////////////////////////////////////////////////
// CDlgExistingPoints dialog

// If pExistingPoints is NULL, we're creating a new entry. Otherwise, we're editing an existing.
CDlgExistingPoints::CDlgExistingPoints(
		CAgilityBookDoc* pDoc,
		ARBDogExistingPointsList& points,
		ARBDogExistingPointsPtr pExistingPoints,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgExistingPoints::IDD, pParent)
	, m_ctrlType(false)
	, m_ctrlOther(true)
	, m_ctrlMultiQ(true)
	, m_ctrlVenues(true)
	, m_ctrlDivisions(true)
	, m_ctrlLevels(true)
	, m_ctrlEvents(true)
	, m_ctrlSubNames(false)
	, m_pDoc(pDoc)
	, m_PointsList(points)
	, m_pExistingPoints(pExistingPoints)
{
	//{{AFX_DATA_INIT(CDlgExistingPoints)
	m_Points = 0.0;
	m_Comments = _T("");
	//}}AFX_DATA_INIT
	if (m_pExistingPoints)
	{
		m_Date = m_pExistingPoints->GetDate();
		m_Points = m_pExistingPoints->GetPoints();
		m_Comments = m_pExistingPoints->GetComment().c_str();
		m_Comments.Replace(_T("\n"), _T("\r\n"));
	}
}

void CDlgExistingPoints::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExistingPoints)
	DDX_Control(pDX, IDC_EXISTING_DATE, m_ctrlDate);
	DDX_Control(pDX, IDC_EXISTING_TYPE, m_ctrlType);
	DDX_Control(pDX, IDC_EXISTING_OTHERPOINTS_TEXT, m_ctrlOtherText);
	DDX_Control(pDX, IDC_EXISTING_OTHERPOINTS, m_ctrlOther);
	DDX_Control(pDX, IDC_EXISTING_MULTIQ_TEXT, m_ctrlMultiQText);
	DDX_Control(pDX, IDC_EXISTING_MULTIQ, m_ctrlMultiQ);
	DDX_Control(pDX, IDC_EXISTING_VENUES_TEXT, m_ctrlVenuesText);
	DDX_Control(pDX, IDC_EXISTING_VENUES, m_ctrlVenues);
	DDX_Control(pDX, IDC_EXISTING_DIVISION_TEXT, m_ctrlDivisionsText);
	DDX_Control(pDX, IDC_EXISTING_DIVISION, m_ctrlDivisions);
	DDX_Control(pDX, IDC_EXISTING_LEVEL_TEXT, m_ctrlLevelsText);
	DDX_Control(pDX, IDC_EXISTING_LEVEL, m_ctrlLevels);
	DDX_Control(pDX, IDC_EXISTING_EVENT_TEXT, m_ctrlEventsText);
	DDX_Control(pDX, IDC_EXISTING_EVENT, m_ctrlEvents);
	DDX_Control(pDX, IDC_EXISTING_SUBNAME_TEXT, m_ctrlSubNamesText);
	DDX_Control(pDX, IDC_EXISTING_SUBNAME, m_ctrlSubNames);
	DDX_CBString(pDX, IDC_EXISTING_SUBNAME, m_SubName);
	DDX_Text(pDX, IDC_EXISTING_POINTS, m_Points);
	DDX_Text(pDX, IDC_EXISTING_COMMENTS, m_Comments);
	DDX_Control(pDX, IDOK, m_ctrlOk);
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

CListPtrData<ARBConfigVenuePtr>* CDlgExistingPoints::GetVenueData(int index) const
{
	CListData* pData = m_ctrlVenues.GetData(index);
	return dynamic_cast<CListPtrData<ARBConfigVenuePtr>*>(pData);
}

CListPtrData<ARBConfigDivisionPtr>* CDlgExistingPoints::GetDivisionData(int index) const
{
	CListData* pData = m_ctrlDivisions.GetData(index);
	return dynamic_cast<CListPtrData<ARBConfigDivisionPtr>*>(pData);
}

CListPtrData<ARBConfigEventPtr>* CDlgExistingPoints::GetEventData(int index) const
{
	CListData* pData = m_ctrlEvents.GetData(index);
	return dynamic_cast<CListPtrData<ARBConfigEventPtr>*>(pData);
}

CDlgPointsDataLevel* CDlgExistingPoints::GetLevelData(int index) const
{
	CListData* pData = m_ctrlLevels.GetData(index);
	return dynamic_cast<CDlgPointsDataLevel*>(pData);
}

CListPtrData<ARBConfigOtherPointsPtr>* CDlgExistingPoints::GetOtherPointData(int index) const
{
	CListData* pData = m_ctrlOther.GetData(index);
	return dynamic_cast<CListPtrData<ARBConfigOtherPointsPtr>*>(pData);
}

CListPtrData<ARBConfigMultiQPtr>* CDlgExistingPoints::GetMultiQData(int index) const
{
	CListData* pData = m_ctrlMultiQ.GetData(index);
	return dynamic_cast<CListPtrData<ARBConfigMultiQPtr>*>(pData);
}

// Control order
//  OtherPoints
//  Venue       MultiQ
//  Div
//  Level
//  Event       SubName
// Other, MultiQ: Never moves, all others do.

void CDlgExistingPoints::GetEnableLists(
		int index,
		BOOL& outOther,
		BOOL& outMQ,
		BOOL& outVenue,
		BOOL& outDiv,
		BOOL& outLevel,
		BOOL& outEvent,
		BOOL& outSubName,
		bool bSet)
{
	outOther = FALSE;
	outMQ = FALSE;
	outVenue = FALSE;
	outDiv = FALSE;
	outLevel = FALSE;
	outEvent = FALSE;
	outSubName = FALSE;

	if (0 <= index)
	{
		ARBDogExistingPoints::PointType type = static_cast<ARBDogExistingPoints::PointType>(m_ctrlType.GetItemData(index));
		switch (type)
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
			if (bSet)
			{
				// other points is at 0
				m_ctrlVenuesText.SetWindowPos(NULL,
					m_ptControls[0][1].x, m_ptControls[0][1].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlVenues.SetWindowPos(NULL,
					m_ptControls[1][1].x, m_ptControls[1][1].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlDivisionsText.SetWindowPos(NULL,
					m_ptControls[0][2].x, m_ptControls[0][2].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlDivisions.SetWindowPos(NULL,
					m_ptControls[1][2].x, m_ptControls[1][2].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlLevelsText.SetWindowPos(NULL,
					m_ptControls[0][3].x, m_ptControls[0][3].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlLevels.SetWindowPos(NULL,
					m_ptControls[1][3].x, m_ptControls[1][3].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlEventsText.SetWindowPos(NULL,
					m_ptControls[0][4].x, m_ptControls[0][4].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlEvents.SetWindowPos(NULL,
					m_ptControls[1][4].x, m_ptControls[1][4].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
			break;

		case ARBDogExistingPoints::eRuns:
			outVenue = TRUE;
			outDiv = TRUE;
			outLevel = TRUE;
			outEvent = TRUE;
			outSubName = TRUE;
			if (bSet)
			{
				m_ctrlVenuesText.SetWindowPos(NULL,
					m_ptControls[0][0].x, m_ptControls[0][0].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlVenues.SetWindowPos(NULL,
					m_ptControls[1][0].x, m_ptControls[1][0].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlDivisionsText.SetWindowPos(NULL,
					m_ptControls[0][1].x, m_ptControls[0][1].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlDivisions.SetWindowPos(NULL,
					m_ptControls[1][1].x, m_ptControls[1][1].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlLevelsText.SetWindowPos(NULL,
					m_ptControls[0][2].x, m_ptControls[0][2].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlLevels.SetWindowPos(NULL,
					m_ptControls[1][2].x, m_ptControls[1][2].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlEventsText.SetWindowPos(NULL,
					m_ptControls[0][3].x, m_ptControls[0][3].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlEvents.SetWindowPos(NULL,
					m_ptControls[1][3].x, m_ptControls[1][3].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlSubNamesText.SetWindowPos(NULL,
					m_ptControls[0][4].x, m_ptControls[0][4].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlSubNames.SetWindowPos(NULL,
					m_ptControls[1][4].x, m_ptControls[1][4].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
			break;

		case ARBDogExistingPoints::eSpeed:
			outVenue = TRUE;
			outDiv = TRUE;
			outLevel = TRUE;
			if (bSet)
			{
				m_ctrlVenuesText.SetWindowPos(NULL,
					m_ptControls[0][0].x, m_ptControls[0][0].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlVenues.SetWindowPos(NULL,
					m_ptControls[1][0].x, m_ptControls[1][0].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlDivisionsText.SetWindowPos(NULL,
					m_ptControls[0][1].x, m_ptControls[0][1].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlDivisions.SetWindowPos(NULL,
					m_ptControls[1][1].x, m_ptControls[1][1].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlLevelsText.SetWindowPos(NULL,
					m_ptControls[0][2].x, m_ptControls[0][2].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlLevels.SetWindowPos(NULL,
					m_ptControls[1][2].x, m_ptControls[1][2].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
			break;

		case ARBDogExistingPoints::eMQ:
			outVenue = TRUE;
			outMQ = TRUE;
			if (bSet)
			{
				m_ctrlVenuesText.SetWindowPos(NULL,
					m_ptControls[0][0].x, m_ptControls[0][0].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlVenues.SetWindowPos(NULL,
					m_ptControls[1][0].x, m_ptControls[1][0].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				// multiq is at 1
			}
			break;

		case ARBDogExistingPoints::eSQ:
			outVenue = TRUE;
			outDiv = TRUE;
			outLevel = TRUE;
			outEvent = TRUE;
			if (bSet)
			{
				m_ctrlVenuesText.SetWindowPos(NULL,
					m_ptControls[0][0].x, m_ptControls[0][0].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlVenues.SetWindowPos(NULL,
					m_ptControls[1][0].x, m_ptControls[1][0].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlDivisionsText.SetWindowPos(NULL,
					m_ptControls[0][1].x, m_ptControls[0][1].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlDivisions.SetWindowPos(NULL,
					m_ptControls[1][1].x, m_ptControls[1][1].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlLevelsText.SetWindowPos(NULL,
					m_ptControls[0][2].x, m_ptControls[0][2].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlLevels.SetWindowPos(NULL,
					m_ptControls[1][2].x, m_ptControls[1][2].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlEventsText.SetWindowPos(NULL,
					m_ptControls[0][3].x, m_ptControls[0][3].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
				m_ctrlEvents.SetWindowPos(NULL,
					m_ptControls[1][3].x, m_ptControls[1][3].y,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
			break;
		}
	}

	if (bSet)
	{
		m_ctrlOtherText.ShowWindow(outOther ? SW_SHOW : SW_HIDE);
		m_ctrlOther.ShowWindow(outOther ? SW_SHOW : SW_HIDE);
		m_ctrlMultiQText.ShowWindow(outMQ ? SW_SHOW : SW_HIDE);
		m_ctrlMultiQ.ShowWindow(outMQ ? SW_SHOW : SW_HIDE);
		m_ctrlVenuesText.ShowWindow(outVenue ? SW_SHOW : SW_HIDE);
		m_ctrlVenues.ShowWindow(outVenue ? SW_SHOW : SW_HIDE);
		m_ctrlDivisionsText.ShowWindow(outDiv ? SW_SHOW : SW_HIDE);
		m_ctrlDivisions.ShowWindow(outDiv ? SW_SHOW : SW_HIDE);
		m_ctrlLevelsText.ShowWindow(outLevel ? SW_SHOW : SW_HIDE);
		m_ctrlLevels.ShowWindow(outLevel ? SW_SHOW : SW_HIDE);
		m_ctrlEventsText.ShowWindow(outEvent ? SW_SHOW : SW_HIDE);
		m_ctrlEvents.ShowWindow(outEvent ? SW_SHOW : SW_HIDE);
		m_ctrlSubNamesText.ShowWindow(outSubName ? SW_SHOW : SW_HIDE);
		m_ctrlSubNames.ShowWindow(outSubName ? SW_SHOW : SW_HIDE);
	}
}

void CDlgExistingPoints::UpdateControls()
{
	int index = m_ctrlType.GetCurSel();
	BOOL bOther = FALSE;
	BOOL bMQ = FALSE;
	BOOL bVenue = FALSE;
	BOOL bDiv = FALSE;
	BOOL bLevel = FALSE;
	BOOL bEvent = FALSE;
	BOOL bSubName = FALSE;
	BOOL bOk = FALSE;
	if (CB_ERR != index)
		bOk = TRUE;
	GetEnableLists(index, bOther, bMQ, bVenue, bDiv, bLevel, bEvent, bSubName, true);
	m_ctrlOk.EnableWindow(bOk);
}

void CDlgExistingPoints::FillVenues()
{
	ARBString venue;
	int index = m_ctrlVenues.GetCurSel();
	if (CB_ERR != index)
	{
		CString str;
		m_ctrlVenues.GetLBText(index, str);
		venue = (LPCTSTR)str;
	}
	else if (m_pExistingPoints)
		venue = m_pExistingPoints->GetVenue();
	m_ctrlVenues.ResetContent();

	ARBDogExistingPoints::PointType type = ARBDogExistingPoints::eUnknown;
	index = m_ctrlType.GetCurSel();
	if (CB_ERR != index)
		type = static_cast<ARBDogExistingPoints::PointType>(m_ctrlType.GetItemData(index));

	for (ARBConfigVenueList::const_iterator iterVenue = m_pDoc->GetConfig().GetVenues().begin();
		iterVenue != m_pDoc->GetConfig().GetVenues().end();
		++iterVenue)
	{
		ARBConfigVenuePtr pVenue = (*iterVenue);
		if (ARBDogExistingPoints::eMQ != type || 0 < pVenue->GetMultiQs().size())
		{
			int index = m_ctrlVenues.AddString(pVenue->GetName().c_str());
			m_ctrlVenues.SetData(index,
				new CListPtrData<ARBConfigVenuePtr>(pVenue));
			if (m_pExistingPoints && venue == pVenue->GetName())
				m_ctrlVenues.SetCurSel(index);
		}
	}
	FillFromVenue();
}

void CDlgExistingPoints::FillFromVenue()
{
	ARBString multiQ;
	int index = m_ctrlMultiQ.GetCurSel();
	if (CB_ERR != index)
	{
		CString str;
		m_ctrlMultiQ.GetLBText(index, str);
		multiQ = (LPCTSTR)str;
	}
	else if (m_pExistingPoints)
		multiQ = m_pExistingPoints->GetMultiQ();
	m_ctrlMultiQ.ResetContent();

	ARBString div;
	index = m_ctrlDivisions.GetCurSel();
	if (CB_ERR != index)
	{
		CString str;
		m_ctrlDivisions.GetLBText(index, str);
		div = (LPCTSTR)str;
	}
	else if (m_pExistingPoints)
		div = m_pExistingPoints->GetDivision();
	m_ctrlDivisions.ResetContent();

	int idxVenue = m_ctrlVenues.GetCurSel();
	if (CB_ERR != idxVenue)
	{
		ARBConfigVenuePtr pVenue = GetVenueData(idxVenue)->GetData();

		for (ARBConfigMultiQList::const_iterator iterQ = pVenue->GetMultiQs().begin();
			iterQ != pVenue->GetMultiQs().end();
			++iterQ)
		{
			ARBConfigMultiQPtr pMulti = *iterQ;
			int index = m_ctrlMultiQ.AddString(pMulti->GetName().c_str());
			m_ctrlMultiQ.SetItemDataPtr(index,
				new CListPtrData<ARBConfigMultiQPtr>(pMulti));
			if (m_pExistingPoints && multiQ == pMulti->GetName())
				m_ctrlMultiQ.SetCurSel(index);
		}

		for (ARBConfigDivisionList::const_iterator iterDiv = pVenue->GetDivisions().begin();
			iterDiv != pVenue->GetDivisions().end();
			++iterDiv)
		{
			ARBConfigDivisionPtr pDiv = (*iterDiv);
			int index = m_ctrlDivisions.AddString(pDiv->GetName().c_str());
			m_ctrlDivisions.SetItemDataPtr(index,
				new CListPtrData<ARBConfigDivisionPtr>(pDiv));
			if (m_pExistingPoints && div == pDiv->GetName())
				m_ctrlDivisions.SetCurSel(index);
		}
	}
	FillLevels();
}

void CDlgExistingPoints::FillLevels()
{
	CString str;
	ARBString level;
	int index = m_ctrlLevels.GetCurSel();
	if (CB_ERR != index)
	{
		m_ctrlLevels.GetLBText(index, str);
		level = (LPCTSTR)str;
	}
	else if (m_pExistingPoints)
		level = m_pExistingPoints->GetLevel();

	m_ctrlLevels.ResetContent();
	int idxDiv = m_ctrlDivisions.GetCurSel();
	if (CB_ERR != idxDiv)
	{
		ARBConfigDivisionPtr pDiv = GetDivisionData(idxDiv)->GetData();
		for (ARBConfigLevelList::const_iterator iter = pDiv->GetLevels().begin();
			iter != pDiv->GetLevels().end();
			++iter)
		{
			ARBConfigLevelPtr pLevel = (*iter);
			if (0 < pLevel->GetSubLevels().size())
			{
				for (ARBConfigSubLevelList::const_iterator iterSub = pLevel->GetSubLevels().begin();
					iterSub != pLevel->GetSubLevels().end();
					++iterSub)
				{
					ARBConfigSubLevelPtr pSubLevel = (*iterSub);
					CDlgPointsDataLevel* pData = new CDlgPointsDataLevel(pLevel, pSubLevel);
					int index = m_ctrlLevels.AddString(pSubLevel->GetName().c_str());
					m_ctrlLevels.SetItemDataPtr(index, pData);
					if (level == pSubLevel->GetName())
						m_ctrlLevels.SetCurSel(index);
				}
			}
			else
			{
				CDlgPointsDataLevel* pData = new CDlgPointsDataLevel(pLevel);
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
	ARBString evt;
	int index = m_ctrlEvents.GetCurSel();
	if (CB_ERR != index)
	{
		m_ctrlEvents.GetLBText(index, str);
		evt = (LPCTSTR)str;
	}
	else if (m_pExistingPoints)
		evt = m_pExistingPoints->GetEvent();
	m_ctrlEvents.ResetContent();

	int idxVenue = m_ctrlVenues.GetCurSel();
	if (CB_ERR != idxVenue)
	{
		ARBConfigVenuePtr pVenue = GetVenueData(idxVenue)->GetData();
		int idxDiv = m_ctrlDivisions.GetCurSel();
		if (CB_ERR != idxDiv)
		{
			ARBConfigDivisionPtr pDiv = GetDivisionData(idxDiv)->GetData();
			int idxLevel = m_ctrlLevels.GetCurSel();
			if (CB_ERR != idxLevel)
			{
				CDlgPointsDataLevel* pData = GetLevelData(idxLevel);
				for (ARBConfigEventList::const_iterator iter = pVenue->GetEvents().begin();
					iter != pVenue->GetEvents().end();
					++iter)
				{
					ARBConfigEventPtr pEvent = (*iter);
					if (pEvent->FindEvent(pDiv->GetName(), pData->m_pLevel->GetName(), m_Date))
					{
						int index = m_ctrlEvents.AddString(pEvent->GetName().c_str());
						m_ctrlEvents.SetItemDataPtr(index,
							new CListPtrData<ARBConfigEventPtr>(pEvent));
						if (evt == pEvent->GetName())
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
	ARBConfigVenuePtr pVenue = GetVenueData(index)->GetData();

	index = m_ctrlEvents.GetCurSel();
	if (CB_ERR == index)
		return;
	ARBConfigEventPtr pEvent = GetEventData(index)->GetData();

	std::set<ARBString> names;
	m_pDoc->GetAllEventSubNames(pVenue->GetName(), pEvent, names);
	for (std::set<ARBString>::const_iterator iter = names.begin();
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

	// We _know_ which controls are where. In the future,
	// we could make this intelligent to just scan all the
	// controls and give back the 5 unique ones.
	CRect rect;
	m_ctrlOtherText.GetWindowRect(rect);
		m_ptControls[0][0] = rect.TopLeft();
		ScreenToClient(&m_ptControls[0][0]);
	m_ctrlOther.GetWindowRect(rect);
		m_ptControls[1][0] = rect.TopLeft();
		ScreenToClient(&m_ptControls[1][0]);
	m_ctrlVenuesText.GetWindowRect(rect);
		m_ptControls[0][1] = rect.TopLeft();
		ScreenToClient(&m_ptControls[0][1]);
	m_ctrlVenues.GetWindowRect(rect);
		m_ptControls[1][1] = rect.TopLeft();
		ScreenToClient(&m_ptControls[1][1]);
	m_ctrlDivisionsText.GetWindowRect(rect);
		m_ptControls[0][2] = rect.TopLeft();
		ScreenToClient(&m_ptControls[0][2]);
	m_ctrlDivisions.GetWindowRect(rect);
		m_ptControls[1][2] = rect.TopLeft();
		ScreenToClient(&m_ptControls[1][2]);
	m_ctrlLevelsText.GetWindowRect(rect);
		m_ptControls[0][3] = rect.TopLeft();
		ScreenToClient(&m_ptControls[0][3]);
	m_ctrlLevels.GetWindowRect(rect);
		m_ptControls[1][3] = rect.TopLeft();
		ScreenToClient(&m_ptControls[1][3]);
	m_ctrlEventsText.GetWindowRect(rect);
		m_ptControls[0][4] = rect.TopLeft();
		ScreenToClient(&m_ptControls[0][4]);
	m_ctrlEvents.GetWindowRect(rect);
		m_ptControls[1][4] = rect.TopLeft();
		ScreenToClient(&m_ptControls[1][4]);

	// Set the contents of the type combo.
	ARBDogExistingPoints::PointType types[] =
	{
		ARBDogExistingPoints::eOtherPoints,
		ARBDogExistingPoints::eRuns,
		ARBDogExistingPoints::eSpeed,
		ARBDogExistingPoints::eSQ
	};
	int nTypes = sizeof(types) / sizeof(types[0]);
	for (int i = 0; i < nTypes; ++i)
	{
		int index = m_ctrlType.AddString(ARBDogExistingPoints::GetPointTypeName(types[i]).c_str());
		m_ctrlType.SetItemData(index, types[i]);
	}
	for (ARBConfigVenueList::const_iterator iterVenue = m_pDoc->GetConfig().GetVenues().begin();
		iterVenue != m_pDoc->GetConfig().GetVenues().end();
		++iterVenue)
	{
		ARBConfigVenuePtr pVenue = (*iterVenue);
		if (0 < pVenue->GetMultiQs().size())
		{
			int index = m_ctrlType.AddString(ARBDogExistingPoints::GetPointTypeName(ARBDogExistingPoints::eMQ).c_str());
			m_ctrlType.SetItemData(index, ARBDogExistingPoints::eMQ);
			break;
		}
	}

	// Set the contents of the other points combo.
	for (ARBConfigOtherPointsList::const_iterator iterOther = m_pDoc->GetConfig().GetOtherPoints().begin();
		iterOther != m_pDoc->GetConfig().GetOtherPoints().end();
		++iterOther)
	{
		ARBConfigOtherPointsPtr pOther = (*iterOther);
		int index = m_ctrlOther.AddString(pOther->GetName().c_str());
		m_ctrlOther.SetItemDataPtr(index,
			new CListPtrData<ARBConfigOtherPointsPtr>(pOther));
	}

	// All other combos are filled based on other content.

	// Now initialize existing data.
	if (m_pExistingPoints)
	{
		if (m_Date.IsValid())
		{
			CTime t = m_Date.GetDate();
			m_ctrlDate.SetTime(&t);
		}
		int index = m_ctrlType.FindStringExact(0,
			ARBDogExistingPoints::GetPointTypeName(m_pExistingPoints->GetType()).c_str());
		if (0 <= index)
			m_ctrlType.SetCurSel(index);
		if (ARBDogExistingPoints::eOtherPoints == m_pExistingPoints->GetType())
		{
			index = m_ctrlOther.FindStringExact(0,
				m_pExistingPoints->GetOtherPoints().c_str());
			if (0 <= index)
				m_ctrlOther.SetCurSel(index);
		}
	}
	FillVenues();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgExistingPoints::OnSelchangeType() 
{
	FillVenues();
}

void CDlgExistingPoints::OnSelchangeVenues() 
{
	FillFromVenue();
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

	BOOL bOther, bMQ, bVenue, bDiv, bLevel, bEvent, bSubName;
	GetEnableLists(index, bOther, bMQ, bVenue, bDiv, bLevel, bEvent, bSubName, false);

	ARBString other;
	if (bOther)
	{
		index = m_ctrlOther.GetCurSel();
		if (CB_ERR == index)
		{
			GotoDlgCtrl(&m_ctrlOther);
			return;
		}
		ARBConfigOtherPointsPtr pOther = GetOtherPointData(index)->GetData();
		other = pOther->GetName();
	}

	ARBString venue;
	if (bVenue)
	{
		index = m_ctrlVenues.GetCurSel();
		if (CB_ERR == index)
		{
			GotoDlgCtrl(&m_ctrlVenues);
			return;
		}
		ARBConfigVenuePtr pVenue = GetVenueData(index)->GetData();
		venue = pVenue->GetName();
	}

	ARBString multiQ;
	if (bMQ)
	{
		index = m_ctrlMultiQ.GetCurSel();
		if (CB_ERR == index)
		{
			GotoDlgCtrl(&m_ctrlMultiQ);
			return;
		}
		ARBConfigMultiQPtr pMultiQ = GetMultiQData(index)->GetData();
		multiQ = pMultiQ->GetName();
	}

	ARBString div;
	if (bDiv)
	{
		index = m_ctrlDivisions.GetCurSel();
		if (CB_ERR == index)
		{
			GotoDlgCtrl(&m_ctrlDivisions);
			return;
		}
		ARBConfigDivisionPtr pDiv = GetDivisionData(index)->GetData();
		div = pDiv->GetName();
	}

	ARBString level;
	if (bLevel)
	{
		index = m_ctrlLevels.GetCurSel();
		if (CB_ERR == index)
		{
			GotoDlgCtrl(&m_ctrlLevels);
			return;
		}
		CDlgPointsDataLevel* pData = GetLevelData(index);
		level = pData->m_pLevel->GetName();
		if (pData->m_pSubLevel)
			level = pData->m_pSubLevel->GetName();
	}

	ARBString evt;
	if (bEvent)
	{
		index = m_ctrlEvents.GetCurSel();
		if (CB_ERR == index)
		{
			GotoDlgCtrl(&m_ctrlEvents);
			return;
		}
		ARBConfigEventPtr pEvent = GetEventData(index)->GetData();
		evt = pEvent->GetName();
	}

	ARBString subName;
	if (bSubName)
	{
		m_SubName.TrimRight();
		m_SubName.TrimLeft();
		subName = (LPCTSTR)m_SubName;
	}

	m_Comments.Replace(_T("\r\n"), _T("\n"));

	if (m_pExistingPoints)
	{
		m_pExistingPoints->SetDate(m_Date);
		m_pExistingPoints->SetType(type);
		m_pExistingPoints->SetOtherPoints(other);
		m_pExistingPoints->SetVenue(venue);
		m_pExistingPoints->SetMultiQ(multiQ);
		m_pExistingPoints->SetDivision(div);
		m_pExistingPoints->SetLevel(level);
		m_pExistingPoints->SetEvent(evt);
		m_pExistingPoints->SetSubName(subName);
		m_pExistingPoints->SetComment((LPCTSTR)m_Comments);
		m_pExistingPoints->SetPoints(m_Points);
	}
	else
	{
		ARBDogExistingPointsPtr pPoints(new ARBDogExistingPoints());
		if (m_PointsList.AddExistingPoints(pPoints))
		{
			pPoints->SetDate(m_Date);
			pPoints->SetType(type);
			pPoints->SetOtherPoints(other);
			pPoints->SetVenue(venue);
			pPoints->SetMultiQ(multiQ);
			pPoints->SetDivision(div);
			pPoints->SetLevel(level);
			pPoints->SetEvent(evt);
			pPoints->SetSubName(subName);
			pPoints->SetComment((LPCTSTR)m_Comments);
			pPoints->SetPoints(m_Points);
		}
	}

	CDlgBaseDialog::OnOK();
}
