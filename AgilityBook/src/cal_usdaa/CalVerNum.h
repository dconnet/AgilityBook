#pragma once

/*
 * Copyright (c) 2002-2009 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
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
#define CAL_VER_BUILD					2524

#define CAL_VER_STR_MAJOR				STR(CAL_VER_MAJOR)
#define CAL_VER_STR_MINOR				STR(CAL_VER_MINOR)
#define CAL_VER_STR_DOT					STR(CAL_VER_DOT)
#define CAL_VER_STR_FILEVER				STR(CAL_VER_BUILD)

#define CAL_VERSION						CAL_VER_MAJOR,CAL_VER_MINOR,CAL_VER_DOT,CAL_VER_BUILD
#define CAL_VERSION_STRING				CAL_VER_STR_MAJOR "." CAL_VER_STR_MINOR "." CAL_VER_STR_DOT "." CAL_VER_STR_FILEVER
