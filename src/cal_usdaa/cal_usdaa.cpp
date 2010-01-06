/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief Defines the initialization routines for the DLL.
 * @author David Connet
 *
 * Revision History
 * @li 2009-11-01 DRC Change how initialization is done.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-07-24 DRC Change wx initialize code. When exiting app, COM was
 *                hanging in uninitialize.
 * @li 2009-02-25 DRC Ported to wxWidgets.
 * @li 2007-08-12 DRC Created
 */

#include "stdafx.h"


BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		wxSetInstance((HINSTANCE)hModule);
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}
