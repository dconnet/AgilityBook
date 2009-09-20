#pragma once

/*
 * Copyright (c) 2008-2009 David Connet. All Rights Reserved.
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
 * @li 2009-09-20 DRC Default TODO_TEST to non-failure
 * @li 2008-12-11 DRC Made TODO_TEST have a better failure message
 * @li 2008-01-11 DRC Created
 */

#include "UnitTest++.h"
#include "ARBTypes.h"

// Set this to 1 to cause TODO_TESTs to fail
#define FAIL_TODO_TEST	0

#define IDR_DTD_AGILITYRECORDBOOK	1
#define IDR_XML_DEFAULT_CONFIG		2
#define IDR_XML_CONFIG08_V10_2		3
#define IDR_XML_CONFIG09_V11_0		4
#define IDR_XML_CONFIG12_V12_1		5
#define IDR_XML_CONFIG14_V12_2		6
#define IDR_XML_CONFIG19_V12_5		7
#define IDR_XML_CONFIG20_V12_6		8
#define IDR_XML_CONFIG21_V12_7		9
#define IDR_XML_CONFIG22_V12_7		10
#define IDR_XML_CONFIG23_V12_8		11
#define IDR_XML_CONFIG24_V12_8		12
#define IDR_XML_CONFIG25_V12_9		13
#define IDR_XML_CONFIG26_V12_9		14
#define IDR_XML_CONFIG27_V12_10		15
// See comments in TestARB.cpp about adding a new config


extern ElementNodePtr LoadXMLData(int id);

extern bool LoadConfigFromTree(ElementNodePtr tree, ARBConfig& config);
extern ElementNodePtr CreateActionList();

#if FAIL_TODO_TEST
#define TODO_TEST	{bool TODO = false; CHECK(TODO);}
#else
#define TODO_TEST
#endif
