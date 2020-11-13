#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Global support functions for testing.
 * @author David Connet
 *
 * Revision History
 * 2019-10-13 Separated ARB specific things from TestLib.
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2011-11-17 Add ability to switch languages
 * 2009-09-20 Default TODO_TEST to non-failure
 * 2008-12-11 Made TODO_TEST have a better failure message
 * 2008-01-11 Created
 */

#include "ARBCommon/ARBTypes.h"
#if defined(USING_CATCH3)
#include "catch2/catch_all.hpp"
#elif defined(USING_CATCH2)
#include "catch2/catch.hpp"
#else
#error Unknown test framework
#endif

// Set this to 1 to cause TODO_TESTs to fail
#define FAIL_TODO_TEST 0

extern bool g_bMicroTest;
extern void SetLang(wxLanguage langId);

#if FAIL_TODO_TEST
#define TODO_TEST \
	{ \
		bool TODO = false; \
		REQUIRE(TODO); \
	}
#else
#define TODO_TEST
#endif
