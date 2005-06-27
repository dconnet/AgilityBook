/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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
 * Note, this class also deals with adding notes on Clubs and Locations,
 * in addition to judges. It probably should be renamed...
 *
 * Remember, when adding an entry, it is only saved if there is a comment.
 *
 * Revision History
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-24-05 DRC If there were 0 items in the list, it crashed.
 *                    Allow saving an entry with no comment.
 * @li 2004-12-11 DRC Added indicators if item is added and/or has comments.
 *                    Merged in club/location support (was in separate files
 *                    that were added 11/18/04.
 * @li 2003-12-07 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgInfoJudge.h"

#include <algorithm>
#include "AgilityBookDoc.h"
#include "DlgName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CDlgInfoJudge::NameInfo::NameInfo()
	: m_Name()
	, m_eInUse(eNotInUse)
	, m_bHasData(false)
{
}

CDlgInfoJudge::NameInfo::NameInfo(std::string const& inName)
	: m_Name(inName)
	, m_eInUse(eNotInUse)
	, m_bHasData(false)
{
}

CDlgInfoJudge::NameInfo::NameInfo(NameInfo const& rhs)
	: m_Name(rhs.m_Name)
	, m_eInUse(rhs.m_eInUse)
	, m_bHasData(rhs.m_bHasData)
{
}

bool CDlgInfoJudge::NameInfo::operator==(NameInfo const& rhs)
{
	return m_Name == rhs.m_Name;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgInfoJudge dialog

CDlgInfoJudge::CDlgInfoJudge(CAgilityBookDoc* pDoc, ARBInfo::eInfoType inType, CWnd* pParent /*=NULL*/)
	: CDlgBaseDialog(CDlgInfoJudge::IDD, pParent)
	, m_pDoc(pDoc)
	, m_Type(inType)
	, m_InfoOrig("") // We don't care about setting the infoname here
	, m_Info("") // We don't care about setting the infoname here
	, m_nAdded(0)
{
	switch (m_Type)
	{
	case ARBInfo::eClubInfo:
		m_pDoc->GetAllClubNames(m_NamesInUse, false);
		break;
	case ARBInfo::eJudgeInfo:
		m_pDoc->GetAllJudges(m_NamesInUse, false);
		break;
	case ARBInfo::eLocationInfo:
		m_pDoc->GetAllTrialLocations(m_NamesInUse, false);
		break;
	}
	m_InfoOrig = m_pDoc->GetInfo().GetInfo(m_Type);
	m_Info = m_InfoOrig;
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
	ON_WM_COMPAREITEM()
	ON_WM_DRAWITEM()
	ON_CBN_SELCHANGE(IDC_JUDGE, OnSelchangeName)
	ON_EN_KILLFOCUS(IDC_JUDGE_COMMENTS, OnKillfocusComments)
	ON_BN_CLICKED(IDC_JUDGE_NEW, OnNew)
	ON_BN_CLICKED(IDC_JUDGE_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgInfoJudge message handlers

BOOL CDlgInfoJudge::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();
	ASSERT(m_ctrlNames.GetStyle() & CBS_OWNERDRAWFIXED);

	CString caption;
	std::set<std::string> names;
	std::string select;
	switch (m_Type)
	{
	case ARBInfo::eClubInfo:
		{
			caption.LoadString(IDS_INFO_CLUB);
			m_pDoc->GetAllClubNames(names);
			ARBDogTrial const* pTrial = m_pDoc->GetCurrentTrial();
			if (pTrial)
				select = pTrial->GetClubs().GetPrimaryClubName();
		}
		break;
	case ARBInfo::eJudgeInfo:
		{
			caption.LoadString(IDS_INFO_JUDGE);
			m_pDoc->GetAllJudges(names);
			ARBDogRun const* pRun = m_pDoc->GetCurrentRun();
			if (pRun)
				select = pRun->GetJudge();
		}
		break;
	case ARBInfo::eLocationInfo:
		{
			caption.LoadString(IDS_INFO_LOCATION);
			m_pDoc->GetAllTrialLocations(names);
			ARBDogTrial const* pTrial = m_pDoc->GetCurrentTrial();
			if (pTrial)
				select = pTrial->GetLocation();
		}
		break;
	}
	SetWindowText(caption);
	m_Names.clear();
	m_Names.reserve(names.size());
	for (std::set<std::string>::iterator iter = names.begin(); iter != names.end(); ++iter)
	{
		NameInfo data(*iter);
		ARBInfoItem* item;
		if (m_Info.FindItem(data.m_Name, &item))
		{
			if (0 < item->GetComment().length())
				data.m_bHasData = true;
			item->Release();
			item = NULL;
		}
		if (m_NamesInUse.end() != std::find(m_NamesInUse.begin(), m_NamesInUse.end(), data.m_Name))
			data.m_eInUse = NameInfo::eInUse;
		else
			++m_nAdded;
		m_Names.push_back(data);
	}
	for (size_t idx = 0; idx < m_Names.size(); ++idx)
	{
		// Combo box is ownerdraw.
		m_ctrlNames.AddString((LPCTSTR)idx);
	}
	if (0 == select.length() || 0 > m_ctrlNames.SelectString(-1, select.c_str()))
		m_ctrlNames.SetCurSel(0);
	OnSelchangeName();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CDlgInfoJudge::OnCompareItem(int nIDCtl, LPCOMPAREITEMSTRUCT lpCompareItemStruct)
{
	CString str1;
	if (-1 == lpCompareItemStruct->itemID1)
		str1 = (LPCTSTR)(lpCompareItemStruct->itemData1);
	else
		str1 = m_Names[lpCompareItemStruct->itemData1].m_Name.c_str();
	CString str2 = m_Names[lpCompareItemStruct->itemData2].m_Name.c_str();
	return str1.CollateNoCase(str2);
}

void CDlgInfoJudge::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (-1 == lpDrawItemStruct->itemID)
		return;

	ASSERT(lpDrawItemStruct->CtlType == ODT_COMBOBOX);
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	// Save these value to restore them when done drawing.
	COLORREF crOldTextColor = dc.GetTextColor();
	COLORREF crOldBkColor = dc.GetBkColor();

	// If this item is selected, set the background color 
	// and the text color to appropriate values. Erase
	// the rect by filling it with the background color.
	if ((lpDrawItemStruct->itemAction | ODA_SELECT)
	&& (lpDrawItemStruct->itemState & ODS_SELECTED))
	{
		dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, ::GetSysColor(COLOR_HIGHLIGHT));
	}
	else
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, crOldBkColor);

	// Draw the text.
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	CRect rect(lpDrawItemStruct->rcItem);
	if (m_Names[lpDrawItemStruct->itemData].m_bHasData)
		dc.DrawText("*", 1, rect, DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	rect.left += 3 * tm.tmAveCharWidth / 2;
	if (0 < m_nAdded)
	{
		if (NameInfo::eNotInUse == m_Names[lpDrawItemStruct->itemData].m_eInUse)
			dc.DrawText("+", 1, rect, DT_LEFT|DT_SINGLELINE|DT_VCENTER);
		rect.left += 3 * tm.tmAveCharWidth / 2;
	}
	rect.left += tm.tmAveCharWidth / 2;
	dc.DrawText(m_Names[lpDrawItemStruct->itemData].m_Name.c_str(),
		static_cast<int>(m_Names[lpDrawItemStruct->itemData].m_Name.length()),
		rect, DT_LEFT|DT_SINGLELINE|DT_VCENTER);

	// Reset the background color and the text color back to their
	// original values.
	dc.SetTextColor(crOldTextColor);
	dc.SetBkColor(crOldBkColor);

	dc.Detach();
}

void CDlgInfoJudge::OnSelchangeName()
{
	BOOL bEnable = FALSE;
	CString data;
	int index = m_ctrlNames.GetCurSel();
	if (CB_ERR != index)
	{
		size_t idx = static_cast<size_t>(m_ctrlNames.GetItemData(index));
		ARBInfoItem* item;
		if (m_Info.FindItem(m_Names[idx].m_Name, &item))
		{
			data = item->GetComment().c_str();
			item->Release();
			item = NULL;
		}
		if (m_NamesInUse.end() == m_NamesInUse.find(m_Names[idx].m_Name))
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
		size_t idx = static_cast<size_t>(m_ctrlNames.GetItemData(index));
		CString data;
		m_ctrlComment.GetWindowText(data);
		data.TrimRight();
		data.Replace("\r\n", "\n");
		ARBInfoItem* item;
		if (!m_Info.FindItem(m_Names[idx].m_Name, &item))
			m_Info.AddItem(m_Names[idx].m_Name, &item);
		if (!item)
			return;
		item->SetComment((LPCTSTR)data);
		item->Release();
		item = NULL;
		m_Names[idx].m_bHasData = (0 < data.GetLength());
		m_ctrlNames.Invalidate();
	}
}

void CDlgInfoJudge::OnNew()
{
	CDlgName dlg("", "", this);
	if (IDOK == dlg.DoModal())
	{
		std::string name = (LPCTSTR)dlg.GetName();
		// First, check if the item exists.
		std::vector<NameInfo>::iterator iter = std::find(m_Names.begin(), m_Names.end(), name);
		if (iter != m_Names.end())
		{
			// Ok, it exists, but it may have been deleted.
			size_t idx = iter - m_Names.begin();
			// Added items cannot be in-use. So we're re-adding.
			if (NameInfo::eDeleted == m_Names[idx].m_eInUse)
			{
				m_Names[idx].m_eInUse = NameInfo::eNotInUse;
				++m_nAdded;
				m_ctrlNames.AddString((LPCTSTR)idx);
				m_ctrlComment.SetWindowText("");
				m_Info.AddItem(m_Names[idx].m_Name);
			}
		}
		else
		{
			// Brand new name!
			NameInfo data(name);
			m_Names.push_back(data);
			size_t idx = m_Names.size() - 1;
			++m_nAdded;
			m_ctrlNames.AddString((LPCTSTR)idx);
			m_ctrlComment.SetWindowText("");
			m_Info.AddItem(m_Names[idx].m_Name);
		}
		m_ctrlNames.Invalidate();
		m_ctrlNames.SelectString(-1, name.c_str());
		OnSelchangeName();
	}
}

void CDlgInfoJudge::OnDelete()
{
	int index = m_ctrlNames.GetCurSel();
	if (CB_ERR != index)
	{
		size_t idx = static_cast<size_t>(m_ctrlNames.GetItemData(index));
		if (m_NamesInUse.end() == m_NamesInUse.find(m_Names[idx].m_Name))
		{
			m_ctrlNames.DeleteString(index);
			ARBInfoItem* item;
			if (m_Info.FindItem(m_Names[idx].m_Name, &item))
			{
				m_Info.DeleteItem(item);
				item->Release();
				item = NULL;
			}
			if (index == m_ctrlNames.GetCount())
				--index;
			if (0 <= index)
				m_ctrlNames.SetCurSel(index);
			--m_nAdded;
			// Do NOT remove the name from m_Names. It will mess up
			// the indices of all the other items. But mark it
			// so we know if it's being re-added later.
			m_Names[idx].m_eInUse = NameInfo::eDeleted;
			m_ctrlNames.Invalidate();
			OnSelchangeName();
		}
		else
			MessageBeep(0);
	}
}

void CDlgInfoJudge::OnOK()
{
	// Not bothering to call UpdateData because we aren't exchanging any
	// data. We're using the controls directly.
	m_Info.CondenseContent(m_NamesInUse);
	if (m_Info != m_InfoOrig)
	{
		m_pDoc->GetInfo().GetInfo(m_Type) = m_Info;
		m_pDoc->SetModifiedFlag();
	}
	CDlgBaseDialog::OnOK();
}
