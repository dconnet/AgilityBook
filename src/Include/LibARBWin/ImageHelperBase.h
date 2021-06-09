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

#include "ImageManager.h"
#include "LibwxARBWin.h"


// Helper functions called from IImageManagerCallback interfaces

namespace ImageHelper
{
ARBWIN_API wxSize GetScaledSize(wxWindow const* pWindow, int logical);
ARBWIN_API wxSize GetScaledSize(wxWindow const* pWindow, wxSize const& szLogical);

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

// These are in each user of this.
// That's where the mapping of names happens via IImageManagerCallback
// ARBWIN_API bool DoCreateBitmap(
//		wxWindow* pWindow,
//		const wxArtID& id,
//		const wxArtClient& client,
//		const wxSize& size,
//		wxBitmap& outBmp);
// ARBWIN_API bool DoCreateIconBundle(
//		wxWindow* pWindow,
//		const wxArtID& id,
//		const wxArtClient& client,
//		wxIconBundle& outIcon);

#if defined(__WINDOWS__)
ARBWIN_API void LoadLocalBitmap(wxWindow const* pWindow, wchar_t const* const pImageName, wxBitmap& outBmp);
#endif
}; // namespace ImageHelper

// Setup image ids for things used in LibARBWin.
// Note the user app must provide the loading of these in the Do* functions.

// Note: wx3.1.5 effectively make wxART_MAKE_ART_ID a private macro.
// So create my own.
#define MAKE_IMAGE_ID(id) wxArtID(#id)

#define ImageMgrBlank     MAKE_IMAGE_ID(ImageMgrBlank)
#define ImageMgrChecked   MAKE_IMAGE_ID(ImageMgrChecked)
#define ImageMgrUnChecked MAKE_IMAGE_ID(ImageMgrUnChecked)
#define ImageMgrCheck     MAKE_IMAGE_ID(ImageMgrCheck)
#define ImageMgrQuestion  MAKE_IMAGE_ID(ImageMgrQuestion)

#define ImageMgrHeaderDown MAKE_IMAGE_ID(ImageMgrHeaderDown)
#define ImageMgrHeaderUp   MAKE_IMAGE_ID(ImageMgrHeaderUp)

#if defined(__WINDOWS__)
#define LOAD_BITMAP_PNG(pWindow, name, outBmp) ImageHelper::LoadLocalBitmap(pWindow, L#name, outBmp)
#define LOAD_BUNDLE_PNG(pWindow, name, outIcon) \
	{ \
		wxBitmap bmp; \
		ImageHelper::LoadLocalBitmap(pWindow, L#name, bmp); \
		outIcon.AddIcon(ImageHelper::CreateIconFromBitmap(bmp)); \
	}

#else
#if !defined(__WXOSX__)
// OSX auto-loads @2 images.
#pragma PRAGMA_FIXME(This is not likely to load the correct image);
#endif
#define LOAD_BITMAP_PNG(pWindow, name, outBmp) outBmp = wxBITMAP_PNG(name)
#define LOAD_BUNDLE_PNG(pWindow, name, outIcon) \
	{ \
		wxBitmap bmp = wxBITMAP_PNG(name); \
		outIcon.AddIcon(ImageHelper::CreateIconFromBitmap(bmp)); \
	}
#endif
