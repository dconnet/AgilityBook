/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgRun class
 * @author David Connet
 *
 * Revision History
 * 2021-01-07 Separated into multiple files.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 */

#include "stdafx.h"
#include "DlgRunPanelComments.h"

#include "DlgListCtrl.h"

#include "ARB/ARBDogRun.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/Validators.h"
#include "LibARBWin/Widgets.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

CDlgRunPanelComments::CDlgRunPanelComments(
	CDlgRun* pDlg,
	CAgilityBookDoc* pDoc,
	ARBDogPtr const& inDog,
	ARBDogTrialPtr const& inTrial,
	ARBDogRunPtr const& inRun,
	wxWindow* parent)
	: CDlgRunPanelBase(pDlg, pDoc, inDog, inTrial, inRun, parent)
	, m_Comments(StringUtil::stringWX(inRun->GetNote()))
	, m_ctrlFaultsList(nullptr)
{
	CSpellCheckCtrl* ctrlComments = new CSpellCheckCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		wxTE_MULTILINE | wxTE_WORDWRAP,
		CTrimValidator(&m_Comments, TRIMVALIDATOR_TRIM_BOTH));
	ctrlComments->SetHelpText(_("HIDC_RUNCOMMENT_COMMENTS"));
	ctrlComments->SetToolTip(_("HIDC_RUNCOMMENT_COMMENTS"));

	m_ctrlFaultsList = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr, 0);
	m_ctrlFaultsList->SetHelpText(_("HIDC_RUNCOMMENT_FAULTS_LIST"));
	m_ctrlFaultsList->SetToolTip(_("HIDC_RUNCOMMENT_FAULTS_LIST"));

	wxButton* btnFaults
		= new wxButton(this, wxID_ANY, _("IDC_RUNCOMMENT_COMMENTS_FAULTS"), wxDefaultPosition, wxDefaultSize, 0);
	btnFaults->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRunPanelComments::OnCommentsFaults, this);
	btnFaults->SetHelpText(_("HIDC_RUNCOMMENT_COMMENTS_FAULTS"));
	btnFaults->SetToolTip(_("HIDC_RUNCOMMENT_COMMENTS_FAULTS"));

	// Sizers

	wxBoxSizer* sizerCommentsFaults = new wxBoxSizer(wxHORIZONTAL);
	sizerCommentsFaults->Add(ctrlComments, 1, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerFaultsList = new wxBoxSizer(wxVERTICAL);
	sizerFaultsList->Add(m_ctrlFaultsList, 1, wxEXPAND, 0);
	sizerFaultsList->Add(btnFaults, 0, wxTOP, wxDLG_UNIT_X(this, 5));

	sizerCommentsFaults->Add(sizerFaultsList, 0, wxEXPAND | wxRIGHT | wxTOP | wxBOTTOM, wxDLG_UNIT_X(this, 5));

	SetSizer(sizerCommentsFaults);
	Layout();
	sizerCommentsFaults->Fit(this);

	SetFaultsText();
}


wxWindow* CDlgRunPanelComments::GetInitialControl()
{
	return m_ctrlFaultsList;
}


void CDlgRunPanelComments::SetFaultsText()
{
	m_ctrlFaultsList->Clear();
	for (ARBDogFaultList::const_iterator iter = m_Run->GetFaults().begin(); iter != m_Run->GetFaults().end(); ++iter)
	{
		m_ctrlFaultsList->Append(StringUtil::stringWX(*iter));
	}
}


void CDlgRunPanelComments::OnCommentsFaults(wxCommandEvent& evt)
{
	CDlgListCtrl dlg(ARBWhatToList::Faults, m_pDoc, m_Run, this);
	if (wxID_OK == dlg.ShowModal())
		SetFaultsText();
}


bool CDlgRunPanelComments::Validate()
{
	return true;
}


bool CDlgRunPanelComments::Save()
{
	m_Run->SetNote(StringUtil::stringW(m_Comments));
	return true;
}

} // namespace dconSoft
