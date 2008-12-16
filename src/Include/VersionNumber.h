#pragma once

/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
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
 * Revision History
 * @li 2008-01-10 DRC Added note
 * @li 2007-12-25 DRC Created
 */


#define _STR(x)							#x
#define STR(x)							_STR(x)

#define ARB_VER_MAJOR					1
#define ARB_VER_MINOR					10
#define ARB_VER_DOT						9
#define ARB_VER_FILEVER					12

#define ARB_VER_STR_MAJOR				STR(ARB_VER_MAJOR)
#define ARB_VER_STR_MINOR				STR(ARB_VER_MINOR)
#define ARB_VER_STR_DOT					STR(ARB_VER_DOT)
#define ARB_VER_STR_FILEVER				STR(ARB_VER_FILEVER)

#define ARB_VERSION						ARB_VER_MAJOR,ARB_VER_MINOR,ARB_VER_DOT,ARB_VER_FILEVER
#define ARB_VERSION_STRING				ARB_VER_STR_MAJOR "." ARB_VER_STR_MINOR "." ARB_VER_STR_DOT "." ARB_VER_STR_FILEVER

#define ARB_VERSION_CompanyName			"dcon Software"
#define ARB_VERSION_FileDescription		"Agility Record Book"
#define ARB_VERSION_LegalCopyright		"Copyright © 2002-2008 David Connet. All Rights Reserved."
#define ARB_VERSION_ProductName			"Agility Record Book"
