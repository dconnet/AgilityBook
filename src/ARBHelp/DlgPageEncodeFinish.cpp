/*
 * Copyright (c) 2007-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgPageEncodeFinish class
 * @author David Connet
 *
 * Revision History
 * @li 2009-03-01 DRC Ported to wxWidgets.
 * @li 2007-01-02 DRC Created
 */

#include "stdafx.h"
#include "DlgPageEncodeFinish.h"

#include "../Win/Widgets.h"
#include "DlgARBHelp.h"


CDlgPageEncodeFinish::CDlgPageEncodeFinish(CDlgARBHelp* pParent)
	: wxWizardPageSimple(pParent)
	, m_Parent(pParent)
{
	wxStaticText* text1 = new wxStaticText(this, wxID_ANY,
		wxT("Now that the information is gathered, send an email to help@agilityrecordbook.com with this data."),
		wxDefaultPosition, wxDefaultSize, 0);
#ifndef __WXMAC__
	// For some reason, this causes 2.9.x sizers to go nuts on a mac
	text1->Wrap(600);
#endif

	wxFont font = GetFont();
	wxFont fontFixed(font.GetPointSize(), wxFONTFAMILY_MODERN, font.GetStyle(), font.GetWeight());
	wxClientDC dc(this);
	dc.SetFont(fontFixed);
	wxSize sz(dc.GetCharWidth()*80, wxDefaultCoord);

	m_ctrlText = new CTextCtrl(this, wxID_ANY,
		wxEmptyString,
		wxDefaultPosition, sz,
		wxTE_MULTILINE|wxTE_READONLY);
	m_ctrlText->SetMaxLength(0);
	m_ctrlText->SetFont(fontFixed);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(text1, 0, wxALL|wxEXPAND, 5);
	bSizer->Add(m_ctrlText, 1, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
}


bool CDlgPageEncodeFinish::TransferDataToWindow()
{
	wxBusyCursor wait;
	m_ctrlText->SetValue(m_Parent->GetEncodedData());
	return true;
}
