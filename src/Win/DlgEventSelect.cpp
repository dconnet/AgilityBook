/*
 * Copyright © 2005-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgEventSelect class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-07-30 DRC Created.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgEventSelect.h"

#include "ARBConfigDivision.h"
#include "ARBConfigEvent.h"
#include "ARBConfigLevel.h"
#include "ARBConfigSubLevel.h"
#include "ARBConfigVenue.h"
#include "ListData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

class CDlgEventSelectData : public CListData
{
public:
	CDlgEventSelectData(ARBConfigLevelPtr pLevel)
		: m_pLevel(pLevel)
		, m_pSubLevel()
	{
	}
	CDlgEventSelectData(
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
// CDlgEventSelect dialog

CDlgEventSelect::CDlgEventSelect(
		ARBConfigVenuePtr inVenue,
		ARBDate const& inDate,
		TCHAR const* const inDivision,
		TCHAR const* const inLevel,
		TCHAR const* const inEvent,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgEventSelect::IDD, pParent)
	, m_ctrlDivisions(true)
	, m_ctrlLevels(true)
	, m_ctrlEvents(false)
	, m_pVenue(inVenue)
	, m_Date(inDate)
	, m_inDivision(inDivision)
	, m_inLevel(inLevel)
	, m_inEvent(inEvent)
	, m_Division(_T(""))
	, m_Level(_T(""))
	, m_Event(_T(""))
{
	//{{AFX_DATA_INIT(CDlgEventSelect)
	//}}AFX_DATA_INIT
}


CDlgEventSelect::~CDlgEventSelect()
{
}


void CDlgEventSelect::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEventSelect)
	DDX_CBString(pDX, IDC_EVENT_SELECT_DIVISION, m_Division);
	DDX_Control(pDX, IDC_EVENT_SELECT_DIVISION, m_ctrlDivisions);
	DDX_CBString(pDX, IDC_EVENT_SELECT_LEVEL, m_Level);
	DDX_Control(pDX, IDC_EVENT_SELECT_LEVEL, m_ctrlLevels);
	DDX_CBString(pDX, IDC_EVENT_SELECT_EVENT, m_Event);
	DDX_Control(pDX, IDC_EVENT_SELECT_EVENT, m_ctrlEvents);
	DDX_Control(pDX, IDOK, m_ctrlOk);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgEventSelect, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgEventSelect)
	ON_CBN_SELCHANGE(IDC_EVENT_SELECT_DIVISION, OnSelchangeDivision)
	ON_CBN_SELCHANGE(IDC_EVENT_SELECT_LEVEL, OnSelchangeLevel)
	ON_CBN_SELCHANGE(IDC_EVENT_SELECT_EVENT, OnSelchangeEvent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDlgEventSelect::UpdateControls()
{
	BOOL bEnable = FALSE;
	if (0 <= m_ctrlDivisions.GetCurSel()
	&& 0 <= m_ctrlLevels.GetCurSel()
	&& 0 <= m_ctrlEvents.GetCurSel())
	{
		bEnable = TRUE;
	}
	m_ctrlOk.EnableWindow(bEnable);
}


void CDlgEventSelect::FillLevels()
{
	CString str;
	tstring level;
	int index = m_ctrlLevels.GetCurSel();
	if (CB_ERR != index)
	{
		m_ctrlLevels.GetLBText(index, str);
		level = (LPCTSTR)str;
	}
	if (m_inLevel)
	{
		level = m_inLevel;
		m_inLevel = NULL;
	}
	m_ctrlLevels.ResetContent();
	index = m_ctrlDivisions.GetCurSel();
	if (CB_ERR != index)
	{
		CListPtrData<ARBConfigDivisionPtr>* pData = reinterpret_cast<CListPtrData<ARBConfigDivisionPtr>*>(m_ctrlDivisions.GetItemDataPtr(index));
		ARBConfigDivisionPtr pDiv = pData->GetData();
		for (ARBConfigLevelList::const_iterator iter = pDiv->GetLevels().begin();
			iter != pDiv->GetLevels().end();
			++iter)
		{
			ARBConfigLevelPtr pLevel = (*iter);
			if (0 < pLevel->GetSubLevels().size())
			{
				for (ARBConfigSubLevelList::const_iterator iterSub = pLevel->GetSubLevels().begin();
					iterSub != pLevel->GetSubLevels().end();
					++iterSub)
				{
					ARBConfigSubLevelPtr pSubLevel = (*iterSub);
					int idx = m_ctrlLevels.AddString(pSubLevel->GetName().c_str());
					m_ctrlLevels.SetItemDataPtr(idx,
						new CDlgEventSelectData(pLevel, pSubLevel));
					if (level == pSubLevel->GetName())
						m_ctrlLevels.SetCurSel(idx);
				}
			}
			else
			{
				int idx = m_ctrlLevels.AddString(pLevel->GetName().c_str());
				m_ctrlLevels.SetItemDataPtr(idx,
					new CDlgEventSelectData(pLevel));
				if (level == pLevel->GetName())
					m_ctrlLevels.SetCurSel(idx);
			}
		}
	}
	FillEvents();
}


void CDlgEventSelect::FillEvents()
{
	CString str;
	tstring evt;
	int index = m_ctrlEvents.GetCurSel();
	if (CB_ERR != index)
	{
		m_ctrlEvents.GetLBText(index, str);
		evt = (LPCTSTR)str;
	}
	if (m_inEvent)
	{
		evt = m_inEvent;
		m_inEvent = NULL;
	}
	m_ctrlEvents.ResetContent();
	int idxDiv = m_ctrlDivisions.GetCurSel();
	if (CB_ERR != idxDiv)
	{
		CListPtrData<ARBConfigDivisionPtr>* pData = reinterpret_cast<CListPtrData<ARBConfigDivisionPtr>*>(m_ctrlDivisions.GetItemDataPtr(idxDiv));
		ARBConfigDivisionPtr pDiv = pData->GetData();
		int idxLevel = m_ctrlLevels.GetCurSel();
		if (CB_ERR != idxLevel)
		{
			CDlgEventSelectData* pEvtData = reinterpret_cast<CDlgEventSelectData*>(m_ctrlLevels.GetItemDataPtr(idxLevel));
			for (ARBConfigEventList::const_iterator iter = m_pVenue->GetEvents().begin();
				iter != m_pVenue->GetEvents().end();
				++iter)
			{
				ARBConfigEventPtr pEvent = (*iter);
				if (pEvent->FindEvent(pDiv->GetName(), pEvtData->m_pLevel->GetName(), m_Date))
				{
					int idx = m_ctrlEvents.AddString(pEvent->GetName().c_str());
					if (evt == pEvent->GetName())
					{
						m_ctrlEvents.SetCurSel(idx);
					}
				}
			}
		}
	}
	UpdateControls();
}

/////////////////////////////////////////////////////////////////////////////
// CDlgEventSelect message handlers

BOOL CDlgEventSelect::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();

	int index;
	for (ARBConfigDivisionList::const_iterator iterDiv = m_pVenue->GetDivisions().begin();
		iterDiv != m_pVenue->GetDivisions().end();
		++iterDiv)
	{
		ARBConfigDivisionPtr pDiv = (*iterDiv);
		index = m_ctrlDivisions.AddString(pDiv->GetName().c_str());
		m_ctrlDivisions.SetItemDataPtr(index, 
			new CListPtrData<ARBConfigDivisionPtr>(pDiv));
		if (m_inDivision && pDiv->GetName() == m_inDivision)
			m_ctrlDivisions.SetCurSel(index);
	}
	m_inDivision = NULL;

	FillLevels(); // This will call	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgEventSelect::OnSelchangeDivision()
{
	FillLevels();
}


void CDlgEventSelect::OnSelchangeLevel()
{
	FillEvents();
}


void CDlgEventSelect::OnSelchangeEvent()
{
	UpdateControls();
}


void CDlgEventSelect::OnOK()
{
	if (!UpdateData(TRUE))
		return;
	if (m_Division.IsEmpty()
	|| m_Level.IsEmpty()
	|| m_Event.IsEmpty())
		return;
	// Copy them because ClearLevels will affect things.
	m_Div = m_Division;
	m_Lev = m_Level;
	m_Evt = m_Event;
	m_ctrlDivisions.ResetContent();
	m_ctrlLevels.ResetContent();
	CDlgBaseDialog::OnOK();
}


void CDlgEventSelect::OnCancel()
{
	m_ctrlDivisions.ResetContent();
	m_ctrlLevels.ResetContent();
	CDlgBaseDialog::OnCancel();
}
