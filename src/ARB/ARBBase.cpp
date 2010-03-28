/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Base class for common functionality.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2003-08-24 DRC Optimized filtering by adding boolean into ARBBase to
 *                    prevent constant re-evaluation.
 */

#include "stdafx.h"
#include "ARBBase.h"

#if defined(_MFC_VER) && defined(_DEBUG)
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////

ARBBase::ARBBase()
{
	for (int i = 0; i < eNumFilters; ++i)
		m_bFiltered[i] = false;
}


ARBBase::~ARBBase()
{
}
