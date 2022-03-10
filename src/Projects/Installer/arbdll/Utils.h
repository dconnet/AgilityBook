#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Utility functions
 * @author David Connet
 *
 * Revision History
 * 2013-08-15 Created
 */

#include <map>
#include <string>


extern void LogMessage(MSIHANDLE hInstall, std::wstring const& msg, INSTALLMESSAGE level = INSTALLMESSAGE_INFO);

// Ops (in WinNT.h):
// VER_EQUAL
// VER_GREATER
// VER_GREATER_EQUAL
// VER_LESS
// VER_LESS_EQUAL
// VER_AND
// VER_OR
extern bool CheckOS(DWORD dwMajor, DWORD dwMinor, int op);

inline bool IsVistaOrBetter()
{
	return CheckOS(6, 0, VER_GREATER_EQUAL);
}

inline bool IsWin7OrBetter()
{
	return CheckOS(6, 1, VER_GREATER_EQUAL);
}


// Returns number of per-machine installs
extern size_t FindProductCodes(WCHAR const* const pUpgradeCode, std::map<std::wstring, bool>& productCodes);


extern bool IsUserAdmin();
