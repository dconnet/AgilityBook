/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
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
 * @brief implementation of the CDlgFixup classes
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgFixup.h"

#include "ARBAgilityRecordBook.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

void CDlgFixupRenameVenue::Commit(ARBAgilityRecordBook& book)
{
	book.GetDogs().RenameVenue(m_oldName, m_newName);
}

void CDlgFixupDeleteVenue::Commit(ARBAgilityRecordBook& book)
{
	book.GetDogs().DeleteVenue(m_Name);
}

/////////////////////////////////////////////////////////////////////////////

void CDlgFixupRenameOtherPoints::Commit(ARBAgilityRecordBook& book)
{
	book.GetDogs().RenameOtherPoints(m_oldName, m_newName);
}

void CDlgFixupDeleteOtherPoints::Commit(ARBAgilityRecordBook& book)
{
	book.GetDogs().DeleteOtherPoints(m_Name);
}

////////////////////////////////////////////////////////////////////////////

void CDlgFixupRenameDivision::Commit(ARBAgilityRecordBook& book)
{
	book.GetDogs().RenameDivision(book.GetConfig().GetVenues().FindVenue(m_Venue), m_oldName, m_newName);
}

void CDlgFixupDeleteDivision::Commit(ARBAgilityRecordBook& book)
{
	book.GetDogs().DeleteDivision(book.GetConfig(), m_Venue, m_Name);
}

////////////////////////////////////////////////////////////////////////////

void CDlgFixupRenameLevel::Commit(ARBAgilityRecordBook& book)
{
	book.GetDogs().RenameLevel(m_Venue, m_Div, m_oldName, m_newName);
}

void CDlgFixupDeleteLevel::Commit(ARBAgilityRecordBook& book)
{
	book.GetDogs().DeleteLevel(m_Venue, m_Div, m_Name);
}

////////////////////////////////////////////////////////////////////////////

void CDlgFixupRenameTitle::Commit(ARBAgilityRecordBook& book)
{
	book.GetDogs().RenameTitle(m_Venue, m_oldName, m_newName);
}

void CDlgFixupDeleteTitle::Commit(ARBAgilityRecordBook& book)
{
	book.GetDogs().DeleteTitle(m_Venue, m_Name);
}

////////////////////////////////////////////////////////////////////////////

void CDlgFixupRenameEvent::Commit(ARBAgilityRecordBook& book)
{
	book.GetDogs().RenameEvent(m_Venue, m_oldName, m_newName);
}

void CDlgFixupDeleteEvent::Commit(ARBAgilityRecordBook& book)
{
	book.GetDogs().DeleteEvent(m_Venue, m_Name);
}
