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
 * 2015-01-01 Changed pixels to dialog units.
 * 2009-02-11 Ported to wxWidgets.
 * 2005-12-12 Created
 */

#include "stdafx.h"
#include "NoteButton.h"

#include "DPI.h"
#include "ImageHelper.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CNoteButton::CNoteButton(wxWindow* parent)
	: wxBitmapButton()
{
	wxBitmap image = CImageManager::Get()->GetBitmap(ImageMgrNoteButton);
	wxSize size = image.GetSize();
	size.x += DPI::Scale(parent, 4);
	size.y += DPI::Scale(parent, 4);
	wxBitmapButton::Create(parent, wxID_ANY, image, wxDefaultPosition, size);
}
