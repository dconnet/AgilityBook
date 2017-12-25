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

	ErrorCallback(std::wostringstream& inMsg) : ARBErrorCallback(inMsg)
	{
	}

	virtual void LogMessage(std::wstring const& inMsg)
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
			std::wostringstream msg;
			ARBErrorCallback err(msg);
			err.LogMessage(L"Testing1");
			REQUIRE(msg.str() == L"Testing1");
		}
	}


	SECTION("Derived")
	{
		if (!g_bMicroTest)
		{
			std::wostringstream emsg;
			ErrorCallback err(emsg);
			err.LogMessage(L"Testing1");
			std::wstring msg = emsg.str();
			REQUIRE(msg == err.m_Msg);
			REQUIRE(msg == L"Testing1");
		}
	}
}
