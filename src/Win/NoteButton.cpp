/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Special button for invoking notes.
 * @author David Connet
 *
 * Revision History
 * 2022-04-15 Use wx DPI support.
 * 2015-01-01 Changed pixels to dialog units.
 * 2009-02-11 Ported to wxWidgets.
 * 2005-12-12 Created
 */

#include "stdafx.h"
#include "NoteButton.h"

#include "ImageHelper.h"

#include "LibARBWin/ResourceManager.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARBWin;

CNoteButton::CNoteButton(wxWindow* parent)
	: wxBitmapButton()
{
	wxBitmap image = CResourceManager::Get()->GetBitmap(ImageMgrNoteButton);
	wxSize size = image.GetSize();
	size += parent->FromDIP(wxSize(4, 4));
	wxBitmapButton::Create(parent, wxID_ANY, image, wxDefaultPosition, size);
}

} // namespace dconSoft
