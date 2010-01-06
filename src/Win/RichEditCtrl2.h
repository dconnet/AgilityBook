#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief Autodetect URLs and open a window when clicked.
 * @author David Connet
 *
 * Revision History
 * @li 2009
 * @li 2009-02-19 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-01-30 DRC Created
 */

#include "Widgets.h"
class wxTextUrlEvent;


class CRichEditCtrl2 : public CTextCtrl
{
public:
	CRichEditCtrl2(
			wxWindow* parent,
			wxWindowID id,
			const wxString& value = wxEmptyString,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			bool bReadOnly = true); // Initial state of readonly

	virtual void SetEditable(bool editable);

protected:
	void OnUrl(wxTextUrlEvent& evt);
};
