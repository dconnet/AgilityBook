/*
 * Copyright © 2004-2007 David Connet. All Rights Reserved.
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
 * Revision History
 * @li 2006-12-03 DRC Complete re-write of action class.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
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

IConfigActionCallback::IConfigActionCallback()
	: m_bContinue(true)
{
}

IConfigActionCallback::~IConfigActionCallback()
{
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigAction::ARBConfigAction()
{
}

ARBConfigAction::~ARBConfigAction()
{
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigActionPtr ARBConfigActionRenameOtherPoints::New(
		ARBString const& inOldName,
		ARBString const& inNewName)
{
	return ARBConfigActionPtr(new ARBConfigActionRenameOtherPoints(
		inOldName,
		inNewName));
}

ARBConfigActionRenameOtherPoints::ARBConfigActionRenameOtherPoints(
		ARBString const& inOldName,
		ARBString const& inNewName)
	: m_OldName(inOldName)
	, m_NewName(inNewName)
{
}

ARBConfigActionRenameOtherPoints::ARBConfigActionRenameOtherPoints(ARBConfigActionRenameOtherPoints const& rhs)
	: m_OldName(rhs.m_OldName)
	, m_NewName(rhs.m_NewName)
{
}

ARBConfigActionPtr ARBConfigActionRenameOtherPoints::Clone() const
{
	return ARBConfigActionPtr(new ARBConfigActionRenameOtherPoints(m_OldName, m_NewName));
}

bool ARBConfigActionRenameOtherPoints::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		ARBostringstream& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	bool bChanged = false;
	ARBConfigOtherPointsPtr oldPoints;
	if (ioConfig.GetOtherPoints().FindOtherPoints(m_OldName, &oldPoints))
	{
		bChanged = true;
		ioInfo << _T("Action: Renaming OtherPoints [")
			<< m_OldName
			<< _T("] to [")
			<< m_NewName
			<< _T("]");
		int nPoints = 0;
		if (ioDogs)
			nPoints = ioDogs->NumOtherPointsInUse(m_OldName);
		if (0 < nPoints && ioDogs)
		{
			ioInfo << _T(", updating ") << nPoints << _T(" OtherPoints");
			ioDogs->RenameOtherPoints(m_OldName, m_NewName);
		}
		ioInfo << _T("\n");
		// If the new event exists, just delete the old.
		// Otherwise, rename the old to new.
		if (ioConfig.GetOtherPoints().FindOtherPoints(m_NewName))
			ioConfig.GetOtherPoints().DeleteOtherPoints(m_OldName);
		else
			oldPoints->SetName(m_NewName);
	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigActionPtr ARBConfigActionDeleteOtherPoints::New(
		ARBString const& inName)
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteOtherPoints(inName));
}

ARBConfigActionDeleteOtherPoints::ARBConfigActionDeleteOtherPoints(
		ARBString const& inName)
	: m_Name(inName)
{
}

ARBConfigActionDeleteOtherPoints::ARBConfigActionDeleteOtherPoints(ARBConfigActionDeleteOtherPoints const& rhs)
	: m_Name(rhs.m_Name)
{
}

ARBConfigActionPtr ARBConfigActionDeleteOtherPoints::Clone() const
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteOtherPoints(m_Name));
}

bool ARBConfigActionDeleteOtherPoints::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		ARBostringstream& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	bool bChanged = false;
	ARBConfigOtherPointsPtr oldPoints;
	if (ioConfig.GetOtherPoints().FindOtherPoints(m_Name, &oldPoints))
	{
		int nPoints = 0;
		if (ioDogs)
			nPoints = ioDogs->NumOtherPointsInUse(m_Name);
		if (0 < nPoints && ioDogs)
		{
			ARBostringstream tmp;
			tmp << _T("DELETING: OtherPoints [")
				<< m_Name
				<< _T("] ")
				<< nPoints
				<< _T(" item(s)");
			ARBString msg = tmp.str();
			ioCallBack.PreDelete(msg);
			if (!ioCallBack.CanContinue())
				return bChanged;
			ioInfo << msg << _T("\n");
			ioDogs->DeleteOtherPoints(m_Name);
		}
		bChanged = true;
		ioInfo << _T("Action: Deleting OtherPoints [")
			<< m_Name
			<< _T("]\n");
		ioConfig.GetOtherPoints().DeleteOtherPoints(m_Name);

	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigActionPtr ARBConfigActionRenameVenue::New(
		ARBString const& inOldName,
		ARBString const& inNewName)
{
	return ARBConfigActionPtr(new ARBConfigActionRenameVenue(inOldName, inNewName));
}

ARBConfigActionRenameVenue::ARBConfigActionRenameVenue(
		ARBString const& inOldName,
		ARBString const& inNewName)
	: m_OldName(inOldName)
	, m_NewName(inNewName)
{
}

ARBConfigActionRenameVenue::ARBConfigActionRenameVenue(ARBConfigActionRenameVenue const& rhs)
	: m_OldName(rhs.m_OldName)
	, m_NewName(rhs.m_NewName)
{
}

ARBConfigActionPtr ARBConfigActionRenameVenue::Clone() const
{
	return ARBConfigActionPtr(new ARBConfigActionRenameVenue(m_OldName, m_NewName));
}

bool ARBConfigActionRenameVenue::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		ARBostringstream& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	bool bChanged = false;
	ARBConfigVenuePtr oldVenue;
	if (ioConfig.GetVenues().FindVenue(m_OldName, &oldVenue))
	{
		bChanged = true;
		ioInfo << _T("Action: Renaming Venue [")
			<< m_OldName
			<< _T("] to [")
			<< m_NewName
			<< _T("]");
		int nVenues = 0;
		if (ioDogs)
		{
			nVenues = ioDogs->NumExistingPointsInVenue(m_OldName)
				+ ioDogs->NumRegNumsInVenue(m_OldName)
				+ ioDogs->NumTitlesInVenue(m_OldName)
				+ ioDogs->NumTrialsInVenue(m_OldName);
		}
		if (0 < nVenues && ioDogs)
		{
			ioInfo << _T(", updating ") << nVenues << _T(" related item(s)");
			ioDogs->RenameVenue(m_OldName, m_NewName);
		}
		ioInfo << _T("\n");
		// If the new venue exists, just delete the old.
		// Otherwise, rename the old to new.
		if (ioConfig.GetVenues().FindVenue(m_NewName))
			ioConfig.GetVenues().DeleteVenue(m_OldName);
		else
			oldVenue->SetName(m_NewName);
	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigActionPtr ARBConfigActionDeleteVenue::New(
		ARBString const& inName)
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteVenue(inName));
}

ARBConfigActionDeleteVenue::ARBConfigActionDeleteVenue(
		ARBString const& inName)
	: m_Name(inName)
{
}

ARBConfigActionDeleteVenue::ARBConfigActionDeleteVenue(ARBConfigActionDeleteVenue const& rhs)
	: m_Name(rhs.m_Name)
{
}

ARBConfigActionPtr ARBConfigActionDeleteVenue::Clone() const
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteVenue(m_Name));
}

bool ARBConfigActionDeleteVenue::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		ARBostringstream& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	bool bChanged = false;
	ARBConfigVenuePtr oldVenue;
	if (ioConfig.GetVenues().FindVenue(m_Name, &oldVenue))
	{
		int nVenues = 0;
		if (ioDogs)
		{
			nVenues = ioDogs->NumExistingPointsInVenue(m_Name)
				+ ioDogs->NumRegNumsInVenue(m_Name)
				+ ioDogs->NumTitlesInVenue(m_Name)
				+ ioDogs->NumTrialsInVenue(m_Name);
		}
		if (0 < nVenues && ioDogs)
		{
			ARBostringstream tmp;
			tmp << _T("DELETING: Venue [")
				<< m_Name
				<< _T("] ")
				<< nVenues
				<< _T(" item(s)");
			ARBString msg = tmp.str();
			ioCallBack.PreDelete(msg);
			if (!ioCallBack.CanContinue())
				return bChanged;
			ioInfo << msg << _T("\n");
			ioDogs->DeleteVenue(m_Name);
		}
		bChanged = true;
		ioInfo << _T("Action: Deleting Venue [")
			<< m_Name
			<< _T("]\n");
		ioConfig.GetVenues().DeleteVenue(m_Name);
	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigActionPtr ARBConfigActionRenameMultiQ::New(
		ARBString const& inVenue,
		ARBString const& inOldName,
		ARBString const& inNewName)
{
	return ARBConfigActionPtr(new ARBConfigActionRenameMultiQ(inVenue, inOldName, inNewName));
}

ARBConfigActionRenameMultiQ::ARBConfigActionRenameMultiQ(
		ARBString const& inVenue,
		ARBString const& inOldName,
		ARBString const& inNewName)
	: m_Venue(inVenue)
	, m_OldName(inOldName)
	, m_NewName(inNewName)
{
}

ARBConfigActionRenameMultiQ::ARBConfigActionRenameMultiQ(ARBConfigActionRenameMultiQ const& rhs)
	: m_Venue(rhs.m_Venue)
	, m_OldName(rhs.m_OldName)
	, m_NewName(rhs.m_NewName)
{
}

ARBConfigActionPtr ARBConfigActionRenameMultiQ::Clone() const
{
	return ARBConfigActionPtr(new ARBConfigActionRenameMultiQ(m_Venue, m_OldName, m_NewName));
}

bool ARBConfigActionRenameMultiQ::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		ARBostringstream& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	bool bChanged = false;
	ARBConfigVenuePtr venue;
	if (ioConfig.GetVenues().FindVenue(m_Venue, &venue))
	{
		ARBConfigMultiQPtr oldMultiQ;
		if (venue->GetMultiQs().FindMultiQ(m_OldName, false, &oldMultiQ))
		{
			bChanged = true;
			ioInfo << _T("Action: Renaming ")
				<< m_Venue
				<< _T(" MultiQ [")
				<< m_OldName
				<< _T("] to [")
				<< m_NewName
				<< _T("]");
			int nMultiQs = 0;
			if (ioDogs)
				nMultiQs = ioDogs->NumMultiQsInUse(m_Venue, m_OldName);
			if (0 < nMultiQs && ioDogs)
			{
				ioInfo << _T(", updating ") << nMultiQs << _T(" MultiQ(s)");
				ioDogs->RenameMultiQs(m_Venue, m_OldName, m_NewName);
			}
			ioInfo << _T("\n");
			// If the new multi exists, just delete the old.
			// Otherwise, rename the old to new.
			if (venue->GetMultiQs().FindMultiQ(m_NewName, false))
				venue->GetMultiQs().DeleteMultiQ(oldMultiQ);
			else
				oldMultiQ->SetName(m_NewName);
		}
	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigActionPtr ARBConfigActionDeleteMultiQ::New(
		ARBString const& inVenue,
		ARBString const& inName)
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteMultiQ(inVenue, inName));
}

ARBConfigActionDeleteMultiQ::ARBConfigActionDeleteMultiQ(
		ARBString const& inVenue,
		ARBString const& inName)
	: m_Venue(inVenue)
	, m_Name(inName)
{
}

ARBConfigActionDeleteMultiQ::ARBConfigActionDeleteMultiQ(ARBConfigActionDeleteMultiQ const& rhs)
	: m_Venue(rhs.m_Venue)
	, m_Name(rhs.m_Name)
{
}

ARBConfigActionPtr ARBConfigActionDeleteMultiQ::Clone() const
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteMultiQ(m_Venue, m_Name));
}

bool ARBConfigActionDeleteMultiQ::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		ARBostringstream& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	bool bChanged = false;
	ARBConfigVenuePtr venue;
	if (ioConfig.GetVenues().FindVenue(m_Venue, &venue))
	{
		ARBConfigMultiQPtr oldMultiQ;
		if (venue->GetMultiQs().FindMultiQ(m_Name, false, &oldMultiQ))
		{
			int nMultiQs = 0;
			if (ioDogs)
				nMultiQs = ioDogs->NumMultiQsInUse(m_Venue, m_Name);
			if (0 < nMultiQs && ioDogs)
			{
				ARBostringstream tmp;
				tmp << _T("DELETING: ")
					<< m_Venue
					<< _T(" MultiQ [")
					<< m_Name
					<< _T("] ")
					<< nMultiQs
					<< _T(" item(s)");
				ARBString msg = tmp.str();
				ioCallBack.PreDelete(msg);
				if (!ioCallBack.CanContinue())
					return bChanged;
				ioInfo << msg << _T("\n");
			}
			bChanged = true;
			ioInfo << _T("Action: Deleting MultiQ [")
				<< m_Name
				<< _T("]\n");
			venue->GetMultiQs().DeleteMultiQ(oldMultiQ);
			// Note: The multiQs in the dogs will be fixed at the very end
			// in ARBAgiltyRecordBook::Update
		}
	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigActionPtr ARBConfigActionRenameDivision::New(
		ARBString const& inVenue,
		ARBString const& inOldName,
		ARBString const& inNewName)
{
	return ARBConfigActionPtr(new ARBConfigActionRenameDivision(
		inVenue,
		inOldName,
		inNewName));
}

ARBConfigActionRenameDivision::ARBConfigActionRenameDivision(
		ARBString const& inVenue,
		ARBString const& inOldName,
		ARBString const& inNewName)
	: m_Venue(inVenue)
	, m_OldName(inOldName)
	, m_NewName(inNewName)
{
}

ARBConfigActionRenameDivision::ARBConfigActionRenameDivision(ARBConfigActionRenameDivision const& rhs)
	: m_Venue(rhs.m_Venue)
	, m_OldName(rhs.m_OldName)
	, m_NewName(rhs.m_NewName)
{
}

ARBConfigActionPtr ARBConfigActionRenameDivision::Clone() const
{
	return ARBConfigActionPtr(new ARBConfigActionRenameDivision(m_Venue, m_OldName, m_NewName));
}

bool ARBConfigActionRenameDivision::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		ARBostringstream& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	bool bChanged = false;

	// Find the venue.
	ARBConfigVenuePtr venue;
	if (ioConfig.GetVenues().FindVenue(m_Venue, &venue))
	{
		ARBConfigDivisionPtr oldDiv;
		if (venue->GetDivisions().FindDivision(m_OldName, &oldDiv))
		{
			bChanged = true;
			ioInfo << _T("Action: Renaming ")
				<< m_Venue
				<< _T(" Division [")
				<< m_OldName
				<< _T("] to [")
				<< m_NewName
				<< _T("]");
			// If the division is in use, create a fixup action.
			int nRuns = 0;
			if (ioDogs)
				nRuns = ioDogs->NumRunsInDivision(venue, m_OldName);
			if (0 < nRuns && ioDogs)
			{
				ioInfo << _T(", updating ") << nRuns << _T(" runs");
				ioDogs->RenameDivision(venue, m_OldName, m_NewName);
			}
			ioInfo << _T("\n");
			// If the new division exists, just delete the old.
			// Otherwise, rename the old to new.
			if (venue->GetDivisions().FindDivision(m_NewName))
				venue->GetDivisions().DeleteDivision(m_OldName, venue->GetEvents());
			else
				oldDiv->SetName(m_NewName);
		}
	}

	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigActionPtr ARBConfigActionDeleteDivision::New(
		ARBString const& inVenue,
		ARBString const& inName)
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteDivision(inVenue, inName));
}

ARBConfigActionDeleteDivision::ARBConfigActionDeleteDivision(
		ARBString const& inVenue,
		ARBString const& inName)
	: m_Venue(inVenue)
	, m_Name(inName)
{
}

ARBConfigActionDeleteDivision::ARBConfigActionDeleteDivision(ARBConfigActionDeleteDivision const& rhs)
	: m_Venue(rhs.m_Venue)
	, m_Name(rhs.m_Name)
{
}

ARBConfigActionPtr ARBConfigActionDeleteDivision::Clone() const
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteDivision(m_Venue, m_Name));
}

bool ARBConfigActionDeleteDivision::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		ARBostringstream& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	bool bChanged = false;
	ARBConfigVenuePtr venue;
	if (ioConfig.GetVenues().FindVenue(m_Venue, &venue))
	{
		ARBConfigDivisionPtr oldDiv;
		if (venue->GetDivisions().FindDivision(m_Name, &oldDiv))
		{
			int nRuns = 0;
			int nPoints = 0;
			if (ioDogs)
			{
				nRuns = ioDogs->NumRunsInDivision(venue, m_Name);
				nPoints = ioDogs->NumExistingPointsInDivision(venue, m_Name);
			}
			if (0 < nRuns + nPoints && ioDogs)
			{
				ARBostringstream tmp;
				tmp << _T("DELETING: ")
					<< m_Venue
					<< _T(" Division [")
					<< m_Name
					<< _T("] ")
					<< nRuns + nPoints
					<< _T(" runs and existing points");
				ARBString msg = tmp.str();
				ioCallBack.PreDelete(msg);
				if (!ioCallBack.CanContinue())
					return bChanged;
				ioInfo << msg << _T("\n");
				ioDogs->DeleteDivision(ioConfig, m_Venue, m_Name);
			}
			bChanged = true;
			ioInfo << _T("Action: Deleting ")
				<< m_Venue
				<< _T(" Division [")
				<< m_Name
				<< _T("]\n");
			if (venue->GetDivisions().DeleteDivision(m_Name, venue->GetEvents()))
				venue->GetMultiQs().DeleteDivision(m_Name);
		}
	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigActionPtr ARBConfigActionRenameLevel::NewLevel(
		ARBString const& inVenue,
		ARBString const& inDiv,
		ARBString const& inOldName,
		ARBString const& inNewName)
{
	return ARBConfigActionPtr(new ARBConfigActionRenameLevel(inVenue, inDiv, ARBString(), inOldName, inNewName));
}

ARBConfigActionPtr ARBConfigActionRenameLevel::NewSubLevel(
		ARBString const& inVenue,
		ARBString const& inDiv,
		ARBString const& inLevel,
		ARBString const& inOldName,
		ARBString const& inNewName)
{
	return ARBConfigActionPtr(new ARBConfigActionRenameLevel(inVenue, inDiv, inLevel, inOldName, inNewName));
}

ARBConfigActionRenameLevel::ARBConfigActionRenameLevel(
		ARBString const& inVenue,
		ARBString const& inDiv,
		ARBString const& inLevel,
		ARBString const& inOldName,
		ARBString const& inNewName)
	: m_Venue(inVenue)
	, m_Div(inDiv)
	, m_OldName(inOldName)
	, m_NewName(inNewName)
{
}

ARBConfigActionRenameLevel::ARBConfigActionRenameLevel(ARBConfigActionRenameLevel const& rhs)
	: m_Venue(rhs.m_Venue)
	, m_Div(rhs.m_Div)
	, m_Level(rhs.m_Level)
	, m_OldName(rhs.m_OldName)
	, m_NewName(rhs.m_NewName)
{
}

ARBConfigActionPtr ARBConfigActionRenameLevel::Clone() const
{
	return ARBConfigActionPtr(new ARBConfigActionRenameLevel(m_Venue, m_Div, m_Level, m_OldName, m_NewName));
}

bool ARBConfigActionRenameLevel::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		ARBostringstream& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	bool bChanged = false;
	ARBConfigVenuePtr venue;
	if (ioConfig.GetVenues().FindVenue(m_Venue, &venue))
	{
		ARBConfigDivisionPtr div;
		if (venue->GetDivisions().FindDivision(m_Div, &div))
		{
			ARBConfigLevelPtr level;
			bool bLeaf = false;
			bool bOk = false;
			// If level is not empty, we're dealing with a sublevel
			if (m_Level.empty())
			{
				bOk = div->GetLevels().FindLevel(m_OldName, &level);
				if (bOk)
					bLeaf = (0 == level->GetSubLevels().size());
			}
			else
			{
				bOk = div->GetLevels().FindLevel(m_Level, &level);
				bLeaf = true;
			}
			if (bOk)
			{
				bChanged = true;
				ioInfo << _T("Action: Renaming ")
					<< m_Venue
					<< _T(" Level [")
					<< m_OldName
					<< _T("] to [")
					<< m_NewName
					<< _T("]");
				int nRuns = 0;
				// Note: Only run the update on leaf nodes
				if (ioDogs && bLeaf)
				{
					nRuns += ioDogs->NumLevelsInUse(m_Venue, m_Div, m_OldName);
					ioInfo << _T(", updating ") << nRuns << _T(" runs");
					ioDogs->RenameLevel(m_Venue, m_Div, m_OldName, m_NewName);
				}
				ioInfo << _T("\n");
				// Only update when the actual level (not sublevel) changes
				if (m_Level.empty())
					venue->GetEvents().RenameLevel(m_OldName, m_NewName);
				// Only update on leafs
				if (bLeaf)
					venue->GetMultiQs().RenameLevel(m_Div, m_OldName, m_NewName);
			}
		}
	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigActionPtr ARBConfigActionDeleteLevel::NewLevel(
		ARBString const& inVenue,
		ARBString const& inDiv,
		ARBString const& inName)
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteLevel(inVenue, inDiv, ARBString(), inName));
}

ARBConfigActionPtr ARBConfigActionDeleteLevel::NewSubLevel(
		ARBString const& inVenue,
		ARBString const& inDiv,
		ARBString const& inLevel,
		ARBString const& inName)
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteLevel(inVenue, inDiv, inLevel, inName));
}

ARBConfigActionDeleteLevel::ARBConfigActionDeleteLevel(
		ARBString const& inVenue,
		ARBString const& inDiv,
		ARBString const& inLevel,
		ARBString const& inName)
	: m_Venue(inVenue)
	, m_Div(inDiv)
	, m_Level(inLevel)
	, m_Name(inName)
{
}

ARBConfigActionDeleteLevel::ARBConfigActionDeleteLevel(ARBConfigActionDeleteLevel const& rhs)
	: m_Venue(rhs.m_Venue)
	, m_Div(rhs.m_Div)
	, m_Level(rhs.m_Level)
	, m_Name(rhs.m_Name)
{
}

ARBConfigActionPtr ARBConfigActionDeleteLevel::Clone() const
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteLevel(m_Venue, m_Div, m_Level, m_Name));
}

bool ARBConfigActionDeleteLevel::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		ARBostringstream& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	bool bChanged = false;
	ARBConfigVenuePtr venue;
	if (ioConfig.GetVenues().FindVenue(m_Venue, &venue))
	{
		ARBConfigDivisionPtr div;
		if (venue->GetDivisions().FindDivision(m_Div, &div))
		{
			// Accumulate all sublevels that will be deleted
			ARBConfigLevelPtr level;
			std::vector<ARBString> sublevels;
			if (m_Level.empty())
			{
				if (div->GetLevels().FindLevel(m_Name, &level))
				{
					if (0 < level->GetSubLevels().size())
					{
						for (ARBConfigSubLevelList::const_iterator i = level->GetSubLevels().begin();
							i != level->GetSubLevels().end();
							++i)
						{
							sublevels.push_back((*i)->GetName());
						}
					}
					else
						sublevels.push_back(m_Name);
				}
			}
			else
			{
				div->GetLevels().FindLevel(m_Level, &level);
				sublevels.push_back(m_Name);
			}
			int nRuns = 0;
			if (ioDogs)
			{
				for (std::vector<ARBString>::iterator i = sublevels.begin();
					i != sublevels.end();
					++i)
				{
					nRuns += ioDogs->NumLevelsInUse(m_Venue, m_Div, *i);
				}
			}
			if (0 < nRuns && ioDogs)
			{
				ARBostringstream tmp;
				tmp << _T("DELETING: ")
					<< m_Venue
					<< _T(" Level [")
					<< m_Name
					<< _T("] ")
					<< nRuns
					<< _T(" runs");
				ARBString msg = tmp.str();
				ioCallBack.PreDelete(msg);
				if (!ioCallBack.CanContinue())
					return bChanged;
				ioInfo << msg << _T("\n");
				for (std::vector<ARBString>::iterator i = sublevels.begin();
					i != sublevels.end();
					++i)
				{
					ioDogs->DeleteLevel(m_Venue, m_Div, *i);
				}
			}
			bChanged = true;
			ioInfo << _T("Action: Deleting ")
				<< m_Venue
				<< _T(" Level [")
				<< m_Name
				<< _T("]\n");
			// Deleting level
			if (m_Level.empty())
			{
				if (div->GetLevels().DeleteLevel(m_Name, venue->GetEvents()))
					venue->GetMultiQs().DeleteLevel(m_Name);
			}
			// Deleting sublevel
			else
			{
				bool bLevelModified = false;
				if (div->GetLevels().DeleteSubLevel(m_Name, bLevelModified))
				{
					venue->GetMultiQs().DeleteLevel(m_Name);
					// Note, if deleting the sublevel caused the level's name
					// to change, just leave it. It causes more trouble to
					// try modifing the name to the old sublevel name.
				}
			}
		}
	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigActionPtr ARBConfigActionRenameTitle::New(
		ARBString const& inVenue,
		ARBString const& inOldName,
		ARBString const& inNewName)
{
	return ARBConfigActionPtr(new ARBConfigActionRenameTitle(inVenue, inOldName, inNewName));
}

ARBConfigActionRenameTitle::ARBConfigActionRenameTitle(
		ARBString const& inVenue,
		ARBString const& inOldName,
		ARBString const& inNewName)
	: m_Venue(inVenue)
	, m_OldName(inOldName)
	, m_NewName(inNewName)
{
}

ARBConfigActionRenameTitle::ARBConfigActionRenameTitle(ARBConfigActionRenameTitle const& rhs)
	: m_Venue(rhs.m_Venue)
	, m_OldName(rhs.m_OldName)
	, m_NewName(rhs.m_NewName)
{
}

ARBConfigActionPtr ARBConfigActionRenameTitle::Clone() const
{
	return ARBConfigActionPtr(new ARBConfigActionRenameTitle(m_Venue, m_OldName, m_NewName));
}

bool ARBConfigActionRenameTitle::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		ARBostringstream& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	bool bChanged = false;

	// Find the venue.
	ARBConfigVenuePtr venue;
	if (ioConfig.GetVenues().FindVenue(m_Venue, &venue))
	{
		// Find the title we're renaming.
		ARBConfigTitlePtr oldTitle;
		if (venue->GetTitles().FindTitle(m_OldName, &oldTitle))
		{
			bChanged = true;
			// Note: If we are deleting/renaming a title due
			// to an error in the config (same title in multiple
			// divisions), all we can do is rename ALL of them.
			// There's no way to differentiate existing titles.
			ioInfo << _T("Action: Renaming ")
				<< m_Venue
				<< _T(" Title [")
				<< m_OldName
				<< _T("] to [")
				<< m_NewName
				<< _T("]");
			// If any titles are in use, create a fixup action.
			int nTitles = 0;
			if (ioDogs)
				nTitles = ioDogs->NumTitlesInUse(m_Venue, m_OldName);
			if (0 < nTitles && ioDogs)
			{
				ioInfo << _T(", updating ") << nTitles << _T(" titles");
				ioDogs->RenameTitle(m_Venue, m_OldName, m_NewName);
			}
			ioInfo << _T("\n");
			// If the new title exists, just delete the old.
			// Otherwise, rename the old to new.
			if (venue->GetTitles().FindTitle(m_NewName))
				venue->GetTitles().DeleteTitle(m_OldName);
			else
				oldTitle->SetName(m_NewName);
		}
	}

	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigActionPtr ARBConfigActionDeleteTitle::New(
		ARBString const& inVenue,
		ARBString const& inDiv,
		ARBString const& inOldName,
		ARBString const& inNewName)
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteTitle(inVenue, inDiv, inOldName, inNewName));
}

ARBConfigActionDeleteTitle::ARBConfigActionDeleteTitle(
		ARBString const& inVenue,
		ARBString const& inDiv,
		ARBString const& inOldName,
		ARBString const& inNewName)
	: m_Venue(inVenue)
	, m_Div(inDiv)
	, m_OldName(inOldName)
	, m_NewName(inNewName)
{
}

ARBConfigActionDeleteTitle::ARBConfigActionDeleteTitle(ARBConfigActionDeleteTitle const& rhs)
	: m_Venue(rhs.m_Venue)
	, m_Div(rhs.m_Div)
	, m_OldName(rhs.m_OldName)
	, m_NewName(rhs.m_NewName)
{
}

ARBConfigActionPtr ARBConfigActionDeleteTitle::Clone() const
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteTitle(m_Venue, m_Div, m_OldName, m_NewName));
}

bool ARBConfigActionDeleteTitle::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		ARBostringstream& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	bool bChanged = false;

	// Find the venue.
	ARBConfigVenuePtr venue;
	if (ioConfig.GetVenues().FindVenue(m_Venue, &venue))
	{
		// Find the title we're deleting.
		ARBConfigTitlePtr oldTitle;
		if (venue->GetTitles().FindTitle(m_OldName, &oldTitle))
		{
			int nTitles = 0;
			if (ioDogs)
				nTitles = ioDogs->NumTitlesInUse(m_Venue, m_OldName);
			// If any titles are in use, create a fixup action.
			if (0 < nTitles && ioDogs)
			{
				// Note: If we are deleting/renaming a title due
				// to an error in the config (same title in multiple
				// divisions), all we can do is rename ALL of them.
				// There's no way to differentiate existing titles.
				if (0 < m_NewName.length())
				{
					ioInfo << _T("Action: Renaming ")
						<< m_Venue
						<< _T(" Title [")
						<< m_OldName
						<< _T("] to [")
						<< m_NewName
						<< _T("], updating ")
						<< nTitles
						<< _T(" titles");
					ioDogs->RenameTitle(m_Venue, m_OldName, m_NewName);
				}
				else
				{
					ARBostringstream tmp;
					tmp << _T("DELETING: ")
						<< m_Venue
						<< _T(" Title [")
						<< m_OldName
						<< _T("] ")
						<< nTitles
						<< _T(" titles");
					ARBString msg = tmp.str();
					ioCallBack.PreDelete(msg);
					if (!ioCallBack.CanContinue())
						return bChanged;
					ioInfo << msg << _T("\n");
					ioDogs->DeleteTitle(m_Venue, m_OldName);
				}
			}
			bChanged = true;
			ioInfo << _T("Action: Deleting Title [")
				<< m_OldName
				<< _T("]\n");
			venue->GetTitles().DeleteTitle(m_OldName);
		}
	}

	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigActionPtr ARBConfigActionRenameEvent::New(
		ARBString const& inVenue,
		ARBString const& inOldName,
		ARBString const& inNewName)
{
	return ARBConfigActionPtr(new ARBConfigActionRenameEvent(inVenue, inOldName, inNewName));
}

ARBConfigActionRenameEvent::ARBConfigActionRenameEvent(
		ARBString const& inVenue,
		ARBString const& inOldName,
		ARBString const& inNewName)
	: m_Venue(inVenue)
	, m_OldName(inOldName)
	, m_NewName(inNewName)
{
}

ARBConfigActionRenameEvent::ARBConfigActionRenameEvent(ARBConfigActionRenameEvent const& rhs)
	: m_Venue(rhs.m_Venue)
	, m_OldName(rhs.m_OldName)
	, m_NewName(rhs.m_NewName)
{
}

ARBConfigActionPtr ARBConfigActionRenameEvent::Clone() const
{
	return ARBConfigActionPtr(new ARBConfigActionRenameEvent(m_Venue, m_OldName, m_NewName));
}

bool ARBConfigActionRenameEvent::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		ARBostringstream& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	bool bChanged = false;

	ARBConfigVenuePtr venue;
	if (ioConfig.GetVenues().FindVenue(m_Venue, &venue))
	{
		ARBConfigEventPtr oldEvent;
		if (venue->GetEvents().FindEvent(m_OldName, &oldEvent))
		{
			bChanged = true;
			ioInfo << _T("Action: Renaming ")
				<< m_Venue
				<< _T(" Event [")
				<< m_OldName
				<< _T("] to [")
				<< m_NewName
				<< _T("]");
			// If any events are in use, create a fixup action.
			int nEvents = 0;
			if (ioDogs)
				nEvents = ioDogs->NumEventsInUse(m_Venue, m_OldName);
			if (0 < nEvents && ioDogs)
			{
				ioInfo << _T(", updating ") << nEvents << _T(" events");
				ioDogs->RenameEvent(m_Venue, m_OldName, m_NewName);
			}
			ioInfo << _T("\n");
			// If the new event exists, just delete the old.
			// Otherwise, rename the old to new.
			if (venue->GetEvents().FindEvent(m_NewName))
				venue->GetEvents().DeleteEvent(m_OldName);
			else
				oldEvent->SetName(m_NewName);
		}
	}

	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigActionPtr ARBConfigActionDeleteEvent::New(
		ARBString const& inVenue,
		ARBString const& inName)
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteEvent(inVenue, inName));
}

ARBConfigActionDeleteEvent::ARBConfigActionDeleteEvent(
		ARBString const& inVenue,
		ARBString const& inName)
	: m_Venue(inVenue)
	, m_Name(inName)
{
}

ARBConfigActionDeleteEvent::ARBConfigActionDeleteEvent(ARBConfigActionDeleteEvent const& rhs)
	: m_Venue(rhs.m_Venue)
	, m_Name(rhs.m_Name)
{
}

ARBConfigActionPtr ARBConfigActionDeleteEvent::Clone() const
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteEvent(m_Venue, m_Name));
}

bool ARBConfigActionDeleteEvent::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		ARBostringstream& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	bool bChanged = false;

	ARBConfigVenuePtr venue;
	if (ioConfig.GetVenues().FindVenue(m_Venue, &venue))
	{
		ARBConfigEventPtr oldEvent;
		if (venue->GetEvents().FindEvent(m_Name, &oldEvent))
		{
			bChanged = true;
			int nEvents = 0;
			if (ioDogs)
				nEvents = ioDogs->NumEventsInUse(m_Venue, m_Name);
			// If any events are in use, create a fixup action.
			if (0 < nEvents && ioDogs)
			{
				ARBostringstream tmp;
				tmp << _T("DELETING: ")
					<< m_Venue
					<< _T(" Event [")
					<< m_Name
					<< _T("] ")
					<< nEvents
					<< _T(" event(s)");
				ARBString msg = tmp.str();
				ioCallBack.PreDelete(msg);
				if (!ioCallBack.CanContinue())
					return bChanged;
				ioInfo << msg << _T("\n");
				ioDogs->DeleteEvent(m_Venue, m_Name);
			}
			ioInfo << _T("Action: Deleting Event [")
				<< m_Name
				<< _T("]\n");
			venue->GetMultiQs().DeleteEvent(m_Name);
			venue->GetEvents().DeleteEvent(m_Name);
		}
	}

	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigActionList::Load(
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBString verb;
	if (Element::eFound != inTree.GetAttrib(ATTRIB_ACTION_VERB, verb)
	|| 0 == verb.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_ACTION, ATTRIB_ACTION_VERB));
		return false;
	}
	ARBString venue, div, oldName, newName;
	inTree.GetAttrib(ATTRIB_ACTION_VENUE, venue);
	inTree.GetAttrib(ATTRIB_ACTION_DIVISION, div);
	inTree.GetAttrib(ATTRIB_ACTION_OLDNAME, oldName);
	inTree.GetAttrib(ATTRIB_ACTION_NEWNAME, newName);

	ARBConfigActionPtr item;
	if (ACTION_VERB_DELETE_TITLE == verb)
	{
		item = ARBConfigActionDeleteTitle::New(venue, div, oldName, newName);
	}
	else if (ACTION_VERB_RENAME_TITLE == verb)
	{
		item = ARBConfigActionRenameTitle::New(venue, oldName, newName);
	}
	else if (ACTION_VERB_DELETE_EVENT == verb)
	{
		item = ARBConfigActionDeleteEvent::New(venue, oldName);
	}
	else if (ACTION_VERB_RENAME_EVENT == verb)
	{
		item = ARBConfigActionRenameEvent::New(venue, oldName, newName);
	}
	else if (ACTION_VERB_RENAME_DIV == verb)
	{
		item = ARBConfigActionRenameDivision::New(venue, oldName, newName);
	}
	bool bOk = false;
	if (item)
	{
		bOk = true;
		push_back(item);
	}
	return bOk;
}

int ARBConfigActionList::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		ARBostringstream& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	int nChanges = 0;
	for (ARBConfigActionList::const_iterator iterAction = begin();
			iterAction != end();
			++iterAction)
	{
		if ((*iterAction)->Apply(ioConfig, ioDogs, ioInfo, ioCallBack))
			++nChanges;
		if (!ioCallBack.CanContinue())
			break;
	}
	if (0 < nChanges)
		ioInfo << _T("\n");
	return nChanges;
}
