/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
#include "DlgNameDesc.h"
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

CDlgConfigureDataVenue::CDlgConfigureDataVenue(ARBConfigVenue* venue)
	: CDlgConfigureData()
	, m_Venue(venue)
{
	m_Venue->AddRef();
}

CDlgConfigureDataVenue::~CDlgConfigureDataVenue()
{
	m_Venue->Release();
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
		str.Replace("\n", " ");
		break;
	}
	return str;
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataFault::CDlgConfigureDataFault(ARBConfigFault* fault)
	: CDlgConfigureData()
	, m_Fault(fault)
{
	m_Fault->AddRef();
}

CDlgConfigureDataFault::~CDlgConfigureDataFault()
{
	m_Fault->Release();
}

CString CDlgConfigureDataFault::OnNeedText(int iColumn) const
{
	return m_Fault->GetName().c_str();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataOtherPoints::CDlgConfigureDataOtherPoints(ARBConfigOtherPoints* otherPoints)
	: CDlgConfigureData()
	, m_OtherPoints(otherPoints)
{
	m_OtherPoints->AddRef();
}

CDlgConfigureDataOtherPoints::~CDlgConfigureDataOtherPoints()
{
	m_OtherPoints->Release();
}

CString CDlgConfigureDataOtherPoints::OnNeedText(int iColumn) const
{
	return m_OtherPoints->GetName().c_str();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataDivision::CDlgConfigureDataDivision(ARBConfigDivision* div)
	: CDlgConfigureData()
	, m_Div(div)
{
	m_Div->AddRef();
}

CDlgConfigureDataDivision::~CDlgConfigureDataDivision()
{
	m_Div->Release();
}

CString CDlgConfigureDataDivision::OnNeedText(int iColumn) const
{
	return m_Div->GetName().c_str();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataLevel::CDlgConfigureDataLevel(ARBConfigDivision* div, ARBConfigLevel* level)
	: CDlgConfigureData()
	, m_Division(div)
	, m_Level(level)
{
	m_Division->AddRef();
	m_Level->AddRef();
}

CDlgConfigureDataLevel::~CDlgConfigureDataLevel()
{
	m_Division->Release();
	m_Level->Release();
}

CString CDlgConfigureDataLevel::OnNeedText(int iColumn) const
{
	return m_Level->GetName().c_str();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataSubLevel::CDlgConfigureDataSubLevel(ARBConfigDivision* div, ARBConfigLevel* level, ARBConfigSubLevel* subLevel)
	: CDlgConfigureData()
	, m_Division(div)
	, m_Level(level)
	, m_SubLevel(subLevel)
{
	m_Division->AddRef();
	m_Level->AddRef();
	m_SubLevel->AddRef();
}

CDlgConfigureDataSubLevel::~CDlgConfigureDataSubLevel()
{
	m_Division->Release();
	m_Level->Release();
	m_SubLevel->Release();
}

CString CDlgConfigureDataSubLevel::OnNeedText(int iColumn) const
{
	return m_SubLevel->GetName().c_str();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataTitle::CDlgConfigureDataTitle(ARBConfigDivision* div, ARBConfigTitle* title)
	: CDlgConfigureData()
	, m_Division(div)
	, m_Title(title)
{
	m_Division->AddRef();
	m_Title->AddRef();
}

CDlgConfigureDataTitle::~CDlgConfigureDataTitle()
{
	m_Division->Release();
	m_Title->Release();
}

CString CDlgConfigureDataTitle::OnNeedText(int iColumn) const
{
	return m_Title->GetCompleteName().c_str();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataEvent::CDlgConfigureDataEvent(ARBConfigEvent* event)
	: CDlgConfigureData()
	, m_Event(event)
{
	m_Event->AddRef();
}

CDlgConfigureDataEvent::~CDlgConfigureDataEvent()
{
	m_Event->Release();
}

CString CDlgConfigureDataEvent::OnNeedText(int iColumn) const
{
	return m_Event->GetName().c_str();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataScoring::CDlgConfigureDataScoring(ARBConfigEvent* event, ARBConfigScoring* scoring)
	: CDlgConfigureData()
	, m_Event(event)
	, m_Scoring(scoring)
{
	m_Event->AddRef();
	m_Scoring->AddRef();
}

CDlgConfigureDataScoring::~CDlgConfigureDataScoring()
{
	m_Event->Release();
	m_Scoring->Release();
}

CString CDlgConfigureDataScoring::OnNeedText(int iColumn) const
{
	CString str, div, level;
	if (m_Scoring->GetDivision() == WILDCARD_DIVISION)
		div.LoadString(IDS_ALL);
	else
		div = m_Scoring->GetDivision().c_str();
	if (m_Scoring->GetLevel() == WILDCARD_LEVEL)
		level.LoadString(IDS_ALL);
	else
		level = m_Scoring->GetLevel().c_str();
	str.FormatMessage(IDS_SCORING_NAME,
		div,
		level,
		m_Scoring->GetScoringStyleStr().c_str());
	std::string validStr = m_Scoring->GetValidDateString();
	if (0 < validStr.length())
	{
		str += ' ';
		str += validStr.c_str();
	}
	return str;
}
