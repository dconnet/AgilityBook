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
 * 2014-11-28 Moved project specific images to project.
 * 2013-10-13 Changed ImageManager to an art provider.
 * 2012-12-29 Created.
 */

#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/icon.h>


// Allow projects to extend without deriving which makes using the same
// class unchanged in multiple projects possible.
class IImageManagerCallback
{
public:
	// Return code indicates image has handled.
	virtual bool OnCreateBitmap(
			const wxArtID& id,
			const wxArtClient& client,
			const wxSize& size,
			wxBitmap& outBmp) = 0;

	virtual bool OnCreateIconBundle(
			const wxArtID& id,
			const wxArtClient& client,
			wxIconBundle& outIcon) = 0;
};


class CImageManager : public wxArtProvider
{
private:
	CImageManager();
	~CImageManager();

public:
	static CImageManager* Get();
	void SetCallback(IImageManagerCallback* pCallback) {m_Callback = pCallback;}

	virtual wxBitmap CreateBitmap(
			const wxArtID& id,
			const wxArtClient& client,
			const wxSize& size);

	virtual wxIconBundle CreateIconBundle(
			const wxArtID& id,
			const wxArtClient& client);

private:
	IImageManagerCallback* m_Callback;
};
