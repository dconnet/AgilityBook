#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Pragma messages to output
 * @author David Connet
 *
 * Include this in a compiled cpp. We could use stdafx.cpp, but that isn't
 * included in the Mac makefiles. So put it in the "main" file in each project.
 * Note: This is not in the main Setup*.h includes since the Mac-makefiles
 * do not use precompiled headers. Putting it there causes the pragma message
 * to output on every file - yuck!
 *
 * Must be included after stdafx.h (or SetupARBPre.h).
 *
 * Revision History
 * 2014-09-17 Created
 */

#ifdef USE_WXWIDGETS
#include <wx/version.h>
#pragma message("Compiling with wxWidgets " wxVERSION_NUM_DOT_STRING)
#endif


#ifdef USE_POCO
#include "Poco/Version.h"
#pragma message("Compiling with POCO " STRING(POCO_VERSION))
#endif
