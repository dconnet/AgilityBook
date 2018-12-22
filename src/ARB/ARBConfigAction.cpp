/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief The classes that make up the configuration information.
 * @author David Connet
 *
 * This class is designed to handle changes in configuration data. For
 * instance, the 2004 USDAA rules have changed some of the abbreviations
 * for titles (PG -> PG3, PS -> PK3) in addition to adding new ones. To
 * maintain our data integrity, we need to update things to deal with this.
 *
 * Revision History
 * 2018-12-16 Convert to fmt.
 * 2016-06-19 Add support for Lifetime names.
 * 2013-01-11 Fix filters on configuration import.
 * 2012-11-21 Add RenameLevel action verb.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-12-03 Complete re-write of action class.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-10-26 Added optional 'Div' to DeleteTitle.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-01-21 Created
 */

#include "stdafx.h"
#include "ARB/ARBConfigAction.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

#define ACTION_VERB_DELETE_CALPLUGIN	L"DeleteCalPlugin"
#define ACTION_VERB_DELETE_TITLE		L"DeleteTitle"
#define ACTION_VERB_RENAME_TITLE		L"RenameTitle"
#define ACTION_VERB_DELETE_EVENT		L"DeleteEvent"
#define ACTION_VERB_RENAME_EVENT		L"RenameEvent"
#define ACTION_VERB_RENAME_LEVEL		L"RenameLevel"
#define ACTION_VERB_RENAME_DIV			L"RenameDivision"
#define ACTION_VERB_RENAME_VENUE		L"RenameVenue"

/////////////////////////////////////////////////////////////////////////////

IConfigActionCallback::IConfigActionCallback()
	: m_bContinue(true)
{
}


IConfigActionCallback::~IConfigActionCallback()
{
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigAction::ARBConfigAction(short configVersion)
	: m_configVersion(configVersion)
{
}


ARBConfigAction::ARBConfigAction(ARBConfigAction const& rhs)
	: m_configVersion(rhs.m_configVersion)
{
}


ARBConfigAction::~ARBConfigAction()
{
}

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionDeleteCalPlugin_concrete : public ARBConfigActionDeleteCalPlugin
{
public:
	ARBConfigActionDeleteCalPlugin_concrete(std::wstring const& inName)
		: ARBConfigActionDeleteCalPlugin(inName)
	{
	}
	ARBConfigActionDeleteCalPlugin_concrete(ARBConfigActionDeleteCalPlugin const& rhs)
		: ARBConfigActionDeleteCalPlugin(rhs)
	{
	}
};


ARBConfigActionPtr ARBConfigActionDeleteCalPlugin::New(
		std::wstring const& inName)
{
	return std::make_shared<ARBConfigActionDeleteCalPlugin_concrete>(inName);
}


ARBConfigActionDeleteCalPlugin::ARBConfigActionDeleteCalPlugin(
		std::wstring const& inName)
	: ARBConfigAction(0)
	, m_Name(inName)
{
}


ARBConfigActionDeleteCalPlugin::ARBConfigActionDeleteCalPlugin(ARBConfigActionDeleteCalPlugin const& rhs)
	: ARBConfigAction(rhs)
	, m_Name(rhs.m_Name)
{
}


ARBConfigActionPtr ARBConfigActionDeleteCalPlugin::Clone() const
{
	return std::make_shared<ARBConfigActionDeleteCalPlugin_concrete>(m_Name);
}


bool ARBConfigActionDeleteCalPlugin::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		fmt::wmemory_buffer& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	bool bChanged = false;
	if (ioConfig.GetCalSites().DeleteSite(m_Name))
	{
		bChanged = true;
		fmt::format_to(ioInfo, L"{}\n", Localization()->ActionDeleteCalPlugin(m_Name));
	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameOtherPoints_concrete : public ARBConfigActionRenameOtherPoints
{
public:
	ARBConfigActionRenameOtherPoints_concrete(
			short configVersion,
			std::wstring const& inOldName,
			std::wstring const& inNewName)
		: ARBConfigActionRenameOtherPoints(configVersion, inOldName, inNewName)
	{
	}
	ARBConfigActionRenameOtherPoints_concrete(
			ARBConfigActionRenameOtherPoints const& rhs)
		: ARBConfigActionRenameOtherPoints(rhs)
	{
	}
};


ARBConfigActionPtr ARBConfigActionRenameOtherPoints::New(
		short configVersion,
		std::wstring const& inOldName,
		std::wstring const& inNewName)
{
	return std::make_shared<ARBConfigActionRenameOtherPoints_concrete>(
		configVersion,
		inOldName,
		inNewName);
}


ARBConfigActionRenameOtherPoints::ARBConfigActionRenameOtherPoints(
		short configVersion,
		std::wstring const& inOldName,
		std::wstring const& inNewName)
	: ARBConfigAction(configVersion)
	, m_OldName(inOldName)
	, m_NewName(inNewName)
{
}


ARBConfigActionRenameOtherPoints::ARBConfigActionRenameOtherPoints(ARBConfigActionRenameOtherPoints const& rhs)
	: ARBConfigAction(rhs)
	, m_OldName(rhs.m_OldName)
	, m_NewName(rhs.m_NewName)
{
}


ARBConfigActionPtr ARBConfigActionRenameOtherPoints::Clone() const
{
	return std::make_shared<ARBConfigActionRenameOtherPoints_concrete>(
		m_configVersion,
		m_OldName,
		m_NewName);
}


bool ARBConfigActionRenameOtherPoints::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		fmt::wmemory_buffer& ioInfo,
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
		fmt::format_to(ioInfo, L"{}\n", Localization()->ActionRenameOtherPoints(m_OldName, m_NewName, nPoints));
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

class ARBConfigActionDeleteOtherPoints_concrete : public ARBConfigActionDeleteOtherPoints
{
public:
	ARBConfigActionDeleteOtherPoints_concrete(
			short configVersion,
			std::wstring const& inName)
		: ARBConfigActionDeleteOtherPoints(configVersion, inName)
	{
	}
	ARBConfigActionDeleteOtherPoints_concrete(
			ARBConfigActionDeleteOtherPoints const& rhs)
		: ARBConfigActionDeleteOtherPoints(rhs)
	{
	}
};


ARBConfigActionPtr ARBConfigActionDeleteOtherPoints::New(
		short configVersion,
		std::wstring const& inName)
{
	return std::make_shared<ARBConfigActionDeleteOtherPoints_concrete>(
		configVersion,
		inName);
}


ARBConfigActionDeleteOtherPoints::ARBConfigActionDeleteOtherPoints(
		short configVersion,
		std::wstring const& inName)
	: ARBConfigAction(configVersion)
	, m_Name(inName)
{
}


ARBConfigActionDeleteOtherPoints::ARBConfigActionDeleteOtherPoints(ARBConfigActionDeleteOtherPoints const& rhs)
	: ARBConfigAction(rhs)
	, m_Name(rhs.m_Name)
{
}


ARBConfigActionPtr ARBConfigActionDeleteOtherPoints::Clone() const
{
	return std::make_shared<ARBConfigActionDeleteOtherPoints_concrete>(
		m_configVersion,
		m_Name);
}


bool ARBConfigActionDeleteOtherPoints::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		fmt::wmemory_buffer& ioInfo,
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
			std::wstring msg = Localization()->ActionPreDeleteOtherPoints(m_Name, nPoints);
			ioCallBack.PreDelete(msg);
			if (!ioCallBack.CanContinue())
				return bChanged;
			fmt::format_to(ioInfo, L"{}\n", msg);
			ioDogs->DeleteOtherPoints(m_Name);
		}
		bChanged = true;
		fmt::format_to(ioInfo, L"{}\n", Localization()->ActionDeleteOtherPoints(m_Name));
		ioConfig.GetOtherPoints().DeleteOtherPoints(m_Name);

	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameVenue_concrete : public ARBConfigActionRenameVenue
{
public:
	ARBConfigActionRenameVenue_concrete(
			short configVersion,
			std::wstring const& inOldName,
			std::wstring const& inNewName)
		: ARBConfigActionRenameVenue(configVersion, inOldName, inNewName)
	{
	}
	ARBConfigActionRenameVenue_concrete(
			ARBConfigActionRenameVenue const& rhs)
		: ARBConfigActionRenameVenue(rhs)
	{
	}
};


ARBConfigActionPtr ARBConfigActionRenameVenue::New(
		short configVersion,
		std::wstring const& inOldName,
		std::wstring const& inNewName)
{
	return std::make_shared<ARBConfigActionRenameVenue_concrete>(
		configVersion,
		inOldName,
		inNewName);
}


ARBConfigActionRenameVenue::ARBConfigActionRenameVenue(
		short configVersion,
		std::wstring const& inOldName,
		std::wstring const& inNewName)
	: ARBConfigAction(configVersion)
	, m_OldName(inOldName)
	, m_NewName(inNewName)
{
}


ARBConfigActionRenameVenue::ARBConfigActionRenameVenue(ARBConfigActionRenameVenue const& rhs)
	: ARBConfigAction(rhs)
	, m_OldName(rhs.m_OldName)
	, m_NewName(rhs.m_NewName)
{
}


ARBConfigActionPtr ARBConfigActionRenameVenue::Clone() const
{
	return std::make_shared<ARBConfigActionRenameVenue_concrete>(
		m_configVersion,
		m_OldName,
		m_NewName);
}


bool ARBConfigActionRenameVenue::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		fmt::wmemory_buffer& ioInfo,
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
		fmt::format_to(ioInfo, L"{}\n", Localization()->ActionRenameVenue(m_OldName, m_NewName, nVenues));
		// If the new venue exists, just delete the old.
		// Otherwise, rename the old to new.
		if (ioConfig.GetVenues().FindVenue(m_NewName))
			ioConfig.GetVenues().DeleteVenue(m_OldName);
		else
			oldVenue->SetName(m_NewName);
	}
	return bChanged;
}


bool ARBConfigActionRenameVenue::Update(
		ARBConfig const& inConfigCurrent,
		std::wstring& ioVenue,
		std::wstring& ioDivision,
		std::wstring& ioSubLevel) const
{
	bool bChanged = false;
	if (!ioVenue.empty() && ioVenue == m_OldName)
	{
		bChanged = true;
		ioVenue = m_NewName;
	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionDeleteVenue_concrete : public ARBConfigActionDeleteVenue
{
public:
	ARBConfigActionDeleteVenue_concrete(
			short configVersion,
			std::wstring const& inName)
		: ARBConfigActionDeleteVenue(configVersion, inName)
	{
	}
	ARBConfigActionDeleteVenue_concrete(
			ARBConfigActionDeleteVenue const& rhs)
		: ARBConfigActionDeleteVenue(rhs)
	{
	}
};


ARBConfigActionPtr ARBConfigActionDeleteVenue::New(
		short configVersion,
		std::wstring const& inName)
{
	return std::make_shared<ARBConfigActionDeleteVenue_concrete>(
		configVersion,
		inName);
}


ARBConfigActionDeleteVenue::ARBConfigActionDeleteVenue(
		short configVersion,
		std::wstring const& inName)
	: ARBConfigAction(configVersion)
	, m_Name(inName)
{
}


ARBConfigActionDeleteVenue::ARBConfigActionDeleteVenue(ARBConfigActionDeleteVenue const& rhs)
	: ARBConfigAction(rhs)
	, m_Name(rhs.m_Name)
{
}


ARBConfigActionPtr ARBConfigActionDeleteVenue::Clone() const
{
	return std::make_shared<ARBConfigActionDeleteVenue_concrete>(
		m_configVersion,
		m_Name);
}


bool ARBConfigActionDeleteVenue::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		fmt::wmemory_buffer& ioInfo,
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
			std::wstring msg = Localization()->ActionPreDeleteVenue(m_Name, nVenues);
			ioCallBack.PreDelete(msg);
			if (!ioCallBack.CanContinue())
				return bChanged;
			fmt::format_to(ioInfo, L"{}\n", msg);
			ioDogs->DeleteVenue(m_Name);
		}
		bChanged = true;
		fmt::format_to(ioInfo, L"{}\n", Localization()->ActionDeleteVenue(m_Name));
		ioConfig.GetVenues().DeleteVenue(m_Name);
	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameMultiQ_concrete : public ARBConfigActionRenameMultiQ
{
public:
	ARBConfigActionRenameMultiQ_concrete(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inOldName,
			std::wstring const& inNewName)
		: ARBConfigActionRenameMultiQ(configVersion, inVenue, inOldName, inNewName)
	{
	}
	ARBConfigActionRenameMultiQ_concrete(
			ARBConfigActionRenameMultiQ const& rhs)
		: ARBConfigActionRenameMultiQ(rhs)
	{
	}
};


ARBConfigActionPtr ARBConfigActionRenameMultiQ::New(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inOldName,
		std::wstring const& inNewName)
{
	return std::make_shared<ARBConfigActionRenameMultiQ_concrete>(
		configVersion,
		inVenue,
		inOldName,
		inNewName);
}


ARBConfigActionRenameMultiQ::ARBConfigActionRenameMultiQ(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inOldName,
		std::wstring const& inNewName)
	: ARBConfigAction(configVersion)
	, m_Venue(inVenue)
	, m_OldName(inOldName)
	, m_NewName(inNewName)
{
}


ARBConfigActionRenameMultiQ::ARBConfigActionRenameMultiQ(ARBConfigActionRenameMultiQ const& rhs)
	: ARBConfigAction(rhs)
	, m_Venue(rhs.m_Venue)
	, m_OldName(rhs.m_OldName)
	, m_NewName(rhs.m_NewName)
{
}


ARBConfigActionPtr ARBConfigActionRenameMultiQ::Clone() const
{
	return std::make_shared<ARBConfigActionRenameMultiQ_concrete>(
		m_configVersion,
		m_Venue,
		m_OldName,
		m_NewName);
}


bool ARBConfigActionRenameMultiQ::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		fmt::wmemory_buffer& ioInfo,
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
			fmt::format_to(ioInfo, L"{}\n", Localization()->ActionRenameMultiQ(m_Venue, m_OldName, m_NewName, nMultiQs));
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

class ARBConfigActionDeleteMultiQ_concrete : public ARBConfigActionDeleteMultiQ
{
public:
	ARBConfigActionDeleteMultiQ_concrete(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inName)
		: ARBConfigActionDeleteMultiQ(configVersion, inVenue, inName)
	{
	}
	ARBConfigActionDeleteMultiQ_concrete(
			ARBConfigActionDeleteMultiQ const& rhs)
		: ARBConfigActionDeleteMultiQ(rhs)
	{
	}
};


ARBConfigActionPtr ARBConfigActionDeleteMultiQ::New(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inName)
{
	return std::make_shared<ARBConfigActionDeleteMultiQ_concrete>(
		configVersion,
		inVenue,
		inName);
}


ARBConfigActionDeleteMultiQ::ARBConfigActionDeleteMultiQ(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inName)
	: ARBConfigAction(configVersion)
	, m_Venue(inVenue)
	, m_Name(inName)
{
}


ARBConfigActionDeleteMultiQ::ARBConfigActionDeleteMultiQ(ARBConfigActionDeleteMultiQ const& rhs)
	: ARBConfigAction(rhs)
	, m_Venue(rhs.m_Venue)
	, m_Name(rhs.m_Name)
{
}


ARBConfigActionPtr ARBConfigActionDeleteMultiQ::Clone() const
{
	return std::make_shared<ARBConfigActionDeleteMultiQ_concrete>(
		m_configVersion,
		m_Venue,
		m_Name);
}


bool ARBConfigActionDeleteMultiQ::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		fmt::wmemory_buffer& ioInfo,
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
				std::wstring msg = Localization()->ActionPreDeleteMultiQ(m_Venue, m_Name, nMultiQs);
				ioCallBack.PreDelete(msg);
				if (!ioCallBack.CanContinue())
					return bChanged;
				fmt::format_to(ioInfo, L"{}\n", msg);
			}
			bChanged = true;
			fmt::format_to(ioInfo, L"{}\n", Localization()->ActionDeleteMultiQ(m_Venue, m_Name));
			venue->GetMultiQs().DeleteMultiQ(oldMultiQ);
			// Note: The multiQs in the dogs will be fixed at the very end
			// in ARBAgiltyRecordBook::Update
		}
	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameDivision_concrete : public ARBConfigActionRenameDivision
{
public:
	ARBConfigActionRenameDivision_concrete(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inOldName,
			std::wstring const& inNewName)
		: ARBConfigActionRenameDivision(configVersion, inVenue, inOldName, inNewName)
	{
	}
	ARBConfigActionRenameDivision_concrete(ARBConfigActionRenameDivision const& rhs)
		: ARBConfigActionRenameDivision(rhs)
	{
	}
};


ARBConfigActionPtr ARBConfigActionRenameDivision::New(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inOldName,
		std::wstring const& inNewName)
{
	return std::make_shared<ARBConfigActionRenameDivision_concrete>(
		configVersion,
		inVenue,
		inOldName,
		inNewName);
}


ARBConfigActionRenameDivision::ARBConfigActionRenameDivision(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inOldName,
		std::wstring const& inNewName)
	: ARBConfigAction(configVersion)
	, m_Venue(inVenue)
	, m_OldName(inOldName)
	, m_NewName(inNewName)
{
}


ARBConfigActionRenameDivision::ARBConfigActionRenameDivision(ARBConfigActionRenameDivision const& rhs)
	: ARBConfigAction(rhs)
	, m_Venue(rhs.m_Venue)
	, m_OldName(rhs.m_OldName)
	, m_NewName(rhs.m_NewName)
{
}


ARBConfigActionPtr ARBConfigActionRenameDivision::Clone() const
{
	return std::make_shared<ARBConfigActionRenameDivision_concrete>(
		m_configVersion,
		m_Venue,
		m_OldName,
		m_NewName);
}


bool ARBConfigActionRenameDivision::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		fmt::wmemory_buffer& ioInfo,
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
			fmt::format_to(ioInfo, L"{}\n", Localization()->ActionRenameDivision(m_Venue, m_OldName, m_NewName,nRuns));
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


bool ARBConfigActionRenameDivision::Update(
		ARBConfig const& inConfigCurrent,
		std::wstring& ioVenue,
		std::wstring& ioDivision,
		std::wstring& ioSubLevel) const
{
	bool bChanged = false;
	if (!ioVenue.empty() && ioVenue == m_Venue
		&& !ioDivision.empty() && ioDivision == m_OldName)
	{
		bChanged = true;
		ioDivision = m_NewName;
	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionDeleteDivision_concrete : public ARBConfigActionDeleteDivision
{
public:
	ARBConfigActionDeleteDivision_concrete(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inName)
		: ARBConfigActionDeleteDivision(configVersion, inVenue, inName)
	{
	}
	ARBConfigActionDeleteDivision_concrete(
			ARBConfigActionDeleteDivision const& rhs)
		: ARBConfigActionDeleteDivision(rhs)
	{
	}
};


ARBConfigActionPtr ARBConfigActionDeleteDivision::New(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inName)
{
	return std::make_shared<ARBConfigActionDeleteDivision_concrete>(
		configVersion,
		inVenue,
		inName);
}


ARBConfigActionDeleteDivision::ARBConfigActionDeleteDivision(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inName)
	: ARBConfigAction(configVersion)
	, m_Venue(inVenue)
	, m_Name(inName)
{
}


ARBConfigActionDeleteDivision::ARBConfigActionDeleteDivision(ARBConfigActionDeleteDivision const& rhs)
	: ARBConfigAction(rhs)
	, m_Venue(rhs.m_Venue)
	, m_Name(rhs.m_Name)
{
}


ARBConfigActionPtr ARBConfigActionDeleteDivision::Clone() const
{
	return std::make_shared<ARBConfigActionDeleteDivision_concrete>(
		m_configVersion,
		m_Venue,
		m_Name);
}


bool ARBConfigActionDeleteDivision::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		fmt::wmemory_buffer& ioInfo,
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
				std::wstring msg = Localization()->ActionPreDeleteDivision(m_Venue, m_Name, nRuns + nPoints);
				ioCallBack.PreDelete(msg);
				if (!ioCallBack.CanContinue())
					return bChanged;
				fmt::format_to(ioInfo, L"{}\n", msg);
				ioDogs->DeleteDivision(ioConfig, m_Venue, m_Name);
			}
			bChanged = true;
			fmt::format_to(ioInfo, L"{}\n", Localization()->ActionDeleteDivision(m_Venue, m_Name));
			if (venue->GetDivisions().DeleteDivision(m_Name, venue->GetEvents()))
				venue->GetMultiQs().DeleteDivision(m_Name);
		}
	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameLevel_concrete : public ARBConfigActionRenameLevel
{
public:
	ARBConfigActionRenameLevel_concrete(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inDiv,
			std::wstring const& inLevel,
			std::wstring const& inOldName,
			std::wstring const& inNewName)
		: ARBConfigActionRenameLevel(configVersion, inVenue, inDiv, inLevel, inOldName, inNewName)
	{
	}
	ARBConfigActionRenameLevel_concrete(
			ARBConfigActionRenameLevel const& rhs)
		: ARBConfigActionRenameLevel(rhs)
	{
	}
};

ARBConfigActionPtr ARBConfigActionRenameLevel::NewLevel(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inDiv,
		std::wstring const& inOldName,
		std::wstring const& inNewName)
{
	return std::make_shared<ARBConfigActionRenameLevel_concrete>(
		configVersion,
		inVenue,
		inDiv,
		std::wstring(),
		inOldName,
		inNewName);
}


ARBConfigActionPtr ARBConfigActionRenameLevel::NewSubLevel(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inDiv,
		std::wstring const& inLevel,
		std::wstring const& inOldName,
		std::wstring const& inNewName)
{
	return std::make_shared<ARBConfigActionRenameLevel_concrete>(
		configVersion,
		inVenue,
		inDiv,
		inLevel,
		inOldName,
		inNewName);
}


ARBConfigActionRenameLevel::ARBConfigActionRenameLevel(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inDiv,
		std::wstring const& inLevel,
		std::wstring const& inOldName,
		std::wstring const& inNewName)
	: ARBConfigAction(configVersion)
	, m_Venue(inVenue)
	, m_Div(inDiv)
	, m_OldName(inOldName)
	, m_NewName(inNewName)
{
}


ARBConfigActionRenameLevel::ARBConfigActionRenameLevel(ARBConfigActionRenameLevel const& rhs)
	: ARBConfigAction(rhs)
	, m_Venue(rhs.m_Venue)
	, m_Div(rhs.m_Div)
	, m_Level(rhs.m_Level)
	, m_OldName(rhs.m_OldName)
	, m_NewName(rhs.m_NewName)
{
}


ARBConfigActionPtr ARBConfigActionRenameLevel::Clone() const
{
	return std::make_shared<ARBConfigActionRenameLevel_concrete>(
		m_configVersion,
		m_Venue,
		m_Div,
		m_Level,
		m_OldName,
		m_NewName);
}


bool ARBConfigActionRenameLevel::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		fmt::wmemory_buffer& ioInfo,
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
				fmt::format_to(ioInfo, L"{}\n", Localization()->ActionRenameLevel(m_Venue, m_OldName, m_NewName, nRuns));
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


bool ARBConfigActionRenameLevel::Update(
		ARBConfig const& inConfigCurrent,
		std::wstring& ioVenue,
		std::wstring& ioDivision,
		std::wstring& ioSubLevel) const
{
	bool bChanged = false;
	if (!ioVenue.empty() && ioVenue == m_Venue
		&& !ioDivision.empty() && ioDivision == m_Div
		&& !ioSubLevel.empty())
	{
		ARBConfigVenuePtr venue;
		if (inConfigCurrent.GetVenues().FindVenue(m_Venue, &venue))
		{
			ARBConfigDivisionPtr div;
			if (venue->GetDivisions().FindDivision(m_Div, &div))
			{
				// If level is not empty, we're dealing with a sublevel
				if (m_Level.empty())
				{
					ARBConfigLevelPtr level;
					if (div->GetLevels().FindLevel(m_Level, &level))
					{
						if (0 != level->GetSubLevels().size())
							return false;
					}
				}
				if (ioSubLevel == m_OldName)
				{
					bChanged = true;
					ioSubLevel = m_NewName;
				}
			}
		}
	}
	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionDeleteLevel_concrete : public ARBConfigActionDeleteLevel
{
public:
	ARBConfigActionDeleteLevel_concrete(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inDiv,
			std::wstring const& inLevel,
			std::wstring const& inName)
		: ARBConfigActionDeleteLevel(configVersion, inVenue, inDiv, inLevel, inName)
	{
	}
	ARBConfigActionDeleteLevel_concrete(
			ARBConfigActionDeleteLevel const& rhs)
		: ARBConfigActionDeleteLevel(rhs)
	{
	}
};


ARBConfigActionPtr ARBConfigActionDeleteLevel::NewLevel(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inDiv,
		std::wstring const& inName)
{
	return std::make_shared<ARBConfigActionDeleteLevel_concrete>(
		configVersion,
		inVenue,
		inDiv,
		std::wstring(),
		inName);
}


ARBConfigActionPtr ARBConfigActionDeleteLevel::NewSubLevel(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inDiv,
		std::wstring const& inLevel,
		std::wstring const& inName)
{
	return std::make_shared<ARBConfigActionDeleteLevel_concrete>(
		configVersion,
		inVenue,
		inDiv,
		inLevel,
		inName);
}


ARBConfigActionDeleteLevel::ARBConfigActionDeleteLevel(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inDiv,
		std::wstring const& inLevel,
		std::wstring const& inName)
	: ARBConfigAction(configVersion)
	, m_Venue(inVenue)
	, m_Div(inDiv)
	, m_Level(inLevel)
	, m_Name(inName)
{
}


ARBConfigActionDeleteLevel::ARBConfigActionDeleteLevel(ARBConfigActionDeleteLevel const& rhs)
	: ARBConfigAction(rhs)
	, m_Venue(rhs.m_Venue)
	, m_Div(rhs.m_Div)
	, m_Level(rhs.m_Level)
	, m_Name(rhs.m_Name)
{
}


ARBConfigActionPtr ARBConfigActionDeleteLevel::Clone() const
{
	return std::make_shared<ARBConfigActionDeleteLevel_concrete>(
		m_configVersion,
		m_Venue,
		m_Div,
		m_Level,
		m_Name);
}


bool ARBConfigActionDeleteLevel::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		fmt::wmemory_buffer& ioInfo,
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
			std::vector<std::wstring> sublevels;
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
				for (std::vector<std::wstring>::iterator i = sublevels.begin();
					i != sublevels.end();
					++i)
				{
					nRuns += ioDogs->NumLevelsInUse(m_Venue, m_Div, *i);
				}
			}
			if (0 < nRuns && ioDogs)
			{
				std::wstring msg = Localization()->ActionPreDeleteLevel(m_Venue, m_Name, nRuns);
				ioCallBack.PreDelete(msg);
				if (!ioCallBack.CanContinue())
					return bChanged;
				fmt::format_to(ioInfo, L"{}\n", msg);
				for (std::vector<std::wstring>::iterator i = sublevels.begin();
					i != sublevels.end();
					++i)
				{
					ioDogs->DeleteLevel(m_Venue, m_Div, *i);
				}
			}
			bChanged = true;
			fmt::format_to(ioInfo, L"{}\n", Localization()->ActionDeleteLevel(m_Venue, m_Name));
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

class ARBConfigActionRenameTitle_concrete : public ARBConfigActionRenameTitle
{
public:
	ARBConfigActionRenameTitle_concrete(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inOldName,
			std::wstring const& inNewName)
		: ARBConfigActionRenameTitle(configVersion, inVenue, inOldName, inNewName)
	{
	}
	ARBConfigActionRenameTitle_concrete(ARBConfigActionRenameTitle const& rhs)
		: ARBConfigActionRenameTitle(rhs)
	{
	}
};


ARBConfigActionPtr ARBConfigActionRenameTitle::New(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inOldName,
		std::wstring const& inNewName)
{
	return std::make_shared<ARBConfigActionRenameTitle_concrete>(
		configVersion,
		inVenue,
		inOldName,
		inNewName);
}


ARBConfigActionRenameTitle::ARBConfigActionRenameTitle(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inOldName,
		std::wstring const& inNewName)
	: ARBConfigAction(configVersion)
	, m_Venue(inVenue)
	, m_OldName(inOldName)
	, m_NewName(inNewName)
{
}


ARBConfigActionRenameTitle::ARBConfigActionRenameTitle(ARBConfigActionRenameTitle const& rhs)
	: ARBConfigAction(rhs)
	, m_Venue(rhs.m_Venue)
	, m_OldName(rhs.m_OldName)
	, m_NewName(rhs.m_NewName)
{
}


ARBConfigActionPtr ARBConfigActionRenameTitle::Clone() const
{
	return std::make_shared<ARBConfigActionRenameTitle_concrete>(
		m_configVersion,
		m_Venue,
		m_OldName,
		m_NewName);
}


bool ARBConfigActionRenameTitle::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		fmt::wmemory_buffer& ioInfo,
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
			fmt::format_to(ioInfo, L"{}\n", Localization()->ActionRenameTitle(m_Venue, m_OldName, m_NewName,nTitles));
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

class ARBConfigActionDeleteTitle_concrete : public ARBConfigActionDeleteTitle
{
public:
	ARBConfigActionDeleteTitle_concrete(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inDiv,
			std::wstring const& inOldName,
			std::wstring const& inNewName)
		: ARBConfigActionDeleteTitle(configVersion, inVenue, inDiv, inOldName, inNewName)
	{
	}
	ARBConfigActionDeleteTitle_concrete(
			ARBConfigActionDeleteTitle const& rhs)
		: ARBConfigActionDeleteTitle(rhs)
	{
	}
};


ARBConfigActionPtr ARBConfigActionDeleteTitle::New(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inDiv,
		std::wstring const& inOldName,
		std::wstring const& inNewName)
{
	return std::make_shared<ARBConfigActionDeleteTitle_concrete>(
		configVersion,
		inVenue,
		inDiv,
		inOldName,
		inNewName);
}


ARBConfigActionDeleteTitle::ARBConfigActionDeleteTitle(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inDiv,
		std::wstring const& inOldName,
		std::wstring const& inNewName)
	: ARBConfigAction(configVersion)
	, m_Venue(inVenue)
	, m_Div(inDiv)
	, m_OldName(inOldName)
	, m_NewName(inNewName)
{
}


ARBConfigActionDeleteTitle::ARBConfigActionDeleteTitle(ARBConfigActionDeleteTitle const& rhs)
	: ARBConfigAction(rhs)
	, m_Venue(rhs.m_Venue)
	, m_Div(rhs.m_Div)
	, m_OldName(rhs.m_OldName)
	, m_NewName(rhs.m_NewName)
{
}


ARBConfigActionPtr ARBConfigActionDeleteTitle::Clone() const
{
	return std::make_shared<ARBConfigActionDeleteTitle_concrete>(
		m_configVersion,
		m_Venue,
		m_Div,
		m_OldName,
		m_NewName);
}


bool ARBConfigActionDeleteTitle::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		fmt::wmemory_buffer& ioInfo,
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
					fmt::format_to(ioInfo, L"{}\n", Localization()->ActionRenameTitle(m_Venue, m_OldName, m_NewName, nTitles));
					ioDogs->RenameTitle(m_Venue, m_OldName, m_NewName);
				}
				else
				{
					std::wstring msg = Localization()->ActionPreDeleteTitle(m_Venue, m_OldName, nTitles);
					ioCallBack.PreDelete(msg);
					if (!ioCallBack.CanContinue())
						return bChanged;
					fmt::format_to(ioInfo, L"{}\n", msg);
					ioDogs->DeleteTitle(venue, m_Venue, m_OldName);
				}
			}
			bChanged = true;
			fmt::format_to(ioInfo, L"{}\n", Localization()->ActionDeleteTitle(m_Venue, m_OldName));
			venue->GetTitles().DeleteTitle(m_OldName);
		}
	}

	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameEvent_concrete : public ARBConfigActionRenameEvent
{
public:
	ARBConfigActionRenameEvent_concrete(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inOldName,
			std::wstring const& inNewName)
		: ARBConfigActionRenameEvent(configVersion, inVenue, inOldName, inNewName)
	{
	}
	ARBConfigActionRenameEvent_concrete(
			ARBConfigActionRenameEvent const& rhs)
		: ARBConfigActionRenameEvent(rhs)
	{
	}
};


ARBConfigActionPtr ARBConfigActionRenameEvent::New(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inOldName,
		std::wstring const& inNewName)
{
	return std::make_shared<ARBConfigActionRenameEvent_concrete>(
		configVersion,
		inVenue,
		inOldName,
		inNewName);
}


ARBConfigActionRenameEvent::ARBConfigActionRenameEvent(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inOldName,
		std::wstring const& inNewName)
	: ARBConfigAction(configVersion)
	, m_Venue(inVenue)
	, m_OldName(inOldName)
	, m_NewName(inNewName)
{
}


ARBConfigActionRenameEvent::ARBConfigActionRenameEvent(ARBConfigActionRenameEvent const& rhs)
	: ARBConfigAction(rhs)
	, m_Venue(rhs.m_Venue)
	, m_OldName(rhs.m_OldName)
	, m_NewName(rhs.m_NewName)
{
}


ARBConfigActionPtr ARBConfigActionRenameEvent::Clone() const
{
	return std::make_shared<ARBConfigActionRenameEvent_concrete>(
		m_configVersion,
		m_Venue,
		m_OldName,
		m_NewName);
}


bool ARBConfigActionRenameEvent::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		fmt::wmemory_buffer& ioInfo,
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
			fmt::format_to(ioInfo, L"{}\n", Localization()->ActionRenameEvent(m_Venue, m_OldName, m_NewName, nEvents));
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

class ARBConfigActionDeleteEvent_concrete : public ARBConfigActionDeleteEvent
{
public:
	ARBConfigActionDeleteEvent_concrete(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inName)
		: ARBConfigActionDeleteEvent(configVersion, inVenue, inName)
	{
	}
	ARBConfigActionDeleteEvent_concrete(
			ARBConfigActionDeleteEvent const& rhs)
		: ARBConfigActionDeleteEvent(rhs)
	{
	}
};


ARBConfigActionPtr ARBConfigActionDeleteEvent::New(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inName)
{
	return std::make_shared<ARBConfigActionDeleteEvent_concrete>(
		configVersion,
		inVenue,
		inName);
}


ARBConfigActionDeleteEvent::ARBConfigActionDeleteEvent(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inName)
	: ARBConfigAction(configVersion)
	, m_Venue(inVenue)
	, m_Name(inName)
{
}


ARBConfigActionDeleteEvent::ARBConfigActionDeleteEvent(ARBConfigActionDeleteEvent const& rhs)
	: ARBConfigAction(rhs)
	, m_Venue(rhs.m_Venue)
	, m_Name(rhs.m_Name)
{
}


ARBConfigActionPtr ARBConfigActionDeleteEvent::Clone() const
{
	return std::make_shared<ARBConfigActionDeleteEvent_concrete>(
		m_configVersion,
		m_Venue,
		m_Name);
}


bool ARBConfigActionDeleteEvent::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		fmt::wmemory_buffer& ioInfo,
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
				std::wstring msg = Localization()->ActionPreDeleteEvent(m_Venue, m_Name, nEvents);
				ioCallBack.PreDelete(msg);
				if (!ioCallBack.CanContinue())
					return bChanged;
				fmt::format_to(ioInfo, L"{}\n", msg);
				ioDogs->DeleteEvent(m_Venue, m_Name);
			}
			fmt::format_to(ioInfo, L"{}\n", Localization()->ActionDeleteEvent(m_Venue, m_Name));
			venue->GetMultiQs().DeleteEvent(m_Name);
			venue->GetEvents().DeleteEvent(m_Name);
		}
	}

	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameLifetimeName_concrete : public ARBConfigActionRenameLifetimeName
{
public:
	ARBConfigActionRenameLifetimeName_concrete(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inOldName,
			std::wstring const& inNewName)
		: ARBConfigActionRenameLifetimeName(configVersion, inVenue, inOldName, inNewName)
	{
	}
	ARBConfigActionRenameLifetimeName_concrete(
			ARBConfigActionRenameLifetimeName const& rhs)
		: ARBConfigActionRenameLifetimeName(rhs)
	{
	}
};


ARBConfigActionPtr ARBConfigActionRenameLifetimeName::New(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inOldName,
		std::wstring const& inNewName)
{
	return std::make_shared<ARBConfigActionRenameLifetimeName_concrete>(
		configVersion,
		inVenue,
		inOldName,
		inNewName);
}


ARBConfigActionRenameLifetimeName::ARBConfigActionRenameLifetimeName(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inOldName,
		std::wstring const& inNewName)
	: ARBConfigAction(configVersion)
	, m_Venue(inVenue)
	, m_OldName(inOldName)
	, m_NewName(inNewName)
{
}


ARBConfigActionRenameLifetimeName::ARBConfigActionRenameLifetimeName(ARBConfigActionRenameLifetimeName const& rhs)
	: ARBConfigAction(rhs)
	, m_Venue(rhs.m_Venue)
	, m_OldName(rhs.m_OldName)
	, m_NewName(rhs.m_NewName)
{
}


ARBConfigActionPtr ARBConfigActionRenameLifetimeName::Clone() const
{
	return std::make_shared<ARBConfigActionRenameLifetimeName_concrete>(
		m_configVersion,
		m_Venue,
		m_OldName,
		m_NewName);
}


bool ARBConfigActionRenameLifetimeName::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		fmt::wmemory_buffer& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	bool bChanged = false;

	ARBConfigVenuePtr venue;
	if (ioConfig.GetVenues().FindVenue(m_Venue, &venue))
	{
		ARBConfigLifetimeNamePtr oldLifetimeName;
		if (venue->GetLifetimeNames().FindLifetimeName(m_OldName, &oldLifetimeName))
		{
			bChanged = true;
			oldLifetimeName->SetName(m_NewName);
			int nEvents = venue->GetEvents().RenameLifetimeName(m_OldName, m_NewName);
			fmt::format_to(ioInfo, L"{}\n", Localization()->ActionRenameLifetimeName(m_Venue, m_OldName, m_NewName, nEvents));
			if (ioDogs)
				ioDogs->RenameLifetimeName(m_Venue, m_OldName, m_NewName);
		}
	}

	return bChanged;
}


/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionDeleteLifetimeName_concrete : public ARBConfigActionDeleteLifetimeName
{
public:
	ARBConfigActionDeleteLifetimeName_concrete(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inName)
		: ARBConfigActionDeleteLifetimeName(configVersion, inVenue, inName)
	{
	}
	ARBConfigActionDeleteLifetimeName_concrete(
			ARBConfigActionDeleteLifetimeName const& rhs)
		: ARBConfigActionDeleteLifetimeName(rhs)
	{
	}
};


ARBConfigActionPtr ARBConfigActionDeleteLifetimeName::New(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inName)
{
	return std::make_shared<ARBConfigActionDeleteLifetimeName_concrete>(
		configVersion,
		inVenue,
		inName);
}


ARBConfigActionDeleteLifetimeName::ARBConfigActionDeleteLifetimeName(
		short configVersion,
		std::wstring const& inVenue,
		std::wstring const& inName)
	: ARBConfigAction(configVersion)
	, m_Venue(inVenue)
	, m_Name(inName)
{
}


ARBConfigActionDeleteLifetimeName::ARBConfigActionDeleteLifetimeName(ARBConfigActionDeleteLifetimeName const& rhs)
	: ARBConfigAction(rhs)
	, m_Venue(rhs.m_Venue)
	, m_Name(rhs.m_Name)
{
}


ARBConfigActionPtr ARBConfigActionDeleteLifetimeName::Clone() const
{
	return std::make_shared<ARBConfigActionDeleteLifetimeName_concrete>(
		m_configVersion,
		m_Venue,
		m_Name);
}


bool ARBConfigActionDeleteLifetimeName::Apply(
		ARBConfig& ioConfig,
		ARBDogList* ioDogs,
		fmt::wmemory_buffer& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	bool bChanged = false;

	ARBConfigVenuePtr venue;
	if (ioConfig.GetVenues().FindVenue(m_Venue, &venue))
	{
		if (venue->GetLifetimeNames().DeleteLifetimeName(m_Name))
		{
			bChanged = true;
			int nEvents = venue->GetEvents().DeleteLifetimeName(m_Name);
			fmt::format_to(ioInfo, L"{}\n", Localization()->ActionDeleteLifetimeName(m_Venue, m_Name, nEvents));
			if (ioDogs)
				ioDogs->DeleteLifetimeName(m_Venue, m_Name);
		}
	}

	return bChanged;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigActionList::Load(
		ElementNodePtr const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_ACTION)
		return false;
	std::wstring verb;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_ACTION_VERB, verb)
	|| 0 == verb.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_ACTION, ATTRIB_ACTION_VERB));
		return false;
	}
	short configVer;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_ACTION_CONFIG, configVer))
	{
		if (inVersion < ARBVersion(12, 12))
		{
			configVer = 0;
		}
		else
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_ACTION, ATTRIB_ACTION_CONFIG));
			return false;
		}
	}
	std::wstring venue, div, oldName, newName;
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
		item = ARBConfigActionDeleteTitle::New(configVer, venue, div, oldName, newName);
	}
	else if (ACTION_VERB_RENAME_TITLE == verb)
	{
		item = ARBConfigActionRenameTitle::New(configVer, venue, oldName, newName);
	}
	else if (ACTION_VERB_DELETE_EVENT == verb)
	{
		item = ARBConfigActionDeleteEvent::New(configVer, venue, oldName);
	}
	else if (ACTION_VERB_RENAME_EVENT == verb)
	{
		item = ARBConfigActionRenameEvent::New(configVer, venue, oldName, newName);
	}
	else if (ACTION_VERB_RENAME_LEVEL == verb)
	{
		item = ARBConfigActionRenameLevel::NewLevel(configVer, venue, div, oldName, newName);
	}
	else if (ACTION_VERB_RENAME_DIV == verb)
	{
		item = ARBConfigActionRenameDivision::New(configVer, venue, oldName, newName);
	}
	else if (ACTION_VERB_RENAME_VENUE == verb)
	{
		item = ARBConfigActionRenameVenue::New(configVer, oldName, newName);
	}
	else
	{
		std::wstring msg(Localization()->ValidValues());
		msg += ACTION_VERB_DELETE_CALPLUGIN;
		msg += L", ";
		msg += ACTION_VERB_DELETE_TITLE;
		msg += L", ";
		msg += ACTION_VERB_RENAME_TITLE;
		msg += L", ";
		msg += ACTION_VERB_DELETE_EVENT;
		msg += L", ";
		msg += ACTION_VERB_RENAME_EVENT;
		msg += L", ";
		msg += ACTION_VERB_RENAME_LEVEL;
		msg += L", ";
		msg += ACTION_VERB_RENAME_DIV;
		msg += L", ";
		msg += ACTION_VERB_RENAME_VENUE;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_ACTION, ATTRIB_ACTION_VERB, msg.c_str()));
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
		fmt::wmemory_buffer& ioInfo,
		IConfigActionCallback& ioCallBack) const
{
	int nChanges = 0;
	for (ARBConfigActionList::const_iterator iterAction = begin();
			iterAction != end();
			++iterAction)
	{
		if (0 != (*iterAction)->GetVersion() && ioConfig.GetVersion() >= (*iterAction)->GetVersion())
			continue;
		if ((*iterAction)->Apply(ioConfig, ioDogs, ioInfo, ioCallBack))
			++nChanges;
		if (!ioCallBack.CanContinue())
			break;
	}
	if (0 < nChanges)
		fmt::format_to(ioInfo, L"\n");
	return nChanges;
}


bool ARBConfigActionList::Update(
		short configVersionPreUpdate,
		ARBConfig const& inConfigCurrent,
		std::wstring& ioVenue,
		std::wstring& ioDivision,
		std::wstring& ioSubLevel) const
{
	std::wstring inVenue(ioVenue);
	std::wstring inDivision(ioDivision);
	std::wstring inSubLevel(ioSubLevel);
	for (ARBConfigActionList::const_iterator iterAction = begin();
			iterAction != end();
			++iterAction)
	{
		if (0 != (*iterAction)->GetVersion() && configVersionPreUpdate >= (*iterAction)->GetVersion())
			continue;
		(*iterAction)->Update(inConfigCurrent, ioVenue, ioDivision, ioSubLevel);
	}
	return inVenue != ioVenue ||
		inDivision != ioDivision ||
		inSubLevel != ioSubLevel;
}
