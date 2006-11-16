/*
 * Copyright � 2002-2006 David Connet. All Rights Reserved.
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
 * @brief implementation of the CAgilityBookDoc class
 * @author David Connet
 *
 * MFC document class, provides a binding between the real data in the
 * CAgilityRecordBook class, XML, and the MFC Doc-View architecture.
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2006-02-08 DRC Added 'RenameEvent' action.
 * @li 2005-12-14 DRC Moved 'Titles' to 'Venue'.
 * @li 2005-10-26 DRC Added option to prevent auto-update user query.
 * @li 2005-10-19 DRC Fixed a problem with CFile::GetStatus (see AgilityBook.cpp).
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-12-19 DRC Changed sort-newest to only do trials, not runs.
 * @li 2004-12-18 DRC Added an extra check before posting the new dog msg.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-07-23 DRC Auto-check the config version on document open.
 * @li 2004-07-20 DRC Moved the user-request updates here so it can check if
 *                    a new configuration is available.
 * @li 2004-06-29 DRC Set filtering on runs that are in hidden trials.
 * @li 2004-04-29 DRC Use default config during auto-update (no file prompt).
 * @li 2004-03-31 DRC Only prompt to merge config if config version number is
 *                    greater (was simply checking for not-equal)
 * @li 2004-03-26 DRC Added code to migrate runs to the new table-in-run form.
 *                    Added menu handlers for 'Show Hidden Titles' (oops)
 * @li 2004-02-26 DRC Moved config update here, test doc for current config.
 * @li 2004-01-26 DRC Display errors on non-fatal load.
 * @li 2003-12-10 DRC Moved import/export into a wizard.
 * @li 2003-12-07 DRC Changed Load/Save api to support new info section.
 * @li 2003-10-31 DRC Added import/export calendar, export config.
 * @li 2003-10-22 DRC Added export dtd/xml menu options.
 * @li 2003-10-09 DRC Added option to not filter runs by selected trial.
 * @li 2003-09-15 DRC Fixed a bug where a trial created for more than one dog
 *                    at the same time actually only created one linked entry.
 * @li 2003-08-27 DRC Added view accessors for calendar, made them public so
 *                    I don't have to use UpdateAllViews. Added methods to allow
 *                    creating titles/trials/runs from the Run view.
 * @li 2003-08-25 DRC Added GetCurrentRun().
 * @li 2003-08-24 DRC Optimized filtering by adding boolean into ARBBase to
 *                    prevent constant re-evaluation.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include <fstream>

#include "AboutDlg.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTree.h"
#include "AgilityBookTreeData.h"
#include "AgilityBookViewCalendar.h"
#include "AgilityBookViewCalendarList.h"
#include "AgilityBookViewTraining.h"
#include "ClipBoard.h"
#include "DlgCalendar.h"
#include "DlgConfigUpdate.h"
#include "DlgConfigure.h"
#include "DlgDog.h"
#include "DlgFindLinks.h"
#include "DlgFixup.h"
#include "DlgInfoJudge.h"
#include "DlgListViewer.h"
#include "DlgMessage.h"
#include "DlgOptions.h"
#include "DlgSelectDog.h"
#include "DlgTraining.h"
#include "DlgTrial.h"
#include "Element.h"
#include "FilterOptions.h"
#include "MainFrm.h"
#include "Splash.h"
#include "TabView.h"
#include "Wizard.h"

#if _MSC_VER < 1300
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static short GetCurrentConfigVersion()
{
	static short ver = 0;
	static bool bLoaded = false;
	if (!bLoaded)
	{
		bLoaded = true;
		ARBConfig config;
		config.Default();
		ver = config.GetVersion();
	}
	return ver;
}

/////////////////////////////////////////////////////////////////////////////

CErrorCallback::CErrorCallback()
	: ARBErrorCallback(m_ErrMsg)
{
}

bool CErrorCallback::OnError(TCHAR const* const pMsg)
{
	CSplashWnd::HideSplashScreen();
	return (IDYES == AfxMessageBox(pMsg, MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2));
}

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookDoc

IMPLEMENT_DYNCREATE(CAgilityBookDoc, CDocument)

BEGIN_MESSAGE_MAP(CAgilityBookDoc, CDocument)
	//{{AFX_MSG_MAP(CAgilityBookDoc)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_HELP_UPDATE, OnHelpUpdate)
	ON_COMMAND(ID_FILE_EXPORT_WIZARD, OnFileExportWizard)
	ON_COMMAND(ID_FILE_LINKED, OnFileLinked)
	ON_UPDATE_COMMAND_UI(ID_COPY_TITLES_LIST, OnUpdateCopyTitles)
	ON_COMMAND(ID_COPY_TITLES_LIST, OnCopyTitles)
	ON_COMMAND(ID_EDIT_CONFIGURATION, OnEditConfiguration)
	ON_COMMAND(ID_AGILITY_NEW_DOG, OnAgilityNewDog)
	ON_COMMAND(ID_AGILITY_NEW_CALENDAR, OnAgilityNewCalendar)
	ON_COMMAND(ID_AGILITY_NEW_TRAINING, OnAgilityNewTraining)
	ON_COMMAND(ID_NOTES_CLUBS, OnNotesClubs)
	ON_COMMAND(ID_NOTES_JUDGES, OnNotesJudges)
	ON_COMMAND(ID_NOTES_LOCATIONS, OnNotesLocations)
	ON_COMMAND(ID_NOTES_SEARCH, OnNotesSearch)
	ON_COMMAND(ID_VIEW_OPTIONS, OnViewOptions)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SORTRUNS, OnUpdateViewSortruns)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RUNS_BY_TRIAL, OnUpdateViewRunsByTrial)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HIDDEN, OnUpdateViewHiddenTitles)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TABLE_IN_YPS, OnUpdateViewTableInYPS)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CAgilityBookDoc::CAgilityBookDoc()
	: m_SuppressUpdates(false)
{
}

CAgilityBookDoc::~CAgilityBookDoc()
{
}

/**
 * Return the dog associated with the currently selected item in the tree.
 */
ARBDogPtr CAgilityBookDoc::GetCurrentDog()
{
	ARBDogPtr pDog;
	CAgilityBookTree* pTree = GetTreeView();
	if (pTree && pTree->GetCurrentTreeItem())
		pDog = pTree->GetCurrentTreeItem()->GetDog();
	return pDog;
}

/**
 * Return the trial associated with the currently selected item in the tree.
 */
ARBDogTrialPtr CAgilityBookDoc::GetCurrentTrial()
{
	ARBDogTrialPtr pTrial;
	CAgilityBookTree* pTree = GetTreeView();
	ASSERT(pTree);
	if (pTree && pTree->GetCurrentTreeItem())
		pTrial = pTree->GetCurrentTreeItem()->GetTrial();
	return pTrial;
}

/**
 * Return the run associated with the currently selected item in the tree.
 */
ARBDogRunPtr CAgilityBookDoc::GetCurrentRun()
{
	ARBDogRunPtr pRun;
	CAgilityBookTree* pTree = GetTreeView();
	ASSERT(pTree);
	if (pTree && pTree->GetCurrentTreeItem())
		pRun = pTree->GetCurrentTreeItem()->GetRun();
	return pRun;
}

/**
 * Called from the Runs view. Since the run is visible in that view and visible
 * runs are controlled by the selected item in the tree, 'pData' should never
 * be NULL.
 */
void CAgilityBookDoc::AddTitle(ARBDogRunPtr pSelectedRun)
{
	CAgilityBookTree* pTree = GetTreeView();
	ASSERT(pTree);
	CAgilityBookTreeData* pData = pTree->FindData(TVI_ROOT, pSelectedRun);
	if (pData)
	{
		pTree->GetTreeCtrl().EnsureVisible(pData->GetHTreeItem());
		if (pData->OnCmd(ID_AGILITY_NEW_TITLE, NULL))
			SetModifiedFlag(TRUE);
	}
}

void CAgilityBookDoc::AddTrial(ARBDogRunPtr pSelectedRun)
{
	CAgilityBookTree* pTree = GetTreeView();
	ASSERT(pTree);
	CAgilityBookTreeData* pData = pTree->FindData(TVI_ROOT, pSelectedRun);
	if (pData)
	{
		pTree->GetTreeCtrl().EnsureVisible(pData->GetHTreeItem());
		if (pData->OnCmd(ID_AGILITY_NEW_TRIAL, NULL))
			SetModifiedFlag(TRUE);
	}
}

void CAgilityBookDoc::AddRun(ARBDogRunPtr pSelectedRun)
{
	CAgilityBookTree* pTree = GetTreeView();
	ASSERT(pTree);
	CAgilityBookTreeData* pData = pTree->FindData(TVI_ROOT, pSelectedRun);
	if (pData)
	{
		pTree->GetTreeCtrl().EnsureVisible(pData->GetHTreeItem());
		if (pData->OnCmd(ID_AGILITY_NEW_RUN, NULL))
			SetModifiedFlag(TRUE);
	}
}

void CAgilityBookDoc::EditRun(ARBDogRunPtr pRun)
{
	CAgilityBookTree* pTree = GetTreeView();
	ASSERT(pTree);
	CAgilityBookTreeData* pData = pTree->FindData(TVI_ROOT, pRun);
	if (pData)
	{
		pTree->GetTreeCtrl().EnsureVisible(pData->GetHTreeItem());
		if (pData->OnCmd(ID_AGILITY_EDIT_RUN, NULL))
			SetModifiedFlag(TRUE);
	}
}

void CAgilityBookDoc::DeleteRun(ARBDogRunPtr pRun)
{
	CAgilityBookTree* pTree = GetTreeView();
	ASSERT(pTree);
	CAgilityBookTreeData* pData = pTree->FindData(TVI_ROOT, pRun);
	if (pData)
	{
		pTree->GetTreeCtrl().EnsureVisible(pData->GetHTreeItem());
		if (pData->OnCmd(ID_AGILITY_DELETE_RUN, NULL))
			SetModifiedFlag(TRUE);
	}
}

bool CAgilityBookDoc::CreateTrialFromCalendar(
		ARBCalendar const& cal,
		CTabView* pTabView)
{
	if (!GetConfig().GetVenues().FindVenue(cal.GetVenue()))
		return false;
	bool bCreated = false;
	ARBDogTrialPtr pTrial(ARBDogTrial::New(cal));
	CDlgTrial dlg(this, pTrial);
	if (IDOK == dlg.DoModal())
	{
		std::vector<ARBDogPtr> dogs;
		CDlgSelectDog dlg(this, dogs);
		if (IDOK == dlg.DoModal() && 0 < dogs.size())
		{
			for (std::vector<ARBDogPtr>::iterator iter = dogs.begin(); iter != dogs.end(); ++iter)
			{
				bCreated = true;
				ARBDogPtr pDog = *iter;
				// If we're inserting this entry into more than one dog, we
				// MUST make a copy. Otherwise the trial will be the same trial
				// for both dogs and all changes will be reflected from one to
				// the other - until you save, exit and reload the program.
				ARBDogTrialPtr pNewTrial = pTrial->Clone();
				pDog->GetTrials().AddTrial(pNewTrial);
				SetModifiedFlag();
				// This is pure evil - casting ARBDogTrialPtr* to a CObject*.
				// On the other side, we reinterpret right back to ARBDogTriaPtrl*
				// Definite abuse of this parameter.
				UpdateAllViews(NULL, UPDATE_NEW_TRIAL, reinterpret_cast<CObject*>(&pNewTrial));
			}
			pTabView->SetCurSel(0);
		}
	}
	return bCreated;
}

void CAgilityBookDoc::SortDates()
{
	bool bDescending = !CAgilityBookOptions::GetNewestDatesFirst();
	for (ARBDogList::iterator iterDogs = GetDogs().begin(); iterDogs != GetDogs().end(); ++iterDogs)
	{
		ARBDogPtr pDog = *iterDogs;
		pDog->GetTrials().sort(bDescending);
	}
}

void CAgilityBookDoc::ImportConfiguration(ARBConfig& update)
{
	ARBostringstream msg;
	for (ARBConfigActionList::const_iterator iterAction = update.GetActions().begin(); iterAction != update.GetActions().end(); ++iterAction)
	{
		ARBConfigActionPtr action = *iterAction;
		if (action->GetVerb() == ACTION_VERB_RENAME_EVENT)
		{
			ARBConfigVenuePtr venue;
			if (GetConfig().GetVenues().FindVenue(action->GetVenue(), &venue))
			{
				ARBConfigEventPtr oldEvent;
				if (venue->GetEvents().FindEvent(action->GetOldName(), &oldEvent))
				{
					msg << _T("Action: Renaming ")
						<< action->GetVenue()
						<< _T(" event [")
						<< action->GetOldName()
						<< _T("] to [")
						<< action->GetNewName()
						<< _T("]");
					// If any events are in use, create a fixup action.
					int nEvents = GetDogs().NumEventsInUse(action->GetVenue(), action->GetOldName());
					if (0 < nEvents)
					{
						msg << _T(", updating ") << nEvents << _T("titles");
						GetDogs().RenameEvent(action->GetVenue(), action->GetOldName(), action->GetNewName());
					}
					msg << _T("\n");
					// If the new event exists, just delete the old.
					// Otherwise, rename the old to new.
					if (venue->GetEvents().FindEvent(action->GetNewName()))
						venue->GetEvents().DeleteEvent(action->GetOldName());
					else
						oldEvent->SetName(action->GetNewName());
				}
			}
		}
		else if (action->GetVerb() == ACTION_VERB_DELETE_EVENT)
		{
			ARBConfigVenuePtr venue;
			if (GetConfig().GetVenues().FindVenue(action->GetVenue(), &venue))
			{
				ARBConfigEventPtr oldEvent;
				if (venue->GetEvents().FindEvent(action->GetOldName(), &oldEvent))
				{
					int nEvents = GetDogs().NumEventsInUse(action->GetVenue(), action->GetOldName());
					// If any events are in use, create a fixup action.
					if (0 < nEvents)
					{
						msg << _T("Action: DELETING existing ")
							<< nEvents
							<< _T(" event(s) [")
							<< action->GetOldName()
							<< _T("]\n");
						GetDogs().DeleteEvent(action->GetVenue(), action->GetOldName());
					}
					msg << _T("Action: Deleting event [")
						<< action->GetOldName()
						<< _T("]\n");
					venue->GetEvents().DeleteEvent(action->GetOldName());
				}
			}
		}
		else if (action->GetVerb() == ACTION_VERB_RENAME_TITLE)
		{
			// Find the venue.
			ARBConfigVenuePtr venue;
			if (GetConfig().GetVenues().FindVenue(action->GetVenue(), &venue))
			{
				// Find the title we're renaming.
				ARBConfigTitlePtr oldTitle;
				if (venue->GetTitles().FindTitle(action->GetOldName(), &oldTitle))
				{
					// Note: If we are deleting/renaming a title due
					// to an error in the config (same title in multiple
					// divisions), all we can do is rename ALL of them.
					// There's no way to differentiate existing titles.
					msg << _T("Action: Renaming title [")
						<< action->GetOldName()
						<< _T("] to [")
						<< action->GetNewName()
						<< _T("]");
					// If any titles are in use, create a fixup action.
					int nTitles = GetDogs().NumTitlesInUse(action->GetVenue(), action->GetOldName());
					if (0 < nTitles)
					{
						msg << _T(", updating ") << nTitles << _T(" titles");
						GetDogs().RenameTitle(action->GetVenue(), action->GetOldName(), action->GetNewName());
					}
					msg << _T("\n");
					// If the new title exists, just delete the old.
					// Otherwise, rename the old to new.
					if (venue->GetTitles().FindTitle(action->GetNewName()))
						venue->GetTitles().DeleteTitle(action->GetOldName());
					else
						oldTitle->SetName(action->GetNewName());
				}
			}
		}
		else if (action->GetVerb() == ACTION_VERB_DELETE_TITLE)
		{
			// Find the venue.
			ARBConfigVenuePtr venue;
			if (GetConfig().GetVenues().FindVenue(action->GetVenue(), &venue))
			{
				// Find the title we're deleting.
				ARBConfigTitlePtr oldTitle;
				if (venue->GetTitles().FindTitle(action->GetOldName(), &oldTitle))
				{
					int nTitles = GetDogs().NumTitlesInUse(action->GetVenue(), action->GetOldName());
					// If any titles are in use, create a fixup action.
					if (0 < nTitles)
					{
						// Note: If we are deleting/renaming a title due
						// to an error in the config (same title in multiple
						// divisions), all we can do is rename ALL of them.
						// There's no way to differentiate existing titles.
						if (0 < action->GetNewName().length())
						{
							msg << _T("Action: Renaming existing ")
								<< nTitles
								<< _T(" title(s) [")
								<< action->GetOldName()
								<< _T("] to [")
								<< action->GetNewName()
								<< _T("]\n");
							GetDogs().RenameTitle(action->GetVenue(), action->GetOldName(), action->GetNewName());
						}
						else
						{
							msg << _T("Action: DELETING existing ")
								<< nTitles
								<< _T(" title(s) [")
								<< action->GetOldName()
								<< _T("]\n");
							GetDogs().DeleteTitle(action->GetVenue(), action->GetOldName());
						}
					}
					msg << _T("Action: Deleting title [")
						<< action->GetOldName()
						<< _T("]\n");
					venue->GetTitles().DeleteTitle(action->GetOldName());
				}
			}
		}
		else
			ASSERT(0);
	}
	ARBString info;
	bool bUpdateRuns = false;
	if (GetConfig().GetVersion() <= 2 && update.GetVersion() == 3)
		bUpdateRuns = true;
	GetConfig().Update(0, update, info);
	std::vector<CDlgFixup*> fixups;
	CDlgConfigure::CheckExistingRuns(this, m_Records.GetDogs(), GetConfig(), fixups, true);
	for (std::vector<CDlgFixup*>::iterator iter = fixups.begin(); iter != fixups.end(); ++iter)
	{
		(*iter)->Commit(m_Records);
		delete (*iter);
	}
	fixups.clear();
	msg << info;
	if (bUpdateRuns)
	{
		CDlgFixupTableInRuns fix;
		fix.Commit(m_Records);
		if (0 < fix.RunsUpdated())
		{
			if (0 < info.length())
				msg << _T("\n\n");
			msg << _T("Table setting updated in ")
				<< fix.RunsUpdated()
				<< _T(" runs.");
		}
	}
	if (0 < msg.tellp())
	{
		CDlgMessage dlg(msg.str().c_str(), 0);
		dlg.DoModal();
		GetDogs().SetMultiQs(GetConfig());
		SetModifiedFlag();
		UpdateAllViews(NULL, UPDATE_CONFIG);
	}
	else
		AfxMessageBox(IDS_CONFIG_NO_UPDATE, MB_ICONINFORMATION);
}

bool CAgilityBookDoc::ImportConfiguration(bool bUseDefault)
{
	bool bOk = false;
	bool bDoIt = false;
	CDlgConfigUpdate dlg;
	if (bUseDefault)
	{
		bDoIt = true;
		dlg.LoadConfig(NULL);
	}
	else
	{
		if (IDOK == dlg.DoModal())
			bDoIt = true;
	}
	if (bDoIt)
	{
		ARBConfig& update = dlg.GetConfig();
		ImportConfiguration(update);
		bOk = true;
	}
	return bOk;
}

void CAgilityBookDoc::ResetVisibility()
{
	std::vector<CVenueFilter> venues;
	CFilterOptions::Options().GetFilterVenue(venues);
	std::set<ARBString> names;
	CFilterOptions::Options().GetTrainingFilterNames(names);

	for (ARBDogList::iterator iterDogs = GetDogs().begin(); iterDogs != GetDogs().end(); ++iterDogs)
	{
		ResetVisibility(venues, *iterDogs);
	}

	for (ARBTrainingList::iterator iterTraining = GetTraining().begin(); iterTraining != GetTraining().end(); ++iterTraining)
	{
		ResetVisibility(names, *iterTraining);
	}

	for (ARBCalendarList::iterator iterCal = GetCalendar().begin(); iterCal != GetCalendar().end(); ++iterCal)
	{
		ARBCalendarPtr pCal = *iterCal;
		bool bVis = CFilterOptions::Options().IsCalendarVisible(pCal);
		pCal->SetFiltered(!bVis);
	}

	if (!m_SuppressUpdates)
		UpdateAllViews(NULL, UPDATE_OPTIONS);
}

void CAgilityBookDoc::ResetVisibility(
		std::vector<CVenueFilter>& venues,
		ARBDogPtr pDog)
{
	for (ARBDogTrialList::iterator iterTrial = pDog->GetTrials().begin(); iterTrial != pDog->GetTrials().end(); ++iterTrial)
		ResetVisibility(venues, *iterTrial);

	for (ARBDogTitleList::iterator iterTitle = pDog->GetTitles().begin(); iterTitle != pDog->GetTitles().end(); ++iterTitle)
		ResetVisibility(venues, *iterTitle);
}

void CAgilityBookDoc::ResetVisibility(
		std::vector<CVenueFilter>& venues,
		ARBDogTrialPtr pTrial)
{
	bool bVisTrial = CFilterOptions::Options().IsTrialVisible(venues, pTrial);
	pTrial->SetFiltered(!bVisTrial);
	int nVisible = 0;
	for (ARBDogRunList::iterator iterRun = pTrial->GetRuns().begin(); iterRun != pTrial->GetRuns().end(); ++iterRun)
	{
		ResetVisibility(venues, pTrial, *iterRun);
		if (!(*iterRun)->IsFiltered())
			++nVisible;
	}
	if (0 == nVisible && 0 < pTrial->GetRuns().size())
		pTrial->SetFiltered(true);
}

void CAgilityBookDoc::ResetVisibility(
		std::vector<CVenueFilter>& venues,
		ARBDogTrialPtr pTrial,
		ARBDogRunPtr pRun)
{
	unsigned short nVisRun = CFilterOptions::Options().IsRunVisible(venues, pTrial, pRun);
	pRun->SetFiltered(ARBBase::eFilter, (nVisRun & (0x1 << ARBBase::eFilter)) ? false : true);
	pRun->SetFiltered(ARBBase::eIgnoreQ, (nVisRun & (0x1 << ARBBase::eIgnoreQ)) ? false : true);
}

void CAgilityBookDoc::ResetVisibility(
		std::vector<CVenueFilter>& venues,
		ARBDogTitlePtr pTitle)
{
	bool bVisTitle = CFilterOptions::Options().IsTitleVisible(venues, pTitle);
	pTitle->SetFiltered(!bVisTitle);
}

void CAgilityBookDoc::ResetVisibility(
		std::set<ARBString>& names,
		ARBTrainingPtr pTraining)
{
	bool bVisTraining = CFilterOptions::Options().IsTrainingLogVisible(names, pTraining);
	pTraining->SetFiltered(!bVisTraining);
}

/**
 * Function to get the tree view. This is used internally and by the runs view.
 */
CAgilityBookTree* CAgilityBookDoc::GetTreeView() const
{
	POSITION pos = this->GetFirstViewPosition();
	while (NULL != pos)
	{
		CView* pView = GetNextView(pos);
		CAgilityBookTree* pView2 = DYNAMIC_DOWNCAST(CAgilityBookTree, pView);
		if (pView2)
			return pView2;
	}
	return NULL;
}

/**
 * Function to get the calendar list view. This is used by the calendar view
 * and by the export wizard.
 */
CAgilityBookViewCalendarList* CAgilityBookDoc::GetCalendarListView() const
{
	POSITION pos = this->GetFirstViewPosition();
	while (NULL != pos)
	{
		CView* pView = GetNextView(pos);
		CAgilityBookViewCalendarList* pView2 = DYNAMIC_DOWNCAST(CAgilityBookViewCalendarList, pView);
		if (pView2)
			return pView2;
	}
	ASSERT(0);
	return NULL;
}

/**
 * Function to get the calendar view. This is used by the calendar list view.
 */
CAgilityBookViewCalendar* CAgilityBookDoc::GetCalendarView() const
{
	POSITION pos = this->GetFirstViewPosition();
	while (NULL != pos)
	{
		CView* pView = GetNextView(pos);
		CAgilityBookViewCalendar* pView2 = DYNAMIC_DOWNCAST(CAgilityBookViewCalendar, pView);
		if (pView2)
			return pView2;
	}
	ASSERT(0);
	return NULL;
}

/**
 * Function to get the trainging view. This is used by the export wizard.
 */
CAgilityBookViewTraining* CAgilityBookDoc::GetTrainingView() const
{
	POSITION pos = this->GetFirstViewPosition();
	while (NULL != pos)
	{
		CView* pView = GetNextView(pos);
		CAgilityBookViewTraining* pView2 = DYNAMIC_DOWNCAST(CAgilityBookViewTraining, pView);
		if (pView2)
			return pView2;
	}
	ASSERT(0);
	return NULL;
}


void CAgilityBookDoc::BackupFile(LPCTSTR lpszPathName)
{
	int nBackups = CAgilityBookOptions::GetNumBackupFiles();
	if (0 < nBackups)
	{
		// First find a hole.
		int nHole = -1;
		int i;
		for (i = 1; i <= nBackups; ++i)
		{
			ARBostringstream backup;
			backup << lpszPathName << _T(".bck") << i;
			if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(backup.str().c_str()))
			{
				nHole = i;
				break;
			}
		}
		if (-1 == nHole)
			nHole = nBackups;
		// Then shift all the files into the hole.
		for (i = nHole; i > 1; --i)
		{
			ARBostringstream backup;
			backup << lpszPathName << _T(".bck") << i;
			if (INVALID_FILE_ATTRIBUTES != GetFileAttributes(backup.str().c_str()))
				DeleteFile(backup.str().c_str());
			ARBostringstream filename;
			filename << lpszPathName << _T(".bck") << i-1;
			MoveFile(filename.str().c_str(), backup.str().c_str());
		}
		ARBostringstream backup;
		backup << lpszPathName << _T(".bck1");
		CopyFile(lpszPathName, backup.str().c_str(), FALSE);
	}
}

/**
 * MFC method to delete contents of current document.
 */
void CAgilityBookDoc::DeleteContents()
{
	m_Records.clear();
	CDocument::DeleteContents();
	SetModifiedFlag(FALSE);
}

/**
 * MFC method to create a new empty document.
 */
BOOL CAgilityBookDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	AfxGetApp()->WriteProfileString(_T("Settings"), _T("LastFile"), _T(""));
	m_Records.Default();
	m_Records.GetConfig().GetActions().clear();

	if (0 == GetDogs().size())
	{
		if (IsWindow(AfxGetMainWnd()->GetSafeHwnd()))
			AfxGetMainWnd()->PostMessage(PM_DELAY_MESSAGE, CREATE_NEWDOG);
	}
	return TRUE;
}

/**
 * Since we are not using the CArchive serialization (via Serialize()), we
 * have to provide a full implementation of the open and save routines.
 * We do not want the CArchive stuff since we're using Xerces to read the XML.
 */
BOOL CAgilityBookDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CFileStatus status;
	if (!GetLocalStatus(lpszPathName, status))
	{
		CSplashWnd::HideSplashScreen();
		AfxGetApp()->WriteProfileString(_T("Settings"), _T("LastFile"), _T(""));
		ReportSaveLoadException(lpszPathName, NULL, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		return FALSE;
	}

	CWaitCursor wait;

	// Do standard CDocument stuff.
	if (IsModified())
	{
		TRACE0("Warning: OnOpenDocument replaces an unsaved document.\n");
	}
	DeleteContents();

	CString source(lpszPathName);
	ARBString err;
	Element tree;
	// Translate the XML to a tree form.
	if (!tree.LoadXMLFile((LPCTSTR)source, err))
	{
		AfxGetApp()->WriteProfileString(_T("Settings"), _T("LastFile"), _T(""));
		CString msg;
		msg.LoadString(AFX_IDP_FAILED_TO_OPEN_DOC);
		if (0 < err.length())
		{
			msg += _T("\n\n");
			msg += err.c_str();
		}
		CSplashWnd::HideSplashScreen();
		AfxMessageBox(msg, MB_ICONEXCLAMATION);
		return FALSE;
	}
	// Translate the tree to a class structure.
	CErrorCallback callback;
	if (!m_Records.Load(tree, callback))
	{
		AfxGetApp()->WriteProfileString(_T("Settings"), _T("LastFile"), _T(""));
		CString msg;
		msg.LoadString(AFX_IDP_FAILED_TO_OPEN_DOC);
		if (0 < callback.m_ErrMsg.length())
		{
			msg += _T("\n\n");
			msg += callback.m_ErrMsg.c_str();
		}
		CSplashWnd::HideSplashScreen();
		AfxMessageBox(msg, MB_ICONEXCLAMATION);
		return FALSE;
	}
	else if (0 < callback.m_ErrMsg.length())
	{
		CString msg(_T("Warning: Some non-fatal messages were generated while loading the file:\n\n"));
		msg += callback.m_ErrMsg.c_str();
		CSplashWnd::HideSplashScreen();
		AfxMessageBox(msg, MB_ICONINFORMATION);
	}
	SortDates();

	m_SuppressUpdates = true;
	ResetVisibility();
	m_SuppressUpdates = false;

	SetModifiedFlag(FALSE);     // start off with unmodified

	if (CAgilityBookOptions::AutoDeleteCalendarEntries())
	{
		ARBDate today(ARBDate::Today());
		today -= CAgilityBookOptions::DaysTillEntryIsPast();
		if (0 < m_Records.GetCalendar().TrimEntries(today))
			SetModifiedFlag(TRUE);
	}

	AfxGetApp()->WriteProfileString(_T("Settings"), _T("LastFile"), lpszPathName);

	// Check our internal config.
	if (GetCurrentConfigVersion() > m_Records.GetConfig().GetVersion()
	&& m_Records.GetConfig().GetUpdateStatus())
	{
		if (CUpdateInfo::UpdateConfig(this))
		{
			if (ImportConfiguration(true))
				SetModifiedFlag(TRUE);
		}
	}
	// Then check the external config.
	else
	{
		CAgilityBookApp* pApp = dynamic_cast<CAgilityBookApp*>(AfxGetApp());
		ASSERT(pApp);
		pApp->UpdateInfo().AutoCheckConfiguration(this);
	}

	if (0 == GetDogs().size() && AfxGetMainWnd() && ::IsWindow(AfxGetMainWnd()->GetSafeHwnd()))
	{
		AfxGetMainWnd()->PostMessage(PM_DELAY_MESSAGE, CREATE_NEWDOG);
	}

	return TRUE;
}

/**
 * MFC method to close a document (will call DeleteContents thru base class)
 */
void CAgilityBookDoc::OnCloseDocument()
{
	ARBDogPtr pDog = GetCurrentDog();
	if (pDog)
		AfxGetApp()->WriteProfileString(_T("Settings"), _T("LastDog"), pDog->GetCallName().c_str());
	else
		AfxGetApp()->WriteProfileString(_T("Settings"), _T("LastDog"), _T(""));
	CDocument::OnCloseDocument();
}

/**
 * MFC method to save a document.
 */
BOOL CAgilityBookDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	CWaitCursor wait;

	CVersionNum ver;
	ARBString verstr = (LPCTSTR)ver.GetVersionString();
	bool bAlreadyWarned = false;
	BOOL bOk = FALSE;
	Element tree;
	// First, we have to push all the class data into a tree.
	if (m_Records.Save(tree, verstr, true, true, true, true, true))
	{
		BackupFile(lpszPathName);
		// Then we can stream that tree out as XML.
		CStringA filename(lpszPathName);
		std::ofstream output(filename, std::ios::out | std::ios::binary);
		output.exceptions(std::ios_base::badbit);
		if (output.is_open())
		{
			if (tree.SaveXML(output))
			{
				AfxGetApp()->WriteProfileString(_T("Settings"), _T("LastFile"), lpszPathName);
				bOk = TRUE;
				SetModifiedFlag(FALSE);
			}
			output.close();
		}
		else
		{
			bAlreadyWarned = true;
			CString errMsg;
			errMsg.FormatMessage(IDS_CANNOT_OPEN, lpszPathName);
			AfxMessageBox(errMsg, MB_OK | MB_ICONEXCLAMATION);
		}
	}
	if (!bOk && !bAlreadyWarned)
	{
		AfxMessageBox(_T("Internal error: File not saved."), MB_ICONSTOP);
	}
	return bOk;
}

#ifdef _DEBUG
// CAgilityBookDoc diagnostics
void CAgilityBookDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAgilityBookDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

// CAgilityBookDoc commands

void CAgilityBookDoc::OnAppAbout()
{
	CAboutDlg aboutDlg(this);
	aboutDlg.DoModal();
}

void CAgilityBookDoc::OnHelpUpdate()
{
	CAgilityBookApp* pApp = dynamic_cast<CAgilityBookApp*>(AfxGetApp());
	ASSERT(pApp);
	pApp->UpdateInfo().UpdateConfiguration(this);
}

void CAgilityBookDoc::OnFileExportWizard()
{
	CWizard wiz(this);
	wiz.DoModal();
}

void CAgilityBookDoc::OnFileLinked()
{
	CDlgFindLinks dlg(GetDogs());
	if (0 == dlg.GetNumLinks())
	{
		AfxMessageBox(_T("No linked files found."), MB_ICONINFORMATION);
	}
	else
		dlg.DoModal();

}

void CAgilityBookDoc::OnUpdateCopyTitles(CCmdUI* pCmdUI)
{
	// As long as a dog is current, enable. This means the copy may have
	// only the dog's name.
	BOOL bEnable = FALSE;
	ARBDogPtr pDog = GetCurrentDog();
	if (pDog)
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookDoc::OnCopyTitles()
{
	ARBDogPtr pDog = GetCurrentDog();
	if (pDog)
	{
		std::vector<CVenueFilter> venues;
		CFilterOptions::Options().GetFilterVenue(venues);

		ARBString preTitles, postTitles;
		for (ARBConfigVenueList::const_iterator iVenue = GetConfig().GetVenues().begin();
			iVenue != GetConfig().GetVenues().end();
			++iVenue)
		{
			if (!CFilterOptions::Options().IsVenueVisible(venues, (*iVenue)->GetName()))
				continue;
			ARBString preTitles2, postTitles2;
			for (ARBConfigTitleList::const_iterator iTitle = (*iVenue)->GetTitles().begin();
				iTitle != (*iVenue)->GetTitles().end();
				++iTitle)
			{
				ARBDogTitlePtr pTitle;
				if (pDog->GetTitles().FindTitle((*iVenue)->GetName(), (*iTitle)->GetName(), &pTitle))
				{
					if (pTitle->GetDate().IsValid()
					&& !pTitle->IsFiltered())
					{
						if ((*iTitle)->GetPrefix())
						{
							if (!preTitles2.empty())
								preTitles2 += ' ';
							preTitles2 += (*iTitle)->GetName();
						}
						else
						{
							if (!postTitles2.empty())
								postTitles2 += ' ';
							postTitles2 += (*iTitle)->GetName();
						}
					}
				}
			}
			if (!preTitles2.empty())
			{
				if (!preTitles.empty())
					preTitles += ' ';
				preTitles += preTitles2;
			}
			if (!postTitles2.empty())
			{
				if (!postTitles.empty())
					postTitles += _T("; ");
				postTitles += postTitles2;
			}
		}

		CString data(preTitles.c_str());
		if (!data.IsEmpty())
			data += ' ';
		data += pDog->GetCallName().c_str();
		if (!postTitles.empty())
		{
			data += _T(": ");
			data += postTitles.c_str();
		}
		CClipboardDataWriter clpData;
		if (clpData.isOkay())
		{
			clpData.SetData(data);
		}
	}
}

void CAgilityBookDoc::OnEditConfiguration()
{
	CDlgConfigure config(this, m_Records);
	config.DoModal();
}

void CAgilityBookDoc::OnAgilityNewDog()
{
	ARBDogPtr dog(ARBDog::New());
	CDlgDog dlg(this, dog);
	if (IDOK == dlg.DoModal())
	{
		CAgilityBookTree* pTree = NULL;
		POSITION pos = GetFirstViewPosition();
		while (NULL != pos)
		{
			CView* pView = GetNextView(pos);
			if (DYNAMIC_DOWNCAST(CAgilityBookTree, pView))
			{
				pTree = reinterpret_cast<CAgilityBookTree*>(pView);
				break;
			}
		}
		if (pTree)
		{
			CMainFrame* pFrame = reinterpret_cast<CMainFrame*>(AfxGetMainWnd());
			pFrame->SetCurTab(0);
			SetModifiedFlag();
			if (!GetDogs().AddDog(dog))
				pTree->InsertDog(dog, true);
		}
	}
}

void CAgilityBookDoc::OnAgilityNewCalendar()
{
	ARBCalendarPtr cal(ARBCalendar::New());
	CDlgCalendar dlg(cal, this);
	if (IDOK == dlg.DoModal())
	{
		if (!(CAgilityBookOptions::AutoDeleteCalendarEntries() && cal->GetEndDate() < ARBDate::Today()))
		{
			CMainFrame* pFrame = reinterpret_cast<CMainFrame*>(AfxGetMainWnd());
			pFrame->SetCurTab(2);
			GetCalendar().AddCalendar(cal);
			GetCalendar().sort();
			SetModifiedFlag();
			UpdateAllViews(NULL, UPDATE_CALENDAR_VIEW);
			POSITION pos = GetFirstViewPosition();
			while (NULL != pos)
			{
				CView* pView = GetNextView(pos);
				if (DYNAMIC_DOWNCAST(CAgilityBookViewCalendar, pView))
				{
					reinterpret_cast<CAgilityBookViewCalendar*>(pView)->SetCurrentDate(cal->GetStartDate(), true);
					break;
				}
			}
		}
	}
}

void CAgilityBookDoc::OnAgilityNewTraining()
{
	ARBTrainingPtr training(ARBTraining::New());
	CDlgTraining dlg(training, this);
	if (IDOK == dlg.DoModal())
	{
		CMainFrame* pFrame = reinterpret_cast<CMainFrame*>(AfxGetMainWnd());
		pFrame->SetCurTab(3);
		GetTraining().AddTraining(training);
		GetTraining().sort();
		SetModifiedFlag();
		UpdateAllViews(NULL, UPDATE_TRAINING_VIEW);
		POSITION pos = GetFirstViewPosition();
		while (NULL != pos)
		{
			CView* pView = GetNextView(pos);
			if (DYNAMIC_DOWNCAST(CAgilityBookViewTraining, pView))
			{
				reinterpret_cast<CAgilityBookViewTraining*>(pView)->SetCurrentDate(training->GetDate());
				break;
			}
		}
	}
}

void CAgilityBookDoc::OnNotesClubs()
{
	ARBString select;
	ARBDogTrialPtr pTrial = GetCurrentTrial();
	if (pTrial)
		select = pTrial->GetClubs().GetPrimaryClubName();
	CDlgInfoJudge dlg(this, ARBInfo::eClubInfo, select);
	dlg.DoModal();
}

void CAgilityBookDoc::OnNotesJudges()
{
	ARBString select;
	ARBDogRunPtr pRun = GetCurrentRun();
	if (pRun)
		select = pRun->GetJudge();
	CDlgInfoJudge dlg(this, ARBInfo::eJudgeInfo, select);
	dlg.DoModal();
}

void CAgilityBookDoc::OnNotesLocations()
{
	ARBString select;
	ARBDogTrialPtr pTrial = GetCurrentTrial();
	if (pTrial)
		select = pTrial->GetLocation();
	CDlgInfoJudge dlg(this, ARBInfo::eLocationInfo, select);
	dlg.DoModal();
}

class CFindInfo : public IFindCallback
{
public:
	CFindInfo(CAgilityBookDoc* pDoc)
		: m_pDoc(pDoc)
	{
		m_strCaption = _T("Search Notes");
		m_bEnableSearch = false;
		m_bSearchAll = true;
		m_bEnableDirection = false;
	}
	virtual bool Search(CDlgFind* pDlg) const;
	mutable std::vector<CFindItemInfo> m_Items;
private:
	CAgilityBookDoc* m_pDoc;
	void Search(
			CString const& search,
			ARBInfo::eInfoType inType,
			std::set<ARBString> const& inUse,
			ARBInfo const& info) const;
};

bool CFindInfo::Search(CDlgFind* pDlg) const
{
	m_Items.clear();
	CString search = Text();
	if (!MatchCase())
		search.MakeLower();
	ARBInfo& info = m_pDoc->GetInfo();
	std::set<ARBString> inUse;
	m_pDoc->GetAllClubNames(inUse, false);
	Search(search, ARBInfo::eClubInfo, inUse, info);
	m_pDoc->GetAllJudges(inUse, false);
	Search(search, ARBInfo::eJudgeInfo, inUse, info);
	m_pDoc->GetAllTrialLocations(inUse, false);
	Search(search, ARBInfo::eLocationInfo, inUse, info);
	if (0 < m_Items.size())
	{
		pDlg->EndDialog(IDOK);
		return true;
	}
	else
	{
		ARBostringstream msg;
		msg << _T("Cannot find \"")
			<< (LPCTSTR)m_strSearch
			<< _T("\"");
		AfxMessageBox(msg.str().c_str(), MB_ICONINFORMATION);
		return false;
	}
}

void CFindInfo::Search(
		CString const& search,
		ARBInfo::eInfoType inType,
		std::set<ARBString> const& inUse,
		ARBInfo const& info) const
{
	for (std::set<ARBString>::const_iterator iter = inUse.begin(); iter != inUse.end(); ++iter)
	{
		CString str((*iter).c_str());
		if (!MatchCase())
			str.MakeLower();
		if (0 <= str.Find(search))
		{
			CFindItemInfo item;
			item.type = inType;
			item.name = *iter;
			item.pItem.reset();
			m_Items.push_back(item);
			break;
		}
	}
	for (ARBInfoItemList::const_iterator iterItem = info.GetInfo(inType).begin();
		iterItem != info.GetInfo(inType).end();
		++iterItem)
	{
		std::set<ARBString> strings;
		if (0 < (*iterItem)->GetSearchStrings(strings))
		{
			for (std::set<ARBString>::iterator iter = strings.begin(); iter != strings.end(); ++iter)
			{
				CString str((*iter).c_str());
				if (!MatchCase())
					str.MakeLower();
				if (0 <= str.Find(search))
				{
					// First, see if we've inserted the item name
					std::vector<CFindItemInfo>::iterator iter2;
					for (iter2 = m_Items.begin();
						iter2 != m_Items.end();
						++iter2)
					{
						if (iter2->name == (*iterItem)->GetName())
							break;
					}
					CFindItemInfo item;
					item.type = inType;
					item.name = (*iterItem)->GetName();
					item.pItem = *iterItem;
					if (iter2 != m_Items.end())
						*iter2 = item;
					else
						m_Items.push_back(item);
					break;
				}
			}
		}
	}
}

void CAgilityBookDoc::OnNotesSearch()
{
	CFindInfo callback(this);
	CDlgFind dlg(callback);
	if (IDOK == dlg.DoModal())
	{
		CString caption;
		caption.LoadString(IDS_COL_NOTES);
		CDlgListViewer dlg(this, caption, callback.m_Items);
		dlg.DoModal();
	}
}

void CAgilityBookDoc::OnViewOptions()
{
	int nPage;
	CMainFrame* pFrame = reinterpret_cast<CMainFrame*>(AfxGetMainWnd());
	switch (pFrame->GetCurTab())
	{
	default:
		nPage = CDlgOptions::GetProgramPage();
		break;
	case 0: // Runs
	case 1: // Points
		nPage = CDlgOptions::GetFilterPage();
		break;
	case 2: // Calendar
		nPage = CDlgOptions::GetFilterPage();
		break;
	case 3: // Training
		nPage = CDlgOptions::GetFilterPage();
		break;
	}
	CDlgOptions options(this, AfxGetMainWnd(), nPage);
	options.DoModal();
}

void CAgilityBookDoc::OnUpdateViewSortruns(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(CAgilityBookOptions::GetNewestDatesFirst() ? 1 : 0);
}

void CAgilityBookDoc::OnUpdateViewRunsByTrial(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(CAgilityBookOptions::GetViewRunsByTrial() ? 1 : 0);
}

void CAgilityBookDoc::OnUpdateViewHiddenTitles(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(CAgilityBookOptions::GetViewHiddenTitles() ? 1 : 0);
}

void CAgilityBookDoc::OnUpdateViewTableInYPS(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(CAgilityBookOptions::GetTableInYPS() ? 1 : 0);
}
