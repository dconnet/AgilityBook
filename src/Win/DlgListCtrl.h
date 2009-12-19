#pragma once

/*
 * Copyright (c) 2002-2009 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgListCtrl class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-12-26 DRC Made dialog resizable.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "ARBDate.h"
#include <vector>
class CAgilityBookDoc;
class CReportListCtrl;
class CTabView;
class wxListEvent;

class CDlgListCtrlData;
typedef std::tr1::shared_ptr<CDlgListCtrlData> CDlgListCtrlDataPtr;


class CDlgListCtrl : public wxDialog
{
	friend class CDlgListCtrlDataCalendar; // To allow easy access to image icons
public:
	typedef enum
	{
		eCalendar,
		eFaults,
		eOtherPoints,
		ePartners
	} WhatToList;

	// Calendar
	CDlgListCtrl(
			CAgilityBookDoc* pDoc,
			ARBDate const& date,
			std::vector<ARBCalendarPtr> const* entries,
			CTabView* pTabView,
			wxWindow* pParent = NULL);
	// Faults, Partners
	CDlgListCtrl(
			WhatToList inType,
			CAgilityBookDoc* pDoc,
			ARBDogRunPtr run,
			wxWindow* pParent = NULL);
	// OtherPoints
	CDlgListCtrl(
			ARBConfig& pConfig,
			ARBDogRunPtr run,
			wxWindow* pParent = NULL);

	void GetAllPartners(
			std::set<wxString>& ioPartners,
			std::set<wxString>& ioDogs) const;

private:
	bool Create(
			wxString const& inCaption,
			wxWindow* pParent,
			bool bHasImageList);
	void FinishCreate(
			int nCols,
			std::vector<CDlgListCtrlDataPtr>& items);
	CDlgListCtrlDataPtr GetItemListData(long item) const;
	void UpdateControls();
	void SwapEntries(
			long oldIndex,
			long newIndex);
	void DoEdit();
	// Only PocketPC calls this directly. We're making use of it internally.
	virtual bool DoOK();

	CReportListCtrl* m_ctrlList;
	wxButton* m_ctrlNew;
	wxButton* m_ctrlEdit;
	wxButton* m_ctrlDelete;
	wxButton* m_ctrlUp;
	wxButton* m_ctrlDown;
	wxButton* m_ctrlCreateTrial;
	WhatToList m_What;
	CAgilityBookDoc* m_pDoc;
	ARBDate m_Date;
	std::vector<ARBCalendarPtr> const* m_CalEntries;
	CTabView* m_pTabView;
	ARBConfig* m_pConfig;
	ARBDogRunPtr m_pRun;
	int m_imgEmpty;
	int m_imgTentative;
	int m_imgPlan;
	int m_imgPlanTentative;
	int m_imgPending;
	int m_imgPendingTentative;
	int m_imgEntered;
	int m_imgEnteredTentative;

	DECLARE_EVENT_TABLE()
	void OnItemSelected(wxListEvent& evt);
	void OnItemActivated(wxListEvent& evt);
	void OnKeyDown(wxKeyEvent& evt);
	void OnNew(wxCommandEvent& evt);
	void OnEdit(wxCommandEvent& evt);
	void OnDelete(wxCommandEvent& evt);
	void OnMoveUp(wxCommandEvent& evt);
	void OnMoveDown(wxCommandEvent& evt);
	void OnCreateTrial(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
