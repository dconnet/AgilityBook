#pragma once

/*
 * Copyright © 2005 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgEventSelect class
 * @author David Connet
 *
 * Revision History
 * @li 2005-07-30 DRC Created.
 */

#include "ARBDate.h"
#include "DlgBaseDialog.h"
class ARBConfigVenue;
class ARBConfigMultiQ;

class CDlgEventSelect : public CDlgBaseDialog
{
public:
	CDlgEventSelect(
			ARBConfigVenue const* inVenue,
			ARBDate const& inDate,
			TCHAR const* const inDivision,
			TCHAR const* const inLevel,
			TCHAR const* const inEvent,
			CWnd* pParent = NULL);
	virtual ~CDlgEventSelect();
	TCHAR const* const GetDivision() const	{return (LPCTSTR)m_Div;}
	TCHAR const* const GetLevel() const		{return (LPCTSTR)m_Lev;}
	TCHAR const* const GetEvent() const		{return (LPCTSTR)m_Evt;}

private:
// Dialog Data
	//{{AFX_DATA(CDlgEventSelect)
	enum { IDD = IDD_EVENT_SELECT };
	CString	m_Division;
	CComboBox	m_ctrlDivisions;
	CString	m_Level;
	CComboBox	m_ctrlLevels;
	CString	m_Event;
	CComboBox	m_ctrlEvents;
	CButton	m_ctrlOk;
	//}}AFX_DATA
	ARBConfigVenue const* m_pVenue;
	ARBDate m_Date;
	TCHAR const* m_inDivision;
	TCHAR const* m_inLevel;
	TCHAR const* m_inEvent;
	CString m_Div;
	CString m_Lev;
	CString m_Evt;

	//{{AFX_VIRTUAL(CDlgEventSelect)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	void UpdateControls();
	void ClearLevels();
	void FillLevels();
	void FillEvents();
	//{{AFX_MSG(CDlgEventSelect)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeDivision();
	afx_msg void OnSelchangeLevel();
	afx_msg void OnSelchangeEvent();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
