#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgPartner class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "ARBTypes.h"
#include <set>


class CDlgPartner : public wxDialog
{
public:
	CDlgPartner(
			ARBDogRunPartnerPtr partner,
			std::set<std::wstring> const& inHandlers,
			std::set<std::wstring> const& inDogs,
			wxWindow* pParent = NULL);

private:
	wxString m_Handler;
	wxString m_Dog;
	wxString m_RegNum;
	ARBDogRunPartnerPtr m_Partner;

	DECLARE_ON_INIT()
	DECLARE_EVENT_TABLE()
	void OnOk(wxCommandEvent& evt);
};
