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
 * 2016-04-29 Separate lifetime points from title (run) points.
 * 2006-07-15 Only adjust column widths the first time.
 *            Add a selected-item total.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-06-16 Changed ARBDate::GetString to put leadingzero into format.
 * 2004-02-03 Created.
 */

#include "stdafx.h"
#include "DlgDogPanelPoints.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "DlgExistingPoints.h"

#include "ARB/ARBDog.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ListData.h"
#include "LibARBWin/ReportListCtrl.h"
#include "LibARBWin/Validators.h"
#include "LibARBWin/Widgets.h"
#include <wx/datectrl.h>
#include <wx/dateevt.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

namespace
{
static const std::vector<CReportListHeader::ColumnInfo> k_columnsPointsInfo{
	{0, wxLIST_FORMAT_LEFT, arbT("IDS_COL_DATE")},
	{1, wxLIST_FORMAT_LEFT, arbT("IDS_COL_TYPE")},
	{2, wxLIST_FORMAT_LEFT, arbT("IDS_COL_POINTS")},
	{3, wxLIST_FORMAT_LEFT, arbT("IDS_OTHERPOINTS")},
	{4, wxLIST_FORMAT_LEFT, arbT("IDS_COL_VENUE")},
	{5, wxLIST_FORMAT_LEFT, arbT("IDS_COL_MULTIQ")},
	{6, wxLIST_FORMAT_LEFT, arbT("IDS_COL_DIVISION")},
	{7, wxLIST_FORMAT_LEFT, arbT("IDS_COL_LEVEL")},
	{8, wxLIST_FORMAT_LEFT, arbT("IDS_COL_EVENT")},
	{9, wxLIST_FORMAT_LEFT, arbT("IDS_COL_SUBNAME")},
	{10, wxLIST_FORMAT_LEFT, arbT("IDS_COL_COMMENTS")},
};
} // namespace

/////////////////////////////////////////////////////////////////////////////

class CDlgDogDataPoint : public CListData
{
public:
	CDlgDogDataPoint(CDlgDogPanelPoints* pDlg, ARBDogExistingPointsPtr const& pts)
		: m_pDlg(pDlg)
		, m_Pts(pts)
	{
	}
	int OnCompare(CListDataPtr const& item, long iCol) const override;
	std::wstring OnNeedText(long iCol) const override;
	ARBDogExistingPointsPtr GetData() const
	{
		return m_Pts;
	}

private:
	CDlgDogPanelPoints* m_pDlg;
	ARBDogExistingPointsPtr m_Pts;
};

int CDlgDogDataPoint::OnCompare(CListDataPtr const& item, long iCol) const
{
	CDlgDogDataPointPtr pData2 = std::dynamic_pointer_cast<CDlgDogDataPoint, CListData>(item);
	if (!pData2)
		return 1;

	ARBDogExistingPointsPtr pExistingPoints1 = GetData();
	ARBDogExistingPointsPtr pExistingPoints2 = pData2->GetData();
	int rc = 0;
	switch (iCol)
	{
	case 0: // date
		if (pExistingPoints1->GetDate() < pExistingPoints2->GetDate())
			rc = -1;
		else if (pExistingPoints1->GetDate() > pExistingPoints2->GetDate())
			rc = 1;
		break;
	case 1: // Type
		if (pExistingPoints1->GetType() < pExistingPoints2->GetType())
			rc = -1;
		else if (pExistingPoints1->GetType() > pExistingPoints2->GetType())
			rc = 1;
		break;
	case 2: // Points
		if (pExistingPoints1->GetPoints() < pExistingPoints2->GetPoints())
			rc = -1;
		else if (pExistingPoints1->GetPoints() > pExistingPoints2->GetPoints())
			rc = 1;
		break;
	case 3: // Other Points
		if (pExistingPoints1->GetTypeName() < pExistingPoints2->GetTypeName())
			rc = -1;
		else if (pExistingPoints1->GetTypeName() > pExistingPoints2->GetTypeName())
			rc = 1;
		break;
	case 4: // Venue
		if (pExistingPoints1->GetVenue() < pExistingPoints2->GetVenue())
			rc = -1;
		else if (pExistingPoints1->GetVenue() > pExistingPoints2->GetVenue())
			rc = 1;
		break;
	case 5: // MultiQ
		if (pExistingPoints1->GetMultiQ() < pExistingPoints2->GetMultiQ())
			rc = -1;
		else if (pExistingPoints1->GetMultiQ() > pExistingPoints2->GetMultiQ())
			rc = 1;
		break;
	case 6: // Division
		if (pExistingPoints1->GetDivision() < pExistingPoints2->GetDivision())
			rc = -1;
		else if (pExistingPoints1->GetDivision() > pExistingPoints2->GetDivision())
			rc = 1;
		break;
	case 7: // Level
		if (pExistingPoints1->GetLevel() < pExistingPoints2->GetLevel())
			rc = -1;
		else if (pExistingPoints1->GetLevel() > pExistingPoints2->GetLevel())
			rc = 1;
		break;
	case 8: // Event
		if (pExistingPoints1->GetEvent() < pExistingPoints2->GetEvent())
			rc = -1;
		else if (pExistingPoints1->GetEvent() > pExistingPoints2->GetEvent())
			rc = 1;
		break;
	case 9: // Subname
		if (pExistingPoints1->GetSubName() < pExistingPoints2->GetSubName())
			rc = -1;
		else if (pExistingPoints1->GetSubName() > pExistingPoints2->GetSubName())
			rc = 1;
		break;
	case 10: // Comment
		if (pExistingPoints1->GetComment() < pExistingPoints2->GetComment())
			rc = -1;
		else if (pExistingPoints1->GetComment() > pExistingPoints2->GetComment())
			rc = 1;
		break;
	}
	return rc;
}


std::wstring CDlgDogDataPoint::OnNeedText(long iCol) const
{
	std::wstring text;
	switch (iCol)
	{
	case 0:
		text = m_Pts->GetDate().GetString();
		break;
	case 1: // Type
		text = ARBDogExistingPoints::GetPointTypeName(m_Pts->GetType());
		break;
	case 2: // Points
		text = fmt::format(L"{}", m_Pts->GetPoints());
		break;
	case 3: // Other Points
		text = m_Pts->GetTypeName();
		break;
	case 4: // Venue
		text = m_Pts->GetVenue();
		break;
	case 5: // MultiQ
		text = m_Pts->GetMultiQ();
		break;
	case 6: // Division
		text = m_Pts->GetDivision();
		break;
	case 7: // Level
		text = m_Pts->GetLevel();
		break;
	case 8: // Event
		text = m_Pts->GetEvent();
		break;
	case 9: // SubName
		text = m_Pts->GetSubName();
		break;
	case 10: // Comment
		text = StringUtil::Replace(m_Pts->GetComment(), L"\n", L" ");
		break;
	}
	return text;
}

/////////////////////////////////////////////////////////////////////////////

CDlgDogPanelPoints::CDlgDogPanelPoints(CAgilityBookDoc* pDoc, ARBDogPtr const& inDog, wxWindow* parent)
	: CDlgDogPanelReportBase(pDoc, inDog, parent)
	, m_ctrlSelectedPts(nullptr)
	, m_ExistingPoints()
{
	m_pDog->GetExistingPoints().Clone(m_ExistingPoints);

	DoCreate(false, false);
	m_ctrlList->SetHelpText(_("HIDC_DOGPTS_POINTS"));
	m_ctrlList->SetToolTip(_("HIDC_DOGPTS_POINTS"));

	m_btnNew->SetLabel(_("IDC_DOGPTS_NEW"));
	m_btnNew->SetHelpText(_("HIDC_DOGPTS_NEW"));
	m_btnNew->SetToolTip(_("HIDC_DOGPTS_NEW"));

	m_btnEdit->SetLabel(_("IDC_DOGPTS_EDIT"));
	m_btnEdit->SetHelpText(_("HIDC_DOGPTS_EDIT"));
	m_btnEdit->SetToolTip(_("HIDC_DOGPTS_EDIT"));

	m_btnDelete->SetLabel(_("IDC_DOGPTS_DELETE"));
	m_btnDelete->SetHelpText(_("HIDC_DOGPTS_DELETE"));
	m_btnDelete->SetToolTip(_("HIDC_DOGPTS_DELETE"));

	wxStaticText* textPts
		= new wxStaticText(this, wxID_ANY, _("IDC_DOGPTS_SELECTED_PTS"), wxDefaultPosition, wxDefaultSize, 0);
	textPts->Wrap(-1);

	m_ctrlSelectedPts = new wxStaticText(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 30), -1),
		wxALIGN_CENTRE | wxST_NO_AUTORESIZE | wxSUNKEN_BORDER);
	m_ctrlSelectedPts->Wrap(-1);

	// Sizers

	m_sizerButtons->Add(0, 0, 1, wxEXPAND, 0);
	m_sizerButtons->Add(textPts, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	m_sizerButtons->Add(m_ctrlSelectedPts, 0, wxALIGN_CENTER_VERTICAL, wxDLG_UNIT_X(this, 5));

	Layout();
	GetSizer()->Fit(this);

	UpdateControls();
}


bool CDlgDogPanelPoints::Save(unsigned int& hint)
{
	bool bModified = false;

	if (m_pDog->GetExistingPoints() != m_ExistingPoints)
	{
		bModified = true;
		hint |= UPDATE_POINTS_VIEW;
		m_pDog->GetExistingPoints() = m_ExistingPoints;
	}

	return bModified;
}


CDlgDogDataPointPtr CDlgDogPanelPoints::GetPointData(long index) const
{
	return std::dynamic_pointer_cast<CDlgDogDataPoint, CListData>(m_ctrlList->GetData(index));
}


void CDlgDogPanelPoints::DoInitList()
{
	m_reportColumn.CreateColumns(k_columnsPointsInfo, -1, L"ExistingPoints");
	ListExistingPoints();
}


void CDlgDogPanelPoints::UpdateControls()
{
	CDlgDogPanelReportBase::UpdateControls();

	if (m_ctrlSelectedPts)
	{
		std::wstring str;
		std::vector<long> indices;
		if (0 < m_ctrlList->GetSelection(indices))
		{
			double total = 0;
			for (auto idx : indices)
			{
				CDlgDogDataPointPtr pData = GetPointData(idx);
				if (pData)
					total += pData->GetData()->GetPoints();
			}
			str = fmt::format(L"{}", total);
		}
		m_ctrlSelectedPts->SetLabel(str);
	}
}


void CDlgDogPanelPoints::DoNew()
{
	CDlgExistingPoints dlg(m_pDoc, m_ExistingPoints, ARBDogExistingPointsPtr(), this);
	if (wxID_OK == dlg.ShowModal())
		ListExistingPoints();
}


void CDlgDogPanelPoints::DoEdit()
{
	long i = m_ctrlList->GetSelection(true);
	if (0 <= i)
	{
		CDlgDogDataPointPtr pData = GetPointData(i);
		ARBDogExistingPointsPtr pExistingPoints;
		if (pData)
			pExistingPoints = pData->GetData();
		CDlgExistingPoints dlg(m_pDoc, m_ExistingPoints, pExistingPoints, this);
		if (wxID_OK == dlg.ShowModal())
			ListExistingPoints();
	}
}


void CDlgDogPanelPoints::DoDelete()
{
	std::vector<long> indices;
	if (0 < m_ctrlList->GetSelection(indices))
	{
		for (std::vector<long>::reverse_iterator i = indices.rbegin(); i != indices.rend(); ++i)
		{
			CDlgDogDataPointPtr pData = GetPointData(*i);
			ARBDogExistingPointsPtr pExistingPoints;
			if (pData)
				pExistingPoints = pData->GetData();
			m_ExistingPoints.DeleteExistingPoints(pExistingPoints);
			m_ctrlList->DeleteItem(*i);
		}
		UpdateControls();
	}
}


void CDlgDogPanelPoints::ListExistingPoints()
{
	wxBusyCursor wait;
	ARBDogExistingPointsPtr pSelected;
	int i = m_ctrlList->GetFirstSelected();
	if (0 <= i)
	{
		CDlgDogDataPointPtr pData = GetPointData(i);
		if (pData)
			pSelected = pData->GetData();
	}
	m_ctrlList->DeleteAllItems();
	i = 0;
	for (ARBDogExistingPointsList::const_iterator iterExistingPoints = m_ExistingPoints.begin();
		 iterExistingPoints != m_ExistingPoints.end();
		 ++i, ++iterExistingPoints)
	{
		CDlgDogDataPointPtr data(std::make_shared<CDlgDogDataPoint>(this, *iterExistingPoints));
		m_ctrlList->InsertItem(data);
	}
	m_reportColumn.SizeColumns();
	m_reportColumn.Sort();

	if (pSelected)
	{
		for (i = 0; i < m_ctrlList->GetItemCount(); ++i)
		{
			CDlgDogDataPointPtr pData = GetPointData(i);
			ARBDogExistingPointsPtr pExistingPoints;
			if (pData)
				pExistingPoints = pData->GetData();
			if (pExistingPoints == pSelected) // compare by ptr is fine.
			{
				m_ctrlList->Select(i, true);
				m_ctrlList->Focus(i);
				break;
			}
		}
	}
	UpdateControls();
}
