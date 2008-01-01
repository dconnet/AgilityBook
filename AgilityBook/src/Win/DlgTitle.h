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
 * @brief interface of the CDlgTitle class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "ARBTypes.h"
#include "ComboBox.h"
#include "DlgBaseDialog.h"
#include "RichEditCtrl2.h"

class CDlgTitle : public CDlgBaseDialog
{
public:
	CDlgTitle(
			ARBConfig const& config,
			ARBDogTitleList& titles,
			ARBDogTitlePtr pTitle,
			CWnd* pParent = NULL);

private:
// Dialog Data
	//{{AFX_DATA(CDlgTitle)
	enum { IDD = IDD_TITLE };
	CButton	m_ctrlEarned;
	CDateTimeCtrl	m_ctrlDate;
	CButton	m_ctrlHidden;
	CComboBox2	m_ctrlVenues;
	CComboBox2	m_ctrlTitles;
	CButton	m_ctrlReceived;
	CRichEditCtrl2	m_ctrlDesc;
	//}}AFX_DATA
	ARBConfig const& m_Config;
	ARBDogTitleList& m_Titles;
	ARBDogTitlePtr m_pTitle;
	bool m_bInit;

	//{{AFX_VIRTUAL(CDlgTitle)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	ARBConfigVenuePtr GetVenueData(int index) const;
	ARBConfigTitlePtr GetTitleData(int index) const;
	ARBDate GetDate();
	void FillTitles();
	void FillTitleInfo();

	//{{AFX_MSG(CDlgTitle)
	virtual BOOL OnInitDialog();
	afx_msg void OnDtnDatetimechangeTitleDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedEarned();
	afx_msg void OnSelchangeVenues();
	afx_msg void OnSelchangeTitles();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
