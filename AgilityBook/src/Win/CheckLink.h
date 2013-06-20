#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Use CInternetSession to access files.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-20 DRC Ported to wxWidgets.
 * @li 2005-09-20 DRC Created
 */

#include "ARB/ARBTypes2.h"


extern bool CheckLink(
		std::wstring const& inLink,
		wxWindow* parent = NULL);
