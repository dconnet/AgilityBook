/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
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
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-03-03 DRC Added CRCDMetaFile, string of Windows Enhanced Metafile.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-09-17 DRC Save would not write data if only CRCD contained info.
 */

#include "StdAfx.h"
#include "ARBDogNotes.h"

#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "ARBBase64.h"
#include "BinaryData.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBMetaDataPtr ARBMetaData::MetaData()
{
	return ARBMetaDataPtr(new ARBMetaData());
}


ARBMetaData::ARBMetaData()
	: m_Bytes(0)
	, m_Data(NULL)
{
}


ARBMetaData::~ARBMetaData()
{
	BinaryData::Release(m_Data);
}

/////////////////////////////////////////////////////////////////////////////

ARBDogNotes::ARBDogNotes()
	: m_Faults()
	, m_CRCD()
	, m_CRCDMeta()
	, m_Note()
{
}


ARBDogNotes::ARBDogNotes(ARBDogNotes const& rhs)
	: m_Faults(rhs.m_Faults)
	, m_CRCD(rhs.m_CRCD)
	, m_CRCDMeta(rhs.m_CRCDMeta)
	, m_Note(rhs.m_Note)
{
}


ARBDogNotes::~ARBDogNotes()
{
}


ARBDogNotes& ARBDogNotes::operator=(ARBDogNotes const& rhs)
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


bool ARBDogNotes::operator==(ARBDogNotes const& rhs) const
{
	return m_Faults == rhs.m_Faults
		&& m_CRCD == rhs.m_CRCD
		&& m_CRCDMeta == rhs.m_CRCDMeta
		&& m_Note == rhs.m_Note;
}


size_t ARBDogNotes::GetSearchStrings(std::set<tstring>& ioStrings) const
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
		ARBConfig const& inConfig,
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ASSERT(inTree);
	if (!inTree || inTree->GetName() != TREE_NOTES)
		return false;
	for (int i = 0; i < inTree->GetElementCount(); ++i)
	{
		ElementNodePtr element = inTree->GetElementNode(i);
		if (!element)
			continue;
		if (element->GetName() == TREE_FAULTS)
		{
			m_Faults.push_back(element->GetValue());
		}
		else if (element->GetName() == TREE_CRCD)
		{
			m_CRCD = element->GetValue();
		}
		else if (element->GetName() == TREE_CRCD_META2)
		{
			m_CRCDMeta = element->GetValue();
		}
		else if (element->GetName() == TREE_CRCD_META)
		{
			// Replaced by TREE_CRCD_META2, this translates.
			tstring tmp = element->GetValue();
			unsigned char* data;
			size_t bytes;
			ARBBase64::Decode(tmp, data, bytes);
			BinaryData::Encode(data, bytes, m_CRCDMeta);
			ARBBase64::Release(data);
		}
		else if (element->GetName() == TREE_OTHER)
		{
			m_Note = element->GetValue();
		}
	}
	return true;
}


bool ARBDogNotes::Save(ElementNodePtr ioTree) const
{
	ASSERT(ioTree);
	if (!ioTree)
		return false;
	if (0 < m_Faults.size()
	|| 0 < m_CRCD.length()
	|| 0 < m_CRCDMeta.length()
	|| 0 < m_Note.length())
	{
		ElementNodePtr notes = ioTree->AddElementNode(TREE_NOTES);
		for (ARBDogFaultList::const_iterator iter = m_Faults.begin(); iter != m_Faults.end(); ++iter)
		{
			if (0 < (*iter).length())
			{
				ElementNodePtr element = notes->AddElementNode(TREE_FAULTS);
				element->SetValue((*iter));
			}
		}
		if (0 < m_CRCD.length())
		{
			ElementNodePtr element = notes->AddElementNode(TREE_CRCD);
			element->SetValue(m_CRCD);
		}
		if (0 < m_CRCDMeta.length())
		{
			ElementNodePtr element = notes->AddElementNode(TREE_CRCD_META2);
			element->SetValue(m_CRCDMeta);
		}
		if (0 < m_Note.length())
		{
			ElementNodePtr element = notes->AddElementNode(TREE_OTHER);
			element->SetValue(m_Note);
		}
	}
	return true;
}


ARBMetaDataPtr ARBDogNotes::GetCRCDMetaData() const
{
	ARBMetaDataPtr data = ARBMetaData::MetaData();
	BinaryData::Decode(m_CRCDMeta, data->m_Data, data->m_Bytes);
	return data;
}


void ARBDogNotes::SetCRCDMetaData(unsigned char const* inCRCDMeta, size_t inBytes)
{
	BinaryData::Encode(inCRCDMeta, inBytes, m_CRCDMeta);
}
