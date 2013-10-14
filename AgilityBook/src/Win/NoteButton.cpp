/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Special button for invoking notes.
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2005-12-12 DRC Created
 */

#include "stdafx.h"
#include "NoteButton.h"

#include "ImageManager.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CNoteButton::CNoteButton(wxWindow* parent)
	: wxBitmapButton()
{
	wxBitmapButton::Create(parent, wxID_ANY, CImageManager::Get()->GetBitmap(ImageMgrNoteButton), wxDefaultPosition, wxSize(20, 20));
}
