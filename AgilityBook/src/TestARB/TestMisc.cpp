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
 * 2015-04-04 Add C99 printf test.
 * 2014-11-17 Added "true" units (nonIEC) to FormatBytes.
 * 2014-08-28 Enhanced FormatBytes
 * 2012-04-03 Added ARBVersion test.
 * 2011-11-17 Add localization test.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-12 Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBCommon/ARBMisc.h"
#include "ARBCommon/ARBTypes.h"
#include "ARBCommon/StringUtil.h"

#include <stdarg.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

#define MAX_LANGS	2

#define NUM_PREC	2
static const struct
{
	double val;
	int prec[NUM_PREC];
	wchar_t const* unitSI[NUM_PREC];
	wchar_t const* unitBinary[NUM_PREC];
	wchar_t const* unitTrue[NUM_PREC];
} sc_FormatUnits[] = {
	{0.0,             {1, 2}, {L"0 B", L"0 B"},            {L"0 B", L"0 B"},            {L"0 B", L"0 B"}},
	{27.0,            {1, 2}, {L"27 B", L"27 B"},          {L"27 B", L"27 B"},          {L"27 B", L"27 B"}},
	{999.0,           {1, 2}, {L"999 B", L"999 B"},        {L"999 B", L"999 B"},        {L"999 B", L"999 B"}},
	{1000.0,          {1, 2}, {L"1 kB", L"1 kB"},          {L"1000 B", L"1000 B"},      {L"1000 B", L"1000 B"}},
	{1023.0,          {1, 2}, {L"1 kB", L"1.02 kB"},       {L"1023 B", L"1023 B"},      {L"1023 B", L"1023 B"}},
	{1024.0,          {1, 2}, {L"1 kB", L"1.02 kB"},       {L"1 KiB", L"1 KiB"},        {L"1 KB", L"1 KB"}},
	{1536.0,          {1, 2}, {L"1.5 kB", L"1.54 kB"},     {L"1.5 KiB", L"1.5 KiB"},    {L"1.5 KB", L"1.5 KB"}},
	{1600.0,          {1, 2}, {L"1.6 kB", L"1.6 kB"},      {L"1.6 KiB", L"1.56 KiB"},   {L"1.6 KB", L"1.56 KB"}},
	{1728.0,          {1, 2}, {L"1.7 kB", L"1.73 kB"},     {L"1.7 KiB", L"1.69 KiB"},   {L"1.7 KB", L"1.69 KB"}},
#ifdef __WXMAC__
	// Note: On Mac, 15.625 is rounding differently than Windows.
	{16000.0,         {1, 2}, {L"16 kB", L"16 kB"},        {L"15.6 KiB", L"15.62 KiB"}, {L"15.6 KB", L"15.62 KB"}},
#else
	{16000.0,         {1, 2}, {L"16 kB", L"16 kB"},        {L"15.6 KiB", L"15.63 KiB"}, {L"15.6 KB", L"15.63 KB"}},
#endif
	{110592.0,        {1, 2}, {L"110.6 kB", L"110.59 kB"}, {L"108 KiB", L"108 KiB"},    {L"108 KB", L"108 KB"}},
	{7077888.0,       {1, 2}, {L"7.1 MB", L"7.08 MB"},     {L"6.8 MiB", L"6.75 MiB"},   {L"6.8 MB", L"6.75 MB"}},
	{452984832.0,     {1, 2}, {L"453 MB", L"452.98 MB"},   {L"432 MiB", L"432 MiB"},    {L"432 MB", L"432 MB"}},
	{28991029248.0,	  {1, 2}, {L"29 GB", L"28.99 GB"},     {L"27 GiB", L"27 GiB"},      {L"27 GB", L"27 GB"}},
	{1855425871872.0, {1, 2}, {L"1.9 TB", L"1.86 TB"},     {L"1.7 TiB", L"1.69 TiB"},   {L"1.7 TB", L"1.69 TB"}}
};


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
		if (!g_bMicroTest)
		{
			for (size_t i = 0; i < ARRAY_SIZE(sc_FormatUnits); ++i)
			{
				for (size_t iPrec = 0; iPrec < NUM_PREC; ++iPrec)
				{
					CHECK(StringUtil::FormatBytes(sc_FormatUnits[i].val, sc_FormatUnits[i].prec[iPrec], StringUtil::eBytesBinary) == sc_FormatUnits[i].unitBinary[iPrec]);
					CHECK(StringUtil::FormatBytes(sc_FormatUnits[i].val, sc_FormatUnits[i].prec[iPrec], StringUtil::eBytesSI) == sc_FormatUnits[i].unitSI[iPrec]);
					CHECK(StringUtil::FormatBytes(sc_FormatUnits[i].val, sc_FormatUnits[i].prec[iPrec], StringUtil::eBytesTrue) == sc_FormatUnits[i].unitTrue[iPrec]);
				}
			}
		}
	}


	TEST(Localization)
	{
		if (!g_bMicroTest)
		{
#if defined(__WXWINDOWS__)
			std::wstring s1(L"IDS_ARB_UNKNOWN_VERSION");
			std::wstring s2(StringUtil::GetTranslation(arbT("IDS_ARB_UNKNOWN_VERSION")));
			CHECK(s1 != s2);
#else
#pragma PRAGMA_TODO(implement non-wx version)
#endif
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

	TEST(C99PrintfToSame)
	{
		if (!g_bMicroTest)
		{
			std::string str("str");
			std::wstring wstr(L"str");
			char buffer[100];
			wchar_t wbuffer[100];

#ifdef ARB_HAS_C99_PRINTF_SPECS
			sprintf(buffer, "%s", str.c_str());
			swprintf(wbuffer, 100, L"%ls", wstr.c_str());
#else
			sprintf(buffer, "%s", str.c_str());
			swprintf(wbuffer, 100, L"%s", wstr.c_str());
#endif
			CHECK(str == buffer);
			CHECK(wstr == wbuffer);
		}
	}

	TEST(C99PrintfToOpposite)
	{
		if (!g_bMicroTest)
		{
			std::string str("str");
			std::wstring wstr(L"str");
			char buffer[100];
			wchar_t wbuffer[100];

#ifdef ARB_HAS_C99_PRINTF_SPECS
			sprintf(buffer, "%ls", wstr.c_str());
			swprintf(wbuffer, 100, L"%s", str.c_str());
#else
			sprintf(buffer, "%S", wstr.c_str());
			swprintf(wbuffer, 100, L"%S", str.c_str());
#endif
			CHECK(str == buffer);
			CHECK(wstr == wbuffer);
		}
	}
}
