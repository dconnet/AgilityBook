/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgConfigEvent class
 * @author David Connet
 *
 * There's one caveat to this dialog - there is no verification done to
 * make sure that a division/level pair isn't multiply defined. If that
 * happens, when we go to check things, the first match in the list will
 * be the only one used.
 * Note: The UI checks when creating div/level, so this shouldn't happen.
 * If the file is hand-edited, it could... But just how paranoid do we get?!
 * (Plus, the paranoia checking should be done when the file is loaded.)
 *
 * Revision History
 * 2020-01-14 Fix initialization of HasTable
 * 2018-12-16 Convert to fmt.
 * 2016-06-19 Add support for named/speedpt lifetime points.
 * 2016-01-16 Cleaned up new/edit/delete buttons.
 * 2014-12-31 Changed pixels to dialog units.
 * 2012-06-16 Display number of faults for each time fault.
 * 2012-02-16 Fix initial focus.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-07-14 Fixed group box creation order.
 * 2009-02-11 Ported to wxWidgets.
 * 2008-07-29 Method overlap detection was wrong.
 * 2007-05-08 Fixed bug when deleting a method.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-04 Added support for NADAC bonus titling points.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2005-01-02 Added subnames to events.
 * 2005-01-01 Renamed MachPts to SpeedPts.
 * 2004-12-18 Added a time fault multiplier.
 * 2004-12-10 Changes weren't saved when lifetime pts were modified.
 * 2004-11-15 Added time fault computation option on T+F.
 * 2004-04-01 Fixed? the memory access fault.
 * 2003-12-27 Added support for from/to dates for the scoring method.
 */

#include "stdafx.h"
#include "DlgConfigEvent.h"

#include "AgilityBook.h"
#include "DlgConfigEventMethod.h"
#include "DlgConfigTitlePoints.h"
#include "DlgConfigure.h"
#include "DlgName.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBConfigEvent.h"
#include "ARB/ARBConfigVenue.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ListData.h"
#include "LibARBWin/Validators.h"
#include "LibARBWin/Widgets.h"
#include <algorithm>
#include <wx/statline.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


class CConfigEventDataLifetimePoints : public wxClientData
{
public:
	CConfigEventDataLifetimePoints(ARBConfigLifetimePointsPtr const& inData)
		: m_Data(inData)
	{
	}
	ARBConfigLifetimePointsPtr GetData() const	{return m_Data;}
private:
	ARBConfigLifetimePointsPtr m_Data;
};


class CConfigEventDataPlaceInfo : public wxClientData
{
public:
	CConfigEventDataPlaceInfo(ARBConfigPlaceInfoPtr const& inData)
		: m_Data(inData)
	{
	}
	ARBConfigPlaceInfoPtr GetData() const	{return m_Data;}
private:
	ARBConfigPlaceInfoPtr m_Data;
};


class CConfigEventDataScoring : public wxClientData
{
public:
	CConfigEventDataScoring(ARBConfigScoringPtr const& inData)
		: m_Data(inData)
	{
	}
	ARBConfigScoringPtr GetData() const	{return m_Data;}
private:
	ARBConfigScoringPtr m_Data;
};


class CConfigEventDataTitlePoints : public wxClientData
{
public:
	CConfigEventDataTitlePoints(ARBConfigTitlePointsPtr const& inData)
		: m_Data(inData)
	{
	}
	ARBConfigTitlePointsPtr GetData() const	{return m_Data;}
private:
	ARBConfigTitlePointsPtr m_Data;
};

/////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(CDlgConfigEvent, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgConfigEvent::OnOk)
wxEND_EVENT_TABLE()


CDlgConfigEvent::CDlgConfigEvent(
		bool bNewEntry,
		ARBConfigVenuePtr const& inVenue,
		ARBConfigEventPtr const& inEvent,
		wxWindow* pParent)
	: wxDialog()
	, m_bNewEntry(bNewEntry)
	, m_pVenue(inVenue)
	, m_pEvent(inEvent)
	, m_DlgFixup()
	, m_Scorings()
	, m_Name(StringUtil::stringWX(m_pEvent->GetName()))
	, m_bHasPartners(m_pEvent->HasPartner())
	, m_Desc(StringUtil::stringWX(m_pEvent->GetDesc()))
	, m_ctrlName(nullptr)
	, m_ctrlTable(nullptr)
	, m_ctrlHasSubnames(nullptr)
	, m_ctrlSubNames(nullptr)
	, m_ctrlSubNamesNew(nullptr)
	, m_ctrlSubNamesEdit(nullptr)
	, m_ctrlSubNamesDelete(nullptr)
	, m_ctrlNew(nullptr)
	, m_ctrlEdit(nullptr)
	, m_ctrlDelete(nullptr)
	, m_ctrlCopy(nullptr)
	, m_ctrlUp(nullptr)
	, m_ctrlDown(nullptr)
	, m_ctrlMethods(nullptr)
	, m_ctrlUnused(nullptr)
	, m_ctrlInfo(nullptr)
	, m_ctrlPointsList(nullptr)
	, m_ctrlPointsNew(nullptr)
	, m_ctrlPointsEdit(nullptr)
	, m_ctrlPointsDelete(nullptr)
	, m_ctrlNote(nullptr)
	, m_idxMethod(-1)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_CONFIG_EVENT"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	assert(m_pVenue);
	assert(m_pEvent);
	// Copy the existing scorings.
	m_pEvent->GetScorings().Clone(m_Scorings);

	// Controls (these are done first to control tab order)

	wxStaticText* textName = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_EVENT"),
		wxDefaultPosition, wxDefaultSize, 0);
	textName->Wrap(-1);

	m_ctrlName = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDLG_UNIT(this, wxSize(50, -1)), 0,
		CTrimValidator(&m_Name, TRIMVALIDATOR_DEFAULT, _("IDS_ENTER_NAME")));
	m_ctrlName->SetHelpText(_("HIDC_CONFIG_EVENT"));
	m_ctrlName->SetToolTip(_("HIDC_CONFIG_EVENT"));

	m_ctrlTable = new wxCheckBox(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_TABLE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlTable->Enable(false);
	m_ctrlTable->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgConfigEvent::OnClickedTable, this);
	m_ctrlTable->SetHelpText(_("HIDC_CONFIG_EVENT_TABLE"));
	m_ctrlTable->SetToolTip(_("HIDC_CONFIG_EVENT_TABLE"));

	wxCheckBox* ctrlPartners = new wxCheckBox(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_PARTNER"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bHasPartners));
	ctrlPartners->SetHelpText(_("HIDC_CONFIG_EVENT_PARTNER"));
	ctrlPartners->SetToolTip(_("HIDC_CONFIG_EVENT_PARTNER"));

	m_ctrlHasSubnames = new wxCheckBox(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_HAS_SUBNAMES"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlHasSubnames->Enable(false);
	m_ctrlHasSubnames->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgConfigEvent::OnClickedSubNames, this);
	m_ctrlHasSubnames->SetHelpText(_("HIDC_CONFIG_EVENT_HAS_SUBNAMES"));
	m_ctrlHasSubnames->SetToolTip(_("HIDC_CONFIG_EVENT_HAS_SUBNAMES"));

	wxStaticText* textNote = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_DESC"),
		wxDefaultPosition, wxDefaultSize, 0);
	textNote->Wrap(-1);

	CSpellCheckCtrl* ctrlNote = new CSpellCheckCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_WORDWRAP,
		CTrimValidator(&m_Desc, TRIMVALIDATOR_TRIM_BOTH));
	ctrlNote->SetHelpText(_("HIDC_CONFIG_EVENT_DESC"));
	ctrlNote->SetToolTip(_("HIDC_CONFIG_EVENT_DESC"));

	wxStaticBox* boxMethods = new wxStaticBox(this, wxID_ANY, _("IDC_CONFIG_EVENT_SCORING"));

	m_ctrlSubNames = new wxListBox(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize,
		0, nullptr, wxLB_SINGLE|wxLB_SORT);
	m_ctrlSubNames->Bind(wxEVT_COMMAND_LISTBOX_SELECTED, &CDlgConfigEvent::OnLbnSelchangeSubnames, this);
	m_ctrlSubNames->Bind(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, &CDlgConfigEvent::OnLbnDblclkSubnames, this);
	m_ctrlSubNames->SetHelpText(_("HIDC_CONFIG_EVENT_SUBNAMES"));
	m_ctrlSubNames->SetToolTip(_("HIDC_CONFIG_EVENT_SUBNAMES"));

	m_ctrlSubNamesNew = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_SUBNAMES_NEW"),
		wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_ctrlSubNamesNew->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigEvent::OnBnClickedSubNamesNew, this);
	m_ctrlSubNamesNew->SetHelpText(_("HIDC_CONFIG_EVENT_SUBNAMES_NEW"));
	m_ctrlSubNamesNew->SetToolTip(_("HIDC_CONFIG_EVENT_SUBNAMES_NEW"));

	m_ctrlSubNamesEdit = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_SUBNAMES_EDIT"),
		wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_ctrlSubNamesEdit->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigEvent::OnBnClickedSubNamesEdit, this);
	m_ctrlSubNamesEdit->SetHelpText(_("HIDC_CONFIG_EVENT_SUBNAMES_EDIT"));
	m_ctrlSubNamesEdit->SetToolTip(_("HIDC_CONFIG_EVENT_SUBNAMES_EDIT"));

	m_ctrlSubNamesDelete = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_SUBNAMES_DELETE"),
		wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_ctrlSubNamesDelete->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigEvent::OnBnClickedSubNamesDelete, this);
	m_ctrlSubNamesDelete->SetHelpText(_("HIDC_CONFIG_EVENT_SUBNAMES_DELETE"));
	m_ctrlSubNamesDelete->SetToolTip(_("HIDC_CONFIG_EVENT_SUBNAMES_DELETE"));

	wxStaticText* textDefined = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_METHODS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textDefined->Wrap(-1);

	m_ctrlMethods = new wxListBox(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize,
		0, nullptr, 0);
	m_ctrlMethods->Bind(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, &CDlgConfigEvent::OnLbnDblclkMethods, this);
	m_ctrlMethods->Bind(wxEVT_COMMAND_LISTBOX_SELECTED, &CDlgConfigEvent::OnLbnSelchangeMethods, this);
	m_ctrlMethods->SetHelpText(_("HIDC_CONFIG_EVENT_METHODS"));
	m_ctrlMethods->SetToolTip(_("HIDC_CONFIG_EVENT_METHODS"));

	m_ctrlNew = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_NEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlNew->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigEvent::OnBnClickedNew, this);
	m_ctrlNew->SetHelpText(_("HIDC_CONFIG_EVENT_NEW"));
	m_ctrlNew->SetToolTip(_("HIDC_CONFIG_EVENT_NEW"));

	m_ctrlEdit = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlEdit->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigEvent::OnBnClickedEdit, this);
	m_ctrlEdit->SetHelpText(_("HIDC_CONFIG_EVENT_EDIT"));
	m_ctrlEdit->SetToolTip(_("HIDC_CONFIG_EVENT_EDIT"));

	m_ctrlDelete = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlDelete->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigEvent::OnBnClickedDelete, this);
	m_ctrlDelete->SetHelpText(_("HIDC_CONFIG_EVENT_DELETE"));
	m_ctrlDelete->SetToolTip(_("HIDC_CONFIG_EVENT_DELETE"));

	m_ctrlCopy = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_COPY"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlCopy->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigEvent::OnBnClickedCopy, this);
	m_ctrlCopy->SetHelpText(_("HIDC_CONFIG_EVENT_COPY"));
	m_ctrlCopy->SetToolTip(_("HIDC_CONFIG_EVENT_COPY"));
 
	m_ctrlUp = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_MOVE_UP"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlUp->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigEvent::OnBnClickedUp, this);
	m_ctrlUp->SetHelpText(_("HIDC_CONFIG_EVENT_MOVE_UP"));
	m_ctrlUp->SetToolTip(_("HIDC_CONFIG_EVENT_MOVE_UP"));

	m_ctrlDown = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_MOVE_DOWN"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlDown->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigEvent::OnBnClickedDown, this);
	m_ctrlDown->SetHelpText(_("HIDC_CONFIG_EVENT_MOVE_DOWN"));
	m_ctrlDown->SetToolTip(_("HIDC_CONFIG_EVENT_MOVE_DOWN"));

	wxStaticText* textNotDefined = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_UNUSED"),
		wxDefaultPosition, wxDefaultSize, 0);
	textNotDefined->Wrap(-1);

	m_ctrlUnused = new wxListBox(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize,
		0, nullptr, 0);
	m_ctrlUnused->SetHelpText(_("HIDC_CONFIG_EVENT_UNUSED"));
	m_ctrlUnused->SetToolTip(_("HIDC_CONFIG_EVENT_UNUSED"));
	m_ctrlUnused->Enable(false);

	m_ctrlInfo = new wxStaticText(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(-1, wxDLG_UNIT_Y(this, 30)), wxST_NO_AUTORESIZE|wxSUNKEN_BORDER);
	m_ctrlInfo->Bind(wxEVT_COMMAND_LEFT_DCLICK, &CDlgConfigEvent::OnDblclickConfigInfo, this);
	m_ctrlInfo->Wrap(-1);
	m_ctrlInfo->SetHelpText(_("HIDC_CONFIG_EVENT_INFO"));
	m_ctrlInfo->SetToolTip(_("HIDC_CONFIG_EVENT_INFO"));

	wxStaticText* textTitlePoints = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_POINTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textTitlePoints->Wrap(-1);

	m_ctrlPointsList = new wxListBox(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize,
		0, nullptr, wxLB_SINGLE);
	m_ctrlPointsList->Bind(wxEVT_COMMAND_LISTBOX_SELECTED, &CDlgConfigEvent::OnSelchangePoints, this);
	m_ctrlPointsList->Bind(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, &CDlgConfigEvent::OnDblclkPoints, this);
	m_ctrlPointsList->SetHelpText(_("HIDC_CONFIG_EVENT_POINTS"));
	m_ctrlPointsList->SetToolTip(_("HIDC_CONFIG_EVENT_POINTS"));

	m_ctrlPointsNew = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_POINTS_NEW"),
		wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_ctrlPointsNew->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigEvent::OnPointsNew, this);
	m_ctrlPointsNew->SetHelpText(_("HIDC_CONFIG_EVENT_POINTS_NEW"));
	m_ctrlPointsNew->SetToolTip(_("HIDC_CONFIG_EVENT_POINTS_NEW"));

	m_ctrlPointsEdit = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_POINTS_EDIT"),
		wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_ctrlPointsEdit->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigEvent::OnPointsEdit, this);
	m_ctrlPointsEdit->SetHelpText(_("HIDC_CONFIG_EVENT_POINTS_EDIT"));
	m_ctrlPointsEdit->SetToolTip(_("HIDC_CONFIG_EVENT_POINTS_EDIT"));

	m_ctrlPointsDelete = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_POINTS_DELETE"),
		wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_ctrlPointsDelete->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigEvent::OnPointsDelete, this);
	m_ctrlPointsDelete->SetHelpText(_("HIDC_CONFIG_EVENT_POINTS_DELETE"));
	m_ctrlPointsDelete->SetToolTip(_("HIDC_CONFIG_EVENT_POINTS_DELETE"));

	wxStaticText* textMethodNote = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_NOTES"),
		wxDefaultPosition, wxDefaultSize, 0);
	textMethodNote->Wrap(-1);

	m_ctrlNote = new CSpellCheckCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_WORDWRAP);
	m_ctrlNote->SetHelpText(_("HIDC_CONFIG_EVENT_NOTES"));
	m_ctrlNote->SetToolTip(_("HIDC_CONFIG_EVENT_NOTES"));

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* sizerTop = new wxFlexGridSizer(2, 2, 0, 0);
	sizerTop->SetFlexibleDirection(wxBOTH);
	sizerTop->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxBoxSizer* sizerName = new wxBoxSizer(wxHORIZONTAL);
	sizerName->Add(textName, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerName->Add(m_ctrlName, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerName->Add(m_ctrlTable, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerName->Add(ctrlPartners, 0, wxALIGN_CENTER_VERTICAL, 0);

	sizerTop->Add(sizerName, 1, wxEXPAND | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerTop->Add(m_ctrlHasSubnames, 0, wxALIGN_CENTER_VERTICAL);

	wxBoxSizer* sizerNotes = new wxBoxSizer(wxHORIZONTAL);
	sizerNotes->Add(textNote, 0, wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerNotes->Add(ctrlNote, 1, wxEXPAND, 0);

	sizerTop->Add(sizerNotes, 1, wxEXPAND | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerSubname = new wxBoxSizer(wxHORIZONTAL);
	sizerSubname->Add(m_ctrlSubNames, 1, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerBtnSub = new wxBoxSizer(wxVERTICAL);
	sizerBtnSub->Add(m_ctrlSubNamesNew, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 0);
	sizerBtnSub->Add(m_ctrlSubNamesEdit, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND | wxTOP, wxDLG_UNIT_X(this, 2));
	sizerBtnSub->Add(m_ctrlSubNamesDelete, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND | wxTOP, wxDLG_UNIT_X(this, 2));

	sizerSubname->Add(sizerBtnSub, 0, wxEXPAND, wxRESERVE_SPACE_EVEN_IF_HIDDEN);

	sizerTop->Add(sizerSubname, 1, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND | wxTOP, wxDLG_UNIT_X(this, 5));

	bSizer->Add(sizerTop, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxStaticBoxSizer* sizerMethods = new wxStaticBoxSizer(boxMethods, wxVERTICAL);

	wxBoxSizer* sizerDefinitions = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerDefined = new wxBoxSizer(wxVERTICAL);
	sizerDefined->Add(textDefined, 0, 0, 0);
	sizerDefined->Add(m_ctrlMethods, 1, wxEXPAND | wxTOP, wxDLG_UNIT_X(this, 1));

	sizerDefinitions->Add(sizerDefined, 1, wxEXPAND | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerBtns = new wxBoxSizer(wxVERTICAL);
	sizerBtns->Add(m_ctrlNew, 0, 0, 0);
	sizerBtns->Add(m_ctrlEdit, 0, wxTOP, wxDLG_UNIT_X(this, 2));
	sizerBtns->Add(m_ctrlDelete, 0, wxTOP, wxDLG_UNIT_X(this, 2));
	sizerBtns->Add(m_ctrlCopy, 0, wxTOP, wxDLG_UNIT_X(this, 2));
	sizerBtns->Add(m_ctrlUp, 0, wxTOP, wxDLG_UNIT_X(this, 2));
	sizerBtns->Add(m_ctrlDown, 0, wxTOP, wxDLG_UNIT_X(this, 2));

	sizerDefinitions->Add(sizerBtns, 0, wxEXPAND | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerNotDefined = new wxBoxSizer(wxVERTICAL);
	sizerNotDefined->Add(textNotDefined, 0, 0, 0);
	sizerNotDefined->Add(m_ctrlUnused, 1, wxEXPAND | wxTOP, wxDLG_UNIT_X(this, 1));

	sizerDefinitions->Add(sizerNotDefined, 1, wxEXPAND, 0);

	sizerMethods->Add(sizerDefinitions, 1, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 3));

	wxStaticLine* staticLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	sizerMethods->Add(staticLine, 0, wxEXPAND | wxTOP | wxBOTTOM, wxDLG_UNIT_X(this, 3));
	sizerMethods->Add(m_ctrlInfo, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 3));

	wxBoxSizer* sizerPoints = new wxBoxSizer(wxHORIZONTAL);
	sizerPoints->Add(textTitlePoints, 0, wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerPoints->Add(m_ctrlPointsList, 1, wxEXPAND | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerBtnPts = new wxBoxSizer(wxVERTICAL);
	sizerBtnPts->Add(m_ctrlPointsNew, 0, wxEXPAND, 0);
	sizerBtnPts->Add(m_ctrlPointsEdit, 0, wxEXPAND | wxTOP, wxDLG_UNIT_X(this, 2));
	sizerBtnPts->Add(m_ctrlPointsDelete, 0, wxEXPAND | wxTOP, wxDLG_UNIT_X(this, 2));

	sizerPoints->Add(sizerBtnPts, 0, wxEXPAND | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerPoints->Add(textMethodNote, 0, wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerPoints->Add(m_ctrlNote, 1, wxEXPAND, 0);

	sizerMethods->Add(sizerPoints, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 3));

	bSizer->Add(sizerMethods, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	ShowSubNames(false);
	FillMethodList();
	FillControls();

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	IMPLEMENT_ON_INIT(CDlgConfigEvent, m_ctrlName)
}


DEFINE_ON_INIT(CDlgConfigEvent)


CDlgConfigEvent::~CDlgConfigEvent()
{
	ClearFixups();
}


void CDlgConfigEvent::GetFixups(std::vector<ARBConfigActionPtr>& ioFixups)
{
	ioFixups.insert(ioFixups.end(), m_DlgFixup.begin(), m_DlgFixup.end());
	m_DlgFixup.clear();
}


void CDlgConfigEvent::ClearFixups()
{
	m_DlgFixup.clear();
}


void CDlgConfigEvent::ShowSubNames(bool bShow)
{
	if (bShow)
	{
		m_ctrlSubNames->Show(true);
		m_ctrlSubNamesNew->Show(true);
		m_ctrlSubNamesEdit->Show(true);
		m_ctrlSubNamesDelete->Show(true);
	}
	else
	{
		m_ctrlSubNames->Show(false);
		m_ctrlSubNamesNew->Show(false);
		m_ctrlSubNamesEdit->Show(false);
		m_ctrlSubNamesDelete->Show(false);
	}
}


CConfigEventDataScoring* CDlgConfigEvent::GetScoringData(int index) const
{
	return dynamic_cast<CConfigEventDataScoring*>(m_ctrlMethods->GetClientObject(index));
}


CConfigEventDataTitlePoints* CDlgConfigEvent::GetTitleData(int index) const
{
	return dynamic_cast<CConfigEventDataTitlePoints*>(m_ctrlPointsList->GetClientObject(index));
}


CConfigEventDataLifetimePoints* CDlgConfigEvent::GetLifetimeData(int index) const
{
	return dynamic_cast<CConfigEventDataLifetimePoints*>(m_ctrlPointsList->GetClientObject(index));
}


CConfigEventDataPlaceInfo* CDlgConfigEvent::GetPlacementData(int index) const
{
	return dynamic_cast<CConfigEventDataPlaceInfo*>(m_ctrlPointsList->GetClientObject(index));
}


wxString CDlgConfigEvent::GetListName(ARBConfigScoringPtr const& inScoring) const
{
	wxString all = _("IDS_ALL");
	wxString str;
	if (inScoring->GetDivision() == WILDCARD_DIVISION)
		str = all;
	else
		str = StringUtil::stringWX(inScoring->GetDivision());
	str += L" / ";
	if (inScoring->GetLevel() == WILDCARD_LEVEL)
		str += all;
	else
		str += StringUtil::stringWX(inScoring->GetLevel());
	wxString validStr = StringUtil::stringWX(ARBDate::GetValidDateString(inScoring->GetValidFrom(), inScoring->GetValidTo()));
	if (0 < validStr.length())
	{
		str += L" ";
		str += validStr;
	}
	return str;
}


void CDlgConfigEvent::EnableSubnameControls()
{
	bool bEnable = false;
	if (wxNOT_FOUND != m_ctrlSubNames->GetSelection())
		bEnable = true;
	m_ctrlSubNamesEdit->Enable(bEnable);
	m_ctrlSubNamesDelete->Enable(bEnable);
}


void CDlgConfigEvent::EditSubname()
{
	int idx = m_ctrlSubNames->GetSelection();
	if (wxNOT_FOUND != idx)
	{
		std::wstring name = StringUtil::stringW(m_ctrlSubNames->GetString(idx));
		CDlgName dlg(name, this);
		if (wxID_OK == dlg.ShowModal())
		{
			m_ctrlSubNames->Delete(idx);
			idx = m_ctrlSubNames->Append(StringUtil::stringWX(dlg.Name()));
			m_ctrlSubNames->SetSelection(idx);
			UpdateSubnames();
			EnableSubnameControls();
		}
	}
}


void CDlgConfigEvent::UpdateSubnames()
{
	int idxMethod = m_ctrlMethods->GetSelection();
	if (wxNOT_FOUND == idxMethod)
		return;

	CConfigEventDataScoring* pScoringData = GetScoringData(m_idxMethod);
	ARBConfigScoringPtr pScoring = pScoringData->GetData();

	if (m_ctrlHasSubnames->IsChecked())
	{
		std::set<std::wstring> subNames;
		int nCount = m_ctrlSubNames->GetCount();
		for (int i = 0; i < nCount; ++i)
		{
			std::wstring str = StringUtil::stringW(m_ctrlSubNames->GetString(i));
			str = StringUtil::Trim(str);
			if (!str.empty())
				subNames.insert(str);
		}
		pScoring->SetSubNames(subNames);
	}
}

void CDlgConfigEvent::EnablePointsControls()
{
	bool bEnable = false;
	if (wxNOT_FOUND != m_ctrlPointsList->GetSelection())
		bEnable = true;
	m_ctrlPointsEdit->Enable(bEnable);
	m_ctrlPointsDelete->Enable(bEnable);
}


void CDlgConfigEvent::FillControls()
{
	bool bEnable = false;
	int idxMethod = m_ctrlMethods->GetSelection();
	m_ctrlSubNames->Clear();
	m_ctrlInfo->SetLabel(L"");
	m_ctrlPointsList->Clear();
	m_ctrlNote->SetValue(L"");
	if (wxNOT_FOUND != idxMethod)
	{
		CConfigEventDataScoring* pScoringData = GetScoringData(idxMethod);
		if (pScoringData)
		{
			bEnable = true;
			ARBConfigScoringPtr pScoring = pScoringData->GetData();
			m_ctrlTable->SetValue(pScoring->HasTable());
			m_ctrlHasSubnames->SetValue(pScoring->HasSubNames());
			if (pScoring->HasSubNames())
			{
				std::set<std::wstring> subNames;
				pScoring->GetSubNames(subNames);
				for (std::set<std::wstring>::const_iterator iter = subNames.begin();
					iter != subNames.end();
					++iter)
				{
					m_ctrlSubNames->Append(StringUtil::stringWX(*iter));
				}
			}
			// Get info
			{
				std::wstring str1, str2;
				fmt::wmemory_buffer info;
				ARBScoringStyle style = pScoring->GetScoringStyle();
				str1 = _("IDS_CONFIGEVENT_STYLE");
				fmt::format_to(info, L"{}: {}\n", str1, ARBConfigScoring::GetScoringStyleStr(style));
				// The following strings should be the same as they are in
				// the Method Configuration dialog.
				switch (style)
				{
				case ARBScoringStyle::Unknown:
					break;
				case ARBScoringStyle::FaultsThenTime:
				case ARBScoringStyle::Faults100ThenTime:
				case ARBScoringStyle::Faults200ThenTime:
					str1 = _("IDS_CONFIGEVENT_TIMEFAULTMULT");
					fmt::format_to(info, L"{}: {}", str1, pScoring->TimeFaultMultiplier());
					break;
				case ARBScoringStyle::OCScoreThenTime:
					str1 = _("IDS_CONFIGEVENT_REQOPEN");
					str2 = _("IDS_CONFIGEVENT_REQCLOSE");
					fmt::format_to(info, L"{}: {}; {}: {}", str1, pScoring->GetRequiredOpeningPoints(), str2, pScoring->GetRequiredClosingPoints());
					if (pScoring->SubtractTimeFaultsFromScore())
					{
						str1 = _("IDS_CONFIGEVENT_TF_FROMSCORE");
						fmt::format_to(info, L"; {}", str1);
						str1 = _("IDS_CONFIGEVENT_TIMEFAULTMULT");
						fmt::format_to(info, L"; {}: {}", str1, pScoring->TimeFaultMultiplier());
						if (pScoring->ComputeTimeFaultsUnder())
						{
							str1 = _("IDS_CONFIGEVENT_TF_UNDER");
							fmt::format_to(info, L"; {}", str1);
						}
						if (pScoring->ComputeTimeFaultsOver())
						{
							str1 = _("IDS_CONFIGEVENT_TF_OVER");
							fmt::format_to(info, L"; {}", str1);
						}
					}
					break;
				case ARBScoringStyle::ScoreThenTime:
					str1 = _("IDS_POINTS");
					fmt::format_to(info, L"{}: {}", str1, pScoring->GetRequiredOpeningPoints());
					if (pScoring->SubtractTimeFaultsFromScore())
					{
						str1 = _("IDS_CONFIGEVENT_TF_FROMSCORE");
						fmt::format_to(info, L"; {}", str1);
						str1 = _("IDS_CONFIGEVENT_TIMEFAULTMULT");
						fmt::format_to(info, L"; {}: {}", str1, pScoring->TimeFaultMultiplier());
						if (pScoring->ComputeTimeFaultsUnder())
						{
							str1 = _("IDS_CONFIGEVENT_TF_UNDER");
							fmt::format_to(info, L"; {}", str1);
						}
						if (pScoring->ComputeTimeFaultsOver())
						{
							str1 = _("IDS_CONFIGEVENT_TF_OVER");
							fmt::format_to(info, L"; {}", str1);
						}
					}
					break;
				case ARBScoringStyle::TimePlusFaults:
					str1 = _("IDS_CONFIGEVENT_TIMEFAULTMULT");
					fmt::format_to(info, L"{}: {}", str1, pScoring->TimeFaultMultiplier());
					if (pScoring->QsMustBeClean())
					{
						// This string is slightly different: Just dropped
						// the 'Time+Fault' at start.
						str1 = _("IDS_CONFIGEVENT_CLEANQ");
						fmt::format_to(info, L"; {}", str1);
					}
					if (pScoring->ComputeTimeFaultsUnder()
						|| pScoring->ComputeTimeFaultsOver())
					{
						str1 = _("IDS_CONFIGEVENT_TIMEFAULTMULT");
						fmt::format_to(info, L"; {}: {}", str1, pScoring->TimeFaultMultiplier());
					}
					if (pScoring->ComputeTimeFaultsUnder())
					{
						str1 = _("IDS_CONFIGEVENT_TF_UNDER");
						fmt::format_to(info, L"; {}", str1);
					}
					if (pScoring->ComputeTimeFaultsOver())
					{
						str1 = _("IDS_CONFIGEVENT_TF_OVER");
						fmt::format_to(info, L"; {}", str1);
					}
					break;
				case ARBScoringStyle::TimeNoPlaces:
					str1.clear();
					break;
				}
				if (pScoring->DropFractions())
				{
					str1 = _("IDS_CONFIGEVENT_DROPFRAC");
					fmt::format_to(info, L"; {}", str1);
				}
				if (pScoring->HasBonusTitlePts())
				{
					str1 = _("IDS_CONFIGEVENT_BONUSTITLE");
					fmt::format_to(info, L"; {}", str1);
				}
				if (pScoring->HasSuperQ())
				{
					str1 = _("IDS_CONFIGEVENT_SUPERQ");
					fmt::format_to(info, L"; {}", str1);
				}
				if (pScoring->HasSpeedPts())
				{
					str1 = _("IDS_CONFIGEVENT_SPEEDPTS");
					fmt::format_to(info, L"; {}", str1);
					if (0 < pScoring->GetPlaceInfo().size())
					{
						fmt::format_to(info, L" [");
						int idx = 0;
						for (ARBConfigPlaceInfoList::iterator iter = pScoring->GetPlaceInfo().begin();
							iter != pScoring->GetPlaceInfo().end();
							++idx, ++iter)
						{
							if (0 < idx)
								fmt::format_to(info, L", ");
							fmt::format_to(info, L"{}={}", (*iter)->GetPlace(), (*iter)->GetValue());
						}
						fmt::format_to(info, L"]");
					}
				}
				m_ctrlInfo->SetLabel(StringUtil::stringWX(fmt::to_string(info)));
			}
			// Take care of title points
			FillTitlePoints(pScoring);
			// And the note.
			std::wstring str = pScoring->GetNote();
			m_ctrlNote->SetValue(StringUtil::stringWX(str));
		}
	}
	m_ctrlTable->Enable(bEnable);
	m_ctrlHasSubnames->Enable(bEnable);
	ShowSubNames(bEnable);
	m_ctrlEdit->Enable(bEnable);
	m_ctrlDelete->Enable(bEnable);
	m_ctrlCopy->Enable(bEnable);
	m_ctrlUp->Enable(bEnable && 1 < m_ctrlMethods->GetCount() && 0 != idxMethod);
	m_ctrlDown->Enable(bEnable && 1 < m_ctrlMethods->GetCount() && static_cast<int>(m_ctrlMethods->GetCount()) - 1 != idxMethod);
	int idxTitle = m_ctrlPointsList->GetSelection();
	m_ctrlPointsList->Enable(bEnable);
	m_ctrlPointsNew->Enable(bEnable);
	m_ctrlPointsEdit->Enable(bEnable && 0 <= idxTitle);
	m_ctrlPointsDelete->Enable(bEnable && 0 <= idxTitle);
	m_ctrlNote->Enable(bEnable);
}


void CDlgConfigEvent::FillMethodList()
{
	m_idxMethod = m_ctrlMethods->GetSelection();
	m_ctrlMethods->Clear();
	wxString str;
	for (ARBConfigScoringList::iterator iter = m_Scorings.begin();
		iter != m_Scorings.end();
		++iter)
	{
		ARBConfigScoringPtr pScoring = (*iter);
		str = GetListName(pScoring);
		int index = m_ctrlMethods->Append(str);
		m_ctrlMethods->SetClientObject(index, new CConfigEventDataScoring(pScoring));
	}
	m_ctrlMethods->SetSelection(m_idxMethod);

	m_ctrlUnused->Clear();
	for (ARBConfigDivisionList::const_iterator iterDiv = m_pVenue->GetDivisions().begin();
		iterDiv != m_pVenue->GetDivisions().end();
		++iterDiv)
	{
		for (ARBConfigLevelList::const_iterator iterLevel = (*iterDiv)->GetLevels().begin();
			iterLevel != (*iterDiv)->GetLevels().end();
			++iterLevel)
		{
			if (!m_Scorings.FindEvent((*iterDiv)->GetName(), (*iterLevel)->GetName(), ARBDate::Today()))
			{
				str = StringUtil::stringWX((*iterDiv)->GetName());
				str += L" / ";
				str += StringUtil::stringWX((*iterLevel)->GetName());
				m_ctrlUnused->Append(str);
			}
			// Remember, configuration doesn't do sublevels.
		}
	}
}


void CDlgConfigEvent::FillTitlePoints(ARBConfigScoringPtr const& inScoring)
{
	ARBConfigTitlePointsPtr pOldTitle;
	ARBConfigLifetimePointsPtr pOldLifetime;
	ARBConfigPlaceInfoPtr pOldPlacement;
	int idxTitle = m_ctrlPointsList->GetSelection();
	if (wxNOT_FOUND != idxTitle)
	{
		CConfigEventDataTitlePoints* pTitleData = GetTitleData(idxTitle);
		if (pTitleData)
		{
			pOldTitle = pTitleData->GetData();
		}
		CConfigEventDataLifetimePoints* pLifeData = GetLifetimeData(idxTitle);
		if (pLifeData)
		{
			pOldLifetime = pLifeData->GetData();
		}
		CConfigEventDataPlaceInfo* pPlacement = GetPlacementData(idxTitle);
		if (pPlacement)
		{
			pOldPlacement = pPlacement->GetData();
		}
	}
	m_ctrlPointsList->Clear();
	for (ARBConfigTitlePointsList::const_iterator iter = inScoring->GetTitlePoints().begin();
		iter != inScoring->GetTitlePoints().end();
		++iter)
	{
		ARBConfigTitlePointsPtr pTitle = (*iter);
		int idx = m_ctrlPointsList->Append(StringUtil::stringWX(pTitle->GetGenericName()));
		if (wxNOT_FOUND != idx)
		{
			m_ctrlPointsList->SetClientObject(idx, new CConfigEventDataTitlePoints(pTitle));
			if (pOldTitle == pTitle)
				m_ctrlPointsList->SetSelection(idx);
		}
	}
	for (ARBConfigLifetimePointsList::const_iterator iter2 = inScoring->GetLifetimePoints().begin();
		iter2 != inScoring->GetLifetimePoints().end();
		++iter2)
	{
		ARBConfigLifetimePointsPtr pLife = (*iter2);
		int idx = m_ctrlPointsList->Append(StringUtil::stringWX(pLife->GetGenericName()));
		if (wxNOT_FOUND != idx)
		{
			m_ctrlPointsList->SetClientObject(idx, new CConfigEventDataLifetimePoints(pLife));
			if (pOldLifetime == pLife)
				m_ctrlPointsList->SetSelection(idx);
		}
	}
	for (ARBConfigPlaceInfoList::const_iterator iter3 = inScoring->GetPlacements().begin();
		iter3 != inScoring->GetPlacements().end();
		++iter3)
	{
		ARBConfigPlaceInfoPtr pPlace = (*iter3);
		int idx = m_ctrlPointsList->Append(StringUtil::stringWX(pPlace->GetGenericName()));
		if (wxNOT_FOUND != idx)
		{
			m_ctrlPointsList->SetClientObject(idx, new CConfigEventDataPlaceInfo(pPlace));
			if (pOldPlacement == pPlace)
				m_ctrlPointsList->SetSelection(idx);
		}
	}
	m_ctrlPointsEdit->Enable(false);
	m_ctrlPointsDelete->Enable(false);
}


bool CDlgConfigEvent::SaveControls()
{
	// Save the last selected method.
	if (0 <= m_idxMethod)
	{
		CConfigEventDataScoring* pScoringData = GetScoringData(m_idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		// Point/faults are already up-to-date.
		std::wstring str = StringUtil::stringW(m_ctrlNote->GetValue());
		pScoring->SetNote(StringUtil::TrimRight(str));
	}
	return true;
}


void CDlgConfigEvent::EditMethod()
{
	int idxMethod = m_ctrlMethods->GetSelection();
	if (wxNOT_FOUND != idxMethod)
	{
		CConfigEventDataScoring* pScoringData = GetScoringData(idxMethod);
		CDlgConfigEventMethod dlg(m_pVenue, pScoringData->GetData(), this);
		if (wxID_OK == dlg.ShowModal())
		{
			FillMethodList();
			FillControls();
		}
	}
}


void CDlgConfigEvent::EditPoints()
{
	int idxMethod = m_ctrlMethods->GetSelection();
	int idx = m_ctrlPointsList->GetSelection();
	if (wxNOT_FOUND != idxMethod && wxNOT_FOUND != idx)
	{
		CConfigEventDataScoring* pScoringData = GetScoringData(idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		CConfigEventDataTitlePoints* pData1 = GetTitleData(idx);
		CConfigEventDataLifetimePoints* pData2 = GetLifetimeData(idx);
		CConfigEventDataPlaceInfo* pData3 = GetPlacementData(idx);
		ARBConfigTitlePointsPtr pTitle;
		ARBConfigLifetimePointsPtr pLife;
		ARBConfigPlaceInfoPtr pPlace;
		if (pData1)
			pTitle = pData1->GetData();
		if (pData2)
			pLife = pData2->GetData();
		if (pData3)
			pPlace = pData3->GetData();
		if (pScoring && (pTitle || pLife || pPlace))
		{
			double value = 0.0;
			double points = 0.0;
			ARBTitlePointType type = ARBTitlePointType::Normal;
			ARBPointsType typeNorm = ARBPointsType::Normal;
			if (pTitle)
			{
				value = pTitle->GetFaults();
				points = pTitle->GetPoints();
				type = ARBTitlePointType::Normal;
				typeNorm = pTitle->GetCalc()->GetType();
			}
			else if (pLife)
			{
				value = pLife->GetFaults();
				points = pLife->GetPoints();
				type = ARBTitlePointType::Lifetime;
				typeNorm = ARBPointsType::Max;
			}
			else
			{
				value = pPlace->GetPlace();
				points = pPlace->GetValue();
				type = ARBTitlePointType::Placement;
				typeNorm = ARBPointsType::Max;
			}
			int rc = 0;
			double dlgFaults = 0.0;
			short dlgPlace = 0;
			bool dlgUseSpeedPts = false;
			double dlgPoints = 0.0;
			wxString dlgLifetimeName;
			ARBTitlePointType dlgType = ARBTitlePointType::Normal;
			ARBPointsType dlgTypeNormal = ARBPointsType::Normal;
			if (pLife)
			{
				CDlgConfigTitlePoints dlg(m_pVenue, pLife);
				rc = dlg.ShowModal();
				dlgFaults = dlg.Faults();
				dlgPlace = dlg.Place();
				dlgUseSpeedPts = dlg.UseSpeedPts();
				dlgPoints = dlg.Points();
				dlgLifetimeName = dlg.LifetimeName();
				dlgType = dlg.Type();
				dlgTypeNormal = dlg.TypeNormal();
			}
			else
			{
				CDlgConfigTitlePoints dlg(m_pVenue, value, points, type, typeNorm);
				rc = dlg.ShowModal();
				dlgFaults = dlg.Faults();
				dlgPlace = dlg.Place();
				dlgUseSpeedPts = dlg.UseSpeedPts();
				dlgPoints = dlg.Points();
				dlgLifetimeName = dlg.LifetimeName();
				dlgType = dlg.Type();
				dlgTypeNormal = dlg.TypeNormal();
			}
			if (wxID_OK == rc)
			{
				if (type != dlgType
				|| typeNorm != dlgTypeNormal
				|| (pTitle && pTitle->GetFaults() != dlgFaults)
				|| (pLife && (pLife->GetName() != dlgLifetimeName || pLife->UseSpeedPts() != dlgUseSpeedPts || pLife->GetFaults() != dlgFaults))
				|| (pPlace && pPlace->GetPlace() != dlgPlace))
				{
					// Clean up.
					if (type == dlgType)
					{
						if (pTitle)
							pScoring->GetTitlePoints().DeleteTitlePoints(pTitle->GetCalc()->GetType(), pTitle->GetFaults());
						else if (pLife)
						{
							pScoring->GetLifetimePoints().DeleteLifetimePoints(pLife->GetName(), pLife->GetFaults());
						}
						else
							pScoring->GetPlacements().DeletePlaceInfo(pPlace->GetPlace());
					}
					bool bOk = false;
					switch (dlgType)
					{
					case ARBTitlePointType::Normal:
						{
							ARBCalcPointsPtr calc = pScoring->GetTitlePoints().GetCalc();
							if (!calc || dlgTypeNormal != calc->GetType())
							{
								bOk = true;
								pScoring->GetTitlePoints().SetType(dlgTypeNormal);
								calc = pScoring->GetTitlePoints().GetCalc();
							}
							if (!calc || calc->AllowConfiguration())
								bOk = pScoring->GetTitlePoints().AddTitlePoints(dlgPoints, dlgFaults);
						}
						if (!bOk)
							wxMessageBox(_("IDS_TITLEPTS_EXISTS"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
						break;
					case ARBTitlePointType::Lifetime:
						bOk = pScoring->GetLifetimePoints().AddLifetimePoints(StringUtil::stringW(dlgLifetimeName), dlgUseSpeedPts, dlgPoints, dlgFaults);
						if (!bOk)
							wxMessageBox(_("IDS_TITLEPTS_EXISTS"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
						break;
					case ARBTitlePointType::Placement:
						bOk = pScoring->GetPlacements().AddPlaceInfo(dlgPlace, dlgPoints, true);
						if (!bOk)
							wxMessageBox(_("IDS_TITLEPTS_EXISTS"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
						break;
					case ARBTitlePointType::Max:
						assert(0);
						break;
					}
					if (bOk && type != dlgType)
					{
						if (pTitle)
							pScoring->GetTitlePoints().DeleteTitlePoints(pTitle->GetCalc()->GetType(), pTitle->GetFaults());
						else if (pLife)
							pScoring->GetLifetimePoints().DeleteLifetimePoints(pLife->GetName(), pLife->GetFaults());
						else
							pScoring->GetPlacements().DeletePlaceInfo(pPlace->GetPlace());
					}
				}
				else
				{
					if (pTitle)
						pTitle->SetPoints(dlgPoints);
					else if (pLife)
						pLife->SetPoints(dlgPoints);
					else
						pPlace->SetValue(dlgPoints);
				}
				FillTitlePoints(pScoring);
				EnablePointsControls();
			}
		}
	}
}


void CDlgConfigEvent::OnClickedTable(wxCommandEvent& evt)
{
	int idxMethod = m_ctrlMethods->GetSelection();
	if (wxNOT_FOUND == idxMethod)
		return;

	CConfigEventDataScoring* pScoringData = GetScoringData(m_idxMethod);
	ARBConfigScoringPtr pScoring = pScoringData->GetData();

	pScoring->SetHasTable(m_ctrlTable->IsChecked());
}


void CDlgConfigEvent::OnClickedSubNames(wxCommandEvent& evt)
{
	int idxMethod = m_ctrlMethods->GetSelection();
	if (wxNOT_FOUND == idxMethod)
		return;

	CConfigEventDataScoring* pScoringData = GetScoringData(m_idxMethod);
	ARBConfigScoringPtr pScoring = pScoringData->GetData();

	bool bHasSubnames = m_ctrlHasSubnames->IsChecked();
	pScoring->SetHasSubNames(bHasSubnames);
	ShowSubNames(bHasSubnames);

	EnableSubnameControls();
}


void CDlgConfigEvent::OnLbnSelchangeSubnames(wxCommandEvent& evt)
{
	EnableSubnameControls();
}


void CDlgConfigEvent::OnLbnDblclkSubnames(wxCommandEvent& evt)
{
	EditSubname();
}


void CDlgConfigEvent::OnBnClickedSubNamesNew(wxCommandEvent& evt)
{
	CDlgName dlg(L"", this);
	if (wxID_OK == dlg.ShowModal())
	{
		int idx = m_ctrlSubNames->Append(StringUtil::stringWX(dlg.Name()));
		m_ctrlSubNames->SetSelection(idx);
		UpdateSubnames();
		EnableSubnameControls();
	}
}


void CDlgConfigEvent::OnBnClickedSubNamesEdit(wxCommandEvent& evt)
{
	EditSubname();
}


void CDlgConfigEvent::OnBnClickedSubNamesDelete(wxCommandEvent& evt)
{
	unsigned int idx = m_ctrlSubNames->GetSelection();
	if (wxNOT_FOUND != idx)
	{
		m_ctrlSubNames->Delete(idx);
		if (idx == m_ctrlSubNames->GetCount())
			--idx;
		m_ctrlSubNames->SetSelection(idx);
		m_ctrlSubNamesEdit->Enable(false);
		m_ctrlSubNamesDelete->Enable(false);
	}
}


void CDlgConfigEvent::OnLbnDblclkMethods(wxCommandEvent& evt)
{
	EditMethod();
}


void CDlgConfigEvent::OnLbnSelchangeMethods(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	SaveControls();
	m_idxMethod = m_ctrlMethods->GetSelection();
	FillControls();
}


void CDlgConfigEvent::OnBnClickedNew(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	SaveControls();
	ARBConfigScoringPtr pScoring = m_Scorings.AddScoring();
	wxString str = GetListName(pScoring);
	m_idxMethod = m_ctrlMethods->Append(str);
	m_ctrlMethods->SetClientObject(m_idxMethod, new CConfigEventDataScoring(pScoring));
	m_ctrlMethods->SetSelection(m_idxMethod);
	FillMethodList();
	FillControls();
}


void CDlgConfigEvent::OnBnClickedEdit(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	EditMethod();
}


void CDlgConfigEvent::OnBnClickedDelete(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	int idxMethod = m_ctrlMethods->GetSelection();
	if (wxNOT_FOUND != idxMethod)
	{
		if (m_idxMethod == idxMethod)
		{
			m_idxMethod = -1;
			m_ctrlMethods->SetSelection(-1);
		}
		CConfigEventDataScoring* pScoringData = GetScoringData(idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		for (ARBConfigScoringList::iterator iter = m_Scorings.begin();
			iter != m_Scorings.end();
			++iter)
		{
			if ((*iter) == pScoring)
			{
				m_Scorings.erase(iter);
				break;
			}
		}
		FillMethodList();
		FillControls();
	}
}


void CDlgConfigEvent::OnBnClickedCopy(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	int idxMethod = m_ctrlMethods->GetSelection();
	if (wxNOT_FOUND != idxMethod)
	{
		CConfigEventDataScoring* pScoringData = GetScoringData(idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		ARBConfigScoringPtr pNewScoring = m_Scorings.AddScoring();
		*pNewScoring = *pScoring;
		wxString str = GetListName(pNewScoring);
		m_idxMethod = m_ctrlMethods->Append(str);
		m_ctrlMethods->SetClientObject(m_idxMethod, new CConfigEventDataScoring(pNewScoring));
		m_ctrlMethods->SetSelection(m_idxMethod);
		FillMethodList();
		FillControls();
	}
}


void CDlgConfigEvent::OnBnClickedUp(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	int idxMethod = m_ctrlMethods->GetSelection();
	if (wxNOT_FOUND != idxMethod && 0 != idxMethod)
	{
		CConfigEventDataScoring* pScoringData = GetScoringData(idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		for (ARBConfigScoringList::iterator iter = m_Scorings.begin();
			iter != m_Scorings.end();
			++iter)
		{
			if ((*iter) == pScoring)
			{
				iter = m_Scorings.erase(iter);
				--iter;
				m_Scorings.insert(iter, pScoring);
				m_ctrlMethods->SetSelection(idxMethod-1);
				break;
			}
		}
		FillMethodList();
		FillControls();
	}
}


void CDlgConfigEvent::OnBnClickedDown(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	int idxMethod = m_ctrlMethods->GetSelection();
	if (wxNOT_FOUND != idxMethod && idxMethod < static_cast<int>(m_ctrlMethods->GetCount()) - 1)
	{
		CConfigEventDataScoring* pScoringData = GetScoringData(idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		for (ARBConfigScoringList::iterator iter = m_Scorings.begin();
			iter != m_Scorings.end();
			++iter)
		{
			if ((*iter) == pScoring)
			{
				iter = m_Scorings.erase(iter);
				++iter;
				m_Scorings.insert(iter, pScoring);
				m_ctrlMethods->SetSelection(idxMethod+1);
				break;
			}
		}
		FillMethodList();
		FillControls();
	}
}


void CDlgConfigEvent::OnDblclickConfigInfo(wxCommandEvent& evt)
{
	EditMethod();
}


void CDlgConfigEvent::OnSelchangePoints(wxCommandEvent& evt)
{
	EnablePointsControls();
}


void CDlgConfigEvent::OnDblclkPoints(wxCommandEvent& evt)
{
	EditPoints();
}


void CDlgConfigEvent::OnPointsNew(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	int idxMethod = m_ctrlMethods->GetSelection();
	if (wxNOT_FOUND != idxMethod)
	{
		CConfigEventDataScoring* pScoringData = GetScoringData(idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		if (pScoring)
		{
			CDlgConfigTitlePoints dlg(m_pVenue, 0.0, 0.0, ARBTitlePointType::Normal, ARBPointsType::Normal, this);
			if (wxID_OK == dlg.ShowModal())
			{
				// The only reason this fails is if the faults entry exists.
				switch (dlg.Type())
				{
				case ARBTitlePointType::Normal:
					if (!pScoring->GetTitlePoints().AddTitlePoints(dlg.Points(), dlg.Faults()))
						wxMessageBox(_("IDS_TITLEPTS_EXISTS"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
					break;
				case ARBTitlePointType::Lifetime:
					if (!pScoring->GetLifetimePoints().AddLifetimePoints(StringUtil::stringW(dlg.LifetimeName()), dlg.UseSpeedPts(), dlg.Points(), dlg.Faults()))
						wxMessageBox(_("IDS_TITLEPTS_EXISTS"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
					break;
				case ARBTitlePointType::Placement:
					if (!pScoring->GetPlacements().AddPlaceInfo(dlg.Place(), dlg.Points(), true))
						wxMessageBox(_("IDS_TITLEPTS_EXISTS"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
					break;
				case ARBTitlePointType::Max:
					assert(0);
					break;
				}
				FillTitlePoints(pScoring);
				EnablePointsControls();
			}
		}
	}
}


void CDlgConfigEvent::OnPointsEdit(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	EditPoints();
}


void CDlgConfigEvent::OnPointsDelete(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	int idxMethod = m_ctrlMethods->GetSelection();
	unsigned int idx = m_ctrlPointsList->GetSelection();
	if (wxNOT_FOUND != idxMethod && wxNOT_FOUND != idx)
	{
		CConfigEventDataScoring* pScoringData = GetScoringData(idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		CConfigEventDataTitlePoints* pData1 = GetTitleData(idx);
		CConfigEventDataLifetimePoints* pData2 = GetLifetimeData(idx);
		CConfigEventDataPlaceInfo* pData3 = GetPlacementData(idx);
		ARBConfigTitlePointsPtr pTitle;
		ARBConfigLifetimePointsPtr pLife;
		ARBConfigPlaceInfoPtr pPlace;
		if (pData1)
			pTitle = pData1->GetData();
		if (pData2)
			pLife = pData2->GetData();
		if (pData3)
			pPlace = pData3->GetData();
		if (pScoring && (pTitle || pLife || pPlace))
		{
			if (pTitle)
				pScoring->GetTitlePoints().DeleteTitlePoints(pTitle->GetCalc()->GetType(), pTitle->GetFaults());
			else if (pLife)
				pScoring->GetLifetimePoints().DeleteLifetimePoints(pLife->GetName(), pLife->GetFaults());
			else
				pScoring->GetPlacements().DeletePlaceInfo(pPlace->GetPlace());
			m_ctrlPointsList->Delete(idx);
			if (idx == m_ctrlPointsList->GetCount())
				--idx;
			m_ctrlPointsList->SetSelection(idx);
			EnablePointsControls();
		}
	}
}


static ARBDate DateEndPoints(ARBDate from1, ARBDate to1, ARBDate from2, ARBDate to2, bool bFirst)
{
	std::vector<ARBDate> dates;
	if (from1.IsValid())
		dates.push_back(from1);
	if (to1.IsValid())
		dates.push_back(to1);
	if (from2.IsValid())
		dates.push_back(from2);
	if (to2.IsValid())
		dates.push_back(to2);
	ARBDate date;
	if (1 <= dates.size())
	{
		date = dates[0];
		for (size_t n = 1; n < dates.size(); ++n)
		{
			if (bFirst)
			{
				if (dates[n] < date)
					date = dates[n];
			}
			else
			{
				if (dates[n] > date)
					date = dates[n];
			}
		}
	}
	return date;
}


void CDlgConfigEvent::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;
	if (!SaveControls())
		return;

	// Check if there is any overlap.
	bool bOverlap = false;
	{
		// No need to clone, we can just copy the pointers
		ARBConfigScoringList scorings(m_Scorings);
		while (!bOverlap && 1 < scorings.size())
		{
			// Extract all similar methods.
			ARBConfigScoringList items;
			ARBConfigScoringList::iterator iter = scorings.begin();
			ARBConfigScoringPtr pScoring = items.AddScoring();
			*pScoring = *(*iter);
			scorings.erase(scorings.begin());
			iter = scorings.begin();
			while (iter != scorings.end())
			{
				ARBConfigScoringPtr pScoring2 = *iter;
				if (pScoring->GetDivision() == pScoring2->GetDivision()
				&& pScoring->GetLevel() == pScoring2->GetLevel())
				{
					pScoring2 = items.AddScoring();
					*pScoring2 = *(*iter);
					iter = scorings.erase(iter);
				}
				else
					++iter;
			}
			// If we have more than one, check overlap.
			while (!bOverlap && 1 < items.size())
			{
				iter = items.begin();
				ARBConfigScoringPtr pScore = *iter;
				for (++iter; !bOverlap && iter != items.end(); ++iter)
				{
					ARBConfigScoringPtr pScoring2 = *iter;
					ARBDate from1 = pScore->GetValidFrom();
					ARBDate to1 = pScore->GetValidTo();
					ARBDate from2 = pScoring2->GetValidFrom();
					ARBDate to2 = pScoring2->GetValidTo();
					ARBDate low = DateEndPoints(from1, to1, from2, to2, true);
					if (!low.IsValid())
					{
						// If date isn't valid, none of the dates are.
						bOverlap = true;
						break;
					}
					ARBDate hi = DateEndPoints(from1, to1, from2, to2, false);
					--low; // Push valid dates off by 1 so they are not in current range
					++hi;
					if (!from1.IsValid())
						from1 = low;
					if (!to1.IsValid())
						to1 = hi;
					if (!from2.IsValid())
						from2 = low;
					if (!to2.IsValid())
						to2 = hi;
					// Now that we've tweaked the dates, we can do plain old
					// isBetween comparisons. This also means that if both
					// methods have no date set on either the from or to dates,
					// it will be properly flagged (since they are equal)
					if (from1.isBetween(from2, to2) || to1.isBetween(from2, to2))
					{
						bOverlap = true;
					}
				}
				items.erase(items.begin());
			}
		}
	}
	if (bOverlap)
	{
		if (wxYES != wxMessageBox(_("IDS_CONFIGEVENT_OVERLAPDATES"), wxMessageBoxCaptionStr, wxYES_NO | wxNO_DEFAULT | wxCENTRE | wxICON_WARNING))
			return;
	}

	ClearFixups();
	std::wstring name(StringUtil::stringW(m_Name));
	if (m_pEvent->GetName() != name)
	{
		if (m_pVenue->GetEvents().FindEvent(name))
		{
			wxMessageBox(_("IDS_NAME_IN_USE"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_WARNING);
			m_ctrlName->SetFocus();
			return;
		}
		if (!m_bNewEntry)
			m_DlgFixup.push_back(ARBConfigActionRenameEvent::New(0, m_pVenue->GetName(), m_pEvent->GetName(), name));
		m_pEvent->SetName(name);
	}
	// m_Book is only valid when editing an existing entry.
	/*
	if (!m_bNewEntry)
	{
		switch (CDlgConfigure::CheckExistingRuns(m_pDoc, m_Book->GetDogs(),
			m_pVenue, m_pEvent->GetName(), m_Scorings, m_DlgFixup))
		{
		case CDlgConfigure::eCancelChanges:
			EndDialog(IDCANCEL);
			return;
		case CDlgConfigure::eDoNotDoIt:
			return;
		case CDlgConfigure::eNoChange:
		case CDlgConfigure::eDoIt:
			break;
		}
	}
	*/

	m_pEvent->SetDesc(StringUtil::stringW(m_Desc));
	m_pEvent->SetHasPartner(m_bHasPartners);
	// No need to clone them, just move them.
	m_pEvent->GetScorings() = m_Scorings;
	EndDialog(wxID_OK);
}
