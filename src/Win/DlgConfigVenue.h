#pragma once

/*
 * Copyright © 2004-2006 David Connet. All Rights Reserved.
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
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-01-24 DRC Created
 */

#include "ARBConfigScoring.h"
#include "DlgBaseDialog.h"
#include "ListCtrl.h"
class ARBAgilityRecordBook;
class CAgilityBookDoc;
class CDlgConfigureData;
class CDlgConfigureDataDivision;
class CDlgConfigureDataEvent;
class CDlgConfigureDataLevel;
class CDlgConfigureDataMultiQ;
class CDlgConfigureDataSubLevel;
class CDlgConfigureDataTitle;

class CDlgConfigVenue : public CDlgBaseDialog
{
public:
	CDlgConfigVenue(
			ARBAgilityRecordBook const& book,
			ARBConfig const& config,
			ARBConfigVenuePtr pVenue,
			CWnd* pParent = NULL);
	virtual ~CDlgConfigVenue();
	void GetFixups(ARBConfigActionList& ioFixups);

private:
// Dialog Data
	//{{AFX_DATA(CDlgConfigVenue)
	enum { IDD = IDD_CONFIG_VENUE };
	CEdit	m_ctrlName;
	CEdit	m_ctrlLongName;
	CEdit	m_ctrlURL;
	CEdit	m_ctrlDesc;
	CListCtrl2	m_ctrlDivisions;
	CTreeCtrl	m_ctrlLevels;
	CListCtrl2	m_ctrlEvents;
	CListCtrl2	m_ctrlMultiQ;
	CListCtrl2	m_ctrlTitles;
	CButton	m_ctrlNew;
	CButton	m_ctrlDelete;
	CButton	m_ctrlEdit;
	CButton	m_ctrlCopy;
	CButton	m_ctrlMoveUp;
	CButton	m_ctrlMoveDown;
	CStatic	m_ctrlComments;
	//}}AFX_DATA
	ARBAgilityRecordBook const& m_Book;
	ARBConfig const& m_Config;
	ARBConfigVenuePtr m_pVenueOrig;
	ARBConfigVenuePtr m_pVenue;
	std::vector<ARBConfigActionPtr> m_DlgFixup;
	typedef enum
	{
		eNone,
		eDivisions,
		eLevels,
		eTitles,
		eEvents,
		eMultiQ
	} eAction;
	eAction m_Action;

	//{{AFX_VIRTUAL(CDlgConfigVenue)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	void SetAction(eAction inAction);
	void UpdateButtons();
	void LoadDivisionData();
	void LoadLevelData();
	void LoadTitleData();
	void LoadEventData();
	void LoadMultiQData();
	int FindCurrentDivision(
			ARBConfigDivisionPtr pDiv,
			bool bSet);
	HTREEITEM FindCurrentLevel(
			ARBConfigLevelPtr pLevel,
			bool bSet);
	HTREEITEM FindCurrentSubLevel(
			ARBConfigSubLevelPtr pSubLevel,
			bool bSet);
	int FindCurrentTitle(
			ARBConfigTitlePtr pTitle,
			bool bSet);
	int FindCurrentEvent(
			ARBConfigEventPtr pEvent,
			bool bSet);
	int FindCurrentMultiQ(
			ARBConfigMultiQPtr pMultiQ,
			bool bSet);
	CDlgConfigureDataDivision* GetCurrentDivisionData();
	CDlgConfigureDataLevel* GetCurrentLevelData();
	CDlgConfigureDataSubLevel* GetCurrentSubLevelData();
	CDlgConfigureDataTitle* GetCurrentTitleData();
	CDlgConfigureDataEvent* GetCurrentEventData();
	CDlgConfigureDataMultiQ* GetCurrentMultiQData();

protected:
	//{{AFX_MSG(CDlgConfigVenue)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfoTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitemTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedDivision(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusDivision(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusLevel(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusTitles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusEvent(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusMultiQ(NMHDR* pNMHDR, LRESULT* pResult);
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
