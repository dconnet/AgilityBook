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

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace ImageHelper
{

wxBitmap GetBitmap(wxWindow* pWindow, const wxArtID& id, const wxArtClient& client, const wxSize& size)
{
	wxSize sz(size);
	if (sz == wxDefaultSize)
	{
		sz = wxArtProvider::GetNativeSizeHint(client);
	}
	return wxArtProvider::GetBitmap(id, client, sz);
}


wxIcon GetIcon(wxWindow* pWindow, const wxArtID& id, const wxArtClient& client, const wxSize& size)
{
	wxSize sz(size);
	if (sz == wxDefaultSize)
	{
		sz = wxArtProvider::GetNativeSizeHint(client);
	}
	return wxArtProvider::GetIcon(id, client, sz);
}


wxIcon CreateIconFromBitmap(const wxBitmap& bitmap)
{
	wxIcon icon;
	icon.CopyFromBitmap(bitmap);
	return icon;
}

} // namespace ImageHelper
