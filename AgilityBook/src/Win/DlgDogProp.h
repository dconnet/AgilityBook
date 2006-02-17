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
 * @brief interface of the CDlgDogProperties class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2003-08-18 DRC Added a deceased date for a dog.
 */

#include "ARBDate.h"
#include "DlgBasePropertyPage.h"
class CAgilityBookDoc;

class CDlgDogProperties : public CDlgBasePropertyPage
{
	friend class CDlgDog;
public:
	CDlgDogProperties(
			CAgilityBookDoc* pDoc,
			ARBDogPtr pDog);
	~CDlgDogProperties();

private:
// Dialog Data
	//{{AFX_DATA(CDlgDogProperties)
	enum { IDD = IDD_DOG_PROP };
	CString	m_CallName;
	CDateTimeCtrl	m_ctrlDateDOB;
	CButton	m_ctrlDeceased;
	CDateTimeCtrl	m_ctrlDateDeceased;
	CString	m_Breed;
	CStatic	m_ctrlAge;
	CString	m_RegName;
	CString	m_Notes;
	//}}AFX_DATA
	CAgilityBookDoc* m_pDoc;
	ARBDogPtr m_pDog;
	ARBDate m_DOB;
	ARBDate m_Deceased;

	//{{AFX_VIRTUAL(CDlgDogProperties)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	void UpdateAge();

// Implementation
protected:
	//{{AFX_MSG(CDlgDogProperties)
	virtual BOOL OnInitDialog();
	afx_msg void OnDatetimechange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnIsDeceased();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
