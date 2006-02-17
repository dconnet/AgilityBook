#pragma once

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
 * @brief Data classes to support DlgConfigure operations.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-09-11 DRC Fix runs when scoring methods change.
 * @li 2004-03-26 DRC Added code to migrate runs to the new table-in-run form.
 */

#include "ARBTypes.h"
class ARBAgilityRecordBook;

/**
 * This class and its derivatives are used to fix up data on 'ok'. When things
 * are edited/deleted, we may need to clean up existing data.
 */
class CDlgFixup
{
public:
	CDlgFixup() {}
	~CDlgFixup() {}
	virtual void Commit(ARBAgilityRecordBook& book) = 0;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgFixupRenameVenue : public CDlgFixup
{
public:
	CDlgFixupRenameVenue(
			ARBString const& oldName,
			ARBString const& newName)
		: m_oldName(oldName)
		, m_newName(newName)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	ARBString m_oldName;
	ARBString m_newName;
};

class CDlgFixupDeleteVenue : public CDlgFixup
{
public:
	CDlgFixupDeleteVenue(ARBString const& name)
		: m_Name(name)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	ARBString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgFixupRenameOtherPoints : public CDlgFixup
{
public:
	CDlgFixupRenameOtherPoints(
			ARBString const& oldName,
			ARBString const& newName)
		: m_oldName(oldName)
		, m_newName(newName)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	ARBString m_oldName;
	ARBString m_newName;
};

class CDlgFixupDeleteOtherPoints : public CDlgFixup
{
public:
	CDlgFixupDeleteOtherPoints(ARBString const& name)
		: m_Name(name)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	ARBString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgFixupRenameMultiQ : public CDlgFixup
{
public:
	CDlgFixupRenameMultiQ(
			ARBString const& inVenue,
			ARBString const& oldName,
			ARBString const& newName)
		: m_Venue(inVenue)
		, m_oldName(oldName)
		, m_newName(newName)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	ARBString m_Venue;
	ARBString m_oldName;
	ARBString m_newName;
};

class CDlgFixupDeleteMultiQ : public CDlgFixup
{
public:
	CDlgFixupDeleteMultiQ(ARBString const& inVenue)
		: m_Venue(inVenue)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	ARBString m_Venue;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgFixupRenameDivision : public CDlgFixup
{
public:
	CDlgFixupRenameDivision(
			ARBString const& venue,
			ARBString const& oldName,
			ARBString const& newName)
		: m_Venue(venue)
		, m_oldName(oldName)
		, m_newName(newName)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	ARBString m_Venue;
	ARBString m_oldName;
	ARBString m_newName;
};

class CDlgFixupDeleteDivision : public CDlgFixup
{
public:
	CDlgFixupDeleteDivision(
			ARBString const& venue,
			ARBString const& name)
		: m_Venue(venue)
		, m_Name(name)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	ARBString m_Venue;
	ARBString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgFixupRenameLevel : public CDlgFixup
{
public:
	CDlgFixupRenameLevel(
			ARBString const& venue,
			ARBString const& div,
			ARBString const& oldName,
			ARBString const& newName)
		: m_Venue(venue)
		, m_Div(div)
		, m_oldName(oldName)
		, m_newName(newName)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	ARBString m_Venue;
	ARBString m_Div;
	ARBString m_oldName;
	ARBString m_newName;
};

class CDlgFixupDeleteLevel : public CDlgFixup
{
public:
	CDlgFixupDeleteLevel(
			ARBString const& venue,
			ARBString const& div,
			ARBString const& name)
		: m_Venue(venue)
		, m_Div(div)
		, m_Name(name)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	ARBString m_Venue;
	ARBString m_Div;
	ARBString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgFixupRenameTitle : public CDlgFixup
{
public:
	CDlgFixupRenameTitle(
			ARBString const& venue,
			ARBString const& oldName,
			ARBString const& newName)
		: m_Venue(venue)
		, m_oldName(oldName)
		, m_newName(newName)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	ARBString m_Venue;
	ARBString m_oldName;
	ARBString m_newName;
};

class CDlgFixupDeleteTitle : public CDlgFixup
{
public:
	CDlgFixupDeleteTitle(
			ARBString const& venue,
			ARBString const& name)
		: m_Venue(venue)
		, m_Name(name)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	ARBString m_Venue;
	ARBString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgFixupRenameEvent : public CDlgFixup
{
public:
	CDlgFixupRenameEvent(
			ARBString const& venue,
			ARBString const& oldName,
			ARBString const& newName)
		: m_Venue(venue)
		, m_oldName(oldName)
		, m_newName(newName)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	ARBString m_Venue;
	ARBString m_oldName;
	ARBString m_newName;
};

class CDlgFixupDeleteEvent : public CDlgFixup
{
public:
	CDlgFixupDeleteEvent(
			ARBString const& venue,
			ARBString const& name)
		: m_Venue(venue)
		, m_Name(name)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	ARBString m_Venue;
	ARBString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * This will delete and fixup existing runs when the scoring methods of an
 * event changes in a configuration.
 */
class CDlgFixupEventScoring : public CDlgFixup
{
public:
	CDlgFixupEventScoring(
			ARBString const& inVenue,
			ARBString const& inEvent)
		: m_Venue(inVenue)
		, m_Event(inEvent)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	ARBString m_Venue;
	ARBString m_Event;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgFixupTableInRuns : public CDlgFixup
{
public:
	CDlgFixupTableInRuns()
		: m_Updated(0)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
	int RunsUpdated() const	{return m_Updated;}
protected:
	int m_Updated;
};
