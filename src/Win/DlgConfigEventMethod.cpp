/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgConfigEventMethod class
 * @author David Connet
 *
 * Revision History
 * 2017-12-31 Add support for using raw faults when determining title points.
 * 2017-11-30 Remove image list from placement list (not needed)
 * 2016-01-16 Cleaned up new/edit/delete buttons.
 * 2015-01-01 Changed pixels to dialog units.
 * 2012-06-16 Show time fault multiplier in point games.
 *            Fixed initial layout with speed points.
 * 2012-05-07 Fixed some comboboxes that should have been readonly.
 * 2012-02-16 Fix initial focus.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-08-12 Fix dates on save.
 * 2009-02-11 Ported to wxWidgets.
 * 2006-11-04 Created.
 */

#include "stdafx.h"
#include "DlgConfigEventMethod.h"

#include "AgilityBook.h"
#include "DlgConfigPlaceInfo.h"
#include "DlgConfigTitlePoints.h"
#include "DlgConfigure.h"
#include "DlgConfigureData.h"
#include "DlgName.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBConfigEvent.h"
#include "ARB/ARBConfigVenue.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/DlgPadding.h"
#include "LibARBWin/ListData.h"
#include "LibARBWin/ReportListCtrl.h"
#include "LibARBWin/Validators.h"
#include <wx/datectrl.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

class CDlgConfigureMethodDataDivision : public wxClientData
{
public:
	CDlgConfigureMethodDataDivision(ARBConfigDivisionPtr const& inDiv)
		: m_Div(inDiv)
	{
	}
	ARBConfigDivisionPtr m_Div;
};


class CDlgConfigureDataPlacement : public CListData
{
public:
	CDlgConfigureDataPlacement(short inPlace, double inVal)
		: m_Place(inPlace)
		, m_Value(inVal)
	{
	}
	wxString OnNeedText(long iColumn) const override;
	short Place() const
	{
		return m_Place;
	}
	void Place(short place)
	{
		m_Place = place;
	}
	double Value() const
	{
		return m_Value;
	}
	void Value(double val)
	{
		m_Value = val;
	}

protected:
	short m_Place;
	double m_Value;
};


wxString CDlgConfigureDataPlacement::OnNeedText(long iColumn) const
{
	wxString str;
	switch (iColumn)
	{
	default:
		break;
	case 0:
		str << m_Place;
		break;
	case 1:
		str << m_Value;
		break;
	}
	return str;
}


int wxCALLBACK ComparePlacement(CListDataPtr const& item1, CListDataPtr const& item2, SortInfo const* pSortInfo)
{
	CDlgConfigureDataPlacementPtr place1 = std::dynamic_pointer_cast<CDlgConfigureDataPlacement, CListData>(item1);
	CDlgConfigureDataPlacementPtr place2 = std::dynamic_pointer_cast<CDlgConfigureDataPlacement, CListData>(item2);
	return place1->Place() > place2->Place();
}

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigEventMethod dialog

CDlgConfigEventMethod::CDlgConfigEventMethod(
	ARBConfigVenuePtr const& inVenue,
	ARBConfigScoringPtr const& inScoring,
	wxWindow* pParent)
	: wxDialog()
	, m_pVenue(inVenue)
	, m_pScoring(inScoring)
	, m_PlaceInfo()
	, m_ctrlDivision(nullptr)
	, m_ctrlLevel(nullptr)
	, m_ctrlValidFrom(nullptr)
	, m_ctrlDateFrom(nullptr)
	, m_ctrlValidTo(nullptr)
	, m_ctrlDateTo(nullptr)
	, m_ctrlType(nullptr)
	, m_ctrlDropFractions(nullptr)
	, m_ctrlBonus(nullptr)
	, m_ctrlSuperQ(nullptr)
	, m_ctrlFEO(nullptr)
	, m_ctrlSpeedPts(nullptr)
	, m_ctrlMultiplyText(nullptr)
	, m_ctrlMultiply(nullptr)
	, m_ctrlPlacementText(nullptr)
	, m_ctrlPlacement(nullptr)
	, m_ctrlPlacementNew(nullptr)
	, m_ctrlPlacementEdit(nullptr)
	, m_ctrlPlacementDelete(nullptr)
	, m_ctrlTimeFaultsCleanQ(nullptr)
	, m_ctrlSubtractTimeFaults(nullptr)
	, m_ctrlTimeFaultsUnder(nullptr)
	, m_ctrlTimeFaultsOver(nullptr)
	, m_ctrlTimeFaultsTitlingPts(nullptr)
	, m_ctrlPointsOpeningText(nullptr)
	, m_ctrlPointsOpening(nullptr)
	, m_ctrlPointsClosingText(nullptr)
	, m_ctrlPointsClosing(nullptr)
	, m_dateFrom(m_pScoring->GetValidFrom())
	, m_dateTo(m_pScoring->GetValidTo())
	, m_DropFractions(m_pScoring->DropFractions())
	, m_Bonus(m_pScoring->HasBonusTitlePts())
	, m_SuperQ(m_pScoring->HasSuperQ())
	, m_FEO(m_pScoring->HasFEO())
	, m_SpeedPts(m_pScoring->HasSpeedPts())
	, m_Multiply(m_pScoring->TimeFaultMultiplier())
	, m_TimeFaultsCleanQ(m_pScoring->QsMustBeClean())
	, m_SubtractTimeFaults(m_pScoring->SubtractTimeFaultsFromScore())
	, m_TimeFaultsUnder(m_pScoring->ComputeTimeFaultsUnder())
	, m_TimeFaultsOver(m_pScoring->ComputeTimeFaultsOver())
	, m_TitlingPointsRawFaults(m_pScoring->ComputeTitlingPointsRawFaults())
	, m_OpeningPts(m_pScoring->GetRequiredOpeningPoints())
	, m_ClosingPts(m_pScoring->GetRequiredClosingPoints())
	, m_trace("CDlgConfigEventMethod")
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_CONFIG_EVENT_METHOD"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	// Copy the existing scorings.
	assert(m_pVenue);
	assert(m_pScoring);
	m_pScoring->GetPlaceInfo().Clone(m_PlaceInfo);

	// Controls (these are done first to control tab order)
	wxArrayString items;

	wxStaticText* textDiv
		= new wxStaticText(this, wxID_ANY, _("IDC_CONFIG_EVENT_DIVISION"), wxDefaultPosition, wxDefaultSize, 0);
	textDiv->Wrap(-1);

	m_ctrlDivision = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY);
	m_ctrlDivision->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgConfigEventMethod::OnSelchangeDivision, this);
	m_ctrlDivision->SetHelpText(_("HIDC_CONFIG_EVENT_DIVISION"));
	m_ctrlDivision->SetToolTip(_("HIDC_CONFIG_EVENT_DIVISION"));
	int idxCur = wxNOT_FOUND;
	items.clear();
	std::vector<wxClientData*> data;
	items.Add(_("IDS_ALL"));
	data.push_back(nullptr);
	if (m_pScoring->GetDivision() == WILDCARD_DIVISION)
		idxCur = static_cast<int>(items.size()) - 1;
	for (ARBConfigDivisionList::iterator iterDiv = m_pVenue->GetDivisions().begin();
		 iterDiv != m_pVenue->GetDivisions().end();
		 ++iterDiv)
	{
		ARBConfigDivisionPtr pDiv = (*iterDiv);
		items.Add(pDiv->GetName());
		data.push_back(new CDlgConfigureMethodDataDivision(pDiv));
		if (m_pScoring->GetDivision() == pDiv->GetName())
			idxCur = static_cast<int>(items.size()) - 1;
	}
	if (!items.empty())
	{
		m_ctrlDivision->Append(items, data.data());
		m_ctrlDivision->SetSelection(idxCur);
	}

	wxStaticText* textLevel
		= new wxStaticText(this, wxID_ANY, _("IDC_CONFIG_EVENT_LEVEL"), wxDefaultPosition, wxDefaultSize, 0);
	textLevel->Wrap(-1);

	m_ctrlLevel = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY);
	m_ctrlLevel->SetHelpText(_("HIDC_CONFIG_EVENT_LEVEL"));
	m_ctrlLevel->SetToolTip(_("HIDC_CONFIG_EVENT_LEVEL"));

	m_ctrlValidFrom = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CONFIG_EVENT_DATE_VALID_FROM"),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_RIGHT);
	m_ctrlValidFrom->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgConfigEventMethod::OnValidFrom, this);
	m_ctrlValidFrom->SetHelpText(_("HIDC_CONFIG_EVENT_DATE_VALID_FROM"));
	m_ctrlValidFrom->SetToolTip(_("HIDC_CONFIG_EVENT_DATE_VALID_FROM"));

	wxDateTime date(wxDateTime::Now());
	if (m_dateFrom.IsValid())
	{
		m_ctrlValidFrom->SetValue(true);
		m_dateFrom.GetDate(date);
	}
	else
	{
		m_ctrlValidFrom->SetValue(false);
	}
	m_ctrlDateFrom = new wxDatePickerCtrl(
		this,
		wxID_ANY,
		date,
		wxDefaultPosition,
		wxDefaultSize,
		wxDP_DROPDOWN | wxDP_SHOWCENTURY,
		CGenericValidator(&m_dateFrom));
	m_ctrlDateFrom->SetHelpText(_("HIDC_CONFIG_EVENT_DATE_START"));
	m_ctrlDateFrom->SetToolTip(_("HIDC_CONFIG_EVENT_DATE_START"));
	if (m_pScoring->GetValidFrom().IsValid())
		m_ctrlDateFrom->Enable(true);
	else
		m_ctrlDateFrom->Enable(false);

	m_ctrlValidTo = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CONFIG_EVENT_DATE_VALID_TO"),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_RIGHT);
	m_ctrlValidTo->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgConfigEventMethod::OnValidTo, this);
	m_ctrlValidTo->SetHelpText(_("HIDC_CONFIG_EVENT_DATE_VALID_TO"));
	m_ctrlValidTo->SetToolTip(_("HIDC_CONFIG_EVENT_DATE_VALID_TO"));

	date = wxDateTime::Now();
	if (m_dateTo.IsValid())
	{
		m_ctrlValidTo->SetValue(true);
		m_dateTo.GetDate(date);
	}
	else
	{
		m_ctrlValidTo->SetValue(false);
	}
	m_ctrlDateTo = new wxDatePickerCtrl(
		this,
		wxID_ANY,
		date,
		wxDefaultPosition,
		wxDefaultSize,
		wxDP_DROPDOWN | wxDP_SHOWCENTURY,
		CGenericValidator(&m_dateTo));
	m_ctrlDateTo->SetHelpText(_("HIDC_CONFIG_EVENT_DATE_END"));
	m_ctrlDateTo->SetToolTip(_("HIDC_CONFIG_EVENT_DATE_END"));
	if (m_pScoring->GetValidTo().IsValid())
		m_ctrlDateTo->Enable(true);
	else
		m_ctrlDateTo->Enable(false);

	wxStaticText* textStyle
		= new wxStaticText(this, wxID_ANY, _("IDC_CONFIG_EVENT_TYPE"), wxDefaultPosition, wxDefaultSize, 0);
	textStyle->Wrap(-1);

	m_ctrlType = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY);
	m_ctrlType->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgConfigEventMethod::OnSelchangeType, this);
	m_ctrlType->SetHelpText(_("HIDC_CONFIG_EVENT_TYPE"));
	m_ctrlType->SetToolTip(_("HIDC_CONFIG_EVENT_TYPE"));
	// If any additional types are added in ARBConfigScoring,
	// they'll have to be manually added here...
	constexpr ARBScoringStyle Styles[] = {
		ARBScoringStyle::FaultsThenTime,
		ARBScoringStyle::Faults100ThenTime,
		ARBScoringStyle::Faults200ThenTime,
		ARBScoringStyle::OCScoreThenTime,
		ARBScoringStyle::ScoreThenTime,
		ARBScoringStyle::TimePlusFaults,
		ARBScoringStyle::TimeNoPlaces,
		ARBScoringStyle::PassFail,
	};
	items.clear();
	std::vector<void*> dataVoid;
	for (auto const& style : Styles)
	{
		items.Add(ARBConfigScoring::GetScoringStyleStr(style));
		dataVoid.push_back(reinterpret_cast<void*>(style));
	}
	m_ctrlType->Append(items, dataVoid.data());

	m_ctrlDropFractions = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CONFIG_EVENT_DROP_FRACTIONS"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_DropFractions));
	m_ctrlDropFractions->SetHelpText(_("HIDC_CONFIG_EVENT_DROP_FRACTIONS"));
	m_ctrlDropFractions->SetToolTip(_("HIDC_CONFIG_EVENT_DROP_FRACTIONS"));

	m_ctrlBonus = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CONFIG_EVENT_BONUSTITLE"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_Bonus));
	m_ctrlBonus->SetHelpText(_("HIDC_CONFIG_EVENT_BONUSTITLE"));
	m_ctrlBonus->SetToolTip(_("HIDC_CONFIG_EVENT_BONUSTITLE"));

	m_ctrlSuperQ = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CONFIG_EVENT_SUPERQ"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_SuperQ));
	m_ctrlSuperQ->SetHelpText(_("HIDC_CONFIG_EVENT_SUPERQ"));
	m_ctrlSuperQ->SetToolTip(_("HIDC_CONFIG_EVENT_SUPERQ"));

	m_ctrlFEO = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CONFIG_EVENT_FEO"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_FEO));
	m_ctrlFEO->SetHelpText(_("HIDC_CONFIG_EVENT_FEO"));
	m_ctrlFEO->SetToolTip(_("HIDC_CONFIG_EVENT_FEO"));

	m_ctrlSpeedPts = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CONFIG_EVENT_SPEED"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_SpeedPts));
	m_ctrlSpeedPts->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgConfigEventMethod::OnSpeedPoints, this);
	m_ctrlSpeedPts->SetHelpText(_("HIDC_CONFIG_EVENT_SPEED"));
	m_ctrlSpeedPts->SetToolTip(_("HIDC_CONFIG_EVENT_SPEED"));

	m_ctrlMultiplyText
		= new wxStaticText(this, wxID_ANY, _("IDC_CONFIG_EVENT_TF_MULTIPLY"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlMultiplyText->Wrap(-1);

	m_ctrlMultiply = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 20), -1),
		0,
		CGenericValidator(&m_Multiply));
	m_ctrlMultiply->SetHelpText(_("HIDC_CONFIG_EVENT_TF_MULTIPLY"));
	m_ctrlMultiply->SetToolTip(_("HIDC_CONFIG_EVENT_TF_MULTIPLY"));

	m_ctrlPlacementText
		= new wxStaticText(this, wxID_ANY, _("IDC_CONFIG_EVENT_PLACEMENT"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlPlacementText->Wrap(-1);
	m_ctrlPlacementText->Show(m_SpeedPts);

	m_ctrlPlacement = new CReportListCtrl(
		this,
		wxDefaultPosition,
		wxDefaultSize,
		true,
		CReportListCtrl::SortHeader::None,
		true,
		false);
	m_ctrlPlacement->Bind(wxEVT_COMMAND_LIST_ITEM_SELECTED, &CDlgConfigEventMethod::OnItemchangedPlacement, this);
	m_ctrlPlacement->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &CDlgConfigEventMethod::OnItemActivatedPlacement, this);
	m_ctrlPlacement->Bind(wxEVT_KEY_DOWN, &CDlgConfigEventMethod::OnKeydownPlacement, this);
	m_ctrlPlacement->SetHelpText(_("HIDC_CONFIG_EVENT_PLACEMENT"));
	m_ctrlPlacement->SetToolTip(_("HIDC_CONFIG_EVENT_PLACEMENT"));
	m_ctrlPlacement->InsertColumn(0, _("IDS_COL_PLACE"));
	m_ctrlPlacement->InsertColumn(1, _("IDS_COL_MULTIPLIER"));
	for (ARBConfigPlaceInfoList::iterator iterPlace = m_PlaceInfo.begin(); iterPlace != m_PlaceInfo.end(); ++iterPlace)
	{
		CDlgConfigureDataPlacementPtr pData(
			std::make_shared<CDlgConfigureDataPlacement>((*iterPlace)->GetPlace(), (*iterPlace)->GetValue()));
		m_ctrlPlacement->InsertItem(pData);
	}
	m_ctrlPlacement->SortItems(ComparePlacement);
	m_ctrlPlacement->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
	m_ctrlPlacement->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
	m_ctrlPlacement->Show(m_SpeedPts);

	m_ctrlPlacementNew = new wxButton(
		this,
		wxID_ANY,
		_("IDC_CONFIG_EVENT_PLACEMENT_NEW"),
		wxDefaultPosition,
		wxDefaultSize,
		wxBU_EXACTFIT);
	m_ctrlPlacementNew->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigEventMethod::OnPlacementNew, this);
	m_ctrlPlacementNew->SetHelpText(_("HIDC_CONFIG_EVENT_PLACEMENT_NEW"));
	m_ctrlPlacementNew->SetToolTip(_("HIDC_CONFIG_EVENT_PLACEMENT_NEW"));
	m_ctrlPlacementNew->Show(m_SpeedPts);

	m_ctrlPlacementEdit = new wxButton(
		this,
		wxID_ANY,
		_("IDC_CONFIG_EVENT_PLACEMENT_EDIT"),
		wxDefaultPosition,
		wxDefaultSize,
		wxBU_EXACTFIT);
	m_ctrlPlacementEdit->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigEventMethod::OnPlacementEdit, this);
	m_ctrlPlacementEdit->SetHelpText(_("HIDC_CONFIG_EVENT_PLACEMENT_EDIT"));
	m_ctrlPlacementEdit->SetToolTip(_("HIDC_CONFIG_EVENT_PLACEMENT_EDIT"));
	m_ctrlPlacementEdit->Show(m_SpeedPts);

	m_ctrlPlacementDelete = new wxButton(
		this,
		wxID_ANY,
		_("IDC_CONFIG_EVENT_PLACEMENT_DELETE"),
		wxDefaultPosition,
		wxDefaultSize,
		wxBU_EXACTFIT);
	m_ctrlPlacementDelete->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigEventMethod::OnPlacementDelete, this);
	m_ctrlPlacementDelete->SetHelpText(_("HIDC_CONFIG_EVENT_PLACEMENT_DELETE"));
	m_ctrlPlacementDelete->SetToolTip(_("HIDC_CONFIG_EVENT_PLACEMENT_DELETE"));
	m_ctrlPlacementDelete->Show(m_SpeedPts);

	m_ctrlTimeFaultsCleanQ = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CONFIG_EVENT_TIME_FAULTS_CLEANQ"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_TimeFaultsCleanQ));
	m_ctrlTimeFaultsCleanQ->SetHelpText(_("HIDC_CONFIG_EVENT_TIME_FAULTS_CLEANQ"));
	m_ctrlTimeFaultsCleanQ->SetToolTip(_("HIDC_CONFIG_EVENT_TIME_FAULTS_CLEANQ"));

	m_ctrlSubtractTimeFaults = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CONFIG_EVENT_SUBTRACT_TIME_FAULTS"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_SubtractTimeFaults));
	m_ctrlSubtractTimeFaults->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgConfigEventMethod::OnComputeTimeFaults, this);
	m_ctrlSubtractTimeFaults->SetHelpText(_("HIDC_CONFIG_EVENT_SUBTRACT_TIME_FAULTS"));
	m_ctrlSubtractTimeFaults->SetToolTip(_("HIDC_CONFIG_EVENT_SUBTRACT_TIME_FAULTS"));

	m_ctrlTimeFaultsUnder = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CONFIG_EVENT_TIME_FAULTS_UNDER"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_TimeFaultsUnder));
	m_ctrlTimeFaultsUnder->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgConfigEventMethod::OnComputeTimeFaults, this);
	m_ctrlTimeFaultsUnder->SetHelpText(_("HIDC_CONFIG_EVENT_TIME_FAULTS_UNDER"));
	m_ctrlTimeFaultsUnder->SetToolTip(_("HIDC_CONFIG_EVENT_TIME_FAULTS_UNDER"));

	m_ctrlTimeFaultsOver = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CONFIG_EVENT_TIME_FAULTS_OVER"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_TimeFaultsOver));
	m_ctrlTimeFaultsOver->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgConfigEventMethod::OnComputeTimeFaults, this);
	m_ctrlTimeFaultsOver->SetHelpText(_("HIDC_CONFIG_EVENT_TIME_FAULTS_OVER"));
	m_ctrlTimeFaultsOver->SetToolTip(_("HIDC_CONFIG_EVENT_TIME_FAULTS_OVER"));

	m_ctrlTimeFaultsTitlingPts = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CONFIG_EVENT_TIME_FAULTS_TITLING_PTS"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_TitlingPointsRawFaults));
	m_ctrlTimeFaultsTitlingPts->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgConfigEventMethod::OnComputeTimeFaults, this);
	m_ctrlTimeFaultsTitlingPts->SetHelpText(_("HIDC_CONFIG_EVENT_TIME_FAULTS_TITLING_PTS"));
	m_ctrlTimeFaultsTitlingPts->SetToolTip(_("HIDC_CONFIG_EVENT_TIME_FAULTS_TITLING_PTS"));

	m_ctrlPointsOpeningText
		= new wxStaticText(this, wxID_ANY, _("IDC_CONFIG_EVENT_OPENING_PTS"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlPointsOpeningText->Wrap(-1);

	m_ctrlPointsOpening = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 20), -1),
		0,
		CGenericValidator(&m_OpeningPts));
	m_ctrlPointsOpening->SetHelpText(_("HIDC_CONFIG_EVENT_OPENING_PTS"));
	m_ctrlPointsOpening->SetToolTip(_("HIDC_CONFIG_EVENT_OPENING_PTS"));

	m_ctrlPointsClosingText
		= new wxStaticText(this, wxID_ANY, _("IDC_CONFIG_EVENT_CLOSING_PTS"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlPointsClosingText->Wrap(-1);

	m_ctrlPointsClosing = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 20), -1),
		0,
		CGenericValidator(&m_ClosingPts));
	m_ctrlPointsClosing->SetHelpText(_("HIDC_CONFIG_EVENT_CLOSING_PTS"));
	m_ctrlPointsClosing->SetToolTip(_("HIDC_CONFIG_EVENT_CLOSING_PTS"));

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerDivDates = new wxBoxSizer(wxHORIZONTAL);

	wxFlexGridSizer* sizerDiv = new wxFlexGridSizer(2, 2, padding.Controls(), padding.Inner()); // rows/cols/vgap/hgap
	sizerDiv->SetFlexibleDirection(wxBOTH);
	sizerDiv->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerDiv->Add(textDiv, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizerDiv->Add(m_ctrlDivision, 1, wxALIGN_CENTER_VERTICAL);
	sizerDiv->Add(textLevel, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizerDiv->Add(m_ctrlLevel, 1, wxALIGN_CENTER_VERTICAL);

	sizerDivDates->Add(sizerDiv, 0, wxEXPAND | wxRIGHT, padding.Controls());

	wxFlexGridSizer* sizerDates = new wxFlexGridSizer(2, 2, padding.Controls(), padding.Controls());
	sizerDates->SetFlexibleDirection(wxBOTH);
	sizerDates->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerDates->Add(m_ctrlValidFrom, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizerDates->Add(m_ctrlDateFrom, 0, wxALIGN_CENTER_VERTICAL);
	sizerDates->Add(m_ctrlValidTo, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizerDates->Add(m_ctrlDateTo, 0, wxALIGN_CENTER_VERTICAL);

	sizerDivDates->Add(sizerDates, 1, wxEXPAND);

	bSizer->Add(sizerDivDates, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerStyle = new wxBoxSizer(wxHORIZONTAL);
	sizerStyle->Add(textStyle, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerStyle->Add(m_ctrlType, 0, wxALIGN_CENTER_VERTICAL);
	bSizer->Add(sizerStyle, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerOptions = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerCol1 = new wxBoxSizer(wxVERTICAL);
	sizerCol1->Add(m_ctrlDropFractions, 0, wxBOTTOM, padding.Controls());
	sizerCol1->Add(m_ctrlBonus, 0, wxBOTTOM, padding.Controls());
	sizerCol1->Add(m_ctrlSuperQ, 0, wxBOTTOM, padding.Controls());
	sizerCol1->Add(m_ctrlFEO, 0, wxBOTTOM, padding.Controls());
	sizerCol1->Add(m_ctrlSpeedPts, 0, wxBOTTOM, padding.Controls());

	wxBoxSizer* sizerMultiplier = new wxBoxSizer(wxHORIZONTAL);
	sizerMultiplier->Add(m_ctrlMultiplyText, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerMultiplier->Add(m_ctrlMultiply, 0, wxALIGN_CENTER_VERTICAL);

	sizerCol1->Add(sizerMultiplier, 0, wxBOTTOM, padding.Controls());
	sizerCol1->Add(m_ctrlPlacementText);

	wxBoxSizer* sizerPlacements = new wxBoxSizer(wxHORIZONTAL);

	sizerPlacements->Add(m_ctrlPlacement, 1, wxEXPAND | wxRIGHT, padding.Controls());

	wxBoxSizer* sizerBtns = new wxBoxSizer(wxVERTICAL);
	sizerBtns->Add(m_ctrlPlacementNew, 0, wxEXPAND | wxBOTTOM, padding.Inner());
	sizerBtns->Add(m_ctrlPlacementEdit, 0, wxEXPAND | wxBOTTOM, padding.Inner());
	sizerBtns->Add(m_ctrlPlacementDelete, 0, wxEXPAND);

	sizerPlacements->Add(sizerBtns, 0, wxEXPAND);

	sizerCol1->Add(sizerPlacements, 0, wxEXPAND);

	sizerOptions->Add(sizerCol1, 0, wxEXPAND | wxRIGHT, padding.Controls());

	wxBoxSizer* sizerCol2 = new wxBoxSizer(wxVERTICAL);
	sizerCol2->Add(m_ctrlTimeFaultsCleanQ, 0, wxBOTTOM, padding.Controls());
	sizerCol2->Add(m_ctrlSubtractTimeFaults, 0, wxBOTTOM, padding.Controls());
	sizerCol2->Add(m_ctrlTimeFaultsUnder, 0, wxBOTTOM, padding.Controls());
	sizerCol2->Add(m_ctrlTimeFaultsOver, 0, wxBOTTOM, padding.Controls());
	sizerCol2->Add(m_ctrlTimeFaultsTitlingPts);

	wxFlexGridSizer* sizerPoints = new wxFlexGridSizer(2, 2, padding.Controls(), padding.Inner());
	sizerPoints->SetFlexibleDirection(wxBOTH);
	sizerPoints->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerPoints->Add(m_ctrlPointsOpeningText, 0, wxALIGN_CENTER_VERTICAL);
	sizerPoints->Add(m_ctrlPointsOpening, 0, wxALIGN_CENTER_VERTICAL);
	sizerPoints->Add(m_ctrlPointsClosingText, 0, wxALIGN_CENTER_VERTICAL);
	sizerPoints->Add(m_ctrlPointsClosing, 0, wxALIGN_CENTER_VERTICAL);
	sizerCol2->Add(sizerPoints, 0, wxEXPAND);

	sizerOptions->Add(sizerCol2, 0, wxEXPAND);

	bSizer->Add(sizerOptions, 0, wxEXPAND | wxALL, padding.Controls());

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.ButtonSizer());
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigEventMethod::OnOk, this, wxID_OK);

	FillLevelList();
	m_ctrlType->SetSelection(-1);
	for (unsigned int idxType = 0; idxType < m_ctrlType->GetCount(); ++idxType)
	{
		ARBScoringStyle style = GetType(idxType);
		if (m_pScoring->GetScoringStyle() == style)
		{
			m_ctrlType->SetSelection(idxType);
			break;
		}
	}

	UpdateControls();

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	IMPLEMENT_ON_INIT(m_ctrlDivision)
}


ARBScoringStyle CDlgConfigEventMethod::GetType(int index) const
{
	return static_cast<ARBScoringStyle>(reinterpret_cast<size_t>(m_ctrlType->GetClientData(index)));
}


CDlgConfigureDataPlacementPtr CDlgConfigEventMethod::GetPlacementData(int index) const
{
	return std::dynamic_pointer_cast<CDlgConfigureDataPlacement, CListData>(m_ctrlPlacement->GetData(index));
}


void CDlgConfigEventMethod::UpdateButtons()
{
	bool bEdit = false;
	bool bDelete = false;
	if (m_SpeedPts && 0 <= m_ctrlPlacement->GetFirstSelected())
		bEdit = bDelete = true;
	m_ctrlPlacementEdit->Enable(bEdit);
	m_ctrlPlacementDelete->Enable(bDelete);
	if (m_ctrlMultiply->IsShown())
	{
		bool bEnable = true;
		if (m_ctrlSubtractTimeFaults->IsShown()
			&& (m_ctrlTimeFaultsUnder->IsShown() || m_ctrlTimeFaultsOver->IsShown()))
		{
			bEnable = (m_SubtractTimeFaults && (m_TimeFaultsUnder || m_TimeFaultsOver));
		}
		m_ctrlMultiply->Enable(bEnable);
	}
	if (m_ctrlTimeFaultsUnder->IsShown() || m_ctrlTimeFaultsOver->IsShown())
	{
		bool bEnableOverUnder = true;
		if (m_ctrlSubtractTimeFaults->IsShown())
		{
			bEnableOverUnder = m_SubtractTimeFaults;
		}
		m_ctrlTimeFaultsUnder->Enable(bEnableOverUnder);
		m_ctrlTimeFaultsOver->Enable(bEnableOverUnder);
	}
}


void CDlgConfigEventMethod::UpdateControls()
{
	int idxType = m_ctrlType->GetSelection();
	if (wxNOT_FOUND == idxType)
	{
		m_ctrlSuperQ->Show(true);
		// Note: m_ctrlFEO is never hidden
		m_ctrlSpeedPts->Show(true);
		m_ctrlPointsOpeningText->Show(false);
		m_ctrlPointsOpening->Show(false);
		m_ctrlPointsClosingText->Show(false);
		m_ctrlPointsClosing->Show(false);
		m_ctrlDropFractions->Show(false);
		m_ctrlTimeFaultsCleanQ->Show(false);
		m_ctrlSubtractTimeFaults->Show(false);
		m_ctrlTimeFaultsUnder->Show(false);
		m_ctrlTimeFaultsOver->Show(false);
		m_ctrlTimeFaultsTitlingPts->Show(false);
		m_ctrlMultiplyText->Show(false);
		m_ctrlMultiply->Show(false);
	}
	else
	{
		switch (GetType(idxType))
		{
		case ARBScoringStyle::Unknown:
			m_ctrlSuperQ->Show(true);
			m_ctrlSpeedPts->Show(true);
			m_ctrlPointsOpeningText->Show(false);
			m_ctrlPointsOpening->Show(false);
			m_ctrlPointsClosingText->Show(false);
			m_ctrlPointsClosing->Show(false);
			m_ctrlDropFractions->Show(false);
			m_ctrlTimeFaultsCleanQ->Show(false);
			m_ctrlSubtractTimeFaults->Show(false);
			m_ctrlTimeFaultsUnder->Show(false);
			m_ctrlTimeFaultsOver->Show(false);
			m_ctrlTimeFaultsTitlingPts->Show(false);
			m_ctrlMultiplyText->Show(false);
			m_ctrlMultiply->Show(false);
			break;
		case ARBScoringStyle::FaultsThenTime:
		case ARBScoringStyle::Faults100ThenTime:
		case ARBScoringStyle::Faults200ThenTime:
			m_ctrlSuperQ->Show(true);
			m_ctrlSpeedPts->Show(true);
			m_ctrlPointsOpeningText->Show(false);
			m_ctrlPointsOpening->Show(false);
			m_ctrlPointsClosingText->Show(false);
			m_ctrlPointsClosing->Show(false);
			m_ctrlDropFractions->Show(true);
			m_ctrlTimeFaultsCleanQ->Show(false);
			m_ctrlSubtractTimeFaults->Show(false);
			m_ctrlTimeFaultsUnder->Show(false);
			m_ctrlTimeFaultsOver->Show(false);
			m_ctrlTimeFaultsTitlingPts->Show(false);
			m_ctrlMultiplyText->Show(true);
			m_ctrlMultiply->Show(true);
			break;
		case ARBScoringStyle::OCScoreThenTime:
			m_ctrlSuperQ->Show(true);
			m_ctrlSpeedPts->Show(true);
			m_ctrlPointsOpeningText->Show(true);
			m_ctrlPointsOpening->Show(true);
			m_ctrlPointsClosingText->Show(true);
			m_ctrlPointsClosing->Show(true);
			m_ctrlDropFractions->Show(true);
			m_ctrlTimeFaultsCleanQ->Show(false);
			m_ctrlSubtractTimeFaults->Show(true);
			m_ctrlTimeFaultsUnder->Show(true);
			m_ctrlTimeFaultsOver->Show(true);
			m_ctrlTimeFaultsTitlingPts->Show(false);
			m_ctrlMultiplyText->Show(true);
			m_ctrlMultiply->Show(true);
			m_ctrlPointsOpeningText->SetLabel(_("IDC_CONFIG_EVENT_OPENING_PTS"));
			break;
		case ARBScoringStyle::ScoreThenTime:
			m_ctrlSuperQ->Show(true);
			m_ctrlSpeedPts->Show(true);
			m_ctrlPointsOpeningText->Show(true);
			m_ctrlPointsOpening->Show(true);
			m_ctrlPointsClosingText->Show(false);
			m_ctrlPointsClosing->Show(false);
			m_ctrlDropFractions->Show(true);
			m_ctrlTimeFaultsCleanQ->Show(false);
			m_ctrlSubtractTimeFaults->Show(true);
			m_ctrlTimeFaultsUnder->Show(true);
			m_ctrlTimeFaultsOver->Show(true);
			m_ctrlTimeFaultsTitlingPts->Show(false);
			m_ctrlMultiplyText->Show(true);
			m_ctrlMultiply->Show(true);
			m_ctrlPointsOpeningText->SetLabel(_("IDS_SCORING_REQUIRED_POINTS"));
			break;
		case ARBScoringStyle::TimePlusFaults:
			m_ctrlSuperQ->Show(true);
			m_ctrlSpeedPts->Show(true);
			m_ctrlPointsOpeningText->Show(false);
			m_ctrlPointsOpening->Show(false);
			m_ctrlPointsClosingText->Show(false);
			m_ctrlPointsClosing->Show(false);
			m_ctrlDropFractions->Show(true);
			m_ctrlTimeFaultsCleanQ->Show(true);
			m_ctrlSubtractTimeFaults->Show(false);
			m_ctrlTimeFaultsUnder->Show(true);
			m_ctrlTimeFaultsOver->Show(true);
			m_ctrlTimeFaultsTitlingPts->Show(true);
			m_ctrlMultiplyText->Show(true);
			m_ctrlMultiply->Show(true);
			break;
		case ARBScoringStyle::TimeNoPlaces:
			m_ctrlSuperQ->Show(true);
			m_ctrlSpeedPts->Show(true);
			m_ctrlPointsOpeningText->Show(false);
			m_ctrlPointsOpening->Show(false);
			m_ctrlPointsClosingText->Show(false);
			m_ctrlPointsClosing->Show(false);
			m_ctrlDropFractions->Show(false);
			m_ctrlTimeFaultsCleanQ->Show(false);
			m_ctrlSubtractTimeFaults->Show(false);
			m_ctrlTimeFaultsUnder->Show(false);
			m_ctrlTimeFaultsOver->Show(false);
			m_ctrlTimeFaultsTitlingPts->Show(false);
			m_ctrlMultiplyText->Show(false);
			m_ctrlMultiply->Show(false);
			break;
		case ARBScoringStyle::PassFail:
			m_ctrlSuperQ->Show(false);
			m_ctrlSpeedPts->Show(false);
			m_ctrlPointsOpeningText->Show(false);
			m_ctrlPointsOpening->Show(false);
			m_ctrlPointsClosingText->Show(false);
			m_ctrlPointsClosing->Show(false);
			m_ctrlDropFractions->Show(false);
			m_ctrlTimeFaultsCleanQ->Show(false);
			m_ctrlSubtractTimeFaults->Show(false);
			m_ctrlTimeFaultsUnder->Show(false);
			m_ctrlTimeFaultsOver->Show(false);
			m_ctrlTimeFaultsTitlingPts->Show(false);
			m_ctrlMultiplyText->Show(false);
			m_ctrlMultiply->Show(false);
			break;
		}
	}
	UpdateButtons();
}


void CDlgConfigEventMethod::FillLevelList()
{
	int idxCur = wxNOT_FOUND;
	wxArrayString items;
	m_ctrlLevel->Clear();
	items.Add(_("IDS_ALL"));
	if (m_pScoring->GetLevel() == WILDCARD_LEVEL)
		idxCur = static_cast<int>(items.size()) - 1;

	int idxDiv = m_ctrlDivision->GetSelection();
	if (0 == idxDiv) // All
	{
		for (auto const& pDiv : m_pVenue->GetDivisions())
		{
			for (auto const& pLevel : pDiv->GetLevels())
			{
				wxString wxName(pLevel->GetName());
				if (wxNOT_FOUND == m_ctrlLevel->FindString(wxName, true))
				{
					items.Add(wxName);
					if (m_pScoring->GetLevel() == pLevel->GetName())
						idxCur = static_cast<int>(items.size()) - 1;
				}
			}
		}
	}
	else if (0 < idxDiv)
	{
		CDlgConfigureMethodDataDivision* pDiv
			= dynamic_cast<CDlgConfigureMethodDataDivision*>(m_ctrlDivision->GetClientObject(idxDiv));
		for (ARBConfigLevelList::iterator iterLevel = pDiv->m_Div->GetLevels().begin();
			 iterLevel != pDiv->m_Div->GetLevels().end();
			 ++iterLevel)
		{
			items.Add((*iterLevel)->GetName());
			if (m_pScoring->GetLevel() == (*iterLevel)->GetName())
				idxCur = static_cast<int>(items.size()) - 1;
		}
	}
	if (!items.empty())
	{
		m_ctrlLevel->Append(items);
		m_ctrlLevel->SetSelection(idxCur);
	}
}


void CDlgConfigEventMethod::DoPlacementEdit()
{
	int idx = m_ctrlPlacement->GetFirstSelected();
	if (0 > idx)
		return;
	std::vector<short> placements;
	int count = m_ctrlPlacement->GetItemCount();
	for (int index = 0; index < count; ++index)
	{
		CDlgConfigureDataPlacementPtr pData = GetPlacementData(index);
		placements.push_back(pData->Place());
	}
	CDlgConfigureDataPlacementPtr pData = GetPlacementData(idx);
	CDlgConfigPlaceInfo dlg(placements, pData->Place(), pData->Value(), this);
	if (wxID_OK == dlg.ShowModal())
	{
		pData->Place(dlg.GetPlace());
		pData->Value(dlg.GetValue());
		m_ctrlPlacement->SortItems(ComparePlacement);
		m_ctrlPlacement->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
		m_ctrlPlacement->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
		UpdateButtons();
	}
}


void CDlgConfigEventMethod::OnSelchangeDivision(wxCommandEvent& evt)
{
	FillLevelList();
}


void CDlgConfigEventMethod::OnValidFrom(wxCommandEvent& evt)
{
	m_ctrlDateFrom->Enable(m_ctrlValidFrom->GetValue());
}


void CDlgConfigEventMethod::OnValidTo(wxCommandEvent& evt)
{
	m_ctrlDateTo->Enable(m_ctrlValidTo->GetValue());
}


void CDlgConfigEventMethod::OnSelchangeType(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	UpdateControls();
	Layout();
	GetSizer()->Fit(this);
}


void CDlgConfigEventMethod::OnSpeedPoints(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	m_ctrlPlacementText->Show(m_SpeedPts);
	m_ctrlPlacement->Show(m_SpeedPts);
	m_ctrlPlacementNew->Show(m_SpeedPts);
	m_ctrlPlacementEdit->Show(m_SpeedPts);
	m_ctrlPlacementDelete->Show(m_SpeedPts);
	Layout();
	GetSizer()->Fit(this);
}


void CDlgConfigEventMethod::OnComputeTimeFaults(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	UpdateButtons();
}


void CDlgConfigEventMethod::OnItemchangedPlacement(wxListEvent& evt)
{
	UpdateButtons();
}


void CDlgConfigEventMethod::OnItemActivatedPlacement(wxListEvent& evt)
{
	DoPlacementEdit();
}


void CDlgConfigEventMethod::OnKeydownPlacement(wxKeyEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		evt.Skip();
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
		DoPlacementEdit();
		break;
	}
}


void CDlgConfigEventMethod::OnPlacementNew(wxCommandEvent& evt)
{
	std::vector<short> placements;
	int count = m_ctrlPlacement->GetItemCount();
	for (int index = 0; index < count; ++index)
	{
		CDlgConfigureDataPlacementPtr pData = GetPlacementData(index);
		placements.push_back(pData->Place());
	}
	CDlgConfigPlaceInfo dlg(placements, -1, 0.0, this);
	if (wxID_OK == dlg.ShowModal())
	{
		CDlgConfigureDataPlacementPtr pData(
			std::make_shared<CDlgConfigureDataPlacement>(dlg.GetPlace(), dlg.GetValue()));
		m_ctrlPlacement->InsertItem(pData);
		m_ctrlPlacement->SortItems(ComparePlacement);
		m_ctrlPlacement->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
		m_ctrlPlacement->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
		UpdateButtons();
	}
}


void CDlgConfigEventMethod::OnPlacementEdit(wxCommandEvent& evt)
{
	DoPlacementEdit();
}


void CDlgConfigEventMethod::OnPlacementDelete(wxCommandEvent& evt)
{
	int idx = m_ctrlPlacement->GetFirstSelected();
	if (0 > idx)
		return;
	m_ctrlPlacement->DeleteItem(idx);
	UpdateButtons();
}


void CDlgConfigEventMethod::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	// Verify we have selections
	int idxDiv = m_ctrlDivision->GetSelection();
	if (wxNOT_FOUND == idxDiv)
	{
		wxMessageBox(_("IDS_SELECT_DIVISION"), _("Agility Record Book"), wxOK | wxCENTRE | wxICON_WARNING);
		m_ctrlDivision->SetFocus();
		return;
	}
	int idxLevel = m_ctrlLevel->GetSelection();
	if (wxNOT_FOUND == idxLevel)
	{
		wxMessageBox(_("IDS_SELECT_LEVEL"), _("Agility Record Book"), wxOK | wxCENTRE | wxICON_WARNING);
		m_ctrlLevel->SetFocus();
		return;
	}
	int idxType = m_ctrlType->GetSelection();
	if (wxNOT_FOUND == idxType)
	{
		wxMessageBox(_("IDS_SELECT_STYLE"), _("Agility Record Book"), wxOK | wxCENTRE | wxICON_WARNING);
		m_ctrlType->SetFocus();
		return;
	}

	if (!m_ctrlValidFrom->GetValue())
		m_dateFrom.clear();
	if (!m_ctrlValidTo->GetValue())
		m_dateTo.clear();

	// Validate that from-to dates are okay.
	if (m_dateFrom.IsValid() && m_dateTo.IsValid() && m_dateFrom > m_dateTo)
	{
		wxMessageBox(_("IDS_CONFIGEVENT_DATEORDER"), _("Agility Record Book"), wxOK | wxCENTRE | wxICON_WARNING);
		m_ctrlDateTo->SetFocus();
		return;
	}

	// Validate placements.
	if (m_SpeedPts)
	{
		if (1 == m_ctrlPlacement->GetItemCount())
		{
			CDlgConfigureDataPlacementPtr pData = GetPlacementData(0);
			if (0 == pData->Place())
			{
				wxMessageBox(
					_("IDS_CONFIGEVENT_PLACEMENT0"),
					_("Agility Record Book"),
					wxOK | wxCENTRE | wxICON_WARNING);
				m_ctrlPlacementNew->SetFocus();
				return;
			}
		}
	}

	// Save it.
	wxString str;
	m_pScoring->SetValidFrom(m_dateFrom);
	m_pScoring->SetValidTo(m_dateTo);
	if (0 == idxDiv)
		str = WILDCARD_DIVISION;
	else
		str = m_ctrlDivision->GetString(idxDiv);
	m_pScoring->SetDivision(str);
	if (0 == idxLevel)
		str = WILDCARD_LEVEL;
	else
		str = m_ctrlLevel->GetString(idxLevel);
	m_pScoring->SetLevel(str);

	m_pScoring->SetDropFractions(m_DropFractions);
	m_pScoring->SetHasBonusTitlePts(m_Bonus);
	m_pScoring->SetHasSuperQ(m_SuperQ);
	m_pScoring->SetHasFEO(m_FEO);
	m_pScoring->SetHasSpeedPts(m_SpeedPts);
	if (m_SpeedPts)
	{
		m_PlaceInfo.clear();
		int count = m_ctrlPlacement->GetItemCount();
		for (int index = 0; index < count; ++index)
		{
			CDlgConfigureDataPlacementPtr pData = GetPlacementData(index);
			m_PlaceInfo.AddPlaceInfo(pData->Place(), pData->Value(), true);
		}
		m_pScoring->GetPlaceInfo() = m_PlaceInfo;
	}

	ARBScoringStyle style = GetType(idxType);
	m_pScoring->SetScoringStyle(style);
	switch (style)
	{
	case ARBScoringStyle::Unknown:
		break;
	case ARBScoringStyle::FaultsThenTime:
	case ARBScoringStyle::Faults100ThenTime:
	case ARBScoringStyle::Faults200ThenTime:
		m_pScoring->SetTimeFaultMultiplier(m_Multiply);
		break;
	case ARBScoringStyle::OCScoreThenTime:
		m_pScoring->SetRequiredOpeningPoints(m_OpeningPts);
		m_pScoring->SetRequiredClosingPoints(m_ClosingPts);
		m_pScoring->SetSubtractTimeFaultsFromScore(m_SubtractTimeFaults);
		m_pScoring->SetComputeTimeFaultsUnder(m_TimeFaultsUnder);
		m_pScoring->SetComputeTimeFaultsOver(m_TimeFaultsOver);
		m_pScoring->SetTimeFaultMultiplier(m_Multiply);
		break;
	case ARBScoringStyle::ScoreThenTime:
		m_pScoring->SetRequiredOpeningPoints(m_OpeningPts);
		m_pScoring->SetSubtractTimeFaultsFromScore(m_SubtractTimeFaults);
		m_pScoring->SetComputeTimeFaultsUnder(m_TimeFaultsUnder);
		m_pScoring->SetComputeTimeFaultsOver(m_TimeFaultsOver);
		m_pScoring->SetTimeFaultMultiplier(m_Multiply);
		break;
	case ARBScoringStyle::TimePlusFaults:
		m_pScoring->SetQsMustBeClean(m_TimeFaultsCleanQ);
		m_pScoring->SetComputeTimeFaultsUnder(m_TimeFaultsUnder);
		m_pScoring->SetComputeTimeFaultsOver(m_TimeFaultsOver);
		m_pScoring->SetComputeTitlingPointsRawFaults(m_TitlingPointsRawFaults);
		m_pScoring->SetTimeFaultMultiplier(m_Multiply);
		break;
	case ARBScoringStyle::TimeNoPlaces:
		break;
	case ARBScoringStyle::PassFail:
		break;
	}

	EndDialog(wxID_OK);
}

} // namespace dconSoft
