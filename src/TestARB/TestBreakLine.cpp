/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test line parsing routines
 * @author David Connet
 *
 * Revision History
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2012-07-25 Add 2 multiline CSV tests.
 * 2010-10-17 Created
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARBCommon/BreakLine.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


void GetFields(std::vector<std::wstring>& fields)
{
	fields.clear();
	// This is the same as field5 and fieldMore[12]
	fields.push_back(L"fld");
	fields.push_back(L"line\n;line");
	fields.push_back(L"\"quote\" here");
	fields.push_back(std::wstring());
	fields.push_back(L"");
}


bool VerifyFields(
		std::vector<std::wstring> const& fields1,
		std::vector<std::wstring> const& fields2)
{
	if (fields1.size() != fields2.size())
		return false;
	std::vector<std::wstring>::const_iterator i1 = fields1.begin();
	std::vector<std::wstring>::const_iterator i2 = fields2.begin();
	for (; i1 != fields1.end(); ++i1, ++i2)
	{
		if (*i1 != *i2)
			return false;
	}
	return true;
}


TEST_CASE("BreakLine")
{
	// String
	static wchar_t const* record4 = L"fld;fld;;fld";
	static wchar_t const* record7 = L"fld;fld;;fld;f\"f;f6;f7";
	static wchar_t const* record5 = L"fld;\"line\n;line\";\"\"\"quote\"\" here\";;";
	static wchar_t const* recordMore1a = L"fld;\"line";
	static wchar_t const* recordMore2a = L";line\";\"\"\"quote\"\" here\";;";
	static wchar_t const* recordMore1b = L"2010-10-30;Name;\"Subname\";\"Line 1";
	static wchar_t const* recordMore2b = L"Line 2\"";
	static wchar_t const* recordMore1c = L"field1;\"Line1";
	static wchar_t const* recordMore2c = L"Line2";
	static wchar_t const* recordMore2cb = L"";
	static wchar_t const* recordMore3c = L"Line3\";field3";


	SECTION("BreakLine")
	{
		std::vector<std::wstring> fields;
		REQUIRE(4 == BreakLine(';', record4, fields));
		REQUIRE(4 == fields.size());
		REQUIRE(4 == BreakLine(';', record4, fields, true));
		REQUIRE(3 == fields.size());
		REQUIRE(7 == BreakLine(';', record7, fields));
		REQUIRE(7 == fields.size());
		REQUIRE(7 == BreakLine(';', record7, fields, true));
		REQUIRE(6 == fields.size());
	}


	SECTION("ReadCSV")
	{
		std::vector<std::wstring> fields;
		REQUIRE(ReadStatus::Error == ReadCSV(';', L"f\";", fields));
		REQUIRE(ReadStatus::Error == ReadCSV(';', record7, fields));
		REQUIRE(ReadStatus::Ok == ReadCSV(';', record5, fields));
		REQUIRE(5 == fields.size());
		if (5 == fields.size())
		{
			std::vector<std::wstring> fields2;
			GetFields(fields2);
			REQUIRE(VerifyFields(fields, fields2));
		}
	}


	SECTION("ReadCSVMultiLine1")
	{
		std::vector<std::wstring> fields;
		REQUIRE(ReadStatus::NeedMore == ReadCSV(';', recordMore1a, fields));
		REQUIRE(ReadStatus::Ok == ReadCSV(';', recordMore2a, fields, true));
		REQUIRE(5 == fields.size());
		if (5 == fields.size())
		{
			std::vector<std::wstring> fields2;
			GetFields(fields2);
			REQUIRE(VerifyFields(fields, fields2));
		}
	}


	SECTION("ReadCSVMultiLine2")
	{
		std::vector<std::wstring> fields;
		REQUIRE(ReadStatus::NeedMore == ReadCSV(';', recordMore1b, fields));
		REQUIRE(ReadStatus::Ok == ReadCSV(';', recordMore2b, fields, true));
		REQUIRE(4 == fields.size());
		if (4 == fields.size())
		{
			REQUIRE(fields[0] == L"2010-10-30");
			REQUIRE(fields[1] == L"Name");
			REQUIRE(fields[2] == L"Subname");
			REQUIRE(fields[3] == L"Line 1\nLine 2");
		}
	}


	SECTION("ReadCSVMultiLine3")
	{
		std::vector<std::wstring> fields;
		REQUIRE(ReadStatus::NeedMore == ReadCSV(';', recordMore1c, fields));
		REQUIRE(ReadStatus::NeedMore == ReadCSV(';', recordMore2c, fields, true));
		REQUIRE(ReadStatus::Ok == ReadCSV(';', recordMore3c, fields, true));
		REQUIRE(3 == fields.size());
		if (3 == fields.size())
		{
			REQUIRE(fields[0] == L"field1");
			REQUIRE(fields[1] == L"Line1\nLine2\nLine3");
			REQUIRE(fields[2] == L"field3");
		}
	}


	SECTION("ReadCSVMultiLine4")
	{
		std::vector<std::wstring> fields;
		REQUIRE(ReadStatus::NeedMore == ReadCSV(';', recordMore1c, fields));
		REQUIRE(ReadStatus::NeedMore == ReadCSV(';', recordMore2c, fields, true));
		REQUIRE(ReadStatus::NeedMore == ReadCSV(';', recordMore2cb, fields, true));
		REQUIRE(ReadStatus::Ok == ReadCSV(';', recordMore3c, fields, true));
		REQUIRE(3 == fields.size());
		if (3 == fields.size())
		{
			REQUIRE(fields[0] == L"field1");
			REQUIRE(fields[1] == L"Line1\nLine2\n\nLine3");
			REQUIRE(fields[2] == L"field3");
		}
	}


	SECTION("WriteCSV")
	{
		std::vector<std::wstring> fields;
		GetFields(fields);
		std::wstring data = WriteCSV(';', fields);
		REQUIRE(data == record5);
	}
}
