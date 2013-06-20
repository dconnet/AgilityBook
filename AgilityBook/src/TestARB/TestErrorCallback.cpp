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
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-13 DRC Created
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


SUITE(TestErrorCallback)
{
	TEST(Callback)
	{
		if (!g_bMicroTest)
		{
			std::wostringstream msg;
			ARBErrorCallback err(msg);
			err.LogMessage(L"Testing1");
			CHECK(msg.str() == L"Testing1");
		}
	}


	TEST(Derived)
	{
		if (!g_bMicroTest)
		{
			std::wostringstream emsg;
			ErrorCallback err(emsg);
			err.LogMessage(L"Testing1");
			std::wstring msg = emsg.str();
			CHECK(msg == err.m_Msg);
			CHECK(msg == L"Testing1");
		}
	}
}
