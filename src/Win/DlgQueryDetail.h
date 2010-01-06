#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgQueryDetail class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2007-12-24 DRC Created
 */

#include "ARBTypes.h"
class ARBConfig;
class CTextCtrl;
class CVenueComboBox;


class CDlgQueryDetail : public wxDialog
{
public:
	CDlgQueryDetail(
			bool bReadOnly,
			wxChar const* inCode,
			wxChar const* inName,
			wxWindow* pParent = NULL,
			ARBConfig const* inConfig = NULL); // Only set when using venues

	wxString const& GetDetailCode() const		{return m_Code;}
	wxString const& GetDetailName() const		{return m_Name;}

private:
	bool m_ReadOnly;
	ARBConfig const* m_Config;
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
