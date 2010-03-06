/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 * @brief Test MD5 class
 * @author David Connet
 *
 * Revision History
 * @li 2010-02-07 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBMsgDigest.h"


SUITE(TestMsgDigest)
{
	// String
	static char const* RawString = "This is a test of a string";
	static wxString DigestString(wxT("b36b52c4835d88fdb548087b9a829bf1"));


	TEST(RawDecode)
	{
		if (!g_bMicroTest)
		{
			wxString digest = ARBMsgDigest::ComputeBuffer(RawString, strlen(RawString));
			CHECK(digest == DigestString);
		}
	}
}
