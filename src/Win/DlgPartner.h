#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgPartner class
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


class CDlgPartner : public wxDialog
{
public:
	CDlgPartner(
		ARB::ARBDogRunPartnerPtr const& inPartner,
		std::set<std::wstring> const& inHandlers,
		std::set<std::wstring> const& inDogs,
		wxWindow* pParent = nullptr);

private:
	wxString m_Handler;
	wxString m_Dog;
	wxString m_RegNum;
	ARB::ARBDogRunPartnerPtr m_Partner;
	ARBWin::CStackLogger m_trace;

	DECLARE_EVENT_TABLE()
	void OnOk(wxCommandEvent& evt);
};

} // namespace dconSoft
