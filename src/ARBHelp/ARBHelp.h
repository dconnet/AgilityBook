#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Identifying strings
 * @author David Connet
 *
 * Revision History
 * 2009-03-01 Ported to wxWidgets.
 * 2007-02-08 Created
 */

#include "LibARBWin/SetupApp.h"


namespace dconSoft
{
constexpr char STREAM_DATA_BEGIN[] = "ARB Begin Data";
constexpr char STREAM_DATA_END[] = "ARB End Data";
constexpr char STREAM_SYSTEM_BEGIN[] = "ARB Begin SysInfo";
constexpr char STREAM_SYSTEM_END[] = "ARB End SysInfo";
constexpr char STREAM_REGISTRY_BEGIN[] = "ARB Begin Registry";
constexpr char STREAM_REGISTRY_END[] = "ARB End Registry";
constexpr char STREAM_FILE_BEGIN[] = "ARB Begin File";
constexpr char STREAM_FILE_END[] = "ARB End File";


class CARBHelpApp : public ARBWin::CBaseApp
{
	DECLARE_NO_COPY_IMPLEMENTED(CARBHelpApp)
public:
	CARBHelpApp();
	bool InitLanguage() override
	{
		return true;
	}
	bool OnInit() override;

	bool GetResImageName(wxArtID const& id, wxArtClient const& client, wxString& outName, bool& outSvg) const override;
};

} // namespace dconSoft

wxDECLARE_APP(dconSoft::CARBHelpApp);
