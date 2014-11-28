/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Image Manager
 * @author David Connet
 *
 * Revision History
 * 2013-10-13 Changed ImageManager to an art provider.
 * 2012-12-29 Created (separated from IconList).
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "AgilityBookImages.h"

#include <wx/artprov.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


bool CAgilityBookApp::OnCreateBitmap(
		const wxArtID& id,
		const wxArtClient& client,
		const wxSize& size,
		wxBitmap& outBmp)
{
#ifdef _DEBUG
	bool bCheckBmp = true;
#endif

	if (id == ImageMgrApp)
	{
		if (client == wxART_MESSAGE_BOX)
			outBmp = wxBITMAP_PNG(AgilityBook32);
		else
			outBmp = wxBITMAP_PNG(AgilityBook16);
	}
	else if (id == ImageMgrApp48)
		outBmp = wxBITMAP_PNG(AgilityBook48);
	else if (id == ImageMgrApp256)
		outBmp = wxBITMAP_PNG(AgilityBook256);

	else if (id == ImageMgrRuns)
	{
		if (client == wxART_TOOLBAR)
			outBmp = wxBITMAP_PNG(toolbarRun);
		else
			outBmp = wxBITMAP_PNG(run);
	}

	else if (id == ImageMgrPoints)
		outBmp = wxBITMAP_PNG(points);

	else if (id == ImageMgrCalendar)
	{
		if (client == wxART_TOOLBAR)
			outBmp = wxBITMAP_PNG(toolbarCalendar);
		else
			outBmp = wxBITMAP_PNG(calendar);
	}

	else if (id == ImageMgrTraining)
	{
		if (client == wxART_TOOLBAR)
			outBmp = wxBITMAP_PNG(toolbarTraining);
		else
			outBmp = wxBITMAP_PNG(training);
	}

	else if (id == ImageMgrDog)
	{
		if (client == wxART_TOOLBAR)
			outBmp = wxBITMAP_PNG(toolbarDog);
		else
			outBmp = wxBITMAP_PNG(dog);
	}

	else if (id == ImageMgrTrial)
	{
		if (client == wxART_TOOLBAR)
			outBmp = wxBITMAP_PNG(toolbarTrial);
		else
			outBmp = wxBITMAP_PNG(trial);
	}

	else if (id == ImageMgrVenueAAC)
		outBmp = wxBITMAP_PNG(venue_aac);
	else if (id == ImageMgrVenueAKC)
		outBmp = wxBITMAP_PNG(venue_akc);
	else if (id == ImageMgrVenueASCA)
		outBmp = wxBITMAP_PNG(venue_asca);
	else if (id == ImageMgrVenueCKC)
		outBmp = wxBITMAP_PNG(venue_ckc);
	else if (id == ImageMgrVenueCPE)
		outBmp = wxBITMAP_PNG(venue_cpe);
	else if (id == ImageMgrVenueDOCNA)
		outBmp = wxBITMAP_PNG(venue_docna);
	else if (id == ImageMgrVenueFCI)
		outBmp = wxBITMAP_PNG(venue_fci);
	else if (id == ImageMgrVenueNADAC)
		outBmp = wxBITMAP_PNG(venue_nadac);
	else if (id == ImageMgrVenueSCC)
		outBmp = wxBITMAP_PNG(venue_scc);
	else if (id == ImageMgrVenueSweepstakes)
		outBmp = wxBITMAP_PNG(venue_sweep);
	else if (id == ImageMgrVenueTDAA)
		outBmp = wxBITMAP_PNG(venue_tdaa);
	else if (id == ImageMgrVenueUKC)
		outBmp = wxBITMAP_PNG(venue_ukc);
	else if (id == ImageMgrVenueUSDAA)
		outBmp = wxBITMAP_PNG(venue_usdaa);

	else if (id == ImageMgrCRCD)
		outBmp = wxBITMAP_PNG(crcd);
	else if (id == ImageMgrBlank)
		outBmp = wxBITMAP_PNG(CalEmpty);
	else if (id == ImageMgrChecked)
		outBmp = wxBITMAP_PNG(checked);
	else if (id == ImageMgrUnChecked)
		outBmp = wxBITMAP_PNG(unchecked);
	else if (id == ImageMgrAccomConfirm)
		outBmp = wxBITMAP_PNG(AccConfirm);
	else if (id == ImageMgrAccomNone)
		outBmp = wxBITMAP_PNG(AccNone);
	else if (id == ImageMgrAccomTodo)
		outBmp = wxBITMAP_PNG(AccTodo);
	else if (id == ImageMgrCalEntered)
		outBmp = wxBITMAP_PNG(CalEntered);
	else if (id == ImageMgrCalEnteredTentative)
		outBmp = wxBITMAP_PNG(CalEnteredTentative);
	else if (id == ImageMgrCalPending)
		outBmp = wxBITMAP_PNG(CalPending);
	else if (id == ImageMgrCalPendingTentative)
		outBmp = wxBITMAP_PNG(CalPendingTentative);
	else if (id == ImageMgrCheck)
		outBmp = wxBITMAP_PNG(CalPlan);
	else if (id == ImageMgrCalPlanTentative)
		outBmp = wxBITMAP_PNG(CalPlanTentative);
	else if (id == ImageMgrQuestion)
		outBmp = wxBITMAP_PNG(CalTentative);
	else if (id == ImageMgrHeaderDown)
		outBmp = wxBITMAP_PNG(HdrDown);
	else if (id == ImageMgrHeaderUp)
		outBmp = wxBITMAP_PNG(HdrUp);

	else if (id == ImageMgrTitle)
		outBmp = wxBITMAP_PNG(toolbarTitle);
	else if (id == ImageMgrTitleHidden)
		outBmp = wxBITMAP_PNG(hidden);
	else if (id == ImageMgrTitleTitledHidden)
		outBmp = wxBITMAP_PNG(title_hidden);
	else if (id == ImageMgrTitleTitledHiddenHave)
		outBmp = wxBITMAP_PNG(title_hidden_have);
	else if (id == ImageMgrTitleTitled)
		outBmp = wxBITMAP_PNG(title_visible);
	else if (id == ImageMgrTitleTitledHave)
		outBmp = wxBITMAP_PNG(title_visible_have);

	else if (id == ImageMgrNoteButton)
		outBmp = wxBITMAP_PNG(Note);
	else if (id == ImageMgrInfoNote)
		outBmp = wxBITMAP_PNG(NoteNote);
	else if (id == ImageMgrInfoNoteAdded)
		outBmp = wxBITMAP_PNG(NoteAdded);
	else if (id == ImageMgrInfoNoteNoteAdded)
		outBmp = wxBITMAP_PNG(NoteNoteAdded);
	
	else if (id == ImageMgrAbout)
		outBmp = wxBITMAP_PNG(toolbarAbout);
	else if (id == ImageMgrCopy)
		outBmp = wxBITMAP_PNG(toolbarCopy);
	else if (id == ImageMgrCut)
		outBmp = wxBITMAP_PNG(toolbarCut);
	else if (id == ImageMgrNew)
		outBmp = wxBITMAP_PNG(toolbarNew);
	else if (id == ImageMgrOpen)
		outBmp = wxBITMAP_PNG(toolbarOpen);
	else if (id == ImageMgrPaste)
		outBmp = wxBITMAP_PNG(toolbarPaste);
	else if (id == ImageMgrPreview)
		outBmp = wxBITMAP_PNG(toolbarPreview);
	else if (id == ImageMgrPrint)
		outBmp = wxBITMAP_PNG(toolbarPrint);
	else if (id == ImageMgrSave)
		outBmp = wxBITMAP_PNG(toolbarSave);

	else
	{
#ifdef _DEBUG
		if (id == wxART_FILE_SAVE_AS)
			;
		else
			assert(0);
		bCheckBmp = false;
#endif
	}

#ifdef _DEBUG
	if (bCheckBmp)
	{
		assert(outBmp.IsOk());
	}
#endif
	return outBmp.IsOk();
}


bool CAgilityBookApp::OnCreateIconBundle(
		const wxArtID& id,
		const wxArtClient& client,
		wxIconBundle& outIcon)
{
	if (id == ImageMgrAppBundle)
	{
		outIcon.AddIcon(CImageManager::Get()->GetIcon(ImageMgrApp));
		outIcon.AddIcon(CImageManager::Get()->GetIcon(ImageMgrApp, wxART_MESSAGE_BOX));
		outIcon.AddIcon(CImageManager::Get()->GetIcon(ImageMgrApp48));
		outIcon.AddIcon(CImageManager::Get()->GetIcon(ImageMgrApp256));
		return true;
	}
	return false;
}
