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

/////////////////////////////////////////////////////////////////////////////

static size_t FilterToIndex(ARBBase::FilterType inFilterType)
{
	switch (inFilterType)
	{
	default:
		assert(0);
		// fall thru
	case ARBBase::eFilter:
		return 0;
	case ARBBase::eIgnoreQ:
		return 1;
	}
}

/////////////////////////////////////////////////////////////////////////////

ARBBase::ARBBase()
	: m_bFiltered({false, false})
{
}


ARBBase::~ARBBase()
{
}


bool ARBBase::IsFiltered(FilterType inFilterType) const
{
	return m_bFiltered[FilterToIndex(inFilterType)];
}


void ARBBase::SetFiltered(
	FilterType inFilterType,
	bool bFiltered)
{
	m_bFiltered[FilterToIndex(inFilterType)] = bFiltered;
}
