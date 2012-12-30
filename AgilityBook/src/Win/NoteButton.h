#pragma once

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
#include <wx/imaglist.h>

class CNoteButton : public wxBitmapButton
{
public:
	CNoteButton(wxWindow* parent);
};
