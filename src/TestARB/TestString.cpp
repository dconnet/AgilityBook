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

#include "ARBString.h"

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
			/*
			std::wstring s2 = StringUtil::tstringW(s);
			CHECK(L"narrow" == s2);
			*/
			std::wstring s2 = StringUtil::stringW(s);
			CHECK(L"narrow" == s2);
		}
	}


	TEST(Convert_ToNarrow)
	{
		if (!g_bMicroTest)
		{
			std::wstring s(L"wide");
			/*
			std::string s2 = StringUtil::stringA(s);
			CHECK("wide" == s2);
			*/
			wxString s2 = StringUtil::stringWX(s);
			CHECK(L"wide" == s2);
		}
	}


	// We know this will fail using wcstombs.
	TEST(Convert_Multi)
	{
		if (!g_bMicroTest)
		{
			wchar_t w = 0xFEFB; // In courier new, Arabic Ligature Lam With Alef Isolated Form (see 'Character Map' program)
			std::wstring s(1, w);
			//std::string s2 = StringUtil::stringA(s);
			std::string s2 = StringUtil::stringA(s);
			CHECK(s.length() == 1);
			CHECK(s2.length() == 3);
		}
	}


	TEST(Atol)
	{
		if (!g_bMicroTest)
		{
			std::wstring s1(L"123");
			long a1 = StringUtil::ToCLong(s1);
			CHECK(a1 == 123);
			std::wstring s2(L"12-3");
			long a2 = StringUtil::ToCLong(s2);
			CHECK(a2 == 12);
			CHECK(!StringUtil::ToCLong(s2, a2));
			CHECK(a2 == 12);
			CHECK(!StringUtil::ToCLong(s2, a2, true));
			CHECK(a2 == 12);
		}
	}


	TEST(Atod)
	{
		if (!g_bMicroTest)
		{
			std::wstring s1(L"12.3");
			double a1 = StringUtil::ToCDouble(s1);
			CHECK(a1 == 12.3);
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
			/*
			std::wstring one(L"one");
			std::wstring two(L"two");
			std::wstring fmt = std::wstring::Format(L"%2$s %1$s", one.c_str(), two.c_str());
			CHECK(fmt == L"two one");
			*/
			CHECK(L"two one" == wxString::Format(L"%2$s %1$s", L"one", L"two"));
		}
	}
}
