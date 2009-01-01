/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgConfigure class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-01-10 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-04-02 DRC Up the ref count to prevent problems. See comments below.
 * @li 2004-01-14 DRC Use complete title name instead of nice name. When
 *                    renaming a title and that name is in use, prompt to merge.
 * @li 2003-02-23 DRC Modifying the scoring methods wasn't updated in the tree.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgConfigureData.h"

#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "ARBConfigDivision.h"
#include "ARBConfigVenue.h"
#include "DlgConfigEvent.h"
#include "DlgConfigOtherPoints.h"
#include "DlgConfigTitle.h"
#include "DlgConfigure.h"
#include "DlgName.h"
#include "DlgReorder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////

// We _must_ ref count these objects. Otherwise, for instance, when we edit
// Event/Scoring in the tree, the new scoring methods are pushed into the
// event object. The GETDISPINFO triggers before we rebuild the tree, causing
// us to use bad pointers.

CDlgConfigureDataVenue::CDlgConfigureDataVenue(ARBConfigVenuePtr venue)
	: m_Venue(venue)
{
}


CDlgConfigureDataVenue::~CDlgConfigureDataVenue()
{
}


tstring CDlgConfigureDataVenue::OnNeedText() const
{
	tstring str(OnNeedText(0));
	for (int i = 1; i <= 2; ++i)
	{
		tstring s(OnNeedText(i));
		if (!s.empty())
		{
			str += _T("  ") + s;
		}
	}
	return str;
}


tstring CDlgConfigureDataVenue::OnNeedText(int iColumn) const
{
	tstring str;
	switch (iColumn)
	{
	case 0:
		str = m_Venue->GetName();
		break;
	case 1:
		str = m_Venue->GetURL();
		break;
	case 2:
		str = m_Venue->GetDesc();
		str = tstringUtil::Replace(str, _T("\n"), _T(" "));
		break;
	}
	return str;
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataFault::CDlgConfigureDataFault(ARBConfigFaultPtr fault)
	: m_Fault(fault)
{
}


CDlgConfigureDataFault::~CDlgConfigureDataFault()
{
}


tstring CDlgConfigureDataFault::OnNeedText() const
{
	return OnNeedText(0);
}


tstring CDlgConfigureDataFault::OnNeedText(int iColumn) const
{
	return m_Fault->GetName();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataOtherPoints::CDlgConfigureDataOtherPoints(ARBConfigOtherPointsPtr otherPoints)
	: m_OtherPoints(otherPoints)
{
}


CDlgConfigureDataOtherPoints::~CDlgConfigureDataOtherPoints()
{
}


tstring CDlgConfigureDataOtherPoints::OnNeedText() const
{
	return OnNeedText(0);
}


tstring CDlgConfigureDataOtherPoints::OnNeedText(int iColumn) const
{
	return m_OtherPoints->GetName();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataDivision::CDlgConfigureDataDivision(ARBConfigDivisionPtr div)
	: m_Div(div)
{
}


CDlgConfigureDataDivision::~CDlgConfigureDataDivision()
{
}


tstring CDlgConfigureDataDivision::OnNeedText() const
{
	return OnNeedText(0);
}


tstring CDlgConfigureDataDivision::OnNeedText(int iColumn) const
{
	return m_Div->GetName();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataLevel::CDlgConfigureDataLevel(
		ARBConfigDivisionPtr div,
		ARBConfigLevelPtr level)
	: m_Division(div)
	, m_Level(level)
{
}


CDlgConfigureDataLevel::~CDlgConfigureDataLevel()
{
}


tstring CDlgConfigureDataLevel::OnNeedText() const
{
	return OnNeedText(0);
}


tstring CDlgConfigureDataLevel::OnNeedText(int iColumn) const
{
	return m_Level->GetName();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataSubLevel::CDlgConfigureDataSubLevel(
		ARBConfigDivisionPtr div,
		ARBConfigLevelPtr level,
		ARBConfigSubLevelPtr subLevel)
	: m_Division(div)
	, m_Level(level)
	, m_SubLevel(subLevel)
{
}


CDlgConfigureDataSubLevel::~CDlgConfigureDataSubLevel()
{
}


tstring CDlgConfigureDataSubLevel::OnNeedText() const
{
	return OnNeedText(0);
}


tstring CDlgConfigureDataSubLevel::OnNeedText(int iColumn) const
{
	return m_SubLevel->GetName();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataTitle::CDlgConfigureDataTitle(
		ARBConfigTitlePtr title)
	: m_Title(title)
{
}


CDlgConfigureDataTitle::~CDlgConfigureDataTitle()
{
}


tstring CDlgConfigureDataTitle::OnNeedText() const
{
	return OnNeedText(0);
}


tstring CDlgConfigureDataTitle::OnNeedText(int iColumn) const
{
	return m_Title->GetCompleteName(-1, false, true, true);
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataEvent::CDlgConfigureDataEvent(ARBConfigEventPtr inEvent)
	: m_Event(inEvent)
{
}


CDlgConfigureDataEvent::~CDlgConfigureDataEvent()
{
}


tstring CDlgConfigureDataEvent::OnNeedText() const
{
	return OnNeedText(0);
}


tstring CDlgConfigureDataEvent::OnNeedText(int iColumn) const
{
	return m_Event->GetName();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataMultiQ::CDlgConfigureDataMultiQ(ARBConfigMultiQPtr multiq)
	: m_MultiQ(multiq)
{
}


CDlgConfigureDataMultiQ::~CDlgConfigureDataMultiQ()
{
}


tstring CDlgConfigureDataMultiQ::OnNeedText() const
{
	return OnNeedText(0);
}


tstring CDlgConfigureDataMultiQ::OnNeedText(int iColumn) const
{
	return m_MultiQ->GetName();
}
