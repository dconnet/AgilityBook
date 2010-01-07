#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief EXE/DLL version number
 * @author David Connet
 *
 * Revision History
 * @li 2009-04-12 DRC Bumped to v2 for wxWidget build
 * @li 2009-02-25 DRC Created
 */


#include "VersionNumber.h"

#define CAL_VER_MAJOR					2
#define CAL_VER_MINOR					1
#define CAL_VER_DOT						0
#define CAL_VER_BUILD					2567

#define CAL_VER_STR_MAJOR				STR(CAL_VER_MAJOR)
#define CAL_VER_STR_MINOR				STR(CAL_VER_MINOR)
#define CAL_VER_STR_DOT					STR(CAL_VER_DOT)
#define CAL_VER_STR_FILEVER				STR(CAL_VER_BUILD)

#define CAL_VERSION						CAL_VER_MAJOR,CAL_VER_MINOR,CAL_VER_DOT,CAL_VER_BUILD
#define CAL_VERSION_STRING				CAL_VER_STR_MAJOR "." CAL_VER_STR_MINOR "." CAL_VER_STR_DOT "." CAL_VER_STR_FILEVER
