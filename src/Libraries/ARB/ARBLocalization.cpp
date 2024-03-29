/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Localization
 * @author David Connet
 *
 * If ARB is ported to a different platform, this class will need to be
 * changed. It makes use of CStrings and resources.
 *
 * Revision History
 * 2007-09-12 Moved hardcoded strings out of ARBAgilityRecordBook.h
 */

#include "stdafx.h"
#include "ARB/ARBLocalization.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARBCommon;
namespace ARB
{

namespace
{
IARBLocalization const* s_Localization = nullptr;
} // namespace


void IARBLocalization::Init(IARBLocalization const* pLocalization)
{
	s_Localization = pLocalization;
}


IARBLocalization const* Localization()
{
	assert(!!s_Localization);
	return s_Localization;
}

} // namespace ARB
} // namespace dconSoft
