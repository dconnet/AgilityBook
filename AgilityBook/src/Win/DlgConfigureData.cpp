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

CString CDlgConfigureDataFault::OnNeedText(int iColumn) const
{
	return m_Fault->GetName().c_str();
}

/////////////////////////////////////////////////////////////////////////////

CString CDlgConfigureDataOtherPoints::OnNeedText(int iColumn) const
{
	return m_OtherPoints->GetName().c_str();
}

/////////////////////////////////////////////////////////////////////////////

CString CDlgConfigureDataDivision::OnNeedText(int iColumn) const
{
	return m_Div->GetName().c_str();
}

/////////////////////////////////////////////////////////////////////////////

CString CDlgConfigureDataLevel::OnNeedText(int iColumn) const
{
	return m_Level->GetName().c_str();
}

/////////////////////////////////////////////////////////////////////////////

CString CDlgConfigureDataSubLevel::OnNeedText(int iColumn) const
{
	return m_SubLevel->GetName().c_str();
}

/////////////////////////////////////////////////////////////////////////////

CString CDlgConfigureDataTitle::OnNeedText(int iColumn) const
{
	return m_Title->GetCompleteName().c_str();
}

/////////////////////////////////////////////////////////////////////////////

CString CDlgConfigureDataEvent::OnNeedText(int iColumn) const
{
	return m_Event->GetName().c_str();
}

/////////////////////////////////////////////////////////////////////////////

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
