#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Resource Manager
 * @author David Connet
 *
 * For consistency across platforms, put all resource in the DAT file and
 * manage from here. This makes testing and distribution much easier.
 *
 * Revision History
 * 2021-12-18 Created.
 */

#include "LibwxARBWin.h"

#include <wx/artprov.h>

class CLibArchive;


class ARBWIN_API IResourceManagerCallback
{
public:
	virtual ~IResourceManagerCallback() = default;

	// Window to use for scaling, typically wxGetApp().GetTopWindow()
	virtual wxWindow* GetResourceWindow() = 0;
	// Get the name of the archived file.
	// outName is the basename, without ".svg", ".png", or "@2x.png"
	// outCall: Call GetResImage to get image (used for combining images into 1)
	virtual bool GetResImageName(
		wxArtID const& id,
		wxArtClient const& client,
		std::wstring& outName,
		bool& outSvg,
		bool& outCall) const = 0;
	virtual wxBitmap GetResImage(wxArtID const& id, wxArtClient const& client) const = 0;
};


class ARBWIN_API CResourceManager : public wxArtProvider
{
	CResourceManager();
	DECLARE_NO_COPY_IMPLEMENTED(CResourceManager)
public:
	static CResourceManager* Get();

	/** Called during wxApp::OnInit
	 * @param pCallback Ask the main app for resource names
	 * @param archiveName Default is the <exename>.dat. Allow for override.
	 */
	void Initialize(IResourceManagerCallback* pCallback, std::wstring const* archiveName = nullptr);
	// Called during wxApp::OnExit
	void Cleanup();

	bool LoadFile(std::wstring const& archiveFile, std::ostream& outData);

	wxBitmap CreateBitmap(wxArtID const& id, wxArtClient const& client, wxSize const& size) override;
	wxIconBundle CreateIconBundle(wxArtID const& id, wxArtClient const& client) override;

private:
	IResourceManagerCallback* m_callback;
	CLibArchive* m_archive;
};
