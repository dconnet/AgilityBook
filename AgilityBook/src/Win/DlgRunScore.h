#pragma once

/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
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
 */

#include <vector>
class ARBConfigEvent;
class ARBConfigOtherPoints;
class ARBConfigScoring;
class ARBConfigVenue;
class ARBDogClub;
class ARBDogRun;
class ARBDogTrial;
class CAgilityBookDoc;

class CDlgRunScore : public CPropertyPage
{
public:
	CDlgRunScore(CAgilityBookDoc* pDoc, const ARBConfigVenue* pVenue,
		const ARBDogTrial* pTrial, const ARBDogRun* pRealRun, ARBDogRun* pRun);
	~CDlgRunScore();

private:
// Dialog Data
	//{{AFX_DATA(CDlgRunScore)
	enum { IDD = IDD_RUN_SCORE };
	CTime	m_Date;
	CString	m_Venue;
	CString	m_Club;
	CString	m_Location;
	CComboBox	m_ctrlDivisions;
	CComboBox	m_ctrlLevels;
	CComboBox	m_ctrlEvents;
	CComboBox	m_ctrlHeight;
	CString	m_Height;
	CComboBox	m_ctrlJudge;
	CString	m_Judge;
	CEdit	m_ctrlConditions;
	CString	m_Conditions;
	CButton	m_ctrlPartnerEdit;
	CEdit	m_ctrlPartner;
	CComboBox	m_ctrlHandler;
	CString	m_Handler;
	CStatic	m_ctrlFaultsText;
	CEdit	m_ctrlFaults;
	short	m_Faults;
	CStatic	m_ctrlTime4FaultText;
	CEdit	m_ctrlTime4Fault;
	double	m_Time4Fault;
	CStatic	m_ctrlYardsText;
	CEdit	m_ctrlYards;
	double	m_Yards;
	CStatic	m_ctrlYPSText;
	CStatic	m_ctrlYPS;
	CStatic	m_ctrlSCTText;
	CEdit	m_ctrlSCT;
	double	m_SCT;
	CStatic m_ctrlTotalFaultsText;
	CStatic	m_ctrlTotalFaults;
	CStatic	m_ctrlOpeningText;
	CEdit	m_ctrlOpening;
	short	m_Opening;
	CStatic	m_ctrlClosingText;
	CEdit	m_ctrlClosing;
	short	m_Closing;
	CStatic	m_ctrlTime4ScoreText;
	CEdit	m_ctrlTime4Score;
	double	m_Time4Score;
	CStatic	m_ctrlOpenText;
	CEdit	m_ctrlOpen;
	short	m_Open;
	CStatic	m_ctrlCloseText;
	CEdit	m_ctrlClose;
	short	m_Close;
	CEdit	m_ctrlPlace;
	short	m_Place;
	CEdit	m_ctrlInClass;
	short	m_InClass;
	CEdit	m_ctrlDogsQd;
	short	m_DogsQd;
	CComboBox	m_ctrlQ;
	CStatic	m_ctrlScore;
	CStatic	m_ctrlTitlePointsText;
	CStatic	m_ctrlTitlePoints;
	CStatic	m_ctrlMachPtsText;
	CStatic	m_ctrlMachPts;
	CButton	m_ctrlDoubleQ;
	CButton	m_ctrlOtherPoints;
	//}}AFX_DATA
	CAgilityBookDoc* m_pDoc;
	const ARBConfigVenue* m_pVenue;
	const ARBDogTrial* m_pTrial;
	const ARBDogRun* m_pRealRun;
	ARBDogRun* m_Run;
	// 0: Current dlg, 1: text for scoring without open/close (like snooker)
	CString m_strOpening[2];
	CString m_strOpen[2];

	//{{AFX_VIRTUAL(CDlgRunScore)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	bool GetText(CEdit* pEdit, short& val) const;
	bool GetText(CEdit* pEdit, double& val) const;
	const ARBConfigEvent* GetEvent() const;
	void ClearLevels();
	void FillLevels();
	void FillEvents();
	void SetPartnerText();
	void SetYPS();
	void SetTotalFaults();
	void FillQ(bool bHasSuperQ);
	void SetDoubleQ();
	void SetTitlePoints();
	void UpdateControls();

// Implementation
protected:
	//{{AFX_MSG(CDlgRunScore)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeDivision();
	afx_msg void OnSelchangeLevel();
	afx_msg void OnSelchangeEvent();
	afx_msg void OnPartnersEdit();
	afx_msg void OnOtherpoints();
	afx_msg void OnKillfocusFaults();
	afx_msg void OnKillfocusTime4fault();
	afx_msg void OnKillfocusYards();
	afx_msg void OnKillfocusSct();
	afx_msg void OnKillfocusOpening();
	afx_msg void OnKillfocusClosing();
	afx_msg void OnKillfocusTime4score();
	afx_msg void OnKillfocusOpen();
	afx_msg void OnKillfocusClose();
	afx_msg void OnKillfocusPlace();
	afx_msg void OnSelchangeQ();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
