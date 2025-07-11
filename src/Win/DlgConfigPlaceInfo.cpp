/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgConfigPlaceInfo class
 * @author David Connet
 *
 * Revision History
 * 2015-01-01 Changed pixels to dialog units.
 * 2012-02-16 Fix initial focus.
 * 2009-02-11 Ported to wxWidgets.
 * 2006-11-04 Created.
 */

#include "stdafx.h"
#include "DlgConfigPlaceInfo.h"

#include "AgilityBook.h"

#include "ARBCommon/StringUtil.h"
#include "LibARBWin/DlgPadding.h"
#include "LibARBWin/Validators.h"
#include "LibARBWin/Widgets.h"
#include <wx/valgen.h>
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

class CPlaceValidator : public CGenericValidator
{
public:
	CPlaceValidator(std::vector<short> const& existingPlaces, short* val)
		: CGenericValidator(val)
		, m_ExistingPlaces(existingPlaces)
		, m_origPlace(*val)
	{
	}
	wxObject* Clone() const override
	{
		return new CPlaceValidator(*this);
	}

	bool Validate(wxWindow* parent) override;

private:
	std::vector<short> const& m_ExistingPlaces;
	short m_origPlace;
};


bool CPlaceValidator::Validate(wxWindow* parent)
{
	// We know...
	CTextCtrl* pControl = dynamic_cast<CTextCtrl*>(m_validatorWindow);
	short val = static_cast<short>(wxAtol(pControl->GetValue()));

	if (val != m_origPlace)
	{
		if (m_ExistingPlaces.end() != std::find(m_ExistingPlaces.begin(), m_ExistingPlaces.end(), val))
		{
			wxMessageBox(_("IDS_CONFIG_PLACE_UNIQUE"), _("Validation conflict"), wxOK | wxICON_WARNING, parent);
			return false;
		}
	}
	if (0 > val)
	{
		wxMessageBox(_("IDS_CONFIG_PLACE_TOOSMALL"), _("Validation conflict"), wxOK | wxICON_WARNING, parent);
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////

class CValueValidator : public CGenericValidator
{
public:
	CValueValidator(double* val)
		: CGenericValidator(val, 0)
		, m_origValue(*val)
	{
	}
	wxObject* Clone() const override
	{
		return new CValueValidator(*this);
	}

	bool Validate(wxWindow* parent) override;

private:
	double m_origValue;
};


bool CValueValidator::Validate(wxWindow* parent)
{
	// We know...
	CTextCtrl* pControl = dynamic_cast<CTextCtrl*>(m_validatorWindow);
	double val = StringUtil::ToDouble(pControl->GetValue(), val);

	if (0.0 > val)
	{
		// Note: In Win-version, this was a yesno question. But the text
		// doesn't support that. Not sure why it was yesno...
		wxMessageBox(_("IDS_CONFIG_CHECK_VALUE"), _("Validation conflict"), wxOK | wxICON_WARNING, parent);
		return false;
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////

CDlgConfigPlaceInfo::CDlgConfigPlaceInfo(
	std::vector<short> const& inExistingPlaces,
	short inPlace,
	double inValue,
	wxWindow* pParent)
	: wxDialog()
	, m_Place(inPlace)
	, m_Value(inValue)
	, m_trace("CDlgConfigPlaceInfo")
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_CONFIG_PLACEINFO"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	// Controls (these are done first to control tab order)

	wxStaticText* textNote
		= new wxStaticText(this, wxID_ANY, _("IDD_CONFIG_PLACEINFO_TEXT"), wxDefaultPosition, wxDefaultSize, 0);
	textNote->Wrap(wxDLG_UNIT_X(this, 150));

	wxStaticText* textPlace
		= new wxStaticText(this, wxID_ANY, _("IDC_CONFIG_PLACEINFO_PLACE"), wxDefaultPosition, wxDefaultSize, 0);
	textPlace->Wrap(-1);

	CTextCtrl* ctrlPlace = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		CPlaceValidator(inExistingPlaces, &m_Place));
	ctrlPlace->SetHelpText(_("HIDC_CONFIG_PLACEINFO_PLACE"));
	ctrlPlace->SetToolTip(_("HIDC_CONFIG_PLACEINFO_PLACE"));

	wxStaticText* textValue
		= new wxStaticText(this, wxID_ANY, _("IDC_CONFIG_PLACEINFO_VALUE"), wxDefaultPosition, wxDefaultSize, 0);
	textValue->Wrap(-1);

	CTextCtrl* ctrlValue
		= new CTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, CValueValidator(&m_Value));
	ctrlValue->SetHelpText(_("HIDC_CONFIG_PLACEINFO_VALUE"));
	ctrlValue->SetToolTip(_("HIDC_CONFIG_PLACEINFO_VALUE"));

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(textNote, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerPlace = new wxBoxSizer(wxHORIZONTAL);
	sizerPlace->Add(textPlace, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerPlace->Add(ctrlPlace, 0, wxALIGN_CENTER_VERTICAL);
	bSizer->Add(sizerPlace, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerValue = new wxBoxSizer(wxHORIZONTAL);
	sizerValue->Add(textValue, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerValue->Add(ctrlValue, 0, wxALIGN_CENTER_VERTICAL);
	bSizer->Add(sizerValue, 0, wxEXPAND | wxALL, padding.Controls());

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.ButtonSizer());

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	IMPLEMENT_ON_INIT(ctrlPlace)
}

} // namespace dconSoft
