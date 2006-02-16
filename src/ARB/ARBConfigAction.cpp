/*
 * Copyright © 2004-2006 David Connet. All Rights Reserved.
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
 * @brief The classes that make up the configuration information.
 * @author David Connet
 *
 * This class is designed to handle changes in configuration data. For
 * instance, the 2004 USDAA rules have changed some of the abbreviations
 * for titles (PG -> PG3, PS -> PK3) in addition to adding new ones. To
 * maintain our data integrity, we need to update things to deal with this.
 *
 * Please note: These items will NOT be handled in the Update routines.
 * The calling program is responsible to do this first. (This is due to the
 * existing architecture - we may be updating the config in the config dialog,
 * which means any 'actions' are delayed until 'ok' - or we may be updating
 * via import, which means actions are live.)
 *
 * Revision History
 * @li 2005-10-26 DRC Added optional 'Div' to DeleteTitle.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-01-21 DRC Created
 */

#include "StdAfx.h"
#include "ARBConfigAction.h"

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfigAction::ARBConfigAction()
	: m_Verb()
	, m_Venue()
	, m_Div()
	, m_OldName()
	, m_NewName()
{
}

ARBConfigAction::ARBConfigAction(ARBConfigAction const& rhs)
	: m_Verb(rhs.m_Verb)
	, m_Venue(rhs.m_Venue)
	, m_Div(rhs.m_Div)
	, m_OldName(rhs.m_OldName)
	, m_NewName(rhs.m_NewName)
{
}

ARBConfigAction::~ARBConfigAction()
{
}

//static
ARBConfigActionPtr ARBConfigAction::New()
{
	return ARBConfigActionPtr(new ARBConfigAction());
}

ARBConfigActionPtr ARBConfigAction::Clone() const
{
	return ARBConfigActionPtr(new ARBConfigAction(*this));
}

ARBConfigAction& ARBConfigAction::operator=(ARBConfigAction const& rhs)
{
	if (this != &rhs)
	{
		m_Verb = rhs.m_Verb;
		m_Venue = rhs.m_Venue;
		m_Div = rhs.m_Div;
		m_OldName = rhs.m_OldName;
		m_NewName = rhs.m_NewName;
	}
	return *this;
}

bool ARBConfigAction::operator==(ARBConfigAction const& rhs) const
{
	return m_Verb == rhs.m_Verb
		&& m_Venue == rhs.m_Venue
		&& m_Div == rhs.m_Div
		&& m_OldName == rhs.m_OldName
		&& m_NewName == rhs.m_NewName;
}

bool ARBConfigAction::operator!=(ARBConfigAction const& rhs) const
{
	return !operator==(rhs);
}

ARBString ARBConfigAction::GetGenericName() const
{
	return m_Verb;
}

size_t ARBConfigAction::GetSearchStrings(std::set<ARBString>& ioStrings) const
{
	size_t nItems = 0;
	return nItems;
}

bool ARBConfigAction::Load(
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	if (Element::eFound != inTree.GetAttrib(ATTRIB_ACTION_VERB, m_Verb)
	|| 0 == m_Verb.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_ACTION, ATTRIB_ACTION_VERB));
		return false;
	}
	inTree.GetAttrib(ATTRIB_ACTION_VENUE, m_Venue);
	inTree.GetAttrib(ATTRIB_ACTION_DIVISION, m_Div);
	inTree.GetAttrib(ATTRIB_ACTION_OLDNAME, m_OldName);
	inTree.GetAttrib(ATTRIB_ACTION_NEWNAME, m_NewName);
	//Value is not currently used, though the DTD supports it.
	//m_Desc = inTree.GetValue();
	return true;
}

bool ARBConfigAction::Save(Element& ioTree) const
{
	Element& action = ioTree.AddElement(TREE_ACTION);
	action.AddAttrib(ATTRIB_ACTION_VERB, m_Verb);
	if (0 < m_Venue.length())
		action.AddAttrib(ATTRIB_ACTION_VENUE, m_Venue);
	if (0 < m_Div.length())
		action.AddAttrib(ATTRIB_ACTION_DIVISION, m_Div);
	if (0 < m_OldName.length())
		action.AddAttrib(ATTRIB_ACTION_OLDNAME, m_OldName);
	if (0 < m_NewName.length())
		action.AddAttrib(ATTRIB_ACTION_NEWNAME, m_NewName);
	return true;
}

ARBString const& ARBConfigAction::GetVerb() const
{
	return m_Verb;
}

ARBString const& ARBConfigAction::GetVenue() const
{
	return m_Venue;
}

ARBString const& ARBConfigAction::GetDivision() const
{
	return m_Div;
}

ARBString const& ARBConfigAction::GetOldName() const
{
	return m_OldName;
}

ARBString const& ARBConfigAction::GetNewName() const
{
	return m_NewName;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigActionList::Load(
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBConfigActionPtr thing(ARBConfigAction::New());
	if (!thing->Load(inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}
