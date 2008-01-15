/*
 * Copyright © 2008 David Connet. All Rights Reserved.
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
 * @brief Main access (in non-mfc dll, we'd have the DllMain here)
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-11 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"
#include "Local.h"

#include "Element.h"

#pragma comment(lib, "WinUnitLib.lib")


// In theory, this stuff should only be done once - but the framework doesn't
// support that, so just provide global functions so it can be implemented in
// one place. In time, when this is added to WinUnit, we can just make these
// empty functions - unless there really is something we want to do across all
// fixture tests.

bool CommonSetup()
{
	static CLocalization localization;
	static bool bInit = false;
	if (!bInit)
	{
		bInit = true;
		IARBLocalization::Init(&localization);
	}
	tstring errs;
	return Element::Initialize(errs);
}


bool CommonTeardown()
{
	Element::Terminate();
	return true;
}
