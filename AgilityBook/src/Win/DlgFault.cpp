/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CDlgFault class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "stdafx.h"
#include "DlgFault.h"

#include "AgilityBook.h"
#include "Validators.h"


CDlgFault::CDlgFault(
		std::set<wxString>& inFaults,
		wxString const& fault,
		wxWindow* pParent)
	: wxDialog()
	, m_Fault(fault)
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_FAULT"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

	wxArrayString faults;
	for (std::set<wxString>::const_iterator iter = inFaults.begin(); iter != inFaults.end(); ++iter)
	{
		faults.Add((*iter));
	}
	faults.Sort();

	// Controls (these are done first to control tab order)

	wxComboBox* comboBox = new wxComboBox(this, wxID_ANY, m_Fault,
		wxDefaultPosition, wxDefaultSize,
		faults,
		wxCB_DROPDOWN|wxCB_SORT,
		CTrimValidator(&m_Fault, _("IDS_ENTER_NAME")));
	comboBox->SetHelpText(_("HIDC_FAULTS"));
	comboBox->SetToolTip(_("HIDC_FAULTS"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(comboBox, 1, wxALL|wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz = GetSize();
	SetSizeHints(sz, wxSize(-1, sz.y));
	CenterOnParent();

	comboBox->SetFocus();
}
