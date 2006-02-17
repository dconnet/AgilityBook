#pragma once

/*
 * Copyright © 2002-2006 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgOptionsView class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-12-18 DRC Added Opening/Closing dates to view, plus color.
 * @li 2003-08-09 DRC Moved fonts to new page.
 */

#include "AgilityBookOptions.h"
#include "ComboBox.h"
#include "DlgBasePropertyPage.h"

class CDlgOptionsView : public CDlgBasePropertyPage
{
	friend class CDlgOptions;
	DECLARE_DYNAMIC(CDlgOptionsView)
public:
	CDlgOptionsView();
	~CDlgOptionsView();

private:
// Dialog Data
	//{{AFX_DATA(CDlgOptionsView)
	enum { IDD = IDD_VIEW_OPTIONS_VIEWING };
	CStatic	m_ctrlFontPrint;
	BOOL	m_bOpeningNear;
	CEdit	m_ctrlOpeningNear;
	int		m_nOpeningNear;
	CStatic	m_ctrlOpeningNearColor;
	CButton	m_ctrlOpeningNearSet;
	BOOL	m_bClosingNear;
	CEdit	m_ctrlClosingNear;
	int		m_nClosingNear;
	CStatic	m_ctrlClosingNearColor;
	CButton	m_ctrlClosingNearSet;
	int		m_DayOfWeek;
	int		m_sizeX;
	int		m_sizeY;
	BOOL	m_bAutoDelete;
	BOOL	m_bHideOld;
	UINT	m_Days;
	BOOL	m_bHideOverlapping;
	BOOL	m_bOpening;
	BOOL	m_bClosing;
	CComboBox2	m_ctrlCalEntries;
	CStatic	m_ctrlColor;
	CStatic	m_ctrlCalView;
	//}}AFX_DATA
	CFontInfo m_fontPrintInfo;
	CFontInfo m_fontCalViewInfo;
	CFont m_fontPrint;
	CFont m_fontCalView;

// Overrides
	//{{AFX_VIRTUAL(CDlgOptionsView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	//{{AFX_MSG(CDlgOptionsView)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCalEntries();
	afx_msg void OnKillfocusSize();
	afx_msg void OnFontPrint();
	afx_msg void OnCalNear();
	afx_msg void OnCalColorOpeningNear();
	afx_msg void OnCalColorClosingNear();
	afx_msg void OnCalColors();
	afx_msg void OnFontCalView();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
