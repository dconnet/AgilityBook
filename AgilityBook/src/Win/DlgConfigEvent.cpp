/*
 * Copyright © 2002-2003 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgConfigEvent class
 * @author David Connet
 *
 * There's one caveat to this dialog - there is no verification done to
 * make sure that a division/level pair isn't multiply defined. If that
 * happens, when we go to check things, the first match in the list will
 * be the only one used.
 * Note: The UI checks when creating div/level, so this shouldn't happen.
 * If the file is hand-edited, it could... But just how paranoid do we get?!
 * (Plus, the paranoia checking should be done when the file is loaded.)
 *
 * Revision History
 * @li 2003-12-27 DRC Added support for from/to dates for the scoring method.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgConfigEvent.h"

#include "ARBAgilityRecordBook.h"
#include "ARBConfigEvent.h"
#include "ARBConfigVenue.h"
#include "DlgConfigTitlePoints.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigEvent dialog

CDlgConfigEvent::CDlgConfigEvent(ARBConfigVenue* pVenue, ARBConfigEvent* pEvent, CWnd* pParent)
	: CDialog(CDlgConfigEvent::IDD, pParent)
	, m_pVenue(pVenue)
	, m_pEvent(pEvent)
	, m_Scorings(pEvent->GetScorings())
	, m_idxMethod(-1)
	, m_bHasPartners(FALSE)
{
	ASSERT(m_pVenue);
	ASSERT(m_pEvent);
	m_Name = m_pEvent->GetName().c_str();
	m_Desc = m_pEvent->GetDesc().c_str();
	m_Desc.Replace("\n", "\r\n");
	m_bHasPartners = m_pEvent->HasPartner() ? TRUE : FALSE;
	//{{AFX_DATA_INIT(CDlgConfigEvent)
	m_OpeningPts = 0;
	m_ClosingPts = 0;
	//}}AFX_DATA_INIT
}

CDlgConfigEvent::~CDlgConfigEvent()
{
}

void CDlgConfigEvent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConfigEvent)
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDX_Check(pDX, IDC_PARTNER, m_bHasPartners);
	DDX_Text(pDX, IDC_DESC, m_Desc);
	DDX_Control(pDX, IDC_NEW, m_ctrlNew);
	DDX_Control(pDX, IDC_COPY, m_ctrlCopy);
	DDX_Control(pDX, IDC_DELETE, m_ctrlDelete);
	DDX_Control(pDX, IDC_MOVE_UP, m_ctrlUp);
	DDX_Control(pDX, IDC_MOVE_DOWN, m_ctrlDown);
	DDX_Control(pDX, IDC_METHODS, m_ctrlMethods);
	DDX_Control(pDX, IDC_UNUSED, m_ctrlUnused);
	DDX_Control(pDX, IDC_DATE_VALID_FROM, m_ctrlValidFrom);
	DDX_Control(pDX, IDC_DATE_START, m_ctrlDateFrom);
	DDX_Control(pDX, IDC_DATE_VALID_TO, m_ctrlValidTo);
	DDX_Control(pDX, IDC_DATE_END, m_ctrlDateTo);
	DDX_Control(pDX, IDC_DIVISION, m_ctrlDivision);
	DDX_Control(pDX, IDC_LEVEL, m_ctrlLevel);
	DDX_Control(pDX, IDC_TYPE, m_ctrlType);
	DDX_Control(pDX, IDC_DROP_FRACTIONS, m_ctrlDropFractions);
	DDX_Control(pDX, IDC_OPENING_PTS_TEXT, m_ctrlPointsOpeningText);
	DDX_Control(pDX, IDC_OPENING_PTS, m_ctrlPointsOpening);
	DDX_Text(pDX, IDC_OPENING_PTS, m_OpeningPts);
	DDX_Control(pDX, IDC_CLOSING_PTS_TEXT, m_ctrlPointsClosingText);
	DDX_Control(pDX, IDC_CLOSING_PTS, m_ctrlPointsClosing);
	DDX_Text(pDX, IDC_CLOSING_PTS, m_ClosingPts);
	DDX_Control(pDX, IDC_NOTES, m_ctrlNote);
	DDX_Control(pDX, IDC_SUPERQ, m_ctrlSuperQ);
	DDX_Control(pDX, IDC_MACH, m_ctrlMachPts);
	DDX_Control(pDX, IDC_DOUBLEQ, m_ctrlDoubleQ);
	DDX_Control(pDX, IDC_LIST, m_ctrlTitleList);
	DDX_Control(pDX, IDC_TITLE_NEW, m_ctrlTitleNew);
	DDX_Control(pDX, IDC_TITLE_EDIT, m_ctrlTitleEdit);
	DDX_Control(pDX, IDC_TITLE_DELETE, m_ctrlTitleDelete);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgConfigEvent, CDialog)
	//{{AFX_MSG_MAP(CDlgConfigEvent)
	ON_BN_CLICKED(IDC_NEW, OnBnClickedNew)
	ON_BN_CLICKED(IDC_COPY, OnBnClickedCopy)
	ON_BN_CLICKED(IDC_DELETE, OnBnClickedDelete)
	ON_BN_CLICKED(IDC_MOVE_UP, OnBnClickedUp)
	ON_BN_CLICKED(IDC_MOVE_DOWN, OnBnClickedDown)
	ON_BN_CLICKED(IDC_DATE_VALID_FROM, OnValidFrom)
	ON_BN_CLICKED(IDC_DATE_VALID_TO, OnValidTo)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATE_START, OnDatetimechangeDate)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATE_END, OnDatetimechangeDate)
	ON_LBN_SELCHANGE(IDC_METHODS, OnLbnSelchangeMethods)
	ON_CBN_SELCHANGE(IDC_DIVISION, OnCbnSelchangeDivision)
	ON_CBN_SELCHANGE(IDC_LEVEL, OnCbnSelchangeLevel)
	ON_CBN_SELCHANGE(IDC_TYPE, OnSelchangeType)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
	ON_BN_CLICKED(IDC_TITLE_NEW, OnTitleNew)
	ON_BN_CLICKED(IDC_TITLE_EDIT, OnTitleEdit)
	ON_BN_CLICKED(IDC_TITLE_DELETE, OnTitleDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CString CDlgConfigEvent::GetListName(ARBConfigScoring* pScoring) const
{
	CString all;
	all.LoadString(IDS_ALL);
	CString str;
	if (pScoring->GetDivision() == WILDCARD_DIVISION)
		str = all;
	else
		str = pScoring->GetDivision().c_str();
	str += " / ";
	if (pScoring->GetLevel() == WILDCARD_LEVEL)
		str += all;
	else
		str += pScoring->GetLevel().c_str();
	std::string validStr = pScoring->GetValidDateString();
	if (0 < validStr.length())
	{
		str += ' ';
		str += validStr.c_str();
	}
	return str;
}

void CDlgConfigEvent::FillControls()
{
	int idxMethod = m_ctrlMethods.GetCurSel();
	BOOL bEnable = FALSE;
	if (LB_ERR != idxMethod)
	{
		CString str;
		ARBConfigScoring* pScoring = reinterpret_cast<ARBConfigScoring*>(m_ctrlMethods.GetItemDataPtr(idxMethod));
		bEnable = TRUE;
		CTime t;
		if (pScoring->GetValidFrom().IsValid())
		{
			m_ctrlValidFrom.SetCheck(1);
			t = pScoring->GetValidFrom().GetDate();
		}
		else
		{
			m_ctrlValidFrom.SetCheck(0);
			t = CTime(1990, 1, 1, 0, 0, 0);
		}
		m_ctrlDateFrom.SetTime(&t);
		if (pScoring->GetValidTo().IsValid())
		{
			m_ctrlValidTo.SetCheck(1);
			t = pScoring->GetValidTo().GetDate();
		}
		else
		{
			m_ctrlValidTo.SetCheck(0);
			t = CTime(1990, 1, 1, 0, 0, 0);
		}
		m_ctrlDateTo.SetTime(&t);

		FillDivisionList();
		FillLevelList();
		m_ctrlType.SetCurSel(-1);
		for (int idxType = 0; idxType < m_ctrlType.GetCount(); ++idxType)
		{
			ARBConfigScoring::ScoringStyle style = static_cast<ARBConfigScoring::ScoringStyle>(m_ctrlType.GetItemData(idxType));
			if (pScoring->GetScoringStyle() == style)
			{
				m_ctrlType.SetCurSel(idxType);
				break;
			}
		}
		FillRequiredPoints();
		FillTitlePoints(pScoring);
		if (pScoring->DropFractions())
			m_ctrlDropFractions.SetCheck(1);
		else
			m_ctrlDropFractions.SetCheck(0);
		if (pScoring->HasSuperQ())
			m_ctrlSuperQ.SetCheck(1);
		else
			m_ctrlSuperQ.SetCheck(0);
		if (pScoring->HasMachPts())
			m_ctrlMachPts.SetCheck(1);
		else
			m_ctrlMachPts.SetCheck(0);
		if (pScoring->HasDoubleQ())
			m_ctrlDoubleQ.SetCheck(1);
		else
			m_ctrlDoubleQ.SetCheck(0);
		str = pScoring->GetNote().c_str();
		str.Replace("\n", "\r\n");
		m_ctrlNote.SetWindowText(str);
	}
	else
	{
		CTime t(1990, 1, 1, 0, 0, 0);
		m_ctrlValidFrom.SetCheck(0);
		m_ctrlDateFrom.SetTime(&t);
		m_ctrlValidTo.SetCheck(0);
		m_ctrlDateTo.SetTime(&t);
		m_ctrlDivision.SetCurSel(-1);
		m_ctrlLevel.SetCurSel(-1);
		m_ctrlType.SetCurSel(-1);
		m_ctrlDropFractions.SetCheck(0);
		m_ctrlTitleList.ResetContent();
		m_ctrlSuperQ.SetCheck(0);
		m_ctrlMachPts.SetCheck(0);
		m_ctrlDoubleQ.SetCheck(0);
		m_ctrlNote.SetWindowText("");
		FillRequiredPoints();
	}
	m_ctrlValidFrom.EnableWindow(bEnable);
	m_ctrlDateFrom.EnableWindow(bEnable && m_ctrlValidFrom.GetCheck());
	m_ctrlValidTo.EnableWindow(bEnable);
	m_ctrlDateTo.EnableWindow(bEnable && m_ctrlValidTo.GetCheck());
	m_ctrlDivision.EnableWindow(bEnable);
	m_ctrlLevel.EnableWindow(bEnable);
	m_ctrlType.EnableWindow(bEnable);
	m_ctrlDropFractions.EnableWindow(bEnable);
	m_ctrlSuperQ.EnableWindow(bEnable);
	m_ctrlMachPts.EnableWindow(bEnable);
	m_ctrlDoubleQ.EnableWindow(bEnable);
	m_ctrlCopy.EnableWindow(bEnable);
	m_ctrlDelete.EnableWindow(bEnable);
	m_ctrlUp.EnableWindow(bEnable && 1 < m_ctrlMethods.GetCount() && 0 != idxMethod);
	m_ctrlDown.EnableWindow(bEnable && 1 < m_ctrlMethods.GetCount() && m_ctrlMethods.GetCount() - 1 != idxMethod);
	m_ctrlTitleList.EnableWindow(bEnable);
	m_ctrlTitleNew.EnableWindow(bEnable);
	m_ctrlTitleEdit.EnableWindow(bEnable);
	m_ctrlTitleDelete.EnableWindow(bEnable);
}

void CDlgConfigEvent::FillMethodList()
{
	m_idxMethod = m_ctrlMethods.GetCurSel();
	m_ctrlMethods.ResetContent();
	CString str;
	for (ARBConfigScoringList::iterator iter = m_Scorings.begin();
		iter != m_Scorings.end();
		++iter)
	{
		ARBConfigScoring* pScoring = (*iter);
		str = GetListName(pScoring);
		int index = m_ctrlMethods.AddString(str);
		m_ctrlMethods.SetItemDataPtr(index, reinterpret_cast<void*>(pScoring));
	}
	m_ctrlMethods.SetCurSel(m_idxMethod);

	m_ctrlUnused.ResetContent();
	for (ARBConfigDivisionList::const_iterator iterDiv = m_pVenue->GetDivisions().begin();
		iterDiv != m_pVenue->GetDivisions().end();
		++iterDiv)
	{
		for (ARBConfigLevelList::const_iterator iterLevel = (*iterDiv)->GetLevels().begin();
			iterLevel != (*iterDiv)->GetLevels().end();
			++iterLevel)
		{
				if (!m_Scorings.FindEvent((*iterDiv)->GetName(), (*iterLevel)->GetName(), ARBDate::Today()))
			{
				str = (*iterDiv)->GetName().c_str();
				str += " / ";
				str += (*iterLevel)->GetName().c_str();
				m_ctrlUnused.AddString(str);
			}
			// Remember, configuration doesn't do sublevels.
		}
	}
}

void CDlgConfigEvent::FillDivisionList()
{
	ARBConfigScoring* pScoring = NULL;
	int idxMethod = m_ctrlMethods.GetCurSel();
	if (LB_ERR != idxMethod)
		pScoring = reinterpret_cast<ARBConfigScoring*>(m_ctrlMethods.GetItemDataPtr(idxMethod));
	m_ctrlDivision.ResetContent();
	CString all;
	all.LoadString(IDS_ALL);
	int index = m_ctrlDivision.AddString(all);
	if (pScoring && pScoring->GetDivision() == WILDCARD_DIVISION)
		m_ctrlDivision.SetCurSel(index);
	for (ARBConfigDivisionList::iterator iter = m_pVenue->GetDivisions().begin();
		iter != m_pVenue->GetDivisions().end();
		++iter)
	{
		ARBConfigDivision* pDiv = (*iter);
		index = m_ctrlDivision.AddString(pDiv->GetName().c_str());
		m_ctrlDivision.SetItemDataPtr(index, reinterpret_cast<void*>(pDiv));
		if (pScoring && pScoring->GetDivision() == pDiv->GetName())
			m_ctrlDivision.SetCurSel(index);
	}
}

void CDlgConfigEvent::FillLevelList()
{
	ARBConfigScoring* pScoring = NULL;
	int idxMethod = m_ctrlMethods.GetCurSel();
	if (LB_ERR != idxMethod)
		pScoring = reinterpret_cast<ARBConfigScoring*>(m_ctrlMethods.GetItemDataPtr(idxMethod));
	m_ctrlLevel.ResetContent();
	CString all;
	all.LoadString(IDS_ALL);
	int index = m_ctrlLevel.AddString(all);
	if (pScoring->GetLevel() == WILDCARD_LEVEL)
		m_ctrlLevel.SetCurSel(index);
	int idxDiv = m_ctrlDivision.GetCurSel();
	if (0 == idxDiv) // All
	{
		for (ARBConfigDivisionList::iterator iter = m_pVenue->GetDivisions().begin();
			iter != m_pVenue->GetDivisions().end();
			++iter)
		{
			ARBConfigDivision* pDiv = (*iter);
			for (ARBConfigLevelList::iterator iterLevel = pDiv->GetLevels().begin();
				iterLevel != pDiv->GetLevels().end();
				++iterLevel)
			{
				if (CB_ERR == m_ctrlLevel.FindStringExact(0, (*iterLevel)->GetName().c_str()))
				{
					index = m_ctrlLevel.AddString((*iterLevel)->GetName().c_str());
					if (pScoring->GetLevel() == (*iterLevel)->GetName())
						m_ctrlLevel.SetCurSel(index);
				}
			}
		}
	}
	else if (0 < idxDiv)
	{
		ARBConfigDivision* pDiv = reinterpret_cast<ARBConfigDivision*>(m_ctrlDivision.GetItemDataPtr(idxDiv));
		for (ARBConfigLevelList::iterator iterLevel = pDiv->GetLevels().begin();
			iterLevel != pDiv->GetLevels().end();
			++iterLevel)
		{
			index = m_ctrlLevel.AddString((*iterLevel)->GetName().c_str());
			if (pScoring->GetLevel() == (*iterLevel)->GetName())
				m_ctrlLevel.SetCurSel(index);
		}
	}
}

void CDlgConfigEvent::FillRequiredPoints()
{
	int idxType = m_ctrlType.GetCurSel();
	if (CB_ERR == idxType)
	{
		m_ctrlPointsOpeningText.ShowWindow(SW_HIDE);
		m_ctrlPointsOpening.ShowWindow(SW_HIDE);
		m_ctrlPointsClosingText.ShowWindow(SW_HIDE);
		m_ctrlPointsClosing.ShowWindow(SW_HIDE);
	}
	else
	{
		ARBConfigScoring* pScoring = NULL;
		int idxMethod = m_ctrlMethods.GetCurSel();
		// If this isn't set, we've got serious problems!
		if (LB_ERR != idxMethod)
			pScoring = reinterpret_cast<ARBConfigScoring*>(m_ctrlMethods.GetItemDataPtr(idxMethod));
		switch (static_cast<ARBConfigScoring::ScoringStyle>(m_ctrlType.GetItemData(idxType)))
		{
		default:
			m_ctrlPointsOpeningText.ShowWindow(SW_HIDE);
			m_ctrlPointsOpening.ShowWindow(SW_HIDE);
			m_ctrlPointsClosingText.ShowWindow(SW_HIDE);
			m_ctrlPointsClosing.ShowWindow(SW_HIDE);
			break;
		case ARBConfigScoring::eOCScoreThenTime:
			m_ctrlPointsOpeningText.SetWindowText(m_strOpening[0]);
			m_ctrlPointsOpeningText.ShowWindow(SW_SHOW);
			m_ctrlPointsOpening.ShowWindow(SW_SHOW);
			m_ctrlPointsClosingText.ShowWindow(SW_SHOW);
			m_ctrlPointsClosing.ShowWindow(SW_SHOW);
			m_OpeningPts = pScoring->GetRequiredOpeningPoints();
			m_ClosingPts = pScoring->GetRequiredClosingPoints();
			UpdateData(FALSE);
			break;
		case ARBConfigScoring::eScoreThenTime:
			m_ctrlPointsOpeningText.SetWindowText(m_strOpening[1]);
			m_ctrlPointsOpeningText.ShowWindow(SW_SHOW);
			m_ctrlPointsOpening.ShowWindow(SW_SHOW);
			m_ctrlPointsClosingText.ShowWindow(SW_HIDE);
			m_ctrlPointsClosing.ShowWindow(SW_HIDE);
			m_OpeningPts = pScoring->GetRequiredOpeningPoints();
			UpdateData(FALSE);
			break;
		}
	}
}

void CDlgConfigEvent::FillTitlePoints(ARBConfigScoring* pScoring)
{
	m_ctrlTitleList.ResetContent();
	for (ARBConfigTitlePointsList::const_iterator iter = pScoring->GetTitlePoints().begin();
		iter != pScoring->GetTitlePoints().end();
		++iter)
	{
		ARBConfigTitlePoints* pTitle = (*iter);
		int idx = m_ctrlTitleList.AddString(pTitle->GetGenericName().c_str());
		if (LB_ERR != idx)
			m_ctrlTitleList.SetItemDataPtr(idx, pTitle);
	}
	m_ctrlTitleEdit.EnableWindow(FALSE);
	m_ctrlTitleDelete.EnableWindow(FALSE);
}

bool CDlgConfigEvent::SaveControls()
{
	// Save the last selected method.
	int idxDiv = m_ctrlDivision.GetCurSel();
	int idxLevel = m_ctrlLevel.GetCurSel();
	int idxType = m_ctrlType.GetCurSel();
	if (0 <= m_idxMethod
	&& LB_ERR != idxDiv
	&& LB_ERR != idxLevel
	&& LB_ERR != idxType)
	{
		CString str;
		ARBConfigScoring* pScoring = reinterpret_cast<ARBConfigScoring*>(m_ctrlMethods.GetItemDataPtr(m_idxMethod));
		ARBDate valid;
		if (m_ctrlValidFrom.GetCheck())
		{
			CTime time;
			if (GDT_VALID == m_ctrlDateFrom.GetTime(time))
				valid = ARBDate(time.GetYear(), time.GetMonth(), time.GetDay());
		}
		pScoring->SetValidFrom(valid);
		valid.clear();
		if (m_ctrlValidTo.GetCheck())
		{
			CTime time;
			if (GDT_VALID == m_ctrlDateTo.GetTime(time))
				valid = ARBDate(time.GetYear(), time.GetMonth(), time.GetDay());
		}
		pScoring->SetValidTo(valid);
		if (0 == idxDiv)
			str = WILDCARD_DIVISION;
		else
			m_ctrlDivision.GetLBText(idxDiv, str);
		pScoring->SetDivision((LPCSTR)str);
		if (0 == idxLevel)
			str = WILDCARD_LEVEL;
		else
			m_ctrlLevel.GetLBText(idxLevel, str);
		pScoring->SetLevel((LPCSTR)str);
		ARBConfigScoring::ScoringStyle style = static_cast<ARBConfigScoring::ScoringStyle>(m_ctrlType.GetItemData(idxType));
		pScoring->SetScoringStyle(style);
		if (m_ctrlDropFractions.GetCheck())
			pScoring->SetDropFractions(true);
		else
			pScoring->SetDropFractions(false);
		if (ARBConfigScoring::eOCScoreThenTime == style)
		{
			pScoring->SetRequiredOpeningPoints(m_OpeningPts);
			pScoring->SetRequiredClosingPoints(m_ClosingPts);
		}
		else if (ARBConfigScoring::eScoreThenTime == style)
		{
			pScoring->SetRequiredOpeningPoints(m_OpeningPts);
		}
		m_ctrlNote.GetWindowText(str);
		str.Replace("\r\n", "\n");
		str.TrimRight();
		pScoring->SetNote((LPCSTR)str);
		if (m_ctrlSuperQ.GetCheck())
			pScoring->SetHasSuperQ(true);
		else
			pScoring->SetHasSuperQ(false);
		if (m_ctrlMachPts.GetCheck())
			pScoring->SetHasMachPts(true);
		else
			pScoring->SetHasMachPts(false);
		if (m_ctrlDoubleQ.GetCheck())
			pScoring->SetHasDoubleQ(true);
		else
			pScoring->SetHasDoubleQ(false);
		// Point/faults are already up-to-date.
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigEvent message handlers

BOOL CDlgConfigEvent::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctrlPointsOpeningText.GetWindowText(m_strOpening[0]);
	m_strOpening[1].LoadString(IDS_SCORING_REQUIRED_POINTS);

	// If any additional types are added in ARBConfigScoring,
	// they'll have to be manually added here...
	static const ARBConfigScoring::ScoringStyle Styles[] =
	{
		ARBConfigScoring::eFaultsThenTime,
		ARBConfigScoring::eFaults100ThenTime,
		ARBConfigScoring::eFaults200ThenTime,
		ARBConfigScoring::eOCScoreThenTime,
		ARBConfigScoring::eScoreThenTime,
		ARBConfigScoring::eTimePlusFaults
	};
	static const int nStyles = sizeof(Styles) / sizeof(Styles[0]);
	for (int index = 0; index < nStyles; ++index)
	{
		std::string str = ARBConfigScoring::GetScoringStyleStr(Styles[index]);
		int idx = m_ctrlType.AddString(str.c_str());
		m_ctrlType.SetItemData(idx, Styles[index]);
	}

	FillMethodList();
	FillControls();
	FillRequiredPoints();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgConfigEvent::OnLbnSelchangeMethods()
{
	UpdateData(TRUE);
	SaveControls();
	m_idxMethod = m_ctrlMethods.GetCurSel();
	FillControls();
}

void CDlgConfigEvent::OnCbnSelchangeDivision()
{
	UpdateData(TRUE);
	SaveControls();
	FillLevelList();
	FillMethodList();
}

void CDlgConfigEvent::OnCbnSelchangeLevel()
{
	UpdateData(TRUE);
	SaveControls();
	FillMethodList();
}

void CDlgConfigEvent::OnBnClickedNew()
{
	UpdateData(TRUE);
	SaveControls();
	ARBConfigScoring* pScoring = m_Scorings.AddScoring();
	CString str = GetListName(pScoring);
	m_idxMethod = m_ctrlMethods.AddString(str);
	m_ctrlMethods.SetItemDataPtr(m_idxMethod, reinterpret_cast<void*>(pScoring));
	m_ctrlMethods.SetCurSel(m_idxMethod);
	FillMethodList();
	FillControls();
}

void CDlgConfigEvent::OnBnClickedCopy()
{
	UpdateData(TRUE);
	int idxMethod = m_ctrlMethods.GetCurSel();
	if (LB_ERR != idxMethod)
	{
		ARBConfigScoring* pScoring = reinterpret_cast<ARBConfigScoring*>(m_ctrlMethods.GetItemDataPtr(idxMethod));
		ARBConfigScoring* pNewScoring = m_Scorings.AddScoring();
		*pNewScoring = *pScoring;
		CString str = GetListName(pNewScoring);
		m_idxMethod = m_ctrlMethods.AddString(str);
		m_ctrlMethods.SetItemDataPtr(m_idxMethod, pNewScoring);
		m_ctrlMethods.SetCurSel(m_idxMethod);
		FillMethodList();
		FillControls();
	}
}

void CDlgConfigEvent::OnBnClickedDelete()
{
	UpdateData(TRUE);
	int idxMethod = m_ctrlMethods.GetCurSel();
	if (LB_ERR != idxMethod)
	{
		ARBConfigScoring* pScoring = reinterpret_cast<ARBConfigScoring*>(m_ctrlMethods.GetItemDataPtr(idxMethod));
		for (ARBConfigScoringList::iterator iter = m_Scorings.begin();
			iter != m_Scorings.end();
			++iter)
		{
			if ((*iter) == pScoring)
			{
				m_Scorings.erase(iter);
				break;
			}
		}
		FillMethodList();
		FillControls();
	}
}

void CDlgConfigEvent::OnBnClickedUp()
{
	UpdateData(TRUE);
	int idxMethod = m_ctrlMethods.GetCurSel();
	if (LB_ERR != idxMethod && 0 != idxMethod)
	{
		ARBConfigScoring* pScoring = reinterpret_cast<ARBConfigScoring*>(m_ctrlMethods.GetItemDataPtr(idxMethod));
		for (ARBConfigScoringList::iterator iter = m_Scorings.begin();
			iter != m_Scorings.end();
			++iter)
		{
			if ((*iter) == pScoring)
			{
				pScoring->AddRef();
				iter = m_Scorings.erase(iter);
				--iter;
				m_Scorings.insert(iter, pScoring);
				m_ctrlMethods.SetCurSel(idxMethod-1);
				break;
			}
		}
		FillMethodList();
		FillControls();
	}
}

void CDlgConfigEvent::OnBnClickedDown()
{
	UpdateData(TRUE);
	int idxMethod = m_ctrlMethods.GetCurSel();
	if (LB_ERR != idxMethod && idxMethod < m_ctrlMethods.GetCount() - 1)
	{
		ARBConfigScoring* pScoring = reinterpret_cast<ARBConfigScoring*>(m_ctrlMethods.GetItemDataPtr(idxMethod));
		for (ARBConfigScoringList::iterator iter = m_Scorings.begin();
			iter != m_Scorings.end();
			++iter)
		{
			if ((*iter) == pScoring)
			{
				pScoring->AddRef();
				iter = m_Scorings.erase(iter);
				++iter;
				m_Scorings.insert(iter, pScoring);
				m_ctrlMethods.SetCurSel(idxMethod+1);
				break;
			}
		}
		FillMethodList();
		FillControls();
	}
}

void CDlgConfigEvent::OnValidFrom() 
{
	UpdateData(TRUE);
	BOOL bEnable = FALSE;
	if (m_ctrlValidFrom.GetCheck())
		bEnable = TRUE;
	m_ctrlDateFrom.EnableWindow(bEnable);
	SaveControls();
	FillMethodList();
}

void CDlgConfigEvent::OnDatetimechangeDate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SaveControls();
	FillMethodList();
	*pResult = 0;
}

void CDlgConfigEvent::OnValidTo() 
{
	UpdateData(TRUE);
	BOOL bEnable = FALSE;
	if (m_ctrlValidTo.GetCheck())
		bEnable = TRUE;
	m_ctrlDateTo.EnableWindow(bEnable);
	SaveControls();
	FillMethodList();
}

void CDlgConfigEvent::OnSelchangeType() 
{
	UpdateData(TRUE);
	FillRequiredPoints();
}

void CDlgConfigEvent::OnSelchangeList() 
{
	UpdateData(TRUE);
	BOOL bEnable = FALSE;
	if (LB_ERR != m_ctrlTitleList.GetCurSel())
		bEnable = TRUE;
	m_ctrlTitleEdit.EnableWindow(bEnable);
	m_ctrlTitleDelete.EnableWindow(bEnable);
}

void CDlgConfigEvent::OnDblclkList() 
{
	UpdateData(TRUE);
	OnTitleEdit();
}

void CDlgConfigEvent::OnTitleNew() 
{
	UpdateData(TRUE);
	int idxMethod = m_ctrlMethods.GetCurSel();
	if (LB_ERR != idxMethod)
	{
		ARBConfigScoring* pScoring = reinterpret_cast<ARBConfigScoring*>(m_ctrlMethods.GetItemDataPtr(idxMethod));
		if (pScoring)
		{
			CDlgConfigTitlePoints dlg(pScoring->GetTitlePoints(), NULL, this);
			if (IDOK == dlg.DoModal())
				FillTitlePoints(pScoring);
		}
	}
}

void CDlgConfigEvent::OnTitleEdit() 
{
	UpdateData(TRUE);
	int idxMethod = m_ctrlMethods.GetCurSel();
	int idx = m_ctrlTitleList.GetCurSel();
	if (LB_ERR != idxMethod && LB_ERR != idx)
	{
		ARBConfigScoring* pScoring = reinterpret_cast<ARBConfigScoring*>(m_ctrlMethods.GetItemDataPtr(idxMethod));
		ARBConfigTitlePoints* pTitle = reinterpret_cast<ARBConfigTitlePoints*>(m_ctrlTitleList.GetItemDataPtr(idx));
		if (pScoring && pTitle)
		{
			CDlgConfigTitlePoints dlg(pScoring->GetTitlePoints(), pTitle, this);
			if (IDOK == dlg.DoModal())
				FillTitlePoints(pScoring);
		}
	}
}

void CDlgConfigEvent::OnTitleDelete()
{
	UpdateData(TRUE);
	int idxMethod = m_ctrlMethods.GetCurSel();
	int idx = m_ctrlTitleList.GetCurSel();
	if (LB_ERR != idxMethod && LB_ERR != idx)
	{
		ARBConfigScoring* pScoring = reinterpret_cast<ARBConfigScoring*>(m_ctrlMethods.GetItemDataPtr(idxMethod));
		ARBConfigTitlePoints* pTitle = reinterpret_cast<ARBConfigTitlePoints*>(m_ctrlTitleList.GetItemDataPtr(idx));
		if (pScoring && pTitle)
		{
			pScoring->GetTitlePoints().DeleteTitlePoints(pTitle->GetFaults());
			m_ctrlTitleList.DeleteString(idx);
			m_ctrlTitleEdit.EnableWindow(FALSE);
			m_ctrlTitleDelete.EnableWindow(FALSE);
		}
	}
}

void CDlgConfigEvent::OnOK()
{
	if (!UpdateData(TRUE))
		return;
	if (!SaveControls())
		return;
#if _MSC_VER < 1300
	m_Name.TrimRight();
	m_Name.TrimLeft();
	m_Desc.TrimRight();
	m_Desc.TrimLeft();
#else
	m_Name.Trim();
	m_Desc.Trim();
#endif
	if (m_Name.IsEmpty())
	{
		GotoDlgCtrl(GetDlgItem(IDC_NAME));
		return;
	}
	// Validate that from-to dates are okay.
	int index;
	for (index = 0; index < m_ctrlMethods.GetCount(); ++index)
	{
		CString str;
		const ARBConfigScoring* pScoring = reinterpret_cast<ARBConfigScoring*>(m_ctrlMethods.GetItemDataPtr(index));
		ARBDate validFrom = pScoring->GetValidFrom();
		ARBDate validTo = pScoring->GetValidTo();
		if (validFrom.IsValid() && validTo.IsValid()
		&& validFrom > validTo)
		{
			m_ctrlMethods.SetCurSel(index);
			m_idxMethod = index;
			FillControls();
			AfxMessageBox("Valid From date is before the Valid To date");
			GotoDlgCtrl(&m_ctrlDateTo);
			return;
		}
	}
	// Check if there is any overlap.
	bool bOverlap = false;
	{
		ARBConfigScoringList scorings = m_Scorings;
		while (!bOverlap && 1 < scorings.size())
		{
			// Extract all similar methods.
			ARBConfigScoringList items;
			ARBConfigScoringList::iterator iter = scorings.begin();
			ARBConfigScoring* pScoring = items.AddScoring();
			*pScoring = *(*iter);
			iter = scorings.erase(scorings.begin());
			while (iter != scorings.end())
			{
				ARBConfigScoring* pScoring2 = *iter;
				if (pScoring->GetDivision() == pScoring2->GetDivision()
				&& pScoring->GetLevel() == pScoring2->GetLevel())
				{
					pScoring2 = items.AddScoring();
					*pScoring2 = *(*iter);
					iter = scorings.erase(iter);
				}
				else
					++iter;
			}
			// If we have more than one, check overlap.
			while (!bOverlap && 1 < items.size())
			{
				iter = items.begin();
				const ARBConfigScoring* pScoring = *iter;
				for (++iter; !bOverlap && iter != items.end(); ++iter)
				{
					const ARBConfigScoring* pScoring2 = *iter;
					if ((!pScoring->GetValidFrom().IsValid()
					&& !pScoring2->GetValidFrom().IsValid())
					|| (!pScoring->GetValidTo().IsValid()
					&& !pScoring2->GetValidTo().IsValid()))
						bOverlap = true;
					if ((pScoring->GetValidTo().IsValid() && pScoring2->GetValidFrom().IsValid()
					&& pScoring->GetValidTo() >= pScoring2->GetValidFrom())
					|| (pScoring2->GetValidTo().IsValid() && pScoring->GetValidFrom().IsValid()
					&& pScoring2->GetValidTo() >= pScoring->GetValidFrom()))
						bOverlap = true;
				}
				items.erase(items.begin());
			}
		}
	}
	if (bOverlap)
	{
		if (IDYES != AfxMessageBox("Warning: Scoring methods have overlapping from/to dates. When searching for the scoring method for a particular run, the first method found will be returned, ignoring the others.\n\nAre you sure you want to save this?", MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2))
			return;
	}

	if (m_pEvent->GetName() != (LPCSTR)m_Name)
	{
		if (m_pVenue->GetEvents().FindEvent((LPCSTR)m_Name))
		{
			AfxMessageBox(IDS_NAME_IN_USE);
			GotoDlgCtrl(GetDlgItem(IDC_NAME));
			return;
		}
		m_pEvent->SetName((LPCSTR)m_Name);
	}
	m_Desc.Replace("\r\n", "\n");
	m_pEvent->SetDesc((LPCSTR)m_Desc);
	m_pEvent->SetHasPartner(m_bHasPartners == TRUE ? true : false);
	m_pEvent->GetScorings() = m_Scorings;
	CDialog::OnOK();
}
