#pragma once

/*
 * Copyright © 2003 David Connet. All Rights Reserved.
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
 * @brief Import/Export Wizard
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-10 DRC Created
 */

#include "ListBox.h"
class CAgilityBookDoc;
class CWizard;

class CWizardStart : public CPropertyPage
{
	DECLARE_DYNAMIC(CWizardStart)

// Construction
public:
	CWizardStart(CWizard* pSheet, CAgilityBookDoc* pDoc);
	~CWizardStart();

private:
// Dialog Data
	//{{AFX_DATA(CWizardStart)
	enum { IDD = IDD_WIZARD_START };
	CListBox2	m_ctrlList;
	CStatic	m_ctrlDesc;
	//}}AFX_DATA
	CWizard* m_pSheet;
	CAgilityBookDoc* m_pDoc;

private:
	void UpdateButtons();

// Overrides
	//{{AFX_VIRTUAL(CWizardStart)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWizardStart)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeExportList();
	afx_msg void OnDblclkExportList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
