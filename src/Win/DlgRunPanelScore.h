#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgRun class
 * @author David Connet
 *
 * Revision History
 * 2021-01-07 Separated into multiple files.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-13 Added direct access to Notes dialog.
 * 2005-12-04 Added support for NADAC bonus titling points.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2005-01-02 Added subnames to events.
 * 2005-01-01 Renamed MachPts to SpeedPts.
 * 2004-09-07 Time+Fault scoring shouldn't include time faults.
 * 2004-02-14 Added Table-in-YPS flag.
 */

#include "DlgRunPanelBase.h"

#include "ARB/ARBTypes2.h"
#include "ARBCommon/ARBDate.h"
#include <wx/listctrl.h>
class wxCheckBox;
class wxDateEvent;
class wxDatePickerCtrl;


namespace dconSoft
{
namespace ARBWin
{
class CRichEditCtrl2;
class CSpellCheckCtrl;
class CTextCtrl;
} // namespace ARBWin
class CAgilityBookDoc;
class CDlgDogDivData;
class CDlgDogLevelData;
class CDlgDogVenueData;
class CNoteButton;
class CQualifyingComboBox;


class CDlgRunPanelScore : public CDlgRunPanelBase
{
public:
	CDlgRunPanelScore(
		CDlgRun* pDlg,
		CAgilityBookDoc* pDoc,
		ARB::ARBDogPtr const& inDog,
		ARB::ARBDogTrialPtr const& inTrial,
		ARB::ARBDogRunPtr const& inRun,
		wxWindow* parent);

	wxWindow* GetInitialControl() override;
	void OnActivation() override
	{
	}
	bool Validate() override;
	bool Save() override;

	std::wstring GetCurrentVenueName() const;

private:
	CDlgDogVenueData* GetVenueData(int index) const;
	CDlgDogDivData* GetDivisionData(int index) const;
	CDlgDogLevelData* GetLevelData(int index) const;
	bool GetEvent(ARB::ARBConfigEventPtr* outEvent) const;
	bool GetScoring(ARB::ARBConfigScoringPtr* outScoring) const;
	void UpdateVenue(bool bOnEventChange);
	void FillDivisions(bool bOnEventChange);
	void FillLevels(bool bOnEventChange);
	void FillEvents(bool bOnEventChange);
	void FillSubNames();
	void FillJudges();
	void SetEventDesc(ARB::ARBConfigEventPtr const& inEvent);
	void SetPartnerText();
	void SetMinYPS();
	void SetYPS();
	void SetObstacles();
	void SetTotalFaults();
	void SetTitlePoints();
	void SetReadOnlyFlag(ARBWin::CTextCtrl* ctrl, bool bReadOnly);
	void FixScoreLayout();
	void UpdateControls(bool bOnEventChange);

	wxColour m_clrBack;

	wxDatePickerCtrl* m_ctrlDate;
	ARBCommon::ARBDate m_Date;
	wxComboBox* m_ctrlVenues;
	wxComboBox* m_ctrlDivisions;
	wxComboBox* m_ctrlLevels;
	wxComboBox* m_ctrlEvents;
	wxStaticText* m_ctrlSubNamesText;
	wxComboBox* m_ctrlSubNames;
	wxString m_SubName;
	wxCheckBox* m_ctrlTable;
	bool m_Table;
	wxStaticText* m_textHeight;
	wxComboBox* m_ctrlHeight;
	wxString m_Height;
	wxStaticText* m_textJudge;
	wxComboBox* m_ctrlJudge;
	CNoteButton* m_ctrlJudgeNote;
	wxString m_Judge;
	// CNoteButton m_ctrlJudgeNotes;
	wxComboBox* m_ctrlHandler;
	wxString m_Handler;
	ARBWin::CSpellCheckCtrl* m_ctrlConditions;
	wxString m_Conditions;
	ARBWin::CRichEditCtrl2* m_ctrlDesc;
	wxButton* m_ctrlPartnerEdit;
	ARBWin::CTextCtrl* m_ctrlPartner;
	wxStaticText* m_ctrlSCTText;
	ARBWin::CTextCtrl* m_ctrlSCT;
	double m_SCT;
	wxStaticText* m_textYardsReqOpeningPts;
	ARBWin::CTextCtrl* m_ctrlYardsReqOpeningPts;
	// CStatic m_ctrlOpeningText;
	// CEdit m_ctrlOpening;
	short m_Opening;
	// CStatic m_ctrlYardsText;
	// CEdit m_ctrlYards;
	double m_Yards;
	wxStaticText* m_textMinYPSClosingTime;
	ARBWin::CTextCtrl* m_ctrlMinYPSClosingTime;
	// CStatic m_ctrlSCT2Text;
	// CEdit m_ctrlSCT2;
	double m_SCT2;
	// CStatic m_ctrlMinYPSText;
	wxStaticText* m_ctrlClosingText;
	ARBWin::CTextCtrl* m_ctrlClosing;
	short m_Closing;
	wxStaticText* m_ctrlObstaclesText;
	ARBWin::CTextCtrl* m_ctrlObstacles;
	short m_Obstacles;
	wxStaticText* m_ctrlTimeText;
	ARBWin::CTextCtrl* m_ctrlTime;
	double m_Time;
	wxStaticText* m_ctrlFaultsText;
	ARBWin::CTextCtrl* m_ctrlFaults;
	short m_Faults;
	wxStaticText* m_textYPSOpeningPts;
	ARBWin::CTextCtrl* m_ctrlYPSOpeningPts;
	// CStatic m_ctrlOpenText;
	// CEdit m_ctrlOpen;
	short m_Open;
	// CStatic m_ctrlYPSText;
	wxStaticText* m_textClosingPtsTotalFaults;
	ARBWin::CTextCtrl* m_ctrlClosingPtsTotalFaults;
	// CStatic m_ctrlCloseText;
	// CEdit m_ctrlClose;
	short m_Close;
	// CStatic m_ctrlTotalFaultsText;
	wxStaticText* m_ctrlObstaclesPSText;
	ARBWin::CTextCtrl* m_ctrlObstaclesPS;
	wxStaticText* m_textPlace;
	ARBWin::CTextCtrl* m_ctrlPlace;
	wxStaticText* m_textPlaceOf;
	short m_Place;
	ARBWin::CTextCtrl* m_ctrlInClass;
	short m_InClass;
	wxStaticText* m_textDogsQd;
	ARBWin::CTextCtrl* m_ctrlDogsQd;
	short m_DogsQd;
	CQualifyingComboBox* m_ctrlQ;
	wxStaticText* m_ctrlBonusPtsText;
	ARBWin::CTextCtrl* m_ctrlBonusTitlePts;
	double m_BonusTitlePts;
	wxStaticText* m_ctrlSpeedPtsText;
	ARBWin::CTextCtrl* m_ctrlSpeedPts;
	wxStaticText* m_ctrlTitlePointsText;
	ARBWin::CTextCtrl* m_ctrlTitlePoints;
	ARBWin::CTextCtrl* m_ctrlScore;

	void OnScoreDateChanged(wxDateEvent& evt);
	void OnSelchangeVenue(wxCommandEvent& evt);
	void OnSelchangeDivision(wxCommandEvent& evt);
	void OnSelchangeLevel(wxCommandEvent& evt);
	void OnSelchangeEvent(wxCommandEvent& evt);
	void OnJudgeNotes(wxCommandEvent& evt);
	void OnPartnersEdit(wxCommandEvent& evt);
	void OnOtherpoints(wxCommandEvent& evt);
	void OnBnClickedTableYps(wxCommandEvent& evt);
	void OnSelchangeQ(wxCommandEvent& evt);
	void OnSCTChange(wxCommandEvent& evt);
	void OnClosingTimeChange(wxCommandEvent& evt);
	void OnNumObsChange(wxCommandEvent& evt);
	void OnReqOpeningYPSChange(wxCommandEvent& evt);
	void OnReqClosingChange(wxCommandEvent& evt);
	void OnTimeChange(wxCommandEvent& evt);
	void OnFaultsChange(wxCommandEvent& evt);
	void OnOpenChange(wxCommandEvent& evt);
	void OnCloseChange(wxCommandEvent& evt);
	void OnPlaceChange(wxCommandEvent& evt);
	void OnInClassChange(wxCommandEvent& evt);
	void OnBonusChange(wxCommandEvent& evt);
};

} // namespace dconSoft
