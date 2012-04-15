#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgMessage class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-09 DRC Ported to wxWidgets.
 */

class CDlgMessage : public wxDialog
{
public:
	CDlgMessage(
			std::wstring const& msg,
			wxWindow* pParent = NULL,
			std::wstring caption = std::wstring());
};
