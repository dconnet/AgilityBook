/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgInfoJudge class
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-07 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgInfoJudge.h"

#include "AgilityBookDoc.h"
#include "DlgName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgInfoJudge dialog

CDlgInfoJudge::CDlgInfoJudge(CAgilityBookDoc* pDoc, eInfoType inType, CWnd* pParent /*=NULL*/)
	: CDlgBaseDialog(CDlgInfoJudge::IDD, pParent)
	, m_pDoc(pDoc)
	, m_Type(inType)
	, m_InfoClub(pDoc->GetInfo().GetClubInfo())
	, m_InfoJudge(pDoc->GetInfo().GetJudgeInfo())
	, m_InfoLocation(pDoc->GetInfo().GetLocationInfo())
{
	switch (m_Type)
	{
	case eClubInfo:
		m_pDoc->GetAllClubNames(m_NamesInUse, false);
		break;
	case eJudgeInfo:
		m_pDoc->GetAllJudges(m_NamesInUse, false);
		break;
	case eLocationInfo:
		m_pDoc->GetAllTrialLocations(m_NamesInUse, false);
		break;
	}
	//{{AFX_DATA_INIT(CDlgInfoJudge)
	//}}AFX_DATA_INIT
}

void CDlgInfoJudge::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgInfoJudge)
	DDX_Control(pDX, IDC_JUDGE_DELETE, m_ctrlDelete);
	DDX_Control(pDX, IDC_JUDGE, m_ctrlNames);
	DDX_Control(pDX, IDC_JUDGE_COMMENTS, m_ctrlComment);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgInfoJudge, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgInfoJudge)
	ON_BN_CLICKED(IDC_JUDGE_NEW, OnNew)
	ON_BN_CLICKED(IDC_JUDGE_DELETE, OnDelete)
	ON_CBN_SELCHANGE(IDC_JUDGE, OnSelchangeName)
	ON_EN_KILLFOCUS(IDC_JUDGE_COMMENTS, OnKillfocusComments)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgInfoJudge message handlers

BOOL CDlgInfoJudge::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();

	CString caption;
	std::set<std::string> names;
	switch (m_Type)
	{
	case eClubInfo:
		caption.LoadString(IDS_INFO_CLUB);
		m_pDoc->GetAllClubNames(names);
		break;
	case eJudgeInfo:
		caption.LoadString(IDS_INFO_JUDGE);
		m_pDoc->GetAllJudges(names);
		break;
	case eLocationInfo:
		caption.LoadString(IDS_INFO_LOCATION);
		m_pDoc->GetAllTrialLocations(names);
		break;
	}
	SetWindowText(caption);
	for (std::set<std::string>::iterator iter = names.begin(); iter != names.end(); ++iter)
	{
		m_ctrlNames.AddString((*iter).c_str());
	}
	ARBDogTrial const* pTrial = m_pDoc->GetCurrentTrial();
	ARBDogRun const* pRun = m_pDoc->GetCurrentRun();
	int index = -1;
	switch (m_Type)
	{
	case eClubInfo:
		if (pTrial)
			index = m_ctrlNames.SelectString(-1, pTrial->GetClubs().GetPrimaryClub()->GetName().c_str());
		break;
	case eJudgeInfo:
		if (pRun)
			index = m_ctrlNames.SelectString(-1, pRun->GetJudge().c_str());
		break;
	case eLocationInfo:
		if (pTrial)
			index = m_ctrlNames.SelectString(-1, pTrial->GetLocation().c_str());
		break;
	}
	if (-1 == index)
		m_ctrlNames.SetCurSel(0);
	OnSelchangeName();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgInfoJudge::OnNew()
{
	CDlgName dlg("", "", this);
	if (IDOK == dlg.DoModal())
	{
		switch (m_Type)
		{
		case eClubInfo:
			{
				ARBInfoClub* club = m_InfoClub.AddClub((LPCTSTR)dlg.GetName());
				if (club)
				{
					m_ctrlNames.AddString(club->GetName().c_str());
					m_ctrlComment.SetWindowText("");
				}
				else
					club = m_InfoClub.FindClub((LPCTSTR)dlg.GetName());
				if (club)
				{
					m_ctrlNames.SelectString(-1, club->GetName().c_str());
					OnSelchangeName();
				}
			}
			break;
		case eJudgeInfo:
			{
				ARBInfoJudge* judge = m_InfoJudge.AddJudge((LPCTSTR)dlg.GetName());
				if (judge)
				{
					m_ctrlNames.AddString(judge->GetName().c_str());
					m_ctrlComment.SetWindowText("");
				}
				else
					judge = m_InfoJudge.FindJudge((LPCTSTR)dlg.GetName());
				if (judge)
				{
					m_ctrlNames.SelectString(-1, judge->GetName().c_str());
					OnSelchangeName();
				}
			}
			break;
		case eLocationInfo:
			{
				ARBInfoLocation* location = m_InfoLocation.AddLocation((LPCTSTR)dlg.GetName());
				if (location)
				{
					m_ctrlNames.AddString(location->GetName().c_str());
					m_ctrlComment.SetWindowText("");
				}
				else
					location = m_InfoLocation.FindLocation((LPCTSTR)dlg.GetName());
				if (location)
				{
					m_ctrlNames.SelectString(-1, location->GetName().c_str());
					OnSelchangeName();
				}
			}
			break;
		}

	}
}

void CDlgInfoJudge::OnDelete()
{
	int index = m_ctrlNames.GetCurSel();
	if (CB_ERR != index)
	{
		CString name;
		m_ctrlNames.GetLBText(index, name);
		if (m_NamesInUse.end() == m_NamesInUse.find((LPCTSTR)name))
		{
			m_ctrlNames.DeleteString(index);
			switch (m_Type)
			{
			case eClubInfo:
				{
					ARBInfoClub* club = m_InfoClub.FindClub((LPCTSTR)name);
					if (club)
						m_InfoClub.DeleteClub(club);
				}
				break;
			case eJudgeInfo:
				{
					ARBInfoJudge* judge = m_InfoJudge.FindJudge((LPCTSTR)name);
					if (judge)
						m_InfoJudge.DeleteJudge(judge);
				}
				break;
			case eLocationInfo:
				{
					ARBInfoLocation* location = m_InfoLocation.FindLocation((LPCTSTR)name);
					if (location)
						m_InfoLocation.DeleteLocation(location);
				}
				break;
			}
			if (index == m_ctrlNames.GetCount())
				--index;
			if (0 <= index)
				m_ctrlNames.SetCurSel(index);
			OnSelchangeName();
		}
		else
			MessageBeep(0);
	}
}

void CDlgInfoJudge::OnSelchangeName()
{
	BOOL bEnable = FALSE;
	CString data;
	int index = m_ctrlNames.GetCurSel();
	if (CB_ERR != index)
	{
		CString name;
		m_ctrlNames.GetLBText(index, name);
		switch (m_Type)
		{
		case eClubInfo:
			{
				ARBInfoClub* club = m_InfoClub.FindClub((LPCTSTR)name);
				if (club)
					data = club->GetComment().c_str();
			}
			break;
		case eJudgeInfo:
			{
				ARBInfoJudge* judge = m_InfoJudge.FindJudge((LPCTSTR)name);
				if (judge)
					data = judge->GetComment().c_str();
			}
			break;
		case eLocationInfo:
			{
				ARBInfoLocation* location = m_InfoLocation.FindLocation((LPCTSTR)name);
				if (location)
					data = location->GetComment().c_str();
			}
			break;
		}
		if (m_NamesInUse.end() == m_NamesInUse.find((LPCTSTR)name))
			bEnable = TRUE;
	}
	data.Replace("\n", "\r\n");
	m_ctrlComment.SetWindowText(data);
	m_ctrlDelete.EnableWindow(bEnable);
}

void CDlgInfoJudge::OnKillfocusComments()
{
	int index = m_ctrlNames.GetCurSel();
	if (CB_ERR != index)
	{
		CString name;
		m_ctrlNames.GetLBText(index, name);
		CString data;
		m_ctrlComment.GetWindowText(data);
		data.TrimRight();
		data.Replace("\r\n", "\n");
		switch (m_Type)
		{
		case eClubInfo:
			{
				ARBInfoClub* club = m_InfoClub.FindClub((LPCTSTR)name);
				if (!club)
					club = m_InfoClub.AddClub((LPCTSTR)name);
				if (!club)
					return;
				club->SetComment((LPCTSTR)data);
			}
			break;
		case eJudgeInfo:
			{
				ARBInfoJudge* judge = m_InfoJudge.FindJudge((LPCTSTR)name);
				if (!judge)
					judge = m_InfoJudge.AddJudge((LPCTSTR)name);
				if (!judge)
					return;
				judge->SetComment((LPCTSTR)data);
			}
			break;
		case eLocationInfo:
			{
				ARBInfoLocation* location = m_InfoLocation.FindLocation((LPCTSTR)name);
				if (!location)
					location = m_InfoLocation.AddLocation((LPCTSTR)name);
				if (!location)
					return;
				location->SetComment((LPCTSTR)data);
			}
			break;
		}
	}
}

void CDlgInfoJudge::OnOK()
{
	// Not bothering to call UpdateData because we aren't exchanging any
	// data. We're using the controls directly.
	switch (m_Type)
	{
	case eClubInfo:
		{
			m_InfoClub.CondenseContent(m_NamesInUse);
			if (m_pDoc->GetInfo().GetClubInfo() != m_InfoClub)
			{
				m_pDoc->GetInfo().GetClubInfo() = m_InfoClub;
				m_pDoc->SetModifiedFlag();
			}
		}
		break;
	case eJudgeInfo:
		{
			m_InfoJudge.CondenseContent(m_NamesInUse);
			if (m_pDoc->GetInfo().GetJudgeInfo() != m_InfoJudge)
			{
				m_pDoc->GetInfo().GetJudgeInfo() = m_InfoJudge;
				m_pDoc->SetModifiedFlag();
			}
		}
		break;
	case eLocationInfo:
		{
			m_InfoLocation.CondenseContent(m_NamesInUse);
			if (m_pDoc->GetInfo().GetLocationInfo() != m_InfoLocation)
			{
				m_pDoc->GetInfo().GetLocationInfo() = m_InfoLocation;
				m_pDoc->SetModifiedFlag();
			}
		}
		break;
	}
	CDlgBaseDialog::OnOK();
}
