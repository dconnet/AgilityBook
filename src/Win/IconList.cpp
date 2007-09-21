/*
 * Copyright © 2007 David Connet. All Rights Reserved.
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
 * @li 2007-04-22 DRC Created.
 */

#include "stdafx.h"
#include "resource.h"
#include "IconList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CIconList::CIconList()
{
	struct
	{
		int* index;
		UINT resId;
	} icons[] =
	{
		// Note: The order of these icons CANNOT be changed. The index
		// is saved in the ARB file to associate an icon with a venue.
		{&m_idxDog,				IDI_DOG},
		{&m_idxTrial,			IDI_TRIAL},
		{&m_idxRun,				IDI_RUN},
		{&m_idxVenueAAC,		IDI_VENUE_AAC},
		{&m_idxVenueAKC,		IDI_VENUE_AKC},
		{&m_idxVenueASCA,		IDI_VENUE_ASCA},
		{&m_idxVenueCKC,		IDI_VENUE_CKC},
		{&m_idxVenueCPE,		IDI_VENUE_CPE},
		{&m_idxVenueDOCNA,		IDI_VENUE_DOCNA},
		{&m_idxVenueNADAC,		IDI_VENUE_NADAC},
		{&m_idxVenueSweep,		IDI_VENUE_SWEEP},
		{&m_idxVenueTDAA,		IDI_VENUE_TDAA},
		{&m_idxVenueUKC,		IDI_VENUE_UKC},
		{&m_idxVenueUSDAA,		IDI_VENUE_USDAA},
		{&m_idxVenueFCI,		IDI_VENUE_FCI},
		{&m_idxVenueSCC,		IDI_VENUE_SCC},
	};
	int nIcons = sizeof(icons) / sizeof(icons[0]);
	CWinApp* app = AfxGetApp();
	Create(16, 16, ILC_MASK | ILC_COLOR32, nIcons, 1);
	for (int idx = 0; idx < nIcons; ++idx)
	{
		*icons[idx].index = Add(app->LoadIcon(icons[idx].resId));
	}
}
