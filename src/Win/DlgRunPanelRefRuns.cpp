/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgRun class
 * @author David Connet
 *
 * Revision History
 * 2021-01-07 Separated into multiple files.
 * 2015-11-01 Added YPS.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-09-20 Added yourself was not getting up-to-date scoring info.
 * 2005-07-10 Add button to add yourself to ref-runs.
 *            Make default ref-run a 'Q'.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-09-28 Accumulate all heights for refrun dlg.
 * 2003-12-27 Changed FindEvent to take a date.
 * 2003-10-13 Make ref run dlg default to perfect score.
 */

#include "stdafx.h"
#include "DlgRunPanelRefRuns.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "DlgReferenceRun.h"
#include "DlgRun.h"

#include "ARB/ARBDogRun.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ListData.h"
#include "LibARBWin/ReportListCtrl.h"
#include <wx/dcbuffer.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

namespace
{
constexpr int k_colQ = 0;
constexpr int k_colPlace = 1;
constexpr int k_colScore = 2;
constexpr int k_colTime = 3;
constexpr int k_colYPS = 4;
constexpr int k_colName = 5;
constexpr int k_colHeight = 6;
constexpr int k_colBreed = 7;
constexpr int k_colNote = 8;
static const std::vector<CReportListHeader::ColumnInfo> k_columns{
	{k_colQ, wxLIST_FORMAT_CENTRE, arbT("IDS_COL_Q")},
	{k_colPlace, wxLIST_FORMAT_CENTRE, arbT("IDS_COL_PLACE")},
	{k_colScore, wxLIST_FORMAT_CENTRE, arbT("IDS_COL_SCORE")},
	{k_colTime, wxLIST_FORMAT_LEFT, arbT("IDS_COL_TIME")},
	{k_colYPS, wxLIST_FORMAT_LEFT, arbT("IDS_COL_YPS")},
	{k_colName, wxLIST_FORMAT_LEFT, arbT("IDS_COL_NAME")},
	{k_colHeight, wxLIST_FORMAT_LEFT, arbT("IDS_COL_HEIGHT")},
	{k_colBreed, wxLIST_FORMAT_LEFT, arbT("IDS_COL_BREED")},
	{k_colNote, wxLIST_FORMAT_LEFT, arbT("IDS_COL_NOTE")},
};
} // namespace

/////////////////////////////////////////////////////////////////////////////

class CDlgDogRefRunData : public CListData
{
	DECLARE_NO_COPY_IMPLEMENTED(CDlgDogRefRunData)
public:
	CDlgDogRefRunData(ARBDogRunPtr const& inRun, ARBDogReferenceRunPtr const& inRefRun)
		: m_Run(inRun)
		, m_RefRun(inRefRun)
	{
	}
	int OnCompare(CListDataPtr const& item, long iCol) const override;
	std::wstring OnNeedText(long iCol) const override;
	ARBDogReferenceRunPtr GetData() const
	{
		return m_RefRun;
	}

private:
	ARBDogRunPtr m_Run;
	ARBDogReferenceRunPtr m_RefRun;
};


int CDlgDogRefRunData::OnCompare(CListDataPtr const& item, long iCol) const
{
	CDlgDogRefRunDataPtr pData2 = std::dynamic_pointer_cast<CDlgDogRefRunData, CListData>(item);
	if (!pData2)
		return 1;

	ARBDogReferenceRunPtr pRefRun1 = GetData();
	ARBDogReferenceRunPtr pRefRun2 = pData2->GetData();
	int rc = 0;
	switch (iCol)
	{
	default:
	case k_colQ:
		if (pRefRun1->GetQ() < pRefRun2->GetQ())
			rc = -1;
		else if (pRefRun1->GetQ() > pRefRun2->GetQ())
			rc = 1;
		break;
	case k_colPlace:
		if (pRefRun1->GetPlace() < pRefRun2->GetPlace())
			rc = -1;
		else if (pRefRun1->GetPlace() > pRefRun2->GetPlace())
			rc = 1;
		break;
	case k_colScore:
		if (pRefRun1->GetScore() < pRefRun2->GetScore())
			rc = -1;
		else if (pRefRun1->GetScore() > pRefRun2->GetScore())
			rc = 1;
		break;
	case k_colTime:
		if (pRefRun1->GetTime() < pRefRun2->GetTime())
			rc = -1;
		else if (pRefRun1->GetTime() > pRefRun2->GetTime())
			rc = 1;
		break;
	case k_colYPS:
	{
		double time1 = pRefRun1->GetTime();
		double yps1;
		double time2 = pRefRun2->GetTime();
		double yps2;
		if (m_Run->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), time1, yps1)
			&& m_Run->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), time2, yps2))
		{
			if (yps1 < yps2)
				rc = -1;
			else if (yps1 > yps2)
				rc = 1;
		}
	}
	break;
	case k_colName:
		if (pRefRun1->GetName() < pRefRun2->GetName())
			rc = -1;
		else if (pRefRun1->GetName() > pRefRun2->GetName())
			rc = 1;
		break;
	case k_colHeight:
		if (pRefRun1->GetHeight() < pRefRun2->GetHeight())
			rc = -1;
		else if (pRefRun1->GetHeight() > pRefRun2->GetHeight())
			rc = 1;
		break;
	case k_colBreed:
		if (pRefRun1->GetBreed() < pRefRun2->GetBreed())
			rc = -1;
		else if (pRefRun1->GetBreed() > pRefRun2->GetBreed())
			rc = 1;
		break;
	case k_colNote:
		if (pRefRun1->GetNote() < pRefRun2->GetNote())
			rc = -1;
		else if (pRefRun1->GetNote() > pRefRun2->GetNote())
			rc = 1;
		break;
	}
	return rc;
}


std::wstring CDlgDogRefRunData::OnNeedText(long iCol) const
{
	std::wstring str;
	switch (iCol)
	{
	default:
		break;
	case k_colQ:
		str = fmt::format(L"{}", m_RefRun->GetQ().str());
		break;
	case k_colPlace:
		str = fmt::format(L"{}", m_RefRun->GetPlace());
		break;
	case k_colScore:
		str = m_RefRun->GetScore();
		break;
	case k_colTime:
		str = ARBDouble::ToString(m_RefRun->GetTime());
		break;
	case k_colYPS:
	{
		double time = m_RefRun->GetTime();
		double yps;
		if (m_Run->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), time, yps))
		{
			str = ARBDouble::ToString(yps, 3);
		}
	}
	break;
	case k_colName:
		str = m_RefRun->GetName();
		break;
	case k_colHeight:
		str = m_RefRun->GetHeight();
		break;
	case k_colBreed:
		str = m_RefRun->GetBreed();
		break;
	case k_colNote:
		str = StringUtil::Replace(m_RefRun->GetNote(), L"\n", L" ");
		break;
	}
	return str;
}

/////////////////////////////////////////////////////////////////////////////

CDlgRunPanelRefRuns::CDlgRunPanelRefRuns(
	CDlgRun* pDlg,
	CAgilityBookDoc* pDoc,
	ARBDogPtr const& inDog,
	ARBDogTrialPtr const& inTrial,
	ARBDogRunPtr const& inRun,
	wxWindow* parent)
	: CDlgRunPanelBase(pDlg, pDoc, inDog, inTrial, inRun, parent)
	, m_pRefRunMe()
	, m_ctrlRefRuns(nullptr)
	, m_reportColumn()
	, m_ctrlRefAddMe(nullptr)
	, m_ctrlRefEdit(nullptr)
	, m_ctrlRefDelete(nullptr)
{
	// Controls (these are done first to control tab order)

	m_ctrlRefRuns = new CReportListCtrl(this, true, CReportListCtrl::SortHeader::Sort, true);
	m_ctrlRefRuns->Bind(wxEVT_COMMAND_LIST_ITEM_SELECTED, &CDlgRunPanelRefRuns::OnRefRunItemSelected, this);
	m_ctrlRefRuns->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &CDlgRunPanelRefRuns::OnRefRunItemActivated, this);
	m_ctrlRefRuns->Bind(wxEVT_KEY_DOWN, &CDlgRunPanelRefRuns::OnRefRunKeyDown, this);
	m_ctrlRefRuns->SetHelpText(_("HIDC_RUNREF_REF_RUNS"));
	m_ctrlRefRuns->SetToolTip(_("HIDC_RUNREF_REF_RUNS"));
	m_reportColumn.Initialize(this, m_ctrlRefRuns);
	m_reportColumn.CreateColumns(k_columns, -1, L"RefRuns");

	wxButton* btnRefNew = new wxButton(this, wxID_ANY, _("IDC_RUNREF_NEW"), wxDefaultPosition, wxDefaultSize, 0);
	btnRefNew->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRunPanelRefRuns::OnRefRunNew, this);
	btnRefNew->SetHelpText(_("HIDC_RUNREF_NEW"));
	btnRefNew->SetToolTip(_("HIDC_RUNREF_NEW"));

	m_ctrlRefAddMe = new wxButton(this, wxID_ANY, _("IDC_RUNREF_ADDDOG"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlRefAddMe->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRunPanelRefRuns::OnRefRunAddMe, this);
	m_ctrlRefAddMe->SetHelpText(_("HIDC_RUNREF_ADDDOG"));
	m_ctrlRefAddMe->SetToolTip(_("HIDC_RUNREF_ADDDOG"));

	m_ctrlRefEdit = new wxButton(this, wxID_ANY, _("IDC_RUNREF_EDIT"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlRefEdit->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRunPanelRefRuns::OnRefRunEdit, this);
	m_ctrlRefEdit->SetHelpText(_("HIDC_RUNREF_EDIT"));
	m_ctrlRefEdit->SetToolTip(_("HIDC_RUNREF_EDIT"));

	m_ctrlRefDelete = new wxButton(this, wxID_ANY, _("IDC_RUNREF_DELETE"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlRefDelete->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRunPanelRefRuns::OnRefRunDelete, this);
	m_ctrlRefDelete->SetHelpText(_("HIDC_RUNREF_DELETE"));
	m_ctrlRefDelete->SetToolTip(_("HIDC_RUNREF_DELETE"));

	// Sizers

	wxBoxSizer* sizerRefRuns = new wxBoxSizer(wxVERTICAL);
	sizerRefRuns->Add(m_ctrlRefRuns, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerRefBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerRefBtns->Add(btnRefNew, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerRefBtns->Add(m_ctrlRefAddMe, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerRefBtns->Add(m_ctrlRefEdit, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerRefBtns->Add(m_ctrlRefDelete, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerRefRuns->Add(sizerRefBtns, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(sizerRefRuns);
	Layout();
	sizerRefRuns->Fit(this);

	ListRefRuns();
}


wxWindow* CDlgRunPanelRefRuns::GetInitialControl()
{
	return m_ctrlRefRuns;
}


void CDlgRunPanelRefRuns::OnActivation()
{
	// Recreate the 'me' reference run on page change. This will push in
	// any changes made in the scoring page.
	CreateRefRunMe();
	UpdateRefRunButtons();
}


bool CDlgRunPanelRefRuns::Validate()
{
	return true;
}


bool CDlgRunPanelRefRuns::Save()
{
	m_reportColumn.Save();
	return true;
}


CDlgDogRefRunDataPtr CDlgRunPanelRefRuns::GetReferenceData(long index) const
{
	return std::dynamic_pointer_cast<CDlgDogRefRunData, CListData>(m_ctrlRefRuns->GetData(index));
}


bool CDlgRunPanelRefRuns::IsRefRunMe()
{
	if (m_pRefRunMe)
	{
		for (ARBDogReferenceRunList::const_iterator iterRef = m_Run->GetReferenceRuns().begin();
			 iterRef != m_Run->GetReferenceRuns().end();
			 ++iterRef)
		{
			if (*iterRef == m_pRefRunMe)
			{
				return true;
			}
		}
	}
	return false;
}


void CDlgRunPanelRefRuns::CreateRefRunMe()
{
	std::wstring venue;
	if (m_Run->GetClub())
		venue = m_Run->GetClub()->GetVenue();
	else
		venue = m_pDlg->GetCurrentVenueName();

	// Create the 'me' reference run.
	m_pRefRunMe = ARBDogReferenceRunPtr(ARBDogReferenceRun::New());
	m_pRefRunMe->SetQ(m_Run->GetQ());
	m_pRefRunMe->SetPlace(m_Run->GetPlace());
	m_pRefRunMe->SetName(m_pDoc->GetCurrentDog()->GetCallName());
	m_pRefRunMe->SetHeight(m_Run->GetHeight());
	m_pRefRunMe->SetBreed(m_pDoc->GetCurrentDog()->GetBreed());
	m_pRefRunMe->SetTime(m_Run->GetScoring().GetTime());
	ARBConfigScoringPtr pScoring;
	if (m_pDoc->Book().GetConfig().GetVenues().FindEvent(
			venue,
			m_Run->GetEvent(),
			m_Run->GetDivision(),
			m_Run->GetLevel(),
			m_Run->GetDate(),
			nullptr,
			&pScoring))
	{
		m_pRefRunMe->SetScore(ARBDouble::ToString(m_Run->GetScore(pScoring)));
	}

	// Now see if I'm already in there.
	// Only compare: Q/Place/Name/Time/Score.
	for (ARBDogReferenceRunList::const_iterator iterRef = m_Run->GetReferenceRuns().begin();
		 iterRef != m_Run->GetReferenceRuns().end();
		 ++iterRef)
	{
		ARBDogReferenceRunPtr pRef = *iterRef;
		if (pRef->GetQ() == m_pRefRunMe->GetQ() && pRef->GetPlace() == m_pRefRunMe->GetPlace()
			&& pRef->GetName() == m_pRefRunMe->GetName() && ARBDouble::equal(pRef->GetTime(), m_pRefRunMe->GetTime())
			&& pRef->GetScore() == m_pRefRunMe->GetScore())
		{
			m_pRefRunMe = pRef;
			break;
		}
	}
	// Note: This means if we create 'me', modify it, switch tabs
	// and come back, 'addme' is enabled again.
}


void CDlgRunPanelRefRuns::UpdateRefRunButtons()
{
	m_ctrlRefAddMe->Enable(!IsRefRunMe());
	bool bSelected = (0 <= m_ctrlRefRuns->GetFirstSelected());
	m_ctrlRefEdit->Enable(bSelected);
	m_ctrlRefDelete->Enable(bSelected);
}


void CDlgRunPanelRefRuns::ListRefRuns()
{
	ARBDogReferenceRunPtr pSelected;
	long index = m_ctrlRefRuns->GetFirstSelected();
	if (0 <= index)
	{
		ARBDogReferenceRunPtr pData = GetReferenceData(index)->GetData();
		if (pData)
			pSelected = pData;
	}
	m_ctrlRefRuns->DeleteAllItems();

	index = 0;
	for (ARBDogReferenceRunList::const_iterator iterRef = m_Run->GetReferenceRuns().begin();
		 iterRef != m_Run->GetReferenceRuns().end();
		 ++index, ++iterRef)
	{
		CDlgDogRefRunDataPtr data(std::make_shared<CDlgDogRefRunData>(m_Run, *iterRef));
		m_ctrlRefRuns->InsertItem(data);
	}
	m_reportColumn.SizeColumns();
	m_reportColumn.Sort();

	if (pSelected)
	{
		for (index = 0; index < m_ctrlRefRuns->GetItemCount(); ++index)
		{
			ARBDogReferenceRunPtr pRefRun = GetReferenceData(index)->GetData();
			if (pRefRun == pSelected) // compare by ptr is fine.
			{
				m_ctrlRefRuns->SetSelection(index, true);
				break;
			}
		}
	}
	UpdateRefRunButtons();
}


void CDlgRunPanelRefRuns::GetAllHeights(std::set<std::wstring>& outNames)
{
	m_pDoc->Book().GetAllHeights(outNames);
	for (long index = 0; index < m_ctrlRefRuns->GetItemCount(); ++index)
	{
		ARBDogReferenceRunPtr pRefRun = GetReferenceData(index)->GetData();
		if (pRefRun)
		{
			std::wstring const& ht = pRefRun->GetHeight();
			if (0 < ht.length())
				outNames.insert(ht);
		}
	}
}


void CDlgRunPanelRefRuns::GetAllCallNames(std::set<std::wstring>& outNames)
{
	m_pDoc->Book().GetAllCallNames(outNames);
	for (int index = 0; index < m_ctrlRefRuns->GetItemCount(); ++index)
	{
		ARBDogReferenceRunPtr pRefRun = GetReferenceData(index)->GetData();
		if (pRefRun)
		{
			std::wstring const& ht = pRefRun->GetName();
			if (0 < ht.length())
				outNames.insert(ht);
		}
	}
}


void CDlgRunPanelRefRuns::GetAllBreeds(std::set<std::wstring>& outNames)
{
	m_pDoc->Book().GetAllBreeds(outNames);
	for (int index = 0; index < m_ctrlRefRuns->GetItemCount(); ++index)
	{
		ARBDogReferenceRunPtr pRefRun = GetReferenceData(index)->GetData();
		if (pRefRun)
		{
			std::wstring const& ht = pRefRun->GetBreed();
			if (0 < ht.length())
				outNames.insert(ht);
		}
	}
}


void CDlgRunPanelRefRuns::EditRefRun()
{
	long nItem = m_ctrlRefRuns->GetFirstSelected();
	if (0 <= nItem)
	{
		std::set<std::wstring> heights, names, breeds;
		GetAllHeights(heights);
		GetAllCallNames(names);
		GetAllBreeds(breeds);
		ARBDogReferenceRunPtr pRef = GetReferenceData(nItem)->GetData();
		CDlgReferenceRun dlg(m_pDoc, m_Run, heights, names, breeds, pRef, this);
		if (wxID_OK == dlg.ShowModal())
			ListRefRuns();
	}
}


void CDlgRunPanelRefRuns::OnRefRunItemSelected(wxListEvent& evt)
{
	UpdateRefRunButtons();
}


void CDlgRunPanelRefRuns::OnRefRunItemActivated(wxListEvent& evt)
{
	EditRefRun();
}


void CDlgRunPanelRefRuns::OnRefRunKeyDown(wxKeyEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		evt.Skip();
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
		EditRefRun();
		break;
	}
}


void CDlgRunPanelRefRuns::OnRefRunNew(wxCommandEvent& evt)
{
	ARBDogReferenceRunPtr ref(ARBDogReferenceRun::New());
	if (ARBScoringType::ByTime == m_Run->GetScoring().GetType())
	{
		std::wstring venue;
		if (m_Run->GetClub())
			venue = m_Run->GetClub()->GetVenue();
		else
			venue = m_pDlg->GetCurrentVenueName();

		ARBConfigScoringPtr pScoring;
		if (m_pDoc->Book().GetConfig().GetVenues().FindEvent(
				venue,
				m_Run->GetEvent(),
				m_Run->GetDivision(),
				m_Run->GetLevel(),
				m_Run->GetDate(),
				nullptr,
				&pScoring))
		{
			std::wstring nScore;
			switch (pScoring->GetScoringStyle())
			{
			case ARBScoringStyle::Unknown:
			case ARBScoringStyle::FaultsThenTime:
			case ARBScoringStyle::OCScoreThenTime:
			case ARBScoringStyle::ScoreThenTime:
			case ARBScoringStyle::TimePlusFaults:
			case ARBScoringStyle::TimeNoPlaces:
				nScore = L"0";
				break;
			case ARBScoringStyle::Faults100ThenTime:
				nScore = L"100";
				break;
			case ARBScoringStyle::Faults200ThenTime:
				nScore = L"200";
				break;
			}
			ref->SetScore(nScore);
		}
	}
	ref->SetQ(Q::Q);
	std::set<std::wstring> heights, names, breeds;
	GetAllHeights(heights);
	GetAllCallNames(names);
	GetAllBreeds(breeds);
	CDlgReferenceRun dlg(m_pDoc, m_Run, heights, names, breeds, ref, this);
	if (wxID_OK == dlg.ShowModal())
	{
		if (m_Run->GetReferenceRuns().AddReferenceRun(ref))
		{
			CDlgDogRefRunDataPtr data(std::make_shared<CDlgDogRefRunData>(m_Run, ref));
			long index = m_ctrlRefRuns->InsertItem(data);
			m_ctrlRefRuns->Select(index);
			// Insert item first to set selection.
			// ListRefRuns then 'fixes' order.
			ListRefRuns();
		}
	}
}


void CDlgRunPanelRefRuns::OnRefRunAddMe(wxCommandEvent& evt)
{
	if (!IsRefRunMe())
	{
		if (m_Run->GetReferenceRuns().AddReferenceRun(m_pRefRunMe))
		{
			CDlgDogRefRunDataPtr data(std::make_shared<CDlgDogRefRunData>(m_Run, m_pRefRunMe));
			long index = m_ctrlRefRuns->InsertItem(data);
			m_ctrlRefRuns->Select(index);
			// Insert item first to set selection.
			// ListRefRuns then 'fixes' order.
			ListRefRuns();
		}
	}
}


void CDlgRunPanelRefRuns::OnRefRunEdit(wxCommandEvent& evt)
{
	EditRefRun();
}


void CDlgRunPanelRefRuns::OnRefRunDelete(wxCommandEvent& evt)
{
	long nItem = m_ctrlRefRuns->GetFirstSelected();
	if (0 <= nItem)
	{
		ARBDogReferenceRunPtr pRef = GetReferenceData(nItem)->GetData();
		if (pRef)
		{
			if (m_Run->GetReferenceRuns().DeleteReferenceRun(pRef))
			{
				m_ctrlRefRuns->DeleteItem(nItem);
				if (nItem == m_ctrlRefRuns->GetItemCount())
					--nItem;
				if (0 <= nItem)
					m_ctrlRefRuns->Select(nItem);
			}
			if (m_pRefRunMe && m_pRefRunMe == pRef)
				CreateRefRunMe();
		}
		UpdateRefRunButtons();
	}
}
