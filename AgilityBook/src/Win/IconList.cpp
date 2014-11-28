/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief List of icons
 * @author David Connet
 *
 * Revision History
 * 2012-12-29 Move icon management to global list.
 * 2009-01-27 Ported to wxWidgets.
 * 2007-04-22 Created.
 */

#include "stdafx.h"
#include "IconList.h"

#include "AgilityBook.h"
#include "AgilityBookImages.h"

#include "ARB/ARBConfig.h"
#include "ARB/ARBDogTrial.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CIconList::CIconList()
	: wxImageList(16, 16)
{
	m_idxDog = Add(CImageManager::Get()->GetIcon(ImageMgrDog));
	m_idxTrial = Add(CImageManager::Get()->GetIcon(ImageMgrTrial));
	m_idxRun = Add(CImageManager::Get()->GetIcon(ImageMgrRuns));
	m_idxVenueAAC = Add(CImageManager::Get()->GetIcon(ImageMgrVenueAAC));
	m_idxVenueAKC = Add(CImageManager::Get()->GetIcon(ImageMgrVenueAKC));
	m_idxVenueASCA = Add(CImageManager::Get()->GetIcon(ImageMgrVenueASCA));
	m_idxVenueCKC = Add(CImageManager::Get()->GetIcon(ImageMgrVenueCKC));
	m_idxVenueCPE = Add(CImageManager::Get()->GetIcon(ImageMgrVenueCPE));
	m_idxVenueDOCNA = Add(CImageManager::Get()->GetIcon(ImageMgrVenueDOCNA));
	m_idxVenueNADAC = Add(CImageManager::Get()->GetIcon(ImageMgrVenueNADAC));
	m_idxVenueSweep = Add(CImageManager::Get()->GetIcon(ImageMgrVenueSweepstakes));
	m_idxVenueTDAA = Add(CImageManager::Get()->GetIcon(ImageMgrVenueTDAA));
	m_idxVenueUKC = Add(CImageManager::Get()->GetIcon(ImageMgrVenueUKC));
	m_idxVenueUSDAA = Add(CImageManager::Get()->GetIcon(ImageMgrVenueUSDAA));
	m_idxVenueFCI = Add(CImageManager::Get()->GetIcon(ImageMgrVenueFCI));
	m_idxVenueSCC = Add(CImageManager::Get()->GetIcon(ImageMgrVenueSCC));
	m_idxARB = Add(CImageManager::Get()->GetIcon(ImageMgrApp));

#ifdef _DEBUG
	struct
	{
		int* index;
		int assertIndex;
	} icons[] =
	{
		// Note: The order of these icons CANNOT be changed. The index
		// is saved in the ARB file to associate an icon with a venue.
		{&m_idxDog,				0},
		{&m_idxTrial,			1},
		{&m_idxRun,				2},
		{&m_idxVenueAAC,		3},
		{&m_idxVenueAKC,		4},
		{&m_idxVenueASCA,		5},
		{&m_idxVenueCKC,		6},
		{&m_idxVenueCPE,		7},
		{&m_idxVenueDOCNA,		8},
		{&m_idxVenueNADAC,		9},
		{&m_idxVenueSweep,		10},
		{&m_idxVenueTDAA,		11},
		{&m_idxVenueUKC,		12},
		{&m_idxVenueUSDAA,		13},
		{&m_idxVenueFCI,		14},
		{&m_idxVenueSCC,		15},
	};
	int nIcons = sizeof(icons) / sizeof(icons[0]);

	for (int idx = 0; idx < nIcons; ++idx)
	{
		assert(icons[idx].assertIndex == *icons[idx].index);
	}
#endif
}


int CIconList::Trial(ARBConfig& config, ARBDogTrialPtr pTrial) const
{
	int idxIcon = IndexTrial();
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
