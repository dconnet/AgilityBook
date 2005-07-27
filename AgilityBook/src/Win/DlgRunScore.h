#pragma once

/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgRunScore class
 * @author David Connet
 *
 * Revision History
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-02 DRC Added subnames to events.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-09-07 DRC Time+Fault scoring shouldn't include time faults.
 * @li 2004-02-14 DRC Added Table-in-YPS flag.
 */

#include <vector>
#include "DlgBasePropertyPage.h"
#include "RichEditCtrl2.h"
class ARBConfigEvent;
class ARBConfigOtherPoints;
class ARBConfigScoring;
class ARBConfigVenue;
class ARBDogClub;
class ARBDogRun;
class ARBDogTrial;
class CAgilityBookDoc;

class CDlgRunScore : public CDlgBasePropertyPage
{
public:
	CDlgRunScore(
			CAgilityBookDoc* pDoc,
			ARBConfigVenue* pVenue,
			ARBDogTrial const* pTrial,
			ARBDogRun const* pRealRun,
			ARBDogRun* pRun);
	~CDlgRunScore();

private:
// Dialog Data
	//{{AFX_DATA(CDlgRunScore)
	enum { IDD = IDD_RUN_SCORE };
	CDateTimeCtrl	m_ctrlDate;
	CString	m_Venue;
	CString	m_Club;
	CString	m_Location;
	CComboBox	m_ctrlDivisions;
	CComboBox	m_ctrlLevels;
	CComboBox	m_ctrlEvents;
	CStatic	m_ctrlSubNamesText;
	CComboBox	m_ctrlSubNames;
	CString	m_SubName;
	CRichEditCtrl2	m_ctrlDesc;
	CComboBox	m_ctrlHeight;
	CString	m_Height;
	CComboBox	m_ctrlJudge;
	CString	m_Judge;
	CComboBox	m_ctrlHandler;
	CString	m_Handler;
	CEdit	m_ctrlConditions;
	CString	m_Conditions;
	CButton	m_ctrlPartnerEdit;
	CEdit	m_ctrlPartner;
	CButton m_ctrlTable;
	CStatic	m_ctrlSCTText;
	CEdit	m_ctrlSCT;
	double	m_SCT;
	CStatic	m_ctrlOpeningText;
	CEdit	m_ctrlOpening;
	short	m_Opening;
	CStatic	m_ctrlYardsText;
	CEdit	m_ctrlYards;
	double	m_Yards;
	CStatic	m_ctrlSCT2Text;
	CEdit	m_ctrlSCT2;
	double	m_SCT2;
	CStatic	m_ctrlClosingText;
	CEdit	m_ctrlClosing;
	short	m_Closing;
	CStatic	m_ctrlTimeText;
	CEdit	m_ctrlTime;
	double	m_Time;
	CStatic	m_ctrlOpenText;
	CEdit	m_ctrlOpen;
	short	m_Open;
	CStatic	m_ctrlYPSText;
	CStatic	m_ctrlYPS;
	CStatic	m_ctrlFaultsText;
	CEdit	m_ctrlFaults;
	short	m_Faults;
	CStatic	m_ctrlCloseText;
	CEdit	m_ctrlClose;
	short	m_Close;
	CStatic m_ctrlTotalFaultsText;
	CStatic	m_ctrlTotalFaults;
	CEdit	m_ctrlPlace;
	short	m_Place;
	CEdit	m_ctrlInClass;
	short	m_InClass;
	CEdit	m_ctrlDogsQd;
	short	m_DogsQd;
	CComboBox	m_ctrlQ;
	CStatic	m_ctrlScore;
	CButton	m_ctrlMultiQ;
	CStatic	m_ctrlSpeedPtsText;
	CStatic	m_ctrlSpeedPts;
	CStatic	m_ctrlTitlePointsText;
	CStatic	m_ctrlTitlePoints;
	CButton	m_ctrlOtherPoints;
	//}}AFX_DATA
	CAgilityBookDoc* m_pDoc;
	ARBConfigVenue* m_pVenue;
	ARBDogTrial const* m_pTrial;
	ARBDogRun const* m_pRealRun;
	ARBDogRun* m_Run;
	// 0: Current dlg, 1: text for scoring without open/close (like snooker)
	CString m_strOpening[2];
	CString m_strOpen[2];

	//{{AFX_VIRTUAL(CDlgRunScore)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	bool GetText(
			CEdit* pEdit,
			short& val) const;
	bool GetText(
			CEdit* pEdit,
			double& val) const;
	bool GetEvent(ARBConfigEvent** outEvent) const;
	bool GetScoring(ARBConfigScoring** outScoring) const;
	void ClearLevels();
	void FillLevels();
	void FillEvents();
	void FillSubNames();
	void SetEventDesc(ARBConfigEvent const* inEvent);
	void SetPartnerText();
	void SetYPS();
	void SetTotalFaults();
	void FillQ(bool bHasSuperQ);
	void SetMultiQ();
	void SetTitlePoints();
	void UpdateControls(bool bOnEventChange = false);

// Implementation
protected:
	//{{AFX_MSG(CDlgRunScore)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnDatetimechangeDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeDivision();
	afx_msg void OnSelchangeLevel();
	afx_msg void OnSelchangeEvent();
	afx_msg void OnPartnersEdit();
	afx_msg void OnOtherpoints();
	afx_msg void OnKillfocusFaults();
	afx_msg void OnKillfocusTime();
	afx_msg void OnKillfocusYards();
	afx_msg void OnKillfocusSct();
	afx_msg void OnKillfocusSct2();
	afx_msg void OnKillfocusOpening();
	afx_msg void OnKillfocusClosing();
	afx_msg void OnKillfocusOpen();
	afx_msg void OnKillfocusClose();
	afx_msg void OnKillfocusPlace();
	afx_msg void OnBnClickedTableYps();
	afx_msg void OnSelchangeQ();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
