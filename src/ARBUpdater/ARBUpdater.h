#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief main header file for the CalSites application
 * @author David Connet
 *
 * Revision History
 * 2020-09-12 Created
 */

#include "LibARBWin/SetupApp.h"


namespace dconSoft
{

class CARBUpdaterApp : public ARBWin::CBaseApp
{
	DECLARE_NO_COPY_IMPLEMENTED(CARBUpdaterApp)
public:
	CARBUpdaterApp();
	bool InitLanguage() override
	{
		return true;
	}
	bool OnInit() override;

	bool GetResImageName(wxArtID const& id, wxArtClient const& client, wxString& outName, bool& outSvg) const override;
};

} // namespace dconSoft

wxDECLARE_APP(dconSoft::CARBUpdaterApp);
