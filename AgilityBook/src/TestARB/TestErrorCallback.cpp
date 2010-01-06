/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
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

#include "ARBTypes.h"


class ErrorCallback : public ARBErrorCallback
{
public:
	wxString m_Msg;

	ErrorCallback(wxString& inMsg) : ARBErrorCallback(inMsg)
	{
	}

	virtual void LogMessage(wxString const& inMsg)
	{
		ARBErrorCallback::LogMessage(inMsg);
		m_Msg += inMsg;
	}
};


SUITE(TestErrorCallback)
{
	TEST(Callback)
	{
		wxString msg;
		ARBErrorCallback err(msg);
		err.LogMessage(wxT("Testing1"));
		CHECK(msg == wxT("Testing1"));
	}


	TEST(Derived)
	{
		wxString msg;
		ErrorCallback err(msg);
		err.LogMessage(wxT("Testing1"));
		CHECK(msg == err.m_Msg);
		CHECK(msg == wxT("Testing1"));
	}
}
