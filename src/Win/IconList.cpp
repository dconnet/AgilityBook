/*
 * Copyright � 2007-2009 David Connet. All Rights Reserved.
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
 * @brief List of icons
 * @author David Connet
 *
 * Revision History
 * @li 2009-01-27 DRC Ported to wxWidgets.
 * @li 2007-04-22 DRC Created.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "IconList.h"

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
