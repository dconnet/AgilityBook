#pragma once

/*
 * Copyright � 2004 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgConfigVenue class
 * @author David Connet
 *
 * Revision History
 * @li 2004-01-24 DRC Created
 */

#include "ARBConfigScoring.h"
#include "ListCtrl.h"
class ARBAgilityRecordBook;
class ARBConfigDivision;
class ARBConfigEvent;
class ARBConfigLevel;
class ARBConfigSubLevel;
class ARBConfigTitle;
class ARBConfigVenue;
class CDlgConfigureData;
class CDlgFixup;

class CDlgConfigVenue : public CDialog
{
public:
	CDlgConfigVenue(ARBAgilityRecordBook& book, ARBConfig& config, ARBConfigVenue* pVenue, CWnd* pParent = NULL);
	virtual ~CDlgConfigVenue();
	void GetFixups(std::vector<CDlgFixup*>& ioFixups);

private:
// Dialog Data
	//{{AFX_DATA(CDlgConfigVenue)
	enum { IDD = IDD_CONFIG_VENUE };
	CEdit	m_ctrlName;
	CEdit	m_ctrlDesc;
	CListCtrl2	m_ctrlDivisions;
	CTreeCtrl	m_ctrlLevels;
	CListCtrl2	m_ctrlTitles;
	CTreeCtrl	m_ctrlEvents;
	CButton	m_ctrlNew;
	CButton	m_ctrlDelete;
	CButton	m_ctrlEdit;
	CButton	m_ctrlCopy;
	CButton	m_ctrlMoveUp;
	CButton	m_ctrlMoveDown;
	CStatic	m_ctrlComments;
	//}}AFX_DATA
	ARBAgilityRecordBook& m_Book;
	ARBConfig& m_Config;
	ARBConfigVenue* m_pVenue;
	std::vector<CDlgFixup*> m_DlgFixup;
	typedef enum {eNone, eDivisions, eLevels, eTitles, eEvents} eAction;
	eAction m_Action;

	//{{AFX_VIRTUAL(CDlgConfigVenue)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	void SetAction(eAction inAction);
	CDlgConfigureData* GetActionData(CWnd*& pWnd);
	void UpdateButtons();
	void LoadDivisionData();
	void LoadLevelData();
	void LoadTitleData();
	void LoadEventData();
	int FindCurrentDivision(const ARBConfigDivision* pDiv, bool bSet);
	HTREEITEM FindCurrentLevel(const ARBConfigLevel* pLevel, bool bSet);
	HTREEITEM FindCurrentSubLevel(const ARBConfigSubLevel* pSubLevel, bool bSet);
	int FindCurrentTitle(const ARBConfigTitle* pTitle, bool bSet);
	HTREEITEM FindCurrentEvent(const ARBConfigEvent* pEvent, bool bSet);

protected:
	//{{AFX_MSG(CDlgConfigVenue)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfoTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitemList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitemTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedDivision(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedTitles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusDivision(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusLevel(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusTitles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusEvent(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNew();
	afx_msg void OnDelete();
	afx_msg void OnEdit();
	afx_msg void OnCopy();
	afx_msg void OnMoveUp();
	afx_msg void OnMoveDown();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
