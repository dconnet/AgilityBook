/*
 * Copyright © 2004 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgConfigTitle class
 * @author David Connet
 *
 * Revision History
 * @li 2004-01-05 DRC Created.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgConfigTitle.h"

#include "ARBConfigTitle.h"
#include "DlgName.h"
#include ".\dlgconfigtitle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigTitle dialog

CDlgConfigTitle::CDlgConfigTitle(const ARBConfigTitleList& titles,
	LPCTSTR name, const ARBConfigTitle* pTitle, CWnd* pParent)
	: CDialog(CDlgConfigTitle::IDD, pParent)
	, m_Titles(titles)
	, m_Name(name)
	, m_LongName("")
	, m_Desc("")
{
	if (pTitle)
	{
		m_LongName = pTitle->GetLongName().c_str();
		for (ARBConfigTitleRequiresList::const_iterator iter = pTitle->GetRequires().begin();
			iter != pTitle->GetRequires().end();
			++iter)
		{
			m_ReqTitles.insert(*iter);
		}
		m_Desc = pTitle->GetDescription().c_str();
		m_Desc.Replace("\n", "\r\n");
	}
	//{{AFX_DATA_INIT(CDlgConfigTitle)
	//}}AFX_DATA_INIT
}

CDlgConfigTitle::~CDlgConfigTitle()
{
}

void CDlgConfigTitle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConfigTitle)
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDX_Text(pDX, IDC_LONG_NAME, m_LongName);
	DDX_Control(pDX, IDC_TITLES, m_ctrlRequires);
	DDX_Text(pDX, IDC_DESC, m_Desc);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgConfigTitle, CDialog)
	//{{AFX_MSG_MAP(CDlgConfigTitle)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TITLES, OnLvnItemchangedTitles)
	ON_NOTIFY(NM_DBLCLK, IDC_TITLES, OnNMDblclkTitles)
	ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
	ON_BN_CLICKED(IDC_EDIT, OnBnClickedEdit)
	ON_BN_CLICKED(IDC_DELETE, OnBnClickedDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

std::string CDlgConfigTitle::GetName() const
{
	return std::string((LPCTSTR)m_Name);
}

void CDlgConfigTitle::SetTitleData(ARBConfigTitle* pTitle)
{
	pTitle->SetLongName((LPCTSTR)m_LongName);
	pTitle->GetRequires() = m_ReqTitles;
	m_Desc.Replace("\r\n", "\n");
	pTitle->SetDescription((LPCTSTR)m_Desc);
}

void CDlgConfigTitle::UpdateButtons()
{
	UINT selected = m_ctrlRequires.GetSelectedCount();
	if (0 == selected)
	{
		GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_EDIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_DELETE)->EnableWindow(TRUE);
	}
}

bool CDlgConfigTitle::VerifyTitle(const ARBConfigTitle* pTitleToReplace, const ARBConfigTitle* pTitleToAdd)
{
	bool bOk = false;
	if (pTitleToReplace != pTitleToAdd
	&& m_Name != pTitleToAdd->GetName().c_str())
	{
		std::vector<const ARBConfigTitle*> titles;
		// Build a list of titles to check. Include ourselves so we can
		// check for circular references.
		const ARBConfigTitle* pTitle = m_Titles.FindTitle((LPCTSTR)m_Name);
		if (pTitle)
			titles.push_back(pTitle);
		for (int i = 0; i < m_ctrlRequires.GetItemCount(); ++i)
		{
			pTitle = reinterpret_cast<const ARBConfigTitle*>(m_ctrlRequires.GetItemData(i));
			if (pTitle != pTitleToReplace)
				titles.push_back(pTitle);
		}
		bOk = true;
		if (0 < titles.size())
		{
			for (std::vector<const ARBConfigTitle*>::iterator iter = titles.begin();
				iter != titles.end();
				++iter)
			{
				if (m_Titles.Requires(*iter, pTitleToAdd->GetName()))
				{
					bOk = false;
					CString msg;
					msg.Format("The title '%s' is already included via existing requirements.", pTitleToAdd->GetName().c_str());
					AfxMessageBox(msg, MB_ICONWARNING);
					break;
				}
				if (m_Titles.Requires(pTitleToAdd, (*iter)->GetName()))
				{
					bOk = false;
					CString msg;
					msg.Format("The title '%s' cannot be included because of existing requirements.", pTitleToAdd->GetName().c_str());
					AfxMessageBox(msg, MB_ICONWARNING);
					break;
				}
			}
		}
	}
	return bOk;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigTitle message handlers

BOOL CDlgConfigTitle::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_ctrlRequires.SetExtendedStyle(m_ctrlRequires.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	m_ctrlRequires.InsertColumn(0, "");
	m_ctrlRequires.InsertColumn(1, "");
	int i = 0;
	for (ARBConfigTitleRequiresList::iterator iter = m_ReqTitles.begin();
		iter != m_ReqTitles.end();
		++iter)
	{
		const ARBConfigTitle* pTitle = m_Titles.FindTitle(*iter);
		if (pTitle)
		{
			int idx = m_ctrlRequires.InsertItem(i++, pTitle->GetName().c_str());
			m_ctrlRequires.SetItemText(idx, 1, pTitle->GetLongName().c_str());
			m_ctrlRequires.SetItemData(idx, reinterpret_cast<LPARAM>(pTitle));
		}
	}
	m_ctrlRequires.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_ctrlRequires.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
	UpdateButtons();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgConfigTitle::OnLvnItemchangedTitles(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	//LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	UpdateButtons();
	*pResult = 0;
}

void CDlgConfigTitle::OnNMDblclkTitles(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnBnClickedEdit();
	*pResult = 0;
}

void CDlgConfigTitle::OnBnClickedAdd()
{
	CDlgName dlg("", "Enter Name of Title", this);
	if (IDOK == dlg.DoModal())
	{
		const ARBConfigTitle* pTitle = m_Titles.FindTitle(dlg.GetName());
		if (pTitle)
		{
			bool bAdd = true;
			for (int i = 0; i < m_ctrlRequires.GetItemCount(); ++i)
			{
				const ARBConfigTitle* pOldTitle = reinterpret_cast<const ARBConfigTitle*>(m_ctrlRequires.GetItemData(i));
				if (pTitle == pOldTitle)
				{
					bAdd = false;
					break;
				}
			}
			if (bAdd)
			{
				if (VerifyTitle(NULL, pTitle))
				{
					int idx = m_ctrlRequires.InsertItem(m_ctrlRequires.GetItemCount(), pTitle->GetName().c_str());
					m_ctrlRequires.SetItemText(idx, 1, pTitle->GetLongName().c_str());
					m_ctrlRequires.SetItemData(idx, reinterpret_cast<LPARAM>(pTitle));
					m_ctrlRequires.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
					m_ctrlRequires.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
				}
			}
			else
				AfxMessageBox("Title already is defined.", MB_ICONINFORMATION);
		}
		else
		{
			CString msg;
			msg.Format("Unable to find title '%s'", dlg.GetName());
			AfxMessageBox(msg, MB_ICONEXCLAMATION);
		}
	}
}

void CDlgConfigTitle::OnBnClickedEdit()
{
	int i = m_ctrlRequires.GetSelection();
	if (0 <= i)
	{
		const ARBConfigTitle* pOldTitle = reinterpret_cast<const ARBConfigTitle*>(m_ctrlRequires.GetItemData(i));
		CDlgName dlg(pOldTitle->GetName().c_str(), "Enter Name of Title", this);
		if (IDOK == dlg.DoModal())
		{
			const ARBConfigTitle* pTitle = m_Titles.FindTitle(dlg.GetName());
			if (pTitle)
			{
				if (VerifyTitle(pOldTitle, pTitle))
				{
					m_ctrlRequires.SetItemText(i, 0, pTitle->GetName().c_str());
					m_ctrlRequires.SetItemText(i, 1, pTitle->GetLongName().c_str());
					m_ctrlRequires.SetItemData(i, reinterpret_cast<LPARAM>(pTitle));
					m_ctrlRequires.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
					m_ctrlRequires.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
				}
			}
			else
			{
				CString msg;
				msg.Format("Unable to find title '%s'", dlg.GetName());
				AfxMessageBox(msg, MB_ICONEXCLAMATION);
			}
		}
	}
}

void CDlgConfigTitle::OnBnClickedDelete()
{
	int i = m_ctrlRequires.GetSelection();
	if (0 <= i)
		m_ctrlRequires.DeleteItem(i);
}

void CDlgConfigTitle::OnOK()
{
	if (!UpdateData(TRUE))
		return;
#if _MSC_VER >= 1300
	m_Name.Trim();
	m_LongName.Trim();
	m_Desc.Trim();
#else
	m_Name.TrimRight();
	m_Name.TrimLeft();
	m_LongName.TrimRight();
	m_LongName.TrimLeft();
	m_Desc.TrimRight();
	m_Desc.TrimLeft();
#endif
	UpdateData(FALSE); // Stuff what we did back.
	if (m_Name.IsEmpty())
	{
		MessageBeep(0);
		GotoDlgCtrl(GetDlgItem(IDC_NAME));
		return;
	}
	m_ReqTitles.clear();
	for (int i = 0; i < m_ctrlRequires.GetItemCount(); ++i)
	{
		const ARBConfigTitle* pTitle = reinterpret_cast<const ARBConfigTitle*>(m_ctrlRequires.GetItemData(i));
		m_ReqTitles.insert(pTitle->GetName());
	}
	CDialog::OnOK();
}
