/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTree.h"
#include "AgilityBookTreeData.h"
#include "AgilityBookViewCalendar.h"
#include "AgilityBookViewCalendarList.h"
#include "AgilityBookViewTraining.h"
#include "DlgCalendar.h"
#include "DlgConfigUpdate.h"
#include "DlgConfigure.h"
#include "DlgDog.h"
#include "DlgInfoJudge.h"
#include "DlgMessage.h"
#include "DlgOptions.h"
#include "DlgSelectDog.h"
#include "DlgTraining.h"
#include "DlgTrial.h"
#include "Element.h"
#include "MainFrm.h"
#include "TabView.h"
#include "Wizard.h"

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
// CAgilityBookDoc

IMPLEMENT_DYNCREATE(CAgilityBookDoc, CDocument)

BEGIN_MESSAGE_MAP(CAgilityBookDoc, CDocument)
	//{{AFX_MSG_MAP(CAgilityBookDoc)
	ON_COMMAND(ID_FILE_EXPORT_WIZARD, OnFileExportWizard)
	ON_COMMAND(ID_EDIT_CONFIGURATION, OnEditConfiguration)
	ON_UPDATE_COMMAND_UI(ID_EDIT_JUDGES, OnUpdateEditJudges)
	ON_COMMAND(ID_EDIT_JUDGES, OnEditJudges)
	ON_COMMAND(ID_AGILITY_NEW_DOG, OnAgilityNewDog)
	ON_COMMAND(ID_AGILITY_NEW_CALENDAR, OnAgilityNewCalendar)
	ON_COMMAND(ID_AGILITY_NEW_TRAINING, OnAgilityNewTraining)
	ON_COMMAND(ID_VIEW_OPTIONS, OnViewOptions)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SORTRUNS, OnUpdateViewSortruns)
	ON_COMMAND(ID_VIEW_SORTRUNS, OnViewSortruns)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RUNS_BY_TRIAL, OnUpdateViewRunsByTrial)
	ON_COMMAND(ID_VIEW_RUNS_BY_TRIAL, OnViewRunsByTrial)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TABLE_IN_YPS, OnUpdateViewTableInYPS)
	ON_COMMAND(ID_VIEW_TABLE_IN_YPS, OnViewTableInYPS)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CAgilityBookDoc::CAgilityBookDoc()
{
}

CAgilityBookDoc::~CAgilityBookDoc()
{
}

/**
 * Return the dog associated with the currently selected item in the tree.
 */
ARBDog* CAgilityBookDoc::GetCurrentDog()
{
	ARBDog* pDog = NULL;
	CAgilityBookTree* pTree = GetTreeView();
	ASSERT(pTree);
	if (pTree && pTree->GetCurrentTreeItem())
		pDog = pTree->GetCurrentTreeItem()->GetDog();
	return pDog;
}

/**
 * Return the trial associated with the currently selected item in the tree.
 */
ARBDogTrial* CAgilityBookDoc::GetCurrentTrial()
{
	ARBDogTrial* pTrial = NULL;
	CAgilityBookTree* pTree = GetTreeView();
	ASSERT(pTree);
	if (pTree && pTree->GetCurrentTreeItem())
		pTrial = pTree->GetCurrentTreeItem()->GetTrial();
	return pTrial;
}

/**
 * Return the run associated with the currently selected item in the tree.
 */
ARBDogRun* CAgilityBookDoc::GetCurrentRun()
{
	ARBDogRun* pRun = NULL;
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
void CAgilityBookDoc::AddTitle(ARBDogRun* pSelectedRun)
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

void CAgilityBookDoc::AddTrial(ARBDogRun* pSelectedRun)
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

void CAgilityBookDoc::AddRun(ARBDogRun* pSelectedRun)
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

void CAgilityBookDoc::EditRun(ARBDogRun* pRun)
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

void CAgilityBookDoc::DeleteRun(ARBDogRun* pRun)
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

bool CAgilityBookDoc::CreateTrialFromCalendar(ARBCalendar const& cal, CTabView* pTabView)
{
	if (!GetConfig().GetVenues().FindVenue(cal.GetVenue()))
		return false;
	bool bCreated = false;
	ARBDogTrial* pTrial = new ARBDogTrial(cal);
	CDlgTrial dlg(this, pTrial);
	if (IDOK == dlg.DoModal())
	{
		std::vector<ARBDog*> dogs;
		CDlgSelectDog dlg(this, dogs);
		if (IDOK == dlg.DoModal() && 0 < dogs.size())
		{
			for (std::vector<ARBDog*>::iterator iter = dogs.begin(); iter != dogs.end(); ++iter)
			{
				bCreated = true;
				ARBDog* pDog = *iter;
				// If we're inserting this entry into more than one dog, we
				// MUST make a copy. Otherwise the trial will be the same trial
				// for both dogs and all changes will be reflected from one to
				// the other - until you save, exit and reload the program.
				ARBDogTrial* pCopyTrial = new ARBDogTrial(*pTrial);
				ARBDogTrial* pNewTrial = pDog->GetTrials().AddTrial(pCopyTrial);
				pCopyTrial->Release();
				SetModifiedFlag();
				UpdateAllViews(NULL, UPDATE_NEW_TRIAL, reinterpret_cast<CObject*>(pNewTrial));
			}
			pTabView->SetCurSel(0);
		}
	}
	pTrial->Release();
	return bCreated;
}

void CAgilityBookDoc::SortDates()
{
	bool bDescending = !CAgilityBookOptions::GetNewestDatesFirst();
	for (ARBDogList::iterator iterDogs = GetDogs().begin(); iterDogs != GetDogs().end(); ++iterDogs)
	{
		ARBDog* pDog = *iterDogs;
		pDog->GetTrials().sort(bDescending);
		for (ARBDogTrialList::iterator iterTrial = pDog->GetTrials().begin(); iterTrial != pDog->GetTrials().end(); ++iterTrial)
		{
			ARBDogTrial* pTrial = *iterTrial;
			pTrial->GetRuns().sort(bDescending);
		}
	}
}

bool CAgilityBookDoc::ImportConfiguration()
{
	bool bOk = false;
	CDlgConfigUpdate dlg;
	if (IDOK == dlg.DoModal())
	{
		ARBConfig& update = dlg.GetConfig();
		CString msg;
		for (ARBConfigActionList::const_iterator iterAction = update.GetActions().begin(); iterAction != update.GetActions().end(); ++iterAction)
		{
			ARBConfigAction const* action = *iterAction;
			if (action->GetVerb() == ACTION_VERB_RENAME_TITLE)
			{
				// Find the venue.
				ARBConfigVenue* venue = GetConfig().GetVenues().FindVenue(action->GetVenue());
				if (venue)
				{
					// Find the title we're renaming.
					ARBConfigTitle* oldTitle = venue->GetDivisions().FindTitle(action->GetOldName());
					if (oldTitle)
					{
						CString tmp;
						tmp.Format("Action: Renaming title [%s] to [%s]",
							action->GetOldName().c_str(),
							action->GetNewName().c_str());
						msg += tmp;
						// If any titles are in use, create a fixup action.
						int nTitles = GetDogs().NumTitlesInUse(action->GetVenue(), action->GetOldName());
						if (0 < nTitles)
						{
							tmp.Format(", updating %d titles\n", nTitles);
							GetDogs().RenameTitle(action->GetVenue(), action->GetOldName(), action->GetNewName());
						}
						else
							tmp = "\n";
						msg += tmp;
						// If the new title exists, just delete the old. Otherwise, rename the old to new.
						ARBConfigTitle const* newTitle = venue->GetDivisions().FindTitle(action->GetNewName());
						if (newTitle)
							venue->GetDivisions().DeleteTitle(action->GetOldName());
						else
							oldTitle->SetName(action->GetNewName());
					}
				}
			}
			else if (action->GetVerb() == ACTION_VERB_DELETE_TITLE)
			{
				// Find the venue.
				ARBConfigVenue* venue = GetConfig().GetVenues().FindVenue(action->GetVenue());
				if (venue)
				{
					// Find the title we're renaming.
					ARBConfigTitle* oldTitle = venue->GetDivisions().FindTitle(action->GetOldName());
					if (oldTitle)
					{
						CString tmp;
						int nTitles = GetDogs().NumTitlesInUse(action->GetVenue(), action->GetOldName());
						// If any titles are in use, create a fixup action.
						if (0 < nTitles)
						{
							if (0 < action->GetNewName().length())
							{
								tmp.Format("Action: Renaming existing %d title(s) [%s] to [%s]\n",
									nTitles,
									action->GetOldName().c_str(),
									action->GetNewName().c_str());
								msg += tmp;
								GetDogs().RenameTitle(action->GetVenue(), action->GetOldName(), action->GetNewName());
							}
							else
							{
								tmp.Format("Action: Deleting existing %d [%s] title(s)\n",
									nTitles,
									action->GetOldName().c_str());
								msg += tmp;
								GetDogs().DeleteTitle(action->GetVenue(), action->GetOldName());
							}
						}
						tmp.Format("Action: Deleting title [%s]\n",
							action->GetOldName().c_str());
						msg += tmp;
						venue->GetDivisions().DeleteTitle(action->GetOldName());
					}
				}
			}
		}
		std::string info;
		GetConfig().Update(0, update, info);
		msg += info.c_str();
		if (0 < msg.GetLength())
		{
			CDlgMessage dlg(msg, 0);
			dlg.DoModal();
			SetModifiedFlag();
			UpdateAllViews(NULL, UPDATE_CONFIG);
		}
		else
			AfxMessageBox(IDS_CONFIG_NO_UPDATE, MB_ICONINFORMATION);
		bOk = true;
	}
	return bOk;
}

void CAgilityBookDoc::ResetVisibility()
{
	std::vector<CVenueFilter> venues;
	CAgilityBookOptions::GetFilterVenue(venues);
	std::set<std::string> names;
	CAgilityBookOptions::GetTrainingFilterNames(names);

	for (ARBDogList::iterator iterDogs = GetDogs().begin(); iterDogs != GetDogs().end(); ++iterDogs)
	{
		ResetVisibility(venues, *iterDogs);
	}

	for (ARBTrainingList::iterator iterTraining = GetTraining().begin(); iterTraining != GetTraining().end(); ++iterTraining)
	{
		ResetVisibility(names, *iterTraining);
	}

// Currently, calendar entries are not filtered.
	//for (ARBCalendarList::iterator iterCal = GetCalendar().begin(); iterCal != GetCalendar().end(); ++iterCal)
	//{
	//	ARBCalendar* pCal = *iterCal;
	//}

	UpdateAllViews(NULL, UPDATE_OPTIONS);
}

void CAgilityBookDoc::ResetVisibility(std::vector<CVenueFilter>& venues, ARBDog* pDog)
{
	for (ARBDogTrialList::iterator iterTrial = pDog->GetTrials().begin(); iterTrial != pDog->GetTrials().end(); ++iterTrial)
		ResetVisibility(venues, *iterTrial);

	for (ARBDogTitleList::iterator iterTitle = pDog->GetTitles().begin(); iterTitle != pDog->GetTitles().end(); ++iterTitle)
		ResetVisibility(venues, *iterTitle);
}

void CAgilityBookDoc::ResetVisibility(std::vector<CVenueFilter>& venues, ARBDogTrial* pTrial)
{
	bool bVisTrial = CAgilityBookOptions::IsTrialVisible(venues, pTrial);
	pTrial->SetFiltered(!bVisTrial);
	if (bVisTrial)
	{
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
}

void CAgilityBookDoc::ResetVisibility(std::vector<CVenueFilter>& venues, ARBDogTrial* pTrial, ARBDogRun* pRun)
{
	bool bVisRun = CAgilityBookOptions::IsRunVisible(venues, pTrial, pRun);
	pRun->SetFiltered(!bVisRun);
}

void CAgilityBookDoc::ResetVisibility(std::vector<CVenueFilter>& venues, ARBDogTitle* pTitle)
{
	bool bVisTitle = CAgilityBookOptions::IsTitleVisible(venues, pTitle);
	pTitle->SetFiltered(!bVisTitle);
}

void CAgilityBookDoc::ResetVisibility(std::set<std::string>& names, ARBTraining* pTraining)
{
	bool bVisTraining = CAgilityBookOptions::IsTrainingLogVisible(names, pTraining);
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
	ASSERT(0);
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
	AfxGetApp()->WriteProfileString("Settings", "LastFile", _T(""));
	m_Records.Default();
	m_Records.GetConfig().GetActions().clear();
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
	if (!CFile::GetStatus(lpszPathName, status))
	{
		AfxGetApp()->WriteProfileString("Settings", "LastFile", _T(""));
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

	// We need a char*, even in UNICODE.
#if _MSC_VER < 1300
	// But since I'm not compiling in unicode...
	CString source(lpszPathName);
#else
	CStringA source(lpszPathName);
#endif
	std::string err;
	Element tree;
	// Translate the XML to a tree form.
	if (!tree.LoadXMLFile((char const*)source, err))
	{
		AfxGetApp()->WriteProfileString("Settings", "LastFile", _T(""));
		CString msg;
		msg.LoadString(AFX_IDP_FAILED_TO_OPEN_DOC);
		if (0 < err.length())
		{
			msg += "\n\n";
			msg += err.c_str();
		}
		AfxMessageBox(msg, MB_ICONEXCLAMATION);
		return FALSE;
	}
	// Translate the tree to a class structure.
	if (!m_Records.Load(tree, err))
	{
		AfxGetApp()->WriteProfileString("Settings", "LastFile", _T(""));
		CString msg;
		msg.LoadString(AFX_IDP_FAILED_TO_OPEN_DOC);
		if (0 < err.length())
		{
			msg += "\n\n";
			msg += err.c_str();
		}
		AfxMessageBox(msg, MB_ICONEXCLAMATION);
		return FALSE;
	}
	else if (0 < err.length())
	{
		CString msg("Warning: Some non-fatal messages were generated while loading the file:\n\n");
		msg += err.c_str();
		AfxMessageBox(msg, MB_ICONINFORMATION);
	}
	SortDates();

	ResetVisibility();

	SetModifiedFlag(FALSE);     // start off with unmodified

	if (CAgilityBookOptions::AutoDeleteCalendarEntries())
	{
		ARBDate today(ARBDate::Today());
		today -= CAgilityBookOptions::DaysTillEntryIsPast();
		if (0 < m_Records.GetCalendar().TrimEntries(today))
			SetModifiedFlag(TRUE);
	}

	AfxGetApp()->WriteProfileString("Settings", "LastFile", lpszPathName);

	if (GetCurrentConfigVersion() != m_Records.GetConfig().GetVersion())
	{
		if (IDYES == AfxMessageBox("The configuration has been updated. Would you like to merge the new one with your data?", MB_ICONQUESTION | MB_YESNO))
		{
			if (ImportConfiguration())
				SetModifiedFlag(TRUE);
		}
	}
	return TRUE;
}

/**
 * MFC method to save a document.
 */
BOOL CAgilityBookDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	CWaitCursor wait;

	BOOL bOk = FALSE;
	Element tree;
	// First, we have to push all the class data into a tree.
	if (m_Records.Save(tree, true, true, true, true, true))
	{
		// Then we can stream that tree out as XML.
		std::ofstream output(lpszPathName, std::ios::out | std::ios::binary);
		output.exceptions(std::ios_base::badbit);
		if (output.is_open())
		{
			if (tree.SaveXML(output))
			{
				AfxGetApp()->WriteProfileString("Settings", "LastFile", lpszPathName);
				bOk = TRUE;
				SetModifiedFlag(FALSE);
			}
			output.close();
		}
		else
		{
			CString errMsg;
			errMsg.FormatMessage(IDS_MIN_RESOLUTION, lpszPathName);
			AfxMessageBox(errMsg, MB_OK | MB_ICONEXCLAMATION);
		}
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

void CAgilityBookDoc::OnFileExportWizard()
{
	CWizard wiz(this);
	wiz.DoModal();
}

void CAgilityBookDoc::OnEditConfiguration()
{
	CDlgConfigure config(this, m_Records);
	config.DoModal();
}

void CAgilityBookDoc::OnUpdateEditJudges(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	std::set<std::string> names;
	GetAllJudges(names);
	if (0 < names.size())
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookDoc::OnEditJudges() 
{
	CDlgInfoJudge dlg(this);
	dlg.DoModal();
}

void CAgilityBookDoc::OnAgilityNewDog()
{
	ARBDog* dog = new ARBDog();
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
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			pFrame->SetCurTab(0);
			SetModifiedFlag();
			ARBDog* pNewDog = GetDogs().AddDog(dog);
			pTree->InsertDog(pNewDog, true);
		}
	}
	dog->Release();
}

void CAgilityBookDoc::OnAgilityNewCalendar()
{
	ARBCalendar* cal = new ARBCalendar();
	CDlgCalendar dlg(cal, this);
	if (IDOK == dlg.DoModal())
	{
		if (!(CAgilityBookOptions::AutoDeleteCalendarEntries() && cal->GetEndDate() < ARBDate::Today()))
		{
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
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
	cal->Release();
}

void CAgilityBookDoc::OnAgilityNewTraining()
{
	ARBTraining* training = new ARBTraining();
	CDlgTraining dlg(training, this);
	if (IDOK == dlg.DoModal())
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
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
	training->Release();
}

void CAgilityBookDoc::OnViewOptions()
{
	int nPage = 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (2 == pFrame->GetCurTab())
		nPage = 1;
	else if (3 == pFrame->GetCurTab())
		nPage = 2;
	CDlgOptions options(this, AfxGetMainWnd(), nPage);
	options.DoModal();
}

void CAgilityBookDoc::OnUpdateViewSortruns(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(CAgilityBookOptions::GetNewestDatesFirst() ? 1 : 0);
}

void CAgilityBookDoc::OnViewSortruns() 
{
	CAgilityBookOptions::SetNewestDatesFirst(!CAgilityBookOptions::GetNewestDatesFirst());
	SortDates();
	UpdateAllViews(NULL, UPDATE_OPTIONS);
}

void CAgilityBookDoc::OnUpdateViewRunsByTrial(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(CAgilityBookOptions::GetViewRunsByTrial() ? 1 : 0);
}

void CAgilityBookDoc::OnViewRunsByTrial()
{
	CAgilityBookOptions::SetViewRunsByTrial(!CAgilityBookOptions::GetViewRunsByTrial());
	UpdateAllViews(NULL, UPDATE_OPTIONS);
}

void CAgilityBookDoc::OnUpdateViewHiddenTrials(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(CAgilityBookOptions::GetViewHiddenTitles() ? 1 : 0);
}

void CAgilityBookDoc::OnViewHiddenTrials()
{
	CAgilityBookOptions::SetViewHiddenTitles(!CAgilityBookOptions::GetViewHiddenTitles());
}

void CAgilityBookDoc::OnUpdateViewTableInYPS(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(CAgilityBookOptions::GetTableInYPS() ? 1 : 0);
}

void CAgilityBookDoc::OnViewTableInYPS()
{
	CAgilityBookOptions::SetTableInYPS(!CAgilityBookOptions::GetTableInYPS());
	AfxGetMainWnd()->Invalidate();
}
