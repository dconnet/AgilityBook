#pragma once

/*
 * Copyright © 2004 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgConfigTitle class
 * @author David Connet
 *
 * Revision History
 * @li 2004-01-05 DRC Created.
 */

#include <set>
#include <string>
#include "ListCtrl.h"
#include "ARBConfigTitle.h"

class CDlgConfigTitle : public CDialog
{
public:
	CDlgConfigTitle(const ARBConfigTitleList& titles,
		LPCTSTR name,
		const ARBConfigTitle* pTitle,
		CWnd* pParent = NULL);
	virtual ~CDlgConfigTitle();

	std::string GetName() const;
	void SetTitleData(ARBConfigTitle* pTitle);

private:
// Dialog Data
	//{{AFX_DATA(CDlgConfigTitle)
	enum { IDD = IDD_CONFIG_TITLE };
	CString m_Name;
	CString m_LongName;
	CListCtrl2 m_ctrlRequires;
	CString m_Desc;
	//}}AFX_DATA
	const ARBConfigTitleList& m_Titles;
	ARBConfigTitleRequiresList m_ReqTitles;

	//{{AFX_VIRTUAL(CDlgConfigTitle)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	void UpdateButtons();
	bool VerifyTitle(const ARBConfigTitle* pTitleToReplace, const ARBConfigTitle* pTitleToAdd);
	//{{AFX_MSG(CDlgConfigTitle)
	virtual BOOL OnInitDialog();
	afx_msg void OnLvnItemchangedTitles(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkTitles(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedEdit();
	afx_msg void OnBnClickedDelete();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};
