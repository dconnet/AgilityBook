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

#include "res/Note.xpm"


CNoteButton::CNoteButton(wxWindow* parent)
	: wxBitmapButton()
	, m_ImageList(16,16)
	, m_idxNormal(-1)
{
	wxBitmapButton::Create(parent, wxID_ANY, wxBitmap(Note_xpm), wxDefaultPosition, wxSize(20, 20));
}
