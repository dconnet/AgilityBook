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

#include "ARB/ARBTypes2.h"
#include "LibARBWin/SetupApp.h"
#include "../Win/ImageHelper.h"
#include "../Win/Localization.h"


class CCalSitesApp : public CBaseApp
{
	DECLARE_NO_COPY_IMPLEMENTED(CCalSitesApp)
public:
	CCalSitesApp();

	// IImageManagerCallback interface
	bool OnCreateBitmap(const wxArtID& id, const wxArtClient& client, const wxSize& size, wxBitmap& outBmp) override;
	bool OnCreateIconBundle(const wxArtID& id, const wxArtClient& client, wxIconBundle& outIcon) override;

	bool InitLanguage() override;
	bool OnInit() override;

private:
	CLocalization m_Localization;
	CImageHelper m_imageHelper;
};

DECLARE_APP(CCalSitesApp)


class CErrorCallback : public ARBErrorCallback
{
public:
	CErrorCallback();
	bool OnError(wchar_t const* const pMsg) override;
	fmt::wmemory_buffer m_ErrMsg;
};
