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
 * @li 2012-04-03 DRC Added ARBVersion test.
 * @li 2011-11-17 DRC Add localization test.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-12 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBCommon/ARBTypes.h"
#include "ARBCommon/StringUtil.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

#define MAX_LANGS	2


SUITE(TestMisc)
{
	TEST(Html_Sanitize)
	{
		if (!g_bMicroTest)
		{
			std::wstring s(L"<&amp>");
			std::wstring s2 = SanitizeStringForHTML(s);
			CHECK(L"&lt;&amp;amp&gt;" == s2);
			s = L"1\r\n2\n3";
			s2 = SanitizeStringForHTML(s, true);
			CHECK(L"1<br/>2<br/>3" == s2);
			s2 = SanitizeStringForHTML(s, false);
			CHECK(L"1\r\n2\n3" == s2);
		}
	}


	TEST(Version)
	{
		if (!g_bMicroTest)
		{
			ARBVersion ver(0xf3f3, 0xff44);
			CHECK(ver.Major() == 0xf3f3);
			CHECK(ver.Minor() == 0xff44);
		}
	}


	TEST(FormatBytes)
	{
		CHECK(StringUtil::FormatBytes(1000.0) == L"1000 bytes");
		CHECK(StringUtil::FormatBytes(1024.0) == L"1 KiB");
		CHECK(StringUtil::FormatBytes(1536.0) == L"1.5 KiB");
		CHECK(StringUtil::FormatBytes(1600.0) == L"1.56 KiB");
#ifdef __WXMAC__
		// Note: On Mac, 15.625 is rounding differently than Windows.
		CHECK(StringUtil::FormatBytes(16000.0) == L"15.62 KiB");
#else
		CHECK(StringUtil::FormatBytes(16000.0) == L"15.63 KiB");
#endif
	}


	TEST(Localization)
	{
		if (!g_bMicroTest)
		{
			std::wstring s1(L"IDS_ARB_UNKNOWN_VERSION");
			std::wstring s2(StringUtil::GetTranslation(arbT("IDS_ARB_UNKNOWN_VERSION")));
			CHECK(s1 != s2);
		}
	}


#if defined(__WXWINDOWS__)
	TEST(LocalizationDesc)
	{
		if (!g_bMicroTest)
		{
			// Update as more languages are added.
			static struct {
				wxChar const* lang;
				int langid;
			} langs[MAX_LANGS] = {
				{L"en_US", wxLANGUAGE_FRENCH},
				{L"fr_FR", wxLANGUAGE_ENGLISH_US}
			};
			struct
			{
				std::wstring Desc[MAX_LANGS];
				std::wstring ArbDesc[MAX_LANGS];
			} langdata[MAX_LANGS];
			std::wostringstream out;
			for (size_t i = 0; i < MAX_LANGS; ++i)
			{
				SetLang(langs[i].langid);
				for (size_t j = 0; j < MAX_LANGS; ++j)
				{
					wxLanguageInfo const* info = wxLocale::FindLanguageInfo(langs[j].lang);
					CHECK(info);
					if (info)
					{
						langdata[i].Desc[j] = info->Description;
						langdata[i].ArbDesc[j] = wxGetTranslation(info->Description);
						out << i << L" " << j << L" "
							<< langdata[i].Desc[j] << L" " << langdata[i].ArbDesc[j] << L"\n";
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
					CHECK(langdata[0].Desc[i] == langdata[j].Desc[i]);
					CHECK(langdata[0].ArbDesc[i] == langdata[j].ArbDesc[i]);
				}
			}
			// Check that our value is different than wx (this test may need
			// to change in the future, depending on things like changing
			// "English" back to "English (U.S.)"
			for (size_t i = 0; i < MAX_LANGS; ++i)
			{
				for (size_t j = 0; j < MAX_LANGS; ++j)
				{
					CHECK(langdata[i].Desc[j] != langdata[i].ArbDesc[j]);
				}
			}
		}
	}
#else
#pragma PRAGMA_TODO(Write localization test)
#endif


#ifdef __WXMSW__
	TEST(VerifyColor)
	{
		if (!g_bMicroTest)
		{
			CHECK(wxSYS_COLOUR_SCROLLBAR == COLOR_SCROLLBAR);
			CHECK(wxSYS_COLOUR_DESKTOP == COLOR_BACKGROUND);
			CHECK(wxSYS_COLOUR_ACTIVECAPTION == COLOR_ACTIVECAPTION);
			CHECK(wxSYS_COLOUR_INACTIVECAPTION == COLOR_INACTIVECAPTION);
			CHECK(wxSYS_COLOUR_MENU == COLOR_MENU);
			CHECK(wxSYS_COLOUR_WINDOW == COLOR_WINDOW);
			CHECK(wxSYS_COLOUR_WINDOWFRAME == COLOR_WINDOWFRAME);
			CHECK(wxSYS_COLOUR_MENUTEXT == COLOR_MENUTEXT);
			CHECK(wxSYS_COLOUR_WINDOWTEXT == COLOR_WINDOWTEXT);
			CHECK(wxSYS_COLOUR_CAPTIONTEXT == COLOR_CAPTIONTEXT);
			CHECK(wxSYS_COLOUR_ACTIVEBORDER == COLOR_ACTIVEBORDER);
			CHECK(wxSYS_COLOUR_INACTIVEBORDER == COLOR_INACTIVEBORDER);
			CHECK(wxSYS_COLOUR_APPWORKSPACE == COLOR_APPWORKSPACE);
			CHECK(wxSYS_COLOUR_HIGHLIGHT == COLOR_HIGHLIGHT);
			CHECK(wxSYS_COLOUR_HIGHLIGHTTEXT == COLOR_HIGHLIGHTTEXT);
			CHECK(wxSYS_COLOUR_BTNFACE == COLOR_BTNFACE);
			CHECK(wxSYS_COLOUR_BTNSHADOW == COLOR_BTNSHADOW);
			CHECK(wxSYS_COLOUR_GRAYTEXT == COLOR_GRAYTEXT);
			CHECK(wxSYS_COLOUR_BTNTEXT == COLOR_BTNTEXT);
			CHECK(wxSYS_COLOUR_INACTIVECAPTIONTEXT == COLOR_INACTIVECAPTIONTEXT);
			CHECK(wxSYS_COLOUR_BTNHIGHLIGHT == COLOR_BTNHIGHLIGHT);
			CHECK(wxSYS_COLOUR_3DDKSHADOW == COLOR_3DDKSHADOW);
			CHECK(wxSYS_COLOUR_3DLIGHT == COLOR_3DLIGHT);
			CHECK(wxSYS_COLOUR_INFOTEXT == COLOR_INFOTEXT);
			CHECK(wxSYS_COLOUR_INFOBK == COLOR_INFOBK);
			//CHECK(wxSYS_COLOUR_LISTBOX == );
			CHECK(wxSYS_COLOUR_HOTLIGHT == COLOR_HOTLIGHT);
			CHECK(wxSYS_COLOUR_GRADIENTACTIVECAPTION == COLOR_GRADIENTACTIVECAPTION);
			CHECK(wxSYS_COLOUR_GRADIENTINACTIVECAPTION == COLOR_GRADIENTINACTIVECAPTION);
			//CHECK(wxSYS_COLOUR_MENUHILIGHT == );
			//CHECK(wxSYS_COLOUR_MENUBAR == );
			//CHECK(wxSYS_COLOUR_LISTBOXTEXT == );
		}
	}
#endif
}
