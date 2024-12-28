/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test misc functions
 * @author David Connet
 *
 * Revision History
 * 2022-05-05 Removed localized language names since WX now supplies them.
 * 2019-10-13 Move config test to TestARB
 * 2019-08-15 Fix tests on unix.
 * 2019-06-28 Moved ConfigPath to TestUtils
 * 2018-04-26 Added roman numeral tests.
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2015-11-01 Added ARBConfig/ARBBook version test.
 * 2015-04-04 Add C99 printf test.
 * 2014-11-17 Added "true" units (nonIEC) to FormatBytes.
 * 2014-08-28 Enhanced FormatBytes
 * 2012-04-03 Added ARBVersion test.
 * 2011-11-17 Add localization test.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-12 Created
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARBCommon/ARBMisc.h"
#include "ARBCommon/ARBTypes.h"
#include "LibARBWin/CheckLink.h"
#include <wx/uilocale.h>
#include <stdarg.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARBCommon;

TEST_CASE("Misc")
{
	SECTION("Localization")
	{
		if (!g_bMicroTest)
		{
#if defined(__WXWINDOWS__)
			wxString s1(L"IDS_ARB_UNKNOWN_VERSION");
			wxString s2(wxGetTranslation(arbT("IDS_ARB_UNKNOWN_VERSION")));
			REQUIRE(s1 != s2);
#else
#pragma PRAGMA_TODO(implement non - wx version)
#endif
		}
	}
}

} // namespace dconSoft
