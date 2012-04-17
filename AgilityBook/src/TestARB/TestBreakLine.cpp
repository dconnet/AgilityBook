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
 * @li 2010-10-17 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "BreakLine.h"

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


SUITE(TestBreakLine)
{
	// String
	static wchar_t const* record4 = L"fld;fld;;fld";
	static wchar_t const* record7 = L"fld;fld;;fld;f\"f;f6;f7";
	static wchar_t const* record5 = L"fld;\"line\n;line\";\"\"\"quote\"\" here\";;";
	static wchar_t const* recordMore1a = L"fld;\"line";
	static wchar_t const* recordMore2a = L";line\";\"\"\"quote\"\" here\";;";
	static wchar_t const* recordMore1b = L"2010-10-30;Name;\"Subname\";\"Line 1";
	static wchar_t const* recordMore2b = L"Line 2\"";


	TEST(BreakLine)
	{
		std::vector<std::wstring> fields;
		CHECK(4 == BreakLine(';', record4, fields));
		CHECK(4 == fields.size());
		CHECK(4 == BreakLine(';', record4, fields, true));
		CHECK(3 == fields.size());
		CHECK(7 == BreakLine(';', record7, fields));
		CHECK(7 == fields.size());
		CHECK(7 == BreakLine(';', record7, fields, true));
		CHECK(6 == fields.size());
	}


	TEST(ReadCSV)
	{
		std::vector<std::wstring> fields;
		CHECK(DataError == ReadCSV(';', L"f\";", fields));
		CHECK(DataError == ReadCSV(';', record7, fields));
		CHECK(DataOk == ReadCSV(';', record5, fields));
		CHECK(5 == fields.size());
		if (5 == fields.size())
		{
			std::vector<std::wstring> fields2;
			GetFields(fields2);
			CHECK(VerifyFields(fields, fields2));
		}
	}


	TEST(ReadCSVMultiLine1)
	{
		std::vector<std::wstring> fields;
		CHECK(DataNeedMore == ReadCSV(';', recordMore1a, fields));
		CHECK(DataOk == ReadCSV(';', recordMore2a, fields, true));
		CHECK(5 == fields.size());
		if (5 == fields.size())
		{
			std::vector<std::wstring> fields2;
			GetFields(fields2);
			CHECK(VerifyFields(fields, fields2));
		}
	}


	TEST(ReadCSVMultiLine2)
	{
		std::vector<std::wstring> fields;
		CHECK(DataNeedMore == ReadCSV(';', recordMore1b, fields));
		CHECK(DataOk == ReadCSV(';', recordMore2b, fields, true));
		CHECK(4 == fields.size());
		if (4 == fields.size())
		{
			CHECK(fields[0] == L"2010-10-30");
			CHECK(fields[1] == L"Name");
			CHECK(fields[2] == L"Subname");
			CHECK(fields[3] == L"Line 1\nLine 2");
		}
	}


	TEST(WriteCSV)
	{
		std::vector<std::wstring> fields;
		GetFields(fields);
		std::wstring data = WriteCSV(';', fields);
		CHECK(data == record5);
	}
}
