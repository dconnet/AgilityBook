/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBSetAllUsers custom action
 * @author David Connet
 *
 * Revision History
 * 2013-08-15 Created
 */

#include "stdafx.h"

#include "Utils.h"
#include <sstream>


namespace
{
constexpr int cchGUID = 38;

void SetAllUserDefaults(MSIHANDLE hInstall, bool bAdmin, bool bSuppressDialog)
{
	if (bSuppressDialog)
	{
		LogMessage(hInstall, L"Set ARBHasPerUserSupport=''");
		::MsiSetProperty(hInstall, L"ARBHasPerUserSupport", L"");
	}

	if (bAdmin)
	{
		if (IsWin7OrBetter())
		{
			LogMessage(hInstall, L"Set ALLUSERS=2");
			::MsiSetProperty(hInstall, L"ALLUSERS", L"2");
			LogMessage(hInstall, L"Set MSIINSTALLPERUSER=''");
			::MsiSetProperty(hInstall, L"MSIINSTALLPERUSER", L"");
		}
		else
		{
			LogMessage(hInstall, L"Set ALLUSERS=1");
			::MsiSetProperty(hInstall, L"ALLUSERS", L"1");
		}
	}
	else
	{
		if (IsWin7OrBetter())
		{
			LogMessage(hInstall, L"Set ALLUSERS=''");
			::MsiSetProperty(hInstall, L"ALLUSERS", L"");
			LogMessage(hInstall, L"Set MSIINSTALLPERUSER=1");
			::MsiSetProperty(hInstall, L"MSIINSTALLPERUSER", L"1");
		}
		else
		{
			LogMessage(hInstall, L"Set ALLUSERS=''");
			::MsiSetProperty(hInstall, L"ALLUSERS", L"");
		}
	}
}
} // namespace

//////////////////////////////////////////////////////////////////////////////

// Both sequences, run before FindRelatedProducts (ALLUSERS influences FRP)
extern "C" UINT __stdcall ARBSetAllUsers(MSIHANDLE hInstall)
{
	UINT rc = ERROR_SUCCESS;
	LogMessage(hInstall, L"==== ARBSetAllUsers");

	// Get the installers UpgradeCode
	WCHAR upgradeCode[cchGUID + 1] = {0};
	DWORD buflen = _countof(upgradeCode);
	if (ERROR_SUCCESS != (rc = MsiGetProperty(hInstall, L"UpgradeCode", upgradeCode, &buflen)))
	{
		std::wstringstream buf;
		buf << L"ERROR: MsiGetProperty failed: " << rc;
		LogMessage(hInstall, buf.str(), INSTALLMESSAGE_ERROR);
		return ERROR_INSTALL_FAILURE;
	}

	// Find any existing installs (both permachine and peruser)
	std::map<std::wstring, bool> productCodes;
	const size_t hasPerMachine = FindProductCodes(upgradeCode, productCodes);

	const bool bIsAdmin = IsUserAdmin();

	std::wstring ARBAllUsers(L"0");
	if (bIsAdmin)
		ARBAllUsers = L"1";

	// Something's installed.
	if (0 < productCodes.size())
	{
		if (0 < hasPerMachine)
		{
			SetAllUserDefaults(hInstall, true, true);
		}
		else
		{
			ARBAllUsers = L"0";
			SetAllUserDefaults(hInstall, false, true);
		}
	}
	else
	{
		SetAllUserDefaults(hInstall, bIsAdmin, false);
	}

	// Set UI default
	{
		std::wstringstream buf;
		buf << L"Set ARBAllUsers=" << ARBAllUsers;
		LogMessage(hInstall, buf.str());
	}
	::MsiSetProperty(hInstall, L"ARBAllUsers", ARBAllUsers.c_str());

	return ERROR_SUCCESS;
}
