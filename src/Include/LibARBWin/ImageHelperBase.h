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
 * 2018-10-11 Moved to Win LibARBWin
 * 2015-08-22 Expose LOAD_BITMAP_PNG for use in OnCreateBitmap.
 * 2013-10-13 Changed ImageManager to an art provider.
 * 2012-12-29 Created.
 */

#include "ImageManager.h"

// Helper functions called from IImageManagerCallback interfaces

namespace ImageHelper
{
	extern wxSize GetScaledSize(wxWindow* pWindow, int logical);
	extern wxSize GetScaledSize(wxWindow* pWindow, wxSize const& szLogical);

	extern wxBitmap GetBitmap(
			wxWindow* pWindow,
			const wxArtID& id,
			const wxArtClient& client = wxART_OTHER,
			const wxSize& size = wxDefaultSize);

	extern wxIcon GetIcon(
			wxWindow* pWindow,
			const wxArtID& id,
			const wxArtClient& client = wxART_OTHER,
			const wxSize& size = wxDefaultSize);

	extern wxIcon CreateIconFromBitmap(const wxBitmap& bitmap);

	// These are in each user of this.
	// That's where the mapping of names happens via IImageManagerCallback
	//extern bool DoCreateBitmap(
	//		wxWindow* pWindow,
	//		const wxArtID& id,
	//		const wxArtClient& client,
	//		const wxSize& size,
	//		wxBitmap& outBmp);
	//extern  bool DoCreateIconBundle(
	//		wxWindow* pWindow,
	//		const wxArtID& id,
	//		const wxArtClient& client,
	//		wxIconBundle& outIcon);

#if defined(__WINDOWS__)
	extern void LoadLocalBitmap(
			wxWindow* pWindow,
			wchar_t const* const pImageName,
			wxBitmap& outBmp);
#endif
};

// Setup image ids for things used in LibARBWin.
// Note the user app must provide the loading of these in the Do* functions.

#define ImageMgrBlank				wxART_MAKE_ART_ID(ImageMgrBlank)
#define ImageMgrChecked				wxART_MAKE_ART_ID(ImageMgrChecked)
#define ImageMgrUnChecked			wxART_MAKE_ART_ID(ImageMgrUnChecked)
#define ImageMgrCheck				wxART_MAKE_ART_ID(ImageMgrCheck)
#define ImageMgrQuestion			wxART_MAKE_ART_ID(ImageMgrQuestion)

#define ImageMgrHeaderDown			wxART_MAKE_ART_ID(ImageMgrHeaderDown)
#define ImageMgrHeaderUp			wxART_MAKE_ART_ID(ImageMgrHeaderUp)

#if defined(__WINDOWS__)
#define LOAD_BITMAP_PNG(pWindow, name, outBmp)	ImageHelper::LoadLocalBitmap(pWindow, L#name, outBmp)
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
#define LOAD_BITMAP_PNG(pWindow, name, outBmp)	outBmp = wxBITMAP_PNG(name)
#define LOAD_BUNDLE_PNG(pWindow, name, outIcon) \
	{ \
		wxBitmap bmp = wxBITMAP_PNG(name); \
		outIcon.AddIcon(ImageHelper::CreateIconFromBitmap(bmp)); \
	}
#endif
