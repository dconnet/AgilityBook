/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CDlgOtherPoint class
 * @author David Connet
 *
 * Revision History
 * @li 2012-02-16 DRC Fix initial focus.
 * @li 2011-12-22 DRC Switch to using Bind on wx2.9+.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "stdafx.h"
#include "DlgOtherPoint.h"

#include "AgilityBook.h"
#include "ARBConfig.h"
#include "ARBConfigOtherPoints.h"
#include "ARBDogRunOtherPoints.h"
#include "DlgConfigOtherPoints.h"
#include "RichEditCtrl2.h"
#include "Validators.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


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
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP | GetExtraStyle());
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_OTHER_POINT"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	m_ctrlOtherPoints = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, NULL,
		wxCB_DROPDOWN|wxCB_READONLY|wxCB_SORT);
	BIND_OR_CONNECT_CTRL(m_ctrlOtherPoints, wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler, CDlgOtherPoint::OnSelchangeOtherpoints);
	m_ctrlOtherPoints->SetHelpText(_("HIDC_OTHER_OTHERPOINTS"));
	m_ctrlOtherPoints->SetToolTip(_("HIDC_OTHER_OTHERPOINTS"));

	wxButton* ctrlNew = new wxButton(this, wxID_ANY,
		_("IDC_OTHER_NEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(ctrlNew, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgOtherPoint::OnOtherNew);
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

	CTextCtrl* ctrlPoints = new CTextCtrl(this, wxID_ANY, wxEmptyString,
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

	IMPLEMENT_ON_INIT(CDlgOtherPoint, m_ctrlOtherPoints)

	LoadPoints(ARBConfigOtherPointsPtr());
}


DEFINE_ON_INIT(CDlgOtherPoint)


void CDlgOtherPoint::LoadPoints(ARBConfigOtherPointsPtr inOther)
{
	m_ctrlOtherPoints->Clear();
	m_ctrlDesc->SetValue(L"");
	for (ARBConfigOtherPointsList::const_iterator iterOther = m_Config.GetOtherPoints().begin();
		iterOther != m_Config.GetOtherPoints().end();
		++iterOther)
	{
		ARBConfigOtherPointsPtr pOther = (*iterOther);
		int index = m_ctrlOtherPoints->Append(pOther->GetName());
		m_ctrlOtherPoints->SetClientObject(index, new COtherPointData(pOther));
		if ((!inOther && pOther->GetName() == m_pRunOther->GetName())
		|| (inOther && *inOther == *pOther))
		{
			m_ctrlOtherPoints->SetSelection(index);
			m_ctrlDesc->SetValue(pOther->GetDescription());
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
