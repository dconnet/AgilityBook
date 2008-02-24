/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgRunScore class
 * @author David Connet
 *
 * Revision History
 * @li 2007-12-03 DRC Refresh judge list after invoking 'notes' button.
 * @li 2007-07-01 DRC Fixed a problem with table flag on a run.
 * @li 2006-11-05 DRC Trim Divisions/Levels if no events are available on date.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-13 DRC Added direct access to Notes dialog.
 * @li 2005-12-04 DRC Added support for NADAC bonus titling points.
 * @li 2005-11-20 DRC Allow 'E's on non-titling runs.
 * @li 2005-08-11 DRC Removed QQ checkbox.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-02 DRC Added subnames to events.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-11-13 DRC Also compute score for NA runs that have no titling pts.
 * @li 2004-09-07 DRC Time+Fault scoring shouldn't include time faults.
 * @li 2004-03-20 DRC The date never got set if the initial entry had no date
 *                    and we didn't change it (first run in a trial).
 *                    Plus, the table-in-yps flag was backwards and didn't
 *                    properly initialize when the event type changed.
 * @li 2004-02-14 DRC Added Table-in-YPS flag.
 * @li 2004-02-09 DRC Update YPS when the time is changed.
 *                    When date changes, update controls.
 * @li 2004-01-19 DRC Total faults weren't updated when course faults changed.
 * @li 2004-01-18 DRC Calling UpdateData during data entry causes way too much
 *                    validation. Only call during OnOK (from dlgsheet)
 * @li 2003-12-27 DRC Changed FindEvent to take a date. Also, update the
 *                    controls when the date changes as the scoring config may
 *                    change.
 * @li 2003-12-09 DRC Fixed a bug where the QQ checkbox didn't get set right.
 * @li 2003-10-13 DRC Made Time/CourseFaults common to all scoring methods.
 *                    This allows faults for things like language!
 * @li 2003-09-29 DRC Required pts were being overwriten with default values
 *                    during dialog initialization.
 * @li 2003-09-01 DRC Total faults weren't being shown when there was no SCT.
 * @li 2003-08-17 DRC Title points were being computed on 'NQ' and the score was
 *                    always being computed. Fixed both.
 * @li 2003-07-14 DRC Changed 'Score' to show data on 'Q' and 'NQ'.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgRunScore.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ARBConfig.h"
#include "ARBConfigOtherPoints.h"
#include "ARBConfigVenue.h"
#include "ARBDogClub.h"
#include "ARBDogRun.h"
#include "ARBDogTrial.h"
#include "DlgInfoJudge.h"
#include "DlgListCtrl.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

class CDlgRunDataLevel : public CListData
{
public:
	CDlgRunDataLevel(ARBConfigLevelPtr pLevel)
		: m_pLevel(pLevel)
		, m_pSubLevel()
	{
	}
	CDlgRunDataLevel(
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
// CDlgRunScore dialog

CDlgRunScore::CDlgRunScore(
		CAgilityBookDoc* pDoc,
		ARBConfigVenuePtr pVenue,
		ARBDogTrialPtr pTrial,
		ARBDogRunPtr pRealRun,
		ARBDogRunPtr pRun)
	: CDlgBasePropertyPage(CDlgRunScore::IDD)
	, m_ctrlDivisions(true)
	, m_ctrlLevels(true)
	, m_ctrlEvents(false)
	, m_ctrlSubNames(false)
	, m_ctrlHeight(false)
	, m_ctrlJudge(false)
	, m_ctrlHandler(false)
	, m_ctrlQ(false)
	, m_pDoc(pDoc)
	, m_pVenue(pVenue)
	, m_pTrial(pTrial)
	, m_pRealRun(pRealRun)
	, m_Run(pRun)
{
	ASSERT(NULL != m_pVenue.get());
	ASSERT(NULL != m_pTrial.get());
	ARBDogClubPtr pClub;
	m_pTrial->GetClubs().GetPrimaryClub(&pClub);
	ASSERT(NULL != pClub.get());
	//{{AFX_DATA_INIT(CDlgRunScore)
	m_Venue = m_pVenue->GetName().c_str();
	m_Club = pClub->GetName().c_str();
	m_Location = m_pTrial->GetLocation().c_str();
	m_SubName = m_Run->GetSubName().c_str();
	m_Height = m_Run->GetHeight().c_str();
	m_Handler = _T("");
	m_Conditions = m_Run->GetConditions().c_str();
	m_SCT = 0.0;
	m_Opening = 0;
	m_Yards = 0.0;
	m_SCT2 = 0.0;
	m_Closing = 0;
	m_Obstacles = 0;
	m_Time = 0.0;
	m_Open = 0;
	m_Faults = 0;
	m_Close = 0;
	m_Place = m_Run->GetPlace();
	m_BonusPts = 0;
	m_InClass = m_Run->GetInClass();
	m_DogsQd = m_Run->GetDogsQd();
	//}}AFX_DATA_INIT
	m_Conditions.Replace(_T("\n"), _T("\r\n"));
}


CDlgRunScore::~CDlgRunScore()
{
}


void CDlgRunScore::DoDataExchange(CDataExchange* pDX)
{
	CDlgBasePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRunScore)
	DDX_Control(pDX, IDC_RUNSCORE_DATE, m_ctrlDate);
	DDX_Text(pDX, IDC_RUNSCORE_VENUE, m_Venue);
	DDX_Text(pDX, IDC_RUNSCORE_CLUB, m_Club);
	DDX_Text(pDX, IDC_RUNSCORE_LOCATION, m_Location);
	DDX_Control(pDX, IDC_RUNSCORE_DIVISION, m_ctrlDivisions);
	DDX_Control(pDX, IDC_RUNSCORE_LEVEL, m_ctrlLevels);
	DDX_Control(pDX, IDC_RUNSCORE_EVENT, m_ctrlEvents);
	DDX_Control(pDX, IDC_RUNSCORE_SUBNAME_TEXT, m_ctrlSubNamesText);
	DDX_Control(pDX, IDC_RUNSCORE_SUBNAME, m_ctrlSubNames);
	DDX_CBString(pDX, IDC_RUNSCORE_SUBNAME, m_SubName);
	DDX_Control(pDX, IDC_RUNSCORE_DESC, m_ctrlDesc);
	DDX_Control(pDX, IDC_RUNSCORE_HEIGHT, m_ctrlHeight);
	DDX_CBString(pDX, IDC_RUNSCORE_HEIGHT, m_Height);
	DDX_Control(pDX, IDC_RUNSCORE_JUDGE, m_ctrlJudge);
	DDX_CBString(pDX, IDC_RUNSCORE_JUDGE, m_Judge);
	DDX_Control(pDX, IDC_RUNSCORE_JUDGE_NOTES, m_ctrlJudgeNotes);
	DDX_Control(pDX, IDC_RUNSCORE_HANDLER, m_ctrlHandler);
	DDX_CBString(pDX, IDC_RUNSCORE_HANDLER, m_Handler);
	DDX_Control(pDX, IDC_RUNSCORE_CONDITIONS, m_ctrlConditions);
	DDX_Text(pDX, IDC_RUNSCORE_CONDITIONS, m_Conditions);
	DDX_Control(pDX, IDC_RUNSCORE_PARTNERS_EDIT, m_ctrlPartnerEdit);
	DDX_Control(pDX, IDC_RUNSCORE_PARTNER, m_ctrlPartner);
	DDX_Control(pDX, IDC_RUNSCORE_TABLE, m_ctrlTable);
	DDX_Control(pDX, IDC_RUNSCORE_SCT_TEXT, m_ctrlSCTText);
	DDX_Control(pDX, IDC_RUNSCORE_SCT, m_ctrlSCT);
	DDX_Text(pDX, IDC_RUNSCORE_SCT, m_SCT);
	DDX_Control(pDX, IDC_RUNSCORE_OPENING_PTS_TEXT, m_ctrlOpeningText);
	DDX_Control(pDX, IDC_RUNSCORE_OPENING_PTS, m_ctrlOpening);
	DDX_Text(pDX, IDC_RUNSCORE_OPENING_PTS, m_Opening);
	DDX_Control(pDX, IDC_RUNSCORE_YARDS_TEXT, m_ctrlYardsText);
	DDX_Control(pDX, IDC_RUNSCORE_YARDS, m_ctrlYards);
	DDX_Text(pDX, IDC_RUNSCORE_YARDS, m_Yards);
	DDX_Control(pDX, IDC_RUNSCORE_SCT2_TEXT, m_ctrlSCT2Text);
	DDX_Control(pDX, IDC_RUNSCORE_SCT2, m_ctrlSCT2);
	DDX_Text(pDX, IDC_RUNSCORE_SCT2, m_SCT2);
	DDX_Control(pDX, IDC_RUNSCORE_CLOSING_PTS_TEXT, m_ctrlClosingText);
	DDX_Control(pDX, IDC_RUNSCORE_CLOSING_PTS, m_ctrlClosing);
	DDX_Text(pDX, IDC_RUNSCORE_CLOSING_PTS, m_Closing);
	DDX_Control(pDX, IDC_RUNSCORE_OBSTACLES_TEXT, m_ctrlObstaclesText);
	DDX_Control(pDX, IDC_RUNSCORE_OBSTACLES, m_ctrlObstacles);
	DDX_Text(pDX, IDC_RUNSCORE_OBSTACLES, m_Obstacles);
	DDX_Control(pDX, IDC_RUNSCORE_TIME_TEXT, m_ctrlTimeText);
	DDX_Control(pDX, IDC_RUNSCORE_TIME, m_ctrlTime);
	DDX_Text(pDX, IDC_RUNSCORE_TIME, m_Time);
	DDX_Control(pDX, IDC_RUNSCORE_OPEN_PTS_TEXT, m_ctrlOpenText);
	DDX_Control(pDX, IDC_RUNSCORE_OPEN_PTS, m_ctrlOpen);
	DDX_Text(pDX, IDC_RUNSCORE_OPEN_PTS, m_Open);
	DDX_Control(pDX, IDC_RUNSCORE_MIN_YPS_TEXT, m_ctrlMinYPSText);
	DDX_Control(pDX, IDC_RUNSCORE_MIN_YPS, m_ctrlMinYPS);
	DDX_Control(pDX, IDC_RUNSCORE_YPS_TEXT, m_ctrlYPSText);
	DDX_Control(pDX, IDC_RUNSCORE_YPS, m_ctrlYPS);
	DDX_Control(pDX, IDC_RUNSCORE_FAULTS_TEXT, m_ctrlFaultsText);
	DDX_Control(pDX, IDC_RUNSCORE_FAULTS, m_ctrlFaults);
	DDX_Text(pDX, IDC_RUNSCORE_FAULTS, m_Faults);
	DDX_Control(pDX, IDC_RUNSCORE_CLOSE_PTS_TEXT, m_ctrlCloseText);
	DDX_Control(pDX, IDC_RUNSCORE_CLOSE_PTS, m_ctrlClose);
	DDX_Text(pDX, IDC_RUNSCORE_CLOSE_PTS, m_Close);
	DDX_Control(pDX, IDC_RUNSCORE_TOTAL_FAULTS_TEXT, m_ctrlTotalFaultsText);
	DDX_Control(pDX, IDC_RUNSCORE_TOTAL_FAULTS, m_ctrlTotalFaults);
	DDX_Control(pDX, IDC_RUNSCORE_OBSTACLES_PER_SEC_TEXT, m_ctrlObstaclesPSText);
	DDX_Control(pDX, IDC_RUNSCORE_OBSTACLES_PER_SEC, m_ctrlObstaclesPS);
	DDX_Control(pDX, IDC_RUNSCORE_PLACE, m_ctrlPlace);
	DDX_Text(pDX, IDC_RUNSCORE_PLACE, m_Place);
	DDX_Control(pDX, IDC_RUNSCORE_IN_CLASS, m_ctrlInClass);
	DDX_Text(pDX, IDC_RUNSCORE_IN_CLASS, m_InClass);
	DDX_Control(pDX, IDC_RUNSCORE_DOGS_QD, m_ctrlDogsQd);
	DDX_Text(pDX, IDC_RUNSCORE_DOGS_QD, m_DogsQd);
	DDX_Control(pDX, IDC_RUNSCORE_Q, m_ctrlQ);
	DDX_Control(pDX, IDC_RUNSCORE_SCORE, m_ctrlScore);
	DDX_Control(pDX, IDC_RUNSCORE_BONUSPTS_TEXT, m_ctrlBonusPtsText);
	DDX_Control(pDX, IDC_RUNSCORE_BONUSPTS, m_ctrlBonusPts);
	DDX_Text(pDX, IDC_RUNSCORE_BONUSPTS, m_BonusPts);
	DDX_Control(pDX, IDC_RUNSCORE_SPEEDPTS_TEXT, m_ctrlSpeedPtsText);
	DDX_Control(pDX, IDC_RUNSCORE_SPEEDPTS, m_ctrlSpeedPts);
	DDX_Control(pDX, IDC_RUNSCORE_TITLE_POINTS_TEXT, m_ctrlTitlePointsText);
	DDX_Control(pDX, IDC_RUNSCORE_TITLE_POINTS, m_ctrlTitlePoints);
	DDX_Control(pDX, IDC_RUNSCORE_OTHERPOINTS, m_ctrlOtherPoints);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		m_Venue.TrimRight();
		m_Venue.TrimLeft();
		m_Club.TrimRight();
		m_Club.TrimLeft();
		m_Location.TrimRight();
		m_Location.TrimLeft();
		m_SubName.TrimRight();
		m_SubName.TrimLeft();
		m_Height.TrimRight();
		m_Height.TrimLeft();
		m_Judge.TrimRight();
		m_Judge.TrimLeft();
		m_Handler.TrimRight();
		m_Handler.TrimLeft();
		m_Conditions.TrimRight();
		m_Conditions.TrimLeft();

		CString str;
		tstring div, level, evt;

		pDX->PrepareCtrl(m_ctrlDivisions.GetDlgCtrlID());
		int index = m_ctrlDivisions.GetCurSel();
		if (CB_ERR == index)
		{
			AfxMessageBox(IDS_SELECT_DIVISION);
			pDX->Fail();
			return;
		}
		m_ctrlDivisions.GetLBText(index, str);
		if (str.IsEmpty())
		{
			AfxMessageBox(IDS_SELECT_DIVISION);
			pDX->Fail();
			return;
		}
		div = (LPCTSTR)str;

		pDX->PrepareCtrl(m_ctrlLevels.GetDlgCtrlID());
		index = m_ctrlLevels.GetCurSel();
		if (CB_ERR == index)
		{
			AfxMessageBox(IDS_SELECT_LEVEL);
			pDX->Fail();
			return;
		}
		m_ctrlLevels.GetLBText(index, str);
		if (str.IsEmpty())
		{
			AfxMessageBox(IDS_SELECT_LEVEL);
			pDX->Fail();
			return;
		}
		level = (LPCTSTR)str;
		CDlgRunDataLevel* pLevel = GetLevelData(index);
		ASSERT(pLevel);

		pDX->PrepareCtrl(m_ctrlEvents.GetDlgCtrlID());
		index = m_ctrlEvents.GetCurSel();
		if (CB_ERR == index)
		{
			AfxMessageBox(IDS_SELECT_EVENT);
			pDX->Fail();
			return;
		}
		m_ctrlEvents.GetLBText(index, str);
		if (str.IsEmpty())
		{
			AfxMessageBox(IDS_SELECT_EVENT);
			pDX->Fail();
			return;
		}
		evt = (LPCTSTR)str;

		pDX->PrepareCtrl(m_ctrlJudge.GetDlgCtrlID());
		if (m_Judge.IsEmpty())
		{
			AfxMessageBox(IDS_SELECT_JUDGE);
			pDX->Fail();
			return;
		}

		pDX->PrepareCtrl(m_ctrlQ.GetDlgCtrlID());
		index = m_ctrlQ.GetCurSel();
		if (CB_ERR == index)
		{
			AfxMessageBox(IDS_SELECT_Q);
			pDX->Fail();
			return;
		}
		ARB_Q q = ARB_Q::GetValidType(static_cast<int>(m_ctrlQ.GetItemData(index)));

		pDX->PrepareCtrl(m_ctrlEvents.GetDlgCtrlID());
		ARBConfigEventPtr pEvent;
		m_pVenue->GetEvents().FindEvent(evt, &pEvent);
		if (!pEvent)
		{
			AfxMessageBox(IDS_BAD_EVENT, MB_ICONSTOP);
			pDX->Fail();
			return;
		}

		pDX->PrepareCtrl(m_ctrlLevels.GetDlgCtrlID());
		ARBConfigScoringPtr pScoring;
		pEvent->FindEvent(div, pLevel->m_pLevel->GetName(), m_Run->GetDate(), &pScoring);
		if (!pScoring)
		{
			AfxMessageBox(IDS_BAD_SCORINGMETHOD, MB_ICONSTOP);
			pDX->Fail();
			return;
		}

		//@todo: Add integrity checks - things like snooker score >=37? is Q set?

		m_Run->SetDivision(div);
		m_Run->SetLevel(level);
		m_Run->SetEvent(evt);
		m_Run->SetSubName((LPCTSTR)m_SubName);
		m_Run->SetHeight((LPCTSTR)m_Height);
		m_Run->SetJudge((LPCTSTR)m_Judge);
		m_Run->SetHandler((LPCTSTR)m_Handler);
		CString tmp(m_Conditions);
		tmp.Replace(_T("\r\n"), _T("\n"));
		m_Run->SetConditions((LPCTSTR)tmp);
		switch (pScoring->GetScoringStyle())
		{
		default:
			ASSERT(0);
			break;
		case ARBConfigScoring::eFaultsThenTime:
		case ARBConfigScoring::eFaults100ThenTime:
		case ARBConfigScoring::eFaults200ThenTime:
		case ARBConfigScoring::eTimePlusFaults:
			m_Run->GetScoring().SetSCT(m_SCT);
			m_Run->GetScoring().SetYards(m_Yards);
			m_Run->GetScoring().SetTime(m_Time);
			m_Run->GetScoring().SetCourseFaults(m_Faults);
			break;
		case ARBConfigScoring::eOCScoreThenTime:
			m_Run->GetScoring().SetSCT(m_SCT);
			m_Run->GetScoring().SetNeedOpenPts(m_Opening);
			m_Run->GetScoring().SetSCT2(m_SCT2);
			m_Run->GetScoring().SetNeedClosePts(m_Closing);
			m_Run->GetScoring().SetTime(m_Time);
			m_Run->GetScoring().SetOpenPts(m_Open);
			m_Run->GetScoring().SetCourseFaults(m_Faults);
			m_Run->GetScoring().SetClosePts(m_Close);
			break;
		case ARBConfigScoring::eScoreThenTime:
			m_Run->GetScoring().SetSCT(m_SCT);
			m_Run->GetScoring().SetNeedOpenPts(m_Opening);
			m_Run->GetScoring().SetTime(m_Time);
			m_Run->GetScoring().SetOpenPts(m_Open);
			m_Run->GetScoring().SetCourseFaults(m_Faults);
			break;
		}
		m_Run->SetPlace(m_Place);
		m_Run->SetInClass(m_InClass);
		m_Run->SetDogsQd(m_DogsQd);
		m_Run->SetQ(q);
		m_Run->GetScoring().SetBonusPts(m_BonusPts);
		m_Run->GetScoring().SetObstacles(m_Obstacles);
	}
}


BEGIN_MESSAGE_MAP(CDlgRunScore, CDlgBasePropertyPage)
	//{{AFX_MSG_MAP(CDlgRunScore)
	ON_WM_DESTROY()
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_RUNSCORE_DATE, OnDatetimechangeDate)
	ON_CBN_SELCHANGE(IDC_RUNSCORE_DIVISION, OnSelchangeDivision)
	ON_CBN_SELCHANGE(IDC_RUNSCORE_LEVEL, OnSelchangeLevel)
	ON_CBN_SELCHANGE(IDC_RUNSCORE_EVENT, OnSelchangeEvent)
	ON_BN_CLICKED(IDC_RUNSCORE_JUDGE_NOTES, OnJudgeNotes)
	ON_BN_CLICKED(IDC_RUNSCORE_PARTNERS_EDIT, OnPartnersEdit)
	ON_BN_CLICKED(IDC_RUNSCORE_OTHERPOINTS, OnOtherpoints)
	ON_EN_KILLFOCUS(IDC_RUNSCORE_FAULTS, OnKillfocusFaults)
	ON_EN_KILLFOCUS(IDC_RUNSCORE_TIME, OnKillfocusTime)
	ON_EN_KILLFOCUS(IDC_RUNSCORE_YARDS, OnKillfocusYards)
	ON_EN_KILLFOCUS(IDC_RUNSCORE_SCT, OnKillfocusSct)
	ON_EN_KILLFOCUS(IDC_RUNSCORE_SCT2, OnKillfocusSct2)
	ON_EN_KILLFOCUS(IDC_RUNSCORE_OPENING_PTS, OnKillfocusOpening)
	ON_EN_KILLFOCUS(IDC_RUNSCORE_CLOSING_PTS, OnKillfocusClosing)
	ON_EN_KILLFOCUS(IDC_RUNSCORE_OBSTACLES, OnKillfocusObstacles)
	ON_EN_KILLFOCUS(IDC_RUNSCORE_OPEN_PTS, OnKillfocusOpen)
	ON_EN_KILLFOCUS(IDC_RUNSCORE_CLOSE_PTS, OnKillfocusClose)
	ON_EN_KILLFOCUS(IDC_RUNSCORE_PLACE, OnKillfocusPlace)
	ON_EN_KILLFOCUS(IDC_RUNSCORE_BONUSPTS, OnKillfocusBonus)
	ON_BN_CLICKED(IDC_RUNSCORE_TABLE, OnBnClickedTableYps)
	ON_CBN_SELCHANGE(IDC_RUNSCORE_Q, OnSelchangeQ)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CListPtrData<ARBConfigDivisionPtr>* CDlgRunScore::GetDivisionData(int index) const
{
	CListData* pData = m_ctrlDivisions.GetData(index);
	return dynamic_cast<CListPtrData<ARBConfigDivisionPtr>*>(pData);
}


CDlgRunDataLevel* CDlgRunScore::GetLevelData(int index) const
{
	CListData* pData = m_ctrlLevels.GetData(index);
	return dynamic_cast<CDlgRunDataLevel*>(pData);
}


// Helper functions to avoid UpdateData. Used during KillFocus events.
bool CDlgRunScore::GetText(
		CEdit* pEdit,
		short& val) const
{
	if (!pEdit)
		return false;
	CString str;
	pEdit->GetWindowText(str);
	val = static_cast<short>(_tstol((LPCTSTR)str));
	return true;
}


bool CDlgRunScore::GetText(
		CEdit* pEdit,
		double& val) const
{
	if (!pEdit)
		return false;
	CString str;
	pEdit->GetWindowText(str);
	val = _tcstod((LPCTSTR)str, NULL);
	return true;
}


bool CDlgRunScore::GetEvent(ARBConfigEventPtr* outEvent) const
{
	if (outEvent)
		outEvent->reset();
	CString str;
	int index = m_ctrlEvents.GetCurSel();
	if (CB_ERR == index)
		return false;
	m_ctrlEvents.GetLBText(index, str);
	if (str.IsEmpty())
		return false;
	tstring evt = (LPCTSTR)str;
	return m_pVenue->GetEvents().FindEvent(evt, outEvent);
}


bool CDlgRunScore::GetScoring(ARBConfigScoringPtr* outScoring) const
{
	bool bFound = false;
	if (outScoring)
		outScoring->reset();
	tstring div, level;
	CString str;
	int index = m_ctrlDivisions.GetCurSel();
	if (CB_ERR != index)
	{
		m_ctrlDivisions.GetLBText(index, str);
		if (!str.IsEmpty())
			div = (LPCTSTR)str;
	}
	index = m_ctrlLevels.GetCurSel();
	if (CB_ERR != index)
	{
		m_ctrlLevels.GetLBText(index, str);
		if (!str.IsEmpty())
			level = (LPCTSTR)str;
		CDlgRunDataLevel* pLevel = GetLevelData(index);
		ASSERT(pLevel);
		ARBConfigEventPtr pEvent;
		if (GetEvent(&pEvent))
		{
			if (0 < div.length() && 0 < level.length())
				bFound = pEvent->FindEvent(div, pLevel->m_pLevel->GetName(), m_Run->GetDate(), outScoring);
		}
	}
	return bFound;
}


void CDlgRunScore::FillDivisions()
{
	CString str;
	tstring div;
	int index = m_ctrlDivisions.GetCurSel();
	if (CB_ERR != index)
	{
		m_ctrlDivisions.GetLBText(index, str);
		div = (LPCTSTR)str;
	}
	if (str.IsEmpty())
		div = m_Run->GetDivision();
	m_ctrlDivisions.ResetContent();

	for (ARBConfigDivisionList::const_iterator iterDiv = m_pVenue->GetDivisions().begin();
		iterDiv != m_pVenue->GetDivisions().end();
		++iterDiv)
	{
		ARBConfigDivisionPtr pDiv = (*iterDiv);
		// Does this division have any events possible on this date?
		for (ARBConfigEventList::const_iterator iterEvent = m_pVenue->GetEvents().begin();
			iterEvent != m_pVenue->GetEvents().end();
			++iterEvent)
		{
			if ((*iterEvent)->VerifyEvent(pDiv->GetName(), WILDCARD_LEVEL, m_Run->GetDate()))
			{
				index = m_ctrlDivisions.AddString(pDiv->GetName().c_str());
				m_ctrlDivisions.SetData(index,
					new CListPtrData<ARBConfigDivisionPtr>(pDiv));
				if (pDiv->GetName() == div)
					m_ctrlDivisions.SetCurSel(index);
				break;
			}
		}
	}
	// First try to find 'div' (in case the divisions changed)
	if (CB_ERR == m_ctrlDivisions.GetCurSel() && !div.empty())
	{
		index = m_ctrlDivisions.FindStringExact(-1, div.c_str());
		if (0 <= index)
			m_ctrlDivisions.SetCurSel(index);
	}
	// Then try to find the last entered
	if (CB_ERR == m_ctrlDivisions.GetCurSel())
	{
		CString last = CAgilityBookOptions::GetLastEnteredDivision();
		if (0 < last.GetLength())
		{
			index = m_ctrlDivisions.FindStringExact(-1, last);
			if (0 <= index)
				m_ctrlDivisions.SetCurSel(index);
		}
	}
	if (CB_ERR == m_ctrlDivisions.GetCurSel())
	{
		if (1 == m_ctrlDivisions.GetCount())
			m_ctrlDivisions.SetCurSel(0);
	}
	FillLevels();
}


void CDlgRunScore::FillLevels()
{
	CString str;
	tstring level;
	int index = m_ctrlLevels.GetCurSel();
	if (CB_ERR != index)
	{
		m_ctrlLevels.GetLBText(index, str);
		level = (LPCTSTR)str;
	}
	if (str.IsEmpty())
		level = m_Run->GetLevel();
	m_ctrlLevels.ResetContent();
	index = m_ctrlDivisions.GetCurSel();
	if (CB_ERR != index)
	{
		ARBConfigDivisionPtr pDiv = GetDivisionData(index)->GetData();
		for (ARBConfigLevelList::const_iterator iter = pDiv->GetLevels().begin();
			iter != pDiv->GetLevels().end();
			++iter)
		{
			ARBConfigLevelPtr pLevel = (*iter);
			// Does this level have any events possible on this date?
			for (ARBConfigEventList::const_iterator iterEvent = m_pVenue->GetEvents().begin();
				iterEvent != m_pVenue->GetEvents().end();
				++iterEvent)
			{
				if ((*iterEvent)->VerifyEvent(pDiv->GetName(), pLevel->GetName(), m_Run->GetDate()))
				{
					if (0 < pLevel->GetSubLevels().size())
					{
						for (ARBConfigSubLevelList::const_iterator iterSub = pLevel->GetSubLevels().begin();
							iterSub != pLevel->GetSubLevels().end();
							++iterSub)
						{
							ARBConfigSubLevelPtr pSubLevel = (*iterSub);
							int idx = m_ctrlLevels.AddString(pSubLevel->GetName().c_str());
							m_ctrlLevels.SetData(idx,
								new CDlgRunDataLevel(pLevel, pSubLevel));
							if (level == pSubLevel->GetName())
								m_ctrlLevels.SetCurSel(idx);
						}
					}
					else
					{
						int idx = m_ctrlLevels.AddString(pLevel->GetName().c_str());
						m_ctrlLevels.SetData(idx,
							new CDlgRunDataLevel(pLevel));
						if (level == pLevel->GetName())
							m_ctrlLevels.SetCurSel(idx);
					}
					break;
				}
			}
		}
		if (CB_ERR == m_ctrlLevels.GetCurSel())
		{
			CString last = CAgilityBookOptions::GetLastEnteredLevel();
			if (0 < last.GetLength())
			{
				int idx = m_ctrlLevels.FindStringExact(-1, last);
				if (0 <= idx)
					m_ctrlLevels.SetCurSel(idx);
			}
		}
	}
	if (CB_ERR == m_ctrlLevels.GetCurSel())
	{
		if (1 == m_ctrlLevels.GetCount())
			m_ctrlLevels.SetCurSel(0);
	}
	FillEvents();
	SetTitlePoints();
}


void CDlgRunScore::FillEvents()
{
	CString str;
	tstring evt;
	int index = m_ctrlEvents.GetCurSel();
	if (CB_ERR != index)
	{
		m_ctrlEvents.GetLBText(index, str);
		evt = (LPCTSTR)str;
	}
	if (str.IsEmpty())
		evt = m_Run->GetEvent();
	m_ctrlEvents.ResetContent();
	int idxDiv = m_ctrlDivisions.GetCurSel();
	if (CB_ERR != idxDiv)
	{
		ARBConfigDivisionPtr pDiv = GetDivisionData(idxDiv)->GetData();
		int idxLevel = m_ctrlLevels.GetCurSel();
		if (CB_ERR != idxLevel)
		{
			CDlgRunDataLevel* pData = GetLevelData(idxLevel);
			for (ARBConfigEventList::const_iterator iter = m_pVenue->GetEvents().begin();
				iter != m_pVenue->GetEvents().end();
				++iter)
			{
				ARBConfigEventPtr pEvent = (*iter);
				if (pEvent->FindEvent(pDiv->GetName(), pData->m_pLevel->GetName(), m_Run->GetDate()))
				{
					int idx = m_ctrlEvents.AddString(pEvent->GetName().c_str());
					if (evt == pEvent->GetName())
					{
						m_ctrlEvents.SetCurSel(idx);
						SetEventDesc(pEvent);
					}
				}
			}
		}
	}
	FillSubNames();
	UpdateControls();
}


void CDlgRunScore::FillSubNames()
{
	ARBConfigEventPtr pEvent;
	if (GetEvent(&pEvent))
	{
		if (pEvent->HasSubNames())
		{
			std::set<tstring> names;
			m_pDoc->Book().GetAllEventSubNames(m_pVenue->GetName(), pEvent, names);
			m_ctrlSubNames.ResetContent();
			for (std::set<tstring>::const_iterator iter = names.begin();
				iter != names.end();
				++iter)
			{
				m_ctrlSubNames.AddString(iter->c_str());
			}
			m_ctrlSubNamesText.ShowWindow(SW_SHOW);
			m_ctrlSubNames.ShowWindow(SW_SHOW);
		}
		else
		{
			m_ctrlSubNamesText.ShowWindow(SW_HIDE);
			m_ctrlSubNames.ShowWindow(SW_HIDE);
		}
	}
	else
	{
		m_ctrlSubNamesText.ShowWindow(SW_HIDE);
		m_ctrlSubNames.ShowWindow(SW_HIDE);
	}
}


void CDlgRunScore::FillJudges()
{
	set<tstring> names;
	m_pDoc->Book().GetAllJudges(names, true, true);
	if (!m_Run->GetJudge().empty())
		names.insert(m_Run->GetJudge());
	m_ctrlJudge.ResetContent();
	for (set<tstring>::const_iterator iter = names.begin(); iter != names.end(); ++iter)
	{
		m_ctrlJudge.AddString((*iter).c_str());
	}
	m_ctrlJudge.SetWindowText(m_Judge);
}


void CDlgRunScore::SetEventDesc(ARBConfigEventPtr inEvent)
{
	CString desc;
	if (inEvent)
		desc += inEvent->GetDesc().c_str();
	ARBConfigScoringPtr pScoring;
	if (GetScoring(&pScoring))
	{
		tstring const& note = pScoring->GetNote();
		if (!desc.IsEmpty() && 0 < note.length())
			desc += _T("\n==========\n");
		desc += note.c_str();
	}
	desc.Replace(_T("\n"), _T("\r\n"));
	m_ctrlDesc.SetWindowText(desc);
}


void CDlgRunScore::SetPartnerText()
{
	CString partners;
	ARBConfigEventPtr pEvent;
	if (GetEvent(&pEvent))
	{
		if (pEvent->HasPartner())
		{
			for (ARBDogRunPartnerList::const_iterator iter = m_Run->GetPartners().begin(); iter != m_Run->GetPartners().end(); ++iter)
			{
				if (!partners.IsEmpty())
					partners += _T(", ");
				partners += (*iter)->GetHandler().c_str();
				partners += _T("/");
				partners += (*iter)->GetDog().c_str();
			}
		}
	}
	m_ctrlPartner.SetWindowText(partners);
}


void CDlgRunScore::SetMinYPS()
{
	CString str;
	double yps;
	if (m_Run->GetScoring().GetMinYPS(CAgilityBookOptions::GetTableInYPS(), yps))
	{
		str = ARBDouble::str(yps, 3).c_str();
	}
	m_ctrlMinYPS.SetWindowText(str);
}


void CDlgRunScore::SetYPS()
{
	CString str;
	double yps;
	if (m_Run->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), yps))
	{
		str = ARBDouble::str(yps, 3).c_str();
	}
	m_ctrlYPS.SetWindowText(str);
}


void CDlgRunScore::SetObstacles()
{
	CString str;
	double ops;
	if (m_Run->GetScoring().GetObstaclesPS(CAgilityBookOptions::GetTableInYPS(), ops))
	{
		str = ARBDouble::str(ops, 3).c_str();
	}
	m_ctrlObstaclesPS.SetWindowText(str);
}


void CDlgRunScore::SetTotalFaults()
{
	CString total;
	if (ARBDogRunScoring::eTypeByTime == m_Run->GetScoring().GetType())
	{
		ARBConfigScoringPtr pScoring;
		GetScoring(&pScoring);
		double faults = m_Run->GetScoring().GetCourseFaults() + m_Run->GetScoring().GetTimeFaults(pScoring);
		total = ARBDouble::str(faults, 3).c_str();
	}
	m_ctrlTotalFaults.SetWindowText(total);
}


void CDlgRunScore::FillQ(ARBConfigScoringPtr inScoring)
{
	m_ctrlQ.ResetContent();
	bool bHasTitling = (0 < inScoring->GetTitlePoints().size());
	int nQs = ARB_Q::GetNumValidTypes();
	for (int index = 0; index < nQs; ++index)
	{
		ARB_Q q = ARB_Q::GetValidType(index);
		if (ARB_Q::eQ_SuperQ == q && !inScoring->HasSuperQ())
			continue;
		// Allow non-titling runs to only have 'NA' and 'E'
		if (!bHasTitling && !(ARB_Q::eQ_E == q || ARB_Q::eQ_NA == q))
			continue;
		int idx = m_ctrlQ.AddString(q.str().c_str());
		m_ctrlQ.SetItemData(idx, index);
		if (m_Run->GetQ() == q)
			m_ctrlQ.SetCurSel(idx);
	}
	if (0 < m_ctrlQ.GetCount())
		m_ctrlQ.EnableWindow(TRUE);
}


void CDlgRunScore::SetTitlePoints()
{
	int index = m_ctrlQ.GetCurSel();
	if (CB_ERR == index)
	{
		GotoDlgCtrl(&m_ctrlQ);
		return;
	}
	ARB_Q q = ARB_Q::GetValidType(static_cast<int>(m_ctrlQ.GetItemData(index)));

	CString strBonus(_T("0"));
	CString strSpeed(_T("0"));
	CString strTitle(_T("0"));
	CString strScore(_T(""));
	ARBConfigScoringPtr pScoring;
	if (GetScoring(&pScoring))
	{
		// 8/17/03: Only compute title points on Q runs.
		if (q.Qualified())
		{
			if (pScoring->HasBonusPts())
			{
				otstringstream str;
				str << m_Run->GetScoring().GetBonusPts();
				strBonus = str.str().c_str();
			}
			if (pScoring->HasSpeedPts())
			{
				otstringstream str;
				str << m_Run->GetSpeedPoints(pScoring);
				strSpeed = str.str().c_str();
			}
			{
				otstringstream str;
				str << m_Run->GetTitlePoints(pScoring);
				strTitle = str.str().c_str();
			}
		}
		// 8/17/03: Only compute score on Q and NQ runs.
		// 11/13/04: Also compute score for NA runs that have no titling pts.
		if (q.Qualified()
		|| ARB_Q::eQ_NQ == q
		|| (ARB_Q::eQ_NA == q && ARBDouble::equal(0.0, static_cast<double>(pScoring->GetTitlePoints().size()))))
			strScore = ARBDouble::str(m_Run->GetScore(pScoring)).c_str();
	}
	// Doesn't matter if they're hidden,..
	m_ctrlBonusPts.SetWindowText(strBonus);
	m_ctrlSpeedPts.SetWindowText(strSpeed);
	m_ctrlTitlePoints.SetWindowText(strTitle);
	m_ctrlScore.SetWindowText(strScore);
}


void CDlgRunScore::UpdateControls(bool bOnEventChange)
{
	m_ctrlHeight.EnableWindow(FALSE);
	m_ctrlJudge.EnableWindow(FALSE);
	m_ctrlHandler.EnableWindow(FALSE);
	m_ctrlConditions.EnableWindow(FALSE);
	m_ctrlPartnerEdit.ShowWindow(SW_HIDE);
	m_ctrlPartner.ShowWindow(SW_HIDE);
	m_ctrlTable.ShowWindow(SW_HIDE);
	m_ctrlSCTText.ShowWindow(SW_HIDE);
	m_ctrlSCT.ShowWindow(SW_HIDE);
	m_ctrlOpeningText.ShowWindow(SW_HIDE);
	m_ctrlOpening.ShowWindow(SW_HIDE);
	m_ctrlYardsText.ShowWindow(SW_HIDE);
	m_ctrlYards.ShowWindow(SW_HIDE);
	m_ctrlSCT2Text.ShowWindow(SW_HIDE);
	m_ctrlSCT2.ShowWindow(SW_HIDE);
	m_ctrlClosingText.ShowWindow(SW_HIDE);
	m_ctrlClosing.ShowWindow(SW_HIDE);
	m_ctrlTimeText.ShowWindow(SW_HIDE);
	m_ctrlTime.ShowWindow(SW_HIDE);
	m_ctrlOpenText.ShowWindow(SW_HIDE);
	m_ctrlOpen.ShowWindow(SW_HIDE);
	m_ctrlMinYPSText.ShowWindow(SW_HIDE);
	m_ctrlMinYPS.ShowWindow(SW_HIDE);
	m_ctrlYPSText.ShowWindow(SW_HIDE);
	m_ctrlYPS.ShowWindow(SW_HIDE);
	m_ctrlFaultsText.ShowWindow(SW_HIDE);
	m_ctrlFaults.ShowWindow(SW_HIDE);
	m_ctrlCloseText.ShowWindow(SW_HIDE);
	m_ctrlClose.ShowWindow(SW_HIDE);
	m_ctrlTotalFaultsText.ShowWindow(SW_HIDE);
	m_ctrlTotalFaults.ShowWindow(SW_HIDE);
	m_ctrlPlace.EnableWindow(FALSE);
	m_ctrlInClass.EnableWindow(FALSE);
	m_ctrlDogsQd.EnableWindow(FALSE);
	m_ctrlQ.EnableWindow(FALSE);
	m_ctrlBonusPtsText.ShowWindow(SW_HIDE);
	m_ctrlBonusPts.ShowWindow(SW_HIDE);
	m_ctrlSpeedPtsText.ShowWindow(SW_HIDE);
	m_ctrlSpeedPts.ShowWindow(SW_HIDE);
	m_ctrlTitlePointsText.ShowWindow(SW_HIDE);
	m_ctrlTitlePoints.ShowWindow(SW_HIDE);

	ARBConfigScoringPtr pScoring;
	if (!GetScoring(&pScoring))
	{
		m_Run->GetScoring().SetType(ARBDogRunScoring::eTypeUnknown, false);
		return;
	}

	ARBConfigEventPtr pEvent;
	if (!GetEvent(&pEvent))
		return;

	m_ctrlHeight.EnableWindow(TRUE);
	m_ctrlJudge.EnableWindow(TRUE);
	m_ctrlHandler.EnableWindow(TRUE);
	m_ctrlConditions.EnableWindow(TRUE);

	if (pEvent->HasPartner())
	{
		m_ctrlPartnerEdit.ShowWindow(SW_SHOW);
		m_ctrlPartner.ShowWindow(SW_SHOW);
	}

	// 7/1/07: Resetting the table status must always be done - not just when
	// the new event has a table. (which is what was done before)
	if (bOnEventChange)
	{
		if (m_Run->GetScoring().HasTable() != pEvent->HasTable())
		{
			m_Run->GetScoring().SetHasTable(pEvent->HasTable());
			// Plus, we need to recompute the YPS.
			SetMinYPS();
			SetYPS();
			SetObstacles();
		}
	}
	if (pEvent->HasTable())
	{
		m_ctrlTable.ShowWindow(SW_SHOW);
		m_ctrlTable.SetCheck(m_Run->GetScoring().HasTable() ? 1 : 0);
	}

	switch (pScoring->GetScoringStyle())
	{
	default:
		ASSERT(0);
		break;
	case ARBConfigScoring::eFaultsThenTime:
	case ARBConfigScoring::eFaults100ThenTime:
	case ARBConfigScoring::eFaults200ThenTime:
	case ARBConfigScoring::eTimePlusFaults:
		m_Run->GetScoring().SetType(ARBDogRunScoring::eTypeByTime, pScoring->DropFractions());
		m_ctrlSCTText.ShowWindow(SW_SHOW);
		m_ctrlSCT.ShowWindow(SW_SHOW);
		m_ctrlYardsText.ShowWindow(SW_SHOW);
		m_ctrlYards.ShowWindow(SW_SHOW);
		m_ctrlTimeText.ShowWindow(SW_SHOW);
		m_ctrlTime.ShowWindow(SW_SHOW);
		m_ctrlMinYPSText.ShowWindow(SW_SHOW);
		m_ctrlMinYPS.ShowWindow(SW_SHOW);
		m_ctrlYPSText.ShowWindow(SW_SHOW);
		m_ctrlYPS.ShowWindow(SW_SHOW);
		m_ctrlFaultsText.ShowWindow(SW_SHOW);
		m_ctrlFaults.ShowWindow(SW_SHOW);
		m_ctrlTotalFaultsText.ShowWindow(SW_SHOW);
		m_ctrlTotalFaults.ShowWindow(SW_SHOW);
		break;
	case ARBConfigScoring::eOCScoreThenTime:
		m_Run->GetScoring().SetType(ARBDogRunScoring::eTypeByOpenClose, pScoring->DropFractions());
		m_Opening = pScoring->GetRequiredOpeningPoints();
		m_Closing = pScoring->GetRequiredClosingPoints();
		// Do not push these (above) back into the run.
		// Otherwise this will overwrite valid values during OnInit.
		m_ctrlSCTText.ShowWindow(SW_SHOW);
		m_ctrlSCT.ShowWindow(SW_SHOW);
		m_ctrlOpeningText.SetWindowText(m_strOpening[0]);
		m_ctrlOpeningText.ShowWindow(SW_SHOW);
		m_ctrlOpening.ShowWindow(SW_SHOW);
		m_ctrlSCT2Text.ShowWindow(SW_SHOW);
		m_ctrlSCT2.ShowWindow(SW_SHOW);
		m_ctrlClosingText.ShowWindow(SW_SHOW);
		m_ctrlClosing.ShowWindow(SW_SHOW);
		m_ctrlTimeText.ShowWindow(SW_SHOW);
		m_ctrlTime.ShowWindow(SW_SHOW);
		m_ctrlOpenText.SetWindowText(m_strOpen[0]);
		m_ctrlOpenText.ShowWindow(SW_SHOW);
		m_ctrlOpen.ShowWindow(SW_SHOW);
		m_ctrlFaultsText.ShowWindow(SW_SHOW);
		m_ctrlFaults.ShowWindow(SW_SHOW);
		m_ctrlCloseText.ShowWindow(SW_SHOW);
		m_ctrlClose.ShowWindow(SW_SHOW);
		// I don't want to call UpdateData here. It could cause a loss of data.
		{
			otstringstream tmp;
			tmp << m_Opening;
			m_ctrlOpening.SetWindowText(tmp.str().c_str());
		}
		{
			otstringstream tmp;
			tmp << m_Closing;
			m_ctrlClosing.SetWindowText(tmp.str().c_str());
		}
		break;
	case ARBConfigScoring::eScoreThenTime:
		m_Run->GetScoring().SetType(ARBDogRunScoring::eTypeByPoints, pScoring->DropFractions());
		m_Opening = pScoring->GetRequiredOpeningPoints();
		// Do not push this back into the run.
		// Otherwise this will overwrite valid values during OnInit.
		m_ctrlSCTText.ShowWindow(SW_SHOW);
		m_ctrlSCT.ShowWindow(SW_SHOW);
		m_ctrlOpeningText.SetWindowText(m_strOpening[1]);
		m_ctrlOpeningText.ShowWindow(SW_SHOW);
		m_ctrlOpening.ShowWindow(SW_SHOW);
		m_ctrlTimeText.ShowWindow(SW_SHOW);
		m_ctrlTime.ShowWindow(SW_SHOW);
		m_ctrlOpenText.SetWindowText(m_strOpen[1]);
		m_ctrlOpenText.ShowWindow(SW_SHOW);
		m_ctrlOpen.ShowWindow(SW_SHOW);
		m_ctrlFaultsText.ShowWindow(SW_SHOW);
		m_ctrlFaults.ShowWindow(SW_SHOW);
		// I don't want to call UpdateData here. It could cause a loss of data.
		{
			otstringstream tmp;
			tmp << m_Opening;
			m_ctrlOpening.SetWindowText(tmp.str().c_str());
		}
		break;
	}

	m_ctrlPlace.EnableWindow(TRUE);
	m_ctrlInClass.EnableWindow(TRUE);
	if (0 < pScoring->GetTitlePoints().size())
	{
		m_ctrlDogsQd.EnableWindow(TRUE);
	}
	FillQ(pScoring);
	if (0 < pScoring->GetTitlePoints().size())
	{
		m_ctrlTitlePointsText.ShowWindow(SW_SHOW);
		m_ctrlTitlePoints.ShowWindow(SW_SHOW);
	}
	if (pScoring->HasBonusPts())
	{
		m_ctrlBonusPtsText.ShowWindow(SW_SHOW);
		m_ctrlBonusPts.ShowWindow(SW_SHOW);
	}
	if (pScoring->HasSpeedPts())
	{
		m_ctrlSpeedPtsText.ShowWindow(SW_SHOW);
		m_ctrlSpeedPts.ShowWindow(SW_SHOW);
	}
	SetTitlePoints();
}

/////////////////////////////////////////////////////////////////////////////
// CDlgRunScore message handlers

BOOL CDlgRunScore::OnInitDialog() 
{
	CDlgBasePropertyPage::OnInitDialog();

	m_ctrlOpeningText.GetWindowText(m_strOpening[0]);
	m_ctrlOpenText.GetWindowText(m_strOpen[0]);
	m_strOpening[1].LoadString(IDS_SCORING_REQUIRED_POINTS);
	m_strOpen[1].LoadString(IDS_POINTS);

	CTime date = CTime::GetCurrentTime();
	if (m_Run->GetDate().IsValid())
		date = CTime(m_Run->GetDate().GetDate());
	else
		m_Run->SetDate(ARBDate::Today());
	m_ctrlDate.SetTime(&date);

	FillDivisions(); // This will call UpdateControls();

	set<tstring> names;
	m_pDoc->Book().GetAllHeights(names);
	set<tstring>::const_iterator iter;
	for (iter = names.begin(); iter != names.end(); ++iter)
	{
		m_ctrlHeight.AddString((*iter).c_str());
	}
	m_Height = m_Run->GetHeight().c_str();
	if (m_Height.IsEmpty())
	{
		CString last = CAgilityBookOptions::GetLastEnteredHeight();
		if (!last.IsEmpty())
			m_Height = last;
	}

	m_Judge = m_Run->GetJudge().c_str();
	if (m_Judge.IsEmpty())
	{
		CString last = CAgilityBookOptions::GetLastEnteredJudge();
		if (!last.IsEmpty())
			m_Judge = last;
	}
	FillJudges();

	m_pDoc->Book().GetAllHandlers(names);
	for (iter = names.begin(); iter != names.end(); ++iter)
	{
		m_ctrlHandler.AddString((*iter).c_str());
	}
	m_Handler = m_Run->GetHandler().c_str();
	if (m_Handler.IsEmpty())
	{
		CString last = CAgilityBookOptions::GetLastEnteredHandler();
		if (!last.IsEmpty())
			m_Handler = last;
	}

	SetPartnerText();
	switch (m_Run->GetScoring().GetType())
	{
	default:
		break;
	case ARBDogRunScoring::eTypeByTime:
		m_Faults = m_Run->GetScoring().GetCourseFaults();
		m_Time = m_Run->GetScoring().GetTime();
		m_ctrlTable.SetCheck(m_Run->GetScoring().HasTable() ? 1 : 0);
		m_Yards = m_Run->GetScoring().GetYards();
		m_SCT = m_Run->GetScoring().GetSCT();
		SetMinYPS();
		SetYPS();
		SetTotalFaults();
		break;
	case ARBDogRunScoring::eTypeByOpenClose:
		m_Faults = m_Run->GetScoring().GetCourseFaults();
		m_Time = m_Run->GetScoring().GetTime();
		m_Opening = m_Run->GetScoring().GetNeedOpenPts();
		m_Closing = m_Run->GetScoring().GetNeedClosePts();
		m_Open = m_Run->GetScoring().GetOpenPts();
		m_Close = m_Run->GetScoring().GetClosePts();
		m_SCT = m_Run->GetScoring().GetSCT();
		m_SCT2 = m_Run->GetScoring().GetSCT2();
		break;
	case ARBDogRunScoring::eTypeByPoints:
		m_Faults = m_Run->GetScoring().GetCourseFaults();
		m_Time = m_Run->GetScoring().GetTime();
		m_Opening = m_Run->GetScoring().GetNeedOpenPts();
		m_Open = m_Run->GetScoring().GetOpenPts();
		m_SCT = m_Run->GetScoring().GetSCT();
		break;
	}
	m_BonusPts = m_Run->GetScoring().GetBonusPts();
	m_Obstacles = m_Run->GetScoring().GetObstacles();
	SetObstacles();

	if (0 < m_pDoc->Book().GetConfig().GetOtherPoints().size())
		m_ctrlOtherPoints.EnableWindow(TRUE);
	else
		m_ctrlOtherPoints.EnableWindow(FALSE);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgRunScore::OnDestroy() 
{
	m_ctrlLevels.ResetContent();
	CDlgBasePropertyPage::OnDestroy();
}


void CDlgRunScore::OnDatetimechangeDate(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	// Do not call UpdateData, it causes too much validation
	CTime time;
	m_ctrlDate.GetTime(time);
	m_Run->SetDate(ARBDate(time.GetYear(), time.GetMonth(), time.GetDay()));
	FillDivisions();
	*pResult = 0;
}


void CDlgRunScore::OnSelchangeDivision()
{
	FillLevels();
}


void CDlgRunScore::OnSelchangeLevel()
{
	FillEvents();
}


void CDlgRunScore::OnSelchangeEvent()
{
	FillSubNames();
	UpdateControls(true);
	ARBConfigEventPtr pEvent;
	GetEvent(&pEvent);
	SetEventDesc(pEvent);
}


void CDlgRunScore::OnJudgeNotes()
{
	UpdateData(TRUE);
	m_Judge.TrimRight();
	m_Judge.TrimLeft();
	CDlgInfoJudge dlg(m_pDoc, ARBInfo::eJudgeInfo, (LPCTSTR)m_Judge, this);
	if (IDOK == dlg.DoModal())
		FillJudges();
}


void CDlgRunScore::OnPartnersEdit() 
{
	CDlgListCtrl dlg(CDlgListCtrl::ePartners, m_pDoc, m_Run, this);
	if (IDOK == dlg.DoModal())
		SetPartnerText();
}


void CDlgRunScore::OnOtherpoints() 
{
	CDlgListCtrl dlg(m_pDoc->Book().GetConfig(), m_Run, this);
	dlg.DoModal();
}


// In all of the killfocus routines, do not call updatedata.
// Clicking on the cancel button causes killfocus, which if we're
// canceling, we obviously don't want to validate!

void CDlgRunScore::OnKillfocusFaults()
{
	GetText(&m_ctrlFaults, m_Faults);
	m_Run->GetScoring().SetCourseFaults(m_Faults);
	SetTotalFaults();
	SetTitlePoints();
}


void CDlgRunScore::OnKillfocusTime() 
{
	GetText(&m_ctrlTime, m_Time);
	m_Run->GetScoring().SetTime(m_Time);
	SetYPS();
	SetObstacles();
	SetTotalFaults();
	SetTitlePoints();
}


void CDlgRunScore::OnKillfocusYards()
{
	GetText(&m_ctrlYards, m_Yards);
	m_Run->GetScoring().SetYards(m_Yards);
	SetMinYPS();
	SetYPS();
	SetTotalFaults();
}


void CDlgRunScore::OnKillfocusSct() 
{
	GetText(&m_ctrlSCT, m_SCT);
	m_Run->GetScoring().SetSCT(m_SCT);
	SetMinYPS();
	SetObstacles();
	SetTotalFaults();
	SetTitlePoints();
}


void CDlgRunScore::OnKillfocusSct2() 
{
	GetText(&m_ctrlSCT2, m_SCT2);
	m_Run->GetScoring().SetSCT2(m_SCT2);
	SetObstacles();
}


void CDlgRunScore::OnKillfocusOpening()
{
	GetText(&m_ctrlOpening, m_Opening);
	m_Run->GetScoring().SetNeedOpenPts(m_Opening);
	SetTitlePoints();
}


void CDlgRunScore::OnKillfocusClosing()
{
	GetText(&m_ctrlClosing, m_Closing);
	m_Run->GetScoring().SetNeedClosePts(m_Closing);
	SetTitlePoints();
}


void CDlgRunScore::OnKillfocusObstacles()
{
	GetText(&m_ctrlObstacles, m_Obstacles);
	m_Run->GetScoring().SetObstacles(m_Obstacles);
	SetObstacles();
}


void CDlgRunScore::OnKillfocusOpen()
{
	GetText(&m_ctrlOpen, m_Open);
	m_Run->GetScoring().SetOpenPts(m_Open);
	SetTitlePoints();
}


void CDlgRunScore::OnKillfocusClose()
{
	GetText(&m_ctrlClose, m_Close);
	m_Run->GetScoring().SetClosePts(m_Close);
	SetTitlePoints();
}


void CDlgRunScore::OnKillfocusPlace()
{
	GetText(&m_ctrlPlace, m_Place);
	m_Run->SetPlace(m_Place);
	SetTitlePoints();
}


void CDlgRunScore::OnKillfocusBonus()
{
	GetText(&m_ctrlBonusPts, m_BonusPts);
	m_Run->GetScoring().SetBonusPts(m_BonusPts);
	SetTitlePoints();
}


void CDlgRunScore::OnBnClickedTableYps()
{
	bool bSetTable = false;
	if (m_ctrlTable.IsWindowVisible())
		bSetTable  = m_ctrlTable.GetCheck() ? true : false;
	m_Run->GetScoring().SetHasTable(bSetTable);
	SetMinYPS();
	SetYPS();
	SetObstacles();
}


void CDlgRunScore::OnSelchangeQ()
{
	ARB_Q q;
	int index = m_ctrlQ.GetCurSel();
	if (CB_ERR != index)
		q = ARB_Q::GetValidType(static_cast<int>(m_ctrlQ.GetItemData(index)));
	m_Run->SetQ(q);
	SetTitlePoints();
}
