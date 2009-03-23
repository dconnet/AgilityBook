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
 * @brief implementation of the CDlgOtherPoint class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "stdafx.h"
#include "DlgOtherPoint.h"

#include "ARBConfig.h"
#include "ARBConfigOtherPoints.h"
#include "ARBDogRunOtherPoints.h"
#include "DlgConfigOtherPoints.h"

#include "RichEditCtrl2.h"
#include "Validators.h"


/////////////////////////////////////////////////////////////////////////////

class COtherPointData : public wxClientData
{
public:
	COtherPointData(ARBConfigOtherPointsPtr other)
		: pOther(other)
	{
	}
	ARBConfigOtherPointsPtr pOther;
};

/////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(CDlgOtherPoint, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgOtherPoint::OnOk)
END_EVENT_TABLE()


CDlgOtherPoint::CDlgOtherPoint(
		ARBConfig& config,
		ARBDogRunOtherPointsPtr pRunOther,
		wxWindow* pParent)
	: wxDialog()
	, m_Config(config)
	, m_pRunOther(pRunOther)
	, m_ctrlOtherPoints(NULL)
	, m_ctrlDesc(NULL)
	, m_Points(pRunOther->GetPoints())
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	Create(pParent, wxID_ANY, _("IDD_OTHER_POINT"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	m_ctrlOtherPoints = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, NULL,
		wxCB_DROPDOWN|wxCB_READONLY|wxCB_SORT);
	m_ctrlOtherPoints->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(CDlgOtherPoint::OnSelchangeOtherpoints), NULL, this);
	m_ctrlOtherPoints->SetHelpText(_("HIDC_OTHER_OTHERPOINTS"));
	m_ctrlOtherPoints->SetToolTip(_("HIDC_OTHER_OTHERPOINTS"));

	wxButton* ctrlNew = new wxButton(this, wxID_ANY,
		_("IDC_OTHER_NEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	ctrlNew->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgOtherPoint::OnOtherNew), NULL, this);
	ctrlNew->SetHelpText(_("HIDC_OTHER_NEW"));
	ctrlNew->SetToolTip(_("HIDC_OTHER_NEW"));

	m_ctrlDesc = new CRichEditCtrl2(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(210,120), true);
	m_ctrlDesc->SetHelpText(_("HIDC_OTHER_DESC"));
	m_ctrlDesc->SetToolTip(_("HIDC_OTHER_DESC"));

	wxStaticText* textPoints = new wxStaticText(this, wxID_ANY,
		_("IDC_OTHER_POINTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textPoints->Wrap(-1);

	wxTextCtrl* ctrlPoints = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CGenericValidator(&m_Points));
	ctrlPoints->SetHelpText(_("HIDC_OTHER_POINTS"));
	ctrlPoints->SetToolTip(_("HIDC_OTHER_POINTS"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerItems = new wxBoxSizer(wxHORIZONTAL);
	sizerItems->Add(m_ctrlOtherPoints, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);
	sizerItems->Add(ctrlNew, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	bSizer->Add(sizerItems, 0, wxEXPAND, 5);
	bSizer->Add(m_ctrlDesc, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerPoints = new wxBoxSizer(wxHORIZONTAL);
	sizerPoints->Add(textPoints, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerPoints->Add(ctrlPoints, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	bSizer->Add(sizerPoints, 0, wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	m_ctrlOtherPoints->SetFocus();

	LoadPoints(ARBConfigOtherPointsPtr());
}


void CDlgOtherPoint::LoadPoints(ARBConfigOtherPointsPtr inOther)
{
	m_ctrlOtherPoints->Clear();
	m_ctrlDesc->SetValue(wxT(""));
	for (ARBConfigOtherPointsList::const_iterator iterOther = m_Config.GetOtherPoints().begin();
		iterOther != m_Config.GetOtherPoints().end();
		++iterOther)
	{
		ARBConfigOtherPointsPtr pOther = (*iterOther);
		int index = m_ctrlOtherPoints->Append(pOther->GetName().c_str());
		m_ctrlOtherPoints->SetClientObject(index, new COtherPointData(pOther));
		if ((!inOther && pOther->GetName() == m_pRunOther->GetName())
		|| (inOther && *inOther == *pOther))
		{
			m_ctrlOtherPoints->SetSelection(index);
			m_ctrlDesc->SetValue(pOther->GetDescription().c_str());
			m_Points = pOther->GetDefault();
			TransferDataToWindow();
		}
	}
}


void CDlgOtherPoint::OnSelchangeOtherpoints(wxCommandEvent& evt)
{
	int index = m_ctrlOtherPoints->GetSelection();
	if (wxNOT_FOUND != index)
	{
		COtherPointData* pData = dynamic_cast<COtherPointData*>(m_ctrlOtherPoints->GetClientObject(index));
		m_ctrlDesc->SetValue(pData->pOther->GetDescription().c_str());
		m_Points = pData->pOther->GetDefault();
		TransferDataToWindow();
	}
}


void CDlgOtherPoint::OnOtherNew(wxCommandEvent& evt)
{
	ARBConfigOtherPointsPtr pOther(ARBConfigOtherPoints::New());
	CDlgConfigOtherPoints dlg(m_Config, pOther, this);
	if (wxID_OK == dlg.ShowModal())
	{
		if (m_Config.GetOtherPoints().AddOtherPoints(pOther))
		{
			LoadPoints(pOther);
		}
	}
}


void CDlgOtherPoint::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	int index = m_ctrlOtherPoints->GetSelection();
	if (wxNOT_FOUND == index)
	{
		return;
	}
	COtherPointData* pData = dynamic_cast<COtherPointData*>(m_ctrlOtherPoints->GetClientObject(index));
	m_pRunOther->SetName(pData->pOther->GetName());
	m_pRunOther->SetPoints(m_Points);
	EndDialog(wxID_OK);
}
