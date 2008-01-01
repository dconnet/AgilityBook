#pragma once

/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgOptionsFilter class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-12-18 DRC Added Opening/Closing dates to view, plus color.
 * @li 2003-08-09 DRC Moved fonts to new page.
 */

#include <set>
#include "CheckTreeCtrl.h"
#include "ComboBox.h"
#include "DlgBasePropertyPage.h"
#include "FilterOptions.h"

class CDlgOptionsFilter : public CDlgBasePropertyPage
{
	friend class CDlgOptions;
	DECLARE_DYNAMIC(CDlgOptionsFilter)
public:
	CDlgOptionsFilter(CAgilityBookDoc* pDoc);
	~CDlgOptionsFilter();

private:
// Dialog Data
	//{{AFX_DATA(CDlgOptionsFilter)
	enum { IDD = IDD_VIEW_OPTIONS_FILTER };
	int		m_ViewDates;
	CButton	m_ctrlDateStartCheck;
	BOOL	m_bDateStart;
	CDateTimeCtrl	m_ctrlDateStart;
	CTime	m_timeStart;
	CButton	m_ctrlDateEndCheck;
	BOOL	m_bDateEnd;
	CDateTimeCtrl	m_ctrlDateEnd;
	CTime	m_timeEnd;
	int		m_ViewNames;
	CCheckTreeCtrl	m_ctrlNames;
	BOOL	m_bNotEntered;
	BOOL	m_bPlanning;
	BOOL	m_bEntered;
	int		m_ViewVenues;
	CCheckTreeCtrl	m_ctrlVenue;
	int		m_ViewQs;
	CComboBox2	m_ctrlFilters;
	CString	m_FilterName;
	CButton	m_ctrlSaveName;
	CButton	m_ctrlDelName;
	//}}AFX_DATA
	CAgilityBookDoc* m_pDoc;
	CFilterOptions m_FilterOptions;

// Overrides
	//{{AFX_VIRTUAL(CDlgOptionsFilter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FillControls();
	void FillFilter(
			HTREEITEM hItem,
			CString path,
			std::vector<CVenueFilter>& outVenues);
	void UpdateControls();
	//{{AFX_MSG(CDlgOptionsFilter)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnSetdispinfoNames(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetdispinfoVenues(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateFilters();
	afx_msg void OnSelchangeFilterNames();
	afx_msg void OnBnClickedOptFilterNamesSave();
	afx_msg void OnBnClickedOptFilterNamesDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
