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
constexpr wchar_t STREAM_DATA_BEGIN[] = L"ARB Begin Data";
constexpr wchar_t STREAM_DATA_END[] = L"ARB End Data";
constexpr wchar_t STREAM_SYSTEM_BEGIN[] = L"ARB Begin SysInfo";
constexpr wchar_t STREAM_SYSTEM_END[] = L"ARB End SysInfo";
constexpr wchar_t STREAM_REGISTRY_BEGIN[] = L"ARB Begin Registry";
constexpr wchar_t STREAM_REGISTRY_END[] = L"ARB End Registry";
constexpr wchar_t STREAM_FILE_BEGIN[] = L"ARB Begin File";
constexpr wchar_t STREAM_FILE_END[] = L"ARB End File";


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
