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
 * @brief interface of the CDlgRun class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "DlgBaseSheet.h"
class CAgilityBookDoc;
class CDlgRunScore;
class CDlgRunComments;
class CDlgRunReference;
class CDlgRunCRCD;
class CDlgRunLink;

class CDlgRun : public CDlgBaseSheet
{
	DECLARE_DYNAMIC(CDlgRun)
public:
	CDlgRun(
			CAgilityBookDoc* pDoc,
			ARBDogTrialPtr pTrial,
			ARBDogRunPtr pRun,
			CWnd* pParent = NULL,
			UINT iSelectPage = 0);
	virtual ~CDlgRun();

// Attributes
private:
	CAgilityBookDoc* m_pDoc;
	ARBDogTrialPtr m_pTrial;
	ARBDogRunPtr m_pRealRun;
	ARBDogRunPtr m_Run;
	CDlgRunScore* m_pageScore;
	CDlgRunComments* m_pageComments;
	CDlgRunReference* m_pageReference;
	CDlgRunCRCD* m_pageCRCD;
	CDlgRunLink* m_pageLink;

// Overrides
	//{{AFX_VIRTUAL(CDlgRun)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CDlgRun)
	afx_msg void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
