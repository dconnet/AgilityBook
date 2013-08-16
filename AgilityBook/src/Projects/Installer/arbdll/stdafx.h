#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @author David Connet
 *
 * Revision History
 * @li 2013-08-15 DRC Created
 */

// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.
#define _WIN32_WINNT 0x0501
#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

#include <windows.h>

#ifndef _WIN32_MSI
#define _WIN32_MSI 301
#endif
#include <msiquery.h>
