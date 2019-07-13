/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Global functions
 * @author David Connet
 *
 * Revision History
 * 2012-10-06 Moved backup function from doc to here.
 * 2012-08-12 Moved FormatBytes to StringUtil
 * 2012-05-04 Added FormatBytes
 * 2011-04-16 Added GetFileTimes
 * 2010-10-30 Moved BreakLine to a separate file.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-14 Separated from AgilityBook.cpp
 */

#include "stdafx.h"
#include "Globals.h"

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


// 8/17/03: Only compute score on Q and NQ runs.
// 11/13/04: Also compute score for NA runs that have no titling pts.
// 11/1/15: Moved test to Globals, allow all NA runs. (AgilityBookViewRuns
//          never had the NA test added.)
bool ShouldComputeScore(ARB_Q q)
{
	return q.Qualified() || Q::NQ == q || Q::NA == q;
}
