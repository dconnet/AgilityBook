/*
 * Copyright © 2008 David Connet. All Rights Reserved.
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
#include <iostream>


SUITE(TestString)
{
	TEST(Convert_ToWide)
	{
		std::string s("narrow");
		std::wstring s2 = tstringUtil::Convert(s);
		CHECK(L"narrow" == s2);
	}


	TEST(Convert_ToNarrow)
	{
		std::wstring s(L"wide");
		std::string s2 = tstringUtil::Convert(s);
		CHECK("wide" == s2);
	}


	TEST(Convert_Multi)
	{
		wchar_t w = 0xFEFB; // In courier new, Arabic Ligature Lam With Alef Isolated Form (see 'Character Map' program)
		std::wstring s(1, w);
		std::string s2 = tstringUtil::Convert(s);
		CHECK(s.length() == 1);
		CHECK(s2.length() == 3);
	}


	TEST(TrimA)
	{
		std::string s(" \tstr\t\r\n ");
		std::string s1 = tstringUtil::Trim(s, ' ');
		CHECK("\tstr\t\r\n" == s1);
		std::string s2 = tstringUtil::TrimLeft(s, ' ');
		CHECK("\tstr\t\r\n " == s2);
		std::string s3 = tstringUtil::TrimRight(s, ' ');
		CHECK(" \tstr\t\r\n" == s3);
	}


	TEST(TrimA2)
	{
		std::string s(" \tstr\t\r\n ");
		std::string s1 = tstringUtil::Trim(s);
		CHECK("str" == s1);
		std::string s2 = tstringUtil::TrimLeft(s);
		CHECK("str\t\r\n " == s2);
		std::string s3 = tstringUtil::TrimRight(s);
		CHECK(" \tstr" == s3);
	}


	TEST(TrimW)
	{
		std::wstring s(L" \tstr\t\r\n ");
		std::wstring s1 = tstringUtil::Trim(s, ' ');
		CHECK(L"\tstr\t\r\n" == s1);
		std::wstring s2 = tstringUtil::TrimLeft(s, ' ');
		CHECK(L"\tstr\t\r\n " == s2);
		std::wstring s3 = tstringUtil::TrimRight(s, ' ');
		CHECK(L" \tstr\t\r\n" == s3);
	}


	TEST(TrimW2)
	{
		std::wstring s(L" \tstr\t\r\n ");
		std::wstring s1 = tstringUtil::Trim(s);
		CHECK(L"str" == s1);
		std::wstring s2 = tstringUtil::TrimLeft(s);
		CHECK(L"str\t\r\n " == s2);
		std::wstring s3 = tstringUtil::TrimRight(s);
		CHECK(L" \tstr" == s3);
	}


	TEST(TrimNone)
	{
		std::string s("This is a test");
		std::string s1 = tstringUtil::Trim(s);
		CHECK(s == s1);
		std::string s2 = tstringUtil::TrimLeft(s);
		CHECK(s == s2);
		std::string s3 = tstringUtil::TrimRight(s);
		CHECK(s == s3);
	}


	TEST(TrimNoneW)
	{
		std::wstring s(L"This is a test");
		std::wstring s1 = tstringUtil::Trim(s);
		CHECK(s == s1);
		std::wstring s2 = tstringUtil::TrimLeft(s);
		CHECK(s == s2);
		std::wstring s3 = tstringUtil::TrimRight(s);
		CHECK(s == s3);
	}


	TEST(TrimAll)
	{
		std::string s("\t  \t\r\n");
		std::string s1 = tstringUtil::Trim(s);
		CHECK(0 == s1.length());
		std::string s2 = tstringUtil::TrimLeft(s);
		CHECK(0 == s2.length());
		std::string s3 = tstringUtil::TrimRight(s);
		CHECK(0 == s3.length());
	}


	TEST(TrimAllW)
	{
		std::wstring s(L"\t  \t\r\n");
		std::wstring s1 = tstringUtil::Trim(s);
		CHECK(0 == s1.length());
		std::wstring s2 = tstringUtil::TrimLeft(s);
		CHECK(0 == s2.length());
		std::wstring s3 = tstringUtil::TrimRight(s);
		CHECK(0 == s3.length());
	}


	TEST(ToLowerA)
	{
		std::string s("As");
		std::string s2 = tstringUtil::ToLower(s);
		CHECK("as" == s2);
	}


	TEST(ToLowerW)
	{
		std::wstring s(L"As");
		std::wstring s2 = tstringUtil::ToLower(s);
		CHECK(L"as" == s2);
	}


	TEST(ToUpperA)
	{
		std::string s("As");
		std::string s2 = tstringUtil::ToUpper(s);
		CHECK("AS" == s2);
	}


	TEST(ToUpperW)
	{
		std::wstring s(L"As");
		std::wstring s2 = tstringUtil::ToUpper(s);
		CHECK(L"AS" == s2);
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
}
