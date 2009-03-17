#pragma once

/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
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
class CAgilityBookDoc;
class CDlgDogDivData;
class CDlgDogLevelData;
class CMetaDataDisplay;
class CQualifyingComboBox;
class CReportListCtrl;
class CRichEditCtrl2;
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
	//bool GetText(
	//		CEdit* pEdit,
	//		short& val) const;
	//bool GetText(
	//		CEdit* pEdit,
	//		double& val) const;
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
	void UpdateControls(bool bOnEventChange = false);

	void SetFaultsText();
	CDlgDogRefRunDataPtr GetReferenceData(long index) const;
	bool IsRefRunMe();
	void CreateRefRunMe();
	void UpdateRefRunButtons();
	void SetRefRunColumnHeaders();
	void ListRefRuns();
	void GetAllHeights(std::set<tstring>& outNames);
	void GetAllCallNames(std::set<tstring>& outNames);
	void GetAllBreeds(std::set<tstring>& outNames);
	void EditRefRun();
	void UpdateCRCDButtons();
	void UpdateLinksButtons();
	void ListLinkFiles(wxChar const* pItem);
	int GetImageIndex(tstring const& inLink);
	void EditLink();

	CAgilityBookDoc* m_pDoc;
	ARBDogTrialPtr m_pTrial;
	ARBDogRunPtr m_pRealRun;
	ARBDogRunPtr m_Run;
	ARBDogReferenceRunPtr m_pRefRunMe;
	ARBDogClubPtr m_Club;
	ARBConfigVenuePtr m_pVenue;

	ARBDate m_Date;
	//CDateTimeCtrl	m_ctrlDate;
	wxComboBox* m_ctrlDivisions;
	wxComboBox* m_ctrlLevels;
	wxComboBox* m_ctrlEvents;
	wxStaticText* m_ctrlSubNamesText;
	wxComboBox* m_ctrlSubNames;
	wxString m_SubName;
	CRichEditCtrl2* m_ctrlDesc;
	//CComboBox2	m_ctrlHeight;
	wxString m_Height;
	wxComboBox* m_ctrlJudge;
	wxString m_Judge;
	//CNoteButton	m_ctrlJudgeNotes;
	//CComboBox2	m_ctrlHandler;
	wxString m_Handler;
	//CEdit	m_ctrlConditions;
	wxString m_Conditions;
	//CButton	m_ctrlPartnerEdit;
	wxTextCtrl* m_ctrlPartner;
	wxCheckBox* m_ctrlTable;
	//CStatic	m_ctrlSCTText;
	wxTextCtrl* m_ctrlSCT;
	//double	m_SCT;
	//CStatic	m_ctrlOpeningText;
	//CEdit	m_ctrlOpening;
	//short	m_Opening;
	//CStatic	m_ctrlYardsText;
	//CEdit	m_ctrlYards;
	//double	m_Yards;
	//CStatic	m_ctrlSCT2Text;
	//CEdit	m_ctrlSCT2;
	//double	m_SCT2;
	//CStatic	m_ctrlClosingText;
	//CEdit	m_ctrlClosing;
	//short	m_Closing;
	//CStatic	m_ctrlObstaclesText;
	//CEdit	m_ctrlObstacles;
	//short	m_Obstacles;
	//CStatic	m_ctrlTimeText;
	//CEdit	m_ctrlTime;
	//double	m_Time;
	//CStatic	m_ctrlOpenText;
	//CEdit	m_ctrlOpen;
	//short	m_Open;
	//CStatic	m_ctrlMinYPSText;
	wxTextCtrl* m_ctrlMinYPSClosingTime;
	//CStatic	m_ctrlYPSText;
	wxTextCtrl* m_ctrlYPSOpeningPts;
	//CStatic	m_ctrlFaultsText;
	//CEdit	m_ctrlFaults;
	//short	m_Faults;
	//CStatic	m_ctrlCloseText;
	//CEdit	m_ctrlClose;
	//short	m_Close;
	//CStatic m_ctrlTotalFaultsText;
	wxTextCtrl* m_ctrlClosingPtsTotalFaults;
	//CStatic	m_ctrlObstaclesPSText;
	wxStaticText* m_ctrlObstaclesPS;
	//CEdit	m_ctrlPlace;
	//short	m_Place;
	//CEdit	m_ctrlInClass;
	//short	m_InClass;
	//CEdit	m_ctrlDogsQd;
	//short	m_DogsQd;
	CQualifyingComboBox* m_ctrlQ;
	//CStatic	m_ctrlBonusPtsText;
	wxTextCtrl* m_ctrlBonusPts;
	//short	m_BonusPts;
	//CStatic	m_ctrlSpeedPtsText;
	wxStaticText* m_ctrlSpeedPts;
	//CStatic	m_ctrlTitlePointsText;
	wxStaticText* m_ctrlTitlePoints;
	wxStaticText* m_ctrlScore;
	//CButton	m_ctrlOtherPoints;
	//}}AFX_DATA
	//CAgilityBookDoc* m_pDoc;
	//ARBConfigVenuePtr m_pVenue;
	//ARBDogTrialPtr m_pTrial;
	//ARBDogRunPtr m_pRealRun;
	//ARBDogRunPtr m_Run;
	// 0: Current dlg, 1: text for scoring without open/close (like snooker)
	//CString m_strOpening[2];
	//CString m_strOpen[2];

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

	wxListView* m_ctrlLinks;
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
	// Comments
	void OnCommentsFaults(wxCommandEvent& evt);
	// RefRuns
	void OnRefRunColumnClick(wxListEvent& evt);
	void OnRefRunItemSelected(wxListEvent& evt);
	void OnRefRunDoubleClick(wxMouseEvent& evt);
	void OnRefRunKeyDown(wxListEvent& evt);
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
	void OnLinksDoubleClick(wxMouseEvent& evt);
	void OnLinksKeyDown(wxListEvent& evt);
	void OnLinksNew(wxCommandEvent& evt);
	void OnLinksEdit(wxCommandEvent& evt);
	void OnLinksDelete(wxCommandEvent& evt);
	void OnLinksOpen(wxCommandEvent& evt);
};
