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
 * @li 2010-02-07 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "BreakLine.h"


void GetFields(std::vector<wxString>& fields)
{
	fields.clear();
	// This is the same as field5 and fieldMore[12]
	fields.push_back(wxT("fld"));
	fields.push_back(wxT("line\r\n;line"));
	fields.push_back(wxT("\"quote\" here"));
	fields.push_back(wxString());
	fields.push_back(wxT(""));
}


bool VerifyFields(
		std::vector<wxString> const& fields1,
		std::vector<wxString> const& fields2)
{
	if (fields1.size() != fields2.size())
		return false;
	std::vector<wxString>::const_iterator i1 = fields1.begin();
	std::vector<wxString>::const_iterator i2 = fields2.begin();
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
	static wxChar const* record4 = wxT("fld;fld;;fld");
	static wxChar const* record7 = wxT("fld;fld;;fld;f\"f;f6;f7");
	static wxChar const* record5 = wxT("fld;\"line\r\n;line\";\"\"\"quote\"\" here\";;");
	static wxChar const* recordMore1 = wxT("fld;\"line\r\n");
	static wxChar const* recordMore2 = wxT(";line\";\"\"\"quote\"\" here\";;");


	TEST(BreakLine)
	{
		std::vector<wxString> fields;
		CHECK(4 == BreakLine(';', record4, fields));
		CHECK(7 == BreakLine(';', record7, fields));
	}


	TEST(ReadCSV)
	{
		std::vector<wxString> fields;
		CHECK(DataError == ReadCSV(';', wxT("f\";"), fields));
		CHECK(DataError == ReadCSV(';', record7, fields));
		CHECK(DataOk == ReadCSV(';', record5, fields));
		CHECK(5 == fields.size());
		if (5 == fields.size())
		{
			std::vector<wxString> fields2;
			GetFields(fields2);
			CHECK(VerifyFields(fields, fields2));
		}
		CHECK(DataNeedMore == ReadCSV(';', recordMore1, fields));
		CHECK(DataOk == ReadCSV(';', recordMore2, fields, true));
		if (5 == fields.size())
		{
			std::vector<wxString> fields2;
			GetFields(fields2);
			CHECK(VerifyFields(fields, fields2));
		}
	}


	TEST(WriteCSV)
	{
		std::vector<wxString> fields;
		GetFields(fields);
		wxString data = WriteCSV(';', fields);
		CHECK(data == record5);
	}
}
