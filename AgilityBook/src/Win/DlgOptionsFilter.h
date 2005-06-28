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
 * @brief interface of the CDlgOptionsFilter class
 * @author David Connet
 *
 * Revision History
 */

#include <string>
#include <vector>
#include "ARBDate.h"
#include "AgilityBookOptions.h"
#include "CheckTreeCtrl.h"
#include "DlgBasePropertyPage.h"
class ARBConfig;

class CDlgOptionsFilter : public CDlgBasePropertyPage
{
	friend class CDlgOptions;
	DECLARE_DYNAMIC(CDlgOptionsFilter)
public:
	CDlgOptionsFilter(ARBConfig const& config);
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
	CTime	m_timeEnd;
	BOOL	m_bDateEnd;
	CDateTimeCtrl	m_ctrlDateEnd;
	int		m_ViewVenues;
	CCheckTreeCtrl	m_ctrlVenue;
	int		m_ViewQs;
	//}}AFX_DATA
	ARBConfig const& m_Config;
	std::vector<CVenueFilter> m_VenueFilter;

private:
	bool Find(std::string const& venue,
		std::string const& div,
		std::string const& level) const;
	void FillFilter(
			HTREEITEM hItem,
			CString path);
	void UpdateControls();

// Overrides
	//{{AFX_VIRTUAL(CDlgOptionsFilter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDlgOptionsFilter)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnViewUpdate();
	afx_msg void OnSetdispinfoVenues(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
