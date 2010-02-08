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
	static wxString DigestString("b36b52c4835d88fdb548087b9a829bf1");


	TEST(RawDecode)
	{
		wxString digest = ARBMsgDigest::ComputeBuffer(RawString, strlen(RawString));
		CHECK(digest == DigestString);
	}
}
