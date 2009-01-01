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
 * @brief interface of the CDlgDog class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-02-03 DRC Broke dialog up into pages.
 * @li 2003-08-18 DRC Added a deceased date for a dog.
 */

#include "DlgBaseSheet.h"
class CAgilityBookDoc;
class CDlgDogNumbers;
class CDlgDogPoints;
class CDlgDogProperties;
class CDlgDogTitles;

class CDlgDog : public CDlgBaseSheet
{
	DECLARE_DYNAMIC(CDlgDog)
public:
	CDlgDog(
			CAgilityBookDoc* pDoc,
			ARBDogPtr pDog,
			CWnd* pParent = NULL,
			UINT iSelectPage = 0);
	virtual ~CDlgDog();

private:
	CAgilityBookDoc* m_pDoc;
	ARBDogPtr m_pDog;
	CDlgDogProperties* m_pageProp;
	CDlgDogTitles* m_pageTitles;
	CDlgDogNumbers* m_pageRegNums;
	CDlgDogPoints* m_pagePoints;
	bool m_viewHidden;

	//{{AFX_VIRTUAL(CDlgDog)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDlgDog)
	afx_msg void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};
