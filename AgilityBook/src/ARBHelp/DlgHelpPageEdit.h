#pragma once

/*
 * Copyright © 2007-2008 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgHelpPageEdit class
 * @author David Connet
 *
 * Revision History
 * @li 2007-02-08 DRC Created
 */

#include "resource.h"
#include <vector>

class CDlgHelpPageEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgHelpPageEdit)
public:
	CDlgHelpPageEdit();
	~CDlgHelpPageEdit();

private:
// Dialog Data
	//{{AFX_DATA(CDlgHelpPageEdit)
	enum { IDD = IDD_HELP_PAGE4 };
	CEdit	m_ctrlEdit;
	//}}AFX_DATA
	std::vector<CString> m_TmpFiles;

// Overrides
	//{{AFX_VIRTUAL(CDlgHelpPageEdit)
public:
	virtual BOOL OnSetActive();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateText();
	//{{AFX_MSG(CDlgHelpPageEdit)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
