#pragma once

/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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
 * @li 2004-09-11 DRC Fix runs when scoring methods change.
 * @li 2004-03-26 DRC Added code to migrate runs to the new table-in-run form.
 */

#include <string>
class ARBAgilityRecordBook;

/////////////////////////////////////////////////////////////////////////////

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
	CDlgFixupRenameVenue(std::string const& oldName, std::string const& newName)
		: m_oldName(oldName)
		, m_newName(newName)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	std::string m_oldName;
	std::string m_newName;
};

class CDlgFixupDeleteVenue : public CDlgFixup
{
public:
	CDlgFixupDeleteVenue(std::string const& name)
		: m_Name(name)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	std::string m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgFixupRenameOtherPoints : public CDlgFixup
{
public:
	CDlgFixupRenameOtherPoints(std::string const& oldName, std::string const& newName)
		: m_oldName(oldName)
		, m_newName(newName)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	std::string m_oldName;
	std::string m_newName;
};

class CDlgFixupDeleteOtherPoints : public CDlgFixup
{
public:
	CDlgFixupDeleteOtherPoints(std::string const& name)
		: m_Name(name)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	std::string m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgFixupRenameDivision : public CDlgFixup
{
public:
	CDlgFixupRenameDivision(std::string const& venue, std::string const& oldName, std::string const& newName)
		: m_Venue(venue)
		, m_oldName(oldName)
		, m_newName(newName)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	std::string m_Venue;
	std::string m_oldName;
	std::string m_newName;
};

class CDlgFixupDeleteDivision : public CDlgFixup
{
public:
	CDlgFixupDeleteDivision(std::string const& venue, std::string const& name)
		: m_Venue(venue)
		, m_Name(name)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	std::string m_Venue;
	std::string m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgFixupRenameLevel : public CDlgFixup
{
public:
	CDlgFixupRenameLevel(
		std::string const& venue,
		std::string const& div,
		std::string const& oldName,
		std::string const& newName)
		: m_Venue(venue)
		, m_Div(div)
		, m_oldName(oldName)
		, m_newName(newName)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	std::string m_Venue;
	std::string m_Div;
	std::string m_oldName;
	std::string m_newName;
};

class CDlgFixupDeleteLevel : public CDlgFixup
{
public:
	CDlgFixupDeleteLevel(
		std::string const& venue,
		std::string const& div,
		std::string const& name)
		: m_Venue(venue)
		, m_Div(div)
		, m_Name(name)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	std::string m_Venue;
	std::string m_Div;
	std::string m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgFixupRenameTitle : public CDlgFixup
{
public:
	CDlgFixupRenameTitle(std::string const& venue, std::string const& oldName, std::string const& newName)
		: m_Venue(venue)
		, m_oldName(oldName)
		, m_newName(newName)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	std::string m_Venue;
	std::string m_oldName;
	std::string m_newName;
};

class CDlgFixupDeleteTitle : public CDlgFixup
{
public:
	CDlgFixupDeleteTitle(std::string const& venue, std::string const& name)
		: m_Venue(venue)
		, m_Name(name)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	std::string m_Venue;
	std::string m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgFixupRenameEvent : public CDlgFixup
{
public:
	CDlgFixupRenameEvent(std::string const& venue, std::string const& oldName, std::string const& newName)
		: m_Venue(venue)
		, m_oldName(oldName)
		, m_newName(newName)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	std::string m_Venue;
	std::string m_oldName;
	std::string m_newName;
};

class CDlgFixupDeleteEvent : public CDlgFixup
{
public:
	CDlgFixupDeleteEvent(std::string const& venue, std::string const& name)
		: m_Venue(venue)
		, m_Name(name)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	std::string m_Venue;
	std::string m_Name;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * This will delete and fixup existing runs when the scoring methods of an
 * event changes in a configuration.
 */
class CDlgFixupEventScoring : public CDlgFixup
{
public:
	CDlgFixupEventScoring(std::string const& inVenue,
		std::string const& inEvent)
		: m_Venue(inVenue)
		, m_Event(inEvent)
	{
	}
	virtual void Commit(ARBAgilityRecordBook& book);
protected:
	std::string m_Venue;
	std::string m_Event;
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
