/*
 * Copyright © 2006-2007 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgConfigEventMethod class
 * @author David Connet
 *
 * Revision History
 * @li 2006-11-04 DRC Created.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgConfigEventMethod.h"

#include "ARBAgilityRecordBook.h"
#include "ARBConfigEvent.h"
#include "ARBConfigVenue.h"
#include "DlgConfigPlaceInfo.h"
#include "DlgConfigTitlePoints.h"
#include "DlgConfigure.h"
#include "DlgConfigureData.h"
#include "DlgName.h"
#include "ListData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataPlacement : public CListData
{
public:
	CDlgConfigureDataPlacement(short inPlace, double inVal)
		: m_Place(inPlace)
		, m_Value(inVal)
	{
	}
	virtual CString OnNeedText(int iColumn) const;
	short Place() const			{return m_Place;}
	void Place(short place)		{m_Place = place;}
	double Value() const		{return m_Value;}
	void Value(double val)		{m_Value = val;}
protected:
	short m_Place;
	double m_Value;
};

CString CDlgConfigureDataPlacement::OnNeedText(int iColumn) const
{
	ARBostringstream str;
	switch (iColumn)
	{
	default:
		break;
	case 0:
		str << m_Place;
		break;
	case 1:
		str << m_Value;
		break;
	}
	return str.str().c_str();
}

int CALLBACK ComparePlacement(
		LPARAM lParam1,
		LPARAM lParam2,
		LPARAM lParamSort)
{
	CDlgConfigureDataPlacement* item1 =
		dynamic_cast<CDlgConfigureDataPlacement*>(
			reinterpret_cast<CListData*>(lParam1));
	CDlgConfigureDataPlacement* item2 =
		dynamic_cast<CDlgConfigureDataPlacement*>(
			reinterpret_cast<CListData*>(lParam2));
	return item1->Place() > item2->Place();
}

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigEventMethod dialog

CDlgConfigEventMethod::CDlgConfigEventMethod(
		ARBConfigVenuePtr pVenue,
		ARBConfigScoringPtr pScoring,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgConfigEventMethod::IDD, pParent)
	, m_ctrlDivision(true)
	, m_ctrlLevel(false)
	, m_ctrlType(false)
	, m_ctrlPlacement(true)
	, m_pVenue(pVenue)
	, m_pScoring(pScoring)
{
	// Copy the existing scorings.
	ASSERT(m_pVenue);
	ASSERT(m_pScoring);
	m_pScoring->GetPlaceInfo().Clone(m_PlaceInfo);
	//{{AFX_DATA_INIT(CDlgConfigEventMethod)
	m_DropFractions = m_pScoring->DropFractions() ? TRUE : FALSE;
	m_Bonus = m_pScoring->HasBonusPts() ? TRUE : FALSE;
	m_SuperQ = m_pScoring->HasSuperQ() ? TRUE : FALSE;
	m_SpeedPts = m_pScoring->HasSpeedPts() ? TRUE : FALSE;
	m_Multiply = m_pScoring->TimeFaultMultiplier();
	m_TimeFaultsCleanQ = m_pScoring->QsMustBeClean() ? TRUE : FALSE;
	m_TimeFaultsUnder = m_pScoring->ComputeTimeFaultsUnder() ? TRUE : FALSE;
	m_TimeFaultsOver = m_pScoring->ComputeTimeFaultsOver() ? TRUE : FALSE;
	m_SubtractTimeFaults = m_pScoring->SubtractTimeFaultsFromScore() ? TRUE : FALSE;
	m_OpeningPts = m_pScoring->GetRequiredOpeningPoints();
	m_ClosingPts = m_pScoring->GetRequiredClosingPoints();
	//}}AFX_DATA_INIT
}

void CDlgConfigEventMethod::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConfigEventMethod)
	DDX_Control(pDX, IDC_CONFIG_EVENT_DIVISION, m_ctrlDivision);
	DDX_Control(pDX, IDC_CONFIG_EVENT_LEVEL, m_ctrlLevel);
	DDX_Control(pDX, IDC_CONFIG_EVENT_DATE_VALID_FROM, m_ctrlValidFrom);
	DDX_Control(pDX, IDC_CONFIG_EVENT_DATE_START, m_ctrlDateFrom);
	DDX_Control(pDX, IDC_CONFIG_EVENT_DATE_VALID_TO, m_ctrlValidTo);
	DDX_Control(pDX, IDC_CONFIG_EVENT_DATE_END, m_ctrlDateTo);
	DDX_Control(pDX, IDC_CONFIG_EVENT_TYPE, m_ctrlType);
	DDX_Control(pDX, IDC_CONFIG_EVENT_DROP_FRACTIONS, m_ctrlDropFractions);
	DDX_Check(pDX, IDC_CONFIG_EVENT_DROP_FRACTIONS, m_DropFractions);
	DDX_Control(pDX, IDC_CONFIG_EVENT_BONUS, m_ctrlBonus);
	DDX_Check(pDX, IDC_CONFIG_EVENT_BONUS, m_Bonus);
	DDX_Control(pDX, IDC_CONFIG_EVENT_SUPERQ, m_ctrlSuperQ);
	DDX_Check(pDX, IDC_CONFIG_EVENT_SUPERQ, m_SuperQ);
	DDX_Control(pDX, IDC_CONFIG_EVENT_SPEED, m_ctrlSpeedPts);
	DDX_Check(pDX, IDC_CONFIG_EVENT_SPEED, m_SpeedPts);
	DDX_Control(pDX, IDC_CONFIG_EVENT_PLACEMENT_TEXT, m_ctrlPlacementText);
	DDX_Control(pDX, IDC_CONFIG_EVENT_PLACEMENT, m_ctrlPlacement);
	DDX_Control(pDX, IDC_CONFIG_EVENT_PLACEMENT_NEW, m_ctrlPlacementNew);
	DDX_Control(pDX, IDC_CONFIG_EVENT_PLACEMENT_EDIT, m_ctrlPlacementEdit);
	DDX_Control(pDX, IDC_CONFIG_EVENT_PLACEMENT_DELETE, m_ctrlPlacementDelete);
	DDX_Control(pDX, IDC_CONFIG_EVENT_TF_MULTIPLY_TEXT, m_ctrlMultiplyText);
	DDX_Control(pDX, IDC_CONFIG_EVENT_TF_MULTIPLY, m_ctrlMultiply);
	DDX_Text(pDX, IDC_CONFIG_EVENT_TF_MULTIPLY, m_Multiply);
	DDX_Control(pDX, IDC_CONFIG_EVENT_TIME_FAULTS_CLEANQ, m_ctrlTimeFaultsCleanQ);
	DDX_Check(pDX, IDC_CONFIG_EVENT_TIME_FAULTS_CLEANQ, m_TimeFaultsCleanQ);
	DDX_Control(pDX, IDC_CONFIG_EVENT_TIME_FAULTS_UNDER, m_ctrlTimeFaultsUnder);
	DDX_Check(pDX, IDC_CONFIG_EVENT_TIME_FAULTS_UNDER, m_TimeFaultsUnder);
	DDX_Control(pDX, IDC_CONFIG_EVENT_TIME_FAULTS_OVER, m_ctrlTimeFaultsOver);
	DDX_Check(pDX, IDC_CONFIG_EVENT_TIME_FAULTS_OVER, m_TimeFaultsOver);
	DDX_Control(pDX, IDC_CONFIG_EVENT_SUBTRACT_TIME_FAULTS, m_ctrlSubtractTimeFaults);
	DDX_Check(pDX, IDC_CONFIG_EVENT_SUBTRACT_TIME_FAULTS, m_SubtractTimeFaults);
	DDX_Control(pDX, IDC_CONFIG_EVENT_OPENING_PTS_TEXT, m_ctrlPointsOpeningText);
	DDX_Control(pDX, IDC_CONFIG_EVENT_OPENING_PTS, m_ctrlPointsOpening);
	DDX_Text(pDX, IDC_CONFIG_EVENT_OPENING_PTS, m_OpeningPts);
	DDX_Control(pDX, IDC_CONFIG_EVENT_CLOSING_PTS_TEXT, m_ctrlPointsClosingText);
	DDX_Control(pDX, IDC_CONFIG_EVENT_CLOSING_PTS, m_ctrlPointsClosing);
	DDX_Text(pDX, IDC_CONFIG_EVENT_CLOSING_PTS, m_ClosingPts);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgConfigEventMethod, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgConfigEventMethod)
	ON_CBN_SELCHANGE(IDC_CONFIG_EVENT_DIVISION, OnCbnSelchangeDivision)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_DATE_VALID_FROM, OnValidFrom)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_DATE_VALID_TO, OnValidTo)
	ON_CBN_SELCHANGE(IDC_CONFIG_EVENT_TYPE, OnSelchangeType)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_SPEED, OnSpeedPoints)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_CONFIG_EVENT_PLACEMENT, OnGetdispinfoPlacement)
	ON_NOTIFY(NM_DBLCLK, IDC_CONFIG_EVENT_PLACEMENT, OnDblclkPlacement)
	ON_NOTIFY(LVN_KEYDOWN, IDC_CONFIG_EVENT_PLACEMENT, OnKeydownPlacement)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CONFIG_EVENT_PLACEMENT, OnItemchangedPlacement)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_PLACEMENT_NEW, OnPlacementNew)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_PLACEMENT_EDIT, OnPlacementEdit)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_PLACEMENT_DELETE, OnPlacementDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgConfigEventMethod::UpdateButtons()
{
	BOOL bEdit = FALSE;
	BOOL bDelete = FALSE;
	if (m_SpeedPts && 0 <= m_ctrlPlacement.GetSelection())
		bEdit = bDelete = TRUE;
	m_ctrlPlacementEdit.EnableWindow(bEdit);
	m_ctrlPlacementDelete.EnableWindow(bDelete);

	m_ctrlPlacementText.ShowWindow(m_SpeedPts ? SW_SHOW : SW_HIDE);
	m_ctrlPlacement.ShowWindow(m_SpeedPts ? SW_SHOW : SW_HIDE);
	m_ctrlPlacementNew.ShowWindow(m_SpeedPts ? SW_SHOW : SW_HIDE);
	m_ctrlPlacementEdit.ShowWindow(m_SpeedPts ? SW_SHOW : SW_HIDE);
	m_ctrlPlacementDelete.ShowWindow(m_SpeedPts ? SW_SHOW : SW_HIDE);
}

void CDlgConfigEventMethod::FillDivisionList()
{
	m_ctrlDivision.ResetContent();
	CString all;
	all.LoadString(IDS_ALL);
	int index = m_ctrlDivision.AddString(all);
	if (m_pScoring->GetDivision() == WILDCARD_DIVISION)
		m_ctrlDivision.SetCurSel(index);
	for (ARBConfigDivisionList::iterator iter = m_pVenue->GetDivisions().begin();
		iter != m_pVenue->GetDivisions().end();
		++iter)
	{
		ARBConfigDivisionPtr pDiv = (*iter);
		index = m_ctrlDivision.AddString(pDiv->GetName().c_str());
		m_ctrlDivision.SetData(index,
			new CListPtrData<ARBConfigDivisionPtr>(pDiv));
		if (m_pScoring->GetDivision() == pDiv->GetName())
			m_ctrlDivision.SetCurSel(index);
	}
}

void CDlgConfigEventMethod::FillLevelList()
{
	m_ctrlLevel.ResetContent();
	CString all;
	all.LoadString(IDS_ALL);
	int index = m_ctrlLevel.AddString(all);
	if (m_pScoring->GetLevel() == WILDCARD_LEVEL)
		m_ctrlLevel.SetCurSel(index);
	int idxDiv = m_ctrlDivision.GetCurSel();
	if (0 == idxDiv) // All
	{
		for (ARBConfigDivisionList::iterator iter = m_pVenue->GetDivisions().begin();
			iter != m_pVenue->GetDivisions().end();
			++iter)
		{
			ARBConfigDivisionPtr pDiv = (*iter);
			for (ARBConfigLevelList::iterator iterLevel = pDiv->GetLevels().begin();
				iterLevel != pDiv->GetLevels().end();
				++iterLevel)
			{
				if (CB_ERR == m_ctrlLevel.FindStringExact(0, (*iterLevel)->GetName().c_str()))
				{
					index = m_ctrlLevel.AddString((*iterLevel)->GetName().c_str());
					if (m_pScoring->GetLevel() == (*iterLevel)->GetName())
						m_ctrlLevel.SetCurSel(index);
				}
			}
		}
	}
	else if (0 < idxDiv)
	{
		CListPtrData<ARBConfigDivisionPtr>* pDiv = reinterpret_cast<CListPtrData<ARBConfigDivisionPtr>*>(m_ctrlDivision.GetItemDataPtr(idxDiv));
		for (ARBConfigLevelList::iterator iterLevel = pDiv->GetData()->GetLevels().begin();
			iterLevel != pDiv->GetData()->GetLevels().end();
			++iterLevel)
		{
			index = m_ctrlLevel.AddString((*iterLevel)->GetName().c_str());
			if (m_pScoring->GetLevel() == (*iterLevel)->GetName())
				m_ctrlLevel.SetCurSel(index);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigEventMethod message handlers

BOOL CDlgConfigEventMethod::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();
	m_ctrlPlacement.SetExtendedStyle(m_ctrlPlacement.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	m_ctrlPointsOpeningText.GetWindowText(m_strOpening[0]);
	m_strOpening[1].LoadString(IDS_SCORING_REQUIRED_POINTS);

	// If any additional types are added in ARBConfigScoring,
	// they'll have to be manually added here...
	static ARBConfigScoring::ScoringStyle const Styles[] =
	{
		ARBConfigScoring::eFaultsThenTime,
		ARBConfigScoring::eFaults100ThenTime,
		ARBConfigScoring::eFaults200ThenTime,
		ARBConfigScoring::eOCScoreThenTime,
		ARBConfigScoring::eScoreThenTime,
		ARBConfigScoring::eTimePlusFaults
	};
	static int const nStyles = sizeof(Styles) / sizeof(Styles[0]);
	int index;
	for (index = 0; index < nStyles; ++index)
	{
		ARBString str = ARBConfigScoring::GetScoringStyleStr(Styles[index]);
		int idx = m_ctrlType.AddString(str.c_str());
		m_ctrlType.SetItemData(idx, Styles[index]);
	}

	CTime t;
	if (m_pScoring->GetValidFrom().IsValid())
	{
		m_ctrlValidFrom.SetCheck(1);
		m_ctrlDateFrom.EnableWindow(TRUE);
		t = m_pScoring->GetValidFrom().GetDate();
	}
	else
	{
		m_ctrlValidFrom.SetCheck(0);
		m_ctrlDateFrom.EnableWindow(FALSE);
		t = CTime::GetCurrentTime();
	}
	m_ctrlDateFrom.SetTime(&t);

	if (m_pScoring->GetValidTo().IsValid())
	{
		m_ctrlValidTo.SetCheck(1);
		m_ctrlDateTo.EnableWindow(TRUE);
		t = m_pScoring->GetValidTo().GetDate();
	}
	else
	{
		m_ctrlValidTo.SetCheck(0);
		m_ctrlDateTo.EnableWindow(FALSE);
		t = CTime::GetCurrentTime();
	}
	m_ctrlDateTo.SetTime(&t);

	FillDivisionList();
	FillLevelList();
	m_ctrlType.SetCurSel(-1);
	for (int idxType = 0; idxType < m_ctrlType.GetCount(); ++idxType)
	{
		ARBConfigScoring::ScoringStyle style = static_cast<ARBConfigScoring::ScoringStyle>(m_ctrlType.GetItemData(idxType));
		if (m_pScoring->GetScoringStyle() == style)
		{
			m_ctrlType.SetCurSel(idxType);
			break;
		}
	}
	OnSelchangeType();

	CString str;
	str.LoadString(IDS_COL_PLACE);
	m_ctrlPlacement.InsertColumn(0, str);
	str.LoadString(IDS_COL_MULTIPLIER);
	m_ctrlPlacement.InsertColumn(1, str);
	index = 0;
	for (ARBConfigPlaceInfoList::iterator iter = m_PlaceInfo.begin();
		iter != m_PlaceInfo.end();
		++index, ++iter)
	{
		CDlgConfigureDataPlacement* pData = new CDlgConfigureDataPlacement(
			(*iter)->GetPlace(), (*iter)->GetValue());
		m_ctrlPlacement.InsertItem(LVIF_TEXT | LVIF_PARAM,
			m_ctrlPlacement.GetItemCount(),
			LPSTR_TEXTCALLBACK, 0, 0, 0,
			reinterpret_cast<LPARAM>(static_cast<CListData*>(pData)));
	}
	m_ctrlPlacement.SortItems(ComparePlacement, 0);
	m_ctrlPlacement.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_ctrlPlacement.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
	UpdateButtons();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgConfigEventMethod::OnCbnSelchangeDivision()
{
	FillLevelList();
}

void CDlgConfigEventMethod::OnValidFrom() 
{
	BOOL bEnable = FALSE;
	if (m_ctrlValidFrom.GetCheck())
		bEnable = TRUE;
	m_ctrlDateFrom.EnableWindow(bEnable);
}

void CDlgConfigEventMethod::OnValidTo() 
{
	BOOL bEnable = FALSE;
	if (m_ctrlValidTo.GetCheck())
		bEnable = TRUE;
	m_ctrlDateTo.EnableWindow(bEnable);
}

void CDlgConfigEventMethod::OnSelchangeType() 
{
	int idxType = m_ctrlType.GetCurSel();
	if (CB_ERR == idxType)
	{
		m_ctrlPointsOpeningText.ShowWindow(SW_HIDE);
		m_ctrlPointsOpening.ShowWindow(SW_HIDE);
		m_ctrlPointsClosingText.ShowWindow(SW_HIDE);
		m_ctrlPointsClosing.ShowWindow(SW_HIDE);
		m_ctrlDropFractions.ShowWindow(SW_HIDE);
		m_ctrlTimeFaultsCleanQ.ShowWindow(SW_HIDE);
		m_ctrlSubtractTimeFaults.ShowWindow(SW_HIDE);
		m_ctrlTimeFaultsUnder.ShowWindow(SW_HIDE);
		m_ctrlTimeFaultsOver.ShowWindow(SW_HIDE);
		m_ctrlMultiplyText.ShowWindow(SW_HIDE);
		m_ctrlMultiply.ShowWindow(SW_HIDE);
	}
	else
	{
		switch (static_cast<ARBConfigScoring::ScoringStyle>(m_ctrlType.GetItemData(idxType)))
		{
		default:
			m_ctrlPointsOpeningText.ShowWindow(SW_HIDE);
			m_ctrlPointsOpening.ShowWindow(SW_HIDE);
			m_ctrlPointsClosingText.ShowWindow(SW_HIDE);
			m_ctrlPointsClosing.ShowWindow(SW_HIDE);
			m_ctrlDropFractions.ShowWindow(SW_HIDE);
			m_ctrlTimeFaultsCleanQ.ShowWindow(SW_HIDE);
			m_ctrlSubtractTimeFaults.ShowWindow(SW_HIDE);
			m_ctrlTimeFaultsUnder.ShowWindow(SW_HIDE);
			m_ctrlTimeFaultsOver.ShowWindow(SW_HIDE);
			m_ctrlMultiplyText.ShowWindow(SW_HIDE);
			m_ctrlMultiply.ShowWindow(SW_HIDE);
			break;
		case ARBConfigScoring::eFaultsThenTime:
		case ARBConfigScoring::eFaults100ThenTime:
		case ARBConfigScoring::eFaults200ThenTime:
			m_ctrlPointsOpeningText.ShowWindow(SW_HIDE);
			m_ctrlPointsOpening.ShowWindow(SW_HIDE);
			m_ctrlPointsClosingText.ShowWindow(SW_HIDE);
			m_ctrlPointsClosing.ShowWindow(SW_HIDE);
			m_ctrlDropFractions.ShowWindow(SW_SHOW);
			m_ctrlTimeFaultsCleanQ.ShowWindow(SW_HIDE);
			m_ctrlSubtractTimeFaults.ShowWindow(SW_HIDE);
			m_ctrlTimeFaultsUnder.ShowWindow(SW_HIDE);
			m_ctrlTimeFaultsOver.ShowWindow(SW_HIDE);
			m_ctrlMultiplyText.ShowWindow(SW_SHOW);
			m_ctrlMultiply.ShowWindow(SW_SHOW);
			break;
		case ARBConfigScoring::eOCScoreThenTime:
			m_ctrlPointsOpeningText.ShowWindow(SW_SHOW);
			m_ctrlPointsOpening.ShowWindow(SW_SHOW);
			m_ctrlPointsClosingText.ShowWindow(SW_SHOW);
			m_ctrlPointsClosing.ShowWindow(SW_SHOW);
			m_ctrlDropFractions.ShowWindow(SW_SHOW);
			m_ctrlTimeFaultsCleanQ.ShowWindow(SW_HIDE);
			m_ctrlSubtractTimeFaults.ShowWindow(SW_SHOW);
			m_ctrlTimeFaultsUnder.ShowWindow(SW_SHOW);
			m_ctrlTimeFaultsOver.ShowWindow(SW_SHOW);
			m_ctrlMultiplyText.ShowWindow(SW_HIDE);
			m_ctrlMultiply.ShowWindow(SW_HIDE);
			m_ctrlPointsOpeningText.SetWindowText(m_strOpening[0]);
			break;
		case ARBConfigScoring::eScoreThenTime:
			m_ctrlPointsOpeningText.ShowWindow(SW_SHOW);
			m_ctrlPointsOpening.ShowWindow(SW_SHOW);
			m_ctrlPointsClosingText.ShowWindow(SW_HIDE);
			m_ctrlPointsClosing.ShowWindow(SW_HIDE);
			m_ctrlDropFractions.ShowWindow(SW_SHOW);
			m_ctrlTimeFaultsCleanQ.ShowWindow(SW_HIDE);
			m_ctrlSubtractTimeFaults.ShowWindow(SW_SHOW);
			m_ctrlTimeFaultsUnder.ShowWindow(SW_SHOW);
			m_ctrlTimeFaultsOver.ShowWindow(SW_SHOW);
			m_ctrlMultiplyText.ShowWindow(SW_HIDE);
			m_ctrlMultiply.ShowWindow(SW_HIDE);
			m_ctrlPointsOpeningText.SetWindowText(m_strOpening[1]);
			break;
		case ARBConfigScoring::eTimePlusFaults:
			m_ctrlPointsOpeningText.ShowWindow(SW_HIDE);
			m_ctrlPointsOpening.ShowWindow(SW_HIDE);
			m_ctrlPointsClosingText.ShowWindow(SW_HIDE);
			m_ctrlPointsClosing.ShowWindow(SW_HIDE);
			m_ctrlDropFractions.ShowWindow(SW_SHOW);
			m_ctrlTimeFaultsCleanQ.ShowWindow(SW_SHOW);
			m_ctrlSubtractTimeFaults.ShowWindow(SW_HIDE);
			m_ctrlTimeFaultsUnder.ShowWindow(SW_SHOW);
			m_ctrlTimeFaultsOver.ShowWindow(SW_SHOW);
			m_ctrlMultiplyText.ShowWindow(SW_SHOW);
			m_ctrlMultiply.ShowWindow(SW_SHOW);
			break;
		}
	}
}

void CDlgConfigEventMethod::OnSpeedPoints()
{
	UpdateData(TRUE);
	m_ctrlPlacementText.ShowWindow(m_SpeedPts ? SW_SHOW : SW_HIDE);
	m_ctrlPlacement.ShowWindow(m_SpeedPts ? SW_SHOW : SW_HIDE);
	m_ctrlPlacementNew.ShowWindow(m_SpeedPts ? SW_SHOW : SW_HIDE);
	m_ctrlPlacementEdit.ShowWindow(m_SpeedPts ? SW_SHOW : SW_HIDE);
	m_ctrlPlacementDelete.ShowWindow(m_SpeedPts ? SW_SHOW : SW_HIDE);
}

void CDlgConfigEventMethod::OnGetdispinfoPlacement(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CListData* pRawData = reinterpret_cast<CListData*>(pDispInfo->item.lParam);
		CDlgConfigureDataPlacement* pData = dynamic_cast<CDlgConfigureDataPlacement*>(pRawData);
		if (pData)
		{
			CString str = pData->OnNeedText(pDispInfo->item.iSubItem);
			::lstrcpyn(pDispInfo->item.pszText, str, pDispInfo->item.cchTextMax);
			pDispInfo->item.pszText[pDispInfo->item.cchTextMax-1] = '\0';
		}
	}
	*pResult = 0;
}

void CDlgConfigEventMethod::OnDblclkPlacement(NMHDR* pNMHDR, LRESULT* pResult)
{
	OnPlacementEdit();
	*pResult = 0;
}

void CDlgConfigEventMethod::OnKeydownPlacement(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_KEYDOWN* pLVKeyDown = reinterpret_cast<LV_KEYDOWN*>(pNMHDR);
	switch (pLVKeyDown->wVKey)
	{
	default:
		break;
	case VK_SPACE:
		OnPlacementEdit();
		break;
	}
	*pResult = 0;
}

void CDlgConfigEventMethod::OnItemchangedPlacement(NMHDR* pNMHDR, LRESULT* pResult)
{
	UpdateButtons();
	*pResult = 0;
}

void CDlgConfigEventMethod::OnPlacementNew()
{
	std::vector<short> placements;
	int count = m_ctrlPlacement.GetItemCount();
	for (int index = 0; index < count; ++index)
	{
		CDlgConfigureDataPlacement* pData = dynamic_cast<CDlgConfigureDataPlacement*>(m_ctrlPlacement.GetData(index));
		placements.push_back(pData->Place());
	}
	CDlgConfigPlaceInfo dlg(placements, -1, 0.0, this);
	if (IDOK == dlg.DoModal())
	{
		CDlgConfigureDataPlacement* pData = new CDlgConfigureDataPlacement(
			dlg.GetPlace(), dlg.GetValue());
		m_ctrlPlacement.InsertItem(LVIF_TEXT | LVIF_PARAM,
			m_ctrlPlacement.GetItemCount(),
			LPSTR_TEXTCALLBACK, 0, 0, 0,
			reinterpret_cast<LPARAM>(static_cast<CListData*>(pData)));
		m_ctrlPlacement.SortItems(ComparePlacement, 0);
		m_ctrlPlacement.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
		m_ctrlPlacement.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
		UpdateButtons();
	}
}

void CDlgConfigEventMethod::OnPlacementEdit()
{
	int idx = m_ctrlPlacement.GetSelection();
	if (0 > idx)
		return;
	std::vector<short> placements;
	int count = m_ctrlPlacement.GetItemCount();
	for (int index = 0; index < count; ++index)
	{
		CDlgConfigureDataPlacement* pData = dynamic_cast<CDlgConfigureDataPlacement*>(m_ctrlPlacement.GetData(index));
		placements.push_back(pData->Place());
	}
	CDlgConfigureDataPlacement* pData = dynamic_cast<CDlgConfigureDataPlacement*>(m_ctrlPlacement.GetData(idx));
	CDlgConfigPlaceInfo dlg(placements, pData->Place(), pData->Value(), this);
	if (IDOK == dlg.DoModal())
	{
		pData->Place(dlg.GetPlace());
		pData->Value(dlg.GetValue());
		m_ctrlPlacement.SortItems(ComparePlacement, 0);
		m_ctrlPlacement.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
		m_ctrlPlacement.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
		UpdateButtons();
	}
}

void CDlgConfigEventMethod::OnPlacementDelete()
{
	int idx = m_ctrlPlacement.GetSelection();
	if (0 > idx)
		return;
	m_ctrlPlacement.DeleteItem(idx);
	UpdateButtons();
}

void CDlgConfigEventMethod::OnOK()
{
	if (!UpdateData(TRUE))
		return;

	// Verify we have selections
	int idxDiv = m_ctrlDivision.GetCurSel();
	if (LB_ERR == idxDiv)
	{
		AfxMessageBox(IDS_SELECT_DIVISION, MB_ICONWARNING);
		GotoDlgCtrl(&m_ctrlDivision);
		return;
	}
	int idxLevel = m_ctrlLevel.GetCurSel();
	if (LB_ERR == idxLevel)
	{
		AfxMessageBox(IDS_SELECT_LEVEL, MB_ICONWARNING);
		GotoDlgCtrl(&m_ctrlLevel);
		return;
	}
	int idxType = m_ctrlType.GetCurSel();
	if (LB_ERR == idxType)
	{
		AfxMessageBox(IDS_SELECT_STYLE, MB_ICONWARNING);
		GotoDlgCtrl(&m_ctrlType);
		return;
	}

	ARBDate dateFrom;
	if (m_ctrlValidFrom.GetCheck())
	{
		CTime time;
		if (GDT_VALID == m_ctrlDateFrom.GetTime(time))
			dateFrom = ARBDate(time.GetYear(), time.GetMonth(), time.GetDay());
	}
	ARBDate dateTo;
	if (m_ctrlValidTo.GetCheck())
	{
		CTime time;
		if (GDT_VALID == m_ctrlDateTo.GetTime(time))
			dateTo = ARBDate(time.GetYear(), time.GetMonth(), time.GetDay());
	}

	// Validate that from-to dates are okay.
	if (dateFrom.IsValid() && dateTo.IsValid()
	&& dateFrom > dateTo)
	{
		AfxMessageBox(IDS_CONFIGEVENT_DATEORDER, MB_ICONWARNING);
		GotoDlgCtrl(&m_ctrlDateTo);
		return;
	}

	// Validate placements.
	if (m_SpeedPts)
	{
		if (1 == m_ctrlPlacement.GetItemCount())
		{
			CDlgConfigureDataPlacement* pData = dynamic_cast<CDlgConfigureDataPlacement*>(m_ctrlPlacement.GetData(0));
			if (0 == pData->Place())
			{
				AfxMessageBox(IDS_CONFIGEVENT_PLACEMENT0, MB_ICONWARNING);
				GotoDlgCtrl(&m_ctrlPlacementNew);
				return;
			}
		}
	}

	// Save it.
	CString str;
	m_pScoring->SetValidFrom(dateFrom);
	m_pScoring->SetValidTo(dateTo);
	if (0 == idxDiv)
		str = WILDCARD_DIVISION;
	else
		m_ctrlDivision.GetLBText(idxDiv, str);
	m_pScoring->SetDivision((LPCTSTR)str);
	if (0 == idxLevel)
		str = WILDCARD_LEVEL;
	else
		m_ctrlLevel.GetLBText(idxLevel, str);
	m_pScoring->SetLevel((LPCTSTR)str);

	m_pScoring->SetDropFractions(m_DropFractions ? true : false);
	m_pScoring->SetHasBonusPts(m_Bonus ? true : false);
	m_pScoring->SetHasSuperQ(m_SuperQ ? true : false);
	m_pScoring->SetHasSpeedPts(m_SpeedPts ? true : false);
	if (m_SpeedPts)
	{
		m_PlaceInfo.clear();
		int count = m_ctrlPlacement.GetItemCount();
		for (int index = 0; index < count; ++index)
		{
			CDlgConfigureDataPlacement* pData = dynamic_cast<CDlgConfigureDataPlacement*>(m_ctrlPlacement.GetData(index));
			m_PlaceInfo.AddPlaceInfo(pData->Place(), pData->Value());
		}
		m_pScoring->GetPlaceInfo() = m_PlaceInfo;
	}

	ARBConfigScoring::ScoringStyle style = static_cast<ARBConfigScoring::ScoringStyle>(m_ctrlType.GetItemData(idxType));
	m_pScoring->SetScoringStyle(style);
	switch (style)
	{
	default:
		break;
	case ARBConfigScoring::eFaultsThenTime:
	case ARBConfigScoring::eFaults100ThenTime:
	case ARBConfigScoring::eFaults200ThenTime:
		m_pScoring->SetTimeFaultMultiplier(m_Multiply);
		break;
	case ARBConfigScoring::eOCScoreThenTime:
		m_pScoring->SetRequiredOpeningPoints(m_OpeningPts);
		m_pScoring->SetRequiredClosingPoints(m_ClosingPts);
		m_pScoring->SetSubtractTimeFaultsFromScore(m_SubtractTimeFaults ? true : false);
		m_pScoring->SetComputeTimeFaultsUnder(m_TimeFaultsUnder ? true : false);
		m_pScoring->SetComputeTimeFaultsOver(m_TimeFaultsOver ? true : false);
		break;
	case ARBConfigScoring::eScoreThenTime:
		m_pScoring->SetRequiredOpeningPoints(m_OpeningPts);
		m_pScoring->SetSubtractTimeFaultsFromScore(m_SubtractTimeFaults ? true : false);
		m_pScoring->SetComputeTimeFaultsUnder(m_TimeFaultsUnder ? true : false);
		m_pScoring->SetComputeTimeFaultsOver(m_TimeFaultsOver ? true : false);
		break;
	case ARBConfigScoring::eTimePlusFaults:
		m_pScoring->SetQsMustBeClean(m_TimeFaultsCleanQ ? true : false);
		m_pScoring->SetComputeTimeFaultsUnder(m_TimeFaultsUnder ? true : false);
		m_pScoring->SetComputeTimeFaultsOver(m_TimeFaultsOver ? true : false);
		m_pScoring->SetTimeFaultMultiplier(m_Multiply);
		break;
	}

	CDlgBaseDialog::OnOK();
}
