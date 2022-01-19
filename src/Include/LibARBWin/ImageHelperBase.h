#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Image Manager
 * @author David Connet
 *
 * Usage:
 * Users of this can extend by associating a callback class.
 *
 * Revision History
 * 2020-08-22 Fixed for usage with wx3.1.5
 * 2018-10-11 Moved to Win LibARBWin
 * 2015-08-22 Expose LOAD_BITMAP_PNG for use in OnCreateBitmap.
 * 2013-10-13 Changed ImageManager to an art provider.
 * 2012-12-29 Created.
 */

#include "LibwxARBWin.h"

#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/icon.h>


namespace ImageHelper
{
ARBWIN_API wxBitmap GetBitmap(
	wxWindow* pWindow,
	const wxArtID& id,
	const wxArtClient& client = wxART_OTHER,
	const wxSize& size = wxDefaultSize);

ARBWIN_API wxIcon GetIcon(
	wxWindow* pWindow,
	const wxArtID& id,
	const wxArtClient& client = wxART_OTHER,
	const wxSize& size = wxDefaultSize);

ARBWIN_API wxIcon CreateIconFromBitmap(const wxBitmap& bitmap);
} // namespace ImageHelper


// Setup image ids for things used in LibARBWin.
// Note the user app must provide the loading of these in the Do* functions.

#define ImageMgrAppBundle wxART_MAKE_ART_ID(ImageMgrAppBundle)
#define ImageMgrApp       wxART_MAKE_ART_ID(ImageMgrApp)

#define ImageMgrBlank     wxART_MAKE_ART_ID(ImageMgrBlank)
#define ImageMgrChecked   wxART_MAKE_ART_ID(ImageMgrChecked)
#define ImageMgrUnChecked wxART_MAKE_ART_ID(ImageMgrUnChecked)
#define ImageMgrCheck     wxART_MAKE_ART_ID(ImageMgrCheck)
#define ImageMgrQuestion  wxART_MAKE_ART_ID(ImageMgrQuestion)

#if !wxCHECK_VERSION(3, 1, 6)
#define ImageMgrHeaderDown wxART_MAKE_ART_ID(ImageMgrHeaderDown)
#define ImageMgrHeaderUp   wxART_MAKE_ART_ID(ImageMgrHeaderUp)
#endif
