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
 * @brief The classes that make up the dog's information.
 * @author David Connet
 *
 * Revision History
 * @li 2004-03-03 DRC Added CRCDMetaFile, string of Windows Enhanced Metafile.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-09-17 DRC Save would not write data if only CRCD contained info.
 */

#include "StdAfx.h"
#include "ARBDogNotes.h"

#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBDogNotes::ARBDogNotes()
	: m_Faults()
	, m_CRCD()
	, m_CRCDMeta()
	, m_Note()
{
}

ARBDogNotes::ARBDogNotes(const ARBDogNotes& rhs)
	: m_Faults(rhs.m_Faults)
	, m_CRCD(rhs.m_CRCD)
	, m_CRCDMeta(rhs.m_CRCDMeta)
	, m_Note(rhs.m_Note)
{
}

ARBDogNotes::~ARBDogNotes()
{
}

ARBDogNotes& ARBDogNotes::operator=(const ARBDogNotes& rhs)
{
	if (this != &rhs)
	{
		m_Faults = rhs.m_Faults;
		m_CRCD = rhs.m_CRCD;
		m_CRCDMeta = rhs.m_CRCDMeta;
		m_Note = rhs.m_Note;
	}
	return *this;
}

bool ARBDogNotes::operator==(const ARBDogNotes& rhs) const
{
	return m_Faults == rhs.m_Faults
		&& m_CRCD == rhs.m_CRCD
		&& m_CRCDMeta == rhs.m_CRCDMeta
		&& m_Note == rhs.m_Note;
}

bool ARBDogNotes::operator!=(const ARBDogNotes& rhs) const
{
	return !operator==(rhs);
}

size_t ARBDogNotes::GetSearchStrings(std::set<std::string>& ioStrings) const
{
	size_t nItems = 0;
	for (ARBDogFaultList::const_iterator iter = m_Faults.begin(); iter != m_Faults.end(); ++iter)
	{
		ioStrings.insert(*iter);
		++nItems;
	}
	if (0 < m_Note.length())
	{
		ioStrings.insert(m_Note);
		++nItems;
	}
	return nItems;
}

bool ARBDogNotes::Load(
	const ARBConfig& inConfig,
	const Element& inTree,
	const ARBVersion& inVersion,
	std::string& ioErrMsg)
{
	for (int i = 0; i < inTree.GetElementCount(); ++i)
	{
		const Element& element = inTree.GetElement(i);
		if (element.GetName() == TREE_FAULTS)
		{
			m_Faults.push_back(element.GetValue());
		}
		else if (element.GetName() == TREE_CRCD)
		{
			m_CRCD = element.GetValue();
		}
		else if (element.GetName() == TREE_CRCD_META)
		{
			m_CRCDMeta = element.GetValue();
		}
		else if (element.GetName() == TREE_OTHER)
		{
			m_Note = element.GetValue();
		}
	}
	return true;
}

bool ARBDogNotes::Save(Element& ioTree) const
{
	if (0 < m_Faults.size()
	|| 0 < m_CRCD.length()
	|| 0 < m_CRCDMeta.length()
	|| 0 < m_Note.length())
	{
		Element& notes = ioTree.AddElement(TREE_NOTES);
		for (ARBDogFaultList::const_iterator iter = m_Faults.begin(); iter != m_Faults.end(); ++iter)
		{
			Element& element = notes.AddElement(TREE_FAULTS);
			element.SetValue((*iter));
		}
		if (0 < m_CRCD.length())
		{
			Element& element = notes.AddElement(TREE_CRCD);
			element.SetValue(m_CRCD);
		}
		if (0 < m_CRCDMeta.length())
		{
			Element& element = notes.AddElement(TREE_CRCD_META);
			element.SetValue(m_CRCDMeta);
		}
		if (0 < m_Note.length())
		{
			Element& element = notes.AddElement(TREE_OTHER);
			element.SetValue(m_Note);
		}
	}
	return true;
}
