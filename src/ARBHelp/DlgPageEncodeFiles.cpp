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
 * @brief implementation of the CDlgPageEncodeFiles class
 * @author David Connet
 *
 * Revision History
 * @li 2009-03-01 DRC Ported to wxWidgets.
 * @li 2007-01-02 DRC Created
 */

#include "stdafx.h"
#include "DlgPageEncodeFiles.h"

#include "DlgARBHelp.h"


CDlgPageEncodeFiles::CDlgPageEncodeFiles(CDlgARBHelp* pParent)
	: wxWizardPageSimple(pParent)
	, m_Parent(pParent)
	, m_ctrlList(NULL)
{
	wxStaticText* text1 = new wxStaticText(this, wxID_ANY,
		wxT("Check the files you would like to be sent"),
		wxDefaultPosition, wxDefaultSize, 0);
	text1->Wrap(-1);

	m_ctrlList = new wxCheckListBox(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize,
		0, NULL, 0);

	wxButton* btnAll = new wxButton(this, wxID_ANY, wxT("Check All"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnAll->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgPageEncodeFiles::OnCheckAll), NULL, this);

	wxButton* btnNone = new wxButton(this, wxID_ANY, wxT("Uncheck All"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnNone->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgPageEncodeFiles::OnCheckNone), NULL, this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(text1, 0, wxALL, 5);

	wxBoxSizer* sizerListBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerListBtns->Add(m_ctrlList, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerBtns = new wxBoxSizer(wxVERTICAL);
	sizerBtns->Add(btnAll, 0, wxALL, 5);
	sizerBtns->Add(btnNone, 0, wxALL, 5);

	sizerListBtns->Add(sizerBtns, 0, wxEXPAND, 5);

	bSizer->Add(sizerListBtns, 1, wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
}


void CDlgPageEncodeFiles::OnCheckAll(wxCommandEvent& evt)
{
	for (unsigned int n = 0; n < m_ctrlList->GetCount(); ++n)
	{
		m_ctrlList->Check(n, true);
	}
}


void CDlgPageEncodeFiles::OnCheckNone(wxCommandEvent& evt)
{
	for (unsigned int n = 0; n < m_ctrlList->GetCount(); ++n)
	{
		m_ctrlList->Check(n, false);
	}
}


bool CDlgPageEncodeFiles::TransferDataToWindow()
{
	m_ctrlList->Clear();
	for (CDlgARBHelp::FileMap::const_iterator i = m_Parent->GetARBFiles().begin();
		i != m_Parent->GetARBFiles().end();
		++i)
	{
		int idx = m_ctrlList->Append((*i).first);
		m_ctrlList->Check(idx, (*i).second);
	}
	return true;
}


bool CDlgPageEncodeFiles::TransferDataFromWindow()
{
	for (unsigned int i = 0; i < m_ctrlList->GetCount(); ++i)
	{
		wxString filename = m_ctrlList->GetString(i);
		m_Parent->SetARBFileStatus(filename, m_ctrlList->IsChecked(i));
	}
	return true;
}
