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

#include "../Win/Localization.h"
#include "ARB/ARBTypes2.h"
#include "LibARBWin/SetupApp.h"


class CCalSitesApp : public CBaseApp
{
	DECLARE_NO_COPY_IMPLEMENTED(CCalSitesApp)
public:
	CCalSitesApp();

	bool OnInit() override;

	bool InitLanguage() override;

	// IResourceManagerCallback
	bool GetResImageName(
		wxArtID const& id,
		wxArtClient const& client,
		std::wstring& outName,
		bool& outSvg,
		bool& outCall) const override;

private:
	CLocalization m_Localization;
};

wxDECLARE_APP(CCalSitesApp);


class CErrorCallback : public ARBErrorCallback
{
public:
	CErrorCallback();
	bool OnError(wchar_t const* const pMsg) override;
	fmt::wmemory_buffer m_ErrMsg;
};
