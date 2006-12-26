#pragma once

/*
 * Copyright � 2002-2007 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgSelectDog class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include <vector>
#include "DlgBaseDialog.h"
#include "ListBox.h"
class CAgilityBookDoc;

#if _MSC_VER >= 1300
#define DOMODAL_RETVAL	INT_PTR
#else
#define DOMODAL_RETVAL	int
#endif

class CDlgSelectDog : public CDlgBaseDialog
{
// Construction
public:
	CDlgSelectDog(
			CAgilityBookDoc* pDoc,
			std::vector<ARBDogPtr>& dogs,
			CWnd* pParent = NULL);

private:
// Dialog Data
	//{{AFX_DATA(CDlgSelectDog)
	enum { IDD = IDD_DOG_PICKER };
	CCheckListBox2	m_ctrlList;
	//}}AFX_DATA
	CAgilityBookDoc* m_pDoc;
	std::vector<ARBDogPtr>& m_Dogs;

// Overrides
	//{{AFX_VIRTUAL(CDlgSelectDog)
	public:
	virtual DOMODAL_RETVAL DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDlgSelectDog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
