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
 * @brief implementation of the CDlgReorder class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-09 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "stdafx.h"
#include "DlgReorder.h"


CDlgReorder::CDlgReorder(
		std::vector<ARBBasePtr>& items,
		wxWindow* pParent)
	: wxDialog()
	, m_Items(items)
	, m_ctrlList(NULL)
	, m_ctrlUp(NULL)
	, m_ctrlDown(NULL)
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	Create(pParent, wxID_ANY, _("IDD_REORDER"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	m_ctrlList = new wxListBox(this, wxID_ANY,
		wxDefaultPosition, wxSize(200,150),
		0, NULL,
		wxLB_NEEDED_SB|wxLB_SINGLE);
	m_ctrlList->Connect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(CDlgReorder::OnItemSelected), NULL, this);
	m_ctrlList->SetHelpText(_("HIDC_REORDER_LIST"));
	m_ctrlList->SetToolTip(_("HIDC_REORDER_LIST"));

	wxButton* btnOk = new wxButton(this, wxID_OK);

	wxButton* btnCancel = new wxButton(this, wxID_CANCEL);

	m_ctrlUp = new wxButton(this, wxID_ANY,
		_("IDC_REORDER_MOVE_UP"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlUp->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgReorder::OnMoveUp), NULL, this);
	m_ctrlUp->SetHelpText(_("HIDC_REORDER_MOVE_UP"));
	m_ctrlUp->SetToolTip(_("HIDC_REORDER_MOVE_UP"));

	m_ctrlDown = new wxButton(this, wxID_ANY,
		_("IDC_REORDER_MOVE_DOWN"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlDown->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgReorder::OnMoveDown), NULL, this);
	m_ctrlDown->SetHelpText(_("HIDC_REORDER_MOVE_DOWN"));
	m_ctrlDown->SetToolTip(_("HIDC_REORDER_MOVE_DOWN"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizerList = new wxBoxSizer(wxHORIZONTAL);
	bSizerList->Add(m_ctrlList, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* bSizerSide = new wxBoxSizer(wxVERTICAL);
	bSizerSide->Add(btnOk, 0, wxALL, 5);
	bSizerSide->Add(btnCancel, 0, wxALL, 5);
	bSizerSide->Add(0, 0, 1, wxEXPAND, 5);
	bSizerSide->Add(m_ctrlUp, 0, wxALL, 5);
	bSizerSide->Add(m_ctrlDown, 0, wxALL, 5);

	bSizerList->Add(bSizerSide, 0, wxEXPAND, 5);

	bSizer->Add(bSizerList, 1, wxEXPAND, 5);

	LoadData();

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	m_ctrlList->SetFocus();
}


void CDlgReorder::LoadData()
{
	m_ctrlList->Clear();
	for (size_t i = 0; i < m_Items.size(); ++i)
		m_ctrlList->Append(m_Items[i]->GetGenericName().c_str());
	UpdateControls();
}


void CDlgReorder::UpdateControls()
{
	bool bEnableUp = false;
	bool bEnableDown = false;
	int index = m_ctrlList->GetSelection();
	if (0 <= index)
	{
		bEnableUp = bEnableDown = true;
		if (0 == index)
			bEnableUp = false;
		if (index == static_cast<int>(m_ctrlList->GetCount()) - 1)
			bEnableDown = false;
	}
	m_ctrlUp->Enable(bEnableUp);
	m_ctrlDown->Enable(bEnableDown);
}


void CDlgReorder::OnItemSelected(wxCommandEvent& /*evt*/)
{
	UpdateControls();
}


void CDlgReorder::OnMoveUp(wxCommandEvent& evt)
{
	int index = m_ctrlList->GetSelection();
	if (wxNOT_FOUND != index)
	{
		int newIndex = index - 1;
		if (0 <= newIndex)
		{
			ARBBasePtr pTmp = m_Items[index];
			m_Items[index] = m_Items[newIndex];
			m_Items[newIndex] = pTmp;
			LoadData();
			m_ctrlList->SetSelection(newIndex);
			UpdateControls();
		}
	}
}


void CDlgReorder::OnMoveDown(wxCommandEvent& evt)
{
	int index = m_ctrlList->GetSelection();
	if (wxNOT_FOUND != index)
	{
		int newIndex = index + 1;
		if (newIndex < static_cast<int>(m_ctrlList->GetCount()))
		{
			ARBBasePtr pTmp = m_Items[index];
			m_Items[index] = m_Items[newIndex];
			m_Items[newIndex] = pTmp;
			LoadData();
			m_ctrlList->SetSelection(newIndex);
			UpdateControls();
		}
	}
}
