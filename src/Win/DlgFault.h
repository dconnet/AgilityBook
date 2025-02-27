#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgFault class
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 */

#include "ARB/ARBTypes2.h"
#include "LibARBWin/Logger.h"
#include <set>


namespace dconSoft
{


class CDlgFault : public wxDialog
{
public:
	CDlgFault(std::set<wxString>& faults, wxString const& fault, wxWindow* pParent = nullptr);
	wxString GetFault() const;

private:
	wxString m_Fault;
	ARBWin::CStackLogger m_trace;
};

} // namespace dconSoft
