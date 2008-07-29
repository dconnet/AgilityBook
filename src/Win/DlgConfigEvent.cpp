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
 * @li 2008-07-29 DRC Method overlap detection was wrong.
 * @li 2007-05-08 DRC Fixed bug when deleting a method.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-04 DRC Added support for NADAC bonus titling points.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-02 DRC Added subnames to events.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-12-18 DRC Added a time fault multiplier.
 * @li 2004-12-10 DRC Changes weren't saved when lifetime pts were modified.
 * @li 2004-11-15 DRC Added time fault computation option on T+F.
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
#include "DlgConfigEventMethod.h"
#include "DlgConfigTitlePoints.h"
#include "DlgConfigure.h"
#include "DlgName.h"
#include "ListData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigEvent dialog

CDlgConfigEvent::CDlgConfigEvent(
		bool bNewEntry,
		ARBConfigVenuePtr pVenue,
		ARBConfigEventPtr pEvent,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgConfigEvent::IDD, pParent)
	, m_ctrlSubNames(false)
	, m_ctrlMethods(true)
	, m_ctrlUnused(false)
	, m_ctrlPointsList(true)
	, m_bNewEntry(bNewEntry)
	, m_pVenue(pVenue)
	, m_pEvent(pEvent)
	, m_Scorings()
	, m_idxMethod(-1)
	, m_bHasTable(FALSE)
	, m_bHasPartners(FALSE)
	, m_bHasSubNames(FALSE)
{
	// Copy the existing scorings.
	pEvent->GetScorings().Clone(m_Scorings);
	assert(m_pVenue);
	assert(m_pEvent);
	m_Name = m_pEvent->GetName().c_str();
	m_Desc = m_pEvent->GetDesc().c_str();
	m_Desc.Replace(_T("\n"), _T("\r\n"));
	m_bHasTable = m_pEvent->HasTable() ? TRUE : FALSE;
	m_bHasPartners = m_pEvent->HasPartner() ? TRUE : FALSE;
	m_bHasSubNames = m_pEvent->HasSubNames() ? TRUE : FALSE;
	//{{AFX_DATA_INIT(CDlgConfigEvent)
	//}}AFX_DATA_INIT
}


CDlgConfigEvent::~CDlgConfigEvent()
{
	ClearFixups();
}


void CDlgConfigEvent::GetFixups(std::vector<ARBConfigActionPtr>& ioFixups)
{
	ioFixups.insert(ioFixups.end(), m_DlgFixup.begin(), m_DlgFixup.end());
	m_DlgFixup.clear();
}


void CDlgConfigEvent::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConfigEvent)
	DDX_Text(pDX, IDC_CONFIG_EVENT, m_Name);
	DDX_Check(pDX, IDC_CONFIG_EVENT_TABLE, m_bHasTable);
	DDX_Check(pDX, IDC_CONFIG_EVENT_PARTNER, m_bHasPartners);
	DDX_Check(pDX, IDC_CONFIG_EVENT_HAS_SUBNAMES, m_bHasSubNames);
	DDX_Control(pDX, IDC_CONFIG_EVENT_SUBNAMES, m_ctrlSubNames);
	DDX_Control(pDX, IDC_CONFIG_EVENT_SUBNAMES_NEW, m_ctrlSubNamesNew);
	DDX_Control(pDX, IDC_CONFIG_EVENT_SUBNAMES_EDIT, m_ctrlSubNamesEdit);
	DDX_Control(pDX, IDC_CONFIG_EVENT_SUBNAMES_DELETE, m_ctrlSubNamesDelete);
	DDX_Text(pDX, IDC_CONFIG_EVENT_DESC, m_Desc);
	DDX_Control(pDX, IDC_CONFIG_EVENT_NEW, m_ctrlNew);
	DDX_Control(pDX, IDC_CONFIG_EVENT_EDIT, m_ctrlEdit);
	DDX_Control(pDX, IDC_CONFIG_EVENT_DELETE, m_ctrlDelete);
	DDX_Control(pDX, IDC_CONFIG_EVENT_COPY, m_ctrlCopy);
	DDX_Control(pDX, IDC_CONFIG_EVENT_MOVE_UP, m_ctrlUp);
	DDX_Control(pDX, IDC_CONFIG_EVENT_MOVE_DOWN, m_ctrlDown);
	DDX_Control(pDX, IDC_CONFIG_EVENT_METHODS, m_ctrlMethods);
	DDX_Control(pDX, IDC_CONFIG_EVENT_UNUSED, m_ctrlUnused);
	DDX_Control(pDX, IDC_CONFIG_EVENT_INFO, m_ctrlInfo);
	DDX_Control(pDX, IDC_CONFIG_EVENT_POINTS, m_ctrlPointsList);
	DDX_Control(pDX, IDC_CONFIG_EVENT_POINTS_NEW, m_ctrlPointsNew);
	DDX_Control(pDX, IDC_CONFIG_EVENT_POINTS_EDIT, m_ctrlPointsEdit);
	DDX_Control(pDX, IDC_CONFIG_EVENT_POINTS_DELETE, m_ctrlPointsDelete);
	DDX_Control(pDX, IDC_CONFIG_EVENT_NOTES, m_ctrlNote);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgConfigEvent, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgConfigEvent)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_HAS_SUBNAMES, OnBnClickedSubNames)
	ON_LBN_SELCHANGE(IDC_CONFIG_EVENT_SUBNAMES, OnLbnSelchangeSubnames)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_SUBNAMES_NEW, OnBnClickedSubNamesNew)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_SUBNAMES_EDIT, OnBnClickedSubNamesEdit)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_SUBNAMES_DELETE, OnBnClickedSubNamesDelete)
	ON_LBN_DBLCLK(IDC_CONFIG_EVENT_METHODS, OnLbnDblclkMethods)
	ON_LBN_SELCHANGE(IDC_CONFIG_EVENT_METHODS, OnLbnSelchangeMethods)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_NEW, OnBnClickedNew)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_EDIT, OnBnClickedEdit)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_DELETE, OnBnClickedDelete)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_COPY, OnBnClickedCopy)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_MOVE_UP, OnBnClickedUp)
	ON_BN_CLICKED(IDC_CONFIG_EVENT_MOVE_DOWN, OnBnClickedDown)
	ON_STN_DBLCLK(IDC_CONFIG_EVENT_INFO, OnDblclickConfigInfo)
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
	m_DlgFixup.clear();
}


void CDlgConfigEvent::FillSubNames(bool bInit)
{
	if (m_bHasSubNames)
	{
		m_ctrlSubNames.ShowWindow(SW_SHOW);
		m_ctrlSubNamesNew.ShowWindow(SW_SHOW);
		m_ctrlSubNamesEdit.ShowWindow(SW_SHOW);
		m_ctrlSubNamesDelete.ShowWindow(SW_SHOW);
		if (bInit)
		{
			m_ctrlSubNames.ResetContent();
			std::set<tstring> subNames;
			m_pEvent->GetSubNames(subNames);
			for (std::set<tstring>::const_iterator iter = subNames.begin();
				iter != subNames.end();
				++iter)
			{
				m_ctrlSubNames.AddString(iter->c_str());
			}
		}
	}
	else
	{
		m_ctrlSubNames.ShowWindow(SW_HIDE);
		m_ctrlSubNamesNew.ShowWindow(SW_HIDE);
		m_ctrlSubNamesEdit.ShowWindow(SW_HIDE);
		m_ctrlSubNamesDelete.ShowWindow(SW_HIDE);
	}
}


CListPtrData<ARBConfigScoringPtr>* CDlgConfigEvent::GetScoringData(int index) const
{
	CListData* pData = m_ctrlMethods.GetData(index);
	return dynamic_cast<CListPtrData<ARBConfigScoringPtr>*>(pData);
}


CListPtrData<ARBConfigTitlePointsPtr>* CDlgConfigEvent::GetTitleData(int index) const
{
	CListData* pData = m_ctrlPointsList.GetData(index);
	return dynamic_cast<CListPtrData<ARBConfigTitlePointsPtr>*>(pData);
}


CListPtrData<ARBConfigLifetimePointsPtr>* CDlgConfigEvent::GetLifetimeData(int index) const
{
	CListData* pData = m_ctrlPointsList.GetData(index);
	return dynamic_cast<CListPtrData<ARBConfigLifetimePointsPtr>*>(pData);
}


CListPtrData<ARBConfigPlaceInfoPtr>* CDlgConfigEvent::GetPlacementData(int index) const
{
	CListData* pData = m_ctrlPointsList.GetData(index);
	return dynamic_cast<CListPtrData<ARBConfigPlaceInfoPtr>*>(pData);
}


CString CDlgConfigEvent::GetListName(ARBConfigScoringPtr pScoring) const
{
	CString all;
	all.LoadString(IDS_ALL);
	CString str;
	if (pScoring->GetDivision() == WILDCARD_DIVISION)
		str = all;
	else
		str = pScoring->GetDivision().c_str();
	str += _T(" / ");
	if (pScoring->GetLevel() == WILDCARD_LEVEL)
		str += all;
	else
		str += pScoring->GetLevel().c_str();
	tstring validStr = ARBDate::GetValidDateString(pScoring->GetValidFrom(), pScoring->GetValidTo());
	if (0 < validStr.length())
	{
		str += ' ';
		str += validStr.c_str();
	}
	return str;
}


void CDlgConfigEvent::FillControls()
{
	BOOL bEnable = FALSE;
	int idxMethod = m_ctrlMethods.GetCurSel();
	m_ctrlInfo.SetWindowText(_T(""));
	m_ctrlPointsList.ResetContent();
	m_ctrlNote.SetWindowText(_T(""));
	if (LB_ERR != idxMethod)
	{
		CListPtrData<ARBConfigScoringPtr>* pScoringData = GetScoringData(idxMethod);
		if (pScoringData)
		{
			bEnable = TRUE;
			ARBConfigScoringPtr pScoring = pScoringData->GetData();
			// Get info
			{
				CString str1, str2;
				otstringstream info;
				ARBConfigScoring::ScoringStyle style = pScoring->GetScoringStyle();
				str1.LoadString(IDS_CONFIGEVENT_STYLE);
				info << (LPCTSTR)str1 << _T(": ")
					<< ARBConfigScoring::GetScoringStyleStr(style)
					<< _T("\r\n");
				// The following strings should be the same as they are in
				// the Method Configuration dialog.
				switch (style)
				{
				default:
					break;
				case ARBConfigScoring::eFaultsThenTime:
				case ARBConfigScoring::eFaults100ThenTime:
				case ARBConfigScoring::eFaults200ThenTime:
					str1.LoadString(IDS_CONFIGEVENT_TIMEFAULTMULT);
					info << (LPCTSTR)str1 << _T(": ")
						<< pScoring->TimeFaultMultiplier();
					break;
				case ARBConfigScoring::eOCScoreThenTime:
    				str1.LoadString(IDS_CONFIGEVENT_REQOPEN);
    				str2.LoadString(IDS_CONFIGEVENT_REQCLOSE);
					info << (LPCTSTR)str1 << _T(": ")
						<< pScoring->GetRequiredOpeningPoints()
						<< _T("; ") << (LPCTSTR)str2 << _T(": ")
						<< pScoring->GetRequiredClosingPoints();
					if (pScoring->SubtractTimeFaultsFromScore())
					{
						str1.LoadString(IDS_CONFIGEVENT_TF_FROMSCORE);
						info << _T("; ") << (LPCTSTR)str1;
					}
					if (pScoring->ComputeTimeFaultsUnder())
					{
						str1.LoadString(IDS_CONFIGEVENT_TF_UNDER);
						info << _T("; ") << (LPCTSTR)str1;
					}
					if (pScoring->ComputeTimeFaultsOver())
					{
						str1.LoadString(IDS_CONFIGEVENT_TF_OVER);
						info << _T("; ") << (LPCTSTR)str1;
					}
					break;
				case ARBConfigScoring::eScoreThenTime:
					str1.LoadString(IDS_POINTS);
					info << (LPCTSTR)str1 << _T(": ")
						<< pScoring->GetRequiredOpeningPoints();
					if (pScoring->SubtractTimeFaultsFromScore())
					{
						str1.LoadString(IDS_CONFIGEVENT_TF_FROMSCORE);
						info << _T("; ") << (LPCTSTR)str1;
					}
					if (pScoring->ComputeTimeFaultsUnder())
					{
						str1.LoadString(IDS_CONFIGEVENT_TF_UNDER);
						info << _T("; ") << (LPCTSTR)str1;
					}
					if (pScoring->ComputeTimeFaultsOver())
					{
						str1.LoadString(IDS_CONFIGEVENT_TF_OVER);
						info << _T("; ") << (LPCTSTR)str1;
					}
					break;
				case ARBConfigScoring::eTimePlusFaults:
					str1.LoadString(IDS_CONFIGEVENT_TIMEFAULTMULT);
					info << (LPCTSTR)str1 << _T(": ")
						<< pScoring->TimeFaultMultiplier();
					if (pScoring->QsMustBeClean())
					{
						// This string is slightly different: Just dropped
						// the 'Time+Fault' at start.
						str1.LoadString(IDS_CONFIGEVENT_CLEANQ);
						info << _T("; ") << (LPCTSTR)str1;
					}
					if (pScoring->ComputeTimeFaultsUnder())
					{
						str1.LoadString(IDS_CONFIGEVENT_TF_UNDER);
						info << _T("; ") << (LPCTSTR)str1;
					}
					if (pScoring->ComputeTimeFaultsOver())
					{
						str1.LoadString(IDS_CONFIGEVENT_TF_OVER);
						info << _T("; ") << (LPCTSTR)str1;
					}
					break;
				}
				if (pScoring->DropFractions())
				{
					str1.LoadString(IDS_CONFIGEVENT_DROPFRAC);
					info << _T("; ") << (LPCTSTR)str1;
				}
				if (pScoring->HasBonusPts())
				{
					str1.LoadString(IDS_CONFIGEVENT_BONUS);
					info << _T("; ") << (LPCTSTR)str1;
				}
				if (pScoring->HasSuperQ())
				{
					str1.LoadString(IDS_CONFIGEVENT_SUPERQ);
					info << _T("; ") << (LPCTSTR)str1;
				}
				if (pScoring->HasSpeedPts())
				{
					str1.LoadString(IDS_CONFIGEVENT_SPEEDPTS);
					info << _T("; ") << (LPCTSTR)str1;
					if (0 < pScoring->GetPlaceInfo().size())
					{
						info << _T(" [");
						int idx = 0;
						for (ARBConfigPlaceInfoList::iterator iter = pScoring->GetPlaceInfo().begin();
							iter != pScoring->GetPlaceInfo().end();
							++idx, ++iter)
						{
							if (0 < idx)
								info << _T(", ");
							info << (*iter)->GetPlace()
								<< _T("=")
								<< (*iter)->GetValue();
						}
						info << _T("]");
					}
				}
				m_ctrlInfo.SetWindowText(info.str().c_str());
			}
			// Take care of title points
			FillTitlePoints(pScoring);
			// And the note.
			CString str = pScoring->GetNote().c_str();
			str.Replace(_T("\n"), _T("\r\n"));
			m_ctrlNote.SetWindowText(str);
		}
	}
	m_ctrlEdit.EnableWindow(bEnable);
	m_ctrlDelete.EnableWindow(bEnable);
	m_ctrlCopy.EnableWindow(bEnable);
	m_ctrlUp.EnableWindow(bEnable && 1 < m_ctrlMethods.GetCount() && 0 != idxMethod);
	m_ctrlDown.EnableWindow(bEnable && 1 < m_ctrlMethods.GetCount() && m_ctrlMethods.GetCount() - 1 != idxMethod);
	int idxTitle = m_ctrlPointsList.GetCurSel();
	m_ctrlPointsList.EnableWindow(bEnable);
	m_ctrlPointsNew.EnableWindow(bEnable);
	m_ctrlPointsEdit.EnableWindow(bEnable && 0 <= idxTitle);
	m_ctrlPointsDelete.EnableWindow(bEnable && 0 <= idxTitle);
	m_ctrlNote.EnableWindow(bEnable);
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
		ARBConfigScoringPtr pScoring = (*iter);
		str = GetListName(pScoring);
		int index = m_ctrlMethods.AddString(str);
		m_ctrlMethods.SetData(index, new CListPtrData<ARBConfigScoringPtr>(pScoring));
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
				str += _T(" / ");
				str += (*iterLevel)->GetName().c_str();
				m_ctrlUnused.AddString(str);
			}
			// Remember, configuration doesn't do sublevels.
		}
	}
}


void CDlgConfigEvent::FillTitlePoints(ARBConfigScoringPtr pScoring)
{
	ARBConfigTitlePointsPtr pOldTitle;
	ARBConfigLifetimePointsPtr pOldLifetime;
	ARBConfigPlaceInfoPtr pOldPlacement;
	int idxTitle = m_ctrlPointsList.GetCurSel();
	if (LB_ERR != idxTitle)
	{
		CListPtrData<ARBConfigTitlePointsPtr>* pTitleData = GetTitleData(idxTitle);
		if (pTitleData)
		{
			pOldTitle = pTitleData->GetData();
		}
		CListPtrData<ARBConfigLifetimePointsPtr>* pLifeData = GetLifetimeData(idxTitle);
		if (pLifeData)
		{
			pOldLifetime = pLifeData->GetData();
		}
		CListPtrData<ARBConfigPlaceInfoPtr>* pPlacement = GetPlacementData(idxTitle);
		if (pPlacement)
		{
			pOldPlacement = pPlacement->GetData();
		}
	}
	m_ctrlPointsList.ResetContent();
	for (ARBConfigTitlePointsList::const_iterator iter = pScoring->GetTitlePoints().begin();
		iter != pScoring->GetTitlePoints().end();
		++iter)
	{
		ARBConfigTitlePointsPtr pTitle = (*iter);
		int idx = m_ctrlPointsList.AddString(pTitle->GetGenericName().c_str());
		if (LB_ERR != idx)
		{
			m_ctrlPointsList.SetData(idx,
				new CListPtrData<ARBConfigTitlePointsPtr>(pTitle));
			if (pOldTitle == pTitle)
				m_ctrlPointsList.SetCurSel(idx);
		}
	}
	for (ARBConfigLifetimePointsList::const_iterator iter2 = pScoring->GetLifetimePoints().begin();
		iter2 != pScoring->GetLifetimePoints().end();
		++iter2)
	{
		ARBConfigLifetimePointsPtr pLife = (*iter2);
		int idx = m_ctrlPointsList.AddString(pLife->GetGenericName().c_str());
		if (LB_ERR != idx)
		{
			m_ctrlPointsList.SetData(idx,
				new CListPtrData<ARBConfigLifetimePointsPtr>(pLife));
			if (pOldLifetime == pLife)
				m_ctrlPointsList.SetCurSel(idx);
		}
	}
	for (ARBConfigPlaceInfoList::const_iterator iter3 = pScoring->GetPlacements().begin();
		iter3 != pScoring->GetPlacements().end();
		++iter3)
	{
		ARBConfigPlaceInfoPtr pPlace = (*iter3);
		int idx = m_ctrlPointsList.AddString(pPlace->GetGenericName().c_str());
		if (LB_ERR != idx)
		{
			m_ctrlPointsList.SetData(idx,
				new CListPtrData<ARBConfigPlaceInfoPtr>(pPlace));
			if (pOldPlacement == pPlace)
				m_ctrlPointsList.SetCurSel(idx);
		}
	}
	m_ctrlPointsEdit.EnableWindow(FALSE);
	m_ctrlPointsDelete.EnableWindow(FALSE);
}


bool CDlgConfigEvent::SaveControls()
{
	// Save the last selected method.
	if (0 <= m_idxMethod)
	{
		CListPtrData<ARBConfigScoringPtr>* pScoringData = GetScoringData(m_idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		// Point/faults are already up-to-date.
		CString str;
		m_ctrlNote.GetWindowText(str);
		str.Replace(_T("\r\n"), _T("\n"));
		str.TrimRight();
		pScoring->SetNote((LPCTSTR)str);
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigEvent message handlers

BOOL CDlgConfigEvent::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();

	FillSubNames(true);
	FillMethodList();
	FillControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgConfigEvent::OnBnClickedSubNames()
{
	UpdateData(TRUE);
	FillSubNames();
	OnLbnSelchangeSubnames();
}


void CDlgConfigEvent::OnLbnSelchangeSubnames()
{
	BOOL bEnable = FALSE;
	if (LB_ERR != m_ctrlSubNames.GetCurSel())
		bEnable = TRUE;
	m_ctrlSubNamesEdit.EnableWindow(bEnable);
	m_ctrlSubNamesDelete.EnableWindow(bEnable);
}


void CDlgConfigEvent::OnBnClickedSubNamesNew()
{
	CDlgName dlg(_T(""), static_cast<UINT>(0), this);
	if (IDOK == dlg.DoModal())
	{
		int idx = m_ctrlSubNames.AddString(dlg.GetName());
		m_ctrlSubNames.SetCurSel(idx);
		OnLbnSelchangeSubnames();
	}
}


void CDlgConfigEvent::OnBnClickedSubNamesEdit()
{
	int idx = m_ctrlSubNames.GetCurSel();
	if (LB_ERR != idx)
	{
		CString name;
		m_ctrlSubNames.GetText(idx, name);
		CDlgName dlg(name, static_cast<UINT>(0), this);
		if (IDOK == dlg.DoModal())
		{
			m_ctrlSubNames.DeleteString(idx);
			m_ctrlSubNames.InsertString(idx, dlg.GetName());
			m_ctrlSubNames.SetCurSel(idx);
			OnLbnSelchangeSubnames();
		}
	}
}


void CDlgConfigEvent::OnBnClickedSubNamesDelete()
{
	int idx = m_ctrlSubNames.GetCurSel();
	if (LB_ERR != idx)
	{
		m_ctrlSubNames.DeleteString(idx);
		if (idx == m_ctrlSubNames.GetCount())
			--idx;
		m_ctrlSubNames.SetCurSel(idx);
		m_ctrlSubNamesEdit.EnableWindow(FALSE);
		m_ctrlSubNamesDelete.EnableWindow(FALSE);
	}
}


void CDlgConfigEvent::OnLbnDblclkMethods()
{
	OnBnClickedEdit();
}


void CDlgConfigEvent::OnLbnSelchangeMethods()
{
	UpdateData(TRUE);
	SaveControls();
	m_idxMethod = m_ctrlMethods.GetCurSel();
	FillControls();
}


void CDlgConfigEvent::OnBnClickedNew()
{
	UpdateData(TRUE);
	SaveControls();
	ARBConfigScoringPtr pScoring = m_Scorings.AddScoring();
	CString str = GetListName(pScoring);
	m_idxMethod = m_ctrlMethods.AddString(str);
	m_ctrlMethods.SetData(m_idxMethod, new CListPtrData<ARBConfigScoringPtr>(pScoring));
	m_ctrlMethods.SetCurSel(m_idxMethod);
	FillMethodList();
	FillControls();
}


void CDlgConfigEvent::OnBnClickedEdit()
{
	UpdateData(TRUE);
	int idxMethod = m_ctrlMethods.GetCurSel();
	if (LB_ERR != idxMethod)
	{
		CListPtrData<ARBConfigScoringPtr>* pScoringData = GetScoringData(idxMethod);
		CDlgConfigEventMethod dlg(m_pVenue, pScoringData->GetData(), this);
		if (IDOK == dlg.DoModal())
		{
			FillMethodList();
			FillControls();
		}
	}
}


void CDlgConfigEvent::OnBnClickedDelete()
{
	UpdateData(TRUE);
	int idxMethod = m_ctrlMethods.GetCurSel();
	if (LB_ERR != idxMethod)
	{
		if (m_idxMethod == idxMethod)
		{
			m_idxMethod = -1;
			m_ctrlMethods.SetCurSel(-1);
		}
		CListPtrData<ARBConfigScoringPtr>* pScoringData = GetScoringData(idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
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


void CDlgConfigEvent::OnBnClickedCopy()
{
	UpdateData(TRUE);
	int idxMethod = m_ctrlMethods.GetCurSel();
	if (LB_ERR != idxMethod)
	{
		CListPtrData<ARBConfigScoringPtr>* pScoringData = GetScoringData(idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		ARBConfigScoringPtr pNewScoring = m_Scorings.AddScoring();
		*pNewScoring = *pScoring;
		CString str = GetListName(pNewScoring);
		m_idxMethod = m_ctrlMethods.AddString(str);
		m_ctrlMethods.SetData(m_idxMethod, new CListPtrData<ARBConfigScoringPtr>(pNewScoring));
		m_ctrlMethods.SetCurSel(m_idxMethod);
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
		CListPtrData<ARBConfigScoringPtr>* pScoringData = GetScoringData(idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		for (ARBConfigScoringList::iterator iter = m_Scorings.begin();
			iter != m_Scorings.end();
			++iter)
		{
			if ((*iter) == pScoring)
			{
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
		CListPtrData<ARBConfigScoringPtr>* pScoringData = GetScoringData(idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		for (ARBConfigScoringList::iterator iter = m_Scorings.begin();
			iter != m_Scorings.end();
			++iter)
		{
			if ((*iter) == pScoring)
			{
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


void CDlgConfigEvent::OnDblclickConfigInfo()
{
	OnBnClickedEdit();
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
	OnPointsEdit();
}


void CDlgConfigEvent::OnPointsNew()
{
	UpdateData(TRUE);
	int idxMethod = m_ctrlMethods.GetCurSel();
	if (LB_ERR != idxMethod)
	{
		CListPtrData<ARBConfigScoringPtr>* pScoringData = GetScoringData(idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		if (pScoring)
		{
			CDlgConfigTitlePoints dlg(m_pVenue, 0.0, 0.0, CDlgConfigTitlePoints::eTitleNormal, this);
			if (IDOK == dlg.DoModal())
			{
				// The only reason this fails is if the faults entry exists.
				switch (dlg.Type())
				{
				default:
					assert(0);
				case CDlgConfigTitlePoints::eTitleNormal:
					if (!pScoring->GetTitlePoints().AddTitlePoints(dlg.Points(), dlg.Faults()))
						AfxMessageBox(IDS_TITLEPTS_EXISTS, MB_ICONEXCLAMATION);
					break;
				case CDlgConfigTitlePoints::eTitleLifetime:
					if (!pScoring->GetLifetimePoints().AddLifetimePoints(dlg.Points(), dlg.Faults()))
						AfxMessageBox(IDS_TITLEPTS_EXISTS, MB_ICONEXCLAMATION);
					break;
				case CDlgConfigTitlePoints::eTitlePlacement:
					if (!pScoring->GetPlacements().AddPlaceInfo(dlg.Place(), dlg.Points(), true))
						AfxMessageBox(IDS_TITLEPTS_EXISTS, MB_ICONEXCLAMATION);
					break;
				}
				FillTitlePoints(pScoring);
				OnSelchangePoints(); // To update buttons
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
		CListPtrData<ARBConfigScoringPtr>* pScoringData = GetScoringData(idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		CListPtrData<ARBConfigTitlePointsPtr>* pData1 = GetTitleData(idx);
		CListPtrData<ARBConfigLifetimePointsPtr>* pData2 = GetLifetimeData(idx);
		CListPtrData<ARBConfigPlaceInfoPtr>* pData3 = GetPlacementData(idx);
		ARBConfigTitlePointsPtr pTitle;
		ARBConfigLifetimePointsPtr pLife;
		ARBConfigPlaceInfoPtr pPlace;
		if (pData1)
			pTitle = pData1->GetData();
		if (pData2)
			pLife = pData2->GetData();
		if (pData3)
			pPlace = pData3->GetData();
		if (pScoring && (pTitle || pLife || pPlace))
		{
			double value;
			double points;
			CDlgConfigTitlePoints::ETitlePointType type;
			if (pTitle)
			{
				value = pTitle->GetFaults();
				points = pTitle->GetPoints();
				type = CDlgConfigTitlePoints::eTitleNormal;
			}
			else if (pLife)
			{
				value = pLife->GetFaults();
				points = pLife->GetPoints();
				type = CDlgConfigTitlePoints::eTitleLifetime;
			}
			else
			{
				value = pPlace->GetPlace();
				points = pPlace->GetValue();
				type = CDlgConfigTitlePoints::eTitlePlacement;
			}
			CDlgConfigTitlePoints dlg(m_pVenue, value, points, type);
			if (IDOK == dlg.DoModal())
			{
				if (type != dlg.Type()
				|| (pTitle && pTitle->GetFaults() != dlg.Faults())
				|| (pLife && pLife->GetFaults() != dlg.Faults())
				|| (pPlace && pPlace->GetPlace() != dlg.Place()))
				{
					// Clean up.
					if (type == dlg.Type())
					{
						if (pTitle)
							pScoring->GetTitlePoints().DeleteTitlePoints(pTitle->GetFaults());
						else if (pLife)
							pScoring->GetLifetimePoints().DeleteLifetimePoints(pLife->GetFaults());
						else
							pScoring->GetPlacements().DeletePlaceInfo(pPlace->GetPlace());
					}
					bool bOk = false;
					switch (dlg.Type())
					{
					default:
						assert(0);
					case CDlgConfigTitlePoints::eTitleNormal:
						bOk = pScoring->GetTitlePoints().AddTitlePoints(dlg.Points(), dlg.Faults());
						if (!bOk)
							AfxMessageBox(IDS_TITLEPTS_EXISTS, MB_ICONEXCLAMATION);
						break;
					case CDlgConfigTitlePoints::eTitleLifetime:
						bOk = pScoring->GetLifetimePoints().AddLifetimePoints(dlg.Points(), dlg.Faults());
						if (!bOk)
							AfxMessageBox(IDS_TITLEPTS_EXISTS, MB_ICONEXCLAMATION);
						break;
					case CDlgConfigTitlePoints::eTitlePlacement:
						bOk = pScoring->GetPlacements().AddPlaceInfo(dlg.Place(), dlg.Points(), true);
						if (!bOk)
							AfxMessageBox(IDS_TITLEPTS_EXISTS, MB_ICONEXCLAMATION);
						break;
					}
					if (bOk && type != dlg.Type())
					{
						if (pTitle)
							pScoring->GetTitlePoints().DeleteTitlePoints(pTitle->GetFaults());
						else if (pLife)
							pScoring->GetLifetimePoints().DeleteLifetimePoints(pLife->GetFaults());
						else
							pScoring->GetPlacements().DeletePlaceInfo(pPlace->GetPlace());
					}
				}
				else
				{
					if (pTitle)
						pTitle->SetPoints(dlg.Points());
					else if (pLife)
						pLife->SetPoints(dlg.Points());
					else
						pPlace->SetValue(dlg.Points());
				}
				FillTitlePoints(pScoring);
				OnSelchangePoints(); // To update buttons
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
		CListPtrData<ARBConfigScoringPtr>* pScoringData = GetScoringData(idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		CListPtrData<ARBConfigTitlePointsPtr>* pData1 = GetTitleData(idx);
		CListPtrData<ARBConfigLifetimePointsPtr>* pData2 = GetLifetimeData(idx);
		CListPtrData<ARBConfigPlaceInfoPtr>* pData3 = GetPlacementData(idx);
		ARBConfigTitlePointsPtr pTitle;
		ARBConfigLifetimePointsPtr pLife;
		ARBConfigPlaceInfoPtr pPlace;
		if (pData1)
			pTitle = pData1->GetData();
		if (pData2)
			pLife = pData2->GetData();
		if (pData3)
			pPlace = pData3->GetData();
		if (pScoring && (pTitle || pLife || pPlace))
		{
			if (pTitle)
				pScoring->GetTitlePoints().DeleteTitlePoints(pTitle->GetFaults());
			else if (pLife)
				pScoring->GetLifetimePoints().DeleteLifetimePoints(pLife->GetFaults());
			else
				pScoring->GetPlacements().DeletePlaceInfo(pPlace->GetPlace());
			m_ctrlPointsList.DeleteString(idx);
			if (idx == m_ctrlPointsList.GetCount())
				--idx;
			m_ctrlPointsList.SetCurSel(idx);
			OnSelchangePoints(); // To update buttons
		}
	}
}


static ARBDate DateEndPoints(ARBDate from1, ARBDate to1, ARBDate from2, ARBDate to2, bool bFirst)
{
	std::vector<ARBDate> dates;
	if (from1.IsValid())
		dates.push_back(from1);
	if (to1.IsValid())
		dates.push_back(to1);
	if (from2.IsValid())
		dates.push_back(from2);
	if (to2.IsValid())
		dates.push_back(to2);
	ARBDate date;
	if (1 <= dates.size())
	{
		date = dates[0];
		for (size_t n = 1; n < dates.size(); ++n)
		{
			if (bFirst)
			{
				if (dates[n] < date)
					date = dates[n];
			}
			else
			{
				if (dates[n] > date)
					date = dates[n];
			}
		}
	}
	return date;
}


void CDlgConfigEvent::OnOK()
{
	if (!UpdateData(TRUE))
		return;
	if (!SaveControls())
		return;
#if _MSC_VER >= 1300
	m_Name.Trim();
	m_Desc.Trim();
#else
	m_Name.TrimRight();
	m_Name.TrimLeft();
	m_Desc.TrimRight();
	m_Desc.TrimLeft();
#endif
	if (m_Name.IsEmpty())
	{
		GotoDlgCtrl(GetDlgItem(IDC_CONFIG_EVENT));
		return;
	}

	// Check if there is any overlap.
	bool bOverlap = false;
	{
		// No need to clone, we can just copy the pointers
		ARBConfigScoringList scorings(m_Scorings);
		while (!bOverlap && 1 < scorings.size())
		{
			// Extract all similar methods.
			ARBConfigScoringList items;
			ARBConfigScoringList::iterator iter = scorings.begin();
			ARBConfigScoringPtr pScoring = items.AddScoring();
			*pScoring = *(*iter);
			iter = scorings.erase(scorings.begin());
			while (iter != scorings.end())
			{
				ARBConfigScoringPtr pScoring2 = *iter;
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
				ARBConfigScoringPtr pScore = *iter;
				for (++iter; !bOverlap && iter != items.end(); ++iter)
				{
					ARBConfigScoringPtr pScoring2 = *iter;
					ARBDate from1 = pScore->GetValidFrom();
					ARBDate to1 = pScore->GetValidTo();
					ARBDate from2 = pScoring2->GetValidFrom();
					ARBDate to2 = pScoring2->GetValidTo();
					ARBDate low = DateEndPoints(from1, to1, from2, to2, true);
					if (!low.IsValid())
					{
						// If date isn't valid, none of the dates are.
						bOverlap = true;
						break;
					}
					ARBDate hi = DateEndPoints(from1, to1, from2, to2, false);
					--low; // Push valid dates off by 1 so they are not in current range
					++hi;
					if (!from1.IsValid())
						from1 = low;
					if (!to1.IsValid())
						to1 = hi;
					if (!from2.IsValid())
						from2 = low;
					if (!to2.IsValid())
						to2 = hi;
					// Now that we've tweaked the dates, we can do plain old
					// isBetween comparisons. This also means that if both
					// methods have no date set on either the from or to dates,
					// it will be properly flagged (since they are equal)
					if (from1.isBetween(from2, to2) || to1.isBetween(from2, to2))
					{
						bOverlap = true;
					}
				}
				items.erase(items.begin());
			}
		}
	}
	if (bOverlap)
	{
		if (IDYES != AfxMessageBox(IDS_CONFIGEVENT_OVERLAPDATES, MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2))
			return;
	}

	ClearFixups();
	if (m_pEvent->GetName() != (LPCTSTR)m_Name)
	{
		if (m_pVenue->GetEvents().FindEvent((LPCTSTR)m_Name))
		{
			AfxMessageBox(IDS_NAME_IN_USE);
			GotoDlgCtrl(GetDlgItem(IDC_CONFIG_EVENT));
			return;
		}
		if (!m_bNewEntry)
			m_DlgFixup.push_back(ARBConfigActionRenameEvent::New(m_pVenue->GetName(), m_pEvent->GetName(), (LPCTSTR)m_Name));
		m_pEvent->SetName((LPCTSTR)m_Name);
	}
	// m_Book is only valid when editing an existing entry.
	/*
	if (!m_bNewEntry)
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
	*/

	m_Desc.Replace(_T("\r\n"), _T("\n"));
	m_pEvent->SetDesc((LPCTSTR)m_Desc);
	m_pEvent->SetHasTable(m_bHasTable == TRUE ? true : false);
	m_pEvent->SetHasPartner(m_bHasPartners == TRUE ? true : false);
	m_pEvent->SetHasSubNames(m_bHasSubNames == TRUE ? true : false);
	if (m_bHasSubNames)
	{
		std::set<tstring> subNames;
		int nCount = m_ctrlSubNames.GetCount();
		for (int i = 0; i < nCount; ++i)
		{
			CString str;
			m_ctrlSubNames.GetText(i, str);
			str.TrimRight();
			str.TrimLeft();
			if (!str.IsEmpty())
				subNames.insert((LPCTSTR)str);
		}
		m_pEvent->SetSubNames(subNames);
	}
	// No need to clone them, just move them.
	m_pEvent->GetScorings() = m_Scorings;
	CDlgBaseDialog::OnOK();
}
