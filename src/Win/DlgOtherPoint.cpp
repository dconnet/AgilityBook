/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgOtherPoint class
 * @author David Connet
 *
 * Revision History
 * 2016-11-29 Fix sizer assertions.
 * 2015-01-01 Changed pixels to dialog units.
 * 2013-01-23 Initialize points correctly when editing existing entry.
 * 2012-02-16 Fix initial focus.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 */

#include "stdafx.h"
#include "DlgOtherPoint.h"

#include "AgilityBook.h"
#include "DlgConfigOtherPoints.h"

#include "ARB/ARBConfig.h"
#include "ARB/ARBConfigOtherPoints.h"
#include "ARB/ARBDogRunOtherPoints.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/DlgPadding.h"
#include "LibARBWin/RichEditCtrl2.h"
#include "LibARBWin/Validators.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

class COtherPointData : public wxClientData
{
public:
	COtherPointData(ARBConfigOtherPointsPtr const& other)
		: pOther(other)
	{
	}
	ARBConfigOtherPointsPtr pOther;
};

/////////////////////////////////////////////////////////////////////////////

CDlgOtherPoint::CDlgOtherPoint(ARBConfig& config, ARBDogRunOtherPointsPtr const& inRunOther, wxWindow* pParent)
	: wxDialog()
	, m_Config(config)
	, m_pRunOther(inRunOther)
	, m_ctrlOtherPoints(nullptr)
	, m_ctrlDesc(nullptr)
	, m_Points(inRunOther->GetPoints())
	, m_trace("CDlgOtherPoint")
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(
		pParent,
		wxID_ANY,
		_("IDD_OTHER_POINT"),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	m_ctrlOtherPoints = new wxComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY | wxCB_SORT);
	m_ctrlOtherPoints->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgOtherPoint::OnSelchangeOtherpoints, this);
	m_ctrlOtherPoints->SetHelpText(_("HIDC_OTHER_OTHERPOINTS"));
	m_ctrlOtherPoints->SetToolTip(_("HIDC_OTHER_OTHERPOINTS"));

	wxButton* ctrlNew = new wxButton(this, wxID_ANY, _("IDC_OTHER_NEW"), wxDefaultPosition, wxDefaultSize, 0);
	ctrlNew->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgOtherPoint::OnOtherNew, this);
	ctrlNew->SetHelpText(_("HIDC_OTHER_NEW"));
	ctrlNew->SetToolTip(_("HIDC_OTHER_NEW"));

	m_ctrlDesc
		= new CRichEditCtrl2(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDLG_UNIT(this, wxSize(120, 65)), true);
	m_ctrlDesc->SetHelpText(_("HIDC_OTHER_DESC"));
	m_ctrlDesc->SetToolTip(_("HIDC_OTHER_DESC"));

	wxStaticText* textPoints
		= new wxStaticText(this, wxID_ANY, _("IDC_OTHER_POINTS"), wxDefaultPosition, wxDefaultSize, 0);
	textPoints->Wrap(-1);

	CTextCtrl* ctrlPoints = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		CGenericValidator(&m_Points));
	ctrlPoints->SetHelpText(_("HIDC_OTHER_POINTS"));
	ctrlPoints->SetToolTip(_("HIDC_OTHER_POINTS"));

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerItems = new wxBoxSizer(wxHORIZONTAL);
	sizerItems->Add(m_ctrlOtherPoints, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Controls());
	sizerItems->Add(ctrlNew, 0, wxALIGN_CENTER_VERTICAL);

	bSizer->Add(sizerItems, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());
	bSizer->Add(m_ctrlDesc, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerPoints = new wxBoxSizer(wxHORIZONTAL);
	sizerPoints->Add(textPoints, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Controls());
	sizerPoints->Add(ctrlPoints, 0, wxALIGN_CENTER_VERTICAL);

	bSizer->Add(sizerPoints, 0, wxEXPAND | wxALL, padding.Controls());

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.ButtonSizer());
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgOtherPoint::OnOk, this, wxID_OK);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	IMPLEMENT_ON_INIT(CDlgOtherPoint, m_ctrlOtherPoints)

	LoadPoints(ARBConfigOtherPointsPtr());
}


void CDlgOtherPoint::LoadPoints(ARBConfigOtherPointsPtr const& inOther)
{
	m_ctrlOtherPoints->Clear();
	m_ctrlDesc->SetValue(L"");

	wxString current;
	wxArrayString items;
	std::vector<wxClientData*> data;
	for (auto const& pOther : m_Config.GetOtherPoints())
	{
		items.Add(pOther->GetName());
		data.push_back(new COtherPointData(pOther));
		if ((!inOther && pOther->GetName() == m_pRunOther->GetName()) || (inOther && *inOther == *pOther))
		{
			current = pOther->GetName();
			m_ctrlDesc->SetValue(pOther->GetDescription());
			if (0.0 == m_Points)
				m_Points = pOther->GetDefault();
			TransferDataToWindow();
		}
	}
	m_ctrlOtherPoints->Append(items, data.data());
	m_ctrlOtherPoints->SetSelection(m_ctrlOtherPoints->FindString(current));
}


void CDlgOtherPoint::OnSelchangeOtherpoints(wxCommandEvent& evt)
{
	int index = m_ctrlOtherPoints->GetSelection();
	if (wxNOT_FOUND != index)
	{
		COtherPointData* pData = dynamic_cast<COtherPointData*>(m_ctrlOtherPoints->GetClientObject(index));
		m_ctrlDesc->SetValue(pData->pOther->GetDescription());
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

} // namespace dconSoft
