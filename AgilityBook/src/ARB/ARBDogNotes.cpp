/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief The classes that make up the dog's information.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-12 DRC Clearing the metadata encoded a 0-length string
 *                causing the program to think it still had metadata.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-03-03 DRC Added CRCDMetaFile, string of Windows Enhanced Metafile.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-09-17 DRC Save would not write data if only CRCD contained info.
 */

#include "stdafx.h"
#include "ARB/ARBDogNotes.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBConfig.h"
#include "ARBCommon/ARBBase64.h"
#include "ARBCommon/BinaryData.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

namespace
{
	class ARBMetaData_concrete : public ARBMetaData
	{
	public:
		ARBMetaData_concrete() {}
	};
};


ARBMetaDataPtr ARBMetaData::MetaData()
{
	return std::make_shared<ARBMetaData_concrete>();
}


ARBMetaData::ARBMetaData()
	: m_Bytes(0)
	, m_Data(NULL)
{
}


ARBMetaData::~ARBMetaData()
{
	Clear();
}


void ARBMetaData::Clear()
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


size_t ARBDogNotes::GetSearchStrings(std::set<std::wstring>& ioStrings) const
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
	assert(inTree);
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
			std::wstring tmp = element->GetValue();
			unsigned char* data;
			size_t bytes;
			if (ARBBase64::Decode(tmp, data, bytes))
			{
				if (!BinaryData::Encode(data, bytes, m_CRCDMeta))
					m_CRCDMeta.erase();
			}
			ARBBase64::Release(data);
		}
		else if (element->GetName() == TREE_OTHER)
		{
			m_Note = element->GetValue();
		}
	}
	// Fix a bug where clearing the metadata still encoded an empty string.
	if (inVersion < ARBVersion(12,9))
	{
		ARBMetaDataPtr data = ARBMetaData::MetaData();
		if (!BinaryData::Decode(m_CRCDMeta, data->m_Data, data->m_Bytes))
			m_CRCDMeta.erase();
		else
		{
			if (data->m_Bytes == 0 || !*data->m_Data)
				m_CRCDMeta.erase();
		}
	}

	return true;
}


bool ARBDogNotes::Save(ElementNodePtr ioTree) const
{
	assert(ioTree);
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
	if (!BinaryData::Decode(m_CRCDMeta, data->m_Data, data->m_Bytes))
		data->Clear();
	return data;
}


void ARBDogNotes::SetCRCDMetaData(unsigned char const* inCRCDMeta, size_t inBytes)
{
	BinaryData::Encode(inCRCDMeta, inBytes, m_CRCDMeta);
}
