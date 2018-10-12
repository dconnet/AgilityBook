#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Globals
 * @author David Connet
 *
 * Revision History
 * 2015-11-01 Added ShouldComputeScore.
 * 2012-10-06 Moved backup function from doc to here.
 * 2012-08-12 Moved FormatBytes to StringUtil
 * 2012-05-04 Added FormatBytes
 * 2010-10-30 Moved BreakLine to a separate file.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-14 Moved from AgilityBook.h
 */

#include "ARB/ARB_Q.h"
#include "LibARBWin/ARBWinUtilities.h"


/**
 * Should a score be computed? (displayed in Runs dialog and main view)
 */
bool ShouldComputeScore(ARB_Q q);
