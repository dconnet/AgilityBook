/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief DLL Entry point
 * @author David Connet
 *
 * Revision History
 * @li 2013-08-15 DRC Created
 */

#include "stdafx.h"

#pragma comment(lib, "msi.lib")


BOOL APIENTRY DllMain(
		HMODULE /*hModule*/,
		DWORD  ul_reason_for_call,
		LPVOID /*lpReserved*/)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
