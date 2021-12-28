#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgQueryDetail class
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-11 Ported to wxWidgets.
 * 2007-12-24 Created
 */

#include "ARB/ARBTypes2.h"
class ARBConfigCalSiteList;
class CTextCtrl;
class CVenueComboBox;


class CDlgQueryDetail : public wxDialog
{
public:
	CDlgQueryDetail(
		bool bReadOnly,
		wchar_t const* inCode,
		wchar_t const* inName,
		wxWindow* pParent = nullptr,
		ARBConfigCalSiteList const* sites = nullptr); // Only set when using venues

	std::wstring GetDetailCode() const;
	std::wstring GetDetailName() const;

private:
	bool m_ReadOnly;
	ARBConfigCalSiteList const* m_sites;
	wxString m_strCode;
	CTextCtrl* m_ctrlCode;
	wxString m_strName;
	CVenueComboBox* m_ctrlVenues;
	wxString m_Code;
	wxString m_Name;

	DECLARE_EVENT_TABLE()
	void OnSelchangeVenues(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
