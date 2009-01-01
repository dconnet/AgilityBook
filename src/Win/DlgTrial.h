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
 * @brief interface of the CDlgTrial class
 * @author David Connet
 *
 * Revision History
 * @li 2008-02-01 DRC Make 'Notes' button change selection.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-13 DRC Added direct access to Notes dialog.
 * @li 2004-12-19 DRC Added Location/Club note information.
 */

#include "ARBDogClub.h"
#include "ComboBox.h"
#include "DlgBaseDialog.h"
#include "ListCtrl.h"
#include "NoteButton.h"
#include "RichEditCtrl2.h"
class CAgilityBookDoc;

class CDlgTrial : public CDlgBaseDialog
{
public:
	CDlgTrial(
			CAgilityBookDoc* pDoc,
			ARBDogTrialPtr pTrial,
			CWnd* pParent = NULL);
	bool RunsWereDeleted() const		{return m_bRunsDeleted;}

private:
// Dialog Data
	//{{AFX_DATA(CDlgTrial)
	enum { IDD = IDD_TRIAL };
	CString	m_Location;
	CComboBox2	m_ctrlLocation;
	BOOL	m_Verified;
	CString	m_Notes;
	CNoteButton	m_ctrlLocationNotes;
	CRichEditCtrl2	m_ctrlLocationInfo;
	CButton	m_ctrlEdit;
	CButton	m_ctrlDelete;
	CListCtrl2	m_ctrlClubs;
	CNoteButton	m_ctrlClubNotes;
	CRichEditCtrl2	m_ctrlClubInfo;
	//}}AFX_DATA
	CAgilityBookDoc* m_pDoc;
	ARBDogTrialPtr m_pTrial;
	ARBDogClubList m_Clubs;
	bool m_bRunsDeleted;

	//{{AFX_VIRTUAL(CDlgTrial)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	ARBDogClubPtr GetClubData(int index) const;
	void UpdateNotes(
			bool bLocation,
			bool bClub);
	void ListLocations();
	void ListClubs(ARBDogClubPtr* inClub = NULL);

// Implementation
protected:
	//{{AFX_MSG(CDlgTrial)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeLocation();
	afx_msg void OnKillfocusLocation();
	afx_msg void OnBeginLabelEditClubs(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEditClubs(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedClubs(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkClubs(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownClubs(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLocationNotes();
	afx_msg void OnClubNotes();
	afx_msg void OnClubNew();
	afx_msg void OnClubEdit();
	afx_msg void OnClubDelete();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
