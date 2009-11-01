/*
 * Copyright (c) 2008-2009 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
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


#if wxCHECK_VERSION(2, 9, 1)
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
#endif


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
