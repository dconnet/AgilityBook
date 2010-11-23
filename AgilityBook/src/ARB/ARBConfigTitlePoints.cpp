/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief The classes that make up the configuration information.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-03-31 DRC Started adding auto-lifetime point accumulation.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-07-12 DRC Added as part of file version 5.
 */

#include "stdafx.h"
#include <algorithm>
#include "ARBConfigTitlePoints.h"

#include "ARBAgilityRecordBook.h"
#include "ARBConfigLifetimePoints.h"
#include "ARBLocalization.h"
#include "Element.h"

#if defined(_MFC_VER) && defined(_DEBUG)
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfigTitlePointsPtr ARBConfigTitlePoints::New()
{
	return ARBConfigTitlePointsPtr(new ARBConfigTitlePoints());
}


ARBConfigTitlePointsPtr ARBConfigTitlePoints::New(
		double inPoints,
		double inFaults)
{
	return ARBConfigTitlePointsPtr(new ARBConfigTitlePoints(inPoints, inFaults));
}


ARBConfigTitlePoints::ARBConfigTitlePoints()
	: m_Points(0.0)
	, m_Faults(0.0)
{
}


ARBConfigTitlePoints::ARBConfigTitlePoints(
		double inPoints,
		double inFaults)
	: m_Points(inPoints)
	, m_Faults(inFaults)
{
}


ARBConfigTitlePoints::ARBConfigTitlePoints(ARBConfigTitlePoints const& rhs)
	: m_Points(rhs.m_Points)
	, m_Faults(rhs.m_Faults)
{
}


ARBConfigTitlePoints::~ARBConfigTitlePoints()
{
}


ARBConfigTitlePointsPtr ARBConfigTitlePoints::Clone() const
{
	return ARBConfigTitlePointsPtr(new ARBConfigTitlePoints(*this));
}


ARBConfigTitlePoints& ARBConfigTitlePoints::operator=(ARBConfigTitlePoints const& rhs)
{
	if (this != &rhs)
	{
		m_Points = rhs.m_Points;
		m_Faults = rhs.m_Faults;
	}
	return *this;
}


bool ARBConfigTitlePoints::operator==(ARBConfigTitlePoints const& rhs) const
{
	return m_Points == rhs.m_Points
		&& m_Faults == rhs.m_Faults;
}


wxString ARBConfigTitlePoints::GetGenericName() const
{
	return Localization()->TitlePointsNameFormat(m_Points, m_Faults);
}


bool ARBConfigTitlePoints::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback,
		ARBConfigLifetimePointsList& ioLifetimePoints)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_TITLE_POINTS)
		return false;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_TITLE_POINTS_POINTS, m_Points))
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_TITLE_POINTS, ATTRIB_TITLE_POINTS_POINTS));
		return false;
	}
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_TITLE_POINTS_FAULTS, m_Faults))
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_TITLE_POINTS, ATTRIB_TITLE_POINTS_FAULTS));
		return false;
	}
	if (inVersion < ARBVersion(10,0))
	{
		bool bLifetime;
		if (ElementNode::eFound == inTree->GetAttrib(wxT("LifeTime"), bLifetime))
		{
			if (bLifetime)
			{
				ioLifetimePoints.AddLifetimePoints(m_Points, m_Faults);
			}
		}
	}
	return true;
}


bool ARBConfigTitlePoints::Save(ElementNodePtr ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr title = ioTree->AddElementNode(TREE_TITLE_POINTS);
	title->AddAttrib(ATTRIB_TITLE_POINTS_POINTS, m_Points, 0);
	title->AddAttrib(ATTRIB_TITLE_POINTS_FAULTS, m_Faults, 0);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigTitlePointsList::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback,
		ARBConfigLifetimePointsList& ioLifetimePoints)
{
	ARBConfigTitlePointsPtr thing(ARBConfigTitlePoints::New());
	if (!thing->Load(inTree, inVersion, ioCallback, ioLifetimePoints))
		return false;
	push_back(thing);
	return true;
}


class SortConfigTitlePoints
{
public:
	SortConfigTitlePoints() {}
	bool operator()(ARBConfigTitlePointsPtr one, ARBConfigTitlePointsPtr two) const
	{
		return one->GetFaults() < two->GetFaults();
	}
};


void ARBConfigTitlePointsList::sort()
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortConfigTitlePoints());
}


double ARBConfigTitlePointsList::GetTitlePoints(double inFaults) const
{
	// This is why we keep the list sorted!
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if (inFaults <= (*iter)->GetFaults())
			return (*iter)->GetPoints();
	}
	return 0;
}


bool ARBConfigTitlePointsList::FindTitlePoints(
		double inFaults,
		ARBConfigTitlePointsPtr* outTitle) const
{
	if (outTitle)
		outTitle->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if (ARBDouble::equal((*iter)->GetFaults(), inFaults))
		{
			if (outTitle)
				*outTitle = *iter;
			return true;
		}
	}
	return false;
}


bool ARBConfigTitlePointsList::AddTitlePoints(
		double inPoints,
		double inFaults,
		ARBConfigTitlePointsPtr* outTitle)
{
	if (outTitle)
		outTitle->reset();
	if (FindTitlePoints(inFaults))
		return false;
	ARBConfigTitlePointsPtr pTitle(ARBConfigTitlePoints::New(inPoints, inFaults));
	push_back(pTitle);
	sort();
	if (outTitle)
		*outTitle = pTitle;
	return true;
}


bool ARBConfigTitlePointsList::DeleteTitlePoints(double inFaults)
{
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if (ARBDouble::equal((*iter)->GetFaults(), inFaults))
		{
			erase(iter);
			return true;
		}
	}
	return false;
}