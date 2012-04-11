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
			wxString s2 = StringUtil::stringWX(s);
			CHECK(wxT("narrow") == s2);
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
			CHECK(wxT("wide") == s2);
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
			wxString s2 = StringUtil::stringWX(s);
			std::string s3 = StringUtil::stringA(s2);
			CHECK(s.length() == 1);
			CHECK(s3.length() == 3);
		}
	}


	TEST(Atol)
	{
		if (!g_bMicroTest)
		{
			wxString s1(wxT("123"));
			long a1 = StringUtil::ToCLong(s1);
			CHECK(a1 == 123);
			wxString s2(wxT("12-3"));
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
			wxString s1(wxT("12.3"));
			double a1 = StringUtil::ToCDouble(s1);
			CHECK(a1 == 12.3);
			wxString s2(wxT("1.3-12"));
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
			wxString s1(wxT("12.3"));
			double a1 = StringUtil::ToDouble(s1);
			CHECK(a1 == 12.3);
			wxString s2(wxT("1.3-12"));
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
			wxString s1(wxT("12,3"));
			double a1 = StringUtil::ToDouble(s1);
			CHECK(a1 == 12.3);
			wxString s2(wxT("1,3-12"));
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
			wxString s1(wxT("12.3"));
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
			wxString one(wxT("one"));
			wxString two(wxT("two"));
			wxString fmt = wxString::Format(wxT("%2$s %1$s"), one.c_str(), two.c_str());
			CHECK(fmt == wxT("two one"));
			*/
			CHECK(wxT("two one") == wxString::Format(wxT("%2$s %1$s"), wxT("one"), wxT("two")));
		}
	}
}
