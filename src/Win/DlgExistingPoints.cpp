/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgExistingPoints class
 * @author David Connet
 *
 * Revision History
 * 2016-04-29 Separate lifetime points from title (run) points.
 * 2014-12-31 Changed pixels to dialog units.
 * 2012-05-07 Fixed some comboboxes that should have been readonly.
 * 2012-02-16 Fix initial focus.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-07-14 Fixed group box creation order.
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-07-31 Changed how QQs are done.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2005-01-01 Renamed MachPts to SpeedPts.
 * 2004-02-03 Created
 */

#include "stdafx.h"
#include "DlgExistingPoints.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"

#include "ARB/ARBConfig.h"
#include "ARB/ARBDogExistingPoints.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ARBWinUtilities.h"
#include "LibARBWin/Validators.h"
#include "LibARBWin/Widgets.h"
#include <wx/datectrl.h>
#include <wx/dateevt.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

class CDlgPointsVenueData : public wxClientData
{
public:
	CDlgPointsVenueData(ARBConfigVenuePtr const& inVenue)
		: m_Venue(inVenue)
	{
	}
	ARBConfigVenuePtr m_Venue;
};

class CDlgPointsDivisionData : public wxClientData
{
public:
	CDlgPointsDivisionData(ARBConfigDivisionPtr const& inDiv)
		: m_Div(inDiv)
	{
	}
	ARBConfigDivisionPtr m_Div;
};

class CDlgPointsLevelData : public wxClientData
{
public:
	CDlgPointsLevelData(ARBConfigLevelPtr const& inLevel)
		: m_Level(inLevel)
		, m_SubLevel()
	{
	}
	CDlgPointsLevelData(ARBConfigLevelPtr const& inLevel, ARBConfigSubLevelPtr const& inSubLevel)
		: m_Level(inLevel)
		, m_SubLevel(inSubLevel)
	{
	}
	ARBConfigLevelPtr m_Level;
	ARBConfigSubLevelPtr m_SubLevel;
};

class CDlgPointsEventData : public wxClientData
{
public:
	CDlgPointsEventData(ARBConfigEventPtr const& inEvt)
		: m_Event(inEvt)
	{
	}
	ARBConfigEventPtr m_Event;
};

class CDlgPointsTypeNameData : public wxClientData
{
public:
	CDlgPointsTypeNameData(ARBConfigOtherPointsPtr const& inOther)
		: m_Other(inOther)
		, m_Lifetime(nullptr)
	{
	}
	CDlgPointsTypeNameData(ARBConfigLifetimeNamePtr const& inOther)
		: m_Other(nullptr)
		, m_Lifetime(inOther)
	{
	}
	ARBConfigOtherPointsPtr m_Other;
	ARBConfigLifetimeNamePtr m_Lifetime;
};

class CDlgPointsMultiQData : public wxClientData
{
public:
	CDlgPointsMultiQData(ARBConfigMultiQPtr const& inMultiQ)
		: m_MultiQ(inMultiQ)
	{
	}
	ARBConfigMultiQPtr m_MultiQ;
};

/////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(CDlgExistingPoints, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgExistingPoints::OnOk)
wxEND_EVENT_TABLE()


// Controls (all are readonly, except subname):
// ARBDogExistingPoints::eOtherPoints:
//  Venue  Division  Level  Event          OtherPts
// ARBDogExistingPoints::eLifetime:
//  Venue  Division  Level  Event          OtherPts
// ARBDogExistingPoints::eTitle:
//  Venue  Division  Level  Event  SubName
// ARBDogExistingPoints::eSQ:
//  Venue  Division  Level  Event
// ARBDogExistingPoints::eSpeed:
//  Venue  Division  Level
// ARBDogExistingPoints::eMQ:
//  Venue  MultiQ

// If inExistingPoints is NULL, we're creating a new entry. Otherwise, we're editing an existing.
CDlgExistingPoints::CDlgExistingPoints(
	CAgilityBookDoc* pDoc,
	ARBDogExistingPointsList& points,
	ARBDogExistingPointsPtr const& inExistingPoints,
	wxWindow* pParent)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_PointsList(points)
	, m_pExistingPoints(inExistingPoints)
	, m_Date()
	, m_Points(0.0)
	, m_Comments()
	, m_ctrlDate(nullptr)
	, m_ctrlType(nullptr)
	, m_ctrlVenues(nullptr)
	, m_TextVenue()
	, m_textDivMultiQs(nullptr)
	, m_ctrlDivMultiQs(nullptr)
	, m_TextDivMultiQ()
	, m_ctrlLevels(nullptr)
	, m_TextLevel()
	, m_ctrlEvents(nullptr)
	, m_TextEvent()
	, m_ctrlSubNames(nullptr)
	, m_TextSubName()
	, m_ctrlTypeNames(nullptr)
	, m_TextTypeName()
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_EXISTING_POINTS"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	if (m_pExistingPoints)
	{
		m_Date = m_pExistingPoints->GetDate();
		m_Points = m_pExistingPoints->GetPoints();
		m_Comments = StringUtil::stringWX(m_pExistingPoints->GetComment());
		// Do not initialize the text for the other combos.
		// Otherwise the validators will try to select before we're ready.
	}
	else
		m_Date.SetToday();

	// Controls (these are done first to control tab order)

	wxStaticText* textType
		= new wxStaticText(this, wxID_ANY, _("IDC_EXISTING_TYPE"), wxDefaultPosition, wxDefaultSize, 0);
	textType->Wrap(-1);

	m_ctrlType = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY | wxCB_SORT);
	m_ctrlType->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgExistingPoints::OnSelchangeType, this);
	m_ctrlType->SetHelpText(_("HIDC_EXISTING_TYPE"));
	m_ctrlType->SetToolTip(_("HIDC_EXISTING_TYPE"));
	// Set the contents of the type combo.
	ARBExistingPointType types[]
		= {ARBExistingPointType::OtherPoints,
		   ARBExistingPointType::Lifetime,
		   ARBExistingPointType::Title,
		   ARBExistingPointType::Speed,
		   //ARBExistingPointType::MQ, Not here - will be added later as long as a venue supports it
		   ARBExistingPointType::SQ};
	int nTypes = sizeof(types) / sizeof(types[0]);
	for (int i = 0; i < nTypes; ++i)
	{
		int index = m_ctrlType->Append(StringUtil::stringWX(ARBDogExistingPoints::GetPointTypeName(types[i])));
		m_ctrlType->SetClientData(index, reinterpret_cast<void*>(types[i]));
		if (m_pExistingPoints && m_pExistingPoints->GetType() == types[i])
			m_ctrlType->SetSelection(index);
		else if (!m_pExistingPoints && ARBExistingPointType::Title == types[i])
			m_ctrlType->SetSelection(index);
	}
	if (wxNOT_FOUND == m_ctrlType->GetSelection())
		m_ctrlType->SetSelection(0);

	wxStaticText* textEarned
		= new wxStaticText(this, wxID_ANY, _("IDC_EXISTING_DATE"), wxDefaultPosition, wxDefaultSize, 0);
	textEarned->Wrap(-1);

	wxDateTime date;
	m_Date.GetDate(date);
	m_ctrlDate = new wxDatePickerCtrl(
		this,
		wxID_ANY,
		date,
		wxDefaultPosition,
		wxDefaultSize,
		wxDP_DROPDOWN | wxDP_SHOWCENTURY);
	m_ctrlDate->Bind(wxEVT_DATE_CHANGED, &CDlgExistingPoints::OnDateChanged, this);
	m_ctrlDate->SetHelpText(_("HIDC_EXISTING_DATE"));
	m_ctrlDate->SetToolTip(_("HIDC_EXISTING_DATE"));

	wxStaticText* textPoints
		= new wxStaticText(this, wxID_ANY, _("IDC_EXISTING_POINTS"), wxDefaultPosition, wxDefaultSize, 0);
	textPoints->Wrap(-1);

	CTextCtrl* ctrlPoints = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 30), -1),
		0,
		CGenericValidator(&m_Points));
	ctrlPoints->SetHelpText(_("HIDC_EXISTING_POINTS"));
	ctrlPoints->SetToolTip(_("HIDC_EXISTING_POINTS"));

	wxStaticBox* boxWhere = new wxStaticBox(this, wxID_ANY, _("IDC_EXISTING_POINTS_WHERE"));

	wxStaticText* textVenue
		= new wxStaticText(this, wxID_ANY, _("IDC_EXISTING_VENUES"), wxDefaultPosition, wxDefaultSize, 0);
	textVenue->Wrap(-1);

	m_ctrlVenues = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY,
		wxGenericValidator(&m_TextVenue));
	m_ctrlVenues->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgExistingPoints::OnSelchangeVenue, this);
	m_ctrlVenues->SetHelpText(_("HIDC_EXISTING_VENUES"));
	m_ctrlVenues->SetToolTip(_("HIDC_EXISTING_VENUES"));

	m_textDivMultiQs = new wxStaticText(this, wxID_ANY, L"", wxDefaultPosition, wxDefaultSize, 0);
	m_textDivMultiQs->Wrap(-1);

	m_ctrlDivMultiQs = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY,
		wxGenericValidator(&m_TextDivMultiQ));
	m_ctrlDivMultiQs->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgExistingPoints::OnSelchangeDivMultiQ, this);

	wxStaticText* textLevel
		= new wxStaticText(this, wxID_ANY, _("IDC_EXISTING_LEVEL"), wxDefaultPosition, wxDefaultSize, 0);
	textLevel->Wrap(-1);

	m_ctrlLevels = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY,
		wxGenericValidator(&m_TextLevel));
	m_ctrlLevels->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgExistingPoints::OnSelchangeLevel, this);
	m_ctrlLevels->SetHelpText(_("HIDC_EXISTING_LEVEL"));
	m_ctrlLevels->SetToolTip(_("HIDC_EXISTING_LEVEL"));

	wxStaticText* textEvent
		= new wxStaticText(this, wxID_ANY, _("IDC_EXISTING_EVENT"), wxDefaultPosition, wxDefaultSize, 0);
	textEvent->Wrap(-1);

	m_ctrlEvents = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY,
		wxGenericValidator(&m_TextEvent));
	m_ctrlEvents->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgExistingPoints::OnSelchangeEvent, this);
	m_ctrlEvents->SetHelpText(_("HIDC_EXISTING_EVENT"));
	m_ctrlEvents->SetToolTip(_("HIDC_EXISTING_EVENT"));

	wxStaticText* textSubName
		= new wxStaticText(this, wxID_ANY, _("IDC_EXISTING_SUBNAME"), wxDefaultPosition, wxDefaultSize, 0);
	textSubName->Wrap(-1);

	m_ctrlSubNames = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY,
		wxGenericValidator(&m_TextSubName));
	m_ctrlSubNames->SetHelpText(_("HIDC_EXISTING_SUBNAME"));
	m_ctrlSubNames->SetToolTip(_("HIDC_EXISTING_SUBNAME"));

	wxStaticText* textOther
		= new wxStaticText(this, wxID_ANY, _("IDC_EXISTING_TYPENAME"), wxDefaultPosition, wxDefaultSize, 0);
	textOther->Wrap(-1);

	m_ctrlTypeNames = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY,
		wxGenericValidator(&m_TextTypeName));
	m_ctrlTypeNames->SetHelpText(_("HIDC_EXISTING_TYPENAME"));
	m_ctrlTypeNames->SetToolTip(_("HIDC_EXISTING_TYPENAME"));

	wxStaticBox* boxComment = new wxStaticBox(this, wxID_ANY, _("IDC_EXISTING_COMMENTS"));

	CSpellCheckCtrl* ctrlNote = new CSpellCheckCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDLG_UNIT(this, wxSize(120, 70)),
		wxTE_MULTILINE | wxTE_WORDWRAP,
		CTrimValidator(&m_Comments, TRIMVALIDATOR_TRIM_RIGHT));
	ctrlNote->SetHelpText(_("HIDC_EXISTING_COMMENTS"));
	ctrlNote->SetToolTip(_("HIDC_EXISTING_COMMENTS"));

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerPoints = new wxBoxSizer(wxHORIZONTAL);
	sizerPoints->Add(textType, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerPoints->Add(m_ctrlType, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerPoints->Add(textEarned, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerPoints->Add(m_ctrlDate, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerPoints->Add(textPoints, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerPoints->Add(ctrlPoints, 0, wxALIGN_CENTER_VERTICAL, 0);

	bSizer->Add(sizerPoints, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerContent = new wxBoxSizer(wxHORIZONTAL);

	wxStaticBoxSizer* sizerWhere = new wxStaticBoxSizer(boxWhere, wxVERTICAL);

	wxFlexGridSizer* sizerCombo = new wxFlexGridSizer(6, 2, 0, 0);
	sizerCombo->SetFlexibleDirection(wxBOTH);
	sizerCombo->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	sizerCombo->Add(textVenue, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 2));
	sizerCombo->Add(m_ctrlVenues, 0, wxALIGN_CENTER_VERTICAL, 0);

	sizerCombo->Add(m_textDivMultiQs, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 2));
	sizerCombo->Add(m_ctrlDivMultiQs, 0, wxALIGN_CENTER_VERTICAL | wxTOP, wxDLG_UNIT_X(this, 2));

	sizerCombo->Add(textLevel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 2));
	sizerCombo->Add(m_ctrlLevels, 0, wxALIGN_CENTER_VERTICAL | wxTOP, wxDLG_UNIT_X(this, 2));

	sizerCombo->Add(textEvent, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 2));
	sizerCombo->Add(m_ctrlEvents, 0, wxALIGN_CENTER_VERTICAL | wxTOP, wxDLG_UNIT_X(this, 2));

	sizerCombo->Add(textSubName, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 2));
	sizerCombo->Add(m_ctrlSubNames, 0, wxALIGN_CENTER_VERTICAL | wxTOP, wxDLG_UNIT_X(this, 2));

	sizerCombo->Add(textOther, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 2));
	sizerCombo->Add(m_ctrlTypeNames, 0, wxALIGN_CENTER_VERTICAL | wxTOP, wxDLG_UNIT_X(this, 2));

	sizerWhere->Add(sizerCombo, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 3));

	sizerContent->Add(sizerWhere, 1, wxEXPAND | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxStaticBoxSizer* sizerComment = new wxStaticBoxSizer(boxComment, wxVERTICAL);
	sizerComment->Add(ctrlNote, 1, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 3));
	sizerContent->Add(sizerComment, 1, wxEXPAND, 0);

	bSizer->Add(sizerContent, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	// Not a separated sizer like most other dialogs because of the groups
	// boxes right above them.
	m_sdbSizer = CreateButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(m_sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	// Add MultiQ to types if at least one venue supports it.
	for (ARBConfigVenueList::const_iterator iterVenue = m_pDoc->Book().GetConfig().GetVenues().begin();
		 iterVenue != m_pDoc->Book().GetConfig().GetVenues().end();
		 ++iterVenue)
	{
		ARBConfigVenuePtr pVenue = (*iterVenue);
		if (0 < pVenue->GetMultiQs().size())
		{
			int index = m_ctrlType->Append(
				StringUtil::stringWX(ARBDogExistingPoints::GetPointTypeName(ARBExistingPointType::MQ)));
			m_ctrlType->SetClientData(index, reinterpret_cast<void*>(ARBExistingPointType::MQ));
			if (m_pExistingPoints && m_pExistingPoints->GetType() == ARBExistingPointType::MQ)
				m_ctrlType->SetSelection(index);
			break;
		}
	}

	// All other combos are filled based on other content.

	FillVenues();
	FillTypeNames();

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	m_ctrlType->SetFocus();
}


ARBExistingPointType CDlgExistingPoints::GetCurrentType() const
{
	ARBExistingPointType type = ARBExistingPointType::Unknown;
	int index = m_ctrlType->GetSelection();
	if (wxNOT_FOUND != index)
	{
		type = static_cast<ARBExistingPointType>(reinterpret_cast<size_t>(m_ctrlType->GetClientData(index)));
	}
	return type;
}


CDlgPointsVenueData* CDlgExistingPoints::GetVenueData(int index) const
{
	wxClientData* pData = m_ctrlVenues->GetClientObject(index);
	return dynamic_cast<CDlgPointsVenueData*>(pData);
}


CDlgPointsDivisionData* CDlgExistingPoints::GetDivisionData(int index) const
{
	wxClientData* pData = m_ctrlDivMultiQs->GetClientObject(index);
	return dynamic_cast<CDlgPointsDivisionData*>(pData);
}


CDlgPointsLevelData* CDlgExistingPoints::GetLevelData(int index) const
{
	wxClientData* pData = m_ctrlLevels->GetClientObject(index);
	return dynamic_cast<CDlgPointsLevelData*>(pData);
}


CDlgPointsEventData* CDlgExistingPoints::GetEventData(int index) const
{
	wxClientData* pData = m_ctrlEvents->GetClientObject(index);
	return dynamic_cast<CDlgPointsEventData*>(pData);
}


CDlgPointsTypeNameData* CDlgExistingPoints::GetTypeNameData(int index) const
{
	wxClientData* pData = m_ctrlTypeNames->GetClientObject(index);
	return dynamic_cast<CDlgPointsTypeNameData*>(pData);
}


CDlgPointsMultiQData* CDlgExistingPoints::GetMultiQData(int index) const
{
	wxClientData* pData = m_ctrlDivMultiQs->GetClientObject(index);
	return dynamic_cast<CDlgPointsMultiQData*>(pData);
}


ARBConfigScoringPtr CDlgExistingPoints::GetConfigScoring() const
{
	ARBConfigScoringPtr scoring;

	long index = m_ctrlDivMultiQs->GetSelection();
	if (wxNOT_FOUND == index)
		return scoring;

	index = m_ctrlLevels->GetSelection();
	if (wxNOT_FOUND == index)
		return scoring;

	index = m_ctrlEvents->GetSelection();
	if (wxNOT_FOUND == index)
		return scoring;

	ARBConfigEventPtr pEvent = GetEventData(index)->m_Event;
	pEvent->FindEvent(
		GetDivisionData(index)->m_Div->GetName(),
		GetLevelData(index)->m_Level->GetName(),
		m_Date,
		&scoring);
	return scoring;
}


void CDlgExistingPoints::SetEnableLists(
	bool& outVenue,
	bool& outDivMQ,
	bool& outLevel,
	bool& outEvent,
	bool& outSubName,
	bool& outTypeName,
	bool bSet)
{
	outVenue = false;
	outDivMQ = false;
	outLevel = false;
	outEvent = false;
	outSubName = false;
	outTypeName = false;

	switch (GetCurrentType())
	{
	case ARBExistingPointType::Unknown:
		break;

	case ARBExistingPointType::OtherPoints:
		outVenue = true;
		outDivMQ = true;
		outLevel = true;
		outEvent = true;
		outTypeName = true;
		break;

	case ARBExistingPointType::Lifetime:
		outVenue = true;
		outDivMQ = true;
		outLevel = true;
		outEvent = true;
		outTypeName = true;
		break;

	case ARBExistingPointType::Title:
		outVenue = true;
		outDivMQ = true;
		outLevel = true;
		outEvent = true;
		{
			ARBConfigScoringPtr scoring = GetConfigScoring();
			if (scoring)
				outSubName = scoring->HasSubNames();
		}
		break;

	case ARBExistingPointType::Speed:
		outVenue = true;
		outDivMQ = true;
		outLevel = true;
		break;

	case ARBExistingPointType::MQ:
		outVenue = true;
		outDivMQ = true;
		break;

	case ARBExistingPointType::SQ:
		outVenue = true;
		outDivMQ = true;
		outLevel = true;
		outEvent = true;
		break;
	}

	if (bSet)
	{
		m_ctrlVenues->Enable(outVenue);
		m_ctrlDivMultiQs->Enable(outDivMQ);
		m_ctrlLevels->Enable(outLevel);
		m_ctrlEvents->Enable(outEvent);
		m_ctrlSubNames->Enable(outSubName);
		m_ctrlTypeNames->Enable(outTypeName);
	}
}


void CDlgExistingPoints::UpdateControls()
{
	bool outVenue;
	bool outDivMQ;
	bool outLevel;
	bool outEvent;
	bool outSubName;
	bool outTypeName;
	SetEnableLists(outVenue, outDivMQ, outLevel, outEvent, outSubName, outTypeName, true);
	ARBExistingPointType type = GetCurrentType();
	if (ARBExistingPointType::MQ == type)
	{
		m_textDivMultiQs->SetLabel(_("IDC_EXISTING_MULTIQ"));
		m_ctrlDivMultiQs->SetHelpText(_("HIDC_EXISTING_MULTIQ"));
		m_ctrlDivMultiQs->SetToolTip(_("HIDC_EXISTING_MULTIQ"));
	}
	else
	{
		m_textDivMultiQs->SetLabel(_("IDC_EXISTING_DIVISION"));
		m_ctrlDivMultiQs->SetHelpText(_("HIDC_EXISTING_DIVISION"));
		m_ctrlDivMultiQs->SetToolTip(_("HIDC_EXISTING_DIVISION"));
	}
	wxWindow* btnOk = FindWindowInSizer(m_sdbSizer, wxID_OK);
	if (btnOk)
		btnOk->Enable(ARBExistingPointType::Unknown != type);
}


void CDlgExistingPoints::FillVenues()
{
	std::wstring venue;
	int index = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND != index)
		venue = m_ctrlVenues->GetString(index);
	else if (m_pExistingPoints)
		venue = m_pExistingPoints->GetVenue();
	m_ctrlVenues->Clear();

	ARBExistingPointType type = GetCurrentType();

	for (ARBConfigVenueList::const_iterator iterVenue = m_pDoc->Book().GetConfig().GetVenues().begin();
		 iterVenue != m_pDoc->Book().GetConfig().GetVenues().end();
		 ++iterVenue)
	{
		ARBConfigVenuePtr pVenue = (*iterVenue);
		if (ARBExistingPointType::MQ != type || 0 < pVenue->GetMultiQs().size())
		{
			int idx = m_ctrlVenues->Append(StringUtil::stringWX(pVenue->GetName()));
			m_ctrlVenues->SetClientObject(idx, new CDlgPointsVenueData(pVenue));
			if (m_pExistingPoints && venue == pVenue->GetName())
				m_ctrlVenues->SetSelection(idx);
		}
	}

	FillDivMultiQ();
}


void CDlgExistingPoints::FillDivMultiQ()
{
	ARBExistingPointType type = GetCurrentType();
	std::wstring divMultiQ;
	int index = m_ctrlDivMultiQs->GetSelection();
	if (wxNOT_FOUND != index)
		divMultiQ = m_ctrlDivMultiQs->GetString(index);
	else if (m_pExistingPoints)
	{
		if (ARBExistingPointType::MQ == type)
			divMultiQ = m_pExistingPoints->GetMultiQ();
		else
			divMultiQ = m_pExistingPoints->GetDivision();
	}
	m_ctrlDivMultiQs->Clear();

	int idxVenue = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND != idxVenue)
	{
		ARBConfigVenuePtr pVenue = GetVenueData(idxVenue)->m_Venue;

		if (ARBExistingPointType::MQ == type)
		{
			for (ARBConfigMultiQList::const_iterator iterQ = pVenue->GetMultiQs().begin();
				 iterQ != pVenue->GetMultiQs().end();
				 ++iterQ)
			{
				ARBConfigMultiQPtr pMulti = *iterQ;
				int idx = m_ctrlDivMultiQs->Append(StringUtil::stringWX(pMulti->GetName()));
				m_ctrlDivMultiQs->SetClientObject(idx, new CDlgPointsMultiQData(pMulti));
				if (m_pExistingPoints && divMultiQ == pMulti->GetName())
					m_ctrlDivMultiQs->SetSelection(idx);
			}
		}
		else
		{
			for (ARBConfigDivisionList::const_iterator iterDiv = pVenue->GetDivisions().begin();
				 iterDiv != pVenue->GetDivisions().end();
				 ++iterDiv)
			{
				ARBConfigDivisionPtr pDiv = (*iterDiv);
				int idx = m_ctrlDivMultiQs->Append(StringUtil::stringWX(pDiv->GetName()));
				m_ctrlDivMultiQs->SetClientObject(idx, new CDlgPointsDivisionData(pDiv));
				if (m_pExistingPoints && divMultiQ == pDiv->GetName())
					m_ctrlDivMultiQs->SetSelection(idx);
			}
		}
	}
	if (ARBExistingPointType::MQ != type)
		FillLevels();
}


void CDlgExistingPoints::FillLevels()
{
	std::wstring level;
	int index = m_ctrlLevels->GetSelection();
	if (wxNOT_FOUND != index)
		level = m_ctrlLevels->GetString(index);
	else if (m_pExistingPoints)
		level = m_pExistingPoints->GetLevel();

	m_ctrlLevels->Clear();
	int idxDiv = m_ctrlDivMultiQs->GetSelection();
	if (wxNOT_FOUND != idxDiv)
	{
		ARBConfigDivisionPtr pDiv = GetDivisionData(idxDiv)->m_Div;
		for (ARBConfigLevelList::const_iterator iter = pDiv->GetLevels().begin(); iter != pDiv->GetLevels().end();
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
					CDlgPointsLevelData* pData = new CDlgPointsLevelData(pLevel, pSubLevel);
					int idx = m_ctrlLevels->Append(StringUtil::stringWX(pSubLevel->GetName()));
					m_ctrlLevels->SetClientObject(idx, pData);
					if (level == pSubLevel->GetName())
						m_ctrlLevels->SetSelection(idx);
				}
			}
			else
			{
				CDlgPointsLevelData* pData = new CDlgPointsLevelData(pLevel);
				int idx = m_ctrlLevels->Append(StringUtil::stringWX(pLevel->GetName()));
				m_ctrlLevels->SetClientObject(idx, pData);
				if (level == pLevel->GetName())
					m_ctrlLevels->SetSelection(idx);
			}
		}
	}
	FillEvents();
}


void CDlgExistingPoints::FillEvents()
{
	std::wstring evt;
	int index = m_ctrlEvents->GetSelection();
	if (wxNOT_FOUND != index)
		evt = m_ctrlEvents->GetString(index);
	else if (m_pExistingPoints)
		evt = m_pExistingPoints->GetEvent();
	m_ctrlEvents->Clear();

	int idxVenue = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND != idxVenue)
	{
		ARBConfigVenuePtr pVenue = GetVenueData(idxVenue)->m_Venue;
		int idxDiv = m_ctrlDivMultiQs->GetSelection();
		if (wxNOT_FOUND != idxDiv)
		{
			ARBConfigDivisionPtr pDiv = GetDivisionData(idxDiv)->m_Div;
			int idxLevel = m_ctrlLevels->GetSelection();
			if (wxNOT_FOUND != idxLevel)
			{
				CDlgPointsLevelData* pData = GetLevelData(idxLevel);
				for (ARBConfigEventList::const_iterator iter = pVenue->GetEvents().begin();
					 iter != pVenue->GetEvents().end();
					 ++iter)
				{
					ARBConfigEventPtr pEvent = (*iter);
					if (pEvent->FindEvent(pDiv->GetName(), pData->m_Level->GetName(), m_Date))
					{
						int idx = m_ctrlEvents->Append(StringUtil::stringWX(pEvent->GetName()));
						m_ctrlEvents->SetClientObject(idx, new CDlgPointsEventData(pEvent));
						if (evt == pEvent->GetName())
							m_ctrlEvents->SetSelection(idx);
					}
				}
			}
		}
	}
	FillSubNames();
	UpdateControls();
}


void CDlgExistingPoints::FillSubNames()
{
	m_ctrlSubNames->Clear();

	int index = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND == index)
		return;
	ARBConfigVenuePtr pVenue = GetVenueData(index)->m_Venue;

	index = m_ctrlEvents->GetSelection();
	if (wxNOT_FOUND == index)
		return;
	ARBConfigEventPtr pEvent = GetEventData(index)->m_Event;

	ARBConfigScoringPtr scoring = GetConfigScoring();
	if (!scoring)
		return;

	if (scoring->HasSubNames())
	{
		m_ctrlSubNames->Enable(true);
		std::set<std::wstring> names;
		m_pDoc->Book().GetAllEventSubNames(pVenue->GetName(), pEvent, names);
		for (std::set<std::wstring>::const_iterator iter = names.begin(); iter != names.end(); ++iter)
		{
			index = m_ctrlSubNames->Append(StringUtil::stringWX(*iter));
			if (m_pExistingPoints && *iter == m_pExistingPoints->GetSubName())
				m_ctrlSubNames->SetSelection(index);
		}
	}
	else
		m_ctrlSubNames->Enable(false);
}


void CDlgExistingPoints::FillTypeNames()
{
	ARBExistingPointType type = GetCurrentType();
	ARBConfigVenuePtr pVenue;
	int idxVenue = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND != idxVenue)
		pVenue = GetVenueData(idxVenue)->m_Venue;

	m_ctrlTypeNames->Clear();

	switch (type)
	{
	case ARBExistingPointType::Unknown:
	case ARBExistingPointType::Title:
	case ARBExistingPointType::Speed:
	case ARBExistingPointType::MQ:
	case ARBExistingPointType::SQ:
		break;

	case ARBExistingPointType::OtherPoints:
		for (ARBConfigOtherPointsList::const_iterator iterOther = m_pDoc->Book().GetConfig().GetOtherPoints().begin();
			 iterOther != m_pDoc->Book().GetConfig().GetOtherPoints().end();
			 ++iterOther)
		{
			ARBConfigOtherPointsPtr pOther = (*iterOther);
			int idxName = m_ctrlTypeNames->Append(StringUtil::stringWX(pOther->GetName()));
			m_ctrlTypeNames->SetClientObject(idxName, new CDlgPointsTypeNameData(pOther));
			if (m_pExistingPoints && ARBExistingPointType::OtherPoints == m_pExistingPoints->GetType())
			{
				if (m_pExistingPoints->GetTypeName() == pOther->GetName())
					m_ctrlTypeNames->SetSelection(idxName);
			}
		}
		break;

	case ARBExistingPointType::Lifetime:
		if (pVenue)
			for (ARBConfigLifetimeNameList::const_iterator iterOther = pVenue->GetLifetimeNames().begin();
				 iterOther != pVenue->GetLifetimeNames().end();
				 ++iterOther)
			{
				ARBConfigLifetimeNamePtr pOther = (*iterOther);
				int idxName = m_ctrlTypeNames->Append(StringUtil::stringWX(pOther->GetName()));
				m_ctrlTypeNames->SetClientObject(idxName, new CDlgPointsTypeNameData(pOther));
				if (m_pExistingPoints && ARBExistingPointType::Lifetime == m_pExistingPoints->GetType())
				{
					if (m_pExistingPoints->GetTypeName() == pOther->GetName())
						m_ctrlTypeNames->SetSelection(idxName);
				}
			}
		break;
	}
}


void CDlgExistingPoints::OnDateChanged(wxDateEvent& evt)
{
	m_Date.SetDate(evt.GetDate().GetYear(), evt.GetDate().GetMonth() + 1, evt.GetDate().GetDay());
	FillEvents();
}


void CDlgExistingPoints::OnSelchangeType(wxCommandEvent& evt)
{
	UpdateControls();
	FillTypeNames();
}


void CDlgExistingPoints::OnSelchangeVenue(wxCommandEvent& evt)
{
	FillDivMultiQ();
	FillTypeNames();
}


void CDlgExistingPoints::OnSelchangeDivMultiQ(wxCommandEvent& evt)
{
	if (ARBExistingPointType::MQ != GetCurrentType())
		FillLevels();
}


void CDlgExistingPoints::OnSelchangeLevel(wxCommandEvent& evt)
{
	FillEvents();
}


void CDlgExistingPoints::OnSelchangeEvent(wxCommandEvent& evt)
{
	FillSubNames();
}


void CDlgExistingPoints::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	ARBExistingPointType type = GetCurrentType();
	std::wstring typeName;
	std::wstring venue;
	std::wstring div;
	std::wstring level;
	std::wstring eventName;
	std::wstring subName;
	std::wstring multiQ;
	switch (type)
	{
	case ARBExistingPointType::Unknown:
		break;
	//  OtherPts Venue    Division Level Event
	case ARBExistingPointType::OtherPoints:
		typeName = m_TextTypeName;
		venue = m_TextVenue;
		div = m_TextDivMultiQ;
		level = m_TextLevel;
		eventName = m_TextEvent;
		break;
	//  OtherPts Venue    Division Level Event
	case ARBExistingPointType::Lifetime:
		typeName = m_TextTypeName;
		venue = m_TextVenue;
		div = m_TextDivMultiQ;
		level = m_TextLevel;
		eventName = m_TextEvent;
		break;
		//  Venue    Division Level    Event SubName
	case ARBExistingPointType::Title:
		venue = m_TextVenue;
		div = m_TextDivMultiQ;
		level = m_TextLevel;
		eventName = m_TextEvent;
		subName = m_TextSubName;
		break;
	//  Venue    Division Level    Event
	case ARBExistingPointType::SQ:
		venue = m_TextVenue;
		div = m_TextDivMultiQ;
		level = m_TextLevel;
		eventName = m_TextEvent;
		break;
	//  Venue    Division Level
	case ARBExistingPointType::Speed:
		venue = m_TextVenue;
		div = m_TextDivMultiQ;
		level = m_TextLevel;
		break;
	//  Venue    MultiQ
	case ARBExistingPointType::MQ:
		venue = m_TextVenue;
		multiQ = m_TextDivMultiQ;
		break;
	}

	if (m_pExistingPoints)
	{
		m_pExistingPoints->SetDate(m_Date);
		m_pExistingPoints->SetType(type);
		m_pExistingPoints->SetTypeName(typeName);
		m_pExistingPoints->SetVenue(venue);
		m_pExistingPoints->SetMultiQ(multiQ);
		m_pExistingPoints->SetDivision(div);
		m_pExistingPoints->SetLevel(level);
		m_pExistingPoints->SetEvent(eventName);
		m_pExistingPoints->SetSubName(subName);
		m_pExistingPoints->SetComment(StringUtil::stringW(m_Comments));
		m_pExistingPoints->SetPoints(m_Points);
	}
	else
	{
		ARBDogExistingPointsPtr pPoints(ARBDogExistingPoints::New());
		if (m_PointsList.AddExistingPoints(pPoints))
		{
			pPoints->SetDate(m_Date);
			pPoints->SetType(type);
			pPoints->SetTypeName(typeName);
			pPoints->SetVenue(venue);
			pPoints->SetMultiQ(multiQ);
			pPoints->SetDivision(div);
			pPoints->SetLevel(level);
			pPoints->SetEvent(eventName);
			pPoints->SetSubName(subName);
			pPoints->SetComment(StringUtil::stringW(m_Comments));
			pPoints->SetPoints(m_Points);
		}
	}

	EndDialog(wxID_OK);
}
