#pragma once

/*
 * Copyright © 2007 David Connet. All Rights Reserved.
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
 * @brief Web authentication
 * @author David Connet
 *
 * Revision History
 * @li 2007-08-03 DRC Created
 */

#include "DlgBaseDialog.h"

class CDlgAuthenticate : public CDlgBaseDialog
{
	DECLARE_DYNAMIC(CDlgAuthenticate)

public:
	CDlgAuthenticate(CString const& userNameHint, CWnd* pParent = NULL);
	LPCTSTR GetUserName() const		{return (LPCTSTR)m_UserName;}
	LPCTSTR GetPassword() const		{return (LPCTSTR)m_Password;}

private:
// Dialog Data
	//{{AFX_DATA(CDlgAuthenticate)
	enum { IDD = IDD_AUTHENTICATE };
	CString m_UserName;
	static CString m_Password;
	//}}AFX_DATA
	CString const& m_userNameHint; // Used to allow different logins to be remembered

	//{{AFX_VIRTUAL(CDlgAuthenticate)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDlgAuthenticate)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
