/*
 * Copyright � 2002-2004 David Connet. All Rights Reserved.
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
 * @li 2004-04-01 DRC Fixed? the memory access fault.
 * @li 2003-12-27 DRC Added support for from/to dates for the scoring method.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgConfigEvent.h"

#include <algorithm>
#include "ARBAgilityRecordBook.h"
#include "ARBConfigEvent.h"
#include "ARBConfigVenue.h"
#include "DlgConfigTitlePoints.h"
#include "DlgConfigure.h"
#include "DlgFixup.h"
#include "DlgName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigEvent dialog

CDlgConfigEvent::CDlgConfigEvent(CAgilityBookDoc* pDoc,
	ARBAgilityRecordBook* book, ARBConfig* config,
	ARBConfigVenue* pVenue, ARBConfigEvent* pEvent, CWnd* pParent)
	: CDlgBaseDialog(CDlgConfigEvent::IDD, pParent)
	, m_pDoc(pDoc)
	, m_Book(book)
	, m_Config(config)
	, m_pVenue(pVenue)
	, m_pEvent(pEvent)
	, m_Scorings(pEvent->GetScorings())
	, m_idxMethod(-1)
	, m_bHasTable(FALSE)
	, m_bHasPartners(FALSE)
{
	ASSERT(m_pVenue);
	ASSERT(m_pEvent);
	m_Name = m_pEvent->GetName().c_str();
	m_Desc = m_pEvent->GetDesc().c_str();
	m_Desc.Replace("\n", "\r\n");
	m_bHasTable = m_pEvent->HasTable() ? TRUE : FALSE;
	m_bHasPartners = m_pEvent->HasPartner() ? TRUE : FALSE;
	//{{AFX_DATA_INIT(CDlgConfigEvent)
	m_OpeningPts = 0;
	m_ClosingPts = 0;
	//}}AFX_DATA_INIT
}

CDlgConfigEvent::~CDlgConfigEvent()
{
	ClearFixups();
}

void CDlgConfigEvent::GetFixups(std::vector<CDlgFixup*>& ioFixups)
{
	for (std::vector<CDlgFixup*>::iterator iter = m_DlgFixup.begin(); iter != m_DlgFixup.end(); ++iter)
		ioFixups.push_back(*iter);
	m_DlgFixup.clear();
}

void CDlgConfigEvent::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConfigEvent)
	DDX_Text(pDX, IDC_CONFIG_EVENT, m_Name);
	DDX_Check(pDX, IDC_CONFIG_EVENT_TABLE, m_bHasTable);
	DDX_Check(pDX, IDC_CONFIG_EVENT_PARTNER, m_bHasPartners);
	DDX_Control(pDX, IDC_CONFIG_EVENT_SUBCAT, m_ctrlSubCat);
	DDX_Control(pDX, IDC_CONFIG_EVENT_SUBCAT_NEW, m_ctrlSubCatNew);
	DDX_Control(pDX, IDC_CONFIG_EVENT_SUBCAT_EDIT, m_ctrlSubCatEdit);
	DDX_Control(pDX, IDC_CONFIG_EVENT_SUBCAT_DELETE, m_ctrlSubCatDelete);
	DDX_Text(pDX, IDC_CONFIG_EVENT_DESC, m_Desc);
	DDX_Control(pDX, IDC_CONFIG_EVENT_NEW, m_ctrlNew);
	DDX_Control(pDX, IDC_CONFIG_EVENT_COPY, m_ctrlCopy);
	DDX_Control(pDX, IDC_CONFIG_EVENT_DELETE, m_ctrlDelete);
	DDX_Control(pDX, IDC_CONFIG_EVENT_MOVE_UP, m_ctrlUp);
	DDX_Control(pDX, IDC_CONFIG_EVENT_MOVE_DOWN, m_ctrlDown);
	DDX_Control(pDX, IDC_CONFIG_EVENT_METHODS, m_ctrlMethods);
	DDX_Control(pDX, IDC_CONFIG_EVENT_UNUSED, m_ctrlUnused);
	DDX_Control(pDX, IDC_CONFIG_EVENT_DATE_VALID_FROM, m_ctrlValidFrom);
	DDX_Control(pDX, IDC_CONFIG_EVENT_DATE_START, m_ctrlDateFrom);
	DDX_Control(pDX, IDC_CONFIG_EVENT_DATE_VALID_TO, m_ctrlValidTo);
	DDX_Control(pDX, IDC_CONFIG_EVENT_DATE_END, m_ctrlDateTo);
	DDX_Control(pDX, IDC_CONFIG_EVENT_DIVISION, m_ctrlDivision);
	DDX_Control(pDX, IDC_CONFIG_EVENT_LEVEL, m_ctrlLevel);
	DDX_Control(pDX, IDC_CONFIG_EVENT_TYPE, m_ctrlType);
	DDX_Control(pDX, IDC_CONFIG_EVENT_DROP_FRACTIONS, m_ctrlDropFractions);
	DDX_Control(pDX, IDC_CONFIG_EVENT_OPENING_PTS_TEXT, m_ctrlPointsOpeningText);
	DDX_Control(pDX, IDC_CONFIG_EVENT_OPENING_PTS, m_ctrlPointsOpening);
	DDX_Text(pDX, IDC_CONFIG_EVENT_OPENING_PTS, m_OpeningPts);
	DDX_Control(pDX, IDC_CONFIG_EVENT_CLOSING_PTS_TEXT, m_ctrlPointsClosingText);
	DDX_Control(pDX, IDC_CONFIG_EVENT_CLOSING_PTS, m_ctrlPointsClosing);
	DDX_Text(pDX, IDC_CONFIG_EVENT_CLOSING_PTS, m_ClosingPts);
	DDX_Control(pDX, IDC_CONFIG_EVENT_NOTES, m_ctrlNote);
	DDX_Control(pDX, IDC_CONFIG_EVENT_SUPERQ, m_ctrlSuperQ);
	DDX_Control(pDX, IDC_CONFIG_EVENT_MACH, m_ctrlMachPts);
	DDX_Control(pDX, IDC_CONFIG_EVENT_DOUBLEQ, m_ctrlDoubleQ);
	DDX_Control(pDX, IDC_CONFIG_EVENT_POINTS, m_ctrlPointsList);
	DDX_Control(pDX, IDC_CONFIG_EVENT_POINTS_NEW, m_ctrlPointsNew);
	DDX_Control(pDX, IDC_CONFIG_EVENT_POINTS_EDIT, m_ctrlPointsEdit);
	DDX_Control(pDX, IDC_CONFIG_EVENT_POINTS_DELETE, m_ctrlPointsDelete);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgConfigEvent, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgConfigEvent)
	ON_LBN_SELCHANGE(IDC_CONFIG_EVENT_SUBCAT, OnSelchangeSubCat)
	ON_LBN_DBLCLK(IDC_CONFIG_EVENT_SUBCAT, OnDblclkSubCat)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_SUBCAT_NEW, OnSubCatNew)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_SUBCAT_EDIT, OnSubCatEdit)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_SUBCAT_DELETE, OnSubCatDelete)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_NEW, OnBnClickedNew)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_COPY, OnBnClickedCopy)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_DELETE, OnBnClickedDelete)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_MOVE_UP, OnBnClickedUp)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_MOVE_DOWN, OnBnClickedDown)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_DATE_VALID_FROM, OnValidFrom)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_DATE_VALID_TO, OnValidTo)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_CONFIG_EVENT_DATE_START, OnDatetimechangeDate)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_CONFIG_EVENT_DATE_END, OnDatetimechangeDate)
	ON_LBN_SELCHANGE(IDC_CONFIG_EVENT_METHODS, OnLbnSelchangeMethods)
	ON_CBN_SELCHANGE(IDC_CONFIG_EVENT_DIVISION, OnCbnSelchangeDivision)
	ON_CBN_SELCHANGE(IDC_CONFIG_EVENT_LEVEL, OnCbnSelchangeLevel)
	ON_CBN_SELCHANGE(IDC_CONFIG_EVENT_TYPE, OnSelchangeType)
	ON_LBN_SELCHANGE(IDC_CONFIG_EVENT_POINTS, OnSelchangePoints)
	ON_LBN_DBLCLK(IDC_CONFIG_EVENT_POINTS, OnDblclkPoints)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_POINTS_NEW, OnPointsNew)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_POINTS_EDIT, OnPointsEdit)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_POINTS_DELETE, OnPointsDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgConfigEvent::ClearFixups()
{
	for (std::vector<CDlgFixup*>::iterator iter = m_DlgFixup.begin(); iter != m_DlgFixup.end(); ++iter)
		delete (*iter);
	m_DlgFixup.clear();
}

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
		m_ctrlPointsList.ResetContent();
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
	m_ctrlPointsList.EnableWindow(bEnable);
	m_ctrlPointsNew.EnableWindow(bEnable);
	m_ctrlPointsEdit.EnableWindow(bEnable);
	m_ctrlPointsDelete.EnableWindow(bEnable);
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
	ARBBase* pOld = NULL;
	int idxTitle = m_ctrlPointsList.GetCurSel();
	if (LB_ERR != idxTitle)
		pOld = reinterpret_cast<ARBBase*>(m_ctrlPointsList.GetItemDataPtr(idxTitle));
	m_ctrlPointsList.ResetContent();
	for (ARBConfigTitlePointsList::const_iterator iter = pScoring->GetTitlePoints().begin();
		iter != pScoring->GetTitlePoints().end();
		++iter)
	{
		ARBConfigTitlePoints* pTitle = (*iter);
		int idx = m_ctrlPointsList.AddString(pTitle->GetGenericName().c_str());
		if (LB_ERR != idx)
		{
			m_ctrlPointsList.SetItemDataPtr(idx, pTitle);
			if (pOld == pTitle)
				m_ctrlPointsList.SetCurSel(idx);
		}
	}
	for (ARBConfigLifetimePointsList::const_iterator iter2 = pScoring->GetLifetimePoints().begin();
		iter2 != pScoring->GetLifetimePoints().end();
		++iter2)
	{
		ARBConfigLifetimePoints* pLife = (*iter2);
		int idx = m_ctrlPointsList.AddString(pLife->GetGenericName().c_str());
		if (LB_ERR != idx)
		{
			m_ctrlPointsList.SetItemDataPtr(idx, pLife);
			if (pOld == pLife)
				m_ctrlPointsList.SetCurSel(idx);
		}
	}
	m_ctrlPointsEdit.EnableWindow(FALSE);
	m_ctrlPointsDelete.EnableWindow(FALSE);
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
	CDlgBaseDialog::OnInitDialog();

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
	for (int index = 0; index < nStyles; ++index)
	{
		std::string str = ARBConfigScoring::GetScoringStyleStr(Styles[index]);
		int idx = m_ctrlType.AddString(str.c_str());
		m_ctrlType.SetItemData(idx, Styles[index]);
	}

	// Fill subcategories.
	for (std::vector<std::string>::iterator iter = m_pEvent->GetSubCategories().begin();
		iter != m_pEvent->GetSubCategories().end();
		++iter)
	{
		m_ctrlSubCat.AddString((*iter).c_str());
	}
	m_ctrlSubCatEdit.EnableWindow(FALSE);
	m_ctrlSubCatDelete.EnableWindow(FALSE);

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

void CDlgConfigEvent::OnSelchangeSubCat()
{
	UpdateData(TRUE);
	BOOL bEnable = FALSE;
	if (LB_ERR != m_ctrlSubCat.GetCurSel())
		bEnable = TRUE;
	m_ctrlSubCatEdit.EnableWindow(bEnable);
	m_ctrlSubCatDelete.EnableWindow(bEnable);
}

void CDlgConfigEvent::OnDblclkSubCat()
{
	UpdateData(TRUE);
	OnSubCatEdit();
}

void CDlgConfigEvent::OnSubCatNew()
{
	CDlgName dlg("", (LPCTSTR)NULL, this);
	if (IDOK == dlg.DoModal())
	{
		std::vector<std::string> subcat;
		for (int index = 0; index < m_ctrlSubCat.GetCount(); ++index)
		{
			CString text;
			m_ctrlSubCat.GetText(index, text);
			subcat.push_back((LPCSTR)text);
		}
		std::string item = dlg.GetName();
		if (subcat.end() == find(subcat.begin(), subcat.end(), item))
		{
			int idx = m_ctrlSubCat.AddString(item.c_str());
			m_ctrlSubCat.SetCurSel(idx);
		}
	}
}

void CDlgConfigEvent::OnSubCatEdit()
{
	int idx = m_ctrlSubCat.GetCurSel();
	if (LB_ERR != idx)
	{
		CString text;
		m_ctrlSubCat.GetText(idx, text);
		CDlgName dlg(text, (LPCTSTR)NULL, this);
		if (IDOK == dlg.DoModal())
		{
			CString oldText;
			m_ctrlSubCat.GetText(idx, oldText);
			m_ctrlSubCat.DeleteString(idx);
			std::vector<std::string> subcat;
			for (int index = 0; index < m_ctrlSubCat.GetCount(); ++index)
			{
				CString text;
				m_ctrlSubCat.GetText(index, text);
				subcat.push_back((LPCSTR)text);
			}
			std::string item = dlg.GetName();
			if (subcat.end() == find(subcat.begin(), subcat.end(), item))
			{
				int idx = m_ctrlSubCat.AddString(item.c_str());
				m_ctrlSubCat.SetCurSel(idx);
			}
			else
			{
				// New name exists, put the old one back.
				int idx = m_ctrlSubCat.AddString(oldText);
				m_ctrlSubCat.SetCurSel(idx);
			}
		}
	}
}

void CDlgConfigEvent::OnSubCatDelete()
{
	int idx = m_ctrlSubCat.GetCurSel();
	if (LB_ERR != idx)
		m_ctrlSubCat.DeleteString(idx);
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

void CDlgConfigEvent::OnSelchangePoints() 
{
	UpdateData(TRUE);
	BOOL bEnable = FALSE;
	if (LB_ERR != m_ctrlPointsList.GetCurSel())
		bEnable = TRUE;
	m_ctrlPointsEdit.EnableWindow(bEnable);
	m_ctrlPointsDelete.EnableWindow(bEnable);
}

void CDlgConfigEvent::OnDblclkPoints() 
{
	UpdateData(TRUE);
	OnPointsEdit();
}

void CDlgConfigEvent::OnPointsNew() 
{
	UpdateData(TRUE);
	int idxMethod = m_ctrlMethods.GetCurSel();
	if (LB_ERR != idxMethod)
	{
		ARBConfigScoring* pScoring = reinterpret_cast<ARBConfigScoring*>(m_ctrlMethods.GetItemDataPtr(idxMethod));
		if (pScoring)
		{
			CDlgConfigTitlePoints dlg(0, 0, false, this);
			if (IDOK == dlg.DoModal())
			{
				// The only reason this fails is if the faults entry exists.
				if (dlg.IsLifetime())
				{
					if (!pScoring->GetLifetimePoints().AddLifetimePoints(dlg.GetPoints(), dlg.GetFaults()))
						AfxMessageBox(IDS_TITLEPTS_EXISTS, MB_ICONEXCLAMATION);
				}
				else
				{
					if (!pScoring->GetTitlePoints().AddTitlePoints(dlg.GetPoints(), dlg.GetFaults()))
						AfxMessageBox(IDS_TITLEPTS_EXISTS, MB_ICONEXCLAMATION);
				}
				FillTitlePoints(pScoring);
			}
		}
	}
}

void CDlgConfigEvent::OnPointsEdit() 
{
	UpdateData(TRUE);
	int idxMethod = m_ctrlMethods.GetCurSel();
	int idx = m_ctrlPointsList.GetCurSel();
	if (LB_ERR != idxMethod && LB_ERR != idx)
	{
		ARBConfigScoring* pScoring = reinterpret_cast<ARBConfigScoring*>(m_ctrlMethods.GetItemDataPtr(idxMethod));
		ARBBase* pBase = reinterpret_cast<ARBBase*>(m_ctrlPointsList.GetItemDataPtr(idx));
		ARBConfigTitlePoints* pTitle = dynamic_cast<ARBConfigTitlePoints*>(pBase);
		ARBConfigLifetimePoints* pLife = dynamic_cast<ARBConfigLifetimePoints*>(pBase);
		if (pScoring && (pTitle || pLife))
		{
			short pts, faults;
			BOOL bLifetime;
			if (pTitle)
			{
				pts = pTitle->GetPoints();
				faults = pTitle->GetFaults();
				bLifetime = FALSE;
			}
			else
			{
				pts = pLife->GetPoints();
				faults = pLife->GetFaults();
				bLifetime = TRUE;
			}
			CDlgConfigTitlePoints dlg(pts, faults, bLifetime);
			if (IDOK == dlg.DoModal())
			{
				if (bLifetime != dlg.IsLifetime())
				{
					// Clean up.
					if (pTitle)
						pScoring->GetTitlePoints().DeleteTitlePoints(pTitle->GetFaults());
					else
						pScoring->GetLifetimePoints().DeleteLifetimePoints(pLife->GetFaults());
					if (bLifetime)
						pScoring->GetLifetimePoints().AddLifetimePoints(dlg.GetPoints(), dlg.GetFaults());
					else
						pScoring->GetTitlePoints().AddTitlePoints(dlg.GetPoints(), dlg.GetFaults());
				}
				else
				{
					if (pTitle)
					{
						pTitle->SetPoints(dlg.GetPoints());
						pTitle->SetFaults(dlg.GetFaults());
					}
					else
					{
						pLife->SetPoints(dlg.GetPoints());
						pLife->SetFaults(dlg.GetFaults());
					}
				}
				if (pTitle)
					pScoring->GetTitlePoints().sort();
				else
					pScoring->GetLifetimePoints().sort();
				FillTitlePoints(pScoring);
			}
		}
	}
}

void CDlgConfigEvent::OnPointsDelete()
{
	UpdateData(TRUE);
	int idxMethod = m_ctrlMethods.GetCurSel();
	int idx = m_ctrlPointsList.GetCurSel();
	if (LB_ERR != idxMethod && LB_ERR != idx)
	{
		ARBConfigScoring* pScoring = reinterpret_cast<ARBConfigScoring*>(m_ctrlMethods.GetItemDataPtr(idxMethod));
		ARBBase* pBase = reinterpret_cast<ARBBase*>(m_ctrlPointsList.GetItemDataPtr(idx));
		ARBConfigTitlePoints* pTitle = reinterpret_cast<ARBConfigTitlePoints*>(pBase);
		ARBConfigLifetimePoints* pLife = reinterpret_cast<ARBConfigLifetimePoints*>(pBase);
		if (pScoring && (pTitle || pLife))
		{
			if (pTitle)
				pScoring->GetTitlePoints().DeleteTitlePoints(pTitle->GetFaults());
			else
				pScoring->GetLifetimePoints().DeleteLifetimePoints(pTitle->GetFaults());
			m_ctrlPointsList.DeleteString(idx);
			m_ctrlPointsEdit.EnableWindow(FALSE);
			m_ctrlPointsDelete.EnableWindow(FALSE);
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
		GotoDlgCtrl(GetDlgItem(IDC_CONFIG_EVENT));
		return;
	}

	// Validate that from-to dates are okay.
	int index;
	for (index = 0; index < m_ctrlMethods.GetCount(); ++index)
	{
		CString str;
		ARBConfigScoring const* pScoring = reinterpret_cast<ARBConfigScoring const*>(m_ctrlMethods.GetItemDataPtr(index));
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
				ARBConfigScoring const* pScoring = *iter;
				for (++iter; !bOverlap && iter != items.end(); ++iter)
				{
					ARBConfigScoring const* pScoring2 = *iter;
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

	ClearFixups();
	if (m_pEvent->GetName() != (LPCSTR)m_Name)
	{
		if (m_pVenue->GetEvents().FindEvent((LPCSTR)m_Name))
		{
			AfxMessageBox(IDS_NAME_IN_USE);
			GotoDlgCtrl(GetDlgItem(IDC_CONFIG_EVENT));
			return;
		}
		if (m_Book)
			m_DlgFixup.push_back(new CDlgFixupRenameEvent(m_pVenue->GetName(), (LPCSTR)m_Name, m_pEvent->GetName()));
		m_pEvent->SetName((LPCSTR)m_Name);
	}
	// m_Book and m_Config are only valid when editing an existing entry.
	if (m_Book)
	{
		switch (CDlgConfigure::CheckExistingRuns(m_pDoc, m_Book->GetDogs(),
			m_pVenue, m_pEvent->GetName(), m_Scorings, m_DlgFixup))
		{
		case CDlgConfigure::eCancelChanges:
			EndDialog(IDCANCEL);
			return;
		default:
		case CDlgConfigure::eDoNotDoIt:
			return;
		case CDlgConfigure::eNoChange:
		case CDlgConfigure::eDoIt:
			break;
		}
	}

	std::vector<std::string> subcat;
	for (index = 0; index < m_ctrlSubCat.GetCount(); ++index)
	{
		CString text;
		m_ctrlSubCat.GetText(index, text);
		std::string item = (LPCSTR)text;
		if (subcat.end() == find(subcat.begin(), subcat.end(), item))
			subcat.push_back(item);
	}

	m_Desc.Replace("\r\n", "\n");
	m_pEvent->GetSubCategories() = subcat;
	m_pEvent->SetDesc((LPCSTR)m_Desc);
	m_pEvent->SetHasTable(m_bHasTable == TRUE ? true : false);
	m_pEvent->SetHasPartner(m_bHasPartners == TRUE ? true : false);
	m_pEvent->GetScorings() = m_Scorings;
	CDlgBaseDialog::OnOK();
}
