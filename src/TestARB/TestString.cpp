/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
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


SUITE(TestString)
{
	TEST(Convert_ToWide)
	{
		std::string s("narrow");
		/*
		std::wstring s2 = tstringUtil::tstringW(s);
		CHECK(L"narrow" == s2);
		*/
		wxString s2 = tstringUtil::TString(s);
		CHECK(wxT("narrow") == s2);
	}


	TEST(Convert_ToNarrow)
	{
		std::wstring s(L"wide");
		/*
		std::string s2 = tstringUtil::tstringA(s);
		CHECK("wide" == s2);
		*/
		wxString s2 = tstringUtil::TString(s);
		CHECK(wxT("wide") == s2);
	}


	// We know this will fail using wcstombs.
	TEST(Convert_Multi)
	{
		wchar_t w = 0xFEFB; // In courier new, Arabic Ligature Lam With Alef Isolated Form (see 'Character Map' program)
		std::wstring s(1, w);
		//std::string s2 = tstringUtil::tstringA(s);
		wxString s2 = tstringUtil::TString(s);
		std::string s3 = tstringUtil::tstringA(s2);
		CHECK(s.length() == 1);
		CHECK(s3.length() == 3);
	}


	TEST(Atol)
	{
		wxString s1(wxT("123"));
		long a1 = tstringUtil::ToCLong(s1);
		CHECK(a1 == 123);
		wxString s2(wxT("12-3"));
		long a2 = tstringUtil::ToCLong(s2);
		CHECK(a2 == 12);
		CHECK(!tstringUtil::ToCLong(s2, a2));
		CHECK(a2 == 12);
		CHECK(!tstringUtil::ToCLong(s2, a2, true));
		CHECK(a2 == 12);
	}


	TEST(Atod)
	{
		wxString s1(wxT("12.3"));
		double a1 = tstringUtil::ToCDouble(s1);
		CHECK(a1 == 12.3);
		wxString s2(wxT("1.3-12"));
		double a2;
		CHECK(!tstringUtil::ToCDouble(s2, a2));
#if wxCHECK_VERSION(2, 9, 1)
		CHECK(a2 == 1.3);
#elif wxCHECK_VERSION(2, 9, 0)
		CHECK(a2 == 0.0);
#else
		CHECK(a2 == 1.3);
#endif
	}


	TEST(AtodUS)
	{
		wxLocale locale(wxLANGUAGE_ENGLISH_US);
		wxString s1(wxT("12.3"));
		double a1 = tstringUtil::ToDouble(s1);
		CHECK(a1 == 12.3);
		wxString s2(wxT("1.3-12"));
		double a2;
		bool bParsed = tstringUtil::ToDouble(s2, a2);
		CHECK(a2 == 1.3);
		CHECK(!bParsed);
	}


	TEST(AtodFR)
	{
		wxLocale locale(wxLANGUAGE_FRENCH);
		wxString s1(wxT("12,3"));
		double a1 = tstringUtil::ToDouble(s1);
		CHECK(a1 == 12.3);
		wxString s2(wxT("1,3-12"));
		double a2;
		bool bParsed = tstringUtil::ToDouble(s2, a2);
		CHECK(a2 == 1.3);
		CHECK(!bParsed);
	}


	TEST(AtodFR2)
	{
		// Even in French, I want to have "." separators parse properly.
		wxLocale locale(wxLANGUAGE_FRENCH);
		wxString s1(wxT("12.3"));
		double a1 = tstringUtil::ToDouble(s1);
		CHECK(a1 == 12.3);
	}


	TEST(ReplaceA)
	{
		std::string s("This is a test");
		std::string s2 = tstringUtil::Replace(s, "is a", "");
		CHECK("This  test" == s2);
		s2 = tstringUtil::Replace(s2, " test", "good");
		CHECK("This good" == s2);
	}


	TEST(ReplaceW)
	{
		std::wstring s(L"This is a test");
		std::wstring s2 = tstringUtil::Replace(s, L"is a", L"");
		CHECK(L"This  test" == s2);
		s2 = tstringUtil::Replace(s2, L" test", L"good");
		CHECK(L"This good" == s2);
	}


	TEST(Formatting)
	{
		wxString one(wxT("one"));
		wxString two(wxT("two"));
		wxString fmt = wxString::Format(wxT("%2$s %1$s"), one.c_str(), two.c_str());
		CHECK(fmt == wxT("two one"));
	}
}
