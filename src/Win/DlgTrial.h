#pragma once

/*
 * Copyright � 2002-2005 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgTrial class
 * @author David Connet
 *
 * Revision History
 * @li 2004-12-19 DRC Added Location/Club note information.
 */

#include "ARBDogClub.h"
#include "DlgBaseDialog.h"
#include "ListCtrl.h"
class ARBDogTrial;
class CAgilityBookDoc;

class CDlgTrial : public CDlgBaseDialog
{
public:
	CDlgTrial(CAgilityBookDoc* pDoc, ARBDogTrial* pTrial, CWnd* pParent = NULL);
	bool RunsWereDeleted() const		{return m_bRunsDeleted;}

private:
// Dialog Data
	//{{AFX_DATA(CDlgTrial)
	enum { IDD = IDD_TRIAL };
	CString	m_Location;
	CComboBox	m_ctrlLocation;
	BOOL	m_Verified;
	CString	m_Notes;
	CEdit	m_ctrlLocationInfo;
	CButton	m_ctrlEdit;
	CButton	m_ctrlDelete;
	CListCtrl2	m_ctrlClubs;
	CEdit	m_ctrlClubInfo;
	//}}AFX_DATA
	CAgilityBookDoc* m_pDoc;
	ARBDogTrial* m_pTrial;
	ARBDogClubList m_Clubs;
	bool m_bRunsDeleted;

	//{{AFX_VIRTUAL(CDlgTrial)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	void UpdateNotes(bool bLocation, bool bClub);
	void ListClubs();

// Implementation
protected:
	//{{AFX_MSG(CDlgTrial)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeLocation();
	afx_msg void OnKillfocusLocation();
	afx_msg void OnItemchangedClubs(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkClubs(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClubNew();
	afx_msg void OnClubEdit();
	afx_msg void OnClubDelete();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
