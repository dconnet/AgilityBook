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
 * @li 2012-05-07 DRC Added autocompletion to combo boxes.
 * @li 2012-02-16 DRC Fix initial focus.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "stdafx.h"
#include "DlgFault.h"

#include "AgilityBook.h"
#include "ComboBoxes.h"
#include "StringUtil.h"
#include "Validators.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CDlgFault::CDlgFault(
		std::set<std::wstring>& inFaults,
		std::wstring const& fault,
		wxWindow* pParent)
	: wxDialog()
	, m_Fault(StringUtil::stringWX(fault))
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP | GetExtraStyle());
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_FAULT"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

	wxArrayString faults;
	for (std::set<std::wstring>::const_iterator iter = inFaults.begin(); iter != inFaults.end(); ++iter)
	{
		faults.Add(StringUtil::stringWX(*iter));
	}
	faults.Sort();

	// Controls (these are done first to control tab order)

	CAutoFillComboBox* comboBox = new CAutoFillComboBox(this, wxID_ANY, m_Fault,
		wxDefaultPosition, wxDefaultSize,
		faults,
		wxCB_DROPDOWN|wxCB_SORT,
		CTrimValidator(&m_Fault, TRIMVALIDATOR_DEFAULT, _("IDS_ENTER_NAME")));
	comboBox->SetHelpText(_("HIDC_FAULTS"));
	comboBox->SetToolTip(_("HIDC_FAULTS"));
	comboBox->AutoComplete(faults);

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

	IMPLEMENT_ON_INIT(CDlgFault, comboBox)
}


DEFINE_ON_INIT(CDlgFault)


std::wstring CDlgFault::GetFault() const
{
	return StringUtil::stringW(m_Fault);
}
