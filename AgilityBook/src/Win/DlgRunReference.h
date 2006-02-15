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
 * @brief interface of the CDlgRunReference class
 * @author David Connet
 *
 * Revision History
 * @li 2005-07-10 DRC Add button to add yourself to ref-runs.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2003-10-13 DRC Make ref run dlg default to perfect score.
 */

#include <set>
#include "ARBTypes.h"
#include "ColumnOrder.h"
#include "DlgBasePropertyPage.h"
#include "ListCtrl.h"
#include "ListData.h"
class CAgilityBookDoc;

class CDlgRunReference : public CDlgBasePropertyPage
{
public:
	CDlgRunReference(
			CAgilityBookDoc* pDoc,
			ARBConfigVenuePtr pVenue,
			ARBDogRunPtr pRun);
	~CDlgRunReference();

private:
// Dialog Data
	//{{AFX_DATA(CDlgRunReference)
	enum { IDD = IDD_RUN_REFERENCE };
	CListCtrl2	m_ctrlRefRuns;
	CButton	m_ctrlAdd;
	CButton	m_ctrlNew;
	CButton	m_ctrlEdit;
	CButton	m_ctrlDelete;
	//}}AFX_DATA
	CAgilityBookDoc* m_pDoc;
	ARBConfigVenuePtr m_Venue;
	ARBDogRunPtr m_Run;
	ARBDogReferenceRunPtr m_pRefRunMe;
	CColumnOrder m_sortRefRuns;

	//{{AFX_VIRTUAL(CDlgRunReference)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CListPtrData<ARBDogReferenceRunPtr>* GetReferenceData(int index) const;
	bool IsRefRunMe();
	void CreateRefRunMe();
	void UpdateButtons();
	void SetColumnHeaders();
	void ListRuns();
	void GetAllHeights(std::set<ARBString>& outNames);
	void GetAllCallNames(std::set<ARBString>& outNames);
	void GetAllBreeds(std::set<ARBString>& outNames);

	//{{AFX_MSG(CDlgRunReference)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetdispinfoRefRuns(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickRefRuns(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedRefRuns(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkRefRuns(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownRefRuns(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRefRunAdd();
	afx_msg void OnRefRunNew();
	afx_msg void OnRefRunEdit();
	afx_msg void OnRefRunDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
