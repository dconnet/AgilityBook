#pragma once

/*
 * Copyright © 2005 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgFilter class
 * @author David Connet
 *
 * Revision History
 * @li 2005-08-18 DRC Separated options and filters into two dialogs.
 */

#include "DlgBaseSheet.h"
#include "DlgFilterDate.h"
#include "DlgFilterRuns.h"
#include "DlgFilterTraining.h"
class CAgilityBookDoc;

/////////////////////////////////////////////////////////////////////////////
// CDlgFilter

class CDlgFilter : public CDlgBaseSheet
{
	DECLARE_DYNAMIC(CDlgFilter)
public:
	static int GetDatePage()		{return 0;}
	static int GetRunsPage()		{return 1;}
	static int GetTrainingPage()	{return 2;}

	CDlgFilter(
			CAgilityBookDoc* pDoc,
			CWnd* pParentWnd = NULL,
			UINT iSelectPage = 0);
	virtual ~CDlgFilter();

// Attributes
public:
	CAgilityBookDoc* m_pDoc;
	CDlgFilterDate m_pageDate;
	CDlgFilterRuns m_pageRuns;
	CDlgFilterTraining m_pageTraining;

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CDlgFilter)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CDlgFilter)
	afx_msg void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
