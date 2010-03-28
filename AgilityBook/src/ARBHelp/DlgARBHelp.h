#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Main propertysheet
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-03-01 DRC Ported to wxWidgets.
 */

#include "ARBTypes.h"
#include <map>
#include <wx/wizard.h>
class CDlgPageEncode;


class CDlgARBHelp : public wxWizard
{
public:
	CDlgARBHelp();

	void AddSysInfo(wxString const& inData);
	void AddRegistryInfo(wxChar const* inData);
	void SetARBFileStatus(wxString const& inFileName, bool bInclude = true);

	typedef std::map<wxString, bool> FileMap;
	FileMap const& GetARBFiles() const			{return m_IncFile;}

	wxString GetEncodedData();

protected:
	wxString m_SysInfo;
	wxString m_RegInfo;
	FileMap m_IncFile;
};
