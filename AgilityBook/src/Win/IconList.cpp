/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief List of icons
 * @author David Connet
 *
 * Revision History
 * @li 2009-01-27 DRC Ported to wxWidgets.
 * @li 2007-04-22 DRC Created.
 */

#include "stdafx.h"
#include "IconList.h"

#include "AgilityBook.h"
#include "ARBConfig.h"
#include "ARBDogTrial.h"

#include "res/AgilityBook16.xpm"
#include "res/dog.xpm"
#include "res/trial.xpm"
#include "res/run.xpm"
#include "res/venue-aac.xpm"
#include "res/venue-akc.xpm"
#include "res/venue-asca.xpm"
#include "res/venue-ckc.xpm"
#include "res/venue-cpe.xpm"
#include "res/venue-docna.xpm"
#include "res/venue-fci.xpm"
#include "res/venue-nadac.xpm"
#include "res/venue-scc.xpm"
#include "res/venue-sweep.xpm"
#include "res/venue-tdaa.xpm"
#include "res/venue-ukc.xpm"
#include "res/venue-usdaa.xpm"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CIconList::CIconList()
	: wxImageList(16, 16)
{
	struct
	{
		int* index;
		int assertIndex;
		const char** xpmData;
	} icons[] =
	{
		// Note: The order of these icons CANNOT be changed. The index
		// is saved in the ARB file to associate an icon with a venue.
		{&m_idxDog,				0,	dog_xpm},
		{&m_idxTrial,			1,	trial_xpm},
		{&m_idxRun,				2,	run_xpm},
		{&m_idxVenueAAC,		3,	venue_aac_xpm},
		{&m_idxVenueAKC,		4,	venue_akc_xpm},
		{&m_idxVenueASCA,		5,	venue_asca_xpm},
		{&m_idxVenueCKC,		6,	venue_ckc_xpm},
		{&m_idxVenueCPE,		7,	venue_cpe_xpm},
		{&m_idxVenueDOCNA,		8,	venue_docna_xpm},
		{&m_idxVenueNADAC,		9,	venue_nadac_xpm},
		{&m_idxVenueSweep,		10,	venue_sweep_xpm},
		{&m_idxVenueTDAA,		11,	venue_tdaa_xpm},
		{&m_idxVenueUKC,		12,	venue_ukc_xpm},
		{&m_idxVenueUSDAA,		13,	venue_usdaa_xpm},
		{&m_idxVenueFCI,		14,	venue_fci_xpm},
		{&m_idxVenueSCC,		15,	venue_scc_xpm},
		{&m_idxARB,				-1,	AgilityBook16_xpm},
	};
	int nIcons = sizeof(icons) / sizeof(icons[0]);

	for (int idx = 0; idx < nIcons; ++idx)
	{
		*icons[idx].index = Add(wxIcon(icons[idx].xpmData));
#ifdef _DEBUG
		if (0 <= icons[idx].assertIndex)
		{
			assert(icons[idx].assertIndex == *icons[idx].index );
		}
#endif
	}
}


int CIconList::Trial(ARBConfig& config, ARBDogTrialPtr pTrial) const
{
	int idxIcon = Trial();
	if (pTrial)
	{
		ARBDogClubPtr pClub;
		if (pTrial->GetClubs().GetPrimaryClub(&pClub))
		{
			ARBConfigVenuePtr pVenue;
			if (config.GetVenues().FindVenue(pClub->GetVenue(), &pVenue))
			{
				short idx = pVenue->GetIcon();
				if (0 <= idx && idx < GetImageCount())
					idxIcon = idx;
			}
		}
	}
	return idxIcon;
}
