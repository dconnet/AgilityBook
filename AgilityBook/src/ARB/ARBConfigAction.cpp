/*
 * Copyright (c) 2004-2009 David Connet. All Rights Reserved.
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
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2006-12-03 DRC Complete re-write of action class.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-10-26 DRC Added optional 'Div' to DeleteTitle.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-01-21 DRC Created
 */

#include "stdafx.h"
#include "ARBConfigAction.h"

#include "ARBAgilityRecordBook.h"
#include "ARBLocalization.h"
#include "Element.h"

#if defined(_MFC_VER) && defined(_DEBUG)
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////

#define ACTION_VERB_DELETE_CALPLUGIN	wxT("DeleteCalPlugin")
#define ACTION_VERB_DELETE_TITLE		wxT("DeleteTitle")
#define ACTION_VERB_RENAME_TITLE		wxT("RenameTitle")
#define ACTION_VERB_DELETE_EVENT		wxT("DeleteEvent")
#define ACTION_VERB_RENAME_EVENT		wxT("RenameEvent")
#define ACTION_VERB_RENAME_DIV			wxT("RenameDivision")
#define ACTION_VERB_RENAME_VENUE		wxT("RenameVenue")

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

ARBConfigActionPtr ARBConfigActionDeleteCalPlugin::New(
		wxString const& inName)
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteCalPlugin(inName));
}


ARBConfigActionDeleteCalPlugin::ARBConfigActionDeleteCalPlugin(
		wxString const& inName)
	: m_Name(inName)
{
}


ARBConfigActionDeleteCalPlugin::ARBConfigActionDeleteCalPlugin(ARBConfigActionDeleteCalPlugin const& rhs)
	: m_Name(rhs.m_Name)
{
}


ARBConfigActionPtr ARBConfigActionDeleteCalPlugin::Clone() const
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteCalPlugin(m_Name));
}


bool ARBConfigActionDeleteCalPlugin::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		wxString& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	bool bChanged = false;
	if (ioConfig.GetCalSites().DeleteSite(m_Name))
	{
		bChanged = true;
		ioInfo << Localization()->ActionDeleteCalPlugin(m_Name) << wxT("\n");
	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigActionPtr ARBConfigActionRenameOtherPoints::New(
		wxString const& inOldName,
		wxString const& inNewName)
{
	return ARBConfigActionPtr(new ARBConfigActionRenameOtherPoints(
		inOldName,
		inNewName));
}


ARBConfigActionRenameOtherPoints::ARBConfigActionRenameOtherPoints(
		wxString const& inOldName,
		wxString const& inNewName)
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
		wxString& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	bool bChanged = false;
	ARBConfigOtherPointsPtr oldPoints;
	if (ioConfig.GetOtherPoints().FindOtherPoints(m_OldName, &oldPoints))
	{
		bChanged = true;
		int nPoints = 0;
		if (ioDogs)
			nPoints = ioDogs->NumOtherPointsInUse(m_OldName);
		ioInfo << Localization()->ActionRenameOtherPoints(m_OldName, m_NewName, nPoints) << wxT("\n");
		if (0 < nPoints && ioDogs)
		{
			ioDogs->RenameOtherPoints(m_OldName, m_NewName);
		}
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
		wxString const& inName)
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteOtherPoints(inName));
}


ARBConfigActionDeleteOtherPoints::ARBConfigActionDeleteOtherPoints(
		wxString const& inName)
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
		wxString& ioInfo,
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
			wxString msg = Localization()->ActionPreDeleteOtherPoints(m_Name, nPoints);
			ioCallBack.PreDelete(msg);
			if (!ioCallBack.CanContinue())
				return bChanged;
			ioInfo << msg << wxT("\n");
			ioDogs->DeleteOtherPoints(m_Name);
		}
		bChanged = true;
		ioInfo << Localization()->ActionDeleteOtherPoints(m_Name) << wxT("\n");
		ioConfig.GetOtherPoints().DeleteOtherPoints(m_Name);

	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigActionPtr ARBConfigActionRenameVenue::New(
		wxString const& inOldName,
		wxString const& inNewName)
{
	return ARBConfigActionPtr(new ARBConfigActionRenameVenue(inOldName, inNewName));
}


ARBConfigActionRenameVenue::ARBConfigActionRenameVenue(
		wxString const& inOldName,
		wxString const& inNewName)
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
		wxString& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	bool bChanged = false;
	ARBConfigVenuePtr oldVenue;
	if (ioConfig.GetVenues().FindVenue(m_OldName, &oldVenue))
	{
		bChanged = true;
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
			ioDogs->RenameVenue(m_OldName, m_NewName);
		}
		ioInfo << Localization()->ActionRenameVenue(m_OldName, m_NewName, nVenues) << wxT("\n");
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
		wxString const& inName)
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteVenue(inName));
}


ARBConfigActionDeleteVenue::ARBConfigActionDeleteVenue(
		wxString const& inName)
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
		wxString& ioInfo,
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
			wxString msg = Localization()->ActionPreDeleteVenue(m_Name, nVenues);
			ioCallBack.PreDelete(msg);
			if (!ioCallBack.CanContinue())
				return bChanged;
			ioInfo << msg << wxT("\n");
			ioDogs->DeleteVenue(m_Name);
		}
		bChanged = true;
		ioInfo << Localization()->ActionDeleteVenue(m_Name) << wxT("\n");
		ioConfig.GetVenues().DeleteVenue(m_Name);
	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigActionPtr ARBConfigActionRenameMultiQ::New(
		wxString const& inVenue,
		wxString const& inOldName,
		wxString const& inNewName)
{
	return ARBConfigActionPtr(new ARBConfigActionRenameMultiQ(inVenue, inOldName, inNewName));
}


ARBConfigActionRenameMultiQ::ARBConfigActionRenameMultiQ(
		wxString const& inVenue,
		wxString const& inOldName,
		wxString const& inNewName)
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
		wxString& ioInfo,
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
			int nMultiQs = 0;

			if (ioDogs)
				nMultiQs = ioDogs->NumMultiQsInUse(m_Venue, m_OldName);
			if (0 < nMultiQs && ioDogs)
			{
				ioDogs->RenameMultiQs(m_Venue, m_OldName, m_NewName);
			}
			ioInfo << Localization()->ActionRenameMultiQ(m_Venue, m_OldName, m_NewName, nMultiQs ) << wxT("\n");
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
		wxString const& inVenue,
		wxString const& inName)
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteMultiQ(inVenue, inName));
}


ARBConfigActionDeleteMultiQ::ARBConfigActionDeleteMultiQ(
		wxString const& inVenue,
		wxString const& inName)
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
		wxString& ioInfo,
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
				wxString msg = Localization()->ActionPreDeleteMultiQ(m_Venue, m_Name, nMultiQs);
				ioCallBack.PreDelete(msg);
				if (!ioCallBack.CanContinue())
					return bChanged;
				ioInfo << msg << wxT("\n");
			}
			bChanged = true;
			ioInfo << Localization()->ActionDeleteMultiQ(m_Venue, m_Name) << wxT("\n");
			venue->GetMultiQs().DeleteMultiQ(oldMultiQ);
			// Note: The multiQs in the dogs will be fixed at the very end
			// in ARBAgiltyRecordBook::Update
		}
	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigActionPtr ARBConfigActionRenameDivision::New(
		wxString const& inVenue,
		wxString const& inOldName,
		wxString const& inNewName)
{
	return ARBConfigActionPtr(new ARBConfigActionRenameDivision(
		inVenue,
		inOldName,
		inNewName));
}


ARBConfigActionRenameDivision::ARBConfigActionRenameDivision(
		wxString const& inVenue,
		wxString const& inOldName,
		wxString const& inNewName)
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
		wxString& ioInfo,
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
			// If the division is in use, create a fixup action.
			int nRuns = 0;
			if (ioDogs)
				nRuns = ioDogs->NumRunsInDivision(venue, m_OldName);
			if (0 < nRuns && ioDogs)
			{
				ioDogs->RenameDivision(venue, m_OldName, m_NewName);
			}
			ioInfo << Localization()->ActionRenameDivision(m_Venue, m_OldName, m_NewName,nRuns) << wxT("\n");
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
		wxString const& inVenue,
		wxString const& inName)
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteDivision(inVenue, inName));
}


ARBConfigActionDeleteDivision::ARBConfigActionDeleteDivision(
		wxString const& inVenue,
		wxString const& inName)
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
		wxString& ioInfo,
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
				wxString msg = Localization()->ActionPreDeleteDivision(m_Venue, m_Name, nRuns + nPoints);
				ioCallBack.PreDelete(msg);
				if (!ioCallBack.CanContinue())
					return bChanged;
				ioInfo << msg << wxT("\n");
				ioDogs->DeleteDivision(ioConfig, m_Venue, m_Name);
			}
			bChanged = true;
			ioInfo << Localization()->ActionDeleteDivision(m_Venue, m_Name) << wxT("\n");
			if (venue->GetDivisions().DeleteDivision(m_Name, venue->GetEvents()))
				venue->GetMultiQs().DeleteDivision(m_Name);
		}
	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigActionPtr ARBConfigActionRenameLevel::NewLevel(
		wxString const& inVenue,
		wxString const& inDiv,
		wxString const& inOldName,
		wxString const& inNewName)
{
	return ARBConfigActionPtr(new ARBConfigActionRenameLevel(inVenue, inDiv, wxString(), inOldName, inNewName));
}


ARBConfigActionPtr ARBConfigActionRenameLevel::NewSubLevel(
		wxString const& inVenue,
		wxString const& inDiv,
		wxString const& inLevel,
		wxString const& inOldName,
		wxString const& inNewName)
{
	return ARBConfigActionPtr(new ARBConfigActionRenameLevel(inVenue, inDiv, inLevel, inOldName, inNewName));
}


ARBConfigActionRenameLevel::ARBConfigActionRenameLevel(
		wxString const& inVenue,
		wxString const& inDiv,
		wxString const& inLevel,
		wxString const& inOldName,
		wxString const& inNewName)
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
		wxString& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	bool bChanged = false;
	ARBConfigVenuePtr venue;
	if (ioConfig.GetVenues().FindVenue(m_Venue, &venue))
	{
		ARBConfigDivisionPtr div;
		if (venue->GetDivisions().FindDivision(m_Div, &div))
		{
			ARBConfigLevelPtr oldLevel;
			bool bLeaf = false;
			bool bOk = false;
			// If level is not empty, we're dealing with a sublevel
			if (m_Level.empty())
			{
				bOk = div->GetLevels().FindLevel(m_OldName, &oldLevel);
				if (bOk)
					bLeaf = (0 == oldLevel->GetSubLevels().size());
			}
			else
			{
				bOk = div->GetLevels().FindLevel(m_Level, &oldLevel);
				bLeaf = true;
			}
			if (bOk)
			{
				bChanged = true;
				int nRuns = 0;
				// Note: Only run the update on leaf nodes
				if (ioDogs && bLeaf)
				{
					nRuns += ioDogs->NumLevelsInUse(m_Venue, m_Div, m_OldName);
					ioDogs->RenameLevel(m_Venue, m_Div, m_OldName, m_NewName);
				}
				ioInfo << Localization()->ActionRenameLevel(m_Venue, m_OldName, m_NewName, nRuns) << wxT("\n");
				// Only update when the actual level (not sublevel) changes
				if (m_Level.empty())
					venue->GetEvents().RenameLevel(m_Div, m_OldName, m_NewName);
				// Only update on leafs
				if (bLeaf)
					venue->GetMultiQs().RenameLevel(m_Div, m_OldName, m_NewName);
				// If the new level exists, just delete the old.
				// Otherwise, rename the old to new.
				if (div->GetLevels().FindLevel(m_NewName))
				{
					if (m_Level.empty())
						div->GetLevels().DeleteLevel(div->GetName(), m_OldName, venue->GetEvents());
					else
					{
						bool bModified;
						div->GetLevels().DeleteSubLevel(m_OldName, bModified);
					}
				}
				else
					oldLevel->SetName(m_NewName);
			}
		}
	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigActionPtr ARBConfigActionDeleteLevel::NewLevel(
		wxString const& inVenue,
		wxString const& inDiv,
		wxString const& inName)
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteLevel(inVenue, inDiv, wxString(), inName));
}


ARBConfigActionPtr ARBConfigActionDeleteLevel::NewSubLevel(
		wxString const& inVenue,
		wxString const& inDiv,
		wxString const& inLevel,
		wxString const& inName)
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteLevel(inVenue, inDiv, inLevel, inName));
}


ARBConfigActionDeleteLevel::ARBConfigActionDeleteLevel(
		wxString const& inVenue,
		wxString const& inDiv,
		wxString const& inLevel,
		wxString const& inName)
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
		wxString& ioInfo,
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
			std::vector<wxString> sublevels;
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
				for (std::vector<wxString>::iterator i = sublevels.begin();
					i != sublevels.end();
					++i)
				{
					nRuns += ioDogs->NumLevelsInUse(m_Venue, m_Div, *i);
				}
			}
			if (0 < nRuns && ioDogs)
			{
				wxString msg = Localization()->ActionPreDeleteLevel(m_Venue, m_Name, nRuns);
				ioCallBack.PreDelete(msg);
				if (!ioCallBack.CanContinue())
					return bChanged;
				ioInfo << msg << wxT("\n");
				for (std::vector<wxString>::iterator i = sublevels.begin();
					i != sublevels.end();
					++i)
				{
					ioDogs->DeleteLevel(m_Venue, m_Div, *i);
				}
			}
			bChanged = true;
			ioInfo << Localization()->ActionDeleteLevel(m_Venue, m_Name) << wxT("\n");
			// Deleting level
			if (m_Level.empty())
			{
				if (div->GetLevels().DeleteLevel(div->GetName(), m_Name, venue->GetEvents()))
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
		wxString const& inVenue,
		wxString const& inOldName,
		wxString const& inNewName)
{
	return ARBConfigActionPtr(new ARBConfigActionRenameTitle(inVenue, inOldName, inNewName));
}


ARBConfigActionRenameTitle::ARBConfigActionRenameTitle(
		wxString const& inVenue,
		wxString const& inOldName,
		wxString const& inNewName)
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
		wxString& ioInfo,
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
			//
			// If any titles are in use, create a fixup action.
			int nTitles = 0;
			if (ioDogs)
				nTitles = ioDogs->NumTitlesInUse(m_Venue, m_OldName);
			if (0 < nTitles && ioDogs)
			{
				ioDogs->RenameTitle(m_Venue, m_OldName, m_NewName);
			}
			ioInfo << Localization()->ActionRenameTitle(m_Venue, m_OldName, m_NewName,nTitles) << wxT("\n");
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
		wxString const& inVenue,
		wxString const& inDiv,
		wxString const& inOldName,
		wxString const& inNewName)
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteTitle(inVenue, inDiv, inOldName, inNewName));
}


ARBConfigActionDeleteTitle::ARBConfigActionDeleteTitle(
		wxString const& inVenue,
		wxString const& inDiv,
		wxString const& inOldName,
		wxString const& inNewName)
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
		wxString& ioInfo,
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
					ioInfo << Localization()->ActionRenameTitle(m_Venue, m_OldName, m_NewName, nTitles);
					ioDogs->RenameTitle(m_Venue, m_OldName, m_NewName);
				}
				else
				{
					wxString msg = Localization()->ActionPreDeleteTitle(m_Venue, m_OldName, nTitles);
					ioCallBack.PreDelete(msg);
					if (!ioCallBack.CanContinue())
						return bChanged;
					ioInfo << msg << wxT("\n");
					ioDogs->DeleteTitle(m_Venue, m_OldName);
				}
			}
			bChanged = true;
			ioInfo << Localization()->ActionDeleteTitle(m_Venue, m_OldName) << wxT("\n");
			venue->GetTitles().DeleteTitle(m_OldName);
		}
	}

	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigActionPtr ARBConfigActionRenameEvent::New(
		wxString const& inVenue,
		wxString const& inOldName,
		wxString const& inNewName)
{
	return ARBConfigActionPtr(new ARBConfigActionRenameEvent(inVenue, inOldName, inNewName));
}


ARBConfigActionRenameEvent::ARBConfigActionRenameEvent(
		wxString const& inVenue,
		wxString const& inOldName,
		wxString const& inNewName)
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
		wxString& ioInfo,
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
			// If any events are in use, create a fixup action.
			int nEvents = 0;
			if (ioDogs)
				nEvents = ioDogs->NumEventsInUse(m_Venue, m_OldName);
			if (0 < nEvents && ioDogs)
			{
				ioDogs->RenameEvent(m_Venue, m_OldName, m_NewName);
			}
			ioInfo << Localization()->ActionRenameEvent(m_Venue, m_OldName, m_NewName, nEvents) << wxT("\n");
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
		wxString const& inVenue,
		wxString const& inName)
{
	return ARBConfigActionPtr(new ARBConfigActionDeleteEvent(inVenue, inName));
}


ARBConfigActionDeleteEvent::ARBConfigActionDeleteEvent(
		wxString const& inVenue,
		wxString const& inName)
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
		wxString& ioInfo,
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
				wxString msg = Localization()->ActionPreDeleteEvent(m_Venue, m_Name, nEvents);
				ioCallBack.PreDelete(msg);
				if (!ioCallBack.CanContinue())
					return bChanged;
				ioInfo << msg << wxT("\n");
				ioDogs->DeleteEvent(m_Venue, m_Name);
			}
			ioInfo << Localization()->ActionDeleteEvent(m_Venue, m_Name) << wxT("\n");
			venue->GetMultiQs().DeleteEvent(m_Name);
			venue->GetEvents().DeleteEvent(m_Name);
		}
	}

	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigActionList::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_ACTION)
		return false;
	wxString verb;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_ACTION_VERB, verb)
	|| 0 == verb.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_ACTION, ATTRIB_ACTION_VERB));
		return false;
	}
	wxString venue, div, oldName, newName;
	inTree->GetAttrib(ATTRIB_ACTION_VENUE, venue);
	inTree->GetAttrib(ATTRIB_ACTION_DIVISION, div);
	inTree->GetAttrib(ATTRIB_ACTION_OLDNAME, oldName);
	inTree->GetAttrib(ATTRIB_ACTION_NEWNAME, newName);

	ARBConfigActionPtr item;
	if (ACTION_VERB_DELETE_CALPLUGIN == verb)
	{
		item = ARBConfigActionDeleteCalPlugin::New(oldName);
	}
	else if (ACTION_VERB_DELETE_TITLE == verb)
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
	else if (ACTION_VERB_RENAME_VENUE == verb)
	{
		item = ARBConfigActionRenameVenue::New(oldName, newName);
	}
	else
	{
		wxString msg(Localization()->ValidValues());
		msg += ACTION_VERB_DELETE_CALPLUGIN;
		msg += wxT(", ");
		msg += ACTION_VERB_DELETE_TITLE;
		msg += wxT(", ");
		msg += ACTION_VERB_RENAME_TITLE;
		msg += wxT(", ");
		msg += ACTION_VERB_DELETE_EVENT;
		msg += wxT(", ");
		msg += ACTION_VERB_RENAME_EVENT;
		msg += wxT(", ");
		msg += ACTION_VERB_RENAME_DIV;
		msg += wxT(", ");
		msg += ACTION_VERB_RENAME_VENUE;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_ACTION, ATTRIB_ACTION_VERB, msg));
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
		wxString& ioInfo,
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
		ioInfo << wxT("\n");
	return nChanges;
}
