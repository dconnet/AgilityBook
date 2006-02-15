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
 * @brief interface of the CDlgTraining class
 * @author David Connet
 *
 * Revision History
 * @li 2003-09-21 DRC Created
 */

#include "DlgBaseDialog.h"
class ARBTraining;
class CAgilityBookDoc;

class CDlgTraining : public CDlgBaseDialog
{
// Construction
public:
	CDlgTraining(
			ARBTrainingPtr pTraining,
			CAgilityBookDoc* pDoc,
			CWnd* pParent = NULL);

private:
// Dialog Data
	//{{AFX_DATA(CDlgTraining)
	enum { IDD = IDD_TRAINING };
	CTime	m_date;
	CComboBox	m_ctrlNames;
	CString	m_Name;
	CComboBox	m_ctrlSubNames;
	CString	m_SubName;
	CString	m_Notes;
	//}}AFX_DATA
	ARBTrainingPtr m_pTraining;
	CAgilityBookDoc* m_pDoc;

// Overrides
	//{{AFX_VIRTUAL(CDlgTraining)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDlgTraining)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
