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
 * @brief interface of the CDlgConfigure class
 * @author David Connet
 *
 * Revision History
 */

#include <string>
#include <vector>
#include "ARBConfig.h"
class ARBAgilityRecordBook;
class ARBConfigDivision;
class ARBConfigEvent;
class ARBConfigFault;
class ARBConfigLevel;
class ARBConfigOtherPoints;
class ARBConfigScoring;
class ARBConfigSubLevel;
class ARBConfigTitle;
class ARBConfigVenue;
class CAgilityBookDoc;
class CDlgConfigureData;
class CDlgFixup;

class CDlgConfigure : public CDialog
{
	friend class CDlgConfigureData;

public:
	CDlgConfigure(CAgilityBookDoc* pDoc, ARBAgilityRecordBook& book);
	virtual ~CDlgConfigure();

private:
// Dialog Data
	//{{AFX_DATA(CDlgConfigure)
	enum { IDD = IDD_CONFIGURE };
	CTreeCtrl m_ctrlTree;
	//}}AFX_DATA
	CAgilityBookDoc* m_pDoc;

	//{{AFX_VIRTUAL(CDlgConfigure)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	// These insertion routines are public so the dlgconfiguredata classes
	// can access them without requiring friendship.
	HTREEITEM InsertVenue(HTREEITEM hParent, ARBConfigVenue* venue);
	HTREEITEM InsertFaultType(HTREEITEM hParent, ARBConfigFault* fault);
	HTREEITEM InsertOtherPoint(HTREEITEM hParent, ARBConfigOtherPoints* other);
	HTREEITEM InsertDivision(HTREEITEM hParent, ARBConfigDivision* div);
	HTREEITEM InsertLevel(HTREEITEM hParent, ARBConfigLevel* level);
	HTREEITEM InsertSubLevel(HTREEITEM hParent, ARBConfigSubLevel* subLevel);
	HTREEITEM InsertTitle(HTREEITEM hParent, ARBConfigTitle* title);
	HTREEITEM InsertEvent(HTREEITEM hParent, ARBConfigEvent* event);
	HTREEITEM InsertScoring(HTREEITEM hParent, ARBConfigScoring* scoring);

private:
	void LoadData();
	void InsertVenues(HTREEITEM hParent);
	void InsertFaultTypes(HTREEITEM hParent);
	void InsertOtherPoints(HTREEITEM hParent);
	void InsertDivisions(HTREEITEM hParent, ARBConfigVenue* venue);
	void InsertLevels(HTREEITEM hParent, ARBConfigDivision* div);
	void InsertTitles(HTREEITEM hParent, ARBConfigDivision* div);
	void InsertEvents(HTREEITEM hParent, ARBConfigVenue* venue);
	void InsertScorings(HTREEITEM hParent, ARBConfigEvent* event);
	CDlgConfigureData* GetItemData(HTREEITEM inItem);
	ARBAgilityRecordBook& m_Book;
	ARBConfig m_Config;
	std::vector<CDlgFixup*> m_DlgFixup;
	bool m_bReset;

protected:
	//{{AFX_MSG(CDlgConfigure)
	virtual BOOL OnInitDialog();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnTvnDeleteitemVenues(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickVenues(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkVenues(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateConfigAdd(CCmdUI *pCmdUI);
	afx_msg void OnConfigAdd();
	afx_msg void OnUpdateConfigDup(CCmdUI *pCmdUI);
	afx_msg void OnConfigDup();
	afx_msg void OnUpdateConfigEdit(CCmdUI *pCmdUI);
	afx_msg void OnConfigEdit();
	afx_msg void OnUpdateConfigDelete(CCmdUI *pCmdUI);
	afx_msg void OnConfigDelete();
	afx_msg void OnUpdateExpand(CCmdUI* pCmdUI);
	afx_msg void OnExpand();
	afx_msg void OnUpdateExpandAll(CCmdUI* pCmdUI);
	afx_msg void OnExpandAll();
	afx_msg void OnUpdateCollapse(CCmdUI* pCmdUI);
	afx_msg void OnCollapse();
	afx_msg void OnUpdateCollapseAll(CCmdUI* pCmdUI);
	afx_msg void OnCollapseAll();
	afx_msg void OnUpdate();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
