#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Main propertysheet
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-03-01 Ported to wxWidgets.
 */

#include "ARBCommon/ARBTypes.h"
#include <map>
#include <wx/wizard.h>
class CDlgPageEncode;


class CDlgARBHelp : public wxWizard
{
public:
	CDlgARBHelp();

	wxWizardPageSimple* GetFirstPage() const	{return m_firstPage;}

	void AddSysInfo(std::wstring const& inData);
	void AddRegistryInfo(wchar_t const* inData);
	void SetARBFileStatus(std::wstring const& inFileName, bool bInclude = true);

	typedef std::map<std::wstring, bool> FileMap;
	FileMap const& GetARBFiles() const			{return m_IncFile;}

	std::wstring GetEncodedData();

protected:
	std::wstring m_SysInfo;
	std::wstring m_RegInfo;
	FileMap m_IncFile;
	wxWizardPageSimple* m_firstPage;
};
