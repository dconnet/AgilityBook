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
 * 2022-04-15 Use wx DPI support.
 * 2016-06-19 Add a blank icon to fix issue on Mac.
 * 2012-12-29 Move icon management to global list.
 * 2009-01-27 Ported to wxWidgets.
 * 2007-04-22 Created.
 */

#include "stdafx.h"
#include "IconList.h"

#include "AgilityBook.h"
#include "ImageHelper.h"

#include "ARB/ARBConfig.h"
#include "ARB/ARBDogTrial.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBWin;

CIconList::CIconList()
	: m_idxEmpty(-1)
	, m_idxARB(-1)
	, m_idxDog(-1)
	, m_idxTrial(-1)
	, m_idxRun(-1)
	, m_idxVenueAAC(-1)
	, m_idxVenueAKC(-1)
	, m_idxVenueASCA(-1)
	, m_idxVenueBHA(-1)
	, m_idxVenueCKC(-1)
	, m_idxVenueCKCSC(-1)
	, m_idxVenueCPE(-1)
	, m_idxVenueDOCNA(-1)
	, m_idxVenueFCI(-1)
	, m_idxVenueNADAC(-1)
	, m_idxVenueSCC(-1)
	, m_idxVenueSweep(-1)
	, m_idxVenueTDAA(-1)
	, m_idxVenueUKC(-1)
	, m_idxVenueUKI(-1)
	, m_idxVenueUSDAA(-1)
	, m_idxVenueVALOR(-1)
{
}


bool CIconList::Create(wxWindow const* pWindow)
{
	if (!wxImageList::Create(pWindow->FromDIP(16), pWindow->FromDIP(16)))
		return false;

	m_idxDog = Add(CResourceManager::Get()->GetIcon(ImageMgrDog));
	m_idxTrial = Add(CResourceManager::Get()->GetIcon(ImageMgrTrial));
	m_idxRun = Add(CResourceManager::Get()->GetIcon(ImageMgrRuns));
	m_idxVenueAAC = Add(CResourceManager::Get()->GetIcon(ImageMgrVenueAAC));
	m_idxVenueAKC = Add(CResourceManager::Get()->GetIcon(ImageMgrVenueAKC));
	m_idxVenueASCA = Add(CResourceManager::Get()->GetIcon(ImageMgrVenueASCA));
	m_idxVenueCKC = Add(CResourceManager::Get()->GetIcon(ImageMgrVenueCKC));
	m_idxVenueCPE = Add(CResourceManager::Get()->GetIcon(ImageMgrVenueCPE));
	m_idxVenueDOCNA = Add(CResourceManager::Get()->GetIcon(ImageMgrVenueDOCNA));
	m_idxVenueNADAC = Add(CResourceManager::Get()->GetIcon(ImageMgrVenueNADAC));
	m_idxVenueSweep = Add(CResourceManager::Get()->GetIcon(ImageMgrVenueSweepstakes));
	m_idxVenueTDAA = Add(CResourceManager::Get()->GetIcon(ImageMgrVenueTDAA));
	m_idxVenueUKC = Add(CResourceManager::Get()->GetIcon(ImageMgrVenueUKC));
	m_idxVenueUSDAA = Add(CResourceManager::Get()->GetIcon(ImageMgrVenueUSDAA));
	m_idxVenueFCI = Add(CResourceManager::Get()->GetIcon(ImageMgrVenueFCI));
	m_idxVenueSCC = Add(CResourceManager::Get()->GetIcon(ImageMgrVenueSCC));
	m_idxVenueBHA = Add(CResourceManager::Get()->GetIcon(ImageMgrVenueBHA));
	m_idxVenueCKCSC = Add(CResourceManager::Get()->GetIcon(ImageMgrVenueCKCSC));
	m_idxVenueUKI = Add(CResourceManager::Get()->GetIcon(ImageMgrVenueUKI));
	m_idxVenueVALOR = Add(CResourceManager::Get()->GetIcon(ImageMgrVenueVALOR));

	m_idxARB = Add(CResourceManager::Get()->GetIcon(ImageMgrApp));
	m_idxEmpty = Add(CResourceManager::Get()->GetIcon(ImageMgrBlank));

#if defined(_DEBUG) || defined(__WXDEBUG__)
	struct
	{
		int* index;
		int assertIndex;
	} icons[] = {
		// clang-format off
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
		{&m_idxVenueBHA,		16},
		{&m_idxVenueCKCSC,		17},
		{&m_idxVenueUKI,		18},
		{&m_idxVenueVALOR,		19},
		// clang-format on
	};
	int nIcons = sizeof(icons) / sizeof(icons[0]);

	for (int idx = 0; idx < nIcons; ++idx)
	{
		if (icons[idx].assertIndex != *icons[idx].index)
		{
			assert(0);
			return false;
		}
	}
#endif
	return true;
}


int CIconList::Trial(ARBConfig& config, ARBDogTrialPtr const& inTrial) const
{
	int idxIcon = IndexTrial();
	if (inTrial)
	{
		ARBDogClubPtr pClub = inTrial->GetClubs().GetMainClub();
		if (pClub)
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

} // namespace dconSoft
