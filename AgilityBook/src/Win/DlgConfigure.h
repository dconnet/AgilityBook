#pragma once

/*
 * Copyright © 2002-2007 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgConfigure class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include <list>
#include <vector>
#include "ARBConfig.h"
#include "ARBTypes.h"
#include "DlgBaseDialog.h"
#include "DlgListViewer.h"
#include "ListCtrl.h"
class ARBAgilityRecordBook;
class CAgilityBookDoc;
class CDlgConfigureData;

class CDlgConfigure : public CDlgBaseDialog
{
public:
	CDlgConfigure(
			CAgilityBookDoc* pDoc,
			ARBAgilityRecordBook& book);
	virtual ~CDlgConfigure();

private:
// Dialog Data
	//{{AFX_DATA(CDlgConfigure)
	enum { IDD = IDD_CONFIGURE };
	CListCtrl2	m_ctrlVenues;
	CListCtrl2	m_ctrlFaults;
	CListCtrl2	m_ctrlOthers;
	CButton	m_ctrlNew;
	CButton	m_ctrlDelete;
	CButton	m_ctrlEdit;
	CButton	m_ctrlCopy;
	CStatic	m_ctrlComments;
	//}}AFX_DATA
	CAgilityBookDoc* m_pDoc;
	ARBAgilityRecordBook& m_Book;
	ARBConfig m_Config;
	typedef enum
	{
		eNone,
		eVenues,
		eFaults,
		eOtherPoints
	} eAction;
	eAction m_Action;

	//{{AFX_VIRTUAL(CDlgConfigure)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	void SetAction(eAction inAction);
	bool GetActionData(
			CListCtrl2*& pCtrl,
			int& index,
			CDlgConfigureData*& pData);
	void UpdateButtons();
	void LoadData();
	int FindCurrentVenue(
			ARBConfigVenuePtr pVenue,
			bool bSet);
	int FindCurrentFault(
			ARBConfigFaultPtr pFault,
			bool bSet);
	int FindCurrentOtherPoints(
			ARBConfigOtherPointsPtr pOther,
			bool bSet);

protected:
	//{{AFX_MSG(CDlgConfigure)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusVenues(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusFaults(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusOtherpoints(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNew();
	afx_msg void OnDelete();
	afx_msg void OnEdit();
	afx_msg void OnCopy();
	afx_msg void OnUpdate();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
