/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgPageEncodeFiles class
 * @author David Connet
 *
 * Revision History
 * 2009-03-01 Ported to wxWidgets.
 * 2007-01-02 Created
 */

#include "stdafx.h"
#include "DlgPageEncodeFiles.h"

#include "DlgARBHelp.h"

#include "ARBCommon/StringUtil.h"
#include "LibARBWin/DlgPadding.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARBCommon;

CDlgPageEncodeFiles::CDlgPageEncodeFiles(CDlgARBHelp* pParent)
	: wxWizardPageSimple(pParent)
	, m_Parent(pParent)
	, m_ctrlList(nullptr)
{
	wxStaticText* text1 = new wxStaticText(
		this,
		wxID_ANY,
		L"Check the files you would like to be sent",
		wxDefaultPosition,
		wxDefaultSize,
		0);

	m_ctrlList = new wxCheckListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxLB_HSCROLL);

	wxButton* btnAll = new wxButton(this, wxID_ANY, L"Check All", wxDefaultPosition, wxDefaultSize, 0);
	btnAll->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgPageEncodeFiles::OnCheckAll, this);

	wxButton* btnNone = new wxButton(this, wxID_ANY, L"Uncheck All", wxDefaultPosition, wxDefaultSize, 0);
	btnNone->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgPageEncodeFiles::OnCheckNone, this);

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(text1, 0, wxEXPAND);

	wxBoxSizer* sizerListBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerListBtns->Add(m_ctrlList, 1, wxEXPAND | wxRIGHT, padding.Controls());

	wxBoxSizer* sizerBtns = new wxBoxSizer(wxVERTICAL);
	sizerBtns->Add(btnAll, 0, wxEXPAND | wxBOTTOM, padding.Inner());
	sizerBtns->Add(btnNone, 0, wxEXPAND);
	sizerListBtns->Add(sizerBtns);

	bSizer->Add(sizerListBtns, 1, wxEXPAND | wxTOP, padding.Controls());

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
	for (CDlgARBHelp::FileMap::const_iterator i = m_Parent->GetARBFiles().begin(); i != m_Parent->GetARBFiles().end();
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

} // namespace dconSoft
