/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBDouble functions
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-12 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBTypes.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


static wxString FormNumber(wxString const& d1, wxString const& dec, wxString const& d2)
{
	wxString val;
	val << d1 << dec << d2;
	return val;
}


static void RunDblTests()
{
	wxString decimalPt = wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_NUMBER);

	// ARBDouble always strips 0s unless prec ==2, unless =".00"
	double p = 3.14159265358979323846;
	wxString s = ARBDouble::ToString(p, 2, ARBDouble::eCurrent);
	CHECK(FormNumber(wxT("3"), decimalPt, wxT("14")) == s);
	s = ARBDouble::ToString(p, 4, ARBDouble::eCurrent);
	CHECK(FormNumber(wxT("3"), decimalPt, wxT("1416")) == s);

	p = 2.1;
	s = ARBDouble::ToString(p, 0, ARBDouble::eCurrent);
	CHECK(FormNumber(wxT("2"), decimalPt, wxT("1")) == s);
	s = ARBDouble::ToString(p, 2, ARBDouble::eCurrent);
	CHECK(FormNumber(wxT("2"), decimalPt, wxT("10")) == s);
	s = ARBDouble::ToString(p, 3, ARBDouble::eCurrent);
	CHECK(FormNumber(wxT("2"), decimalPt, wxT("1")) == s);

	p = 2;
	s = ARBDouble::ToString(p, 0, ARBDouble::eCurrent);
	CHECK(wxT("2") == s);
	s = ARBDouble::ToString(p, 1, ARBDouble::eCurrent);
	CHECK(wxT("2") == s);
	s = ARBDouble::ToString(p, 2, ARBDouble::eCurrent);
	CHECK(wxT("2") == s);
}


SUITE(TestDouble)
{
	TEST(strPrecUS)
	{
		if (!g_bMicroTest)
		{
			wxLocale locale(wxLANGUAGE_ENGLISH_US);
			RunDblTests();
		}
	}


	TEST(strPrecFR)
	{
		if (!g_bMicroTest)
		{
			// Using French since the default decimal separator is a comma.
			wxLocale locale(wxLANGUAGE_FRENCH);
			RunDblTests();
		}
	}


	TEST(equal)
	{
		if (!g_bMicroTest)
		{
			double p1 = 3.14159265;
			double p2 = 3.141592;
			double p3 = 3.141592657;
			CHECK(ARBDouble::equal(p1, p2, 0.00001));
			CHECK(!ARBDouble::equal(p1, p2, 0.00000001));
			CHECK(ARBDouble::equal(p1, p3, 0.00000001));
		}
	}
}
