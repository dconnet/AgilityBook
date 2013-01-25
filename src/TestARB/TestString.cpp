/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test string util functions
 * @author David Connet
 *
 * Revision History
 * @li 2008-06-29 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "StringUtil.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


SUITE(TestString)
{
	TEST(Convert_ToWide)
	{
		if (!g_bMicroTest)
		{
			std::string s("narrow");
			std::wstring s2 = StringUtil::stringW(s);
			CHECK(L"narrow" == s2);
		}
	}


	TEST(Convert_ToNarrow)
	{
		if (!g_bMicroTest)
		{
			std::wstring s(L"wide");
			std::string s2 = StringUtil::stringA(s);
			CHECK("wide" == s2);
		}
	}


	// We know this will fail using wcstombs.
	TEST(Convert_Multi)
	{
		if (!g_bMicroTest)
		{
			// http://www.ftrain.com/unicode/#65275
			wchar_t w = 0xFEFB; // In courier new, Arabic Ligature Lam With Alef Isolated Form (see 'Character Map' program)
			std::wstring s(1, w);
			std::string s2 = StringUtil::stringA(s);
			CHECK(s.length() == 1);
			// MBCS: 0, UTF8: 3
			CHECK(s2.length() == 3);
		}
	}


	TEST(Convert_Multi2)
	{
		if (!g_bMicroTest)
		{
			// http://www.ftrain.com/unicode/#247
			wchar_t w = 0x00f7; // Division sign
			std::wstring s(1, w);
			std::string s2 = StringUtil::stringA(s);
			CHECK(s.length() == 1);
			// MBCS: 1, UTF8: 2
			CHECK(s2.length() > 0);
		}
	}


	TEST(AtolGoodData)
	{
		if (!g_bMicroTest)
		{
			std::wstring s1(L"123");
			long a1 = StringUtil::ToCLong(s1);
			CHECK(a1 == 123);
			CHECK(StringUtil::ToCLong(s1, a1));
		}
	}


	TEST(AtolBadData)
	{
		if (!g_bMicroTest)
		{
			std::wstring s2(L"12-3");
			long a2 = StringUtil::ToCLong(s2);
			CHECK(a2 == 12);
			CHECK(!StringUtil::ToCLong(s2, a2));
			CHECK(a2 == 12);
			CHECK(!StringUtil::ToCLong(s2, a2, true));
			CHECK(a2 == 12);
		}
	}


	TEST(AtodGoodData)
	{
		if (!g_bMicroTest)
		{
			std::wstring s1(L"12.3");
			double a1 = StringUtil::ToCDouble(s1);
			CHECK(a1 == 12.3);
			CHECK(StringUtil::ToCDouble(s1, a1));
		}
	}


	TEST(AtodBadData)
	{
		if (!g_bMicroTest)
		{
			std::wstring s2(L"1.3-12");
			double a2;
			CHECK(!StringUtil::ToCDouble(s2, a2));
			CHECK(a2 == 1.3);
		}
	}


	TEST(AtodUS)
	{
		if (!g_bMicroTest)
		{
			wxLocale locale(wxLANGUAGE_ENGLISH_US);
			std::wstring s1(L"12.3");
			double a1 = StringUtil::ToDouble(s1);
			CHECK(a1 == 12.3);
			std::wstring s2(L"1.3-12");
			double a2;
			bool bParsed = StringUtil::ToDouble(s2, a2);
			CHECK(a2 == 1.3);
			CHECK(!bParsed);
		}
	}


	TEST(AtodFR)
	{
		if (!g_bMicroTest)
		{
			wxLocale locale(wxLANGUAGE_FRENCH);
			std::wstring s1(L"12,3");
			double a1 = StringUtil::ToDouble(s1);
			CHECK(a1 == 12.3);
			std::wstring s2(L"1,3-12");
			double a2;
			bool bParsed = StringUtil::ToDouble(s2, a2);
			CHECK(a2 == 1.3);
			CHECK(!bParsed);
		}
	}


	TEST(AtodFR2)
	{
		if (!g_bMicroTest)
		{
			// Even in French, I want to have "." separators parse properly.
			wxLocale locale(wxLANGUAGE_FRENCH);
			std::wstring s1(L"12.3");
			double a1 = StringUtil::ToDouble(s1);
			CHECK(a1 == 12.3);
		}
	}


	TEST(ReplaceA)
	{
		if (!g_bMicroTest)
		{
			std::string s("This is a test");
			std::string s2 = StringUtil::Replace(s, "is a", "");
			CHECK("This  test" == s2);
			s2 = StringUtil::Replace(s2, " test", "good");
			CHECK("This good" == s2);
		}
	}


	TEST(ReplaceW)
	{
		if (!g_bMicroTest)
		{
			std::wstring s(L"This is a test");
			std::wstring s2 = StringUtil::Replace(s, L"is a", L"");
			CHECK(L"This  test" == s2);
			s2 = StringUtil::Replace(s2, L" test", L"good");
			CHECK(L"This good" == s2);
		}
	}


	TEST(Formatting)
	{
		if (!g_bMicroTest)
		{
			CHECK(L"two one" == wxString::Format(L"%2$s %1$s", L"one", L"two"));
		}
	}


	TEST(Trim)
	{
		std::wstring str(L"  xyx  ");
		CHECK(StringUtil::Trim(str) == L"xyx");
		CHECK(StringUtil::TrimLeft(str) == L"xyx  ");
		CHECK(StringUtil::TrimRight(str) == L"  xyx");
	}


	TEST(TrimChar)
	{
		std::wstring str(L"\"xyx\"");
		CHECK(StringUtil::Trim(str, '"') == L"xyx");
		CHECK(StringUtil::TrimLeft(str, '"') == L"xyx\"");
		CHECK(StringUtil::TrimRight(str, '"') == L"\"xyx");
	}

}
