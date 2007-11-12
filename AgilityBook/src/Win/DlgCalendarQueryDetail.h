#pragma once

/*
 * Copyright � 2007 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgCalendarQueryDetail class
 * @author David Connet
 *
 * Revision History
 * @li 2007-10-05 DRC Created
 */

#include "DlgBaseDialog.h"
#include <map>

class CDlgCalendarQueryDetail : public CDlgBaseDialog
{
public:
	CDlgCalendarQueryDetail(
			std::map<tstring, tstring> const& inLocCodes,
			std::vector<tstring> const& inSelectedLocCodes,
			std::map<tstring, tstring> const& inVenueCodes,
			std::vector<tstring> const& inSelectedVenueCodes,
			CWnd* pParent = NULL);
	std::vector<tstring> const& GetSelectedLocationCodes() const
	{
		return m_Locations;
	}
	std::vector<tstring> const& GetSelectedVenueCodes() const
	{
		return m_Venues;
	}

private:
// Dialog Data
	//{{AFX_DATA(CDlgCalendarQueryDetail)
	enum { IDD = IDD_CALENDAR_QUERY_DETAIL };
	CListCtrl	m_ctrlLocations;
	CListCtrl	m_ctrlVenues;
	//}}AFX_DATA
	std::map<tstring, tstring> const& m_LocCodes;
	std::map<tstring, tstring> const& m_VenueCodes;
	std::vector<tstring> m_Locations;
	std::vector<tstring> m_Venues;

	//{{AFX_VIRTUAL(CDlgCalendarQueryDetail)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDlgCalendarQueryDetail)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
