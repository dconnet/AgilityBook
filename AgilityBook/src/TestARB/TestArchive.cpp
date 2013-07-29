/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test LibArchive
 * @author David Connet
 *
 * Revision History
 * @li 2013-07-29 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "LibArchive/LibArchive.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


SUITE(TestArchive)
{
	TEST(ReplaceFile)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		}
	}


	TEST(ExtractFile)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		}
	}
}
