#pragma once

/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgInfoJudge class
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-07 DRC Created
 */

#include <set>
#include <string>
#include "ARBInfoClub.h"
#include "ARBInfoJudge.h"
#include "ARBInfoLocation.h"
#include "DlgBaseDialog.h"
class CAgilityBookDoc;

class CDlgInfoJudge : public CDlgBaseDialog
{
// Construction
public:
	typedef enum
	{
		eClubInfo,
		eJudgeInfo,
		eLocationInfo
	} eInfoType;
	CDlgInfoJudge(CAgilityBookDoc* pDoc, eInfoType inType, CWnd* pParent = NULL);

private:
// Dialog Data
	//{{AFX_DATA(CDlgInfoJudge)
	enum { IDD = IDD_JUDGE_INFO };
	CButton	m_ctrlDelete;
	CComboBox	m_ctrlNames;
	CEdit		m_ctrlComment;
	//}}AFX_DATA
	CAgilityBookDoc* m_pDoc;
	eInfoType m_Type;
	std::set<std::string> m_NamesInUse;
	ARBInfoClubList m_InfoClub;
	ARBInfoJudgeList m_InfoJudge;
	ARBInfoLocationList m_InfoLocation;

// Overrides
	//{{AFX_VIRTUAL(CDlgInfoJudge)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDlgInfoJudge)
	virtual BOOL OnInitDialog();
	afx_msg void OnNew();
	afx_msg void OnDelete();
	afx_msg void OnSelchangeName();
	afx_msg void OnKillfocusComments();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
