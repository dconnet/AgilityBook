/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBErrorCallback
 * @author David Connet
 *
 * Revision History
 * 2018-12-16 Convert to fmt.
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-13 Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARB/ARBTypes2.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


class ErrorCallback : public ARBErrorCallback
{
public:
	std::wstring m_Msg;

	ErrorCallback(fmt::wmemory_buffer& inMsg) : ARBErrorCallback(inMsg)
	{
	}

	void LogMessage(std::wstring const& inMsg) override
	{
		ARBErrorCallback::LogMessage(inMsg);
		m_Msg += inMsg;
	}
};


TEST_CASE("ErrorCallback")
{
	SECTION("Callback")
	{
		if (!g_bMicroTest)
		{
			fmt::wmemory_buffer msg;
			ARBErrorCallback err(msg);
			err.LogMessage(L"Testing1");
			REQUIRE(fmt::to_string(msg) == L"Testing1");
		}
	}


	SECTION("Derived")
	{
		if (!g_bMicroTest)
		{
			fmt::wmemory_buffer emsg;
			ErrorCallback err(emsg);
			err.LogMessage(L"Testing1");
			std::wstring msg = fmt::to_string(emsg);
			REQUIRE(msg == err.m_Msg);
			REQUIRE(msg == L"Testing1");
		}
	}
}
