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
 * @li 2013-10-13 DRC Changed ImageManager to an art provider.
 * @li 2012-12-29 DRC Created (separated from IconList).
 */

#include "stdafx.h"
#include "ImageManager.h"

#include <wx/artprov.h>

#include "res/AccConfirm.xpm"
#include "res/AccNone.xpm"
#include "res/AccTodo.xpm"
#include "res/AgilityBook16.xpm"
#include "res/AgilityBook32.xpm"
#include "res/AgilityBook48.xpm"
#include "res/AgilityBook256.xpm"
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
#include "res/crcd.xpm"
#include "res/dog.xpm"
#include "res/HdrDown.xpm"
#include "res/HdrUp.xpm"
#include "res/hidden.xpm"
#include "res/Note.xpm"
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

#include "res/toolbarNew.xpm"
#include "res/toolbarOpen.xpm"
#include "res/toolbarSave.xpm"
#include "res/toolbarDog.xpm"
#include "res/toolbarTitle.xpm"
#include "res/toolbarTrial.xpm"
#include "res/toolbarRun.xpm"
#include "res/toolbarCalendar.xpm"
#include "res/toolbarTraining.xpm"
#include "res/toolbarCut.xpm"
#include "res/toolbarCopy.xpm"
#include "res/toolbarPaste.xpm"
#include "res/toolbarPreview.xpm"
#include "res/toolbarPrint.xpm"
#include "res/toolbarAbout.xpm"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CImageManager* CImageManager::Get()
{
	static CImageManager* mgr = NULL;
	if (!mgr)
	{
		// Note, this is cleanuped in wxWidget code for wxArtProviders
		mgr = new CImageManager();
		wxArtProvider::Push(mgr);
	}
	return mgr;
}


CImageManager::CImageManager()
{
}


CImageManager::~CImageManager()
{
}


wxBitmap CImageManager::CreateBitmap(
		const wxArtID& id,
		const wxArtClient& client,
		const wxSize& size)
{
	if (id == ImageMgrApp)
	{
		if (client == wxART_MESSAGE_BOX)
			return wxBitmap(AgilityBook32_xpm);
		else
			return wxBitmap(AgilityBook16_xpm);
	}

	else if (id == ImageMgrRuns)
	{
		if (client == wxART_TOOLBAR)
			return wxBitmap(toolbarRun_xpm);
		else
			return wxBitmap(run_xpm);
	}

	else if (id == ImageMgrPoints)
		return wxBitmap(points_xpm);

	else if (id == ImageMgrCalendar)
	{
		if (client == wxART_TOOLBAR)
			return wxBitmap(toolbarCalendar_xpm);
		else
			return wxBitmap(calendar_xpm);
	}

	else if (id == ImageMgrTraining)
	{
		if (client == wxART_TOOLBAR)
			return wxBitmap(toolbarTraining_xpm);
		else
			return wxBitmap(training_xpm);
	}

	else if (id == ImageMgrDog)
	{
		if (client == wxART_TOOLBAR)
			return wxBitmap(toolbarDog_xpm);
		else
			return wxBitmap(dog_xpm);
	}

	else if (id == ImageMgrTrial)
	{
		if (client == wxART_TOOLBAR)
			return wxBitmap(toolbarTrial_xpm);
		else
			return wxBitmap(trial_xpm);
	}

	else if (id == ImageMgrVenueAAC)
		return wxBitmap(venue_aac_xpm);
	else if (id == ImageMgrVenueAKC)
		return wxBitmap(venue_akc_xpm);
	else if (id == ImageMgrVenueASCA)
		return wxBitmap(venue_asca_xpm);
	else if (id == ImageMgrVenueCKC)
		return wxBitmap(venue_ckc_xpm);
	else if (id == ImageMgrVenueCPE)
		return wxBitmap(venue_cpe_xpm);
	else if (id == ImageMgrVenueDOCNA)
		return wxBitmap(venue_docna_xpm);
	else if (id == ImageMgrVenueNADAC)
		return wxBitmap(venue_nadac_xpm);
	else if (id == ImageMgrVenueSweepstakes)
		return wxBitmap(venue_sweep_xpm);
	else if (id == ImageMgrVenueTDAA)
		return wxBitmap(venue_tdaa_xpm);
	else if (id == ImageMgrVenueUKC)
		return wxBitmap(venue_ukc_xpm);
	else if (id == ImageMgrVenueUSDAA)
		return wxBitmap(venue_usdaa_xpm);
	else if (id == ImageMgrVenueFCI)
		return wxBitmap(venue_fci_xpm);
	else if (id == ImageMgrVenueSCC)
		return wxBitmap(venue_scc_xpm);

	else if (id == ImageMgrCRCD)
		return wxBitmap(crcd_xpm);
	else if (id == ImageMgrBlank)
		return wxBitmap(CalEmpty_xpm);
	else if (id == ImageMgrChecked)
		return wxBitmap(checked_xpm);
	else if (id == ImageMgrUnChecked)
		return wxBitmap(unchecked_xpm);
	else if (id == ImageMgrAccomConfirm)
		return wxBitmap(AccConfirm_xpm);
	else if (id == ImageMgrAccomNone)
		return wxBitmap(AccNone_xpm);
	else if (id == ImageMgrAccomTodo)
		return wxBitmap(AccTodo_xpm);
	else if (id == ImageMgrCalEntered)
		return wxBitmap(CalEntered_xpm);
	else if (id == ImageMgrCalEnteredTentative)
		return wxBitmap(CalEnteredTentative_xpm);
	else if (id == ImageMgrCalPending)
		return wxBitmap(CalPending_xpm);
	else if (id == ImageMgrCalPendingTentative)
		return wxBitmap(CalPendingTentative_xpm);
	else if (id == ImageMgrCheck)
		return wxBitmap(CalPlan_xpm);
	else if (id == ImageMgrCalPlanTentative)
		return wxBitmap(CalPlanTentative_xpm);
	else if (id == ImageMgrQuestion)
		return wxBitmap(CalTentative_xpm);
	else if (id == ImageMgrHeaderDown)
		return wxBitmap(HdrDown_xpm);
	else if (id == ImageMgrHeaderUp)
		return wxBitmap(HdrUp_xpm);

	else if (id == ImageMgrTitle)
		return wxBitmap(toolbarTitle_xpm);
	else if (id == ImageMgrTitleHidden)
		return wxBitmap(hidden_xpm);
	else if (id == ImageMgrTitleTitledHidden)
		return wxBitmap(title_hidden_xpm);
	else if (id == ImageMgrTitleTitledHiddenHave)
		return wxBitmap(title_hidden_have_xpm);
	else if (id == ImageMgrTitleTitled)
		return wxBitmap(title_visible_xpm);
	else if (id == ImageMgrTitleTitledHave)
		return wxBitmap(title_visible_have_xpm);

	else if (id == ImageMgrNoteButton)
		return wxBitmap(Note_xpm);
	else if (id == ImageMgrInfoNote)
		return wxBitmap(NoteNote_xpm);
	else if (id == ImageMgrInfoNoteAdded)
		return wxBitmap(NoteAdded_xpm);
	else if (id == ImageMgrInfoNoteNoteAdded)
		return wxBitmap(NoteNoteAdded_xpm);
	
	else if (id == ImageMgrAbout)
		return wxBitmap(toolbarAbout_xpm);
	else if (id == ImageMgrCopy)
		return wxBitmap(toolbarCopy_xpm);
	else if (id == ImageMgrCut)
		return wxBitmap(toolbarCut_xpm);
	else if (id == ImageMgrNew)
		return wxBitmap(toolbarNew_xpm);
	else if (id == ImageMgrOpen)
		return wxBitmap(toolbarOpen_xpm);
	else if (id == ImageMgrPaste)
		return wxBitmap(toolbarPaste_xpm);
	else if (id == ImageMgrPreview)
		return wxBitmap(toolbarPreview_xpm);
	else if (id == ImageMgrPrint)
		return wxBitmap(toolbarPrint_xpm);
	else if (id == ImageMgrSave)
		return wxBitmap(toolbarSave_xpm);

	else
		return wxArtProvider::CreateBitmap(id, client, size);
}


wxIconBundle CImageManager::CreateIconBundle(
		const wxArtID& id,
		const wxArtClient& client)
{
	if (id == ImageMgrAppBundle)
	{
		wxIconBundle icons;
		icons.AddIcon(wxIcon(AgilityBook16_xpm));
		icons.AddIcon(wxIcon(AgilityBook32_xpm));
		icons.AddIcon(wxIcon(AgilityBook48_xpm));
		icons.AddIcon(wxIcon(AgilityBook256_xpm));
		return icons;
	}
	else
		return wxArtProvider::CreateIconBundle(id, client);
}
