/*
 * Copyright © 2004 Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgRunLint class
 * @author David Connet
 *
 * Revision History
 * @li 2004-03-30 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgRunLink.h"

#include "ARBDogRun.h"
#include "DlgSelectURL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRunLink dialog

CDlgRunLink::CDlgRunLink(CAgilityBookDoc* pDoc, ARBDogRun* pRun)
	: CPropertyPage(CDlgRunLink::IDD)
	, m_pDoc(pDoc)
	, m_Run(pRun)
{
	//{{AFX_DATA_INIT(CDlgRunLink)
	//}}AFX_DATA_INIT
}

CDlgRunLink::~CDlgRunLink()
{
}

void CDlgRunLink::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRunLink)
	DDX_Control(pDX, IDC_LIST, m_ctrlLinks);
	DDX_Control(pDX, IDC_NEW, m_ctrlNew);
	DDX_Control(pDX, IDC_EDIT, m_ctrlEdit);
	DDX_Control(pDX, IDC_DELETE, m_ctrlDelete);
	DDX_Control(pDX, IDC_OPEN, m_ctrlOpen);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgRunLink, CPropertyPage)
	//{{AFX_MSG_MAP(CDlgRunLink)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemchangedList)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgRunLink::UpdateButtons()
{
	BOOL bEnable = FALSE;
	if (0 <= m_ctrlLinks.GetSelection())
		bEnable = TRUE;
	m_ctrlEdit.EnableWindow(bEnable);
	m_ctrlDelete.EnableWindow(bEnable);
	m_ctrlOpen.EnableWindow(bEnable);
}

void CDlgRunLink::ListFiles(char const* pItem)
{
	m_ctrlLinks.DeleteAllItems();
	std::set<std::string> links;
	m_Run->GetLinks(links);
	int i = 0;
	for (std::set<std::string>::iterator iter = links.begin();
		iter != links.end();
		++iter)
	{
		int idx = m_ctrlLinks.InsertItem(i++, (*iter).c_str());
		if (pItem && *iter == pItem)
			m_ctrlLinks.SetSelection(idx);
	}
	m_ctrlLinks.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	UpdateButtons();
}

/////////////////////////////////////////////////////////////////////////////
// CDlgRunLink message handlers

BOOL CDlgRunLink::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_ctrlLinks.SetExtendedStyle(m_ctrlLinks.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_ctrlLinks.InsertColumn(0, "");
	ListFiles(NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRunLink::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	UpdateButtons();
	*pResult = 0;
}

void CDlgRunLink::OnNew() 
{
	CDlgSelectURL dlg("", this);
	if (IDOK == dlg.DoModal())
	{
		std::string newName = dlg.GetName();
		m_Run->AddLink(newName);
		ListFiles(newName.c_str());
	}
}

void CDlgRunLink::OnEdit() 
{
	int nItem = m_ctrlLinks.GetSelection();
	if (0 <= nItem)
	{
		std::string name = (LPCTSTR)m_ctrlLinks.GetItemText(nItem, 0);
		CDlgSelectURL dlg(name.c_str(), this);
		if (IDOK == dlg.DoModal())
		{
			std::string newName = dlg.GetName();
			if (name != newName)
			{
				m_Run->RemoveLink(name);
				m_Run->AddLink(newName);
				ListFiles(newName.c_str());
			}
		}
	}
}

void CDlgRunLink::OnDelete() 
{
	int nItem = m_ctrlLinks.GetSelection();
	if (0 <= nItem)
	{
		std::string name = (LPCTSTR)m_ctrlLinks.GetItemText(nItem, 0);
		m_Run->RemoveLink(name);
		ListFiles(NULL);
	}
}

void CDlgRunLink::OnOpen()
{
	int nItem = m_ctrlLinks.GetSelection();
	if (0 <= nItem)
	{
		CString name = m_ctrlLinks.GetItemText(nItem, 0);
		INT_PTR result = reinterpret_cast<INT_PTR>(ShellExecute(NULL, _T("open"), name, NULL, NULL, SW_SHOW));
		if (result <= HINSTANCE_ERROR)
		{
			CString str;
			switch (result)
			{
			case 0:
				str = "The operating system is out of memory or resources.";
				break;
			case SE_ERR_PNF:
				str = "The specified path was not found.";
				break;
			case SE_ERR_FNF:
				str = "The specified file was not found.";
				break;
			case ERROR_BAD_FORMAT:
				str = "The .EXE file is invalid (non-Win32 .EXE or error in .EXE image).";
				break;
			case SE_ERR_ACCESSDENIED:
				str = "The operating system denied access to the specified file.";
				break;
			case SE_ERR_ASSOCINCOMPLETE:
				str = "The filename association is incomplete or invalid.";
				break;
			case SE_ERR_DDEBUSY:
				str = "The DDE transaction could not be completed because other DDE transactions were being processed.";
				break;
			case SE_ERR_DDEFAIL:
				str = "The DDE transaction failed.";
				break;
			case SE_ERR_DDETIMEOUT:
				str = "The DDE transaction could not be completed because the request timed out.";
				break;
			case SE_ERR_DLLNOTFOUND:
				str = "The specified dynamic-link library was not found.";
				break;
			case SE_ERR_NOASSOC:
				str = "There is no application associated with the given filename extension.";
				break;
			case SE_ERR_OOM:
				str = "There was not enough memory to complete the operation.";
				break;
			case SE_ERR_SHARE:
				str = "A sharing violation occurred. ";
				break;
			default:
				str.Format(_T("Unknown Error (%d) occurred."), result);
				break;
			}
			str = "Unable to open " + str;
			AfxMessageBox(str, MB_ICONEXCLAMATION | MB_OK);
		}
	}
}
