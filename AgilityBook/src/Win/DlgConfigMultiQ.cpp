/*
 * Copyright © 2005-2006 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgConfigMultiQ class
 * @author David Connet
 *
 * Revision History
 * @li 2005-07-30 DRC Created.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgConfigMultiQ.h"

#include "ARBConfigDivision.h"
#include "ARBConfigEvent.h"
#include "ARBConfigLevel.h"
#include "ARBConfigSubLevel.h"
#include "ARBConfigVenue.h"
#include "DlgEventSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigMultiQ dialog

CDlgConfigMultiQ::CDlgConfigMultiQ(
		ARBConfigVenuePtr inVenue,
		ARBConfigMultiQPtr inMultiQ,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgConfigMultiQ::IDD, pParent)
	, m_ctrlItems(false)
	, m_pVenue(inVenue)
	, m_pMultiQ(inMultiQ)
	, m_Name(inMultiQ->GetName().c_str())
	, m_ShortName(inMultiQ->GetShortName().c_str())
	, m_DateFrom(inMultiQ->GetValidFrom())
	, m_bFrom(inMultiQ->GetValidFrom().IsValid() ? TRUE : FALSE)
	, m_DateTo(inMultiQ->GetValidTo())
	, m_bTo(inMultiQ->GetValidTo().IsValid() ? TRUE : FALSE)
{
	//{{AFX_DATA_INIT(CDlgConfigMultiQ)
	//}}AFX_DATA_INIT
}

CDlgConfigMultiQ::~CDlgConfigMultiQ()
{
}

void CDlgConfigMultiQ::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConfigMultiQ)
	DDX_Text(pDX, IDC_CONFIG_MULTIQ_NAME, m_Name);
	DDX_Text(pDX, IDC_CONFIG_MULTIQ_SHORTNAME, m_ShortName);
	DDX_Check(pDX, IDC_CONFIG_MULTIQ_DATE_VALID_FROM, m_bFrom);
	DDX_Control(pDX, IDC_CONFIG_MULTIQ_DATE_START, m_ctrlDateFrom);
	DDX_Check(pDX, IDC_CONFIG_MULTIQ_DATE_VALID_TO, m_bTo);
	DDX_Control(pDX, IDC_CONFIG_MULTIQ_DATE_END, m_ctrlDateTo);
	DDX_Control(pDX, IDC_CONFIG_MULTIQ_ITEMS, m_ctrlItems);
	DDX_Control(pDX, IDC_CONFIG_MULTIQ_EDIT, m_ctrlEdit);
	DDX_Control(pDX, IDC_CONFIG_MULTIQ_DELETE, m_ctrlRemove);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		CTime time;
		if (GDT_VALID == m_ctrlDateFrom.GetTime(time))
			m_DateFrom = ARBDate(time.GetYear(), time.GetMonth(), time.GetDay());
		if (GDT_VALID == m_ctrlDateTo.GetTime(time))
			m_DateTo = ARBDate(time.GetYear(), time.GetMonth(), time.GetDay());
	}
}

BEGIN_MESSAGE_MAP(CDlgConfigMultiQ, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgConfigMultiQ)
	ON_NOTIFY(NM_DBLCLK, IDC_CONFIG_MULTIQ_ITEMS, OnNMDblclk)
	ON_NOTIFY(LVN_KEYDOWN, IDC_CONFIG_MULTIQ_ITEMS, OnKeydown)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CONFIG_MULTIQ_ITEMS, OnItemchanged)
	ON_BN_CLICKED(IDC_CONFIG_MULTIQ_DATE_VALID_FROM, OnClickFrom)
	ON_BN_CLICKED(IDC_CONFIG_MULTIQ_DATE_VALID_TO, OnClickTo)
	ON_BN_CLICKED(IDC_CONFIG_MULTIQ_ADD, OnAdd)
	ON_BN_CLICKED(IDC_CONFIG_MULTIQ_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_CONFIG_MULTIQ_DELETE, OnRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDlgConfigMultiQ::UpdateControls()
{
	if (m_bFrom)
		m_ctrlDateFrom.EnableWindow(TRUE);
	else
		m_ctrlDateFrom.EnableWindow(FALSE);
	if (m_bTo)
		m_ctrlDateTo.EnableWindow(TRUE);
	else
		m_ctrlDateTo.EnableWindow(FALSE);
	int idx = m_ctrlItems.GetSelection();
	BOOL bEnable = FALSE;
	if (0 <= idx)
		bEnable = TRUE;
	m_ctrlEdit.EnableWindow(bEnable);
	m_ctrlRemove.EnableWindow(bEnable);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigMultiQ message handlers

BOOL CDlgConfigMultiQ::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();
	m_ctrlItems.SetExtendedStyle(m_ctrlItems.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	CTime t = CTime::GetCurrentTime();
	if (m_DateFrom.IsValid())
		t = m_DateFrom.GetDate();
	m_ctrlDateFrom.SetTime(&t);
	t = CTime::GetCurrentTime();
	if (m_DateTo.IsValid())
		t = m_DateTo.GetDate();
	m_ctrlDateTo.SetTime(&t);

	m_ctrlItems.InsertColumn(0, _T("Division"));
	m_ctrlItems.InsertColumn(1, _T("Level"));
	m_ctrlItems.InsertColumn(2, _T("Event"));

	size_t n = m_pMultiQ->GetNumItems();
	for (size_t i = 0; i < n; ++i)
	{
		ARBString div, level, evt;
		if (m_pMultiQ->GetItem(i, div, level, evt))
		{
			int idx = m_ctrlItems.InsertItem(static_cast<int>(i), div.c_str());
			m_ctrlItems.SetItemText(idx, 1, level.c_str());
			m_ctrlItems.SetItemText(idx, 2, evt.c_str());
		}
	}
	m_ctrlItems.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_ctrlItems.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
	m_ctrlItems.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);

	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgConfigMultiQ::OnNMDblclk(
		NMHDR* /*pNMHDR*/,
		LRESULT* pResult)
{
	OnEdit();
	*pResult = 0;
}

void CDlgConfigMultiQ::OnKeydown(
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

void CDlgConfigMultiQ::OnItemchanged(
		NMHDR* /*pNMHDR*/,
		LRESULT* pResult)
{
	//LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	UpdateControls();
	*pResult = 0;
}

void CDlgConfigMultiQ::OnClickFrom()
{
	if (!UpdateData(TRUE))
		return;
	UpdateControls();
}

void CDlgConfigMultiQ::OnClickTo()
{
	if (!UpdateData(TRUE))
		return;
	UpdateControls();
}

void CDlgConfigMultiQ::OnAdd()
{
	if (!UpdateData(TRUE))
		return;
	ARBDate date = ARBDate::Today();
	if (m_bFrom)
		date = m_DateFrom;
	else if (m_bTo)
		date = m_DateTo;
	CDlgEventSelect dlg(m_pVenue, date, NULL, NULL, NULL, this);
	if (IDOK == dlg.DoModal())
	{
		if (m_pMultiQ->AddItem(dlg.GetDivision(), dlg.GetLevel(), dlg.GetEvent()))
		{
			int idx = m_ctrlItems.InsertItem(m_ctrlItems.GetItemCount(), dlg.GetDivision());
			m_ctrlItems.SetItemText(idx, 1, dlg.GetLevel());
			m_ctrlItems.SetItemText(idx, 2, dlg.GetEvent());
			m_ctrlItems.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
			m_ctrlItems.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
			m_ctrlItems.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);
		}
		else
			MessageBeep(MB_ICONEXCLAMATION);
	}
}

void CDlgConfigMultiQ::OnEdit()
{
	if (!UpdateData(TRUE))
		return;
	int idx = m_ctrlItems.GetSelection();
	if (0 <= idx)
	{
		CString div = m_ctrlItems.GetItemText(idx, 0);
		CString level = m_ctrlItems.GetItemText(idx, 1);
		CString evt = m_ctrlItems.GetItemText(idx, 2);
		ARBDate date = ARBDate::Today();
		if (m_bFrom)
			date = m_DateFrom;
		else if (m_bTo)
			date = m_DateTo;
		CDlgEventSelect dlg(m_pVenue, date, (LPCTSTR)div, (LPCTSTR)level, (LPCTSTR)evt, this);
		if (IDOK == dlg.DoModal())
		{
			if (m_pMultiQ->RemoveItem((LPCTSTR)div, (LPCTSTR)level, (LPCTSTR)evt))
				m_ctrlItems.DeleteItem(idx);
			if (m_pMultiQ->AddItem(dlg.GetDivision(), dlg.GetLevel(), dlg.GetEvent()))
			{
				idx = m_ctrlItems.InsertItem(idx,dlg.GetDivision());
				m_ctrlItems.SetItemText(idx, 1, dlg.GetLevel());
				m_ctrlItems.SetItemText(idx, 2, dlg.GetEvent());
				m_ctrlItems.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
				m_ctrlItems.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
				m_ctrlItems.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);
			}
			else
				MessageBeep(MB_ICONEXCLAMATION);
		}
	}
}

void CDlgConfigMultiQ::OnRemove()
{
	int idx = m_ctrlItems.GetSelection();
	if (0 <= idx)
		m_ctrlItems.DeleteItem(idx);
}

void CDlgConfigMultiQ::OnOK()
{
	if (!UpdateData(TRUE))
		return;
	m_Name.TrimRight();
	m_Name.TrimLeft();
	if (m_Name.IsEmpty())
	{
		AfxMessageBox(_T("Please enter a name."), MB_ICONINFORMATION);
		GotoDlgCtrl(GetDlgItem(IDC_CONFIG_MULTIQ_NAME));
		return;
	}
	m_ShortName.TrimRight();
	m_ShortName.TrimLeft();
	if (m_ShortName.IsEmpty())
	{
		AfxMessageBox(_T("Please enter a short name."), MB_ICONINFORMATION);
		GotoDlgCtrl(GetDlgItem(IDC_CONFIG_MULTIQ_SHORTNAME));
		return;
	}
	if (m_pMultiQ->GetName() != (LPCTSTR)m_Name)
	{
		if (m_pVenue->GetMultiQs().FindMultiQ((LPCTSTR)m_Name))
		{
			AfxMessageBox(IDS_NAME_IN_USE);
			GotoDlgCtrl(GetDlgItem(IDC_CONFIG_MULTIQ_NAME));
			return;
		}
	}

	m_pMultiQ->SetName((LPCTSTR)m_Name);
	m_pMultiQ->SetShortName((LPCTSTR)m_ShortName);
	if (!m_bFrom)
		m_DateFrom.clear();
	m_pMultiQ->SetValidFrom(m_DateFrom);
	if (!m_bTo)
		m_DateTo.clear();
	m_pMultiQ->SetValidTo(m_DateTo);
	m_pMultiQ->RemoveAllItems();
	for (int idx = m_ctrlItems.GetItemCount() - 1; idx >= 0; --idx)
	{
		CString div = m_ctrlItems.GetItemText(idx, 0);
		CString level = m_ctrlItems.GetItemText(idx, 1);
		CString evt = m_ctrlItems.GetItemText(idx, 2);
		m_pMultiQ->AddItem((LPCTSTR)div, (LPCTSTR)level, (LPCTSTR)evt);
	}
	CDlgBaseDialog::OnOK();
}
