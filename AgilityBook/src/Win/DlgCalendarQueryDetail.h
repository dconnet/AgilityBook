#pragma once

/*
 * Copyright © 2007-2009 David Connet. All Rights Reserved.
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
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2007-10-05 DRC Created
 */

//#include "DlgBaseDialog.h"
//#include "ListCtrl.h"
#include "ARBTypes.h"
#include <map>
#include <vector>
class ARBConfig;


class CDlgCalendarQueryDetail
{
public:
	CDlgCalendarQueryDetail(
			ARBConfig const& inConfig,
			std::map<tstring, tstring> const& inLocCodes,
			std::map<tstring, tstring> const& inVenueCodes,
			wxWindow* pParent = NULL) {}
	CDlgCalendarQueryDetail(
			ARBConfig const& inConfig,
			std::map<tstring, tstring> const& inLocCodes,
			std::vector<tstring> const& inSelectedLocCodes,
			std::map<tstring, tstring> const& inVenueCodes,
			std::vector<tstring> const& inSelectedVenueCodes,
			wxWindow* pParent = NULL) {}
	std::map<tstring, tstring> const& GetLocationCodes() const
	{
		return m_LocCodes;
	}
	std::map<tstring, tstring> const& GetVenueCodes() const
	{
		return m_VenueCodes;
	}
	std::vector<tstring> const& GetSelectedLocationCodes() const
	{
		return m_Locations;
	}
	std::vector<tstring> const& GetSelectedVenueCodes() const
	{
		return m_Venues;
	}
	int ShowModal()
	{
		wxMessageBox(wxT("CDlgCalendarQueryDetail"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_INFORMATION);
		return wxID_CANCEL;
	}
private:
	std::map<tstring, tstring> m_LocCodes;
	std::map<tstring, tstring> m_VenueCodes;
	std::vector<tstring> m_Locations;
	std::vector<tstring> m_Venues;
};

#if 0
class CDlgCalendarQueryDetail : public CDlgBaseDialog
{
public:
	// For configuring what codes are available
	CDlgCalendarQueryDetail(
			ARBConfig const& inConfig,
			std::map<tstring, tstring> const& inLocCodes,
			std::map<tstring, tstring> const& inVenueCodes,
			CWnd* pParent = NULL);
	// For selecting from available list
	CDlgCalendarQueryDetail(
			ARBConfig const& inConfig,
			std::map<tstring, tstring> const& inLocCodes,
			std::vector<tstring> const& inSelectedLocCodes,
			std::map<tstring, tstring> const& inVenueCodes,
			std::vector<tstring> const& inSelectedVenueCodes,
			CWnd* pParent = NULL);

	std::map<tstring, tstring> const& GetLocationCodes() const
	{
		return m_LocCodes;
	}
	std::map<tstring, tstring> const& GetVenueCodes() const
	{
		return m_VenueCodes;
	}
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
	CListCtrl2	m_ctrlLocations;
	CListCtrl2	m_ctrlVenues;
	CButton	m_ctrlNewLoc;
	CButton	m_ctrlEditLoc;
	CButton	m_ctrlDeleteLoc;
	CButton	m_ctrlNewVenue;
	CButton	m_ctrlEditVenue;
	CButton	m_ctrlDeleteVenue;
	//}}AFX_DATA
	bool m_EditCodes;
	ARBConfig const& m_Config;
	std::map<tstring, tstring> m_LocCodes;
	std::map<tstring, tstring> m_VenueCodes;
	std::vector<tstring> m_Locations;
	std::vector<tstring> m_Venues;

	void UpdateButtons();

	//{{AFX_VIRTUAL(CDlgCalendarQueryDetail)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDlgCalendarQueryDetail)
	virtual BOOL OnInitDialog();
	afx_msg void OnNMDblclkQueryLocations(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedQueryLocations(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkQueryVenues(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedQueryVenues(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNewLocationCode();
	afx_msg void OnEditLocationCode();
	afx_msg void OnDeleteLocationCode();
	afx_msg void OnNewVenueCode();
	afx_msg void OnEditVenueCode();
	afx_msg void OnDeleteVenueCode();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
