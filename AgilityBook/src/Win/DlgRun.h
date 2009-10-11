#pragma once

/*
 * Copyright (c) 2002-2009 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
 */

/**
 * @file
 *
 * @brief interface of the CDlgRun class
 * @author David Connet
 *
 * Revision History
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

#include "ARBDate.h"
#include "ARBTypes.h"
#include "ColumnOrder.h"
#include <wx/notebook.h>
#include <wx/listctrl.h>
class CAgilityBookDoc;
class CDlgDogDivData;
class CDlgDogLevelData;
class CListCtrl;
class CMetaDataDisplay;
class CQualifyingComboBox;
class CReportListCtrl;
class CRichEditCtrl2;
class wxCheckBox;
class wxDateEvent;

class CDlgDogRefRunData;
typedef tr1::shared_ptr<CDlgDogRefRunData> CDlgDogRefRunDataPtr;


class CDlgRun : public wxDialog
{
public:
	CDlgRun(
			CAgilityBookDoc* pDoc,
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
	void FillDivisions();
	void FillLevels();
	void FillEvents();
	void FillSubNames();
	void FillJudges();
	void SetEventDesc(ARBConfigEventPtr inEvent);
	void SetPartnerText();
	void SetMinYPS();
	void SetYPS();
	void SetObstacles();
	void SetTotalFaults();
	void SetTitlePoints();
	void SetReadOnlyFlag(wxTextCtrl* ctrl, bool bReadOnly);
	void FixScoreLayout();
	void UpdateControls(bool bOnEventChange = false);

	void SetFaultsText();
	CDlgDogRefRunDataPtr GetReferenceData(long index) const;
	bool IsRefRunMe();
	void CreateRefRunMe();
	void UpdateRefRunButtons();
	void SetRefRunColumnHeaders();
	void ListRefRuns();
	void GetAllHeights(std::set<wxString>& outNames);
	void GetAllCallNames(std::set<wxString>& outNames);
	void GetAllBreeds(std::set<wxString>& outNames);
	void EditRefRun();
	void UpdateCRCDButtons();
	void UpdateLinksButtons();
	void ListLinkFiles(wxChar const* pItem);
	int GetImageIndex(wxString const& inLink);
	void EditLink();

	CAgilityBookDoc* m_pDoc;
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
	wxComboBox* m_ctrlSubNames;
	wxString m_SubName;
	wxCheckBox* m_ctrlTable;
	bool m_Table;
	wxComboBox* m_ctrlHeight;
	wxString m_Height;
	wxComboBox* m_ctrlJudge;
	wxString m_Judge;
	//CNoteButton m_ctrlJudgeNotes;
	wxComboBox* m_ctrlHandler;
	wxString m_Handler;
	wxTextCtrl* m_ctrlConditions;
	wxString m_Conditions;
	CRichEditCtrl2* m_ctrlDesc;
	wxButton* m_ctrlPartnerEdit;
	wxTextCtrl* m_ctrlPartner;
	wxStaticText* m_ctrlSCTText;
	wxTextCtrl* m_ctrlSCT;
	double m_SCT;
	wxStaticText* m_textYardsReqOpeningPts;
	wxTextCtrl* m_ctrlYardsReqOpeningPts;
		//CStatic m_ctrlOpeningText;
		//CEdit m_ctrlOpening;
		short m_Opening;
		//CStatic m_ctrlYardsText;
		//CEdit m_ctrlYards;
		double m_Yards;
	wxStaticText* m_textMinYPSClosingTime;
	wxTextCtrl* m_ctrlMinYPSClosingTime;
		//CStatic m_ctrlSCT2Text;
		//CEdit m_ctrlSCT2;
		double m_SCT2;
		//CStatic m_ctrlMinYPSText;
	wxStaticText* m_ctrlClosingText;
	wxTextCtrl* m_ctrlClosing;
	short m_Closing;
	wxTextCtrl* m_ctrlObstacles;
	short m_Obstacles;
	wxStaticText* m_ctrlTimeText;
	wxTextCtrl* m_ctrlTime;
	double m_Time;
	wxStaticText* m_ctrlFaultsText;
	wxTextCtrl* m_ctrlFaults;
	short m_Faults;
	wxStaticText* m_textYPSOpeningPts;
	wxTextCtrl* m_ctrlYPSOpeningPts;
		//CStatic m_ctrlOpenText;
		//CEdit m_ctrlOpen;
		short m_Open;
		//CStatic m_ctrlYPSText;
	wxStaticText* m_textClosingPtsTotalFaults;
	wxTextCtrl* m_ctrlClosingPtsTotalFaults;
		//CStatic m_ctrlCloseText;
		//CEdit m_ctrlClose;
		short m_Close;
		//CStatic m_ctrlTotalFaultsText;
	wxTextCtrl* m_ctrlObstaclesPS;
	wxTextCtrl* m_ctrlPlace;
	short m_Place;
	wxTextCtrl* m_ctrlInClass;
	short m_InClass;
	wxTextCtrl* m_ctrlDogsQd;
	short m_DogsQd;
	CQualifyingComboBox* m_ctrlQ;
	wxStaticText* m_ctrlBonusPtsText;
	wxTextCtrl* m_ctrlBonusPts;
	short m_BonusPts;
	wxStaticText* m_ctrlSpeedPtsText;
	wxTextCtrl* m_ctrlSpeedPts;
	wxStaticText* m_ctrlTitlePointsText;
	wxTextCtrl* m_ctrlTitlePoints;
	wxTextCtrl* m_ctrlScore;

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

	DECLARE_EVENT_TABLE()
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
