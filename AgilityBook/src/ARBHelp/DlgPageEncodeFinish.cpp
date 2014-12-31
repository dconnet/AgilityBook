/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgPageEncodeFinish class
 * @author David Connet
 *
 * Revision History
 * 2013-12-24 Fixed line wrap on wx3.
 * 2009-03-01 Ported to wxWidgets.
 * 2007-01-02 Created
 */

#include "stdafx.h"
#include "DlgPageEncodeFinish.h"

#include "DlgARBHelp.h"

#include "../Win/Widgets.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CDlgPageEncodeFinish::CDlgPageEncodeFinish(CDlgARBHelp* pParent)
	: wxWizardPageSimple(pParent)
	, m_Parent(pParent)
{
	wxFont font = GetFont();
	wxFont fontFixed(font.GetPointSize(), wxFONTFAMILY_MODERN, font.GetStyle(), font.GetWeight());
	wxClientDC dc(this);
	dc.SetFont(fontFixed);
	wxSize sz(dc.GetCharWidth()*80, wxDefaultCoord);

	wxStaticText* text1 = new wxStaticText(this, wxID_ANY,
		L"Now that the information is gathered, send an email to help@agilityrecordbook.com with this data.",
		wxDefaultPosition, wxDefaultSize, 0);
	text1->Wrap(sz.x);

#pragma PRAGMA_TODO("Add option to create a compressed file")

	m_ctrlText = new CTextCtrl(this, wxID_ANY,
		wxEmptyString,
		wxDefaultPosition, sz,
		wxTE_MULTILINE|wxTE_READONLY);
	m_ctrlText->SetMaxLength(0);
	m_ctrlText->SetFont(fontFixed);

	// Sizers
#pragma PRAGMA_TODO(convert to dialog units)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(text1, 0, wxALL|wxEXPAND, 5);
	bSizer->Add(m_ctrlText, 1, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
}


bool CDlgPageEncodeFinish::TransferDataToWindow()
{
	wxBusyCursor wait;
	m_ctrlText->SetValue(m_Parent->GetEncodedData().c_str());
	return true;
}
