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
 * Revision History
 * 2018-10-11 Moved to Win LibARBWin
 * 2017-12-01 Tweak scaling to avoid roundoff.
 * 2015-08-22 Expose LOAD_BITMAP_PNG for use in OnCreateBitmap.
 * 2013-10-13 Changed ImageManager to an art provider.
 * 2012-12-29 Created (separated from IconList).
 */

#include "stdafx.h"
#include "LibARBWin/ImageHelperBase.h"

#include "LibARBWin/DPI.h"
#include <wx/artprov.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace ImageHelper
{

wxSize GetScaledSize(wxWindow const* pWindow, int logical)
{
	return GetScaledSize(pWindow, wxSize(logical, logical));
}


wxSize GetScaledSize(wxWindow const* pWindow, wxSize const& szLogical)
{
	wxSize sz(szLogical);
#if defined(__WINDOWS__) && !wxCHECK_VERSION(3, 1, 2)
	unsigned int scale = DPI::GetScale(pWindow);
	if (scale > 100)
	{
		sz.x = sz.x * scale / 100;
		sz.y = sz.y * scale / 100;
	}
#endif
	return sz;
}


wxBitmap GetBitmap(
		wxWindow* pWindow,
		const wxArtID& id,
		const wxArtClient& client,
		const wxSize& size)
{
	wxSize sz(size);
	if (sz == wxDefaultSize)
	{
		sz = wxArtProvider::GetNativeSizeHint(client);
		if (sz != wxDefaultSize)
			sz = GetScaledSize(pWindow, sz);
	}
	return wxArtProvider::GetBitmap(id, client, sz);
}


wxIcon GetIcon(
		wxWindow* pWindow,
		const wxArtID& id,
		const wxArtClient& client,
		const wxSize& size)
{
	wxSize sz(size);
	if (sz == wxDefaultSize)
	{
		sz = wxArtProvider::GetNativeSizeHint(client);
		if (sz != wxDefaultSize)
			sz = GetScaledSize(pWindow, sz);
	}
	return wxArtProvider::GetIcon(id, client, sz);
}


wxIcon CreateIconFromBitmap(const wxBitmap& bitmap)
{
	wxIcon icon;
	icon.CopyFromBitmap(bitmap);
	return icon;
}


#if defined(__WINDOWS__)
void LoadLocalBitmap(
		wxWindow const* pWindow,
		wchar_t const* const pImageName,
		wxBitmap& outBmp)
{
	wxLogNull suppress;
	unsigned int scale = DPI::GetScale(pWindow);
	unsigned int rescale = 1;
	if (scale > 100)
	{
		wxString s(pImageName);
		s += L"_2";
		outBmp = wxBitmap(s, wxBITMAP_TYPE_PNG_RESOURCE);
		if (outBmp.IsOk())
			rescale = 2;
		else
			outBmp = wxBitmap(pImageName, wxBITMAP_TYPE_PNG_RESOURCE);
	}
	else
		outBmp = wxBitmap(pImageName, wxBITMAP_TYPE_PNG_RESOURCE);

	if (100 * rescale != scale && outBmp.IsOk())
	{
		wxImage image = outBmp.ConvertToImage();
		int x = (outBmp.GetWidth() * scale / 100) / rescale;
		int y = (outBmp.GetHeight() * scale / 100) / rescale;
		image.Rescale(x, y);
		outBmp = wxBitmap(image);
	}
}
#endif

};
