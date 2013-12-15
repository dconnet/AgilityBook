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
#include "ImageManager.h"

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

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


bool CImageManager::s_bNoReInit = false;
CImageManager* CImageManager::s_pImageManager = NULL;


CImageManager* CImageManager::Get()
{
	if (!s_pImageManager && !s_bNoReInit)
		s_pImageManager = new CImageManager();
	return s_pImageManager;
}


void CImageManager::Reset()
{
	Delete();
	s_bNoReInit = false;
}


void CImageManager::Delete()
{
	s_bNoReInit = true;
	delete s_pImageManager;
	s_pImageManager = NULL;
}


CImageManager::CImageManager()
	: m_iconARB16(AgilityBook16_xpm)
	, m_iconARB32(AgilityBook32_xpm)
	, m_iconARB48(AgilityBook48_xpm)
	, m_iconARB256(AgilityBook256_xpm)
	, m_iconRun(run_xpm)
	, m_iconPoints(points_xpm)
	, m_iconCal(calendar_xpm)
	, m_iconLog(training_xpm)
	, m_iconDog(dog_xpm)
	, m_iconTrial(trial_xpm)
	, m_iconVenueAAC(venue_aac_xpm)
	, m_iconVenueAKC(venue_akc_xpm)
	, m_iconVenueASCA(venue_asca_xpm)
	, m_iconVenueCKC(venue_ckc_xpm)
	, m_iconVenueCPE(venue_cpe_xpm)
	, m_iconVenueDOCNA(venue_docna_xpm)
	, m_iconVenueNADAC(venue_nadac_xpm)
	, m_iconVenueSweep(venue_sweep_xpm)
	, m_iconVenueTDAA(venue_tdaa_xpm)
	, m_iconVenueUKC(venue_ukc_xpm)
	, m_iconVenueUSDAA(venue_usdaa_xpm)
	, m_iconVenueFCI(venue_fci_xpm)
	, m_iconVenueSCC(venue_scc_xpm)
	, m_iconCRCD(crcd_xpm)
	, m_iconBlank(CalEmpty_xpm)
	, m_iconChecked(checked_xpm)
	, m_iconUnChecked(unchecked_xpm)
	, m_iconCheck()
	, m_iconQuestion()
	, m_iconAccConfirm(AccConfirm_xpm)
	, m_iconAccNone(AccNone_xpm)
	, m_iconAccTodo(AccTodo_xpm)
	, m_iconCalEntered(CalEntered_xpm)
	, m_iconCalEnteredTentative(CalEnteredTentative_xpm)
	, m_iconCalPending(CalPending_xpm)
	, m_iconCalPendingTentative(CalPendingTentative_xpm)
	, m_iconCalPlan(CalPlan_xpm)
	, m_iconCalPlanTentative(CalPlanTentative_xpm)
	, m_iconCalTentative(CalTentative_xpm)
	, m_iconHdrDown(HdrDown_xpm)
	, m_iconHdrUp(HdrUp_xpm)
	, m_iconNoteAdded(NoteAdded_xpm)
	, m_iconNoteNote(NoteNote_xpm)
	, m_iconNodeNoteAdded(NoteNoteAdded_xpm)
	, m_iconTitleHidden(hidden_xpm)
	, m_iconTitledHidden(title_hidden_xpm)
	, m_iconTitledHiddenHave(title_hidden_have_xpm)
	, m_iconTitled(title_visible_xpm)
	, m_iconTitledHave(title_visible_have_xpm)
	, m_bmpNote(Note_xpm)
	, m_bmpInfoEmpty(CalEmpty_xpm)
	, m_bmpInfoNote(NoteNote_xpm)
	, m_bmpInfoNoteAdded(NoteAdded_xpm)
	, m_bmpInfoNoteNoteAdded(NoteNoteAdded_xpm)
{
	m_iconCheck = m_iconCalPlan;
	m_iconQuestion = m_iconCalTentative;
}


CImageManager::~CImageManager()
{
}
