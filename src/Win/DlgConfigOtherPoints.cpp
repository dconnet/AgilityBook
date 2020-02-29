/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgConfigOtherPoints class
 * @author David Connet
 *
 * Revision History
 * 2015-01-01 Changed pixels to dialog units.
 * 2012-02-16 Fix initial focus.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-05-22 Fixed CRNL sequences.
 */

#include "stdafx.h"
#include "DlgConfigOtherPoints.h"

#include "AgilityBook.h"

#include "ARB/ARBConfig.h"
#include "ARB/ARBConfigOtherPoints.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/Validators.h"
#include "LibARBWin/Widgets.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

class CNameValidator : public CTrimValidator
{
public:
	CNameValidator(
			CDlgConfigOtherPoints* dlg,
			wxString* valPtr)
		: CTrimValidator(valPtr)
		, m_Dlg(dlg)
	{
	}

	bool Validate(wxWindow* parent) override;

private:
	CDlgConfigOtherPoints* m_Dlg;
};


bool CNameValidator::Validate(wxWindow* parent)
{
	if (!CTrimValidator::Validate(parent))
		return false;

	wxString* tmpString = m_pString;
	wxString tmp;
	m_pString = &tmp;
	TransferFromWindow();
	m_pString = tmpString;

	if (!m_Dlg->IsNameOkay(StringUtil::stringW(tmp)))
	{
		m_validatorWindow->SetFocus();
		wxMessageBox(_("IDS_NAME_IN_USE"), _("Validation conflict"), wxOK | wxICON_EXCLAMATION, parent);
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////

namespace
{
	static struct
	{
		wchar_t const* idDesc;
		ARBOtherPointsTally tally;
	} const sc_Tally[] =
	{
		{arbT("IDS_OTHERPTS_ALL"), ARBOtherPointsTally::All},
		{arbT("IDS_OTHERPTS_EVENT"), ARBOtherPointsTally::AllByEvent},
		{arbT("IDS_OTHERPTS_LEVEL"), ARBOtherPointsTally::Level},
		{arbT("IDS_OTHERPTS_EVENT_LEVEL"), ARBOtherPointsTally::LevelByEvent}
	};
	constexpr int sc_numTally = sizeof(sc_Tally) / sizeof(sc_Tally[0]);
}


class DlgConfigOtherPointData : public wxClientData 
{
public:
	DlgConfigOtherPointData(ARBOtherPointsTally tally)
		: m_Tally(tally)
	{
	}
	ARBOtherPointsTally m_Tally;
};

/////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(CDlgConfigOtherPoints, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgConfigOtherPoints::OnOk)
wxEND_EVENT_TABLE()


CDlgConfigOtherPoints::CDlgConfigOtherPoints(
		ARBConfig& config,
		ARBConfigOtherPointsPtr const& inOther,
		wxWindow* pParent)
	: wxDialog()
	, m_Config(config)
	, m_pOther(inOther)
	, m_Name(StringUtil::stringWX(inOther->GetName()))
	, m_ctrlTally(nullptr)
	, m_Default(inOther->GetDefault())
	, m_Desc(StringUtil::stringWX(inOther->GetDescription()))
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_CONFIG_OTHERPOINTS"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	wxStaticText* textName = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_OTHER_NAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	textName->Wrap(-1);

	CTextCtrl* ctrlName = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CNameValidator(this, &m_Name));
	ctrlName->SetHelpText(_("HIDC_CONFIG_OTHER_NAME"));
	ctrlName->SetToolTip(_("HIDC_CONFIG_OTHER_NAME"));

	wxStaticText* textTally = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_OTHER_TALLY"),
		wxDefaultPosition, wxDefaultSize, 0);
	textTally->Wrap(-1);

	m_ctrlTally = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, nullptr,
		wxCB_DROPDOWN|wxCB_READONLY|wxCB_SORT);
	for (int index = 0; index < sc_numTally; ++index)
	{
		wxString desc = wxGetTranslation(sc_Tally[index].idDesc);
		int id = m_ctrlTally->Append(desc);
		m_ctrlTally->SetClientObject(id, new DlgConfigOtherPointData(sc_Tally[index].tally));
		if (m_pOther->GetTally() == sc_Tally[index].tally)
			m_ctrlTally->SetSelection(id);
	}
	m_ctrlTally->SetHelpText(_("HIDC_CONFIG_OTHER_TALLY"));
	m_ctrlTally->SetToolTip(_("HIDC_CONFIG_OTHER_TALLY"));

	wxStaticText* textPoints = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_OTHER_DEFAULT"),
		wxDefaultPosition, wxDefaultSize, 0);
	textPoints->Wrap(-1);

	CTextCtrl* ctrlPoints = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 30), -1), 0,
		CGenericValidator(&m_Default));
	ctrlPoints->SetHelpText(_("HIDC_CONFIG_OTHER_DEFAULT"));
	ctrlPoints->SetToolTip(_("HIDC_CONFIG_OTHER_DEFAULT"));

	wxStaticText* textDesc = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_OTHER_DESC"),
		wxDefaultPosition, wxDefaultSize, 0);
	textDesc->Wrap(-1);

	CSpellCheckCtrl* ctrlDesc = new CSpellCheckCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDLG_UNIT(this, wxSize(120, 60)),
		wxTE_MULTILINE|wxTE_WORDWRAP,
		CTrimValidator(&m_Desc, TRIMVALIDATOR_TRIM_BOTH));
	ctrlDesc->SetHelpText(_("HIDC_CONFIG_OTHER_DESC"));
	ctrlDesc->SetToolTip(_("HIDC_CONFIG_OTHER_DESC"));

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerName = new wxBoxSizer(wxHORIZONTAL);
	sizerName->Add(textName, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerName->Add(ctrlName, 1, wxALIGN_CENTER_VERTICAL, 0);

	bSizer->Add(sizerName, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerTally = new wxBoxSizer(wxHORIZONTAL);
	sizerTally->Add(textTally, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerTally->Add(m_ctrlTally, 1, wxALIGN_CENTER_VERTICAL, 0);

	bSizer->Add(sizerTally, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerPoints = new wxBoxSizer(wxHORIZONTAL);
	sizerPoints->Add(textPoints, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerPoints->Add(ctrlPoints, 0, wxALIGN_CENTER_VERTICAL, 0);

	bSizer->Add(sizerPoints, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	bSizer->Add(textDesc, 0, wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	bSizer->Add(ctrlDesc, 1, wxEXPAND | wxLEFT | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	IMPLEMENT_ON_INIT(CDlgConfigOtherPoints, ctrlName)
}


DEFINE_ON_INIT(CDlgConfigOtherPoints)


bool CDlgConfigOtherPoints::IsNameOkay(std::wstring const& name) const
{
	if (m_pOther->GetName() != name
	&& m_Config.GetOtherPoints().FindOtherPoints(name))
	{
		return false;
	}
	return true;
}


void CDlgConfigOtherPoints::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	m_pOther->SetName(StringUtil::stringW(m_Name));
	m_pOther->SetDefault(m_Default);
	int index = m_ctrlTally->GetSelection();
	if (index != wxNOT_FOUND)
	{
		m_pOther->SetTally(dynamic_cast<DlgConfigOtherPointData*>(m_ctrlTally->GetClientObject(index))->m_Tally);
	}
	m_pOther->SetDescription(StringUtil::stringW(m_Desc));
	EndDialog(wxID_OK);
}
