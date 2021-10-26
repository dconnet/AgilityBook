#pragma once

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
 * Usage:
 * Users of this can extend by associating a callback class.
 *
 * Revision History
 * 2015-08-22 Expose LOAD_BITMAP_PNG for use in OnCreateBitmap.
 * 2013-10-13 Changed ImageManager to an art provider.
 * 2012-12-29 Created.
 */

#include "LibARBWin/ImageHelperBase.h"
#include "LibARBWin/ImageManager.h"


class CImageHelper
{
public:
	CImageHelper()
	{
	}

	bool DoCreateBitmap(
		wxWindow const* pWindow,
		const wxArtID& id,
		const wxArtClient& client,
		const wxSize& size,
		wxBitmap& outBmp);

	bool DoCreateIconBundle(
		wxWindow const* pWindow,
		const wxArtID& id,
		const wxArtClient& client,
		wxIconBundle& outIcon);
};


#define ImageMgrAppBundle MAKE_IMAGE_ID(ImageMgrAppBundle)
#define ImageMgrApp       MAKE_IMAGE_ID(ImageMgrApp)
#define ImageMgrApp48     MAKE_IMAGE_ID(ImageMgrApp48)
#define ImageMgrApp256    MAKE_IMAGE_ID(ImageMgrApp256)

#define ImageMgrRuns                MAKE_IMAGE_ID(ImageMgrRuns)
#define ImageMgrPoints              MAKE_IMAGE_ID(ImageMgrPoints)
#define ImageMgrCalendar            MAKE_IMAGE_ID(ImageMgrCalendar)
#define ImageMgrTraining            MAKE_IMAGE_ID(ImageMgrTraining)
#define ImageMgrDog                 MAKE_IMAGE_ID(ImageMgrDog)
#define ImageMgrTrial               MAKE_IMAGE_ID(ImageMgrTrial)
#define ImageMgrVenueAAC            MAKE_IMAGE_ID(ImageMgrVenueAAC)
#define ImageMgrVenueAKC            MAKE_IMAGE_ID(ImageMgrVenueAKC)
#define ImageMgrVenueASCA           MAKE_IMAGE_ID(ImageMgrVenueASCA)
#define ImageMgrVenueBHA            MAKE_IMAGE_ID(ImageMgrVenueBHA)
#define ImageMgrVenueCKC            MAKE_IMAGE_ID(ImageMgrVenueCKC)
#define ImageMgrVenueCKCSC          MAKE_IMAGE_ID(ImageMgrVenueCKCSC)
#define ImageMgrVenueCPE            MAKE_IMAGE_ID(ImageMgrVenueCPE)
#define ImageMgrVenueDOCNA          MAKE_IMAGE_ID(ImageMgrVenueDOCNA)
#define ImageMgrVenueFCI            MAKE_IMAGE_ID(ImageMgrVenueFCI)
#define ImageMgrVenueNADAC          MAKE_IMAGE_ID(ImageMgrVenueNADAC)
#define ImageMgrVenueSCC            MAKE_IMAGE_ID(ImageMgrVenueSCC)
#define ImageMgrVenueSweepstakes    MAKE_IMAGE_ID(ImageMgrVenueSweepstakes)
#define ImageMgrVenueTDAA           MAKE_IMAGE_ID(ImageMgrVenueTDAA)
#define ImageMgrVenueUKC            MAKE_IMAGE_ID(ImageMgrVenueUKC)
#define ImageMgrVenueUKI            MAKE_IMAGE_ID(ImageMgrVenueUKI)
#define ImageMgrVenueUSDAA          MAKE_IMAGE_ID(ImageMgrVenueUSDAA)
#define ImageMgrVenueVALOR          MAKE_IMAGE_ID(ImageMgrVenueVALOR)
#define ImageMgrCRCD                MAKE_IMAGE_ID(ImageMgrCRCD)
#define ImageMgrAccomConfirm        MAKE_IMAGE_ID(ImageMgrAccomConfirm)
#define ImageMgrAccomNone           MAKE_IMAGE_ID(ImageMgrAccomNone)
#define ImageMgrAccomTodo           MAKE_IMAGE_ID(ImageMgrAccomTodo)
#define ImageMgrCalEntered          MAKE_IMAGE_ID(ImageMgrCalEntered)
#define ImageMgrCalEnteredTentative MAKE_IMAGE_ID(ImageMgrCalEnteredTentative)
#define ImageMgrCalPending          MAKE_IMAGE_ID(ImageMgrCalPending)
#define ImageMgrCalPendingTentative MAKE_IMAGE_ID(ImageMgrCalPendingTentative)
#define ImageMgrCalPlanTentative    MAKE_IMAGE_ID(ImageMgrCalPlanTentative)

// Hidden: !
// Titled: ribbon
// Have: check
#define ImageMgrTitle                 MAKE_IMAGE_ID(ImageMgrTitle)
#define ImageMgrTitleHidden           MAKE_IMAGE_ID(ImageMgrTitleHidden)
#define ImageMgrTitleTitledHidden     MAKE_IMAGE_ID(ImageMgrTitleTitledHidden)
#define ImageMgrTitleTitledHiddenHave MAKE_IMAGE_ID(ImageMgrTitleTitledHiddenHave)
#define ImageMgrTitleTitled           MAKE_IMAGE_ID(ImageMgrTitleTitled)
#define ImageMgrTitleTitledHave       MAKE_IMAGE_ID(ImageMgrTitleTitledHave)

// Added: +
// Note: Yellow-sticky
#define ImageMgrNoteButton        MAKE_IMAGE_ID(ImageMgrNoteButton)
#define ImageMgrInfoNote          MAKE_IMAGE_ID(ImageMgrInfoNote)
#define ImageMgrInfoNoteAdded     MAKE_IMAGE_ID(ImageMgrInfoNoteAdded)
#define ImageMgrInfoNoteNoteAdded MAKE_IMAGE_ID(ImageMgrInfoNoteNoteAdded)

#define ImageMgrAbout    MAKE_IMAGE_ID(ImageMgrAbout)
#define ImageMgrCopy     MAKE_IMAGE_ID(ImageMgrCopy)
#define ImageMgrCut      MAKE_IMAGE_ID(ImageMgrCut)
#define ImageMgrDelete   MAKE_IMAGE_ID(ImageMgrDelete)
#define ImageMgrNew      MAKE_IMAGE_ID(ImageMgrNew)
#define ImageMgrOpen     MAKE_IMAGE_ID(ImageMgrOpen)
#define ImageMgrPaste    MAKE_IMAGE_ID(ImageMgrPaste)
#define ImageMgrPreview  MAKE_IMAGE_ID(ImageMgrPreview)
#define ImageMgrPrint    MAKE_IMAGE_ID(ImageMgrPrint)
#define ImageMgrSave     MAKE_IMAGE_ID(ImageMgrSave)
#define ImageMgrSearch   MAKE_IMAGE_ID(ImageMgrSearch)
#define ImageMgrSettings MAKE_IMAGE_ID(ImageMgrSettings)
