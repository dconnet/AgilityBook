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
 * @brief interface of the CDlgReorder class
 * @author David Connet
 *
 * Revision History
 */

#include <vector>
#include "ARBTypes.h"
#include "DlgBaseDialog.h"
#include "ListBox.h"

class CDlgConfigureData;

class CDlgReorder : public CDlgBaseDialog
{
// Construction
public:
	CDlgReorder(
			std::vector<ARBBasePtr>& items,
			CWnd* pParent = NULL);

private:
// Dialog Data
	//{{AFX_DATA(CDlgReorder)
	enum { IDD = IDD_REORDER };
	CListBox2	m_ctrlList;
	CButton	m_ctrlUp;
	CButton	m_ctrlDown;
	//}}AFX_DATA
	std::vector<ARBBasePtr>& m_Items;

// Overrides
	//{{AFX_VIRTUAL(CDlgReorder)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void LoadData();
	void EnableControls();

	//{{AFX_MSG(CDlgReorder)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeList();
	afx_msg void OnMoveUp();
	afx_msg void OnMoveDown();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
