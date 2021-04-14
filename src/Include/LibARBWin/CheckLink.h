#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Check if a file exists, or the link is valid.
 * @author David Connet
 *
 * Revision History
 * 2018-10-11 Moved to Win LibARBWin
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-20 Ported to wxWidgets.
 * 2005-09-20 Created
 */

#include "LibwxARBWin.h"


ARBWIN_API bool CheckLink(std::wstring const& inLink, wxWindow* parent = nullptr);
