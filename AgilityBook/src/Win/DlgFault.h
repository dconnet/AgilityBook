#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgFault class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "ARB/ARBTypes.h"
#include <set>


class CDlgFault : public wxDialog
{
public:
	CDlgFault(
			std::set<std::wstring>& faults,
			std::wstring const& fault,
			wxWindow* pParent = NULL);
	std::wstring GetFault() const;

private:
	wxString m_Fault;

	DECLARE_ON_INIT()
};
