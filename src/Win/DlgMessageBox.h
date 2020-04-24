#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Message box with a details button.
 * @author David Connet
 *
 * Revision History
 * 2009-02-25 Ported to wxWidgets.
 * 2004-09-11 Created
 */

/**
 * Callback to support the Details button.
 */
class IMessageBoxCallback
{
public:
	virtual void OnDetails(wxWindow* pParent) = 0;
};


extern int AfxMessageBox2(
	wxString const& inText,
	long inFlags = wxOK, //wxOK, wxCANCEL, wxYES, wxNO, wxHELP, wxNO_DEFAULT
	IMessageBoxCallback* inCallback = nullptr,
	wxWindow* inParent = nullptr);
