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
#include "TestLib.h"

#include "ARB/ARBTypes2.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;

class ErrorCallback : public ARBErrorCallback
{
public:
	wxString m_Msg;

	ErrorCallback(wxString& inMsg)
		: ARBErrorCallback(inMsg)
	{
	}

	void LogMessage(wxString const& inMsg) override
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
			wxString msg;
			ARBErrorCallback err(msg);
			err.LogMessage(L"Testing1");
			REQUIRE(msg == L"Testing1");
		}
	}


	SECTION("Derived")
	{
		if (!g_bMicroTest)
		{
			wxString emsg;
			ErrorCallback err(emsg);
			err.LogMessage(L"Testing1");
			wxString msg = emsg;
			REQUIRE(msg == err.m_Msg);
			REQUIRE(msg == L"Testing1");
		}
	}
}

} // namespace dconSoft
