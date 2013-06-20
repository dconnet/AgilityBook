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
 * @li 2011-11-17 DRC Add ability to switch languages
 * @li 2009-09-20 DRC Default TODO_TEST to non-failure
 * @li 2008-12-11 DRC Made TODO_TEST have a better failure message
 * @li 2008-01-11 DRC Created
 */

#include "UnitTest++.h"
#include <ARBCommon/ARBTypes.h>
class ARBConfig;

// Set this to 1 to cause TODO_TESTs to fail
#define FAIL_TODO_TEST	0

#define IDR_DTD_AGILITYRECORDBOOK	1


extern const wchar_t* const gc_Configs[];
extern size_t gc_NumConfigs;


extern bool g_bMicroTest;
extern void SetLang(int langId);

extern ElementNodePtr LoadXMLData(size_t id = 0); // 0 is default config

extern bool LoadConfigFromTree(ElementNodePtr tree, ARBConfig& config);
extern ElementNodePtr CreateActionList();

#if FAIL_TODO_TEST
#define TODO_TEST	{bool TODO = false; CHECK(TODO);}
#else
#define TODO_TEST
#endif
