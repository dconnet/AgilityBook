/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief The classes that make up the dog's information.
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-12 Clearing the metadata encoded a 0-length string
 *            causing the program to think it still had metadata.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-03-03 Added CRCDMetaFile, string of Windows Enhanced Metafile.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-09-17 Save would not write data if only CRCD contained info.
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


namespace dconSoft
{
using namespace ARBCommon;
namespace ARB
{

namespace
{
class ARBMetaData_concrete : public ARBMetaData
{
public:
	ARBMetaData_concrete()
	{
	}
};
}; // namespace


ARBMetaDataPtr ARBMetaData::MetaData()
{
	return std::make_shared<ARBMetaData_concrete>();
}


ARBMetaData::ARBMetaData()
	: m_Data()
{
}


ARBMetaData::~ARBMetaData()
{
	Clear();
}


void ARBMetaData::Clear()
{
	m_Data.clear();
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


ARBDogNotes::ARBDogNotes(ARBDogNotes&& rhs)
	: m_Faults(std::move(rhs.m_Faults))
	, m_CRCD(std::move(rhs.m_CRCD))
	, m_CRCDMeta(std::move(rhs.m_CRCDMeta))
	, m_Note(std::move(rhs.m_Note))
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


ARBDogNotes& ARBDogNotes::operator=(ARBDogNotes&& rhs)
{
	if (this != &rhs)
	{
		m_Faults = std::move(rhs.m_Faults);
		m_CRCD = std::move(rhs.m_CRCD);
		m_CRCDMeta = std::move(rhs.m_CRCDMeta);
		m_Note = std::move(rhs.m_Note);
	}
	return *this;
}


bool ARBDogNotes::operator==(ARBDogNotes const& rhs) const
{
	// clang-format off
	return m_Faults == rhs.m_Faults
		&& m_CRCD == rhs.m_CRCD
		&& m_CRCDMeta == rhs.m_CRCDMeta
		&& m_Note == rhs.m_Note;
	// clang-format on
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
	ElementNodePtr const& inTree,
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
			std::vector<unsigned char> data;
			if (ARBBase64::Decode(tmp, data))
			{
				if (!BinaryData::Encode(data, m_CRCDMeta))
					m_CRCDMeta.clear();
			}
		}
		else if (element->GetName() == TREE_OTHER)
		{
			m_Note = element->GetValue();
		}
	}
	// Fix a bug where clearing the metadata still encoded an empty string.
	if (inVersion < ARBVersion(12, 9))
	{
		ARBMetaDataPtr data = ARBMetaData::MetaData();
		if (!BinaryData::Decode(m_CRCDMeta, data->m_Data))
			m_CRCDMeta.clear();
		else
		{
			if (data->m_Data.empty())
				m_CRCDMeta.clear();
		}
	}

	return true;
}


bool ARBDogNotes::Save(ElementNodePtr const& ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	if (0 < m_Faults.size() || 0 < m_CRCD.length() || 0 < m_CRCDMeta.length() || 0 < m_Note.length())
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
	if (!BinaryData::Decode(m_CRCDMeta, data->m_Data))
		data->Clear();
	return data;
}


void ARBDogNotes::SetCRCDMetaData(std::vector<unsigned char> const& inCRCDMeta)
{
	BinaryData::Encode(inCRCDMeta, m_CRCDMeta);
}

} // namespace ARB
} // namespace dconSoft
