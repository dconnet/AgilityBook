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
 * @brief implementation of the CDlgSelectEvent class
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgSelectEvent.h"

#include "ARBConfigDivision.h"
#include "ARBConfigEvent.h"
#include "ARBConfigLevel.h"
#include "ARBConfigSubLevel.h"
#include "ARBConfigVenue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

class CDlgSelectEventDataLevel
{
public:
	CDlgSelectEventDataLevel(ARBConfigLevel const* pLevel)
		: m_pLevel(pLevel)
		, m_pSubLevel(NULL)
	{
	}
	CDlgSelectEventDataLevel(
			ARBConfigLevel const* pLevel,
			ARBConfigSubLevel const* pSubLevel)
		: m_pLevel(pLevel)
		, m_pSubLevel(pSubLevel)
	{
	}
	ARBConfigLevel const* m_pLevel;
	ARBConfigSubLevel const* m_pSubLevel;
};

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectEvent dialog

CDlgSelectEvent::CDlgSelectEvent(
		ARBConfigVenue const* pVenue,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgSelectEvent::IDD, pParent)
	, m_pVenue(pVenue)
{
	//{{AFX_DATA_INIT(CDlgSelectEvent)
	//}}AFX_DATA_INIT
}

void CDlgSelectEvent::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelectEvent)
	DDX_Control(pDX, IDC_DIVISION, m_ctrlDivision);
	DDX_Control(pDX, IDC_LEVEL, m_ctrlLevel);
	DDX_Control(pDX, IDC_EVENT, m_ctrlEvent);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgSelectEvent, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgSelectEvent)
	ON_CBN_SELCHANGE(IDC_DIVISION, OnSelchangeDivision)
	ON_CBN_SELCHANGE(IDC_LEVEL, OnSelchangeLevel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgSelectEvent::ClearLevels()
{
	for (int index = m_ctrlLevel.GetCount() - 1; 0 <= index; --index)
	{
		CDlgSelectEventDataLevel* pData = reinterpret_cast<CDlgSelectEventDataLevel*>(m_ctrlLevel.GetItemDataPtr(index));
		delete pData;
		m_ctrlLevel.SetItemDataPtr(index, NULL);
	}
	m_ctrlLevel.ResetContent();
}

void CDlgSelectEvent::FillLevels()
{
	ClearLevels();
	int index = m_ctrlDivision.GetCurSel();
	if (CB_ERR != index)
	{
		ARBConfigDivision const* pDiv = reinterpret_cast<ARBConfigDivision const*>(m_ctrlDivision.GetItemDataPtr(index));
		for (ARBConfigLevelList::const_iterator iter = pDiv->GetLevels().begin();
			iter != pDiv->GetLevels().end();
			++iter)
		{
			ARBConfigLevel const* pLevel = (*iter);
			if (0 < pLevel->GetSubLevels().size())
			{
				for (ARBConfigSubLevelList::const_iterator iterSub = pLevel->GetSubLevels().begin();
					iterSub != pLevel->GetSubLevels().end();
					++iterSub)
				{
					ARBConfigSubLevel const* pSubLevel = (*iterSub);
					CDlgSelectEventDataLevel* pData = new CDlgSelectEventDataLevel(pLevel, pSubLevel);
					int idx = m_ctrlLevel.AddString(pSubLevel->GetName().c_str());
					m_ctrlLevel.SetItemDataPtr(idx, pData);
				}
			}
			else
			{
				CDlgSelectEventDataLevel* pData = new CDlgSelectEventDataLevel(pLevel);
				int idx = m_ctrlLevel.AddString(pLevel->GetName().c_str());
				m_ctrlLevel.SetItemDataPtr(idx, pData);
			}
		}
	}
	FillEvents();
}

void CDlgSelectEvent::FillEvents()
{
	m_ctrlEvent.ResetContent();
	int idxDiv = m_ctrlDivision.GetCurSel();
	if (CB_ERR == idxDiv)
		return;
	ARBConfigDivision const* pDiv = reinterpret_cast<ARBConfigDivision const*>(m_ctrlDivision.GetItemDataPtr(idxDiv));
	int idxLevel = m_ctrlLevel.GetCurSel();
	if (CB_ERR == idxLevel)
		return;
	CDlgSelectEventDataLevel* pData = reinterpret_cast<CDlgSelectEventDataLevel*>(m_ctrlLevel.GetItemDataPtr(idxLevel));
	for (ARBConfigEventList::const_iterator iter = m_pVenue->GetEvents().begin();
		iter != m_pVenue->GetEvents().end();
		++iter)
	{
		ARBConfigEvent const* pEvent = (*iter);
		if (pEvent->FindEvent(pDiv->GetName(), pData->m_pLevel->GetName()))
		{
			m_ctrlEvent.AddString(pEvent->GetName().c_str());
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectEvent message handlers

BOOL CDlgSelectEvent::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();

	for (ARBConfigDivisionList::const_iterator iter = m_pVenue->GetDivisions().begin();
		iter != m_pVenue->GetDivisions().end();
		++iter)
	{
		ARBConfigDivision* pDiv = (*iter);
		int index = m_ctrlDivision.AddString(pDiv->GetName().c_str());
		if (CB_ERR != index)
			m_ctrlDivision.SetItemDataPtr(index, pDiv);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectEvent::OnSelchangeDivision()
{
	FillLevels();
}

void CDlgSelectEvent::OnSelchangeLevel()
{
	FillEvents();
}

void CDlgSelectEvent::OnOK()
{
	if (!UpdateData(TRUE))
		return;
	int index = m_ctrlDivision.GetCurSel();
	if (CB_ERR == index)
	{
		GotoDlgCtrl(&m_ctrlDivision);
		return;
	}
	m_ctrlDivision.GetLBText(index, m_Division);
	index = m_ctrlLevel.GetCurSel();
	if (CB_ERR == index)
	{
		GotoDlgCtrl(&m_ctrlLevel);
		return;
	}
	m_ctrlLevel.GetLBText(index, m_Level);
	index = m_ctrlEvent.GetCurSel();
	if (CB_ERR == index)
	{
		GotoDlgCtrl(&m_ctrlEvent);
		return;
	}
	m_ctrlEvent.GetLBText(index, m_Event);
	ClearLevels();
	CDlgBaseDialog::OnOK();
}

void CDlgSelectEvent::OnCancel()
{
	ClearLevels();
	CDlgBaseDialog::OnCancel();
}
