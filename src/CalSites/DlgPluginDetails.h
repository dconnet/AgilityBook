#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Configure plugins for direct download of calendar data.
 * @author David Connet
 *
 * Revision History
 * 2009-02-11 Ported to wxWidgets.
 * 2007-12-24 Created
 */

#include "ARBConfigCalSite.h"
class ARBConfig;
class CTextCtrl;


class CDlgPluginDetails : public wxDialog
{
public:
	CDlgPluginDetails(ARBConfigCalSiteList& sites, ARBConfigCalSitePtr const& inCalSite, wxWindow* pParent = nullptr);

private:
	void SetCodeText();

	ARBConfigCalSiteList& m_sites;
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
