#pragma once

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
 * @brief Global support functions for testing.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-11 DRC Created
 */

// These are needed in order to generate a decent pragma message
#define STRING2(x) #x
#define STRING(x) STRING2(x)
//#pragma message ( __FILE__ "(" STRING(__LINE__) ") : message" )
#define PRAGMA_MESSAGE(x) ( __FILE__ "(" STRING(__LINE__) ") : " x )
//#pragma PRAGMA_MESSAGE("My message")

#include "WinUnit.h"
#include "ARBTypes.h"

extern bool CommonSetup();
extern bool CommonTeardown();

extern ElementNodePtr LoadXMLData(UINT id);
extern bool LoadConfigFromTree(ElementNodePtr tree, ARBConfig& config);
extern ElementNodePtr CreateActionList();

/*
Quick reference for winunit macros:
WIN_ASSERT_EQUAL(expected, actual, ...)			uses '=='
WIN_ASSERT_NOT_EQUAL(notExpected, actual, ...)	uses '!='
WIN_ASSERT_STRING_EQUAL(expected, actual, ...)	case-sensitve string compare
WIN_ASSERT_ZERO(zeroExpression, ...)			compares to 0, fail if not equal
WIN_ASSERT_NOT_ZERO(expr, ...)					compares to 0, fail if equal
WIN_ASSERT_NULL(expr, ...)						compare ptr to NULL
WIN_ASSERT_NOT_NULL(expr, ...)					compare ptr to NULL
WIN_ASSERT_FAIL(message, ...)					always fails
WIN_ASSERT_TRUE(expr, ...)						succeed if true
WIN_ASSERT_FALSE(expr, ...)						succeed if !expr
WIN_ASSERT_WINAPI_SUCCESS(expr, ...)			Makes use of GetLastError
WIN_ASSERT_THROWS(expr, exceptionType, ...)		succeed if expr throws a C++ exception of type
WIN_TRACE(message, ...)							output info msg for debugging
*/
