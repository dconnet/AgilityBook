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
 * @li 2012-12-29 DRC Move icon management to global list.
 * @li 2009-01-27 DRC Ported to wxWidgets.
 * @li 2007-04-22 DRC Created.
 */

#include "stdafx.h"
#include "IconList.h"

#include "AgilityBook.h"
#include "ARBConfig.h"
#include "ARBDogTrial.h"

#include "res/AccConfirm.xpm"
#include "res/AccNone.xpm"
#include "res/AccTodo.xpm"
#include "res/AgilityBook16.xpm"
#include "res/CalEmpty.xpm"
#include "res/calendar.xpm"
#include "res/CalEntered.xpm"
#include "res/CalEnteredTentative.xpm"
#include "res/CalPending.xpm"
#include "res/CalPendingTentative.xpm"
#include "res/CalPlan.xpm"
#include "res/CalPlanTentative.xpm"
#include "res/CalTentative.xpm"
#include "res/checked.xpm"
#include "res/dog.xpm"
#include "res/HdrDown.xpm"
#include "res/HdrUp.xpm"
#include "res/hidden.xpm"
#include "res/NoteAdded.xpm"
#include "res/NoteNote.xpm"
#include "res/NoteNoteAdded.xpm"
#include "res/points.xpm"
#include "res/run.xpm"
#include "res/title-hidden-have.xpm"
#include "res/title-hidden.xpm"
#include "res/title-visible-have.xpm"
#include "res/title-visible.xpm"
#include "res/training.xpm"
#include "res/trial.xpm"
#include "res/unchecked.xpm"
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
			assert(icons[idx].assertIndex == *icons[idx].index);
		}
#endif
	}
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


wxIcon CIconList::ARB()
{
	return wxIcon(AgilityBook16_xpm);
}


wxIcon CIconList::TabRuns()
{
	return wxIcon(run_xpm);
}


wxIcon CIconList::TabPoints()
{
	return wxIcon(points_xpm);
}


wxIcon CIconList::TabCalendar()
{
	return wxIcon(calendar_xpm);
}


wxIcon CIconList::TabTraining()
{
	return wxIcon(training_xpm);
}


wxIcon CIconList::Dog()
{
	return wxIcon(dog_xpm);
}


wxIcon CIconList::Run()
{
	return wxIcon(run_xpm);
}


wxIcon CIconList::Blank()
{
	return wxIcon(CalEmpty_xpm);
}


wxIcon CIconList::Checked()
{
	return wxIcon(checked_xpm);

}


wxIcon CIconList::UnChecked()
{
	return wxIcon(unchecked_xpm);
}


wxIcon CIconList::Check()
{
	return wxIcon(CalPlan_xpm);
}


wxIcon CIconList::Question()
{
	return wxIcon(CalTentative_xpm);
}


wxIcon CIconList::AccConfirm()
{
	return wxIcon(AccConfirm_xpm);
}


wxIcon CIconList::AccNone()
{
	return wxIcon(AccNone_xpm);
}


wxIcon CIconList::AccTodo()
{
	return wxIcon(AccTodo_xpm);
}


wxIcon CIconList::CalEntered()
{
	return wxIcon(CalEntered_xpm);
}


wxIcon CIconList::CalEnteredTentative()
{
	return wxIcon(CalEnteredTentative_xpm);
}


wxIcon CIconList::CalPending()
{
	return wxIcon(CalPending_xpm);
}


wxIcon CIconList::CalPendingTentative()
{
	return wxIcon(CalPendingTentative_xpm);
}


wxIcon CIconList::CalPlanning()
{
	return wxIcon(CalPlan_xpm);
}


wxIcon CIconList::CalPlanTentative()
{
	return wxIcon(CalPlanTentative_xpm);
}


wxIcon CIconList::CalTentative()
{
	return wxIcon(CalTentative_xpm);
}


wxIcon CIconList::HeaderDown()
{
	return wxIcon(HdrDown_xpm);
}


wxIcon CIconList::HeaderUp()
{
	return wxIcon(HdrUp_xpm);
}


wxIcon CIconList::NoteAdded()
{
	return wxIcon(NoteAdded_xpm);
}


wxIcon CIconList::NoteNote()
{
	return wxIcon(NoteNote_xpm);
}


wxIcon CIconList::NoteNoteAdded()
{
	return wxIcon(NoteNoteAdded_xpm);
}


wxIcon CIconList::Hidden()
{
	return wxIcon(hidden_xpm);
}


wxIcon CIconList::TitledHidden()
{
	return wxIcon(title_hidden_xpm);
}


wxIcon CIconList::TitledHiddenHave()
{
	return wxIcon(title_hidden_have_xpm);
}


wxIcon CIconList::Title()
{
	return wxIcon(title_visible_xpm);
}


wxIcon CIconList::TitleHave()
{
	return wxIcon(title_visible_have_xpm);
}
