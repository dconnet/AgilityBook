/*
 * Copyright © 2002-2006 David Connet. All Rights Reserved.
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
#include "DlgFixup.h"
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
	: CDlgConfigureData()
	, m_Venue(venue)
{
}

CDlgConfigureDataVenue::~CDlgConfigureDataVenue()
{
}

CString CDlgConfigureDataVenue::OnNeedText(int iColumn) const
{
	CString str;
	switch (iColumn)
	{
	case 0:
		str = m_Venue->GetName().c_str();
		break;
	case 1:
		str = m_Venue->GetDesc().c_str();
		str.Replace(_T("\n"), _T(" "));
		break;
	}
	return str;
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataFault::CDlgConfigureDataFault(ARBConfigFaultPtr fault)
	: CDlgConfigureData()
	, m_Fault(fault)
{
}

CDlgConfigureDataFault::~CDlgConfigureDataFault()
{
}

CString CDlgConfigureDataFault::OnNeedText(int iColumn) const
{
	return m_Fault->GetName().c_str();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataOtherPoints::CDlgConfigureDataOtherPoints(ARBConfigOtherPointsPtr otherPoints)
	: CDlgConfigureData()
	, m_OtherPoints(otherPoints)
{
}

CDlgConfigureDataOtherPoints::~CDlgConfigureDataOtherPoints()
{
}

CString CDlgConfigureDataOtherPoints::OnNeedText(int iColumn) const
{
	return m_OtherPoints->GetName().c_str();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataDivision::CDlgConfigureDataDivision(ARBConfigDivisionPtr div)
	: CDlgConfigureData()
	, m_Div(div)
{
}

CDlgConfigureDataDivision::~CDlgConfigureDataDivision()
{
}

CString CDlgConfigureDataDivision::OnNeedText(int iColumn) const
{
	return m_Div->GetName().c_str();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataLevel::CDlgConfigureDataLevel(
		ARBConfigDivisionPtr div,
		ARBConfigLevelPtr level)
	: CDlgConfigureData()
	, m_Division(div)
	, m_Level(level)
{
}

CDlgConfigureDataLevel::~CDlgConfigureDataLevel()
{
}

CString CDlgConfigureDataLevel::OnNeedText(int iColumn) const
{
	return m_Level->GetName().c_str();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataSubLevel::CDlgConfigureDataSubLevel(
		ARBConfigDivisionPtr div,
		ARBConfigLevelPtr level,
		ARBConfigSubLevelPtr subLevel)
	: CDlgConfigureData()
	, m_Division(div)
	, m_Level(level)
	, m_SubLevel(subLevel)
{
}

CDlgConfigureDataSubLevel::~CDlgConfigureDataSubLevel()
{
}

CString CDlgConfigureDataSubLevel::OnNeedText(int iColumn) const
{
	return m_SubLevel->GetName().c_str();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataTitle::CDlgConfigureDataTitle(
		ARBConfigTitlePtr title)
	: CDlgConfigureData()
	, m_Title(title)
{
}

CDlgConfigureDataTitle::~CDlgConfigureDataTitle()
{
}

CString CDlgConfigureDataTitle::OnNeedText(int iColumn) const
{
	return m_Title->GetCompleteName(-1, true, true).c_str();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataEvent::CDlgConfigureDataEvent(ARBConfigEventPtr inEvent)
	: CDlgConfigureData()
	, m_Event(inEvent)
{
}

CDlgConfigureDataEvent::~CDlgConfigureDataEvent()
{
}

CString CDlgConfigureDataEvent::OnNeedText(int iColumn) const
{
	return m_Event->GetName().c_str();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataMultiQ::CDlgConfigureDataMultiQ(ARBConfigMultiQPtr multiq)
	: CDlgConfigureData()
	, m_MultiQ(multiq)
{
}

CDlgConfigureDataMultiQ::~CDlgConfigureDataMultiQ()
{
}

CString CDlgConfigureDataMultiQ::OnNeedText(int iColumn) const
{
	return m_MultiQ->GetName().c_str();
}
