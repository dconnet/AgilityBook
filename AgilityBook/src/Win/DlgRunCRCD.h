#pragma once

/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgRunCRCD class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "ARBTypes.h"


class CDlgRunCRCD
{
public:
	CDlgRunCRCD(ARBDogRunPtr pRun) {}
};

#if 0
#include "DlgBasePropertyPage.h"

class CDlgRunCRCD : public CDlgBasePropertyPage
{
public:
	CDlgRunCRCD(ARBDogRunPtr pRun);
	~CDlgRunCRCD();

private:
// Dialog Data
	//{{AFX_DATA(CDlgRunCRCD)
	enum { IDD = IDD_RUN_CRCD };
	CButton	m_ctrlEdit;
	CButton	m_ctrlView;
	CButton	m_ctrlInsert;
	CButton	m_ctrlImage;
	CEdit	m_ctrlText;
	CStatic	m_ctrlCRCD;
	//}}AFX_DATA
	ARBDogRunPtr m_Run;
	HENHMETAFILE m_metaFile;
	HENHMETAFILE m_metaFileBack;
	bool m_ViewText;
	bool m_Insert;
	CRect m_rCRCDwin; // Original CRCD rectangle.
	CRect m_rCRCDclient;

	//{{AFX_VIRTUAL(CDlgRunCRCD)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DeleteMetaFile();
	void AdjustCRCD();
	void SetView();
	void SetCRCDData();
	//{{AFX_MSG(CDlgRunCRCD)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEdit();
	afx_msg void OnView();
	afx_msg void OnCopy();
	afx_msg void OnImage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
