/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test mailto functions
 * @author David Connet
 *
 * Revision History
 * 2021-03-16 Created
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARBCommon/MailTo.h"
#include "fmt/printf.h"

#include <stdarg.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


TEST_CASE("MailTo")
{
	SECTION("To")
	{
		if (!g_bMicroTest)
		{
			CMailTo mailto;
			mailto.AddTo(L"someone@example.com");
			REQUIRE("mailto:someone@example.com" == mailto.Uri());
		}
	}

	SECTION("To2")
	{
		if (!g_bMicroTest)
		{
			CMailTo mailto;
			mailto.AddTo(L"someone2@example.com");
			mailto.AddTo(L"someone@example.com");
			REQUIRE("mailto:someone2@example.com,someone@example.com" == mailto.Uri());
		}
	}

	SECTION("To3")
	{
		if (!g_bMicroTest)
		{
			CMailTo mailto;
			mailto.AddTo(L"someone@example.com");
			mailto.AddTo(L"someone2@example.com");
			REQUIRE("mailto:someone@example.com,someone2@example.com" == mailto.Uri());
		}
	}

	SECTION("Body")
	{
		if (!g_bMicroTest)
		{
			CMailTo mailto;
			mailto.AddTo(L"someone@example.com");
			mailto.SetSubject(L"My Subject");
			// \u00A3 is the Pound currency sign
			std::wstring s(L"Line 1\r\nLine 2\u00A3");
			mailto.SetBody(s);
			REQUIRE(
				"mailto:someone@example.com?subject=My%20Subject&body=Line%201%0d%0aLine%202%c2%a3" == mailto.Uri());
		}
	}
}
