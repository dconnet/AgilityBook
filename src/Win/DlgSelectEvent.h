#pragma once

/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
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
 * @brief interface of the CDlgSelectEvent class
 * @author David Connet
 *
 * Revision History
 */

class ARBConfigVenue;

class CDlgSelectEvent : public CDialog
{
public:
	CDlgSelectEvent(const ARBConfigVenue* pVenue, CWnd* pParent = NULL);
	const char* GetDivision() const		{return (const char*)m_Division;}
	const char* GetLevel() const		{return (const char*)m_Level;}
	const char* GetEvent() const		{return (const char*)m_Event;}

private:
// Dialog Data
	//{{AFX_DATA(CDlgSelectEvent)
	enum { IDD = IDD_SELECT_EVENT };
	CComboBox	m_ctrlDivision;
	CComboBox	m_ctrlLevel;
	CComboBox	m_ctrlEvent;
	//}}AFX_DATA
	const ARBConfigVenue* m_pVenue;
	CString m_Division;
	CString m_Level;
	CString m_Event;

	//{{AFX_VIRTUAL(CDlgSelectEvent)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	void ClearLevels();
	void FillLevels();
	void FillEvents();

// Implementation
protected:
	//{{AFX_MSG(CDlgSelectEvent)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeDivision();
	afx_msg void OnSelchangeLevel();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
