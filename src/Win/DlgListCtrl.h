#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgListCtrl class
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-11 Ported to wxWidgets.
 * 2006-12-26 Made dialog resizable.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 */

#include "ARB/ARBTypes2.h"
#include "ARBCommon/ARBDate.h"
#include "LibARBWin/Logger.h"
#include <vector>
class wxListEvent;


namespace dconSoft
{
namespace ARBWin
{
class CReportListCtrl;
} // namespace ARBWin
class CAgilityBookDoc;
class CTabView;

class CDlgListCtrlData;
typedef std::shared_ptr<CDlgListCtrlData> CDlgListCtrlDataPtr;


enum class ARBWhatToList
{
	Calendar,
	Faults,
	OtherPoints,
	Partners
};


class CDlgListCtrl : public wxDialog
{
	friend class CDlgListCtrlDataCalendar; // To allow easy access to image icons
	friend class CDlgListCtrlDataFaults;   // To allow access to GetItemListData
public:
	// Calendar
	CDlgListCtrl(
		CAgilityBookDoc* pDoc,
		ARBCommon::ARBDate const& date,
		std::vector<ARB::ARBCalendarPtr> const* entries,
		CTabView* pTabView,
		wxWindow* pParent = nullptr);
	// Faults, Partners
	CDlgListCtrl(
		ARBWhatToList inType,
		CAgilityBookDoc* pDoc,
		ARB::ARBDogRunPtr const& inRun,
		wxWindow* pParent = nullptr);
	// OtherPoints
	CDlgListCtrl(ARB::ARBConfig& pConfig, ARB::ARBDogRunPtr const& inRun, wxWindow* pParent = nullptr);

	void GetAllPartners(std::set<std::wstring>& ioPartners, std::set<std::wstring>& ioDogs) const;

private:
	bool Create(std::wstring const& inCaption, wxWindow* pParent, bool bHasImageList);
	void FinishCreate(int nCols, std::vector<CDlgListCtrlDataPtr>& items);
	CDlgListCtrlDataPtr GetItemListData(long item) const;
	void UpdateControls();
	void DoEdit();
	// Only PocketPC calls this directly. We're making use of it internally.
	virtual bool DoOK();

	ARBWin::CReportListCtrl* m_ctrlList;
	wxButton* m_ctrlNew;
	wxButton* m_ctrlEdit;
	wxButton* m_ctrlDelete;
	wxButton* m_ctrlUp;
	wxButton* m_ctrlDown;
	wxButton* m_ctrlCreateTrial;
	ARBWhatToList m_What;
	CAgilityBookDoc* m_pDoc;
	ARBCommon::ARBDate m_Date;
	std::vector<ARB::ARBCalendarPtr> const* m_CalEntries;
	CTabView* m_pTabView;
	ARB::ARBConfig* m_pConfig;
	ARB::ARBDogRunPtr m_pRun;
	int m_imgTentative;
	int m_imgPlan;
	int m_imgPlanTentative;
	int m_imgPending;
	int m_imgPendingTentative;
	int m_imgEntered;
	int m_imgEnteredTentative;
	ARBWin::CStackLogger m_trace;

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

} // namespace dconSoft
