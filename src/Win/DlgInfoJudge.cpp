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

CDlgInfoJudge::CDlgInfoJudge(CAgilityBookDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInfoJudge::IDD, pParent)
	, m_pDoc(pDoc)
	, m_Info(pDoc->GetInfo().GetJudgeInfo())
{
	m_pDoc->GetAllJudges(m_NamesInUse, false);
	//{{AFX_DATA_INIT(CDlgInfoJudge)
	//}}AFX_DATA_INIT
}

void CDlgInfoJudge::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgInfoJudge)
	DDX_Control(pDX, IDC_JUDGE_DELETE, m_ctrlDelete);
	DDX_Control(pDX, IDC_JUDGE, m_ctrlJudge);
	DDX_Control(pDX, IDC_JUDGE_COMMENTS, m_ctrlComment);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgInfoJudge, CDialog)
	//{{AFX_MSG_MAP(CDlgInfoJudge)
	ON_BN_CLICKED(IDC_JUDGE_NEW, OnNew)
	ON_BN_CLICKED(IDC_JUDGE_DELETE, OnDelete)
	ON_CBN_SELCHANGE(IDC_JUDGE, OnSelchangeJudge)
	ON_EN_KILLFOCUS(IDC_JUDGE_COMMENTS, OnKillfocusComments)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgInfoJudge message handlers

BOOL CDlgInfoJudge::OnInitDialog()
{
	CDialog::OnInitDialog();

	std::set<std::string> names;
	m_pDoc->GetAllJudges(names);
	for (std::set<std::string>::iterator iter = names.begin(); iter != names.end(); ++iter)
	{
		m_ctrlJudge.AddString((*iter).c_str());
	}
	ARBDogRun const* pRun = m_pDoc->GetCurrentRun();
	int index = CB_ERR;
	if (pRun)
		index = m_ctrlJudge.SelectString(-1, pRun->GetJudge().c_str());
	if (-1 == index)
		m_ctrlJudge.SetCurSel(0);
	OnSelchangeJudge();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgInfoJudge::OnNew()
{
	CDlgName dlg("", "", this);
	if (IDOK == dlg.DoModal())
	{
		ARBInfoJudge* judge = m_Info.AddJudge((LPCTSTR)dlg.GetName());
		if (judge)
		{
			m_ctrlJudge.AddString(judge->GetName().c_str());
			m_ctrlComment.SetWindowText("");
		}
		else
			judge = m_Info.FindJudge((LPCTSTR)dlg.GetName());
		if (judge)
		{
			m_ctrlJudge.SelectString(-1, judge->GetName().c_str());
			OnSelchangeJudge();
		}
	}
}

void CDlgInfoJudge::OnDelete()
{
	int index = m_ctrlJudge.GetCurSel();
	if (CB_ERR != index)
	{
		CString name;
		m_ctrlJudge.GetLBText(index, name);
		if (m_NamesInUse.end() == m_NamesInUse.find((LPCTSTR)name))
		{
			m_ctrlJudge.DeleteString(index);
			ARBInfoJudge* judge = m_Info.FindJudge((LPCTSTR)name);
			if (judge)
				m_Info.DeleteJudge(judge);
			if (index == m_ctrlJudge.GetCount())
				--index;
			if (0 <= index)
				m_ctrlJudge.SetCurSel(index);
			OnSelchangeJudge();
		}
		else
			MessageBeep(0);
	}
}

void CDlgInfoJudge::OnSelchangeJudge()
{
	BOOL bEnable = FALSE;
	CString data;
	int index = m_ctrlJudge.GetCurSel();
	if (CB_ERR != index)
	{
		CString name;
		m_ctrlJudge.GetLBText(index, name);
		ARBInfoJudge* judge = m_Info.FindJudge((LPCTSTR)name);
		if (judge)
			data = judge->GetComment().c_str();
		if (m_NamesInUse.end() == m_NamesInUse.find((LPCTSTR)name))
			bEnable = TRUE;
	}
	data.Replace("\n", "\r\n");
	m_ctrlComment.SetWindowText(data);
	m_ctrlDelete.EnableWindow(bEnable);
}

void CDlgInfoJudge::OnKillfocusComments()
{
	int index = m_ctrlJudge.GetCurSel();
	if (CB_ERR != index)
	{
		CString name;
		m_ctrlJudge.GetLBText(index, name);
		ARBInfoJudge* judge = m_Info.FindJudge((LPCTSTR)name);
		if (!judge)
			judge = m_Info.AddJudge((LPCTSTR)name);
		if (!judge)
			return;
		CString data;
		m_ctrlComment.GetWindowText(data);
		data.TrimRight();
		data.Replace("\r\n", "\n");
		judge->SetComment((LPCTSTR)data);
	}
}

void CDlgInfoJudge::OnOK()
{
	// Not bothering to call UpdateData because we aren't exchanging any
	// data. We're using the controls directly.
	m_Info.CondenseContent(m_NamesInUse);
	if (m_pDoc->GetInfo().GetJudgeInfo() != m_Info)
	{
		m_pDoc->GetInfo().GetJudgeInfo() = m_Info;
		m_pDoc->SetModifiedFlag();
	}
	CDialog::OnOK();
}
