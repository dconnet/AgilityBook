/*
 * Copyright © 2004-2007 David Connet. All Rights Reserved.
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
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-06-02 DRC Moved ShellExecute code to AgilityBook.cpp, added icons.
 * @li 2004-03-30 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgRunLink.h"

#include "ARBDogRun.h"
#include "CheckLink.h"
#include "DlgSelectURL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRunLink dialog

CDlgRunLink::CDlgRunLink(
		CAgilityBookDoc* pDoc,
		ARBDogRunPtr pRun)
	: CDlgBasePropertyPage(CDlgRunLink::IDD)
	, m_ctrlLinks(false)
	, m_pDoc(pDoc)
	, m_Run(pRun)
	, m_Session(_T("runLink"))
{
	m_ImageList.Create(16, 16, ILC_MASK | ILC_COLOR32, 2, 0);
	CWinApp* app = AfxGetApp();
	m_imgEmpty = m_ImageList.Add(app->LoadIcon(IDI_EMPTY));
	m_imgOk = m_ImageList.Add(app->LoadIcon(IDI_CHECKMARK));
	m_imgMissing = m_ImageList.Add(app->LoadIcon(IDI_QUESTIONMARK));
	//{{AFX_DATA_INIT(CDlgRunLink)
	//}}AFX_DATA_INIT
}

CDlgRunLink::~CDlgRunLink()
{
}

void CDlgRunLink::DoDataExchange(CDataExchange* pDX)
{
	CDlgBasePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRunLink)
	DDX_Control(pDX, IDC_RUNLINK_LIST, m_ctrlLinks);
	DDX_Control(pDX, IDC_RUNLINK_NEW, m_ctrlNew);
	DDX_Control(pDX, IDC_RUNLINK_EDIT, m_ctrlEdit);
	DDX_Control(pDX, IDC_RUNLINK_DELETE, m_ctrlDelete);
	DDX_Control(pDX, IDC_RUNLINK_OPEN, m_ctrlOpen);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgRunLink, CDlgBasePropertyPage)
	//{{AFX_MSG_MAP(CDlgRunLink)
	ON_NOTIFY(NM_DBLCLK, IDC_RUNLINK_LIST, OnDblclkList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_RUNLINK_LIST, OnKeydownList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_RUNLINK_LIST, OnItemchangedList)
	ON_BN_CLICKED(IDC_RUNLINK_NEW, OnNew)
	ON_BN_CLICKED(IDC_RUNLINK_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_RUNLINK_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_RUNLINK_OPEN, OnOpen)
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

void CDlgRunLink::ListFiles(TCHAR const* pItem)
{
	CWaitCursor wait;
	m_ctrlLinks.DeleteAllItems();
	std::set<ARBString> links;
	m_Run->GetLinks(links);
	int i = 0;
	for (std::set<ARBString>::iterator iter = links.begin();
		iter != links.end();
		++iter)
	{
		int idx = m_ctrlLinks.InsertItem(i++, (*iter).c_str(), GetImageIndex((*iter).c_str()));
		if (pItem && *iter == pItem)
			m_ctrlLinks.SetSelection(idx);
	}
	m_ctrlLinks.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	UpdateButtons();
}

int CDlgRunLink::GetImageIndex(ARBString const& inLink)
{
	CWaitCursor wait;
	int img = m_imgEmpty;
	if (0 < inLink.length())
	{
		if (CheckLink(m_Session, inLink))
			img = m_imgOk;
		else
			img = m_imgMissing;
	}
	return img;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgRunLink message handlers

BOOL CDlgRunLink::OnInitDialog() 
{
	CDlgBasePropertyPage::OnInitDialog();
	m_ctrlLinks.SetExtendedStyle(m_ctrlLinks.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	m_ctrlLinks.SetImageList(&m_ImageList, LVSIL_SMALL);
	m_ctrlLinks.InsertColumn(0, _T(""));
	ListFiles(NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRunLink::OnDblclkList(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	OnEdit();
	*pResult = 0;
}

void CDlgRunLink::OnKeydownList(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDown = reinterpret_cast<LV_KEYDOWN*>(pNMHDR);
	switch (pLVKeyDown->wVKey)
	{
	default:
		break;
	case VK_SPACE:
		OnEdit();
		break;
	}
	*pResult = 0;
}

void CDlgRunLink::OnItemchangedList(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
//	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	UpdateButtons();
	*pResult = 0;
}

void CDlgRunLink::OnNew() 
{
	CDlgSelectURL dlg(_T(""), this);
	if (IDOK == dlg.DoModal())
	{
		ARBString newName = dlg.GetName();
		if (0 < newName.length())
		{
			m_Run->AddLink(newName);
			ListFiles(newName.c_str());
		}
	}
}

void CDlgRunLink::OnEdit() 
{
	int nItem = m_ctrlLinks.GetSelection();
	if (0 <= nItem)
	{
		ARBString name = (LPCTSTR)m_ctrlLinks.GetItemText(nItem, 0);
		CDlgSelectURL dlg(name.c_str(), this);
		if (IDOK == dlg.DoModal())
		{
			ARBString newName = dlg.GetName();
			if (name != newName)
			{
				m_Run->RemoveLink(name);
				if (0 < newName.length())
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
		ARBString name = (LPCTSTR)m_ctrlLinks.GetItemText(nItem, 0);
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
		RunCommand(name);
	}
}
