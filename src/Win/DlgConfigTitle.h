#pragma once

/*
 * Copyright © 2004-2006 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgConfigTitle class
 * @author David Connet
 *
 * Revision History
 * @li 2005-01-11 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-01-05 DRC Created.
 */

#include "DlgBaseDialog.h"

class CDlgConfigTitle : public CDlgBaseDialog
{
public:
	CDlgConfigTitle(
			ARBConfigTitlePtr inTitle,
			CWnd* pParent = NULL);
	virtual ~CDlgConfigTitle();

private:
// Dialog Data
	//{{AFX_DATA(CDlgConfigTitle)
	enum { IDD = IDD_CONFIG_TITLE };
	CString m_Name;
	CString m_LongName;
	CString m_Desc;
	BOOL	m_Prefix;
	BOOL	m_AllowMany;
	CEdit	m_ctrlMultiple;
	short	m_Multiple;
	BOOL	m_DateFrom;
	CDateTimeCtrl	m_ctrlDateFrom;
	BOOL	m_DateTo;
	CDateTimeCtrl	m_ctrlDateTo;
	//}}AFX_DATA
	ARBConfigTitlePtr m_Title;

	//{{AFX_VIRTUAL(CDlgConfigTitle)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	void UpdateButtons();
	//{{AFX_MSG(CDlgConfigTitle)
	virtual BOOL OnInitDialog();
	afx_msg void OnAllowMultiple();
	afx_msg void OnCheck();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
