/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Data-driven helper to create menus and toolbar items.
 * @author David Connet
 *
 * Revision History
 * 2019-03-05 Moved out of MenuHelper.cpp
 */

#include "stdafx.h"
#include "LibARBWin/MenuHelper.h"

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


CMenuHelper::CDlgConfigAccel::CDlgConfigAccel(
		wxWindow* pParent,
		CMenuHelper::ItemData const menuItems[],
		size_t numMenuItems)
{
	Create(pParent, wxID_ANY, _("IDD_DLGCONFIGACCEL"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	// Controls (these are done first to control tab order)

	// Load data

	// Sizers

	//...
	//SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();
}


bool CMenuHelper::CDlgConfigAccel::GetAccelData(std::vector<AccelData>& accelData)
{
	//if (my accel data is the same as accelData)
		return false;
	//accelData = myData;
	//return true;
}
