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
 * 2013-10-13 Changed ImageManager to an art provider.
 * 2012-12-29 Created (separated from IconList).
 */

#include "stdafx.h"
#include "ImageManager.h"

#include <wx/artprov.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CImageManager* CImageManager::Get()
{
	static CImageManager* mgr = nullptr;
	if (!mgr)
	{
		// Note, this is cleanuped in wxWidget code for wxArtProviders
		mgr = new CImageManager();
		wxArtProvider::Push(mgr);
	}
	return mgr;
}


CImageManager::CImageManager()
	: m_Callback(nullptr)
{
}


CImageManager::~CImageManager()
{
}


wxBitmap CImageManager::CreateBitmap(
		const wxArtID& id,
		const wxArtClient& client,
		const wxSize& size)
{
	if (m_Callback)
	{
		wxBitmap bmp;
		if (m_Callback->OnCreateBitmap(id, client, size, bmp))
			return bmp;
	}
	return wxArtProvider::CreateBitmap(id, client, size);
}


wxIconBundle CImageManager::CreateIconBundle(
		const wxArtID& id,
		const wxArtClient& client)
{
	if (m_Callback)
	{
		wxIconBundle icons;
		if (m_Callback->OnCreateIconBundle(id, client, icons))
			return icons;
	}
	return wxArtProvider::CreateIconBundle(id, client);
}
