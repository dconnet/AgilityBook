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
 * 2019-10-13 Move config test to TestARB
 * 2019-08-15 Fix tests on unix.
 * 2019-06-28 Moved ConfigPath to TestUtils
 * 2019-01-06 fmt 5.3 cannot sprintf a wide string into a narrow format anymore
 * 2018-12-16 Convert to fmt.
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
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/CheckLink.h"
#include "fmt/printf.h"
#include "fmt/xchar.h"

#include <stdarg.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

#define MAX_LANGS 2


TEST_CASE("Misc")
{
	SECTION("Localization")
	{
		if (!g_bMicroTest)
		{
#if defined(__WXWINDOWS__)
			std::wstring s1(L"IDS_ARB_UNKNOWN_VERSION");
			std::wstring s2(StringUtil::GetTranslation(arbT("IDS_ARB_UNKNOWN_VERSION")));
			REQUIRE(s1 != s2);
#else
#pragma PRAGMA_TODO(implement non - wx version)
#endif
		}
	}


#if defined(__WXWINDOWS__)
	SECTION("LocalizationDesc")
	{
		if (!g_bMicroTest)
		{
			// Update as more languages are added.
			static struct
			{
				wxChar const* lang;
				wxLanguage langid;
			} langs[MAX_LANGS] = {
				// clang-format off
				{L"en_US", wxLANGUAGE_FRENCH},
				{L"fr_FR", wxLANGUAGE_ENGLISH_US}
				// clang-format on
			};
			struct
			{
				std::wstring Desc[MAX_LANGS];
				std::wstring ArbDesc[MAX_LANGS];
			} langdata[MAX_LANGS];
			fmt::wmemory_buffer out;
			for (size_t i = 0; i < MAX_LANGS; ++i)
			{
				SetLang(langs[i].langid);
				for (size_t j = 0; j < MAX_LANGS; ++j)
				{
					wxLanguageInfo const* info = wxLocale::FindLanguageInfo(langs[j].lang);
					REQUIRE(info);
					if (info)
					{
						langdata[i].Desc[j] = info->Description;
						langdata[i].ArbDesc[j] = wxGetTranslation(info->Description);
						fmt::format_to(
							std::back_inserter(out),
							L"{} {} {} {}\n",
							i,
							j,
							langdata[i].Desc[j],
							langdata[i].ArbDesc[j]);
					}
				}
			}
			SetLang(wxLANGUAGE_ENGLISH_US); // Reset
			// Sanity check that wx is still returning the same (english) value
			// Check that we have the same value for a language in all languages
			for (size_t i = 0; i < MAX_LANGS; ++i)
			{
				for (size_t j = 1; j < MAX_LANGS; ++j)
				{
					REQUIRE(langdata[0].Desc[i] == langdata[j].Desc[i]);
					REQUIRE(langdata[0].ArbDesc[i] == langdata[j].ArbDesc[i]);
				}
			}
			// Check that our value is different than wx (this test may need
			// to change in the future, depending on things like changing
			// "English" back to "English (U.S.)"
			for (size_t i = 0; i < MAX_LANGS; ++i)
			{
				for (size_t j = 0; j < MAX_LANGS; ++j)
				{
					// A failure here means the string in wx changed.
					// Edit LibARBWin/LanguageManager.cpp, around line 183.
					// Add the new strings so poedit can capture them.
					REQUIRE(langdata[i].Desc[j] != langdata[i].ArbDesc[j]);
				}
			}
		}
	}
#else
#pragma PRAGMA_TODO(Write localization test)
#endif
}
