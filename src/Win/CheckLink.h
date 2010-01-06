#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
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

#include "ARBTypes.h"

extern bool CheckLink(
		wxString const& inLink,
		wxWindow* parent = NULL);
