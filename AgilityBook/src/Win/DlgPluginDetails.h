#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief Configure plugins for direct download of calendar data.
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2007-12-24 DRC Created
 */

#include "ARBConfigCalSite.h"
class ARBConfig;
class CTextCtrl;


class CDlgPluginDetails : public wxDialog
{
public:
	CDlgPluginDetails(
			ARBConfig& inConfig,
			ARBConfigCalSitePtr calSite,
			wxWindow* pParent = NULL);

private:
	void SetCodeText();

	ARBConfig& m_Config;
	ARBConfigCalSitePtr m_OrigCalSite;
	ARBConfigCalSitePtr m_CalSite;
	CTextCtrl* m_ctrlName;
	wxString m_strName;
	wxString m_strDesc;
	wxString m_strSearch;
	wxStaticText* m_ctrlCodes;
	wxString m_strHelp;

	DECLARE_EVENT_TABLE()
	void OnPluginDetailCodes(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
