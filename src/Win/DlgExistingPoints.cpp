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
#include "Globals.h"
#include "Validators.h"
#include "Widgets.h"

#include "ARB/ARBConfig.h"
#include "ARB/ARBDogExistingPoints.h"
#include "ARBCommon/StringUtil.h"
#include <wx/datectrl.h>
#include <wx/dateevt.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

class CDlgPointsVenueData : public wxClientData
{
public:
	CDlgPointsVenueData(ARBConfigVenuePtr venue)
		: m_Venue(venue)
	{
	}
	ARBConfigVenuePtr m_Venue;
};

class CDlgPointsDivisionData : public wxClientData
{
public:
	CDlgPointsDivisionData(ARBConfigDivisionPtr div)
		: m_Div(div)
	{
	}
	ARBConfigDivisionPtr m_Div;
};

class CDlgPointsLevelData : public wxClientData
{
public:
	CDlgPointsLevelData(ARBConfigLevelPtr level)
		: m_Level(level)
		, m_SubLevel()
	{
	}
	CDlgPointsLevelData(
			ARBConfigLevelPtr level,
			ARBConfigSubLevelPtr subLevel)
		: m_Level(level)
		, m_SubLevel(subLevel)
	{
	}
	ARBConfigLevelPtr m_Level;
	ARBConfigSubLevelPtr m_SubLevel;
};

class CDlgPointsEventData : public wxClientData
{
public:
	CDlgPointsEventData(ARBConfigEventPtr evt)
		: m_Event(evt)
	{
	}
	ARBConfigEventPtr m_Event;
};

class CDlgPointsOtherPtData : public wxClientData
{
public:
	CDlgPointsOtherPtData(ARBConfigOtherPointsPtr other)
		: m_Other(other)
	{
	}
	ARBConfigOtherPointsPtr m_Other;
};

class CDlgPointsMultiQData : public wxClientData
{
public:
	CDlgPointsMultiQData(ARBConfigMultiQPtr multiQ)
		: m_MultiQ(multiQ)
	{
	}
	ARBConfigMultiQPtr m_MultiQ;
};

/////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(CDlgExistingPoints, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgExistingPoints::OnOk)
END_EVENT_TABLE()


// Controls (all are readonly, except subname):
// ARBDogExistingPoints::eOtherPoints:
//  Venue  Division  Level  Event          OtherPts 
// ARBDogExistingPoints::eRuns:
//  Venue  Division  Level  Event  SubName
// ARBDogExistingPoints::eSQ:
//  Venue  Division  Level  Event
// ARBDogExistingPoints::eSpeed:
//  Venue  Division  Level
// ARBDogExistingPoints::eMQ:
//  Venue  MultiQ

// If pExistingPoints is NULL, we're creating a new entry. Otherwise, we're editing an existing.
CDlgExistingPoints::CDlgExistingPoints(
		CAgilityBookDoc* pDoc,
		ARBDogExistingPointsList& points,
		ARBDogExistingPointsPtr pExistingPoints,
		wxWindow* pParent)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_PointsList(points)
	, m_pExistingPoints(pExistingPoints)
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
	, m_ctrlOthers(nullptr)
	, m_TextOther()
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

	wxStaticText* textType = new wxStaticText(this, wxID_ANY,
		_("IDC_EXISTING_TYPE"),
		wxDefaultPosition, wxDefaultSize, 0);
	textType->Wrap(-1);

	m_ctrlType = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, nullptr, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SORT);
	BIND_OR_CONNECT_CTRL(m_ctrlType, wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler, CDlgExistingPoints::OnSelchangeType);
	m_ctrlType->SetHelpText(_("HIDC_EXISTING_TYPE"));
	m_ctrlType->SetToolTip(_("HIDC_EXISTING_TYPE"));
	// Set the contents of the type combo.
	ARBDogExistingPoints::PointType types[] =
	{
		ARBDogExistingPoints::eOtherPoints,
		ARBDogExistingPoints::eRuns,
		ARBDogExistingPoints::eSpeed,
		//ARBDogExistingPoints::eMQ, Not here - will be added later as long as a venue supports it
		ARBDogExistingPoints::eSQ
	};
	int nTypes = sizeof(types) / sizeof(types[0]);
	for (int i = 0; i < nTypes; ++i)
	{
		int index = m_ctrlType->Append(StringUtil::stringWX(ARBDogExistingPoints::GetPointTypeName(types[i])));
		m_ctrlType->SetClientData(index, reinterpret_cast<void*>(types[i]));
		if (m_pExistingPoints && m_pExistingPoints->GetType() == types[i])
			m_ctrlType->SetSelection(index);
		else if (!m_pExistingPoints && ARBDogExistingPoints::eRuns == types[i])
			m_ctrlType->SetSelection(index);
	}
	if (wxNOT_FOUND == m_ctrlType->GetSelection())
		m_ctrlType->SetSelection(0);

	wxStaticText* textEarned = new wxStaticText(this, wxID_ANY,
		_("IDC_EXISTING_DATE"),
		wxDefaultPosition, wxDefaultSize, 0);
	textEarned->Wrap(-1);

	wxDateTime date;
	m_Date.GetDate(date);
	m_ctrlDate = new wxDatePickerCtrl(this, wxID_ANY,
		date,
		wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN|wxDP_SHOWCENTURY);
	BIND_OR_CONNECT_CTRL(m_ctrlDate, wxEVT_DATE_CHANGED, wxDateEventHandler, CDlgExistingPoints::OnDateChanged);
	m_ctrlDate->SetHelpText(_("HIDC_EXISTING_DATE"));
	m_ctrlDate->SetToolTip(_("HIDC_EXISTING_DATE"));

	wxStaticText* textPoints = new wxStaticText(this, wxID_ANY,
		_("IDC_EXISTING_POINTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textPoints->Wrap(-1);

	CTextCtrl* ctrlPoints = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0,
		CGenericValidator(&m_Points));
	ctrlPoints->SetHelpText(_("HIDC_EXISTING_POINTS"));
	ctrlPoints->SetToolTip(_("HIDC_EXISTING_POINTS"));

	wxStaticBox* boxWhere = new wxStaticBox(this, wxID_ANY, _("IDC_EXISTING_POINTS_WHERE"));

	wxStaticText* textVenue = new wxStaticText(this, wxID_ANY,
		_("IDC_EXISTING_VENUES"),
		wxDefaultPosition, wxDefaultSize, 0);
	textVenue->Wrap(-1);

	m_ctrlVenues = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, nullptr, wxCB_DROPDOWN|wxCB_READONLY,
		wxGenericValidator(&m_TextVenue));
	BIND_OR_CONNECT_CTRL(m_ctrlVenues, wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler, CDlgExistingPoints::OnSelchangeVenue);
	m_ctrlVenues->SetHelpText(_("HIDC_EXISTING_VENUES"));
	m_ctrlVenues->SetToolTip(_("HIDC_EXISTING_VENUES"));

	m_textDivMultiQs = new wxStaticText(this, wxID_ANY, L"",
		wxDefaultPosition, wxDefaultSize, 0);
	m_textDivMultiQs->Wrap(-1);

	m_ctrlDivMultiQs = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, nullptr, wxCB_DROPDOWN|wxCB_READONLY,
		wxGenericValidator(&m_TextDivMultiQ));
	BIND_OR_CONNECT_CTRL(m_ctrlDivMultiQs, wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler, CDlgExistingPoints::OnSelchangeDivMultiQ);

	wxStaticText* textLevel = new wxStaticText(this, wxID_ANY, _("IDC_EXISTING_LEVEL"),
		wxDefaultPosition, wxDefaultSize, 0);
	textLevel->Wrap(-1);

	m_ctrlLevels = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, nullptr, wxCB_DROPDOWN|wxCB_READONLY,
		wxGenericValidator(&m_TextLevel));
	BIND_OR_CONNECT_CTRL(m_ctrlLevels, wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler, CDlgExistingPoints::OnSelchangeLevel);
	m_ctrlLevels->SetHelpText(_("HIDC_EXISTING_LEVEL"));
	m_ctrlLevels->SetToolTip(_("HIDC_EXISTING_LEVEL"));

	wxStaticText* textEvent = new wxStaticText(this, wxID_ANY, _("IDC_EXISTING_EVENT"),
		wxDefaultPosition, wxDefaultSize, 0);
	textEvent->Wrap(-1);

	m_ctrlEvents = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, nullptr, wxCB_DROPDOWN|wxCB_READONLY,
		wxGenericValidator(&m_TextEvent));
	BIND_OR_CONNECT_CTRL(m_ctrlEvents, wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler, CDlgExistingPoints::OnSelchangeEvent);
	m_ctrlEvents->SetHelpText(_("HIDC_EXISTING_EVENT"));
	m_ctrlEvents->SetToolTip(_("HIDC_EXISTING_EVENT"));

	wxStaticText* textSubName = new wxStaticText(this, wxID_ANY, _("IDC_EXISTING_SUBNAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	textSubName->Wrap(-1);

	m_ctrlSubNames = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, nullptr, wxCB_DROPDOWN|wxCB_READONLY,
		wxGenericValidator(&m_TextSubName));
	m_ctrlSubNames->SetHelpText(_("HIDC_EXISTING_SUBNAME"));
	m_ctrlSubNames->SetToolTip(_("HIDC_EXISTING_SUBNAME"));

	wxStaticText* textOther = new wxStaticText(this, wxID_ANY, _("IDC_EXISTING_OTHERPOINTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textOther->Wrap(-1);

	m_ctrlOthers = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, nullptr, wxCB_DROPDOWN|wxCB_READONLY,
		wxGenericValidator(&m_TextOther));
	m_ctrlOthers->SetHelpText(_("HIDC_EXISTING_OTHERPOINTS"));
	m_ctrlOthers->SetToolTip(_("HIDC_EXISTING_OTHERPOINTS"));

	wxStaticBox* boxComment = new wxStaticBox(this, wxID_ANY, _("IDC_EXISTING_COMMENTS"));

	CTextCtrl* ctrlNote = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(200, 150), wxTE_MULTILINE|wxTE_WORDWRAP,
		CTrimValidator(&m_Comments, TRIMVALIDATOR_TRIM_RIGHT));
	ctrlNote->SetHelpText(_("HIDC_EXISTING_COMMENTS"));
	ctrlNote->SetToolTip(_("HIDC_EXISTING_COMMENTS"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerPoints = new wxBoxSizer(wxHORIZONTAL);
	sizerPoints->Add(textType, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerPoints->Add(m_ctrlType, 0, wxALL, 5);
	sizerPoints->Add(textEarned, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerPoints->Add(m_ctrlDate, 0, wxALL, 5);
	sizerPoints->Add(textPoints, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerPoints->Add(ctrlPoints, 0, wxALL, 5);

	bSizer->Add(sizerPoints, 0, wxEXPAND, 0);

	wxBoxSizer* sizerContent = new wxBoxSizer(wxHORIZONTAL);

	wxStaticBoxSizer* sizerWhere = new wxStaticBoxSizer(boxWhere, wxVERTICAL);

	wxFlexGridSizer* sizerCombo = new wxFlexGridSizer(6, 2, 0, 0);
	sizerCombo->SetFlexibleDirection(wxBOTH);
	sizerCombo->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	sizerCombo->Add(textVenue, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerCombo->Add(m_ctrlVenues, 0, wxALL, 5);

	sizerCombo->Add(m_textDivMultiQs, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerCombo->Add(m_ctrlDivMultiQs, 0, wxALL, 5);

	sizerCombo->Add(textLevel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerCombo->Add(m_ctrlLevels, 0, wxALL, 5);

	sizerCombo->Add(textEvent, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerCombo->Add(m_ctrlEvents, 0, wxALL, 5);

	sizerCombo->Add(textSubName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerCombo->Add(m_ctrlSubNames, 0, wxALL, 5);

	sizerCombo->Add(textOther, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerCombo->Add(m_ctrlOthers, 0, wxALL, 5);

	sizerWhere->Add(sizerCombo, 0, wxEXPAND, 0);

	sizerContent->Add(sizerWhere, 1, wxALL|wxEXPAND, 5);

	wxStaticBoxSizer* sizerComment = new wxStaticBoxSizer(boxComment, wxVERTICAL);
	sizerComment->Add(ctrlNote, 1, wxALL|wxEXPAND, 5);
	sizerContent->Add(sizerComment, 1, wxALL|wxEXPAND, 5);

	bSizer->Add(sizerContent, 1, wxEXPAND, 0);

	// Not a separated sizer like most other dialogs because of the groups
	// boxes right above them.
	m_sdbSizer = CreateButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(m_sdbSizer, 0, wxALL|wxEXPAND, 5);

	// Add MultiQ to types if at least one venue supports it.
	for (ARBConfigVenueList::const_iterator iterVenue = m_pDoc->Book().GetConfig().GetVenues().begin();
		iterVenue != m_pDoc->Book().GetConfig().GetVenues().end();
		++iterVenue)
	{
		ARBConfigVenuePtr pVenue = (*iterVenue);
		if (0 < pVenue->GetMultiQs().size())
		{
			int index = m_ctrlType->Append(StringUtil::stringWX(ARBDogExistingPoints::GetPointTypeName(ARBDogExistingPoints::eMQ)));
			m_ctrlType->SetClientData(index, reinterpret_cast<void*>(ARBDogExistingPoints::eMQ));
			if (m_pExistingPoints && m_pExistingPoints->GetType() == ARBDogExistingPoints::eMQ)
				m_ctrlType->SetSelection(index);
			break;
		}
	}

	// Set the contents of the other points combo.
	for (ARBConfigOtherPointsList::const_iterator iterOther = m_pDoc->Book().GetConfig().GetOtherPoints().begin();
		iterOther != m_pDoc->Book().GetConfig().GetOtherPoints().end();
		++iterOther)
	{
		ARBConfigOtherPointsPtr pOther = (*iterOther);
		int index = m_ctrlOthers->Append(StringUtil::stringWX(pOther->GetName()));
		m_ctrlOthers->SetClientObject(index, new CDlgPointsOtherPtData(pOther));
		if (m_pExistingPoints && ARBDogExistingPoints::eOtherPoints == m_pExistingPoints->GetType())
		{
			if (m_pExistingPoints->GetOtherPoints() == pOther->GetName())
				m_ctrlOthers->SetSelection(index);
		}
	}

	// All other combos are filled based on other content.

	FillVenues();

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	m_ctrlType->SetFocus();
}


ARBDogExistingPoints::PointType CDlgExistingPoints::GetCurrentType() const
{
	ARBDogExistingPoints::PointType type = ARBDogExistingPoints::eUnknown;
	int index = m_ctrlType->GetSelection();
	if (wxNOT_FOUND != index)
	{
		type = static_cast<ARBDogExistingPoints::PointType>(
			reinterpret_cast<int>(m_ctrlType->GetClientData(index)));
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


CDlgPointsOtherPtData* CDlgExistingPoints::GetOtherPointData(int index) const
{
	wxClientData* pData = m_ctrlOthers->GetClientObject(index);
	return dynamic_cast<CDlgPointsOtherPtData*>(pData);
}


CDlgPointsMultiQData* CDlgExistingPoints::GetMultiQData(int index) const
{
	wxClientData* pData = m_ctrlDivMultiQs->GetClientObject(index);
	return dynamic_cast<CDlgPointsMultiQData*>(pData);
}


void CDlgExistingPoints::SetEnableLists(
		bool& outVenue,
		bool& outDivMQ,
		bool& outLevel,
		bool& outEvent,
		bool& outSubName,
		bool& outOther,
		bool bSet)
{
	outVenue = false;
	outDivMQ = false;
	outLevel = false;
	outEvent = false;
	outSubName = false;
	outOther = false;

	switch (GetCurrentType())
	{
	default:
		break;

	case ARBDogExistingPoints::eOtherPoints:
		outVenue = true;
		outDivMQ = true;
		outLevel = true;
		outEvent = true;
		outOther = true;
		break;

	case ARBDogExistingPoints::eRuns:
		outVenue = true;
		outDivMQ = true;
		outLevel = true;
		outEvent = true;
		{
			long index = m_ctrlEvents->GetSelection();
			if (wxNOT_FOUND != index)
			{
				ARBConfigEventPtr pEvent = GetEventData(index)->m_Event;
				outSubName = pEvent->HasSubNames();
			}
		}
		break;

	case ARBDogExistingPoints::eSpeed:
		outVenue = true;
		outDivMQ = true;
		outLevel = true;
		break;

	case ARBDogExistingPoints::eMQ:
		outVenue = true;
		outDivMQ = true;
		break;

	case ARBDogExistingPoints::eSQ:
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
		m_ctrlOthers->Enable(outOther);
	}
}


void CDlgExistingPoints::UpdateControls()
{
	bool outVenue;
	bool outDivMQ;
	bool outLevel;
	bool outEvent;
	bool outSubName;
	bool outOther;
	SetEnableLists(outVenue, outDivMQ, outLevel, outEvent, outSubName, outOther, true);
	ARBDogExistingPoints::PointType type = GetCurrentType();
	if (ARBDogExistingPoints::eMQ == type)
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
		btnOk->Enable(ARBDogExistingPoints::eUnknown != type);
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

	ARBDogExistingPoints::PointType type = GetCurrentType();

	for (ARBConfigVenueList::const_iterator iterVenue = m_pDoc->Book().GetConfig().GetVenues().begin();
		iterVenue != m_pDoc->Book().GetConfig().GetVenues().end();
		++iterVenue)
	{
		ARBConfigVenuePtr pVenue = (*iterVenue);
		if (ARBDogExistingPoints::eMQ != type || 0 < pVenue->GetMultiQs().size())
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
	ARBDogExistingPoints::PointType type = GetCurrentType();
	std::wstring divMultiQ;
	int index = m_ctrlDivMultiQs->GetSelection();
	if (wxNOT_FOUND != index)
		divMultiQ = m_ctrlDivMultiQs->GetString(index);
	else if (m_pExistingPoints)
	{
		if (ARBDogExistingPoints::eMQ == type)
			divMultiQ = m_pExistingPoints->GetMultiQ();
		else
			divMultiQ = m_pExistingPoints->GetDivision();
	}
	m_ctrlDivMultiQs->Clear();

	int idxVenue = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND != idxVenue)
	{
		ARBConfigVenuePtr pVenue = GetVenueData(idxVenue)->m_Venue;

		if (ARBDogExistingPoints::eMQ == type)
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
	if (ARBDogExistingPoints::eMQ != type)
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

	if (pEvent->HasSubNames())
	{
		m_ctrlSubNames->Enable(true);
		std::set<std::wstring> names;
		m_pDoc->Book().GetAllEventSubNames(pVenue->GetName(), pEvent, names);
		for (std::set<std::wstring>::const_iterator iter = names.begin();
			iter != names.end();
			++iter)
		{
			index = m_ctrlSubNames->Append(StringUtil::stringWX(*iter));
			if (m_pExistingPoints && *iter == m_pExistingPoints->GetSubName())
				m_ctrlSubNames->SetSelection(index);
		}
	}
	else
		m_ctrlSubNames->Enable(false);
}


void CDlgExistingPoints::OnDateChanged(wxDateEvent& evt)
{
	m_Date.SetDate(evt.GetDate().GetYear(), evt.GetDate().GetMonth() + 1, evt.GetDate().GetDay());
	FillEvents();
}


void CDlgExistingPoints::OnSelchangeType(wxCommandEvent& evt)
{
	UpdateControls();
}


void CDlgExistingPoints::OnSelchangeVenue(wxCommandEvent& evt)
{
	FillDivMultiQ();
}


void CDlgExistingPoints::OnSelchangeDivMultiQ(wxCommandEvent& evt)
{
	if (ARBDogExistingPoints::eMQ != GetCurrentType())
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

	ARBDogExistingPoints::PointType type = GetCurrentType();
	std::wstring other;
	std::wstring venue;
	std::wstring div;
	std::wstring level;
	std::wstring eventName;
	std::wstring subName;
	std::wstring multiQ;
	switch (type)
	{
	default:
		break;
	//  OtherPts Venue    Division Level Event
	case ARBDogExistingPoints::eOtherPoints:
		other = m_TextOther;
		venue = m_TextVenue;
		div = m_TextDivMultiQ;
		level = m_TextLevel;
		eventName = m_TextEvent;
		break;
	//  Venue    Division Level    Event SubName
	case ARBDogExistingPoints::eRuns:
		venue = m_TextVenue;
		div = m_TextDivMultiQ;
		level = m_TextLevel;
		eventName = m_TextEvent;
		subName = m_TextSubName;
		break;
	//  Venue    Division Level    Event
	case ARBDogExistingPoints::eSQ:
		venue = m_TextVenue;
		div = m_TextDivMultiQ;
		level = m_TextLevel;
		eventName = m_TextEvent;
		break;
	//  Venue    Division Level
	case ARBDogExistingPoints::eSpeed:
		venue = m_TextVenue;
		div = m_TextDivMultiQ;
		level = m_TextLevel;
		break;
	//  Venue    MultiQ
	case ARBDogExistingPoints::eMQ:
		venue = m_TextVenue;
		multiQ = m_TextDivMultiQ;
		break;
	}

	if (m_pExistingPoints)
	{
		m_pExistingPoints->SetDate(m_Date);
		m_pExistingPoints->SetType(type);
		m_pExistingPoints->SetOtherPoints(other);
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
			pPoints->SetOtherPoints(other);
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
