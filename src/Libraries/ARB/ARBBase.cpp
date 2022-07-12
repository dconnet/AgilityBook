/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Base class for common functionality.
 * @author David Connet
 *
 * Revision History
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2003-08-24 Optimized filtering by adding boolean into ARBBase to
 *            prevent constant re-evaluation.
 */

#include "stdafx.h"
#include "ARB/ARBBase.h"

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

size_t FilterToIndex(ARBFilterType inFilterType)
{
	switch (inFilterType)
	{
	case ARBFilterType::Full:
		return 0;
	case ARBFilterType::IgnoreQ:
		return 1;
	}
	// 'enum class' handles all cases via the switch above
	return 0;
}

} // namespace


unsigned short GetFilterMask(ARBFilterType type)
{
	return 0x1 << static_cast<unsigned short>(type);
}

/////////////////////////////////////////////////////////////////////////////

ARBBase::ARBBase()
	: m_bFiltered({{false, false}})
{
}


ARBBase::~ARBBase()
{
}


bool ARBBase::IsFiltered(ARBFilterType inFilterType) const
{
	return m_bFiltered[FilterToIndex(inFilterType)];
}


void ARBBase::SetFiltered(ARBFilterType inFilterType, bool bFiltered)
{
	m_bFiltered[FilterToIndex(inFilterType)] = bFiltered;
}

} // namespace ARB
} // namespace dconSoft
