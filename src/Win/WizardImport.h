#pragma once

/*
 * Copyright © 2003-2006 David Connet. All Rights Reserved.
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
 * @brief Import Wizard
 * @author David Connet
 *
 * Revision History
 * @li 2004-01-04 DRC Implemented import, except for trials/runs.
 * @li 2003-12-10 DRC Created
 */

#include "AgilityBookOptions.h"
#include "DlgBasePropertyPage.h"
#include "ListCtrl.h"
#include <vector>
class CAgilityBookDoc;
class CWizard;

class CWizardImport : public CDlgBasePropertyPage
{
	DECLARE_DYNAMIC(CWizardImport)

// Construction
public:
	CWizardImport(
			CWizard* pSheet,
			CAgilityBookDoc* pDoc);
	~CWizardImport();
	void ResetData();

private:
// Dialog Data
	//{{AFX_DATA(CWizardImport)
	enum { IDD = IDD_WIZARD_IMPORT };
	int		m_Row;
	CSpinButtonCtrl	m_ctrlSpin;
	int		m_Delim;
	CString	m_Delimiter;
	CButton	m_ctrlAssign;
	CComboBox	m_ctrlDateFormat;
	CStatic	m_ctrlPreviewFile;
	CListCtrl2	m_ctrlPreview;
	//}}AFX_DATA
	CWizard* m_pSheet;
	CAgilityBookDoc* m_pDoc;
	CString m_FileName;
	CStringArray m_FileData; ///< For reading generic text files.
	std::vector< std::vector<CString> > m_ExcelData; ///< For reading excel directly.

private:
	CAgilityBookOptions::ColumnOrder GetColumnInfo() const;
	CString GetDelim() const;
	void UpdateButtons();
	void UpdatePreview();

// Overrides
	//{{AFX_VIRTUAL(CWizardImport)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWizardImport)
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposImportRowSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnImportKillFocus();
	afx_msg void OnImportDelim();
	afx_msg void OnImportAssign();
	afx_msg void OnImportFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
