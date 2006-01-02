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
 * @brief interface of the CDlgOptionsProgram class
 * @author David Connet
 *
 * Revision History
 * @li 2004-08-31 DRC Added option to disable splash screen.
 * @li 2004-07-23 DRC Auto-check the config version on document open.
 * @li 2004-04-08 DRC Created
 */

#include "DlgBasePropertyPage.h"

class CDlgOptionsProgram : public CDlgBasePropertyPage
{
	friend class CDlgOptions;
	DECLARE_DYNCREATE(CDlgOptionsProgram)
public:
	CDlgOptionsProgram();
	~CDlgOptionsProgram();

private:
// Dialog Data
	//{{AFX_DATA(CDlgOptionsProgram)
	enum { IDD = IDD_VIEW_OPTIONS_PROGRAM };
	BOOL	m_bAutoCheck;
	int	m_Backups;
	BOOL	m_bAutoShow;
	BOOL	m_bShowSplash;
	CString	m_Splash;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CDlgOptionsProgram)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateButtons();
	//{{AFX_MSG(CDlgOptionsProgram)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowSplash();
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
