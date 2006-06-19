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
 * @brief interface of the CDlgOtherPoint class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "ComboBox.h"
#include "DlgBaseDialog.h"
#include "RichEditCtrl2.h"

class CDlgOtherPoint : public CDlgBaseDialog
{
public:
	CDlgOtherPoint(
			ARBConfig& config,
			ARBDogRunOtherPointsPtr pRunOther,
			CWnd* pParent = NULL);

private:
// Dialog Data
	//{{AFX_DATA(CDlgOtherPoint)
	enum { IDD = IDD_OTHER_POINT };
	CComboBox2	m_ctrlOtherPoints;
	CRichEditCtrl2	m_ctrlDesc;
	short	m_Points;
	//}}AFX_DATA
	ARBConfig& m_Config;
	ARBDogRunOtherPointsPtr m_pRunOther;

	//{{AFX_VIRTUAL(CDlgOtherPoint)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void LoadPoints(ARBConfigOtherPointsPtr inOther);
	//{{AFX_MSG(CDlgOtherPoint)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeOtherpoints();
	afx_msg void OnBnClickedOtherNew();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
