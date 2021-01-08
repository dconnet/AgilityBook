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
 * 2021-01-07 Separated from DlgDog.cpp
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2003-08-18 Added a deceased date for a dog.
 */

#include "DlgDogPanelBase.h"

#include "ARB/ARBTypes2.h"
#include "ARBCommon/ARBDate.h"
class CAgilityBookDoc;
class CReportListCtrl;
class CTextCtrl;
class wxDateEvent;
class wxDatePickerCtrl;

class CDlgDogPanelProp : public CDlgDogPanelBase
{
	DECLARE_NO_COPY_IMPLEMENTED(CDlgDogPanelProp)
public:
	CDlgDogPanelProp(CAgilityBookDoc* pDoc, ARBDogPtr const& inDog, wxWindow* parent);

	wxWindow* GetInitialControl() override;

	bool Save(unsigned int& hint) override;

private:
	void UpdateAge();
	void UpdateDeceased();

	wxString m_CallName;
	wxString m_Breed;
	wxString m_RegName;
	ARBDate m_DOB;
	bool m_IsDeceased;
	ARBDate m_Deceased;
	wxString m_Notes;
	CTextCtrl* m_ctrlName;
	wxStaticText* m_ctrlAge;
	wxDatePickerCtrl* m_ctrlDDay;

	void OnDateChanged(wxDateEvent& evt);
	void OnDeceased(wxCommandEvent& evt);
};
