#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Use CInternetSession to access files.
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-20 Ported to wxWidgets.
 * 2005-09-20 Created
 */

#include "ARB/ARBTypes2.h"


extern bool CheckLink(
		std::wstring const& inLink,
		wxWindow* parent = NULL);
