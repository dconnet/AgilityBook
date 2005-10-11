#pragma once

/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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

#include <list>
#include <string>
#include <vector>
#include "ARBConfig.h"
#include "DlgBaseDialog.h"
#include "DlgListViewer.h"
#include "ListCtrl.h"
class ARBAgilityRecordBook;
class ARBConfigScoringList;
class ARBDog;
class ARBDogList;
class ARBDogRun;
class ARBDogTrial;
class CAgilityBookDoc;
class CDlgConfigureData;
class CDlgFixup;


class CDlgConfigure : public CDlgBaseDialog
{
public:
	typedef enum
	{
		eNoChange,		// No runs to update
		eCancelChanges,	// IDCANCEL
		eDoNotDoIt,		// IDNO
		eDoIt			// IDYES
	} eCheck;
	/**
	 * Update existing runs in case event scoring methods have changed.
	 * @param inDoc MFC Document
	 * @param inDogs List of existing runs.
	 * @param inConfig New configuration to verify against.
	 * @param ioDlgFixup Fixup commands to update existing runs.
	 * @param bCommitChanges Changes cannot be cancelled.
	 */
	static eCheck CheckExistingRuns(
			CAgilityBookDoc* inDoc,
			ARBDogList const& inDogs,
			ARBConfig const& inConfig,
			std::vector<CDlgFixup*>& ioDlgFixup,
			bool bCommitChanges);
	/**
	 * Update existing runs in case event scoring methods have changed.
	 * @param inDoc MFC Document
	 * @param inDogs List of existing runs.
	 * @param inVenue Name of venue to verify against.
	 * @param inEvent Name of event to verify against.
	 * @param inScorings New scoring methods to verify against.
	 * @param ioDlgFixup Fixup commands to update existing runs.
	 * @param inRunsDeleted Used only when checking all venues.
	 * @param inRunsChanged Used only when checking all venues.
	 * @param inScoringRuns Used only when checking all venues.
	 */
	static eCheck CheckExistingRuns(
			CAgilityBookDoc* inDoc,
			ARBDogList const& inDogs,
			ARBConfigVenue const* inVenue,
			std::string const& inEvent,
			ARBConfigScoringList const& inScorings,
			std::vector<CDlgFixup*>& ioDlgFixup,
			// These settings are only used from the preceeding api.
			int* inRunsDeleted = NULL,
			int* inRunsChanged = NULL,
			std::list<ScoringRunInfo>* inScoringRuns = NULL);

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
	std::vector<CDlgFixup*> m_DlgFixup;
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
			ARBConfigVenue const* pVenue,
			bool bSet);
	int FindCurrentFault(
			ARBConfigFault const* pFault,
			bool bSet);
	int FindCurrentOtherPoints(
			ARBConfigOtherPoints const* pOther,
			bool bSet);

protected:
	//{{AFX_MSG(CDlgConfigure)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
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
