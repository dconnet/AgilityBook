#pragma once

/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgDog class
 * @author David Connet
 *
 * Revision History
 * @li 2009-03-14 DRC Merged DlgRun* into here.
 * @li 2009-02-09 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-02-03 DRC Broke dialog up into pages.
 * @li 2003-08-18 DRC Added a deceased date for a dog.
 * DlgDogNumbers
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-02-03 DRC Broke dialog up into pages.
 * @li 2003-08-18 DRC Added a deceased date for a dog.
 * DlgDogPoints
 * @li 2006-07-15 DRC Add a selected-item total.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-02-03 DRC Created.
 * DlgDogProp
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2003-08-18 DRC Added a deceased date for a dog.
 * DlgDogTitles
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-02-03 DRC Broke dialog up into pages.
 * @li 2003-08-18 DRC Added a deceased date for a dog.
 */

#include "ARBDate.h"
#include "ARBDogExistingPoints.h"
#include "ARBDogRegNum.h"
#include "ARBDogTitle.h"
#include "ARBTypes.h"
#include "ColumnOrder.h"
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
public:
	CDlgDog(
			CAgilityBookDoc* pDoc,
			ARBDogPtr pDog,
			wxWindow* pParent = NULL,
			int iSelectPage = 0);
	~CDlgDog();

	CDlgDogDataTitlePtr GetTitleData(long index) const;
	CDlgDogDataTitlePtr GetTitleDataByData(long index) const;
	CDlgDogDataRegNumPtr GetRegNumData(long index) const;
	CDlgDogDataRegNumPtr GetRegNumDataByData(long index) const;
	CDlgDogDataPointPtr GetPointData(long index) const;
	CDlgDogDataPointPtr GetPointDataByData(long index) const;
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

	DECLARE_EVENT_TABLE()
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
