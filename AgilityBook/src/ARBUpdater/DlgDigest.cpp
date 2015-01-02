/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Generate a checksum for the given file.
 * @author David Connet
 *
 * Revision History
 * 2010-02-09 Created
 */

#include "stdafx.h"
#include "DlgDigest.h"

#include "ARBCommon/ARBMsgDigest.h"
#include "ARBCommon/StringUtil.h"

#include "../Win/Globals.h"
#include "../Win/ImageHelper.h"
#include <wx/filedlg.h>
#include <wx/valgen.h>
#include <wx/valtext.h>
#include <wx/wfstream.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

class CLongValidator : public wxValidator
{
DECLARE_CLASS(CLongValidator)
public:
	CLongValidator(long* val)
		: m_pLong(val)
	{
	}
	CLongValidator(CLongValidator const& rhs)
		: m_pLong(rhs.m_pLong)
	{
		Copy(rhs);
	}
	virtual wxObject *Clone() const {return new CLongValidator(*this);}
	virtual bool TransferFromWindow();
	virtual bool TransferToWindow();
	virtual bool Validate(wxWindow* parent);
private:
	long* m_pLong;
};


IMPLEMENT_CLASS(CLongValidator, wxValidator)


bool CLongValidator::TransferFromWindow()
{
	// Following the example of wxGenericValidator
	if (m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
	{
		wxTextCtrl* pControl = (wxTextCtrl*)m_validatorWindow;
		if (m_pLong)
		{
			if (!StringUtil::ToLong(StringUtil::stringW(pControl->GetValue()), *m_pLong))
				return false;
			return true;
		}
	}
	return false;
}


bool CLongValidator::TransferToWindow()
{
	if (m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
	{
		wxTextCtrl* pControl = (wxTextCtrl*)m_validatorWindow;
		if (m_pLong)
		{
			wxString str;
			str.Printf(L"%ld", *m_pLong);
			pControl->SetValue(str);
			return true;
		}
	}
	return false;
}


bool CLongValidator::Validate(wxWindow* parent)
{
	if (!m_validatorWindow->IsEnabled())
		return true;
	if (m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
	{
		wxTextCtrl* pControl = (wxTextCtrl*)m_validatorWindow;
		if (m_pLong)
		{
			long val;
			if (!StringUtil::ToLong(StringUtil::stringW(pControl->GetValue()), val))
				return false;
		}
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(CDlgDigest, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgDigest::OnOk)
END_EVENT_TABLE()


CDlgDigest::CDlgDigest(wxString const& inFile)
	: m_File(inFile)
	, m_MD5()
	, m_SHA1()
	, m_SHA256()
	, m_Size(0)
{
	Create(nullptr, wxID_ANY, L"MD5/SHA1/SHA256 Checksum", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	wxIconBundle icons;
	icons.AddIcon(ImageHelper::CreateIconFromBitmap(wxBITMAP_PNG(AgilityBook16)));
	icons.AddIcon(ImageHelper::CreateIconFromBitmap(wxBITMAP_PNG(AgilityBook32)));
	icons.AddIcon(ImageHelper::CreateIconFromBitmap(wxBITMAP_PNG(AgilityBook48)));
	icons.AddIcon(ImageHelper::CreateIconFromBitmap(wxBITMAP_PNG(AgilityBook256)));
	SetIcons(icons);

	if (!m_File.empty())
	{
		wxBusyCursor wait;
		size_t size = 0;

		struct {
			ARBMsgDigest::ARBDigest type;
			wxString* pHash;
			size_t* pSize;
		} types[] = {
			{ARBMsgDigest::ARBDigestMD5, &m_MD5, &size},
			{ARBMsgDigest::ARBDigestSHA1, &m_SHA1, nullptr},
			{ARBMsgDigest::ARBDigestSHA256, &m_SHA256, nullptr},
		};
		for (size_t i = 0; i < _countof(types); ++i)
		{
			wxFileInputStream file(m_File);
			wxStdInputStream stdfile(file);
			*(types[i].pHash) = ARBMsgDigest::Compute(stdfile, types[i].type, types[i].pSize);
		}
		m_Size = static_cast<long>(size);

		if (m_MD5.empty())
			m_File.erase();
		else
			m_Size = static_cast<long>(size);
	}

	wxTextCtrl* ctrlFile = new wxTextCtrl(this, wxID_ANY,
		wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 260), -1), 0,
		wxTextValidator(wxFILTER_NONE, &m_File));

	wxTextCtrl* ctrlMD5 = new wxTextCtrl(this, wxID_ANY,
		wxEmptyString,
		wxDefaultPosition, wxDefaultSize, wxTE_READONLY,
		wxTextValidator(wxFILTER_NONE, &m_MD5));
	ctrlMD5->SetBackgroundColour(GetBackgroundColour());

	wxTextCtrl* ctrlSHA1 = new wxTextCtrl(this, wxID_ANY,
		wxEmptyString,
		wxDefaultPosition, wxDefaultSize, wxTE_READONLY,
		wxTextValidator(wxFILTER_NONE, &m_SHA1));
	ctrlSHA1->SetBackgroundColour(GetBackgroundColour());

	wxTextCtrl* ctrlSHA256 = new wxTextCtrl(this, wxID_ANY,
		wxEmptyString,
		wxDefaultPosition, wxDefaultSize, wxTE_READONLY,
		wxTextValidator(wxFILTER_NONE, &m_SHA256));
	ctrlSHA256->SetBackgroundColour(GetBackgroundColour());

	wxTextCtrl* ctrlSize = new wxTextCtrl(this, wxID_ANY,
		wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 115), -1), wxTE_READONLY,
		CLongValidator(&m_Size));
	ctrlSize->SetBackgroundColour(GetBackgroundColour());

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(ctrlFile, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	bSizer->Add(ctrlMD5, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	bSizer->Add(ctrlSHA1, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	bSizer->Add(ctrlSHA256, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	bSizer->Add(ctrlSize, 0, wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	wxButton* ok = wxDynamicCast(FindWindowInSizer(sdbSizer, wxID_OK), wxButton);
	ok->SetLabel(L"Find...");
	wxButton* cancel = wxDynamicCast(FindWindowInSizer(sdbSizer, wxID_CANCEL), wxButton);
	cancel->SetLabel(L"Close");
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz = GetSize();
	SetSizeHints(sz, wxSize(-1, sz.y));
	ok->SetFocus();
	CenterOnParent();
}


void CDlgDigest::OnOk(wxCommandEvent& evt)
{
	wxFileDialog dlg(nullptr, wxFileSelectorPromptStr, wxEmptyString, m_File);
	if (wxID_OK == dlg.ShowModal())
	{
		wxBusyCursor wait;
		m_File = dlg.GetPath();
		size_t size = 0;
		struct {
			ARBMsgDigest::ARBDigest type;
			wxString* pHash;
			size_t* pSize;
		} types[] = {
			{ARBMsgDigest::ARBDigestMD5, &m_MD5, &size},
			{ARBMsgDigest::ARBDigestSHA1, &m_SHA1, nullptr},
			{ARBMsgDigest::ARBDigestSHA256, &m_SHA256, nullptr},
		};
		for (size_t i = 0; i < _countof(types); ++i)
		{
			wxFileInputStream file(m_File);
			wxStdInputStream stdfile(file);
			*(types[i].pHash) = ARBMsgDigest::Compute(stdfile, types[i].type, types[i].pSize);
		}
		m_Size = static_cast<long>(size);
		TransferDataToWindow();
	}
}
