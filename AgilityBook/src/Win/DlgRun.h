#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgRun class
 * @author David Connet
 *
 * Revision History
 * @li 2012-05-07 DRC Added autocompletion to combo boxes.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-03-16 DRC Merged DlgRun* into here.
 * @li 2009-02-09 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * DlgRunCRCD
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * DlgRunComments
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * DlgRunLink
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-06-02 DRC Moved ShellExecute code to AgilityBook.cpp, added icons.
 * @li 2004-03-30 DRC Created
 * DlgRunReference
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-07-10 DRC Add button to add yourself to ref-runs.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2003-10-13 DRC Make ref run dlg default to perfect score.
 * DlgRunScore
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-13 DRC Added direct access to Notes dialog.
 * @li 2005-12-04 DRC Added support for NADAC bonus titling points.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-02 DRC Added subnames to events.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-09-07 DRC Time+Fault scoring shouldn't include time faults.
 * @li 2004-02-14 DRC Added Table-in-YPS flag.
 */

#include "ColumnOrder.h"
#include "ARB/ARBTypes2.h"
#include "ARBCommon/ARBDate.h"
#include <wx/listctrl.h>
class CAgilityBookDoc;
class CAutoFillComboBox;
class CDlgDogDivData;
class CDlgDogLevelData;
class CListCtrl;
class CMetaDataDisplay;
class CQualifyingComboBox;
class CReportListCtrl;
class CRichEditCtrl2;
class CTextCtrl;
class wxCheckBox;
class wxDateEvent;

class CDlgDogRefRunData;
typedef std::tr1::shared_ptr<CDlgDogRefRunData> CDlgDogRefRunDataPtr;


class CDlgRun : public wxDialog
{
	friend class CLinkDropTarget;
public:
	CDlgRun(
			CAgilityBookDoc* pDoc,
			ARBDogPtr pDog,
			ARBDogTrialPtr pTrial,
			ARBDogRunPtr pRun,
			wxWindow* pParent = NULL,
			int iSelectPage = 0);

	CDlgDogRefRunDataPtr GetReferenceDataByData(long index) const;
private:
	CDlgDogDivData* GetDivisionData(int index) const;
	CDlgDogLevelData* GetLevelData(int index) const;
	bool GetEvent(ARBConfigEventPtr* outEvent) const;
	bool GetScoring(ARBConfigScoringPtr* outScoring) const;
	void FillDivisions(bool bOnEventChange);
	void FillLevels(bool bOnEventChange);
	void FillEvents(bool bOnEventChange);
	void FillSubNames();
	void FillJudges();
	void SetEventDesc(ARBConfigEventPtr inEvent);
	void SetPartnerText();
	void SetMinYPS();
	void SetYPS();
	void SetObstacles();
	void SetTotalFaults();
	void SetTitlePoints();
	void SetReadOnlyFlag(CTextCtrl* ctrl, bool bReadOnly);
	void FixScoreLayout();
	void UpdateControls(bool bOnEventChange);

	void SetFaultsText();
	CDlgDogRefRunDataPtr GetReferenceData(long index) const;
	bool IsRefRunMe();
	void CreateRefRunMe();
	void UpdateRefRunButtons();
	void SetRefRunColumnHeaders();
	void ListRefRuns();
	void GetAllHeights(std::set<std::wstring>& outNames);
	void GetAllCallNames(std::set<std::wstring>& outNames);
	void GetAllBreeds(std::set<std::wstring>& outNames);
	void EditRefRun();
	void UpdateCRCDButtons();
	void UpdateLinksButtons();
	void ListLinkFiles(wchar_t const* pItem);
	int GetImageIndex(std::wstring const& inLink);
	void EditLink();

	CAgilityBookDoc* m_pDoc;
	ARBDogPtr m_pDog;
	ARBDogTrialPtr m_pTrial;
	ARBDogRunPtr m_pRealRun;
	ARBDogRunPtr m_Run;
	ARBDogReferenceRunPtr m_pRefRunMe;
	ARBDogClubPtr m_Club;
	ARBConfigVenuePtr m_pVenue;
	wxColour m_clrBack;

	wxPanel* m_panelScore;
	ARBDate m_Date;
	wxComboBox* m_ctrlDivisions;
	wxComboBox* m_ctrlLevels;
	wxComboBox* m_ctrlEvents;
	wxStaticText* m_ctrlSubNamesText;
	CAutoFillComboBox* m_ctrlSubNames;
	wxString m_SubName;
	wxCheckBox* m_ctrlTable;
	bool m_Table;
	CAutoFillComboBox* m_ctrlHeight;
	wxString m_Height;
	CAutoFillComboBox* m_ctrlJudge;
	wxString m_Judge;
	//CNoteButton m_ctrlJudgeNotes;
	CAutoFillComboBox* m_ctrlHandler;
	wxString m_Handler;
	CTextCtrl* m_ctrlConditions;
	wxString m_Conditions;
	CRichEditCtrl2* m_ctrlDesc;
	wxButton* m_ctrlPartnerEdit;
	CTextCtrl* m_ctrlPartner;
	wxStaticText* m_ctrlSCTText;
	CTextCtrl* m_ctrlSCT;
	double m_SCT;
	wxStaticText* m_textYardsReqOpeningPts;
	CTextCtrl* m_ctrlYardsReqOpeningPts;
		//CStatic m_ctrlOpeningText;
		//CEdit m_ctrlOpening;
		short m_Opening;
		//CStatic m_ctrlYardsText;
		//CEdit m_ctrlYards;
		double m_Yards;
	wxStaticText* m_textMinYPSClosingTime;
	CTextCtrl* m_ctrlMinYPSClosingTime;
		//CStatic m_ctrlSCT2Text;
		//CEdit m_ctrlSCT2;
		double m_SCT2;
		//CStatic m_ctrlMinYPSText;
	wxStaticText* m_ctrlClosingText;
	CTextCtrl* m_ctrlClosing;
	short m_Closing;
	CTextCtrl* m_ctrlObstacles;
	short m_Obstacles;
	wxStaticText* m_ctrlTimeText;
	CTextCtrl* m_ctrlTime;
	double m_Time;
	wxStaticText* m_ctrlFaultsText;
	CTextCtrl* m_ctrlFaults;
	short m_Faults;
	wxStaticText* m_textYPSOpeningPts;
	CTextCtrl* m_ctrlYPSOpeningPts;
		//CStatic m_ctrlOpenText;
		//CEdit m_ctrlOpen;
		short m_Open;
		//CStatic m_ctrlYPSText;
	wxStaticText* m_textClosingPtsTotalFaults;
	CTextCtrl* m_ctrlClosingPtsTotalFaults;
		//CStatic m_ctrlCloseText;
		//CEdit m_ctrlClose;
		short m_Close;
		//CStatic m_ctrlTotalFaultsText;
	CTextCtrl* m_ctrlObstaclesPS;
	CTextCtrl* m_ctrlPlace;
	short m_Place;
	CTextCtrl* m_ctrlInClass;
	short m_InClass;
	CTextCtrl* m_ctrlDogsQd;
	short m_DogsQd;
	CQualifyingComboBox* m_ctrlQ;
	wxStaticText* m_ctrlBonusPtsText;
	CTextCtrl* m_ctrlBonusPts;
	double m_BonusPts;
	wxStaticText* m_ctrlSpeedPtsText;
	CTextCtrl* m_ctrlSpeedPts;
	wxStaticText* m_ctrlTitlePointsText;
	CTextCtrl* m_ctrlTitlePoints;
	CTextCtrl* m_ctrlScore;

	wxString m_Comments;
	CColumnOrder m_sortRefRuns;
	int m_idxRefRunPage;
	wxListBox* m_ctrlFaultsList;
	CReportListCtrl* m_ctrlRefRuns;
	wxButton* m_ctrlRefAddMe;
	wxButton* m_ctrlRefEdit;
	wxButton* m_ctrlRefDelete;
	CMetaDataDisplay* m_CRCDDisplay;
	wxButton* m_ctrlCourse;
	wxButton* m_ctrlCRCDView;
	wxButton* m_ctrlCRCDCopy;
	wxCheckBox* m_ctrlIncImage;
	CListCtrl* m_ctrlLinks;
	wxImageList m_ImageList;
	int m_imgEmpty;
	int m_imgOk;
	int m_imgMissing;
	wxButton* m_ctrlLinkEdit;
	wxButton* m_ctrlLinkDelete;
	wxButton* m_ctrlLinkOpen;

	DECLARE_ON_INIT()
	DECLARE_EVENT_TABLE()
	void OnPageChanging(wxNotebookEvent& evt);
	void OnPageChanged(wxNotebookEvent& evt);
	void OnOk(wxCommandEvent& evt);
	// Score
	void OnScoreDateChanged(wxDateEvent& evt);
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
	void OnBonusChange(wxCommandEvent& evt);
	// Comments
	void OnCommentsFaults(wxCommandEvent& evt);
	// RefRuns
	void OnRefRunColumnClick(wxListEvent& evt);
	void OnRefRunItemSelected(wxListEvent& evt);
	void OnRefRunItemActivated(wxListEvent& evt);
	void OnRefRunKeyDown(wxKeyEvent& evt);
	void OnRefRunNew(wxCommandEvent& evt);
	void OnRefRunAddMe(wxCommandEvent& evt);
	void OnRefRunEdit(wxCommandEvent& evt);
	void OnRefRunDelete(wxCommandEvent& evt);
	// CRCD
	void OnCRCDEdit(wxCommandEvent& evt);
	void OnCRCDView(wxCommandEvent& evt);
	void OnCRCDCopy(wxCommandEvent& evt);
	void OnCRCDImage(wxCommandEvent& evt);
	// Links
	void OnLinksItemSelected(wxListEvent& evt);
	void OnLinksItemActivated(wxListEvent& evt);
	void OnLinksKeyDown(wxKeyEvent& evt);
	void OnLinksNew(wxCommandEvent& evt);
	void OnLinksEdit(wxCommandEvent& evt);
	void OnLinksDelete(wxCommandEvent& evt);
	void OnLinksOpen(wxCommandEvent& evt);
};
