#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgDog class
 * @author David Connet
 *
 * Revision History
 * 2009-03-14 Merged DlgRun* into here.
 * 2009-02-09 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-02-03 Broke dialog up into pages.
 * 2003-08-18 Added a deceased date for a dog.
 *
 * DlgDogNumbers
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-02-03 Broke dialog up into pages.
 * 2003-08-18 Added a deceased date for a dog.
 *
 * DlgDogPoints
 * 2006-07-15 Add a selected-item total.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-02-03 Created.
 *
 * DlgDogProp
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2003-08-18 Added a deceased date for a dog.
 *
 * DlgDogTitles
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-02-03 Broke dialog up into pages.
 * 2003-08-18 Added a deceased date for a dog.
 */

#include "ARB/ARBDogExistingPoints.h"
#include "ARB/ARBDogRegNum.h"
#include "ARB/ARBDogTitle.h"
#include "ARBCommon/ARBDate.h"
#include "ARBCommon/ARBTypes.h"
#include "LibARBWin/ColumnOrder.h"
#include <wx/listctrl.h>
class CAgilityBookDoc;
class CDlgDogDataTitle;
class CReportListCtrl;
class wxDateEvent;
class wxDatePickerCtrl;

ARB_TYPEDEF(CDlgDogDataPoint)
ARB_TYPEDEF(CDlgDogDataRegNum)
ARB_TYPEDEF(CDlgDogDataTitle)


class CDlgDog : public wxDialog
{
	friend class CDlgDogDataTitle;
	DECLARE_NO_COPY_IMPLEMENTED(CDlgDog)
public:
	CDlgDog(
			CAgilityBookDoc* pDoc,
			ARBDogPtr const& inDog,
			wxWindow* pParent = nullptr,
			int iSelectPage = 0);
	~CDlgDog();

	CDlgDogDataTitlePtr GetTitleData(long index) const;
	CDlgDogDataRegNumPtr GetRegNumData(long index) const;
	CDlgDogDataPointPtr GetPointData(long index) const;
private:
	void UpdateAge();
	void UpdateDeceased();
	void SetColumnTitleHeaders();
	void ListTitles();
	void UpdateTitleButtons();
	void EditTitle();
	void SetColumnRegNumHeaders();
	void ListRegNums();
	void UpdateRegNumButtons();
	void EditRegNum();
	void SetColumnPointsHeaders();
	void ListExistingPoints();
	void UpdatePointsButtons();
	void EditPoints();

	CAgilityBookDoc* m_pDoc;
	ARBDogPtr m_pDog;
	bool m_viewHidden;
	int m_imgTitlesEmpty;
	int m_imgTitlesTitled;
	int m_imgTitlesTitledReceived;
	int m_imgTitlesHidden;
	int m_imgTitlesTitledHidden;
	int m_imgTitlesTitledHiddenReceived;
	// Properties
	wxString m_CallName;
	wxString m_Breed;
	wxString m_RegName;
	ARBDate m_DOB;
	bool m_IsDeceased;
	ARBDate m_Deceased;
	wxString m_Notes;
	wxStaticText* m_ctrlAge;
	wxDatePickerCtrl* m_ctrlDDay;
	CReportListCtrl* m_ctrlTitles;
	wxButton* m_ctrlTitleEdit;
	wxButton* m_ctrlTitleDelete;
	CReportListCtrl* m_ctrlRegNums;
	wxButton* m_ctrlRegEdit;
	wxButton* m_ctrlRegDelete;
	CReportListCtrl* m_ctrlPoints;
	wxButton* m_ctrlPointsEdit;
	wxButton* m_ctrlPointsDelete;
	wxStaticText* m_ctrlSelectedPts;
	// Titles
	ARBDogTitleList m_Titles;
	CColumnOrder m_sortTitles;
	bool m_ViewHiddenTitles;
	// RegNums
	ARBDogRegNumList m_RegNums;
	CColumnOrder m_sortRegNums;
	// Points
	ARBDogExistingPointsList m_ExistingPoints;
	CColumnOrder m_sortPoints;

	DECLARE_ON_INIT()
	DECLARE_EVENT_TABLE()
	void OnPageChanging(wxBookCtrlEvent& evt);
	void OnOk(wxCommandEvent& evt);
	// Properties
	void OnDateChanged(wxDateEvent& evt);
	void OnDeceased(wxCommandEvent& evt);
	// Titles
	void OnTitleColumnClick(wxListEvent& evt);
	void OnTitleItemSelected(wxListEvent& evt);
	void OnTitleItemActivated(wxListEvent& evt);
	void OnTitleKeyDown(wxKeyEvent& evt);
	void OnTitleNew(wxCommandEvent& evt);
	void OnTitleEdit(wxCommandEvent& evt);
	void OnTitleDelete(wxCommandEvent& evt);
	void OnTitleHidden(wxCommandEvent& evt);
	// RegNums
	void OnRegNumColumnClick(wxListEvent& evt);
	void OnRegNumItemSelected(wxListEvent& evt);
	void OnRegNumItemActivated(wxListEvent& evt);
	void OnRegNumKeyDown(wxKeyEvent& evt);
	void OnRegNumNew(wxCommandEvent& evt);
	void OnRegNumEdit(wxCommandEvent& evt);
	void OnRegNumDelete(wxCommandEvent& evt);
	// Points
	void OnPointsColumnClick(wxListEvent& evt);
	void OnPointsItemSelected(wxListEvent& evt);
	void OnPointsItemActivated(wxListEvent& evt);
	void OnPointsKeyDown(wxKeyEvent& evt);
	void OnPointsNew(wxCommandEvent& evt);
	void OnPointsEdit(wxCommandEvent& evt);
	void OnPointsDelete(wxCommandEvent& evt);
};
