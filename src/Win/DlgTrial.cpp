/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgTrial class
 * @author David Connet
 *
 * Revision History
 * 2017-11-30 Remove image list from club list (not needed)
 * 2015-01-01 Changed pixels to dialog units.
 * 2014-02-20 Trial start date was not properly saved.
 * 2012-05-22 Change KillFocus handler to text change handler.
 * 2012-05-07 Added autocompletion to combo boxes.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2011-08-16 Make trial notes multi-line.
 * 2009-10-14 Add dog's name to dialog caption.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-08-12 Fix killfocus handling.
 * 2009-02-09 Ported to wxWidgets.
 * 2008-02-01 Make 'Notes' button change selection.
 * 2007-12-03 Refresh location list after invoking 'notes' button.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-13 Added direct access to Notes dialog.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-12-19 Added Location/Club note information.
 * 2003-12-27 Changed FindEvent to take a date.
 */

#include "stdafx.h"
#include "DlgTrial.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "DlgClub.h"
#include "DlgInfoNote.h"
#include "NoteButton.h"

#include "ARB/ARBDogTrial.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/DlgPadding.h"
#include "LibARBWin/ListData.h"
#include "LibARBWin/ReportListCtrl.h"
#include "LibARBWin/RichEditCtrl2.h"
#include "LibARBWin/Validators.h"
#include <wx/datectrl.h>
#include <wx/valgen.h>
#include <set>

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
constexpr int DEF_TRIAL_NOTES_HEIGHT = 35;
constexpr int DEF_CLUB_WIDTH = 180;
constexpr int DEF_CLUB_HEIGHT = 50;
constexpr int DEF_NOTE_WIDTH = 95;
} // namespace


class CListTrialData : public CListData
{
	DECLARE_NO_COPY_IMPLEMENTED(CListTrialData)
public:
	CListTrialData(ARBDogClubPtr const& inClub)
		: m_Club(inClub)
	{
	}
	wxString OnNeedText(long iCol) const override;
	ARBDogClubPtr GetClub() const
	{
		return m_Club;
	}

private:
	ARBDogClubPtr m_Club;
};
typedef std::shared_ptr<CListTrialData> CListTrialDataPtr;


wxString CListTrialData::OnNeedText(long iCol) const
{
	wxString text;
	switch (iCol)
	{
	default:
		assert(0);
		return wxString();
	case 0:
		text = m_Club->GetName();
		break;
	case 1:
		text = m_Club->GetVenue();
		break;
	case 2:
		if (m_Club->GetPrimaryClub())
			text = m_Club->GetPrimaryClub()->GetVenue();
		break;
	}
	return text;
}

/////////////////////////////////////////////////////////////////////////////

CDlgTrial::CDlgTrial(CAgilityBookDoc* pDoc, ARBDogTrialPtr const& inTrial, wxWindow* pParent)
	: wxDialog()
	, m_dateStart(inTrial->GetStartDate())
	, m_Verified(inTrial->IsVerified())
	, m_Location(inTrial->GetLocation())
	, m_Notes(inTrial->GetNote())
	, m_ctrlLocationInfo(nullptr)
	, m_ctrlEdit(nullptr)
	, m_ctrlDelete(nullptr)
	, m_ctrlUp(nullptr)
	, m_ctrlDn(nullptr)
	, m_ctrlClubNotes(nullptr)
	, m_pDoc(pDoc)
	, m_pTrial(inTrial)
	, m_Clubs()
	, m_bShowCoSanction(CAgilityBookOptions::ShowCoSanctioning())
	, m_bFixup(false)
	, m_bRunsDeleted(false)
	, m_trace("CDlgTrial")
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(
		pParent,
		wxID_ANY,
		pDoc->AddDogToCaption(_("IDD_TRIAL")),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	m_pTrial->GetClubs().Clone(m_Clubs);

	if (!m_bShowCoSanction)
	{
		for (ARBDogClubList::const_iterator iter = m_Clubs.begin(); !m_bShowCoSanction && iter != m_Clubs.end(); ++iter)
			if ((*iter)->GetPrimaryClub())
				m_bShowCoSanction = true;
	}

	if (!m_dateStart.IsValid())
		m_dateStart.SetToday();

	// Controls (these are done first to control tab order)

	wxStaticText* textStart
		= new wxStaticText(this, wxID_ANY, _("IDC_TRIAL_DATE_START"), wxDefaultPosition, wxDefaultSize, 0);
	textStart->Wrap(-1);

	m_ctrlStart = new wxDatePickerCtrl(
		this,
		wxID_ANY,
		wxDefaultDateTime,
		wxDefaultPosition,
		wxDefaultSize,
		wxDP_DROPDOWN | wxDP_SHOWCENTURY,
		CGenericValidator(&m_dateStart));
	m_ctrlStart->SetHelpText(_("HIDC_TRIAL_DATE_START"));
	m_ctrlStart->SetToolTip(_("HIDC_TRIAL_DATE_START"));
	if (0 < m_pTrial->GetRuns().size())
		m_ctrlStart->Enable(false);

	wxCheckBox* checkVerified = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_TRIAL_VERIFIED"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_Verified));
	checkVerified->SetHelpText(_("HIDC_TRIAL_VERIFIED"));
	checkVerified->SetToolTip(_("HIDC_TRIAL_VERIFIED"));

	wxStaticText* textLocation
		= new wxStaticText(this, wxID_ANY, _("IDC_TRIAL_LOCATION"), wxDefaultPosition, wxDefaultSize, 0);
	textLocation->Wrap(-1);

	m_ctrlLocation = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_SORT,
		CTrimValidator(&m_Location),
		_("IDS_ENTER_NAME"));
	m_ctrlLocation->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgTrial::OnEnChangeLocation, this);
	m_ctrlLocation->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgTrial::OnSelchangeLocation, this);
	m_ctrlLocation->SetHelpText(_("HIDC_TRIAL_LOCATION"));
	m_ctrlLocation->SetToolTip(_("HIDC_TRIAL_LOCATION"));

	wxStaticText* textNotes
		= new wxStaticText(this, wxID_ANY, _("IDC_TRIAL_NOTES"), wxDefaultPosition, wxDefaultSize, 0);
	textNotes->Wrap(-1);

	CSpellCheckCtrl* ctrlTrialNotes = new CSpellCheckCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(-1, wxDLG_UNIT_Y(this, DEF_TRIAL_NOTES_HEIGHT)),
		wxTE_MULTILINE,
		CTrimValidator(&m_Notes, TRIMVALIDATOR_TRIM_BOTH));
	ctrlTrialNotes->SetHelpText(_("HIDC_TRIAL_NOTES"));
	ctrlTrialNotes->SetToolTip(_("HIDC_TRIAL_NOTES"));

	m_ctrlLocationNotes = new CNoteButton(this);
	m_ctrlLocationNotes->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgTrial::OnLocationNotes, this);
	m_ctrlLocationNotes->SetHelpText(_("HIDC_TRIAL_LOCATION_NOTES"));
	m_ctrlLocationNotes->SetToolTip(_("HIDC_TRIAL_LOCATION_NOTES"));

	wxStaticText* noteLocationNotes
		= new wxStaticText(this, wxID_ANY, _("IDC_TRIAL_LOCATION_INFO"), wxDefaultPosition, wxDefaultSize, 0);
	noteLocationNotes->Wrap(-1);

	m_ctrlLocationInfo = new CRichEditCtrl2(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, DEF_NOTE_WIDTH), -1),
		true);
	m_ctrlLocationInfo->SetHelpText(_("HIDC_TRIAL_LOCATION_INFO"));
	m_ctrlLocationInfo->SetToolTip(_("HIDC_TRIAL_LOCATION_INFO"));

	wxStaticText* textClub
		= new wxStaticText(this, wxID_ANY, _("IDC_TRIAL_CLUBS"), wxDefaultPosition, wxDefaultSize, 0);
	textClub->Wrap(-1);

	wxButton* btnNew
		= new wxButton(this, wxID_ANY, _("IDC_TRIAL_CLUB_NEW"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	btnNew->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgTrial::OnClubNew, this);
	btnNew->SetHelpText(_("HIDC_TRIAL_CLUB_NEW"));
	btnNew->SetToolTip(_("HIDC_TRIAL_CLUB_NEW"));

	m_ctrlEdit
		= new wxButton(this, wxID_ANY, _("IDC_TRIAL_CLUB_EDIT"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_ctrlEdit->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgTrial::OnClubEdit, this);
	m_ctrlEdit->SetHelpText(_("HIDC_TRIAL_CLUB_EDIT"));
	m_ctrlEdit->SetToolTip(_("HIDC_TRIAL_CLUB_EDIT"));

	m_ctrlDelete
		= new wxButton(this, wxID_ANY, _("IDC_TRIAL_CLUB_DELETE"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_ctrlDelete->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgTrial::OnClubDelete, this);
	m_ctrlDelete->SetHelpText(_("HIDC_TRIAL_CLUB_DELETE"));
	m_ctrlDelete->SetToolTip(_("HIDC_TRIAL_CLUB_DELETE"));

	m_ctrlUp = new wxButton(this, wxID_ANY, _("IDC_TRIAL_CLUB_UP"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_ctrlUp->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgTrial::OnClubMoveUp, this);
	m_ctrlUp->SetHelpText(_("HIDC_TRIAL_CLUB_UP"));
	m_ctrlUp->SetToolTip(_("HIDC_TRIAL_CLUB_UP"));

	m_ctrlDn = new wxButton(this, wxID_ANY, _("IDC_TRIAL_CLUB_DOWN"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_ctrlDn->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgTrial::OnClubMoveDown, this);
	m_ctrlDn->SetHelpText(_("HIDC_TRIAL_CLUB_DOWN"));
	m_ctrlDn->SetToolTip(_("HIDC_TRIAL_CLUB_DOWN"));

	m_ctrlClubs = new CReportListCtrl(
		this,
		wxDefaultPosition,
		wxDLG_UNIT(this, wxSize(DEF_CLUB_WIDTH, DEF_CLUB_HEIGHT)),
		true,
		CReportListCtrl::SortHeader::NoSort,
		true,
		false);
	m_ctrlClubs->Bind(wxEVT_COMMAND_LIST_ITEM_SELECTED, &CDlgTrial::OnItemSelectedClubs, this);
	m_ctrlClubs->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &CDlgTrial::OnItemActivatedClubs, this);
	m_ctrlClubs->Bind(wxEVT_KEY_DOWN, &CDlgTrial::OnKeydownClubs, this);
	m_ctrlClubs->SetHelpText(_("HIDC_TRIAL_CLUBS"));
	m_ctrlClubs->SetToolTip(_("HIDC_TRIAL_CLUBS"));

	m_ctrlClubNotes = new CNoteButton(this);
	m_ctrlClubNotes->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgTrial::OnClubNotes, this);
	m_ctrlClubNotes->SetHelpText(_("HIDC_TRIAL_CLUB_NOTES"));
	m_ctrlClubNotes->SetToolTip(_("HIDC_TRIAL_CLUB_NOTES"));

	wxStaticText* textClubNotes
		= new wxStaticText(this, wxID_ANY, _("IDC_TRIAL_CLUB_INFO"), wxDefaultPosition, wxDefaultSize, 0);
	textClubNotes->Wrap(-1);

	m_ctrlClubInfo = new CRichEditCtrl2(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, DEF_NOTE_WIDTH), -1),
		true);
	m_ctrlClubInfo->SetHelpText(_("HIDC_TRIAL_CLUB_INFO"));
	m_ctrlClubInfo->SetToolTip(_("HIDC_TRIAL_CLUB_INFO"));

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerDate = new wxBoxSizer(wxHORIZONTAL);
	sizerDate->Add(textStart, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerDate->Add(m_ctrlStart);

	wxBoxSizer* sizerLocation = new wxBoxSizer(wxHORIZONTAL);
	sizerLocation->Add(textLocation, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerLocation->Add(m_ctrlLocation, 1, wxEXPAND);

	wxBoxSizer* sizerLocationNotes = new wxBoxSizer(wxHORIZONTAL);
	sizerLocationNotes->Add(m_ctrlLocationNotes, 0, wxALIGN_BOTTOM | wxRIGHT, padding.Inner());
	sizerLocationNotes->Add(noteLocationNotes, 0, wxALIGN_BOTTOM);

	wxBoxSizer* sizerNote = new wxBoxSizer(wxHORIZONTAL);
	sizerNote->Add(textNotes, 0, wxALIGN_TOP | wxRIGHT, padding.Inner());
	sizerNote->Add(ctrlTrialNotes, 1, wxEXPAND);

	wxBoxSizer* sizerClubBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerClubBtns->Add(textClub, 0, wxALIGN_BOTTOM);
	sizerClubBtns->AddStretchSpacer();
	sizerClubBtns->Add(btnNew, 0, wxRIGHT, padding.Inner());
	sizerClubBtns->Add(m_ctrlEdit, 0, wxRIGHT, padding.Inner());
	sizerClubBtns->Add(m_ctrlDelete, 0, wxRIGHT, padding.Inner());
	sizerClubBtns->Add(m_ctrlUp, 0, wxRIGHT, padding.Inner());
	sizerClubBtns->Add(m_ctrlDn);

	wxBoxSizer* sizerClubNotes = new wxBoxSizer(wxHORIZONTAL);
	sizerClubNotes->Add(m_ctrlClubNotes, 0, wxALIGN_BOTTOM | wxRIGHT, padding.Inner());
	sizerClubNotes->Add(textClubNotes);

	wxFlexGridSizer* sizerGrid = new wxFlexGridSizer(5, 2, 0, padding.Controls()); // rows/cols/vgap/hgap
	sizerGrid->SetFlexibleDirection(wxBOTH);
	sizerGrid->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);
	sizerGrid->AddGrowableCol(0, 3);
	sizerGrid->AddGrowableCol(1, 2);
	sizerGrid->AddGrowableRow(2);
	sizerGrid->AddGrowableRow(4);

	// row 0
	sizerGrid->Add(sizerDate, 0, wxEXPAND);
	sizerGrid->Add(checkVerified, 0, wxEXPAND);
	// row 1
	sizerGrid->Add(sizerLocation, 0, wxEXPAND | wxTOP, padding.Controls());
	sizerGrid->Add(sizerLocationNotes, 0, wxEXPAND | wxTOP, padding.Controls());
	// row 2
	sizerGrid->Add(sizerNote, 0, wxEXPAND | wxTOP, padding.TightControls());
	sizerGrid->Add(m_ctrlLocationInfo, 0, wxEXPAND | wxTOP, padding.TightControls());
	// row 3
	sizerGrid->Add(sizerClubBtns, 0, wxEXPAND | wxTOP, padding.Controls());
	sizerGrid->Add(sizerClubNotes, 0, wxEXPAND | wxTOP, padding.Controls());
	// row 4
	sizerGrid->Add(m_ctrlClubs, 0, wxEXPAND | wxTOP, padding.TightControls());
	sizerGrid->Add(m_ctrlClubInfo, 0, wxEXPAND | wxTOP, padding.TightControls());

	bSizer->Add(sizerGrid, 1, wxEXPAND | wxALL, padding.Controls());

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.ButtonSizer());
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgTrial::OnOk, this, wxID_OK);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgTrial::OnCancel, this, wxID_CANCEL);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	m_ctrlClubs->InsertColumn(0, _("IDS_COL_CLUB"));
	m_ctrlClubs->InsertColumn(1, _("IDS_COL_VENUE"));
	if (m_bShowCoSanction)
		m_ctrlClubs->InsertColumn(2, _("IDS_COL_COSANCTION"));
	m_ctrlEdit->Enable(false);
	m_ctrlClubNotes->Enable(false);
	m_ctrlDelete->Enable(false);
	m_ctrlUp->Enable(false);
	m_ctrlDn->Enable(false);
	TransferDataToWindow();
	ListLocations();
	ListClubs();
	UpdateNotes(m_Location, true, true);

	IMPLEMENT_ON_INIT(m_ctrlLocation)
}


ARBDogClubPtr CDlgTrial::GetClubData(long index) const
{
	CListTrialDataPtr pData = std::dynamic_pointer_cast<CListTrialData, CListData>(m_ctrlClubs->GetData(index));
	if (pData)
		return pData->GetClub();
	return ARBDogClubPtr();
}


void CDlgTrial::UpdateNotes(wxString const& location, bool bLocation, bool bClub)
{
	if (bLocation)
	{
		wxString str;
		ARBInfoItemPtr pItem;
		if (m_pDoc->Book().GetInfo().GetInfo(ARBInfoType::Location).FindItem(location, &pItem))
		{
			str = pItem->GetComment();
		}
		m_ctrlLocationInfo->SetValue(str);
	}
	if (bClub)
	{
		wxString str;
		ARBDogClubPtr pClub = GetClubData(m_ctrlClubs->GetFirstSelected());
		if (pClub)
		{
			ARBInfoItemPtr pItem;
			if (m_pDoc->Book().GetInfo().GetInfo(ARBInfoType::Club).FindItem(pClub->GetName(), &pItem))
			{
				str = pItem->GetComment();
			}
		}
		m_ctrlClubInfo->SetValue(str);
	}
}


void CDlgTrial::ListLocations()
{
	std::set<wxString> locations;
	m_pDoc->Book().GetAllTrialLocations(locations, true, true);
	if (!m_pTrial->GetLocation().empty())
		locations.insert(m_pTrial->GetLocation());
	wxString loc(m_Location);
	if (m_Location.empty())
		loc = m_pTrial->GetLocation();
	m_ctrlLocation->Clear();
	wxArrayString items;
	for (auto const& location : locations)
	{
		items.Add(location);
	}
	items.Sort();
	m_ctrlLocation->Append(items);
	m_ctrlLocation->SetSelection(m_ctrlLocation->FindString(loc));
	m_ctrlLocation->AutoComplete(items);
}


void CDlgTrial::ListClubs(ARBDogClubPtr const* inClub)
{
	m_ctrlClubs->DeleteAllItems();
	for (ARBDogClubList::const_iterator iter = m_Clubs.begin(); iter != m_Clubs.end(); ++iter)
	{
		ARBDogClubPtr pClub = *iter;
		CListTrialDataPtr data(std::make_shared<CListTrialData>(pClub));
		long idx = m_ctrlClubs->InsertItem(data);
		if (inClub && *(*inClub) == *pClub)
			m_ctrlClubs->Select(idx);
	}
	if (1 == m_ctrlClubs->GetItemCount())
		m_ctrlClubs->Select(0);
	int nColumns = m_ctrlClubs->GetColumnCount();
	for (int i = 0; i < nColumns; ++i)
		m_ctrlClubs->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
}


void CDlgTrial::EditClub()
{
	long index = m_ctrlClubs->GetFirstSelected();
	if (0 <= index)
	{
#pragma PRAGMA_TODO(Should prevent user from renaming club to an existing club)
		ARBDogClubPtr pClub = GetClubData(index);
		CDlgClub dlg(m_pDoc, m_Clubs, pClub, m_bShowCoSanction, this);
		if (wxID_OK == dlg.ShowModal())
			ListClubs(&pClub);
	}
}


void CDlgTrial::OnEnChangeLocation(wxCommandEvent& evt)
{
	wxString s = m_ctrlLocation->GetValue();
	UpdateNotes(s, true, false);
	evt.Skip();
}


void CDlgTrial::OnSelchangeLocation(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	UpdateNotes(m_Location, true, false);
}


void CDlgTrial::OnItemSelectedClubs(wxListEvent& evt)
{
	TransferDataFromWindow();
	long index = m_ctrlClubs->GetFirstSelected();
	if (0 <= index)
	{
		const int count = m_ctrlClubs->GetItemCount();
		m_ctrlEdit->Enable(true);
		m_ctrlClubNotes->Enable(true);
		m_ctrlDelete->Enable(true);
		m_ctrlUp->Enable(0 < index);
		m_ctrlDn->Enable(index < count - 1);
	}
	else
	{
		m_ctrlEdit->Enable(false);
		m_ctrlClubNotes->Enable(false);
		m_ctrlDelete->Enable(false);
		m_ctrlUp->Enable(false);
		m_ctrlDn->Enable(false);
	}
	UpdateNotes(m_Location, false, true);
}


void CDlgTrial::OnItemActivatedClubs(wxListEvent& evt)
{
	EditClub();
}


void CDlgTrial::OnKeydownClubs(wxKeyEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		evt.Skip();
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
		EditClub();
		break;
	}
}


void CDlgTrial::OnLocationNotes(wxCommandEvent& evt)
{
	if (TransferDataFromWindow())
	{
		CDlgInfoNote dlg(m_pDoc, ARBInfoType::Location, m_Location, this);
		if (wxID_OK == dlg.ShowModal())
		{
			m_Location = dlg.CurrentSelection();
			TransferDataToWindow();
			ListLocations();
			UpdateNotes(m_Location, true, false);
		}
	}
}


void CDlgTrial::OnClubNotes(wxCommandEvent& evt)
{
	long index = m_ctrlClubs->GetFirstSelected();
	if (0 <= index)
	{
		TransferDataFromWindow();
		ARBDogClubPtr pClub = GetClubData(index);
		CDlgInfoNote dlg(m_pDoc, ARBInfoType::Club, pClub->GetName(), this);
		if (wxID_OK == dlg.ShowModal())
		{
			if (pClub->GetName() != dlg.CurrentSelection())
			{
				pClub->SetName(dlg.CurrentSelection());
				ListClubs(&pClub);
			}
			else
				UpdateNotes(m_Location, false, true);
		}
	}
}


void CDlgTrial::OnClubNew(wxCommandEvent& evt)
{
	CDlgClub dlg(m_pDoc, m_Clubs, ARBDogClubPtr(), m_bShowCoSanction, this);
	if (wxID_OK == dlg.ShowModal())
	{
		ARBDogClubPtr club = dlg.AddClub(m_Clubs);
		if (club)
			ListClubs(&club);
		else
			wxMessageBox(_("IDS_ADD_CLUB_FAILED"), _("Agility Record Book"), wxOK | wxCENTRE | wxICON_WARNING);
	}
}


void CDlgTrial::OnClubEdit(wxCommandEvent& evt)
{
	EditClub();
}


void CDlgTrial::OnClubDelete(wxCommandEvent& evt)
{
	long index = m_ctrlClubs->GetFirstSelected();
	if (0 <= index)
	{
		ARBDogClubPtr pClub = GetClubData(index);
		// Before deleting the club, swap co-sanctioning if it exists.
		ARBDogClubPtr pCoSanction = m_Clubs.FindCoSanctioningClub(pClub);
		if (pCoSanction)
		{
			for (ARBDogRunList::iterator iterRun = m_pTrial->GetRuns().begin(); iterRun != m_pTrial->GetRuns().end();
				 ++iterRun)
			{
				ARBDogClubPtr pRunClub = (*iterRun)->GetClub();
				if (!pRunClub)
					continue;
				ARBDogClubPtr pClub2;
				if (m_Clubs.FindClub(pRunClub->GetName(), pRunClub->GetVenue(), &pClub2) && pClub2 == pClub)
				{
					m_bFixup = true;
					(*iterRun)->SetClub(pCoSanction);
				}
			}
		}
		m_Clubs.DeleteClub(pClub->GetName(), pClub->GetVenue());
		m_ctrlClubs->DeleteItem(index);
		m_ctrlClubs->Refresh();
	}
}


void CDlgTrial::OnClubMoveUp(wxCommandEvent& evt)
{
	int nItem = m_ctrlClubs->GetSelection(true);
	if (0 <= nItem)
	{
		int newIndex = nItem - 1;
		if (0 <= newIndex)
		{
			m_ctrlClubs->SwapEntries(nItem, newIndex);
			std::swap(m_Clubs.at(nItem), m_Clubs.at(newIndex));
		}
	}
}


void CDlgTrial::OnClubMoveDown(wxCommandEvent& evt)
{
	int nItem = m_ctrlClubs->GetSelection(true);
	if (0 <= nItem)
	{
		int newIndex = nItem + 1;
		if (newIndex < m_ctrlClubs->GetItemCount())
		{
			m_ctrlClubs->SwapEntries(nItem, newIndex);
			std::swap(m_Clubs.at(nItem), m_Clubs.at(newIndex));
		}
	}
}


void CDlgTrial::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	// Before the final commit, verify the trial still makes sense!
	// A user could have changed the venue of a club, this will make
	// any existing runs possibly invalid.
	m_bRunsDeleted = false;
	if (0 < m_pTrial->GetRuns().size())
	{
		std::set<wxString> oldVenues;
		std::set<wxString> newVenues;
		ARBDogClubList::iterator iterClub;
		for (iterClub = m_pTrial->GetClubs().begin(); iterClub != m_pTrial->GetClubs().end(); ++iterClub)
		{
			ARBDogClubPtr pClub = *iterClub;
			oldVenues.insert(pClub->GetVenue());
		}
		for (iterClub = m_Clubs.begin(); iterClub != m_Clubs.end(); ++iterClub)
		{
			ARBDogClubPtr pClub = *iterClub;
			newVenues.insert(pClub->GetVenue());
		}
		bool bAllThere = true;
		for (std::set<wxString>::iterator iterVenues = oldVenues.begin(); iterVenues != oldVenues.end(); ++iterVenues)
		{
			if (newVenues.end() == newVenues.find((*iterVenues)))
			{
				bAllThere = false;
				break;
			}
		}
		if (!bAllThere)
		{
			int nDelete = 0;
			for (ARBDogRunList::iterator iterRun = m_pTrial->GetRuns().begin(); iterRun != m_pTrial->GetRuns().end();
				 ++iterRun)
			{
				ARBDogRunPtr pRun = *iterRun;
				bool bFound = false;
				for (std::set<wxString>::iterator iterVenues = newVenues.begin(); iterVenues != newVenues.end();
					 ++iterVenues)
				{
					if (m_pDoc->Book().GetConfig().GetVenues().FindEvent(
							(*iterVenues),
							pRun->GetEvent(),
							pRun->GetDivision(),
							pRun->GetLevel(),
							pRun->GetDate()))
					{
						bFound = true;
						break;
					}
				}
				if (!bFound)
				{
					++nDelete;
				}
			}
			if (0 < nDelete)
			{
				auto msg = wxString::Format(_("IDS_CONFIG_DELETE_RUNS"), m_pTrial->GetRuns().size());
				if (wxYES
					!= wxMessageBox(msg, _("Agility Record Book"), wxYES_NO | wxNO_DEFAULT | wxCENTRE | wxICON_WARNING))
					return;
				m_pTrial->GetRuns().clear();
				m_bRunsDeleted = true;
			}
		}
	}

	ARBDogClubPtr pOldPrimary, pNewPrimary;
	for (auto pClub : m_pTrial->GetClubs())
	{
		if (pClub->GetPrimaryClub())
		{
			pOldPrimary = pClub->GetPrimaryClub();
			break;
		}
	}
	for (auto pClub : m_Clubs)
	{
		if (pClub->GetPrimaryClub())
		{
			pNewPrimary = pClub->GetPrimaryClub();
			break;
		}
	}
	if (pOldPrimary != pNewPrimary && pNewPrimary)
	{
		ARBDogClubPtr pCoSanction = m_Clubs.FindCoSanctioningClub(pNewPrimary);
		if (pCoSanction)
		{
			for (ARBDogRunList::iterator iterRun = m_pTrial->GetRuns().begin(); iterRun != m_pTrial->GetRuns().end();
				 ++iterRun)
			{
				ARBDogClubPtr pRunClub = (*iterRun)->GetClub();
				if (!pRunClub)
					continue;
				if (*pRunClub == *pCoSanction)
					(*iterRun)->SetClub(pNewPrimary);
			}
		}
	}

	m_pTrial->SetDefaultDate(m_dateStart);
	m_pTrial->SetLocation(m_Location);
	m_pTrial->SetNote(m_Notes);
	m_pTrial->SetVerified(m_Verified);
	m_pTrial->GetClubs() = m_Clubs;

	m_pDoc->Modify(true);

	EndDialog(wxID_OK);
}


void CDlgTrial::OnCancel(wxCommandEvent& evt)
{
	if (m_bFixup)
	{
		ARBDogClubPtr pPrimary;
		for (auto pClub : m_pTrial->GetClubs())
		{
			pPrimary = pClub->GetPrimaryClub();
			if (pPrimary)
				break;
		}
		if (pPrimary)
		{
			ARBDogClubPtr pCoSanction = m_pTrial->GetClubs().FindCoSanctioningClub(pPrimary);
			if (pCoSanction)
			{
				for (ARBDogRunList::iterator iterRun = m_pTrial->GetRuns().begin();
					 iterRun != m_pTrial->GetRuns().end();
					 ++iterRun)
				{
					ARBDogClubPtr pRunClub = (*iterRun)->GetClub();
					if (!pRunClub)
						continue;
					if (*pRunClub == *pCoSanction)
						(*iterRun)->SetClub(pPrimary);
				}
			}
		}
	}
	EndDialog(wxID_CANCEL);
}

} // namespace dconSoft
