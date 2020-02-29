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
 * 2017-12-01 Tweak scaling to avoid roundoff.
 * 2015-08-22 Expose LOAD_BITMAP_PNG for use in OnCreateBitmap.
 * 2013-10-13 Changed ImageManager to an art provider.
 * 2012-12-29 Created (separated from IconList).
 */

#include "stdafx.h"
#include "ImageHelper.h"

#include "LibARBWin/DPI.h"
#include <wx/artprov.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

 // Copy test from gdicmn.h for wxBITMAP_PNG
#if !((defined(__WINDOWS__) && wxUSE_WXDIB) || defined(__WXOSX__))
#include "images/AccConfirm_png.c"
#include "images/AccNone_png.c"
#include "images/AccTodo_png.c"
#include "images/AgilityBook16_png.c"
#include "images/AgilityBook256_png.c"
#include "images/AgilityBook32_png.c"
#include "images/AgilityBook48_png.c"
#include "images/CalEmpty_png.c"
#include "images/calendar_png.c"
#include "images/CalEntered_png.c"
#include "images/CalEnteredTentative_png.c"
#include "images/CalPending_png.c"
#include "images/CalPendingTentative_png.c"
#include "images/CalPlan_png.c"
#include "images/CalPlanTentative_png.c"
#include "images/CalTentative_png.c"
#include "images/checked_png.c"
#include "images/crcd_png.c"
#include "images/dog_png.c"
#include "images/HdrDown_png.c"
#include "images/HdrUp_png.c"
#include "images/hidden_png.c"
#include "images/Note_png.c"
#include "images/NoteAdded_png.c"
#include "images/NoteNote_png.c"
#include "images/NoteNoteAdded_png.c"
#include "images/points_png.c"
#include "images/run_png.c"
#include "images/title_hidden_have_png.c"
#include "images/title_hidden_png.c"
#include "images/title_visible_have_png.c"
#include "images/title_visible_png.c"
#include "images/toolbarAbout_png.c"
#include "images/toolbarCalendar_png.c"
#include "images/toolbarCopy_png.c"
#include "images/toolbarCut_png.c"
#include "images/toolbarDelete_png.c"
#include "images/toolbarDog_png.c"
#include "images/toolbarNew_png.c"
#include "images/toolbarOpen_png.c"
#include "images/toolbarPaste_png.c"
#include "images/toolbarPreview_png.c"
#include "images/toolbarPrint_png.c"
#include "images/toolbarRun_png.c"
#include "images/toolbarSave_png.c"
#include "images/toolbarTitle_png.c"
#include "images/toolbarTraining_png.c"
#include "images/toolbarTrial_png.c"
#include "images/training_png.c"
#include "images/trial_png.c"
#include "images/unchecked_png.c"
#include "images/venue_aac_png.c"
#include "images/venue_akc_png.c"
#include "images/venue_asca_png.c"
#include "images/venue_bha_png.c"
#include "images/venue_ckc_png.c"
#include "images/venue_ckcsc_png.c"
#include "images/venue_cpe_png.c"
#include "images/venue_docna_png.c"
#include "images/venue_fci_png.c"
#include "images/venue_nadac_png.c"
#include "images/venue_scc_png.c"
#include "images/venue_sweep_png.c"
#include "images/venue_tdaa_png.c"
#include "images/venue_ukc_png.c"
#include "images/venue_uki_png.c"
#include "images/venue_usdaa_png.c"
#include "images/venue_valor_png.c"
#endif


bool CImageHelper::DoCreateBitmap(
		wxWindow const* pWindow,
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
			LOAD_BITMAP_PNG(pWindow, AgilityBook32, outBmp);
		else
			LOAD_BITMAP_PNG(pWindow, AgilityBook16, outBmp);
	}
	else if (id == ImageMgrApp48)
		LOAD_BITMAP_PNG(pWindow, AgilityBook48, outBmp);
	else if (id == ImageMgrApp256)
		LOAD_BITMAP_PNG(pWindow, AgilityBook256, outBmp);

	else if (id == ImageMgrRuns)
	{
		if (client == wxART_TOOLBAR)
			LOAD_BITMAP_PNG(pWindow, toolbarRun, outBmp);
		else
			LOAD_BITMAP_PNG(pWindow, run, outBmp);
	}

	else if (id == ImageMgrPoints)
		LOAD_BITMAP_PNG(pWindow, points, outBmp);

	else if (id == ImageMgrCalendar)
	{
		if (client == wxART_TOOLBAR)
			LOAD_BITMAP_PNG(pWindow, toolbarCalendar, outBmp);
		else
			LOAD_BITMAP_PNG(pWindow, calendar, outBmp);
	}

	else if (id == ImageMgrTraining)
	{
		if (client == wxART_TOOLBAR)
			LOAD_BITMAP_PNG(pWindow, toolbarTraining, outBmp);
		else
			LOAD_BITMAP_PNG(pWindow, training, outBmp);
	}

	else if (id == ImageMgrDog)
	{
		if (client == wxART_TOOLBAR)
			LOAD_BITMAP_PNG(pWindow, toolbarDog, outBmp);
		else
			LOAD_BITMAP_PNG(pWindow, dog, outBmp);
	}

	else if (id == ImageMgrTrial)
	{
		if (client == wxART_TOOLBAR)
			LOAD_BITMAP_PNG(pWindow, toolbarTrial, outBmp);
		else
			LOAD_BITMAP_PNG(pWindow, trial, outBmp);
	}

	else if (id == ImageMgrVenueAAC)
		LOAD_BITMAP_PNG(pWindow, venue_aac, outBmp);
	else if (id == ImageMgrVenueAKC)
		LOAD_BITMAP_PNG(pWindow, venue_akc, outBmp);
	else if (id == ImageMgrVenueASCA)
		LOAD_BITMAP_PNG(pWindow, venue_asca, outBmp);
	else if (id == ImageMgrVenueBHA)
		LOAD_BITMAP_PNG(pWindow, venue_bha, outBmp);
	else if (id == ImageMgrVenueCKC)
		LOAD_BITMAP_PNG(pWindow, venue_ckc, outBmp);
	else if (id == ImageMgrVenueCKCSC)
		LOAD_BITMAP_PNG(pWindow, venue_ckcsc, outBmp);
	else if (id == ImageMgrVenueCPE)
		LOAD_BITMAP_PNG(pWindow, venue_cpe, outBmp);
	else if (id == ImageMgrVenueDOCNA)
		LOAD_BITMAP_PNG(pWindow, venue_docna, outBmp);
	else if (id == ImageMgrVenueFCI)
		LOAD_BITMAP_PNG(pWindow, venue_fci, outBmp);
	else if (id == ImageMgrVenueNADAC)
		LOAD_BITMAP_PNG(pWindow, venue_nadac, outBmp);
	else if (id == ImageMgrVenueSCC)
		LOAD_BITMAP_PNG(pWindow, venue_scc, outBmp);
	else if (id == ImageMgrVenueSweepstakes)
		LOAD_BITMAP_PNG(pWindow, venue_sweep, outBmp);
	else if (id == ImageMgrVenueTDAA)
		LOAD_BITMAP_PNG(pWindow, venue_tdaa, outBmp);
	else if (id == ImageMgrVenueUKC)
		LOAD_BITMAP_PNG(pWindow, venue_ukc, outBmp);
	else if (id == ImageMgrVenueUKI)
		LOAD_BITMAP_PNG(pWindow, venue_uki, outBmp);
	else if (id == ImageMgrVenueUSDAA)
		LOAD_BITMAP_PNG(pWindow, venue_usdaa, outBmp);
	else if (id == ImageMgrVenueVALOR)
		LOAD_BITMAP_PNG(pWindow, venue_valor, outBmp);

	else if (id == ImageMgrCRCD)
		LOAD_BITMAP_PNG(pWindow, crcd, outBmp);
	else if (id == ImageMgrBlank)
		LOAD_BITMAP_PNG(pWindow, CalEmpty, outBmp);
	else if (id == ImageMgrChecked)
		LOAD_BITMAP_PNG(pWindow, checked, outBmp);
	else if (id == ImageMgrUnChecked)
		LOAD_BITMAP_PNG(pWindow, unchecked, outBmp);
	else if (id == ImageMgrAccomConfirm)
		LOAD_BITMAP_PNG(pWindow, AccConfirm, outBmp);
	else if (id == ImageMgrAccomNone)
		LOAD_BITMAP_PNG(pWindow, AccNone, outBmp);
	else if (id == ImageMgrAccomTodo)
		LOAD_BITMAP_PNG(pWindow, AccTodo, outBmp);
	else if (id == ImageMgrCalEntered)
		LOAD_BITMAP_PNG(pWindow, CalEntered, outBmp);
	else if (id == ImageMgrCalEnteredTentative)
		LOAD_BITMAP_PNG(pWindow, CalEnteredTentative, outBmp);
	else if (id == ImageMgrCalPending)
		LOAD_BITMAP_PNG(pWindow, CalPending, outBmp);
	else if (id == ImageMgrCalPendingTentative)
		LOAD_BITMAP_PNG(pWindow, CalPendingTentative, outBmp);
	else if (id == ImageMgrCheck)
		LOAD_BITMAP_PNG(pWindow, CalPlan, outBmp);
	else if (id == ImageMgrCalPlanTentative)
		LOAD_BITMAP_PNG(pWindow, CalPlanTentative, outBmp);
	else if (id == ImageMgrQuestion)
		LOAD_BITMAP_PNG(pWindow, CalTentative, outBmp);
	else if (id == ImageMgrHeaderDown)
		LOAD_BITMAP_PNG(pWindow, HdrDown, outBmp);
	else if (id == ImageMgrHeaderUp)
		LOAD_BITMAP_PNG(pWindow, HdrUp, outBmp);

	else if (id == ImageMgrTitle)
		LOAD_BITMAP_PNG(pWindow, toolbarTitle, outBmp);
	else if (id == ImageMgrTitleHidden)
		LOAD_BITMAP_PNG(pWindow, hidden, outBmp);
	else if (id == ImageMgrTitleTitledHidden)
		LOAD_BITMAP_PNG(pWindow, title_hidden, outBmp);
	else if (id == ImageMgrTitleTitledHiddenHave)
		LOAD_BITMAP_PNG(pWindow, title_hidden_have, outBmp);
	else if (id == ImageMgrTitleTitled)
		LOAD_BITMAP_PNG(pWindow, title_visible, outBmp);
	else if (id == ImageMgrTitleTitledHave)
		LOAD_BITMAP_PNG(pWindow, title_visible_have, outBmp);

	else if (id == ImageMgrNoteButton)
		LOAD_BITMAP_PNG(pWindow, Note, outBmp);
	else if (id == ImageMgrInfoNote)
		LOAD_BITMAP_PNG(pWindow, NoteNote, outBmp);
	else if (id == ImageMgrInfoNoteAdded)
		LOAD_BITMAP_PNG(pWindow, NoteAdded, outBmp);
	else if (id == ImageMgrInfoNoteNoteAdded)
		LOAD_BITMAP_PNG(pWindow, NoteNoteAdded, outBmp);
	
	else if (id == ImageMgrAbout)
		LOAD_BITMAP_PNG(pWindow, toolbarAbout, outBmp);
	else if (id == ImageMgrCopy)
		LOAD_BITMAP_PNG(pWindow, toolbarCopy, outBmp);
	else if (id == ImageMgrCut)
		LOAD_BITMAP_PNG(pWindow, toolbarCut, outBmp);
	else if (id == ImageMgrDelete)
		LOAD_BITMAP_PNG(pWindow, toolbarDelete, outBmp);
	else if (id == ImageMgrNew)
		LOAD_BITMAP_PNG(pWindow, toolbarNew, outBmp);
	else if (id == ImageMgrOpen)
		LOAD_BITMAP_PNG(pWindow, toolbarOpen, outBmp);
	else if (id == ImageMgrPaste)
		LOAD_BITMAP_PNG(pWindow, toolbarPaste, outBmp);
	else if (id == ImageMgrPreview)
		LOAD_BITMAP_PNG(pWindow, toolbarPreview, outBmp);
	else if (id == ImageMgrPrint)
		LOAD_BITMAP_PNG(pWindow, toolbarPrint, outBmp);
	else if (id == ImageMgrSave)
		LOAD_BITMAP_PNG(pWindow, toolbarSave, outBmp);

	else
	{
#ifdef _DEBUG
		if (id == wxART_FILE_SAVE_AS
		|| id == wxART_INFORMATION
		|| id == wxART_PRINT
		|| id == wxART_GOTO_FIRST
		|| id == wxART_GOTO_LAST
		|| id == wxART_GO_BACK
		|| id == wxART_GO_FORWARD
		|| id == wxART_MINUS
		|| id == wxART_PLUS
		|| id == wxART_WARNING
		|| id == wxART_ERROR)
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


bool CImageHelper::DoCreateIconBundle(
		wxWindow const* pWindow,
		const wxArtID& id,
		const wxArtClient& client,
		wxIconBundle& outIcon)
{
	if (id == ImageMgrAppBundle)
	{
		outIcon.AddIcon(CImageManager::Get()->GetIcon(ImageMgrApp, wxART_OTHER, ImageHelper::GetScaledSize(pWindow, 16)));
		outIcon.AddIcon(CImageManager::Get()->GetIcon(ImageMgrApp, wxART_MESSAGE_BOX, ImageHelper::GetScaledSize(pWindow, 32)));
		outIcon.AddIcon(CImageManager::Get()->GetIcon(ImageMgrApp48, wxART_OTHER, ImageHelper::GetScaledSize(pWindow, 48)));
		outIcon.AddIcon(CImageManager::Get()->GetIcon(ImageMgrApp256, wxART_OTHER, ImageHelper::GetScaledSize(pWindow, 256)));
		return true;
	}
	return false;
}
