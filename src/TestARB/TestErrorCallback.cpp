/*
 * Copyright © 2008 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
 */

/**
 * @file
 * @brief Test ARBErrorCallback
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-13 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBTypes.h"


class ErrorCallback : public ARBErrorCallback
{
public:
	tstring m_Msg;

	ErrorCallback(tstring& inMsg) : ARBErrorCallback(inMsg)
	{
	}

	virtual void LogMessage(tstring const& inMsg)
	{
		__super::LogMessage(inMsg);
		m_Msg += inMsg;
	}
};


SUITE(TestErrorCallback)
{
	TEST(Callback)
	{
		tstring msg;
		ARBErrorCallback err(msg);
		err.LogMessage(_T("Testing1"));
		CHECK(msg == _T("Testing1"));
	}


	TEST(Derived)
	{
		tstring msg;
		ErrorCallback err(msg);
		err.LogMessage(_T("Testing1"));
		CHECK(msg == err.m_Msg);
		CHECK(msg == _T("Testing1"));
	}
}
