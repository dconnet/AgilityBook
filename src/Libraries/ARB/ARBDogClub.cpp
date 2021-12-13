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
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-12-27 Changed FindEvent to take a date.
 * 2003-11-26 Changed version number to a complex value.
 */

#include "stdafx.h"
#include "ARB/ARBDogTrial.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBConfig.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/ARBDate.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

namespace
{
class ARBDogClub_concrete : public ARBDogClub
{
public:
	ARBDogClub_concrete()
	{
	}
	ARBDogClub_concrete(ARBDogClub const& rhs)
		: ARBDogClub(rhs)
	{
	}
};
}; // namespace


ARBDogClubPtr ARBDogClub::New()
{
	return std::make_shared<ARBDogClub_concrete>();
}


ARBDogClub::ARBDogClub()
	: m_Name()
	, m_Venue()
	, m_PrimaryClubVenue()
	, m_PrimaryClub()
{
}


ARBDogClub::ARBDogClub(ARBDogClub const& rhs)
	: m_Name(rhs.m_Name)
	, m_Venue(rhs.m_Venue)
	, m_PrimaryClubVenue(rhs.m_PrimaryClubVenue)
	, m_PrimaryClub(rhs.m_PrimaryClub)
{
}


ARBDogClub::ARBDogClub(ARBDogClub&& rhs)
	: m_Name(std::move(rhs.m_Name))
	, m_Venue(std::move(rhs.m_Venue))
	, m_PrimaryClubVenue(std::move(rhs.m_PrimaryClubVenue))
	, m_PrimaryClub(std::move(rhs.m_PrimaryClub))
{
}


ARBDogClub::~ARBDogClub()
{
}


ARBDogClubPtr ARBDogClub::Clone() const
{
	return std::make_shared<ARBDogClub_concrete>(*this);
}


ARBDogClub& ARBDogClub::operator=(ARBDogClub const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_Venue = rhs.m_Venue;
		m_PrimaryClubVenue = rhs.m_PrimaryClubVenue;
		m_PrimaryClub = rhs.m_PrimaryClub;
	}
	return *this;
}


ARBDogClub& ARBDogClub::operator=(ARBDogClub&& rhs)
{
	if (this != &rhs)
	{
		m_Name = std::move(rhs.m_Name);
		m_Venue = std::move(rhs.m_Venue);
		m_PrimaryClubVenue = std::move(rhs.m_PrimaryClubVenue);
		m_PrimaryClub = std::move(rhs.m_PrimaryClub);
	}
	return *this;
}


bool ARBDogClub::operator==(ARBDogClub const& rhs) const
{
	// Equality is only name/venue, not cosanctioning.
	return m_Name == rhs.m_Name && m_Venue == rhs.m_Venue;
}


size_t ARBDogClub::GetSearchStrings(std::set<std::wstring>& ioStrings) const
{
	size_t nItems = 0;

	if (0 < m_Name.length())
	{
		ioStrings.insert(m_Name);
		++nItems;
	}

	if (0 < m_Venue.length())
	{
		ioStrings.insert(m_Venue);
		++nItems;
	}

	return nItems;
}


bool ARBDogClub::Load(
	ARBConfig const& inConfig,
	ElementNodePtr const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_CLUB)
		return false;
	if (inVersion == ARBVersion(1, 0))
	{
		if (ARBAttribLookup::Found != inTree->GetAttrib(L"Name", m_Name) || 0 == m_Name.length())
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_CLUB, L"Name"));
			return false;
		}
	}
	else
		m_Name = inTree->GetValue();

	if (ARBAttribLookup::Found != inTree->GetAttrib(ATTRIB_CLUB_VENUE, m_Venue) || 0 == m_Venue.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_CLUB, ATTRIB_CLUB_VENUE));
		return false;
	}

	inTree->GetAttrib(ATTRIB_CLUB_PRIMARY, m_PrimaryClubVenue);

	if (!inConfig.GetVenues().VerifyVenue(m_Venue))
	{
		std::wstring msg(Localization()->InvalidVenueName());
		msg += m_Venue;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_CLUB, ATTRIB_CLUB_VENUE, msg.c_str()));
		return false;
	}

	return true;
}


bool ARBDogClub::PostLoad(ARBDogClubList const& clubList, ARBErrorCallback& ioCallback)
{
	bool bOk = true;
	if (!m_PrimaryClubVenue.empty())
	{
		ARBDogClubPtr club;
		if (clubList.GetPrimaryClub(m_PrimaryClubVenue, &club))
		{
			m_PrimaryClub = club;
		}
		else
		{
			bOk = false;
			// The linked cosanctioning venue isn't in the trial list.
			ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_CLUB, ATTRIB_CLUB_PRIMARY));
		}
		m_PrimaryClubVenue.clear();
	}
	return bOk;
}


bool ARBDogClub::Save(ElementNodePtr const& ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr club = ioTree->AddElementNode(TREE_CLUB);
	club->AddAttrib(ATTRIB_CLUB_VENUE, m_Venue);
	ARBDogClubPtr pClub = m_PrimaryClub.lock();
	if (pClub)
		club->AddAttrib(ATTRIB_CLUB_PRIMARY, pClub->GetVenue());
	if (0 < m_Name.length())
		club->SetValue(m_Name);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBDogClubList::Load(
	ARBConfig const& inConfig,
	ElementNodePtr const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	ARBDogClubPtr thing(ARBDogClub::New());
	if (!thing->Load(inConfig, inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


bool ARBDogClubList::PostLoad(ARBErrorCallback& ioCallback)
{
	bool bOk = true;
	for (auto item : *this)
	{
		// Continue on error
		if (!item->PostLoad(*this, ioCallback))
			bOk = false;
	}
	return bOk;
}


ARBDogClubPtr ARBDogClubList::GetMainClub() const
{
	ARBDogClubPtr club;
#pragma PRAGMA_TODO(Maybe add a user defined main club)
	if (size() > 0)
		club = at(0);
	return club;
}


bool ARBDogClubList::GetPrimaryClub(std::wstring const& inVenue, ARBDogClubPtr* outClub) const
{
	if (inVenue.empty())
		return false;
	for (auto item : *this)
	{
		if (item->GetPrimaryClub())
			continue;
		if (item->GetVenue() == inVenue)
		{
			if (outClub)
				*outClub = item;
			return true;
		}
	}
	return false;
}


std::wstring ARBDogClubList::GetClubList(bool bNames, bool bIncCosanction) const
{
	std::set<std::wstring> dedup;
	int i = 0;
	fmt::wmemory_buffer buf;
	for (ARBDogClubList::const_iterator iter = begin(); iter != end(); ++iter)
	{
		if (!bIncCosanction && (*iter)->GetPrimaryClub())
			continue;
		std::wstring str = bNames ? (*iter)->GetName() : (*iter)->GetVenue();
		if (dedup.find(str) == dedup.end())
		{
			dedup.insert(str);
			if (0 < i)
				fmt::format_to(std::back_inserter(buf), L"{}", L"/");
			++i;
			fmt::format_to(std::back_inserter(buf), L"{}", str);
		}
	}
	return fmt::to_string(buf);
}


std::wstring ARBDogClubList::GetClubList(bool bNames, ARBDogRunPtr inRun) const
{
	if (!inRun || !inRun->GetClub())
		return std::wstring();
	fmt::wmemory_buffer buf;
	std::wstring str = bNames ? inRun->GetClub()->GetName() : inRun->GetClub()->GetVenue();
	fmt::format_to(std::back_inserter(buf), L"{}", str);
	ARBDogClubPtr pPrimary = FindCoSanctioningClub(inRun->GetClub());
	if (pPrimary)
	{
		str = bNames ? pPrimary->GetName() : pPrimary->GetVenue();
		fmt::format_to(std::back_inserter(buf), L"/{}", str);
	}

	return fmt::to_string(buf);
}


ARBDogClubPtr ARBDogClubList::FindCoSanctioningClub(ARBDogClubPtr inClub) const
{
	if (!inClub->GetPrimaryClub())
	{
		for (const_iterator iter = begin(); iter != end(); ++iter)
		{
			if (*iter == inClub)
				continue;
			ARBDogClubPtr pClub = (*iter)->GetPrimaryClub();
			if (pClub && *pClub == *inClub)
				return *iter;
		}
	}
	return ARBDogClubPtr();
}


bool ARBDogClubList::FindClubIndex(ARBDogClubPtr const& inClub, size_t& outIndex) const
{
	assert(!!inClub);
	if (!inClub)
		return false;
	for (size_t i = 0; i < size(); ++i)
	{
		if (*inClub == *(*this)[i])
		{
			outIndex = i;
			return true;
		}
	}
	return false;
}


bool ARBDogClubList::FindClub(std::wstring const& inName, std::wstring const& inVenue, ARBDogClubPtr* outClub) const
{
	if (outClub)
		outClub->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName && (*iter)->GetVenue() == inVenue)
		{
			if (outClub)
				*outClub = *iter;
			return true;
		}
	}
	return false;
}


bool ARBDogClubList::FindEvent(
	ARBConfig const& inConfig,
	std::wstring const& inEvent,
	std::wstring const& inDivision,
	std::wstring const& inLevel,
	ARBDate const& inDate,
	ARBErrorCallback& ioCallback,
	ARBConfigEventPtr* outEvent,
	ARBConfigScoringPtr* outScoring) const
{
	if (outEvent)
		outEvent->reset();
	if (outScoring)
		outScoring->reset();
	ARBConfigEventPtr pEvent;
	ARBConfigScoringPtr pScoring;
	for (const_iterator iter = begin(); !pScoring && iter != end(); ++iter)
	{
		if (inConfig.GetVenues()
				.FindEvent((*iter)->GetVenue(), inEvent, inDivision, inLevel, inDate, &pEvent, &pScoring))
			break;
	}
	bool bFound = false;
	if (pScoring)
	{
		bFound = true;
		if (outEvent)
			*outEvent = pEvent;
		if (outScoring)
			*outScoring = pScoring;
	}
	else
	{
		std::wstring msg(Localization()->InvalidEvent());
		msg += inEvent;
		msg += L" (";
		msg += inDivision;
		msg += L"/";
		msg += inLevel;
		msg += L")";
		for (const_iterator iter = begin(); !pScoring && iter != end(); ++iter)
		{
			msg += L"\n";
			msg += (*iter)->GetName();
			msg += L" [";
			msg += (*iter)->GetVenue();
			msg += L"]";
		}
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_RUN, ATTRIB_RUN_EVENT, msg.c_str()));
	}
	return bFound;
}


bool ARBDogClubList::FindVenue(std::wstring const& inVenue, ARBDogClubPtr* outClub) const
{
	if (outClub)
		outClub->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue)
		{
			if (outClub)
				*outClub = *iter;
			return true;
		}
	}
	return false;
}


bool ARBDogClubList::AddClub(std::wstring const& inName, std::wstring const& inVenue, ARBDogClubPtr* outClub)
{
	if (FindClub(inName, inVenue))
		return false;
	ARBDogClubPtr pClub(ARBDogClub::New());
	pClub->SetName(inName);
	pClub->SetVenue(inVenue);
	push_back(pClub);
	if (outClub)
		*outClub = pClub;
	return true;
}


bool ARBDogClubList::DeleteClub(std::wstring const& inName, std::wstring const& inVenue)
{
	std::wstring name(inName);
	std::wstring venue(inVenue);
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == name && (*iter)->GetVenue() == venue)
		{
			for (iterator iter2 = begin(); iter2 != end(); ++iter2)
			{
				ARBDogClubPtr pPrimary = (*iter2)->GetPrimaryClub();
				if (pPrimary && *pPrimary == *(*iter))
					(*iter2)->SetPrimaryClub(ARBDogClubPtr());
			}
			erase(iter);
			return true;
		}
	}
	return false;
}
