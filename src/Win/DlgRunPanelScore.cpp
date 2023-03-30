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
 * 2008-02-01 Make 'Notes' button change selection.
 * 2008-11-21 Enable tallying runs that have only lifetime points.
 * 2007-12-03 Refresh judge list after invoking 'notes' button.
 * 2007-07-01 Fixed a problem with table flag on a run.
 * 2006-11-05 Trim Divisions/Levels if no events are available on date.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-13 Added direct access to Notes dialog.
 * 2005-12-04 Added support for NADAC bonus titling points.
 * 2005-11-20 Allow 'E's on non-titling runs.
 * 2005-08-11 Removed QQ checkbox.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2005-01-02 Added subnames to events.
 * 2005-01-01 Renamed MachPts to SpeedPts.
 * 2004-11-13 Also compute score for NA runs that have no titling pts.
 * 2004-09-07 Time+Fault scoring shouldn't include time faults.
 * 2004-03-20 The date never got set if the initial entry had no date
 *            and we didn't change it (first run in a trial).
 *            Plus, the table-in-yps flag was backwards and didn't
 *            properly initialize when the event type changed.
 * 2004-02-14 Added Table-in-YPS flag.
 * 2004-02-09 Update YPS when the time is changed.
 *            When date changes, update controls.
 * 2004-01-19 Total faults weren't updated when course faults changed.
 * 2004-01-18 Calling UpdateData during data entry causes way too much
 *            validation. Only call during OnOK (from dlgsheet)
 * 2003-12-27 Changed FindEvent to take a date. Also, update the controls
 *            when the date changes as the scoring config may change.
 * 2003-12-09 Fixed a bug where the QQ checkbox didn't get set right.
 * 2003-10-13 Made Time/CourseFaults common to all scoring methods.
 *            This allows faults for things like language!
 * 2003-09-29 Required pts were being overwriten with default values
 *            during dialog initialization.
 * 2003-09-01 Total faults weren't being shown when there was no SCT.
 * 2003-08-17 Title points were being computed on 'NQ' and the score was
 *            always being computed. Fixed both.
 * 2003-07-14 Changed 'Score' to show data on 'Q' and 'NQ'.
 */

#include "stdafx.h"
#include "DlgRunPanelScore.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ComboBoxQ.h"
#include "DlgInfoNote.h"
#include "DlgListCtrl.h"
#include "NoteButton.h"

#include "ARBCommon/StringUtil.h"
#include "LibARBWin/RichEditCtrl2.h"
#include "LibARBWin/Validators.h"
#include <wx/datectrl.h>
#include <wx/dateevt.h>
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
// This is just to get the text in a sunken static control to look better
std::wstring Pad(std::wstring const& val)
{
	return fmt::format(L" {} ", val);
}
} // namespace


class CDlgDogVenueData : public wxClientData
{
public:
	CDlgDogVenueData(ARBDogClubPtr const& inClub, ARBConfigVenuePtr const& inVenue)
		: m_Club(inClub)
		, m_Venue(inVenue)
	{
	}
	ARBDogClubPtr m_Club;
	ARBConfigVenuePtr m_Venue;
};


class CDlgDogDivData : public wxClientData
{
public:
	CDlgDogDivData(ARBConfigDivisionPtr const& inDiv)
		: m_Div(inDiv)
	{
	}
	ARBConfigDivisionPtr m_Div;
};


class CDlgDogLevelData : public wxClientData
{
public:
	CDlgDogLevelData(ARBConfigLevelPtr const& inLevel)
		: m_pLevel(inLevel)
		, m_pSubLevel()
	{
	}
	CDlgDogLevelData(ARBConfigLevelPtr const& inLevel, ARBConfigSubLevelPtr const& inSubLevel)
		: m_pLevel(inLevel)
		, m_pSubLevel(inSubLevel)
	{
	}
	ARBConfigLevelPtr m_pLevel;
	ARBConfigSubLevelPtr m_pSubLevel;
};

/////////////////////////////////////////////////////////////////////////////

CDlgRunPanelScore::CDlgRunPanelScore(
	CDlgRun* pDlg,
	CAgilityBookDoc* pDoc,
	ARBDogPtr const& inDog,
	ARBDogTrialPtr const& inTrial,
	ARBDogRunPtr const& inRun,
	wxWindow* parent)
	: CDlgRunPanelBase(pDlg, pDoc, inDog, inTrial, inRun, parent)
	, m_ctrlDate(nullptr)
	, m_Date(inRun->GetDate())
	, m_ctrlVenues(nullptr)
	, m_ctrlDivisions(nullptr)
	, m_ctrlLevels(nullptr)
	, m_ctrlEvents(nullptr)
	, m_ctrlSubNamesText(nullptr)
	, m_ctrlSubNames(nullptr)
	, m_SubName(StringUtil::stringWX(inRun->GetSubName()))
	, m_ctrlTable(nullptr)
	, m_Table(false)
	, m_textHeight(nullptr)
	, m_ctrlHeight(nullptr)
	, m_Height(StringUtil::stringWX(inRun->GetHeight()))
	, m_textJudge(nullptr)
	, m_ctrlJudge(nullptr)
	, m_ctrlJudgeNote(nullptr)
	, m_Judge(StringUtil::stringWX(inRun->GetJudge()))
	, m_ctrlHandler(nullptr)
	, m_Handler(StringUtil::stringWX(inRun->GetHandler()))
	, m_ctrlConditions(nullptr)
	, m_Conditions(StringUtil::stringWX(inRun->GetConditions()))
	, m_ctrlDesc(nullptr)
	, m_ctrlPartnerEdit(nullptr)
	, m_ctrlPartner(nullptr)
	, m_ctrlSCTText(nullptr)
	, m_ctrlSCT(nullptr)
	, m_SCT(0.0)
	, m_textYardsReqOpeningPts(nullptr)
	, m_ctrlYardsReqOpeningPts(nullptr)
	, m_Opening(0)
	, m_Yards(0.0)
	, m_textMinYPSClosingTime(nullptr)
	, m_ctrlMinYPSClosingTime(nullptr)
	, m_SCT2(0.0)
	, m_ctrlClosingText(nullptr)
	, m_ctrlClosing(nullptr)
	, m_Closing(0)
	, m_ctrlObstaclesText(nullptr)
	, m_ctrlObstacles(nullptr)
	, m_Obstacles(inRun->GetScoring().GetObstacles())
	, m_ctrlTimeText(nullptr)
	, m_ctrlTime(nullptr)
	, m_Time(0.0)
	, m_ctrlFaultsText(nullptr)
	, m_ctrlFaults(nullptr)
	, m_Faults(0)
	, m_textYPSOpeningPts(nullptr)
	, m_ctrlYPSOpeningPts(nullptr)
	, m_Open(0)
	, m_textClosingPtsTotalFaults(nullptr)
	, m_ctrlClosingPtsTotalFaults(nullptr)
	, m_Close(0)
	, m_ctrlObstaclesPSText(nullptr)
	, m_ctrlObstaclesPS(nullptr)
	, m_textPlace(nullptr)
	, m_ctrlPlace(nullptr)
	, m_textPlaceOf(nullptr)
	, m_Place(inRun->GetPlace())
	, m_ctrlInClass(nullptr)
	, m_InClass(inRun->GetInClass())
	, m_textDogsQd(nullptr)
	, m_ctrlDogsQd(nullptr)
	, m_DogsQd(inRun->GetDogsQd())
	, m_ctrlQ(nullptr)
	, m_ctrlBonusPtsText(nullptr)
	, m_ctrlBonusTitlePts(nullptr)
	, m_BonusTitlePts(inRun->GetScoring().GetBonusTitlePts())
	, m_ctrlSpeedPtsText(nullptr)
	, m_ctrlSpeedPts(nullptr)
	, m_ctrlTitlePointsText(nullptr)
	, m_ctrlTitlePoints(nullptr)
	, m_ctrlScore(nullptr)
{
	m_clrBack = GetBackgroundColour();

	if (!m_Run->GetDate().IsValid())
		m_Date.SetToday();

	if (ARBScoringType::BySpeed != m_Run->GetScoring().GetType())
	{
		if (m_Judge.empty())
		{
			std::wstring last = CAgilityBookOptions::GetLastEnteredJudge();
			if (!last.empty())
				m_Judge = StringUtil::stringWX(last);
		}
	}

	if (m_Handler.empty())
	{
		std::wstring last = CAgilityBookOptions::GetLastEnteredHandler(m_pDog);
		if (!last.empty())
			m_Handler = StringUtil::stringWX(last);
	}

	switch (m_Run->GetScoring().GetType())
	{
	case ARBScoringType::Unknown:
		break;
	case ARBScoringType::ByTime:
		m_Faults = m_Run->GetScoring().GetCourseFaults();
		m_Time = m_Run->GetScoring().GetTime();
		m_Table = m_Run->GetScoring().HasTable();
		m_Yards = m_Run->GetScoring().GetYards();
		m_SCT = m_Run->GetScoring().GetSCT();
		break;
	case ARBScoringType::ByOpenClose:
		m_Faults = m_Run->GetScoring().GetCourseFaults();
		m_Time = m_Run->GetScoring().GetTime();
		m_Opening = m_Run->GetScoring().GetNeedOpenPts();
		m_Closing = m_Run->GetScoring().GetNeedClosePts();
		m_Open = m_Run->GetScoring().GetOpenPts();
		m_Close = m_Run->GetScoring().GetClosePts();
		m_SCT = m_Run->GetScoring().GetSCT();
		m_SCT2 = m_Run->GetScoring().GetSCT2();
		break;
	case ARBScoringType::ByPoints:
		m_Faults = m_Run->GetScoring().GetCourseFaults();
		m_Time = m_Run->GetScoring().GetTime();
		m_Opening = m_Run->GetScoring().GetNeedOpenPts();
		m_Open = m_Run->GetScoring().GetOpenPts();
		m_SCT = m_Run->GetScoring().GetSCT();
		break;
	case ARBScoringType::BySpeed:
		m_Faults = m_Run->GetScoring().GetCourseFaults();
		m_Time = m_Run->GetScoring().GetTime();
		m_Yards = m_Run->GetScoring().GetYards();
		break;
	}

	// Controls (these are done first to control tab order)

	wxStaticText* textDate
		= new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_DATE"), wxDefaultPosition, wxDefaultSize, 0);
	textDate->Wrap(-1);

	m_ctrlDate = new wxDatePickerCtrl(
		this,
		wxID_ANY,
		wxDefaultDateTime,
		wxDefaultPosition,
		wxDefaultSize,
		wxDP_DROPDOWN | wxDP_SHOWCENTURY,
		CGenericValidator(&m_Date));
	m_ctrlDate->Bind(wxEVT_DATE_CHANGED, &CDlgRunPanelScore::OnScoreDateChanged, this);
	m_ctrlDate->SetHelpText(_("HIDC_RUNSCORE_DATE"));
	m_ctrlDate->SetToolTip(_("HIDC_RUNSCORE_DATE"));

	m_ctrlVenues = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY);
	m_ctrlVenues->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgRunPanelScore::OnSelchangeVenue, this);
	m_ctrlVenues->SetHelpText(_("HIDC_RUNSCORE_VENUE"));
	m_ctrlVenues->SetToolTip(_("HIDC_RUNSCORE_VENUE"));
	for (auto pClub : inTrial->GetClubs())
	{
		if (pClub->GetPrimaryClub())
			continue;
		ARBConfigVenuePtr pVenue;
		m_pDoc->Book().GetConfig().GetVenues().FindVenue(pClub->GetVenue(), &pVenue);
		int index = m_ctrlVenues->Append(fmt::format(L"[{}] {}", pClub->GetVenue(), pClub->GetName()));
		m_ctrlVenues->SetClientObject(index, new CDlgDogVenueData(pClub, pVenue));
		if (m_Run->GetClub() && *(m_Run->GetClub()) == *pClub)
			m_ctrlVenues->SetSelection(index);
	}
	if (wxNOT_FOUND == m_ctrlVenues->GetSelection())
	{
		m_ctrlVenues->SetSelection(0);
		m_Run->SetClub(GetVenueData(0)->m_Club);
	}

	wxStaticText* textLocation = new wxStaticText(
		this,
		wxID_ANY,
		Pad(m_pTrial->GetLocation()),
		wxDefaultPosition,
		wxDefaultSize,
		wxSUNKEN_BORDER);
	textLocation->Wrap(-1);

	wxStaticText* textDiv
		= new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_DIVISION"), wxDefaultPosition, wxDefaultSize, 0);
	textDiv->Wrap(-1);

	m_ctrlDivisions = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY);
	m_ctrlDivisions->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgRunPanelScore::OnSelchangeDivision, this);
	m_ctrlDivisions->SetHelpText(_("HIDC_RUNSCORE_DIVISION"));
	m_ctrlDivisions->SetToolTip(_("HIDC_RUNSCORE_DIVISION"));

	wxStaticText* textLevel
		= new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_LEVEL"), wxDefaultPosition, wxDefaultSize, 0);
	textLevel->Wrap(-1);

	m_ctrlLevels = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY);
	m_ctrlLevels->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgRunPanelScore::OnSelchangeLevel, this);
	m_ctrlLevels->SetHelpText(_("HIDC_RUNSCORE_LEVEL"));
	m_ctrlLevels->SetToolTip(_("HIDC_RUNSCORE_LEVEL"));

	wxStaticText* textEvent
		= new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_EVENT"), wxDefaultPosition, wxDefaultSize, 0);
	textEvent->Wrap(-1);

	m_ctrlEvents = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY);
	m_ctrlEvents->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgRunPanelScore::OnSelchangeEvent, this);
	m_ctrlEvents->SetHelpText(_("HIDC_RUNSCORE_EVENT"));
	m_ctrlEvents->SetToolTip(_("HIDC_RUNSCORE_EVENT"));

	m_ctrlSubNamesText
		= new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_SUBNAME"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlSubNamesText->Wrap(-1);

	m_ctrlSubNames = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN,
		CTrimValidator(&m_SubName, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlSubNames->SetHelpText(_("HIDC_RUNSCORE_SUBNAME"));
	m_ctrlSubNames->SetToolTip(_("HIDC_RUNSCORE_SUBNAME"));

	m_ctrlTable = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_RUNSCORE_TABLE"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_Table));
	m_ctrlTable->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgRunPanelScore::OnBnClickedTableYps, this);
	m_ctrlTable->SetHelpText(_("HIDC_RUNSCORE_TABLE"));
	m_ctrlTable->SetToolTip(_("HIDC_RUNSCORE_TABLE"));

	m_textHeight = new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_HEIGHT"), wxDefaultPosition, wxDefaultSize, 0);
	m_textHeight->Wrap(-1);

	m_ctrlHeight = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
#ifdef __WXMAC__
		wxSize(wxDLG_UNIT_X(this, 50), -1), // Just not wide enough on a Mac...
#else
		wxSize(wxDLG_UNIT_X(this, 30), -1),
#endif
		0,
		nullptr,
		wxCB_DROPDOWN,
		CTrimValidator(&m_Height, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlHeight->SetHelpText(_("HIDC_RUNSCORE_HEIGHT"));
	m_ctrlHeight->SetToolTip(_("HIDC_RUNSCORE_HEIGHT"));
	std::set<std::wstring> names;
	m_pDoc->Book().GetAllHeights(names);
	std::set<std::wstring>::const_iterator iter;
	wxArrayString choices;
	for (iter = names.begin(); iter != names.end(); ++iter)
	{
		wxString wxName(StringUtil::stringWX(*iter));
		m_ctrlHeight->Append(wxName);
		choices.Add(wxName);
	}
	m_ctrlHeight->AutoComplete(choices);

	m_textJudge = new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_JUDGE"), wxDefaultPosition, wxDefaultSize, 0);
	m_textJudge->Wrap(-1);

	m_ctrlJudge = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN,
		CTrimValidator(&m_Judge, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlJudge->SetHelpText(_("HIDC_RUNSCORE_JUDGE"));
	m_ctrlJudge->SetToolTip(_("HIDC_RUNSCORE_JUDGE"));

	m_ctrlJudgeNote = new CNoteButton(this);
	m_ctrlJudgeNote->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRunPanelScore::OnJudgeNotes, this);
	m_ctrlJudgeNote->SetHelpText(_("HIDC_RUNSCORE_JUDGE_NOTES"));
	m_ctrlJudgeNote->SetToolTip(_("HIDC_RUNSCORE_JUDGE_NOTES"));

	wxStaticText* textHandler
		= new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_HANDLER"), wxDefaultPosition, wxDefaultSize, 0);
	textHandler->Wrap(-1);

	m_ctrlHandler = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN,
		CTrimValidator(&m_Handler, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlHandler->SetHelpText(_("HIDC_RUNSCORE_HANDLER"));
	m_ctrlHandler->SetToolTip(_("HIDC_RUNSCORE_HANDLER"));
	m_pDoc->Book().GetAllHandlers(names);
	choices.Clear();
	for (iter = names.begin(); iter != names.end(); ++iter)
	{
		wxString wxName(StringUtil::stringWX(*iter));
		m_ctrlHandler->Append(wxName);
		choices.Add(wxName);
	}
	m_ctrlHandler->AutoComplete(choices);

	wxStaticText* textConditions
		= new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_CONDITIONS"), wxDefaultPosition, wxDefaultSize, 0);
	textConditions->Wrap(-1);

	m_ctrlConditions = new CSpellCheckCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDLG_UNIT(this, wxSize(200, 25)),
		wxTE_MULTILINE | wxTE_WORDWRAP,
		CTrimValidator(&m_Conditions, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlConditions->SetHelpText(_("HIDC_RUNSCORE_CONDITIONS"));
	m_ctrlConditions->SetToolTip(_("HIDC_RUNSCORE_CONDITIONS"));

	m_ctrlDesc = new CRichEditCtrl2(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(-1, wxDLG_UNIT_Y(this, 35)),
		true);
	m_ctrlDesc->SetHelpText(_("HIDC_RUNSCORE_DESC"));
	m_ctrlDesc->SetToolTip(_("HIDC_RUNSCORE_DESC"));

	m_ctrlPartnerEdit
		= new wxButton(this, wxID_ANY, _("IDC_RUNSCORE_PARTNERS_EDIT"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlPartnerEdit->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRunPanelScore::OnPartnersEdit, this);
	m_ctrlPartnerEdit->SetHelpText(_("HIDC_RUNSCORE_PARTNERS_EDIT"));
	m_ctrlPartnerEdit->SetToolTip(_("HIDC_RUNSCORE_PARTNERS_EDIT"));

	m_ctrlPartner = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 115), -1),
		wxTE_READONLY);
	m_ctrlPartner->SetBackgroundColour(m_clrBack);
	m_ctrlPartner->SetHelpText(_("HIDC_RUNSCORE_PARTNER"));
	m_ctrlPartner->SetToolTip(_("HIDC_RUNSCORE_PARTNER"));

	m_ctrlSCTText = new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_SCT"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlSCTText->Wrap(-1);

	m_ctrlSCT = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 30), -1),
		0,
		CGenericValidator(&m_SCT));
	m_ctrlSCT->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRunPanelScore::OnSCTChange, this);
	m_ctrlSCT->SetHelpText(_("HIDC_RUNSCORE_SCT"));
	m_ctrlSCT->SetToolTip(_("HIDC_RUNSCORE_SCT"));

	m_textYardsReqOpeningPts
		= new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_OPENING_PTS"), wxDefaultPosition, wxDefaultSize, 0);
	m_textYardsReqOpeningPts->Wrap(-1);

	m_ctrlYardsReqOpeningPts
		= new CTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 30), -1), 0);
	m_ctrlYardsReqOpeningPts->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRunPanelScore::OnReqOpeningYPSChange, this);
	m_ctrlYardsReqOpeningPts->SetHelpText(_("HIDC_RUNSCORE_OPENING_PTS"));
	m_ctrlYardsReqOpeningPts->SetToolTip(_("HIDC_RUNSCORE_OPENING_PTS"));

	m_textMinYPSClosingTime
		= new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_MIN_YPS"), wxDefaultPosition, wxDefaultSize, 0);
	m_textMinYPSClosingTime->Wrap(-1);

	m_ctrlMinYPSClosingTime
		= new CTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 30), -1), 0);
	m_ctrlMinYPSClosingTime->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRunPanelScore::OnClosingTimeChange, this);
	m_ctrlMinYPSClosingTime->SetHelpText(_("HIDC_RUNSCORE_SCT2"));
	m_ctrlMinYPSClosingTime->SetToolTip(_("HIDC_RUNSCORE_SCT2"));

	m_ctrlClosingText
		= new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_CLOSING_PTS"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlClosingText->Wrap(-1);

	m_ctrlClosing = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 30), -1),
		0,
		CGenericValidator(&m_Closing));
	m_ctrlClosing->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRunPanelScore::OnReqClosingChange, this);
	m_ctrlClosing->SetHelpText(_("HIDC_RUNSCORE_CLOSING_PTS"));
	m_ctrlClosing->SetToolTip(_("HIDC_RUNSCORE_CLOSING_PTS"));

	m_ctrlObstaclesText
		= new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_OBSTACLES"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlObstaclesText->Wrap(-1);

	m_ctrlObstacles = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 30), -1),
		0,
		CGenericValidator(&m_Obstacles));
	m_ctrlObstacles->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRunPanelScore::OnNumObsChange, this);
	m_ctrlObstacles->SetHelpText(_("HIDC_RUNSCORE_OBSTACLES"));
	m_ctrlObstacles->SetToolTip(_("HIDC_RUNSCORE_OBSTACLES"));

	m_ctrlTimeText = new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_TIME"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlTimeText->Wrap(-1);

	m_ctrlTime = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 30), -1),
		0,
		CGenericValidator(&m_Time));
	m_ctrlTime->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRunPanelScore::OnTimeChange, this);
	m_ctrlTime->SetHelpText(_("HIDC_RUNSCORE_TIME"));
	m_ctrlTime->SetToolTip(_("HIDC_RUNSCORE_TIME"));

	m_ctrlFaultsText = new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_FAULTS"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlFaultsText->Wrap(-1);

	m_ctrlFaults = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 30), -1),
		0,
		CGenericValidator(&m_Faults));
	m_ctrlFaults->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRunPanelScore::OnFaultsChange, this);
	m_ctrlFaults->SetHelpText(_("HIDC_RUNSCORE_FAULTS"));
	m_ctrlFaults->SetToolTip(_("HIDC_RUNSCORE_FAULTS"));

	m_textYPSOpeningPts
		= new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_OPEN_PTS"), wxDefaultPosition, wxDefaultSize, 0);
	m_textYPSOpeningPts->Wrap(-1);

	m_ctrlYPSOpeningPts
		= new CTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 30), -1), 0);
	m_ctrlYPSOpeningPts->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRunPanelScore::OnOpenChange, this);
	m_ctrlYPSOpeningPts->SetHelpText(_("HIDC_RUNSCORE_OPEN_PTS"));
	m_ctrlYPSOpeningPts->SetToolTip(_("HIDC_RUNSCORE_OPEN_PTS"));

	m_textClosingPtsTotalFaults
		= new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_CLOSE_PTS"), wxDefaultPosition, wxDefaultSize, 0);
	m_textClosingPtsTotalFaults->Wrap(-1);

	m_ctrlClosingPtsTotalFaults
		= new CTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 30), -1), 0);
	m_ctrlClosingPtsTotalFaults->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRunPanelScore::OnCloseChange, this);
	m_ctrlClosingPtsTotalFaults->SetHelpText(_("HIDC_RUNSCORE_CLOSE_PTS"));
	m_ctrlClosingPtsTotalFaults->SetToolTip(_("HIDC_RUNSCORE_CLOSE_PTS"));

	m_ctrlObstaclesPSText
		= new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_OBSTACLES_PER_SEC"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlObstaclesPSText->Wrap(-1);

	m_ctrlObstaclesPS = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 30), -1),
		wxTE_READONLY);
	m_ctrlObstaclesPS->SetBackgroundColour(m_clrBack);
	m_ctrlObstaclesPS->SetHelpText(_("HIDC_RUNSCORE_OBSTACLES_PER_SEC"));
	m_ctrlObstaclesPS->SetToolTip(_("HIDC_RUNSCORE_OBSTACLES_PER_SEC"));

	m_textPlace = new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_PLACE"), wxDefaultPosition, wxDefaultSize, 0);
	m_textPlace->Wrap(-1);

	m_ctrlPlace = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 20), -1),
		0,
		CGenericValidator(&m_Place));
	m_ctrlPlace->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRunPanelScore::OnPlaceChange, this);
	m_ctrlPlace->SetHelpText(_("HIDC_RUNSCORE_PLACE"));
	m_ctrlPlace->SetToolTip(_("HIDC_RUNSCORE_PLACE"));

	m_textPlaceOf = new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_IN_CLASS"), wxDefaultPosition, wxDefaultSize, 0);
	m_textPlaceOf->Wrap(-1);

	m_ctrlInClass = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 20), -1),
		0,
		CGenericValidator(&m_InClass));
	m_ctrlInClass->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRunPanelScore::OnInClassChange, this);
	m_ctrlInClass->SetHelpText(_("HIDC_RUNSCORE_IN_CLASS"));
	m_ctrlInClass->SetToolTip(_("HIDC_RUNSCORE_IN_CLASS"));

	m_textDogsQd = new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_DOGS_QD"), wxDefaultPosition, wxDefaultSize, 0);
	m_textDogsQd->Wrap(-1);

	m_ctrlDogsQd = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 20), -1),
		0,
		CGenericValidator(&m_DogsQd, -1));
	m_ctrlDogsQd->SetHelpText(_("HIDC_RUNSCORE_DOGS_QD"));
	m_ctrlDogsQd->SetToolTip(_("HIDC_RUNSCORE_DOGS_QD"));

	wxStaticText* textQ = new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_Q"), wxDefaultPosition, wxDefaultSize, 0);
	textQ->Wrap(-1);

	m_ctrlQ = new CQualifyingComboBox(this, m_Run);
	m_ctrlQ->Bind(wxEVT_COMMAND_CHOICE_SELECTED, &CDlgRunPanelScore::OnSelchangeQ, this);
	m_ctrlQ->SetHelpText(_("HIDC_RUNSCORE_Q"));
	m_ctrlQ->SetToolTip(_("HIDC_RUNSCORE_Q"));

	m_ctrlBonusPtsText
		= new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_BONUSPTS"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlBonusPtsText->Wrap(-1);

	m_ctrlBonusTitlePts = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 30), -1),
		0,
		CGenericValidator(&m_BonusTitlePts, -1));
	m_ctrlBonusTitlePts->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRunPanelScore::OnBonusChange, this);
	m_ctrlBonusTitlePts->SetHelpText(_("HIDC_RUNSCORE_BONUSTITLEPTS"));
	m_ctrlBonusTitlePts->SetToolTip(_("HIDC_RUNSCORE_BONUSTITLEPTS"));

	m_ctrlSpeedPtsText
		= new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_SPEEDPTS"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlSpeedPtsText->Wrap(-1);

	m_ctrlSpeedPts = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 30), -1),
		wxTE_READONLY);
	m_ctrlSpeedPts->SetBackgroundColour(m_clrBack);

	m_ctrlTitlePointsText
		= new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_TITLE_POINTS"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlTitlePointsText->Wrap(-1);

	m_ctrlTitlePoints = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 30), -1),
		wxTE_READONLY);
	m_ctrlTitlePoints->SetBackgroundColour(m_clrBack);

	wxStaticText* textScore
		= new wxStaticText(this, wxID_ANY, _("IDC_RUNSCORE_SCORE"), wxDefaultPosition, wxDefaultSize, 0);
	textScore->Wrap(-1);

	m_ctrlScore = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 30), -1),
		wxTE_READONLY);
	m_ctrlScore->SetBackgroundColour(m_clrBack);

	wxButton* btnOtherPoints
		= new wxButton(this, wxID_ANY, _("IDC_RUNSCORE_OTHERPOINTS"), wxDefaultPosition, wxDefaultSize, 0);
	btnOtherPoints->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRunPanelScore::OnOtherpoints, this);
	btnOtherPoints->SetHelpText(_("HIDC_RUNSCORE_OTHERPOINTS"));
	btnOtherPoints->SetToolTip(_("HIDC_RUNSCORE_OTHERPOINTS"));
	if (0 < m_pDoc->Book().GetConfig().GetOtherPoints().size())
		btnOtherPoints->Enable(true);
	else
		btnOtherPoints->Enable(false);

	// Sizers

	wxBoxSizer* sizerScorePanel = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerDate = new wxBoxSizer(wxHORIZONTAL);
	sizerDate
		->Add(textDate, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerDate
		->Add(m_ctrlDate, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerDate->Add(
		m_ctrlVenues,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerDate->Add(textLocation, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerScorePanel->Add(
		sizerDate,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND | wxLEFT | wxRIGHT | wxTOP,
		wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerDivHt = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerDivHtTable = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* sizerEvent = new wxFlexGridSizer(4, 2, 0, 0);
	sizerEvent->SetFlexibleDirection(wxBOTH);
	sizerEvent->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerEvent->Add(
		textDiv,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerEvent->Add(m_ctrlDivisions, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0);
	sizerEvent->Add(
		textLevel,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxRIGHT | wxTOP,
		wxDLG_UNIT_X(this, 5));
	sizerEvent->Add(m_ctrlLevels, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxTOP, wxDLG_UNIT_X(this, 5));
	sizerEvent->Add(
		textEvent,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxRIGHT | wxTOP,
		wxDLG_UNIT_X(this, 5));
	sizerEvent->Add(m_ctrlEvents, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxTOP, wxDLG_UNIT_X(this, 5));
	sizerEvent->Add(
		m_ctrlSubNamesText,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxRIGHT | wxTOP,
		wxDLG_UNIT_X(this, 5));
	sizerEvent->Add(m_ctrlSubNames, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxTOP, wxDLG_UNIT_X(this, 5));

	sizerDivHtTable->Add(sizerEvent, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 0);
	sizerDivHtTable->Add(m_ctrlTable, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	sizerDivHt->Add(sizerDivHtTable, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerHtCond = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerHt = new wxBoxSizer(wxHORIZONTAL);
	sizerHt->Add(
		m_textHeight,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerHt->Add(
		m_ctrlHeight,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerHt->Add(
		m_textJudge,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerHt->Add(
		m_ctrlJudge,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerHt->Add(
		m_ctrlJudgeNote,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerHt->Add(
		textHandler,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerHt->Add(m_ctrlHandler, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 0);

	sizerHtCond->Add(sizerHt, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 0);

	wxBoxSizer* sizerCond = new wxBoxSizer(wxHORIZONTAL);
	sizerCond->Add(textConditions, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerCond->Add(m_ctrlConditions, 1, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 0);

	sizerHtCond->Add(sizerCond, 1, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerComments = new wxBoxSizer(wxHORIZONTAL);
	sizerComments->Add(m_ctrlDesc, 1, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 0);

	wxBoxSizer* sizerPartner = new wxBoxSizer(wxVERTICAL);
	sizerPartner->Add(m_ctrlPartnerEdit, 0, 0, 0);
	sizerPartner->Add(m_ctrlPartner, 0, wxTOP, wxDLG_UNIT_X(this, 3));

	sizerComments->Add(sizerPartner, 0, wxEXPAND | wxLEFT, wxDLG_UNIT_X(this, 5));

	sizerHtCond->Add(sizerComments, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND | wxTOP, wxDLG_UNIT_X(this, 5));

	sizerDivHt->Add(sizerHtCond, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 0);

	sizerScorePanel->Add(
		sizerDivHt,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND | wxLEFT | wxRIGHT | wxTOP,
		wxDLG_UNIT_X(this, 5));

	wxFlexGridSizer* sizerResults = new wxFlexGridSizer(5, 4, 0, 0);
	sizerResults->SetFlexibleDirection(wxBOTH);
	sizerResults->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxBoxSizer* sizerSCT = new wxBoxSizer(wxHORIZONTAL);
	sizerSCT->Add(
		m_ctrlSCTText,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerSCT->Add(m_ctrlSCT, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerSCT, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerTime = new wxBoxSizer(wxHORIZONTAL);
	sizerTime->Add(
		m_ctrlTimeText,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerTime->Add(m_ctrlTime, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerTime, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerPlace = new wxBoxSizer(wxHORIZONTAL);
	sizerPlace->Add(
		m_textPlace,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerPlace->Add(
		m_ctrlPlace,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerPlace->Add(
		m_textPlaceOf,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerPlace->Add(m_ctrlInClass, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerPlace, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerBonus = new wxBoxSizer(wxHORIZONTAL);
	sizerBonus->Add(
		m_ctrlBonusPtsText,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerBonus->Add(m_ctrlBonusTitlePts, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerBonus, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT, 0);

	wxBoxSizer* sizerYardsReqOpeningPts = new wxBoxSizer(wxHORIZONTAL);
	sizerYardsReqOpeningPts->Add(
		m_textYardsReqOpeningPts,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerYardsReqOpeningPts
		->Add(m_ctrlYardsReqOpeningPts, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(
		sizerYardsReqOpeningPts,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT | wxTOP,
		wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerFaults = new wxBoxSizer(wxHORIZONTAL);
	sizerFaults->Add(
		m_ctrlFaultsText,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerFaults->Add(m_ctrlFaults, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults
		->Add(sizerFaults, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerDogsQd = new wxBoxSizer(wxHORIZONTAL);
	sizerDogsQd->Add(
		m_textDogsQd,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerDogsQd->Add(m_ctrlDogsQd, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults
		->Add(sizerDogsQd, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerSpeed = new wxBoxSizer(wxHORIZONTAL);
	sizerSpeed->Add(
		m_ctrlSpeedPtsText,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerSpeed->Add(m_ctrlSpeedPts, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerSpeed, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerMinYPSClosingTime = new wxBoxSizer(wxHORIZONTAL);
	sizerMinYPSClosingTime->Add(
		m_textMinYPSClosingTime,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerMinYPSClosingTime
		->Add(m_ctrlMinYPSClosingTime, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(
		sizerMinYPSClosingTime,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT | wxTOP,
		wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerYPSOpeningPts = new wxBoxSizer(wxHORIZONTAL);
	sizerYPSOpeningPts->Add(
		m_textYPSOpeningPts,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerYPSOpeningPts->Add(m_ctrlYPSOpeningPts, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(
		sizerYPSOpeningPts,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT | wxTOP,
		wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerQ = new wxBoxSizer(wxHORIZONTAL);
	sizerQ->Add(textQ, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerQ->Add(m_ctrlQ, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults
		->Add(sizerQ, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerTitlePts = new wxBoxSizer(wxHORIZONTAL);
	sizerTitlePts->Add(
		m_ctrlTitlePointsText,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerTitlePts->Add(m_ctrlTitlePoints, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerTitlePts, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerReqClosingPts = new wxBoxSizer(wxHORIZONTAL);
	sizerReqClosingPts->Add(
		m_ctrlClosingText,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerReqClosingPts->Add(m_ctrlClosing, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(
		sizerReqClosingPts,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT | wxTOP,
		wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerClosingPtsTotalFaults = new wxBoxSizer(wxHORIZONTAL);
	sizerClosingPtsTotalFaults->Add(
		m_textClosingPtsTotalFaults,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerClosingPtsTotalFaults
		->Add(m_ctrlClosingPtsTotalFaults, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(
		sizerClosingPtsTotalFaults,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT | wxTOP,
		wxDLG_UNIT_X(this, 5));

	sizerResults->Add(0, 0, 1, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 0);

	wxBoxSizer* sizerScore = new wxBoxSizer(wxHORIZONTAL);
	sizerScore
		->Add(textScore, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerScore->Add(m_ctrlScore, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerScore, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerNumObs = new wxBoxSizer(wxHORIZONTAL);
	sizerNumObs->Add(
		m_ctrlObstaclesText,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerNumObs->Add(m_ctrlObstacles, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults
		->Add(sizerNumObs, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerObsSec = new wxBoxSizer(wxHORIZONTAL);
	sizerObsSec->Add(
		m_ctrlObstaclesPSText,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT,
		wxDLG_UNIT_X(this, 5));
	sizerObsSec->Add(m_ctrlObstaclesPS, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults
		->Add(sizerObsSec, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	sizerResults->Add(0, 0, 1, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 0);
	sizerResults->Add(
		btnOtherPoints,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxTOP,
		wxDLG_UNIT_X(this, 5));

	sizerScorePanel->Add(sizerResults, 1, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(sizerScorePanel);
	Layout();
	sizerScorePanel->Fit(this);

	UpdateVenue(false); // This will call UpdateControls();
	FillJudges();
	SetPartnerText();
}


wxWindow* CDlgRunPanelScore::GetInitialControl()
{
	return m_ctrlDate;
}


bool CDlgRunPanelScore::Validate()
{
	int index = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND == index)
	{
		wxMessageBox(_("IDS_SELECT_VENUE"), _("Agility Record Book"), wxOK | wxCENTRE | wxICON_STOP);
		m_ctrlVenues->SetFocus();
		return false;
	}
	ARBConfigVenuePtr pVenue = GetVenueData(index)->m_Venue;

	index = m_ctrlDivisions->GetSelection();
	if (wxNOT_FOUND == index)
	{
		wxMessageBox(_("IDS_SELECT_DIVISION"), _("Agility Record Book"), wxOK | wxCENTRE | wxICON_STOP);
		m_ctrlDivisions->SetFocus();
		return false;
	}
	std::wstring curDiv = StringUtil::stringW(m_ctrlDivisions->GetString(index));

	index = m_ctrlLevels->GetSelection();
	if (wxNOT_FOUND == index)
	{
		wxMessageBox(_("IDS_SELECT_LEVEL"), _("Agility Record Book"), wxOK | wxCENTRE | wxICON_STOP);
		m_ctrlLevels->SetFocus();
		return false;
	}
	CDlgDogLevelData* pLevel = GetLevelData(index);
	assert(pLevel);

	index = m_ctrlEvents->GetSelection();
	if (wxNOT_FOUND == index)
	{
		wxMessageBox(_("IDS_SELECT_EVENT"), _("Agility Record Book"), wxOK | wxCENTRE | wxICON_STOP);
		m_ctrlEvents->SetFocus();
		return false;
	}
	std::wstring curEvent = StringUtil::stringW(m_ctrlEvents->GetString(index));

	ARBConfigEventPtr pEvent;
	pVenue->GetEvents().FindEvent(curEvent, &pEvent);
	if (!pEvent)
	{
		wxMessageBox(_("IDS_BAD_EVENT"), _("Agility Record Book"), wxOK | wxCENTRE | wxICON_STOP);
		m_ctrlEvents->SetFocus();
		return false;
	}

	if (!pEvent->FindEvent(curDiv, pLevel->m_pLevel->GetName(), m_Run->GetDate()))
	{
		wxMessageBox(_("IDS_BAD_SCORINGMETHOD"), _("Agility Record Book"), wxOK | wxCENTRE | wxICON_STOP);
		m_ctrlLevels->SetFocus();
		return false;
	}

	if (wxNOT_FOUND == m_ctrlQ->GetSelection())
	{
		wxMessageBox(_("IDS_SELECT_Q"), _("Agility Record Book"), wxOK | wxCENTRE | wxICON_STOP);
		m_ctrlQ->SetFocus();
		return false;
	}

	return true;
}


bool CDlgRunPanelScore::Save()
{
	// Already validated.
	int index = m_ctrlVenues->GetSelection();
	ARBConfigVenuePtr pVenue = GetVenueData(index)->m_Venue;

#if defined(_DEBUG) || defined(__WXDEBUG__)
	index = m_ctrlDivisions->GetSelection();
	std::wstring curDiv = StringUtil::stringW(m_ctrlDivisions->GetString(index));

	index = m_ctrlLevels->GetSelection();
	CDlgDogLevelData* pLevel = GetLevelData(index);
	assert(pLevel);

	index = m_ctrlEvents->GetSelection();
	std::wstring curEvent = StringUtil::stringW(m_ctrlEvents->GetString(index));

	ARBConfigEventPtr pEvent;
	pVenue->GetEvents().FindEvent(curEvent, &pEvent);
#endif

	//@todo: Add integrity checks - things like snooker score >=37? is Q set?

	m_Run->SetDate(m_Date);
	if (m_ctrlSubNames->IsShown())
		m_Run->SetSubName(StringUtil::stringW(m_SubName));
	if (m_ctrlHeight->IsShown())
		m_Run->SetHeight(StringUtil::stringW(m_Height));
	if (m_ctrlJudge->IsShown())
		m_Run->SetJudge(StringUtil::stringW(m_Judge));
	if (m_ctrlHandler->IsShown())
		m_Run->SetHandler(StringUtil::stringW(m_Handler));
	m_Run->SetConditions(StringUtil::stringW(m_Conditions));
	if (m_ctrlInClass->IsShown())
		m_Run->SetInClass(m_InClass);
	if (m_ctrlDogsQd->IsShown())
		m_Run->SetDogsQd(m_DogsQd);

		// TODO: Remove debugging code
#if defined(_DEBUG) || defined(__WXDEBUG__)
	{
		assert(!!pEvent.get());
		ARBConfigScoringPtr scoring;
		pEvent->FindEvent(curDiv, pLevel->m_pLevel->GetName(), m_Run->GetDate(), &scoring);
		assert(!!scoring.get());
		if (!scoring->HasTable())
			if (m_Run->GetScoring().HasTable())
				wxMessageBox(L"Poof!", _("Agility Record Book"), wxOK | wxCENTRE);
	}
#endif
	// End TODO

	CAgilityBookOptions::SetLastEnteredDivision(m_pDog, pVenue, m_Run->GetDivision().c_str());
	CAgilityBookOptions::SetLastEnteredLevel(m_pDog, pVenue, m_Run->GetLevel().c_str());
	if (m_ctrlHeight->IsShown())
		CAgilityBookOptions::SetLastEnteredHeight(m_pDog, pVenue, m_Run->GetHeight().c_str());
	if (m_ctrlJudge->IsShown())
		CAgilityBookOptions::SetLastEnteredJudge(m_Run->GetJudge().c_str());
	if (m_ctrlHandler->IsShown())
		CAgilityBookOptions::SetLastEnteredHandler(m_pDog, m_Run->GetHandler().c_str());

	return true;
}


std::wstring CDlgRunPanelScore::GetCurrentVenueName() const
{
	std::wstring venue;
	int index = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND != index)
		venue = GetVenueData(index)->m_Venue->GetName();
	return venue;
}


CDlgDogVenueData* CDlgRunPanelScore::GetVenueData(int index) const
{
	return dynamic_cast<CDlgDogVenueData*>(m_ctrlVenues->GetClientObject(index));
}


CDlgDogDivData* CDlgRunPanelScore::GetDivisionData(int index) const
{
	return dynamic_cast<CDlgDogDivData*>(m_ctrlDivisions->GetClientObject(index));
}


CDlgDogLevelData* CDlgRunPanelScore::GetLevelData(int index) const
{
	return dynamic_cast<CDlgDogLevelData*>(m_ctrlLevels->GetClientObject(index));
}


bool CDlgRunPanelScore::GetEvent(ARBConfigEventPtr* outEvent) const
{
	if (outEvent)
		outEvent->reset();
	int idxVenue = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND == idxVenue)
		return false;
	int idxEvent = m_ctrlEvents->GetSelection();
	if (wxNOT_FOUND == idxEvent)
		return false;
	std::wstring str = StringUtil::stringW(m_ctrlEvents->GetString(idxEvent));
	if (str.empty())
		return false;
	ARBConfigVenuePtr pVenue = GetVenueData(idxVenue)->m_Venue;
	return pVenue->GetEvents().FindEvent(str, outEvent);
}


bool CDlgRunPanelScore::GetScoring(ARBConfigScoringPtr* outScoring) const
{
	bool bFound = false;
	if (outScoring)
		outScoring->reset();
	std::wstring div, level;
	int index = m_ctrlDivisions->GetSelection();
	if (wxNOT_FOUND != index)
	{
		div = m_ctrlDivisions->GetString(index);
	}
	index = m_ctrlLevels->GetSelection();
	if (wxNOT_FOUND != index)
	{
		level = m_ctrlLevels->GetString(index);
		CDlgDogLevelData* pLevel = GetLevelData(index);
		assert(pLevel);
		ARBConfigEventPtr pEvent;
		if (GetEvent(&pEvent))
		{
			if (0 < div.length() && 0 < level.length())
				bFound = pEvent->FindEvent(div, pLevel->m_pLevel->GetName(), m_Run->GetDate(), outScoring);
		}
	}
	return bFound;
}


void CDlgRunPanelScore::UpdateVenue(bool bOnEventChange)
{
	int index = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND == index)
		return;
	CDlgDogVenueData* pData = GetVenueData(index);

	if (m_ctrlHeight->IsShown() && ARBScoringType::BySpeed != m_Run->GetScoring().GetType())
	{
		if (bOnEventChange || m_Height.empty())
		{
			std::wstring last;
			last = CAgilityBookOptions::GetLastEnteredHeight(m_pDog, pData->m_Venue);
			if (!last.empty())
			{
				m_Height = StringUtil::stringWX(last);
				if (bOnEventChange)
					m_ctrlHeight->SetLabel(m_Height);
			}
		}
	}

	FillDivisions(bOnEventChange);
}


void CDlgRunPanelScore::FillDivisions(bool bOnEventChange)
{
	int index = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND == index)
		return;
	ARBConfigVenuePtr pVenue = GetVenueData(index)->m_Venue;

	std::wstring div;
	index = m_ctrlDivisions->GetSelection();
	if (wxNOT_FOUND != index)
	{
		div = m_ctrlDivisions->GetString(index);
	}
	if (div.empty())
		div = m_Run->GetDivision();
	m_ctrlDivisions->Clear();

	for (ARBConfigDivisionList::const_iterator iterDiv = pVenue->GetDivisions().begin();
		 iterDiv != pVenue->GetDivisions().end();
		 ++iterDiv)
	{
		ARBConfigDivisionPtr pDiv = (*iterDiv);
		// Does this division have any events possible on this date?
		for (ARBConfigEventList::const_iterator iterEvent = pVenue->GetEvents().begin();
			 iterEvent != pVenue->GetEvents().end();
			 ++iterEvent)
		{
			if ((*iterEvent)->VerifyEvent(pDiv->GetName(), WILDCARD_LEVEL, m_Run->GetDate()))
			{
				index = m_ctrlDivisions->Append(StringUtil::stringWX(pDiv->GetName()));
				m_ctrlDivisions->SetClientObject(index, new CDlgDogDivData(pDiv));
				if (pDiv->GetName() == div)
					m_ctrlDivisions->SetSelection(index);
				break;
			}
		}
	}
	// First try to find 'div' (in case the divisions changed)
	if (wxNOT_FOUND == m_ctrlDivisions->GetSelection() && !div.empty())
	{
		index = m_ctrlDivisions->FindString(StringUtil::stringWX(div), true);
		if (0 <= index)
			m_ctrlDivisions->SetSelection(index);
	}
	// Then try to find the last entered
	if (wxNOT_FOUND == m_ctrlDivisions->GetSelection())
	{
		std::wstring last = CAgilityBookOptions::GetLastEnteredDivision(m_pDog, pVenue);
		if (0 < last.length())
		{
			index = m_ctrlDivisions->FindString(StringUtil::stringWX(last), true);
			if (0 <= index)
				m_ctrlDivisions->SetSelection(index);
		}
	}
	if (wxNOT_FOUND == m_ctrlDivisions->GetSelection())
	{
		if (1 == m_ctrlDivisions->GetCount())
			m_ctrlDivisions->SetSelection(0);
	}
	// Force the division into the run. SetSelection will not cause an update.
	int idxDiv = m_ctrlDivisions->GetSelection();
	if (wxNOT_FOUND != idxDiv)
		m_Run->SetDivision(StringUtil::stringW(m_ctrlDivisions->GetString(idxDiv)));
	FillLevels(bOnEventChange);
}


void CDlgRunPanelScore::FillLevels(bool bOnEventChange)
{
	int index = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND == index)
		return;
	ARBConfigVenuePtr pVenue = GetVenueData(index)->m_Venue;

	std::wstring level;
	index = m_ctrlLevels->GetSelection();
	if (wxNOT_FOUND != index)
	{
		level = m_ctrlLevels->GetString(index);
	}
	if (level.empty())
		level = m_Run->GetLevel();
	m_ctrlLevels->Clear();
	index = m_ctrlDivisions->GetSelection();
	if (wxNOT_FOUND != index)
	{
		ARBConfigDivisionPtr pDiv = GetDivisionData(index)->m_Div;
		for (ARBConfigLevelList::const_iterator iter = pDiv->GetLevels().begin(); iter != pDiv->GetLevels().end();
			 ++iter)
		{
			ARBConfigLevelPtr pLevel = (*iter);
			// Does this level have any events possible on this date?
			for (ARBConfigEventList::const_iterator iterEvent = pVenue->GetEvents().begin();
				 iterEvent != pVenue->GetEvents().end();
				 ++iterEvent)
			{
				if ((*iterEvent)->VerifyEvent(pDiv->GetName(), pLevel->GetName(), m_Run->GetDate()))
				{
					if (0 < pLevel->GetSubLevels().size())
					{
						for (ARBConfigSubLevelList::const_iterator iterSub = pLevel->GetSubLevels().begin();
							 iterSub != pLevel->GetSubLevels().end();
							 ++iterSub)
						{
							ARBConfigSubLevelPtr pSubLevel = (*iterSub);
							int idx = m_ctrlLevels->Append(StringUtil::stringWX(pSubLevel->GetName()));
							m_ctrlLevels->SetClientObject(idx, new CDlgDogLevelData(pLevel, pSubLevel));
							if (level == pSubLevel->GetName())
								m_ctrlLevels->SetSelection(idx);
						}
					}
					else
					{
						int idx = m_ctrlLevels->Append(StringUtil::stringWX(pLevel->GetName()));
						m_ctrlLevels->SetClientObject(idx, new CDlgDogLevelData(pLevel));
						if (level == pLevel->GetName())
							m_ctrlLevels->SetSelection(idx);
					}
					break;
				}
			}
		}
		if (wxNOT_FOUND == m_ctrlLevels->GetSelection())
		{
			std::wstring last = CAgilityBookOptions::GetLastEnteredLevel(m_pDog, pVenue);
			if (0 < last.length())
			{
				int idx = m_ctrlLevels->FindString(StringUtil::stringWX(last), true);
				if (0 <= idx)
					m_ctrlLevels->SetSelection(idx);
			}
		}
	}
	if (wxNOT_FOUND == m_ctrlLevels->GetSelection())
	{
		if (1 == m_ctrlLevels->GetCount())
			m_ctrlLevels->SetSelection(0);
	}
	int idxLvl = m_ctrlLevels->GetSelection();
	if (wxNOT_FOUND != idxLvl)
		m_Run->SetLevel(StringUtil::stringW(m_ctrlLevels->GetString(idxLvl)));
	FillEvents(bOnEventChange);
	SetTitlePoints();
}


void CDlgRunPanelScore::FillEvents(bool bOnEventChange)
{
	int index = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND == index)
		return;
	ARBConfigVenuePtr pVenue = GetVenueData(index)->m_Venue;

	std::wstring evt;
	index = m_ctrlEvents->GetSelection();
	if (wxNOT_FOUND != index)
	{
		evt = m_ctrlEvents->GetString(index);
	}
	if (evt.empty())
		evt = m_Run->GetEvent();
	m_ctrlEvents->Clear();
	int idxDiv = m_ctrlDivisions->GetSelection();
	if (wxNOT_FOUND != idxDiv)
	{
		ARBConfigDivisionPtr pDiv = GetDivisionData(idxDiv)->m_Div;
		int idxLevel = m_ctrlLevels->GetSelection();
		if (wxNOT_FOUND != idxLevel)
		{
			ARBConfigEventPtr pLastAddedEvent;
			CDlgDogLevelData* pData = GetLevelData(idxLevel);
			for (ARBConfigEventList::const_iterator iter = pVenue->GetEvents().begin();
				 iter != pVenue->GetEvents().end();
				 ++iter)
			{
				ARBConfigEventPtr pEvent = (*iter);
				if (pEvent->FindEvent(pDiv->GetName(), pData->m_pLevel->GetName(), m_Run->GetDate()))
				{
					int idx = m_ctrlEvents->Append(StringUtil::stringWX(pEvent->GetName()));
					if (evt == pEvent->GetName())
					{
						m_ctrlEvents->SetSelection(idx);
						SetEventDesc(pEvent);
					}
					else
					{
						// If we're not looking for an event (and haven't found one!),
						// remember this.
						pLastAddedEvent = pEvent;
					}
				}
			}
			// If there's only one event, auto-select it.
			if (pLastAddedEvent && 1 == m_ctrlEvents->GetCount() && 0 > m_ctrlEvents->GetSelection())
			{
				m_ctrlEvents->SetSelection(0);
				SetEventDesc(pLastAddedEvent);
				m_Run->SetEvent(pLastAddedEvent->GetName());
			}
		}
	}
	FillSubNames();
	UpdateControls(bOnEventChange);
}


void CDlgRunPanelScore::FillSubNames()
{
	ARBConfigEventPtr pEvent;
	ARBConfigScoringPtr pScoring;
	if (GetEvent(&pEvent) && GetScoring(&pScoring))
	{
		if (pScoring->HasSubNames())
		{
			int index = m_ctrlVenues->GetSelection();
			if (wxNOT_FOUND == index)
				return;
			ARBConfigVenuePtr pVenue = GetVenueData(index)->m_Venue;

			std::set<std::wstring> names;
			m_pDoc->Book().GetAllEventSubNames(pVenue->GetName(), pEvent, names);
			m_ctrlSubNames->Clear();
			wxArrayString choices;
			for (std::set<std::wstring>::const_iterator iter = names.begin(); iter != names.end(); ++iter)
			{
				wxString wxName(StringUtil::stringWX(*iter));
				m_ctrlSubNames->Append(wxName);
				choices.Add(wxName);
			}
			m_ctrlSubNames->AutoComplete(choices);
			m_ctrlSubNamesText->Show(true);
			m_ctrlSubNames->Show(true);
		}
		else
		{
			m_ctrlSubNamesText->Show(false);
			m_ctrlSubNames->Show(false);
		}
	}
	else
	{
		m_ctrlSubNamesText->Show(false);
		m_ctrlSubNames->Show(false);
	}
}


void CDlgRunPanelScore::FillJudges()
{
	std::set<std::wstring> names;
	m_pDoc->Book().GetAllJudges(names, true, true);
	if (!m_Run->GetJudge().empty())
		names.insert(m_Run->GetJudge());
	m_ctrlJudge->Clear();
	wxArrayString choices;
	for (std::set<std::wstring>::const_iterator iter = names.begin(); iter != names.end(); ++iter)
	{
		wxString wxName(StringUtil::stringWX(*iter));
		m_ctrlJudge->Append(wxName);
		choices.Add(wxName);
	}
	m_ctrlJudge->AutoComplete(choices);
	m_ctrlJudge->SetValue(m_Judge);
}


void CDlgRunPanelScore::SetEventDesc(ARBConfigEventPtr const& inEvent)
{
	wxString desc;
	if (inEvent)
		desc += StringUtil::stringWX(inEvent->GetDesc());
	ARBConfigScoringPtr pScoring;
	if (GetScoring(&pScoring))
	{
		std::wstring const& note = pScoring->GetNote();
		if (!desc.empty() && 0 < note.length())
			desc += L"\n==========\n";
		desc += StringUtil::stringWX(note);
	}
	m_ctrlDesc->ChangeValue(desc);
}


void CDlgRunPanelScore::SetPartnerText()
{
	wxString partners;
	ARBConfigEventPtr pEvent;
	if (GetEvent(&pEvent))
	{
		if (pEvent->HasPartner())
		{
			for (ARBDogRunPartnerList::const_iterator iter = m_Run->GetPartners().begin();
				 iter != m_Run->GetPartners().end();
				 ++iter)
			{
				if (!partners.empty())
					partners += L", ";
				partners += StringUtil::stringWX((*iter)->GetHandler());
				partners += L"/";
				partners += StringUtil::stringWX((*iter)->GetDog());
			}
		}
	}
	m_ctrlPartner->ChangeValue(partners);
}


void CDlgRunPanelScore::SetMinYPS()
{
	if (ARBScoringType::ByTime == m_Run->GetScoring().GetType())
	{
		wxString str;
		double yps;
		if (m_Run->GetScoring().GetMinYPS(CAgilityBookOptions::GetTableInYPS(), yps))
		{
			str = StringUtil::stringWX(ARBDouble::ToString(yps, 3));
		}
		m_ctrlMinYPSClosingTime->ChangeValue(str);
	}
}


void CDlgRunPanelScore::SetYPS()
{
	if (ARBScoringType::ByTime == m_Run->GetScoring().GetType()
		|| ARBScoringType::BySpeed == m_Run->GetScoring().GetType())
	{
		wxString str;
		double yps;
		if (m_Run->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), yps))
		{
			str = StringUtil::stringWX(ARBDouble::ToString(yps, 3));
		}
		m_ctrlYPSOpeningPts->ChangeValue(str);
	}
}


void CDlgRunPanelScore::SetObstacles()
{
	wxString str;
	double ops;
	int prec;
	if (m_Run->GetScoring()
			.GetObstaclesPS(CAgilityBookOptions::GetTableInYPS(), CAgilityBookOptions::GetRunTimeInOPS(), ops, prec))
	{
		str = StringUtil::stringWX(ARBDouble::ToString(ops, prec));
	}
	m_ctrlObstaclesPS->ChangeValue(str);
}


void CDlgRunPanelScore::SetTotalFaults()
{
	if (ARBScoringType::ByTime == m_Run->GetScoring().GetType())
	{
		wxString total;
		ARBConfigScoringPtr pScoring;
		GetScoring(&pScoring);
		double faults = m_Run->GetScoring().GetCourseFaults() + m_Run->GetScoring().GetTimeFaults(pScoring);
		total = StringUtil::stringWX(ARBDouble::ToString(faults, 3));
		m_ctrlClosingPtsTotalFaults->ChangeValue(total);
	}
}


void CDlgRunPanelScore::SetTitlePoints()
{
	int index = m_ctrlQ->GetSelection();
	if (wxNOT_FOUND == index)
	{
		m_ctrlQ->SetFocus();
		return;
	}
	ARB_Q q = m_ctrlQ->GetQ(index);

	std::wstring strSpeed(L"0");
	std::wstring strTitle(L"0");
	std::wstring strScore(L"");
	ARBConfigScoringPtr pScoring;
	if (GetScoring(&pScoring))
	{
		// 8/17/03: Only compute title points on Q runs.
		if (q.Qualified())
		{
			if (pScoring->HasSpeedPts())
			{
				strSpeed = fmt::format(L"{}", m_Run->GetSpeedPoints(pScoring));
			}
			strTitle = fmt::format(L"{}", m_Run->GetTitlePoints(pScoring));
		}
		if (q.ShouldComputeScore())
			strScore = ARBDouble::ToString(m_Run->GetScore(pScoring));
	}
	// Doesn't matter if they're hidden,..
	m_ctrlSpeedPts->ChangeValue(strSpeed);
	m_ctrlTitlePoints->ChangeValue(strTitle);
	m_ctrlScore->ChangeValue(strScore);
}


void CDlgRunPanelScore::SetReadOnlyFlag(CTextCtrl* ctrl, bool bReadOnly)
{
	if (!ctrl)
		return;
	if (bReadOnly)
	{
		ctrl->SetEditable(false);
		ctrl->SetBackgroundColour(m_clrBack);
	}
	else
	{
		ctrl->SetEditable(true);
		ctrl->SetBackgroundColour(wxNullColour);
	}
}


void CDlgRunPanelScore::FixScoreLayout()
{
	Layout();
	GetSizer()->Fit(this);
	auto parent = GetParent();
	if (parent && parent->GetSizer())
	{
		parent->Layout();
		parent->GetSizer()->Fit(parent);
	}
}


void CDlgRunPanelScore::UpdateControls(bool bOnEventChange)
{
	m_textHeight->Show(false);
	m_ctrlHeight->Show(false);
	m_textJudge->Show(false);
	m_ctrlJudge->Show(false);
	m_ctrlJudgeNote->Show(false);
	m_ctrlHandler->Enable(false);
	m_ctrlConditions->Enable(false);
	m_ctrlPartnerEdit->Show(false);
	m_ctrlPartner->Show(false);
	m_ctrlTable->Show(false);
	m_ctrlSCTText->Show(false);
	m_ctrlSCT->Show(false);
	m_textYardsReqOpeningPts->Show(false);
	m_ctrlYardsReqOpeningPts->Show(false);
	m_textMinYPSClosingTime->Show(false);
	m_ctrlMinYPSClosingTime->Show(false);
	m_ctrlClosingText->Show(false);
	m_ctrlClosing->Show(false);
	m_ctrlObstaclesText->Show(false);
	m_ctrlObstacles->Show(false);
	m_ctrlTimeText->Show(false);
	m_ctrlTime->Show(false);
	m_ctrlFaultsText->Show(false);
	m_ctrlFaults->Show(false);
	m_textYPSOpeningPts->Show(false);
	m_ctrlYPSOpeningPts->Show(false);
	m_textClosingPtsTotalFaults->Show(false);
	m_ctrlClosingPtsTotalFaults->Show(false);
	m_ctrlObstaclesPS->Enable(false);
	m_textPlace->Show(false);
	m_ctrlPlace->Show(false);
	m_textPlaceOf->Show(false);
	m_ctrlInClass->Show(false);
	m_textDogsQd->Show(false);
	m_ctrlDogsQd->Show(false);
	m_ctrlQ->Enable(false);
	m_ctrlBonusPtsText->Show(false);
	m_ctrlBonusTitlePts->Show(false);
	m_ctrlSpeedPtsText->Show(false);
	m_ctrlSpeedPts->Show(false);
	m_ctrlTitlePointsText->Show(false);
	m_ctrlTitlePoints->Show(false);

	ARBConfigScoringPtr pScoring;
	if (!GetScoring(&pScoring))
	{
		m_Run->GetScoring().SetType(ARBScoringType::Unknown, false);
		if (bOnEventChange)
			FixScoreLayout();
		return;
	}

	ARBConfigEventPtr pEvent;
	if (!GetEvent(&pEvent))
	{
		if (bOnEventChange)
			FixScoreLayout();
		return;
	}

	m_ctrlHandler->Enable(true);
	m_ctrlConditions->Enable(true);

	if (pEvent->HasPartner())
	{
		m_ctrlPartnerEdit->Show(true);
		m_ctrlPartner->Show(true);
	}

	// 7/1/07: Resetting the table status must always be done - not just when
	// the new event has a table. (which is what was done before)
	if (bOnEventChange)
	{
		if (m_Run->GetScoring().HasTable() != pScoring->HasTable())
		{
			m_Run->GetScoring().SetHasTable(pScoring->HasTable());
			// Plus, we need to recompute the YPS.
			SetMinYPS();
			SetYPS();
			SetObstacles();
		}
	}
	if (pScoring->HasTable())
	{
		m_ctrlTable->Show(true);
		m_Table = m_Run->GetScoring().HasTable();
		m_ctrlTable->SetValue(m_Table);
	}

	wxString str;
	switch (pScoring->GetScoringStyle())
	{
	case ARBScoringStyle::Unknown:
		assert(0);
		break;
	case ARBScoringStyle::FaultsThenTime:
	case ARBScoringStyle::Faults100ThenTime:
	case ARBScoringStyle::Faults200ThenTime:
	case ARBScoringStyle::TimePlusFaults:
		m_Run->GetScoring().SetType(ARBScoringType::ByTime, pScoring->DropFractions());
		m_ctrlSCTText->Show(true);
		m_ctrlSCT->Show(true);
		m_textYardsReqOpeningPts->SetLabel(_("IDC_RUNSCORE_YARDS"));
		m_textYardsReqOpeningPts->Show(true);
		m_ctrlYardsReqOpeningPts->SetHelpText(_("HIDC_RUNSCORE_YARDS"));
		m_ctrlYardsReqOpeningPts->SetToolTip(_("HIDC_RUNSCORE_YARDS"));
		m_ctrlYardsReqOpeningPts->ChangeValue(StringUtil::stringWX(ARBDouble::ToString(m_Yards)));
		m_ctrlYardsReqOpeningPts->Show(true);
		m_textMinYPSClosingTime->SetLabel(_("IDC_RUNSCORE_MIN_YPS"));
		m_textMinYPSClosingTime->Show(true);
		m_ctrlMinYPSClosingTime->Show(true);
		m_ctrlMinYPSClosingTime->SetHelpText(wxEmptyString);
		m_ctrlMinYPSClosingTime->SetToolTip(wxEmptyString);
		SetReadOnlyFlag(m_ctrlMinYPSClosingTime, true);
		m_ctrlTimeText->Show(true);
		m_ctrlTime->Show(true);
		m_ctrlFaultsText->Show(true);
		m_ctrlFaults->Show(true);
		m_textYPSOpeningPts->SetLabel(_("IDC_RUNSCORE_YPS"));
		m_textYPSOpeningPts->Show(true);
		m_ctrlYPSOpeningPts->SetHelpText(wxEmptyString);
		m_ctrlYPSOpeningPts->SetToolTip(wxEmptyString);
		m_ctrlYPSOpeningPts->Show(true);
		SetReadOnlyFlag(m_ctrlYPSOpeningPts, true);
		m_textClosingPtsTotalFaults->SetLabel(_("IDC_RUNSCORE_TOTAL_FAULTS"));
		m_textClosingPtsTotalFaults->Show(true);
		m_ctrlClosingPtsTotalFaults->SetHelpText(wxEmptyString);
		m_ctrlClosingPtsTotalFaults->SetToolTip(wxEmptyString);
		m_ctrlClosingPtsTotalFaults->Show(true);
		SetReadOnlyFlag(m_ctrlClosingPtsTotalFaults, true);
		break;
	case ARBScoringStyle::OCScoreThenTime:
		m_Run->GetScoring().SetType(ARBScoringType::ByOpenClose, pScoring->DropFractions());
		if (bOnEventChange)
		{
			m_Opening = pScoring->GetRequiredOpeningPoints();
			if (0 != m_Opening)
				m_Run->GetScoring().SetNeedOpenPts(m_Opening);
			m_Closing = pScoring->GetRequiredClosingPoints();
			if (0 != m_Closing)
				m_Run->GetScoring().SetNeedClosePts(m_Closing);
		}
		// Do not push these (above) back into the run.
		// Otherwise this will overwrite valid values during OnInit.
		m_ctrlSCTText->Show(true);
		m_ctrlSCT->Show(true);
		m_textYardsReqOpeningPts->SetLabel(_("IDC_RUNSCORE_OPENING_PTS"));
		m_textYardsReqOpeningPts->Show(true);
		m_ctrlYardsReqOpeningPts->SetHelpText(_("HIDC_RUNSCORE_OPENING_PTS"));
		m_ctrlYardsReqOpeningPts->SetToolTip(_("HIDC_RUNSCORE_OPENING_PTS"));
		str.Printf(L"%hd", m_Opening);
		m_ctrlYardsReqOpeningPts->ChangeValue(str);
		m_ctrlYardsReqOpeningPts->Show(true);
		m_textMinYPSClosingTime->SetLabel(_("IDC_RUNSCORE_SCT2"));
		m_textMinYPSClosingTime->Show(true);
		m_ctrlMinYPSClosingTime->Show(true);
		m_ctrlMinYPSClosingTime->SetHelpText(_("HIDC_RUNSCORE_SCT2"));
		m_ctrlMinYPSClosingTime->SetToolTip(_("HIDC_RUNSCORE_SCT2"));
		m_ctrlMinYPSClosingTime->ChangeValue(StringUtil::stringWX(ARBDouble::ToString(m_SCT2)));
		SetReadOnlyFlag(m_ctrlMinYPSClosingTime, false);
		m_ctrlClosingText->Show(true);
		str.Printf(L"%hd", m_Closing);
		m_ctrlClosing->ChangeValue(str);
		m_ctrlClosing->Show(true);
		m_ctrlTimeText->Show(true);
		m_ctrlTime->Show(true);
		m_ctrlFaultsText->Show(true);
		m_ctrlFaults->Show(true);
		m_textYPSOpeningPts->SetLabel(_("IDC_RUNSCORE_OPEN_PTS"));
		m_textYPSOpeningPts->Show(true);
		m_ctrlYPSOpeningPts->SetHelpText(_("HIDC_RUNSCORE_OPEN_PTS"));
		m_ctrlYPSOpeningPts->SetToolTip(_("HIDC_RUNSCORE_OPEN_PTS"));
		str.Printf(L"%hd", m_Open);
		m_ctrlYPSOpeningPts->ChangeValue(str);
		m_ctrlYPSOpeningPts->Show(true);
		SetReadOnlyFlag(m_ctrlYPSOpeningPts, false);
		m_textClosingPtsTotalFaults->SetLabel(_("IDC_RUNSCORE_CLOSE_PTS"));
		m_textClosingPtsTotalFaults->Show(true);
		m_ctrlClosingPtsTotalFaults->SetHelpText(_("HIDC_RUNSCORE_CLOSE_PTS"));
		m_ctrlClosingPtsTotalFaults->SetToolTip(_("HIDC_RUNSCORE_CLOSE_PTS"));
		str.Printf(L"%hd", m_Close);
		m_ctrlClosingPtsTotalFaults->ChangeValue(str);
		m_ctrlClosingPtsTotalFaults->Show(true);
		SetReadOnlyFlag(m_ctrlClosingPtsTotalFaults, false);
		break;
	case ARBScoringStyle::ScoreThenTime:
		m_Run->GetScoring().SetType(ARBScoringType::ByPoints, pScoring->DropFractions());
		if (bOnEventChange)
		{
			m_Opening = pScoring->GetRequiredOpeningPoints();
			if (0 != m_Opening)
				m_Run->GetScoring().SetNeedOpenPts(m_Opening);
		}
		// Do not push this back into the run.
		// Otherwise this will overwrite valid values during OnInit.
		m_ctrlSCTText->Show(true);
		m_ctrlSCT->Show(true);
		m_textYardsReqOpeningPts->SetLabel(_("IDS_SCORING_REQUIRED_POINTS"));
		m_textYardsReqOpeningPts->Show(true);
		m_ctrlYardsReqOpeningPts->SetHelpText(_("HIDC_RUNSCORE_OPENING_PTS"));
		m_ctrlYardsReqOpeningPts->SetToolTip(_("HIDC_RUNSCORE_OPENING_PTS"));
		str.Printf(L"%hd", m_Opening);
		m_ctrlYardsReqOpeningPts->ChangeValue(str);
		m_ctrlYardsReqOpeningPts->Show(true);
		m_ctrlTimeText->Show(true);
		m_ctrlTime->Show(true);
		m_ctrlFaultsText->Show(true);
		m_ctrlFaults->Show(true);
		m_textYPSOpeningPts->SetLabel(_("IDS_POINTS"));
		m_textYPSOpeningPts->Show(true);
		m_ctrlYPSOpeningPts->SetHelpText(_("HIDC_RUNSCORE_OPEN_PTS"));
		m_ctrlYPSOpeningPts->SetToolTip(_("HIDC_RUNSCORE_OPEN_PTS"));
		str.Printf(L"%hd", m_Open);
		m_ctrlYPSOpeningPts->ChangeValue(str);
		m_ctrlYPSOpeningPts->Show(true);
		SetReadOnlyFlag(m_ctrlYPSOpeningPts, false);
		break;
	case ARBScoringStyle::TimeNoPlaces:
		m_Run->GetScoring().SetType(ARBScoringType::BySpeed, pScoring->DropFractions());
		m_ctrlSCTText->Show(false);
		m_ctrlSCT->Show(false);
		m_textYardsReqOpeningPts->SetLabel(_("IDC_RUNSCORE_YARDS"));
		m_textYardsReqOpeningPts->Show(true);
		m_ctrlYardsReqOpeningPts->SetHelpText(_("HIDC_RUNSCORE_YARDS"));
		m_ctrlYardsReqOpeningPts->SetToolTip(_("HIDC_RUNSCORE_YARDS"));
		m_ctrlYardsReqOpeningPts->ChangeValue(StringUtil::stringWX(ARBDouble::ToString(m_Yards)));
		m_ctrlYardsReqOpeningPts->Show(true);
		m_textMinYPSClosingTime->Show(false);
		m_ctrlMinYPSClosingTime->Show(false);
		m_ctrlTimeText->Show(true);
		m_ctrlTime->Show(true);
		m_ctrlFaultsText->Show(false);
		m_ctrlFaults->Show(false);
		m_textYPSOpeningPts->SetLabel(_("IDC_RUNSCORE_YPS"));
		m_textYPSOpeningPts->Show(true);
		m_ctrlYPSOpeningPts->SetHelpText(wxEmptyString);
		m_ctrlYPSOpeningPts->SetToolTip(wxEmptyString);
		m_ctrlYPSOpeningPts->Show(true);
		SetReadOnlyFlag(m_ctrlYPSOpeningPts, true);
		m_textClosingPtsTotalFaults->Show(false);
		m_ctrlClosingPtsTotalFaults->Show(false);
		break;
	}

	if (ARBScoringStyle::TimeNoPlaces != pScoring->GetScoringStyle())
	{
		m_textHeight->Show(true);
		m_ctrlHeight->Show(true);
		m_textJudge->Show(true);
		m_ctrlJudge->Show(true);
		m_ctrlJudgeNote->Show(true);
		m_ctrlObstaclesText->Show(true);
		m_ctrlObstacles->Show(true);
		m_textPlace->Show(true);
		m_ctrlPlace->Show(true);
		m_textPlaceOf->Show(true);
		m_ctrlInClass->Show(true);
		m_textDogsQd->Show(true);
		m_ctrlDogsQd->Show(true);
		m_ctrlDogsQd->Enable(0 < pScoring->GetTitlePoints().size() || 0 < pScoring->GetLifetimePoints().size());
		m_ctrlObstaclesPSText->SetLabel(_("IDC_RUNSCORE_OBSTACLES_PER_SEC"));
		m_ctrlObstaclesPSText->Show(true);
		m_ctrlObstaclesPS->Enable(true);
		m_ctrlObstaclesPS->SetHelpText(_("HIDC_RUNSCORE_OBSTACLES_PER_SEC"));
		m_ctrlObstaclesPS->SetToolTip(_("HIDC_RUNSCORE_OBSTACLES_PER_SEC"));
	}
	else
	{
		m_ctrlObstaclesPSText->SetLabel(_("IDC_RUNSCORE_MILESPERHOUR"));
		m_ctrlObstaclesPSText->Show(true);
		m_ctrlObstaclesPS->Enable(true);
		m_ctrlObstaclesPS->SetHelpText(_("HIDC_RUNSCORE_MILESPERHOUR"));
		m_ctrlObstaclesPS->SetToolTip(_("HIDC_RUNSCORE_MILESPERHOUR"));
	}
	m_ctrlQ->Enable(true);
	m_ctrlQ->ResetContent(pScoring);
	if (0 < pScoring->GetTitlePoints().size())
	{
		m_ctrlTitlePointsText->Show(true);
		m_ctrlTitlePoints->Show(true);
	}
	if (pScoring->HasBonusTitlePts())
	{
		m_ctrlBonusPtsText->Show(true);
		m_ctrlBonusTitlePts->Show(true);
	}
	if (pScoring->HasSpeedPts())
	{
		m_ctrlSpeedPtsText->Show(true);
		m_ctrlSpeedPts->Show(true);
	}
	SetTitlePoints();
	SetObstacles();
	if (ARBScoringType::ByTime == m_Run->GetScoring().GetType()
		|| ARBScoringType::BySpeed == m_Run->GetScoring().GetType())
	{
		SetMinYPS();
		SetYPS();
		SetTotalFaults();
	}
	if (bOnEventChange)
		FixScoreLayout();
}


void CDlgRunPanelScore::OnScoreDateChanged(wxDateEvent& evt)
{
	m_Run->SetDate(ARBDate(evt.GetDate().GetYear(), evt.GetDate().GetMonth() + 1, evt.GetDate().GetDay()));
	FillDivisions(true);
}


void CDlgRunPanelScore::OnSelchangeVenue(wxCommandEvent& evt)
{
	bool bChanged = true;
	int index = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND != index)
		m_Run->SetClub(GetVenueData(index)->m_Club);
	// We could check if the venue changed. But I'm finding it convenient when
	// co-sanctioning changes and changes don't necessarily update the club,
	// re-selecting the venue "fixes" it.
	if (bChanged)
		UpdateVenue(true);
	else
		FillDivisions(true);
}


void CDlgRunPanelScore::OnSelchangeDivision(wxCommandEvent& evt)
{
	int index = m_ctrlDivisions->GetSelection();
	if (wxNOT_FOUND != index)
		m_Run->SetDivision(StringUtil::stringW(m_ctrlDivisions->GetString(index)));
	FillLevels(true);
}


void CDlgRunPanelScore::OnSelchangeLevel(wxCommandEvent& evt)
{
	int index = m_ctrlLevels->GetSelection();
	if (wxNOT_FOUND != index)
		m_Run->SetLevel(StringUtil::stringW(m_ctrlLevels->GetString(index)));
	FillEvents(true);
}


void CDlgRunPanelScore::OnSelchangeEvent(wxCommandEvent& evt)
{
	int index = m_ctrlEvents->GetSelection();
	if (wxNOT_FOUND != index)
		m_Run->SetEvent(StringUtil::stringW(m_ctrlEvents->GetString(index)));
	FillSubNames();
	UpdateControls(true);
	ARBConfigEventPtr pEvent;
	GetEvent(&pEvent);
	SetEventDesc(pEvent);
}


void CDlgRunPanelScore::OnJudgeNotes(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	CDlgInfoNote dlg(m_pDoc, ARBInfoType::Judge, StringUtil::stringW(m_Judge), this);
	if (wxID_OK == dlg.ShowModal())
	{
		m_Judge = StringUtil::stringWX(dlg.CurrentSelection());
		FillJudges();
	}
}


void CDlgRunPanelScore::OnPartnersEdit(wxCommandEvent& evt)
{
	CDlgListCtrl dlg(ARBWhatToList::Partners, m_pDoc, m_Run, this);
	if (wxID_OK == dlg.ShowModal())
		SetPartnerText();
}


void CDlgRunPanelScore::OnOtherpoints(wxCommandEvent& evt)
{
	CDlgListCtrl dlg(m_pDoc->Book().GetConfig(), m_Run, this);
	dlg.ShowModal();
}


void CDlgRunPanelScore::OnBnClickedTableYps(wxCommandEvent& evt)
{
	bool bSetTable = false;
	if (m_ctrlTable->IsShown())
		bSetTable = m_ctrlTable->GetValue();
	m_Run->GetScoring().SetHasTable(bSetTable);
	SetMinYPS();
	SetYPS();
	SetObstacles();
}


void CDlgRunPanelScore::OnSelchangeQ(wxCommandEvent& evt)
{
	ARB_Q q;
	int index = m_ctrlQ->GetSelection();
	if (wxNOT_FOUND != index)
		q = m_ctrlQ->GetQ(index);
	m_Run->SetQ(q);
	SetTitlePoints();
}


void CDlgRunPanelScore::OnSCTChange(wxCommandEvent& evt)
{
	// See DlgCalendar.cpp on why TransferDataFromWindow is bad here!
	StringUtil::ToDouble(StringUtil::stringW(m_ctrlSCT->GetValue()), m_SCT);
	m_Run->GetScoring().SetSCT(m_SCT);
	SetMinYPS();
	SetObstacles();
	SetTotalFaults();
	SetTitlePoints();
}


void CDlgRunPanelScore::OnClosingTimeChange(wxCommandEvent& evt)
{
	StringUtil::ToDouble(StringUtil::stringW(m_ctrlMinYPSClosingTime->GetValue()), m_SCT2);
	m_Run->GetScoring().SetSCT2(m_SCT2);
	SetObstacles();
}


void CDlgRunPanelScore::OnNumObsChange(wxCommandEvent& evt)
{
	m_Obstacles = static_cast<short>(wxAtol(m_ctrlObstacles->GetValue()));
	m_Run->GetScoring().SetObstacles(m_Obstacles);
	SetObstacles();
}


void CDlgRunPanelScore::OnReqOpeningYPSChange(wxCommandEvent& evt)
{
	ARBConfigScoringPtr pScoring;
	if (GetScoring(&pScoring))
	{
		short val = static_cast<short>(wxAtol(m_ctrlYardsReqOpeningPts->GetValue()));
		switch (pScoring->GetScoringStyle())
		{
		case ARBScoringStyle::Unknown:
		case ARBScoringStyle::FaultsThenTime:
		case ARBScoringStyle::Faults100ThenTime:
		case ARBScoringStyle::Faults200ThenTime:
		case ARBScoringStyle::TimePlusFaults:
			m_Yards = val;
			m_Run->GetScoring().SetYards(m_Yards);
			SetMinYPS();
			SetYPS();
			SetTotalFaults();
			break;
		case ARBScoringStyle::ScoreThenTime:
		case ARBScoringStyle::OCScoreThenTime:
			m_Opening = val;
			m_Run->GetScoring().SetNeedOpenPts(m_Opening);
			SetTitlePoints();
			break;
		case ARBScoringStyle::TimeNoPlaces:
			m_Yards = val;
			m_Run->GetScoring().SetYards(m_Yards);
			SetYPS();
			break;
		}
	}
}


void CDlgRunPanelScore::OnReqClosingChange(wxCommandEvent& evt)
{
	m_Closing = static_cast<short>(wxAtol(m_ctrlClosing->GetValue()));
	m_Run->GetScoring().SetNeedClosePts(m_Closing);
	SetTitlePoints();
}


void CDlgRunPanelScore::OnTimeChange(wxCommandEvent& evt)
{
	StringUtil::ToDouble(StringUtil::stringW(m_ctrlTime->GetValue()), m_Time);
	m_Run->GetScoring().SetTime(m_Time);
	SetYPS();
	SetObstacles();
	SetTotalFaults();
	SetTitlePoints();
}


void CDlgRunPanelScore::OnFaultsChange(wxCommandEvent& evt)
{
	m_Faults = static_cast<short>(wxAtol(m_ctrlFaults->GetValue()));
	m_Run->GetScoring().SetCourseFaults(m_Faults);
	SetTotalFaults();
	SetTitlePoints();
}


void CDlgRunPanelScore::OnOpenChange(wxCommandEvent& evt)
{
	m_Open = static_cast<short>(wxAtol(m_ctrlYPSOpeningPts->GetValue()));
	m_Run->GetScoring().SetOpenPts(m_Open);
	SetTitlePoints();
}


void CDlgRunPanelScore::OnCloseChange(wxCommandEvent& evt)
{
	m_Close = static_cast<short>(wxAtol(m_ctrlClosingPtsTotalFaults->GetValue()));
	m_Run->GetScoring().SetClosePts(m_Close);
	SetTitlePoints();
}


void CDlgRunPanelScore::OnPlaceChange(wxCommandEvent& evt)
{
	m_Place = static_cast<short>(wxAtol(m_ctrlPlace->GetValue()));
	m_Run->SetPlace(m_Place);
	SetTitlePoints();
}


void CDlgRunPanelScore::OnInClassChange(wxCommandEvent& evt)
{
	m_InClass = static_cast<short>(wxAtol(m_ctrlInClass->GetValue()));
	m_Run->SetInClass(m_InClass);
	SetTitlePoints();
}


void CDlgRunPanelScore::OnBonusChange(wxCommandEvent& evt)
{
	StringUtil::ToDouble(StringUtil::stringW(m_ctrlBonusTitlePts->GetValue()), m_BonusTitlePts);
	m_Run->GetScoring().SetBonusTitlePts(m_BonusTitlePts);
	SetTitlePoints();
}

} // namespace dconSoft
