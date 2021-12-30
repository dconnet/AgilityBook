#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief EXE/DLL version number
 * @author David Connet
 *
 * @note To use this in VERSIONINFO, the VERSIONINFO resource must be moved
 * into the .rc2 file (non-editable resources). In addition, the vcproj files
 * need to have "../../Include" set on the project resource section (that path
 * is relative to the vcproj file). DO NOT include this file in the main .rc
 * as an include in the TEXTINCLUDE2 section. Otherwise you will have a
 * problem opening the resources in the resource editor as that needs a path
 * relative to the .rc file. (Ok, you could do this if you _also_ add
 * "../Include" to the vcproj resource path, but that just seems weird).
 * Since this file is only used in the rc2 file, just include it there.
 * This keeps all editors/compilers happy.
 *
 * IMPORTANT: When updating the copyright, also update ../../configure.in
 * (the version number is auto-updated)
 *
 * Revision History
 * 2019-02-13 Created
 */


#include "VersionNumber.h"

#define _STR(x) #x
#define STR(x)  _STR(x)

#define ARB_VER_STR_MAJOR   STR(ARB_VER_MAJOR)
#define ARB_VER_STR_MINOR   STR(ARB_VER_MINOR)
#define ARB_VER_STR_DOT     STR(ARB_VER_DOT)
#define ARB_VER_STR_FILEVER STR(ARB_VER_BUILD)

#define ARB_VERSION        ARB_VER_MAJOR, ARB_VER_MINOR, ARB_VER_DOT, ARB_VER_BUILD
#define ARB_VERSION_STRING ARB_VER_STR_MAJOR "." ARB_VER_STR_MINOR "." ARB_VER_STR_DOT "." ARB_VER_STR_FILEVER

#define ARB_VERSION_CompanyName     "dcon Software"
#define ARB_VERSION_FileDescription "Agility Record Book"
#define ARB_VERSION_LegalCopyright  "Copyright (c) 2002-2022 David Connet. All Rights Reserved."
#define ARB_VERSION_ProductName     "Agility Record Book"
