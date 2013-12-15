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
 * 2009-02-11 Ported to wxWidgets.
 * 2005-12-12 Created
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
	wxBitmapButton::Create(parent, wxID_ANY, CImageManager::Get()->NoteButton(), wxDefaultPosition, wxSize(20, 20));
}
