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
 */

#include "ARBCommon/ARBTypes.h"
class ARBConfig;

#define IDR_DTD_AGILITYRECORDBOOK 1

extern const wchar_t* const gc_Configs[];
extern size_t gc_NumConfigs;


extern std::wstring GetDataFile();
extern ElementNodePtr LoadXMLData(size_t id = 0); // 0 is default config

extern bool LoadConfigFromTree(ElementNodePtr tree, ARBConfig& config);
extern ElementNodePtr CreateActionList();
