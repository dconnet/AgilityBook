/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Utility functions
 * @author David Connet
 *
 * Revision History
 * @li 2013-08-15 DRC Created
 */

#include "stdafx.h"
#include "Utils.h"


static const int cchGUID = 38;

//////////////////////////////////////////////////////////////////////////////

void LogMessage(MSIHANDLE hInstall, std::wstring const& msg, INSTALLMESSAGE level)
{
	PMSIHANDLE hRecord = MsiCreateRecord(1);
	MsiRecordSetString(hRecord, 1, msg.c_str());
	MsiProcessMessage(hInstall, level, hRecord);
}

//////////////////////////////////////////////////////////////////////////////

bool CheckOS(DWORD dwMajor, DWORD dwMinor, int op)
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;

	ZeroMemory(&osvi, sizeof(osvi));
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	osvi.dwMajorVersion = dwMajor;
	osvi.dwMinorVersion = dwMinor;
	osvi.wServicePackMajor = 0;
	osvi.wServicePackMinor = 0;

#pragma warning (push)
// warning C4244: 'argument' : conversion from 'int' to 'BYTE', possible loss of data
#pragma warning (disable : 4244)
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, op);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, op);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMAJOR, op);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMINOR, op);
#pragma warning (pop)

	return !!VerifyVersionInfo(&osvi, 
			VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
			dwlConditionMask);
}

//////////////////////////////////////////////////////////////////////////////

size_t FindProductCodes(
		WCHAR const* const pUpgradeCode,
		std::map<std::wstring, bool>& productCodes)
{
	size_t hasPerMachine = 0;

	productCodes.clear();

	DWORD iProductIndex = 0;
	WCHAR productCode[cchGUID+1];
	while (ERROR_SUCCESS == MsiEnumRelatedProducts(pUpgradeCode, 0, iProductIndex++, productCode))
	{
		WCHAR szAssignmentType[10] = {0};
		DWORD cchAssignmentType = _countof(szAssignmentType);
		bool bPerMachine = false;

		// Note: This will only see the current users per-user installs.
		// Other user per-user installs are invisible.
		if (ERROR_SUCCESS == MsiGetProductInfo(productCode, INSTALLPROPERTY_ASSIGNMENTTYPE, szAssignmentType, &cchAssignmentType))
		{
			if (L'1' == szAssignmentType[0])
			{
				++hasPerMachine;
				bPerMachine = true;
			}
		}
		productCodes[productCode] = bPerMachine;
	}

	return hasPerMachine;
}

//////////////////////////////////////////////////////////////////////////////

class CHandle
{
public:
	CHandle(HANDLE& handle) : m_Handle(handle)
	{
	}
	~CHandle()
	{
		if (m_Handle)
			CloseHandle(m_Handle);
	}

	HANDLE& m_Handle;
private:
	CHandle(CHandle const&);
	CHandle& operator=(CHandle const&);
};


bool IsUserAdmin()
{
	HANDLE hProcess = GetCurrentProcess();
	CHandle cleanupProcess(hProcess);

	HANDLE hProcessToken = NULL;
	CHandle cleanupProcessToken(hProcessToken);

	if (!OpenProcessToken(hProcess, TOKEN_QUERY, &hProcessToken))
		return false;

	// Get admin SID
	char AdminSID[SECURITY_MAX_SID_SIZE];
	DWORD dwLength = _countof(AdminSID);
	if (!CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, &AdminSID, &dwLength))
		return false;

	// Check to see if the current token contains admin SID
	BOOL bIsAdmin;
	if (!CheckTokenMembership(NULL, &AdminSID, &bIsAdmin))
		return false;

	if (bIsAdmin)
		return true;

	// On Vista+, UAC may be inhibiting the admin SID. Go deeper.
	if (IsVistaOrBetter())
	{
		HANDLE hLinkedToken = NULL;
		CHandle cleanupToken(hLinkedToken);

		if (GetTokenInformation(hProcessToken, TokenLinkedToken, (void*)&hLinkedToken, sizeof(HANDLE), &dwLength))
		{
			if (!CheckTokenMembership(hLinkedToken, &AdminSID, &bIsAdmin))
			{
				bIsAdmin = FALSE;
			}
		}
	}

	return !!bIsAdmin;
}
