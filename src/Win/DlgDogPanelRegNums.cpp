/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgDog class
 * @author David Connet
 *
 * Revision History
 * 2021-01-07 Separated from DlgDog.cpp
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-02-03 Broke dialog up into pages.
 * 2004-01-04 Changed ARBDate::GetString to take a format code.
 * 2003-08-18 Added a deceased date for a dog.
 */

#include "stdafx.h"
#include "DlgDogPanelRegNums.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "DlgRegNum.h"

#include "ARB/ARBDog.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ListData.h"
#include "LibARBWin/ReportListCtrl.h"
#include "LibARBWin/ReportListHeader.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

namespace
{
constexpr int k_colVenue = 0;
constexpr int k_colNumber = 1;
constexpr int k_colHeight = 2;
constexpr int k_colReceived = 3;
constexpr int k_colNote = 4;
static const std::vector<CReportListHeader::ColumnInfo> k_columnsRegNumInfo{
	{k_colVenue, wxLIST_FORMAT_LEFT, arbT("IDS_COL_VENUE")},
	{k_colNumber, wxLIST_FORMAT_LEFT, arbT("IDS_COL_NUMBER")},
	{k_colHeight, wxLIST_FORMAT_LEFT, arbT("IDS_COL_HEIGHT")},
	{k_colReceived, wxLIST_FORMAT_LEFT, arbT("IDS_COL_RECEIVED")},
	{k_colNote, wxLIST_FORMAT_LEFT, arbT("IDS_COL_NOTE")},
};
} // namespace

/////////////////////////////////////////////////////////////////////////////

class CDlgDogDataRegNum : public CListData
{
public:
	CDlgDogDataRegNum(CDlgDogPanelRegNums* pDlg, ARBDogRegNumPtr const& inRegnum)
		: m_pDlg(pDlg)
		, m_RegNum(inRegnum)
	{
	}
	int OnCompare(CListDataPtr const& item, long iCol) const override;
	std::wstring OnNeedText(long iCol) const override;
	ARBDogRegNumPtr GetData() const
	{
		return m_RegNum;
	}

private:
	CDlgDogPanelRegNums* m_pDlg;
	ARBDogRegNumPtr m_RegNum;
};


int CDlgDogDataRegNum::OnCompare(CListDataPtr const& item, long iCol) const
{
	CDlgDogDataRegNumPtr pData2 = std::dynamic_pointer_cast<CDlgDogDataRegNum, CListData>(item);
	if (!pData2)
		return 1;

	ARBDogRegNumPtr pRegNum1 = GetData();
	ARBDogRegNumPtr pRegNum2 = pData2->GetData();
	int rc = 0;
	switch (iCol)
	{
	case k_colVenue:
		if (pRegNum1->GetVenue() < pRegNum2->GetVenue())
			rc = -1;
		else if (pRegNum1->GetVenue() > pRegNum2->GetVenue())
			rc = 1;
		break;
	case k_colNumber:
		if (pRegNum1->GetNumber() < pRegNum2->GetNumber())
			rc = -1;
		else if (pRegNum1->GetNumber() > pRegNum2->GetNumber())
			rc = 1;
		break;
	case k_colHeight:
		if (pRegNum1->GetNumber() < pRegNum2->GetNumber())
			rc = -1;
		else if (pRegNum1->GetNumber() > pRegNum2->GetNumber())
			rc = 1;
		break;
	case k_colReceived:
		if (!pRegNum1->GetReceived() && pRegNum2->GetReceived())
			rc = -1;
		else if (pRegNum1->GetReceived() && !pRegNum2->GetReceived())
			rc = 1;
		break;
	case k_colNote:
		if (pRegNum1->GetNote() < pRegNum2->GetNote())
			rc = -1;
		else if (pRegNum1->GetNote() > pRegNum2->GetNote())
			rc = 1;
		break;
	default:
		break;
	}
	return rc;
}


std::wstring CDlgDogDataRegNum::OnNeedText(long iCol) const
{
	std::wstring text;
	switch (iCol)
	{
	case k_colVenue:
		text = m_RegNum->GetVenue();
		break;
	case k_colNumber:
		text = m_RegNum->GetNumber();
		break;
	case k_colHeight:
		text = m_RegNum->GetHeight();
		break;
	case k_colReceived:
		text = m_RegNum->GetReceived() ? L"x" : L"";
		break;
	case k_colNote:
		text = StringUtil::Replace(m_RegNum->GetNote(), L"\n", L" ");
		break;
	default:
		break;
	}
	return text;
}

/////////////////////////////////////////////////////////////////////////////

CDlgDogPanelRegNums::CDlgDogPanelRegNums(CAgilityBookDoc* pDoc, ARBDogPtr const& inDog, wxWindow* parent)
	: CDlgDogPanelReportBase(pDoc, inDog, parent)
	, m_RegNums()
{
	m_pDog->GetRegNums().Clone(m_RegNums);

	DoCreate(true, false);
	m_ctrlList->SetHelpText(_("HIDC_DOGNUM_REG_NUMS"));
	m_ctrlList->SetToolTip(_("HIDC_DOGNUM_REG_NUMS"));

	m_btnNew->SetLabel(_("IDC_DOGNUM_NEW"));
	m_btnNew->SetHelpText(_("HIDC_DOGNUM_NEW"));
	m_btnNew->SetToolTip(_("HIDC_DOGNUM_NEW"));

	m_btnEdit->SetLabel(_("IDC_DOGNUM_EDIT"));
	m_btnEdit->SetHelpText(_("HIDC_DOGNUM_EDIT"));
	m_btnEdit->SetToolTip(_("HIDC_DOGNUM_EDIT"));

	m_btnDelete->SetLabel(_("IDC_DOGNUM_DELETE"));
	m_btnDelete->SetHelpText(_("HIDC_DOGNUM_DELETE"));
	m_btnDelete->SetToolTip(_("HIDC_DOGNUM_DELETE"));

	UpdateControls();
}


bool CDlgDogPanelRegNums::Save(unsigned int& hint)
{
	CDlgDogPanelReportBase::Save(hint);

	bool bModified = false;
	if (m_pDog->GetRegNums() != m_RegNums)
	{
		bModified = true;
		hint |= UPDATE_POINTS_VIEW;
		m_pDog->GetRegNums() = m_RegNums;
	}
	return bModified;
}


CDlgDogDataRegNumPtr CDlgDogPanelRegNums::GetRegNumData(long index) const
{
	return std::dynamic_pointer_cast<CDlgDogDataRegNum, CListData>(m_ctrlList->GetData(index));
}


void CDlgDogPanelRegNums::DoInitList()
{
	m_reportColumn.CreateColumns(k_columnsRegNumInfo, -1, L"RegNums");
	ListRegNums();
}


void CDlgDogPanelRegNums::DoNew()
{
	CDlgRegNum dlg(m_pDoc->Book().GetConfig(), m_RegNums, ARBDogRegNumPtr(), this);
	if (wxID_OK == dlg.ShowModal())
		ListRegNums();
}


void CDlgDogPanelRegNums::DoEdit()
{
	long i = m_ctrlList->GetFirstSelected();
	if (0 <= i)
	{
		CDlgDogDataRegNumPtr pData = GetRegNumData(i);
		ARBDogRegNumPtr pRegNum;
		if (pData)
			pRegNum = pData->GetData();
		CDlgRegNum dlg(m_pDoc->Book().GetConfig(), m_RegNums, pRegNum, this);
		if (wxID_OK == dlg.ShowModal())
			ListRegNums();
	}
}


void CDlgDogPanelRegNums::DoDelete()
{
	long i = m_ctrlList->GetFirstSelected();
	if (0 <= i)
	{
		CDlgDogDataRegNumPtr pData = GetRegNumData(i);
		ARBDogRegNumPtr pRegNum;
		if (pData)
			pRegNum = pData->GetData();
		m_RegNums.DeleteRegNum(pRegNum->GetVenue(), pRegNum->GetNumber());
		DeleteListItem(i);
		UpdateControls();
	}
}


void CDlgDogPanelRegNums::ListRegNums()
{
	wxBusyCursor wait;
	ARBDogRegNumPtr pSelected;
	long i = m_ctrlList->GetFirstSelected();
	if (0 <= i)
	{
		CDlgDogDataRegNumPtr pData = GetRegNumData(i);
		if (pData)
			pSelected = pData->GetData();
	}
	m_ctrlList->DeleteAllItems();
	for (ARBDogRegNumList::const_iterator iterRegNum = m_RegNums.begin(); iterRegNum != m_RegNums.end(); ++iterRegNum)
	{
		CDlgDogDataRegNumPtr pData(std::make_shared<CDlgDogDataRegNum>(this, *iterRegNum));
		m_ctrlList->InsertItem(pData);
	}
	m_reportColumn.SizeColumns();
	m_reportColumn.Sort();

	if (pSelected)
	{
		for (i = 0; i < m_ctrlList->GetItemCount(); ++i)
		{
			CDlgDogDataRegNumPtr pData = GetRegNumData(i);
			if (pData && pData->GetData() == pSelected) // compare by ptr is fine.
			{
				m_ctrlList->Select(i, true);
				m_ctrlList->Focus(i);
				break;
			}
		}
	}
	UpdateControls();
}

} // namespace dconSoft
