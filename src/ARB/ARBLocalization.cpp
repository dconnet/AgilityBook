/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief Localization
 * @author David Connet
 *
 * If ARB is ported to a different platform, this class will need to be
 * changed. It makes use of CStrings and resources.
 *
 * Revision History
 * @li 2007-09-12 DRC Moved hardcoded strings out of ARBAgilityRecordBook.h
 */

#include "stdafx.h"
#include "ARBLocalization.h"

#if defined(_MFC_VER) && defined(_DEBUG)
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////

static IARBLocalization const* s_Localization = NULL;

void IARBLocalization::Init(IARBLocalization const* pLocalization)
{
	s_Localization = pLocalization;
}


IARBLocalization const* Localization()
{
	assert(NULL != s_Localization);
	return s_Localization;
}
