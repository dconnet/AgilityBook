#pragma once

/*
 * Copyright © 2005-2006 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgConfigMultiQ class
 * @author David Connet
 *
 * Revision History
 * @li 2005-07-30 DRC Created.
 */

#include "ARBDate.h"
#include "DlgBaseDialog.h"
#include "ListCtrl.h"
class ARBConfigVenue;
class ARBConfigMultiQ;

class CDlgConfigMultiQ : public CDlgBaseDialog
{
public:
	CDlgConfigMultiQ(
			ARBConfigVenuePtr inVenue,
			ARBConfigMultiQPtr inMultiQ,
			CWnd* pParent = NULL);
	virtual ~CDlgConfigMultiQ();

private:
// Dialog Data
	//{{AFX_DATA(CDlgConfigMultiQ)
	enum { IDD = IDD_CONFIG_MULTI_Q };
	CString	m_Name;
	CString	m_ShortName;
	BOOL	m_bFrom;
	CDateTimeCtrl	m_ctrlDateFrom;
	ARBDate	m_DateFrom;
	BOOL	m_bTo;
	CDateTimeCtrl	m_ctrlDateTo;
	ARBDate	m_DateTo;
	CListCtrl2	m_ctrlItems;
	CButton	m_ctrlEdit;
	CButton	m_ctrlRemove;
	//}}AFX_DATA
	ARBConfigVenuePtr m_pVenue;
	ARBConfigMultiQPtr m_pMultiQ;

	//{{AFX_VIRTUAL(CDlgConfigMultiQ)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	void UpdateControls();
	//{{AFX_MSG(CDlgConfigMultiQ)
	virtual BOOL OnInitDialog();
	afx_msg void OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickFrom();
	afx_msg void OnClickTo();
	afx_msg void OnAdd();
	afx_msg void OnEdit();
	afx_msg void OnRemove();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
