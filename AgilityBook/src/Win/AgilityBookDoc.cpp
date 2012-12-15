/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CAgilityBookDoc class
 * @author David Connet
 *
 * Revision History
 * @li 2012-09-29 DRC Strip the Runs View.
 * @li 2012-07-11 DRC After importing an ARB file, sort it.
 * @li 2012-07-04 DRC Add option to use run time or opening time in gamble OPS.
 * @li 2012-03-16 DRC Renamed LoadXML functions, added stream version.
 * @li 2011-12-22 DRC Switch to using Bind on wx2.9+.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-08-25 DRC Refresh properly after changing filter in context menu.
 * @li 2009-08-19 DRC Create a 'fake' dog when creating a new file.
 * @li 2009-02-05 DRC Ported to wxWidgets.
 * @li 2008-11-19 DRC Added context menus to status bar.
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
#include "AgilityBookDoc.h"

#include "AgilityBook.h"
#include "AgilityBookCalendarListView.h"
#include "AgilityBookCalendarView.h"
#include "AgilityBookMenu.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTrainingView.h"
#include "AgilityBookTreeModel.h"
#include "AgilityBookTreeView.h"
#include "ClipBoard.h"
#include "ConfigHandler.h"
#include "DlgAbout.h"
#include "DlgCalendar.h"
#include "DlgConfigUpdate.h"
#include "DlgConfigure.h"
#include "DlgDog.h"
#include "DlgFindLinks.h"
#include "DlgInfoNote.h"
#include "DlgListViewer.h"
#include "DlgMessage.h"
#include "DlgOptions.h"
#include "DlgSelectDog.h"
#include "DlgTitle.h"
#include "DlgTraining.h"
#include "DlgTrial.h"
#include "Element.h"
#include "FilterOptions.h"
#include "Globals.h"
#include "MainFrm.h"
#include "RegItems.h"
#include "TabView.h"
#include "UpdateInfo.h"
#include "VersionNum.h"
#include "Wizard.h"
#include <algorithm>
#include <fstream>
#include <wx/config.h>
#include <wx/file.h>
#include <wx/filefn.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


IMPLEMENT_DYNAMIC_CLASS(CAgilityBookDoc, wxDocument)

/////////////////////////////////////////////////////////////////////////////

static short GetCurrentConfigVersion()
{
	static short ver = 0;
	static bool bLoaded = false;
	if (!bLoaded)
	{
		wxBusyCursor wait;
		bLoaded = true;
		CConfigHandler handler;
		ARBConfig config;
		config.Default(&handler);
		ver = config.GetVersion();
	}
	return ver;
}


static void UpdateFutureTrials(
		ARBConfig const& config,
		ARBDogPtr pDog,
		ARBDogTitlePtr title)
{
	// Find all trials in the future in the same venue.
	std::vector<ARBDogTrialPtr> trials;
	for (ARBDogTrialList::iterator i = pDog->GetTrials().begin();
		i != pDog->GetTrials().end();
		++i)
	{
		if ((*i)->HasVenue(title->GetVenue())
		&& 0 < (*i)->GetRuns().size())
		{
			ARBDate d1 = (*((*i)->GetRuns().begin()))->GetDate();
			if (d1 > ARBDate::Today())
			{
				trials.push_back(*i);
			}
		}
	}
	if (0 < trials.size())
	{
		/*
		Interesting problem. Level order can be implied by the actual order in
		the config. But what does moveup mean? I could be moving from (USDAA)
		Ch to Pf.
		- Need user option to suppress moveup if in top level
		- Need to add info to a title for what events it affects (or determining
		  if a moveup can happen can't be done)
		- more than one event may be a move (USDAA AD implies everything goes up
		Oh. Even more complicated. Earning NovA means NovB moveOVER.
		*/
#pragma PRAGMA_TODO("Add dialog to update future trials")
		/*
		Thought 2. Maybe we just fly an info dialog here 'You are entered in
		N upcoming trials (list). Please check for moveups.'
		*/
	}
}

/////////////////////////////////////////////////////////////////////////////

CErrorCallback::CErrorCallback()
	: ARBErrorCallback(m_ErrMsg)
{
}


bool CErrorCallback::OnError(wchar_t const* const pMsg)
{
	return wxYES == wxMessageBox(pMsg, wxMessageBoxCaptionStr, wxYES_NO | wxNO_DEFAULT | wxCENTRE | wxICON_EXCLAMATION);
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(CAgilityBookDoc, wxDocument)
	EVT_UPDATE_UI(ID_FILE_EXPORT_WIZARD, CAgilityBookDoc::OnUpdateCmd)
	EVT_MENU(ID_FILE_EXPORT_WIZARD, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_FILE_LINKED, CAgilityBookDoc::OnUpdateCmd)
	EVT_MENU(ID_FILE_LINKED, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_COPY_TITLES_LIST, CAgilityBookDoc::OnUpdateCmd)
	EVT_MENU(ID_COPY_TITLES_LIST, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_EDIT_CONFIGURATION, CAgilityBookDoc::OnUpdateCmd)
	EVT_MENU(ID_EDIT_CONFIGURATION, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_AGILITY_NEW_DOG, CAgilityBookDoc::OnUpdateCmd)
	EVT_MENU(ID_AGILITY_NEW_DOG, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_AGILITY_NEW_CALENDAR, CAgilityBookDoc::OnUpdateCmd)
	EVT_MENU(ID_AGILITY_NEW_CALENDAR, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_AGILITY_UPDATE_CALENDAR, CAgilityBookDoc::OnUpdateCmd)
	EVT_MENU(ID_AGILITY_UPDATE_CALENDAR, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_AGILITY_NEW_TRAINING, CAgilityBookDoc::OnUpdateCmd)
	EVT_MENU(ID_AGILITY_NEW_TRAINING, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_NOTES_CLUBS, CAgilityBookDoc::OnUpdateCmd)
	EVT_MENU(ID_NOTES_CLUBS, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_NOTES_JUDGES, CAgilityBookDoc::OnUpdateCmd)
	EVT_MENU(ID_NOTES_JUDGES, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_NOTES_LOCATIONS, CAgilityBookDoc::OnUpdateCmd)
	EVT_MENU(ID_NOTES_LOCATIONS, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_NOTES_SEARCH, CAgilityBookDoc::OnUpdateCmd)
	EVT_MENU(ID_NOTES_SEARCH, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(wxID_PREFERENCES, CAgilityBookDoc::OnUpdateCmd)
	EVT_MENU(wxID_PREFERENCES, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_VIEW_SORTRUNS, CAgilityBookDoc::OnUpdateCmd)
	EVT_UPDATE_UI(ID_VIEW_RUNS_BY_TRIAL, CAgilityBookDoc::OnUpdateCmd)
	EVT_UPDATE_UI(ID_VIEW_HIDDEN, CAgilityBookDoc::OnUpdateCmd)
	EVT_UPDATE_UI(ID_VIEW_TABLE_IN_YPS, CAgilityBookDoc::OnUpdateCmd)
	EVT_UPDATE_UI(ID_VIEW_RUNTIME_IN_OPS, CAgilityBookDoc::OnUpdateCmd)
	EVT_UPDATE_UI(ID_VIEW_LIFETIME_EVENTS, CAgilityBookDoc::OnUpdateCmd)
	EVT_UPDATE_UI(wxID_ABOUT, CAgilityBookDoc::OnUpdateCmd)
	EVT_MENU(wxID_ABOUT, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_HELP_UPDATE, CAgilityBookDoc::OnUpdateCmd)
	EVT_MENU(ID_HELP_UPDATE, CAgilityBookDoc::OnCmd)
END_EVENT_TABLE()


CAgilityBookDoc::CAgilityBookDoc()
	: m_Records()
	, m_CalSites()
	, m_StatusData(NULL)
{
}


CAgilityBookDoc::~CAgilityBookDoc()
{
}


class CStatusHandler
{
public:
	CStatusHandler() {}
	// For dog menu
	std::vector<ARBDogPtr> dogs;
	// For filter menu
	CFilterOptions filterOptions;
	std::vector<std::wstring> filterNames;
};


static const int baseID = 100;


void CAgilityBookDoc::OnStatusDog(wxCommandEvent& evt)
{
	if (m_StatusData
	&& baseID <= evt.GetId()
	&& evt.GetId() < static_cast<int>(m_StatusData->dogs.size()) + baseID
	&& m_StatusData->dogs[evt.GetId()-baseID] != GetCurrentDog())
	{
#pragma PRAGMA_TODO("Fix me")
#if 0
		CAgilityBookTreeView* pTree = GetTreeView();
		if (pTree)
			pTree->SelectDog(m_StatusData->dogs[evt.GetId()-baseID]);
#endif
	}
}


void CAgilityBookDoc::OnStatusFilter(wxCommandEvent& evt)
{
	if (m_StatusData
	&& baseID <= evt.GetId()
	&& evt.GetId() < static_cast<int>(m_StatusData->filterNames.size()) + baseID
	&& m_StatusData->filterNames[evt.GetId()-baseID] != m_StatusData->filterOptions.GetCurrentFilter())
	{
		m_StatusData->filterOptions.SetCurrentFilter(m_StatusData->filterNames[evt.GetId()-baseID]);
		m_StatusData->filterOptions.Save();
		CFilterOptions::Options().Load();
		if (ResetVisibility())
		{
			CUpdateHint hint(UPDATE_ALL_VIEW);
			UpdateAllViews(NULL, &hint);
		}
	}
}


bool CAgilityBookDoc::StatusBarContextMenu(
		wxWindow* parent,
		int id,
		wxPoint const& point)
{
	if (!parent)
		return false;
	bool bHandled = false;
	CAgilityBookBaseExtraView* pView = wxDynamicCast(GetDocumentManager()->GetCurrentView(), CAgilityBookBaseExtraView);
	if (pView && pView->AllowStatusContext(id))
	{
		switch (id)
		{
		case STATUS_DOG:
			if (GetTreeModel() && 1 < m_Records.GetDogs().size())
			{
				ARBDogPtr curDog = GetCurrentDog();
				wxMenu* menu = new wxMenu();
				int menuId = baseID;
				CStatusHandler data;
				ARBDogList::const_iterator iDog;
				for (iDog = m_Records.GetDogs().begin(); iDog != m_Records.GetDogs().end(); ++iDog, ++menuId)
				{
					BIND_OR_CONNECT_ID_CTRL(parent, menuId, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookDoc::OnStatusDog);
					std::wstring item = StringUtil::Replace((*iDog)->GetGenericName(), L"&", L"&&");
					wxMenuItem* menuitem = menu->AppendCheckItem(menuId, StringUtil::stringWX(item));
					if (*(*iDog) == *curDog)
						menuitem->Check(true);
					data.dogs.push_back(*iDog);
				}
				bHandled = true;
				m_StatusData = &data;
				parent->PopupMenu(menu, point);
				delete menu;
				menuId = baseID;
				for (iDog = m_Records.GetDogs().begin(); iDog != m_Records.GetDogs().end(); ++iDog, ++menuId)
				{
					UNBIND_OR_DISCONNECT_ID_CTRL(parent, menuId, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookDoc::OnStatusDog);
				}
			}
			break;
		case STATUS_STATUS:
			break;
		case STATUS_FILTERED:
			{
				CStatusHandler data;
				data.filterOptions.GetAllFilterNames(data.filterNames);
				if (0 < data.filterNames.size())
				{
					std::wstring filterName = data.filterOptions.GetCurrentFilter();
					wxMenu* menu = new wxMenu();
					int menuId = baseID;
					std::vector<std::wstring>::const_iterator iFilter;
					for (iFilter = data.filterNames.begin();
						iFilter != data.filterNames.end();
						++iFilter, ++menuId)
					{
						BIND_OR_CONNECT_ID_CTRL(parent, menuId, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookDoc::OnStatusFilter);
						std::wstring item = StringUtil::Replace(*iFilter, L"&", L"&&");
						wxMenuItem* menuitem = menu->AppendCheckItem(menuId, StringUtil::stringWX(item));
						if (*iFilter == filterName)
							menuitem->Check(true);
					}
					bHandled = true;
					m_StatusData = &data;
					parent->PopupMenu(menu, point);
					delete menu;
					menuId = baseID;
					for (iFilter = data.filterNames.begin();
						iFilter != data.filterNames.end();
						++iFilter, ++menuId)
					{
						UNBIND_OR_DISCONNECT_ID_CTRL(parent, menuId, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookDoc::OnStatusFilter);
					}
				}
			}
			break;
		}
	}
	return bHandled;
}


std::wstring CAgilityBookDoc::AddDogToCaption(std::wstring const& caption) const
{
	if (GetCurrentDog())
		return GetCurrentDog()->GetCallName() + L": " + caption;
	else
		return caption;
}


/**
 * Return the dog associated with the currently selected item in the tree.
 */
ARBDogPtr CAgilityBookDoc::GetCurrentDog() const
{
	ARBDogPtr pDog;
	if (1 == m_Records.GetDogs().size())
	{
		pDog = *(m_Records.GetDogs().begin());
	}
	else
	{
#pragma PRAGMA_TODO("Fix me")
#if 0
	CAgilityBookTreeView* pTree = GetTreeView();
	if (pTree && pTree->GetCurrentTreeItem())
		pDog = pTree->GetCurrentTreeItem()->GetDog();
#endif
	}
	return pDog;
}


/**
 * Return the trial associated with the currently selected item in the tree.
 */
ARBDogTrialPtr CAgilityBookDoc::GetCurrentTrial() const
{
	ARBDogTrialPtr pTrial;
#pragma PRAGMA_TODO("Fix me")
#if 0
	CAgilityBookTreeView* pTree = GetTreeView();
	assert(pTree);
	if (pTree && pTree->GetCurrentTreeItem())
		pTrial = pTree->GetCurrentTreeItem()->GetTrial();
#endif
	return pTrial;
}


/**
 * Return the run associated with the currently selected item in the tree.
 */
ARBDogRunPtr CAgilityBookDoc::GetCurrentRun() const
{
	ARBDogRunPtr pRun;
#pragma PRAGMA_TODO("Fix me")
#if 0
	CAgilityBookTreeView* pTree = GetTreeView();
	assert(pTree);
	if (pTree && pTree->GetCurrentTreeItem())
		pRun = pTree->GetCurrentTreeItem()->GetRun();
#endif
	return pRun;
}


bool CAgilityBookDoc::AddTitle(ARBDogPtr pDog)
{
	if (pDog)
	{
		CDlgTitle dlg(Book().GetConfig(), pDog->GetTitles(), ARBDogTitlePtr());
		if (wxID_OK == dlg.ShowModal())
		{
			std::vector<CVenueFilter> venues;
			CFilterOptions::Options().GetFilterVenue(venues);
			ResetVisibility(venues, dlg.GetNewTitle());
			UpdateFutureTrials(Book().GetConfig(), pDog, dlg.GetNewTitle());
			CUpdateHint hint(UPDATE_POINTS_VIEW);
			UpdateAllViews(NULL, &hint);
			Modify(true);
			if (CAgilityBookOptions::AutoShowPropertiesOnNewTitle())
			{
				CDlgDog dlg(this, pDog, wxGetApp().GetTopWindow(), 1);
				dlg.ShowModal();
			}
			return true;
		}
	}
	return false;
}


/**
 * Called from the Runs view. Since the run is visible in that view and visible
 * runs are controlled by the selected item in the tree, 'pData' should never
 * be NULL.
 */

void CAgilityBookDoc::AddTrial(ARBDogRunPtr pSelectedRun)
{
#pragma PRAGMA_TODO("Fix me")
#if 0
	CAgilityBookTreeView* pTree = GetTreeView();
	assert(pTree);
	CAgilityBookTreeData* pData = pTree->FindData(pSelectedRun);
	if (pData)
	{
		pTree->EnsureVisible(pData->GetId());
		bool bModified = false;
		if (pData->OnCmd(ID_AGILITY_NEW_TRIAL, bModified, NULL))
		{
			if (bModified)
				Modify(true);
		}
	}
#endif
}


void CAgilityBookDoc::AddRun(ARBDogRunPtr pSelectedRun)
{
#pragma PRAGMA_TODO("Fix me")
#if 0
	CAgilityBookTreeView* pTree = GetTreeView();
	assert(pTree);
	CAgilityBookTreeData* pData = pTree->FindData(pSelectedRun);
	if (pData)
	{
		pTree->EnsureVisible(pData->GetId());
		bool bModified = false;
		if (pData->OnCmd(ID_AGILITY_NEW_RUN, bModified, NULL))
		{
			if (bModified)
				Modify(true);
		}
	}
#endif
}


void CAgilityBookDoc::EditRun(ARBDogRunPtr pRun)
{
#pragma PRAGMA_TODO("Fix me")
#if 0
	CAgilityBookTreeView* pTree = GetTreeView();
	assert(pTree);
	CAgilityBookTreeData* pData = pTree->FindData(pRun);
	if (pData)
	{
		pTree->EnsureVisible(pData->GetId());
		bool bModified = false;
		if (pData->OnCmd(ID_AGILITY_EDIT_RUN, bModified, NULL))
		{
			if (bModified)
				Modify(true);
		}
	}
#endif
}


void CAgilityBookDoc::DeleteRun(ARBDogRunPtr pRun)
{
#pragma PRAGMA_TODO("Fix me")
#if 0
	CAgilityBookTreeView* pTree = GetTreeView();
	assert(pTree);
	CAgilityBookTreeData* pData = pTree->FindData(pRun);
	if (pData)
	{
		pTree->EnsureVisible(pData->GetId());
		bool bModified = false;
		if (pData->OnCmd(ID_AGILITY_DELETE_RUN, bModified, NULL))
		{
			if (bModified)
				Modify(true);
		}
	}
#endif
}


bool CAgilityBookDoc::CreateTrialFromCalendar(
		ARBCalendar const& cal,
		CTabView* pTabView)
{
	if (!m_Records.GetConfig().GetVenues().FindVenue(cal.GetVenue()))
		return false;
	bool bCreated = false;
	ARBDogTrialPtr pTrial(ARBDogTrial::New(cal));
	CDlgTrial dlg(this, pTrial, wxGetApp().GetTopWindow());
	if (wxID_OK == dlg.ShowModal())
	{
		std::vector<ARBDogPtr> dogs;
		CDlgSelectDog dlgDogs(this, dogs,wxGetApp().GetTopWindow());
		if (wxID_OK == dlgDogs.ShowModal() && 0 < dogs.size())
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
				Modify(true);
				CUpdateHint hint(UPDATE_NEW_TRIAL, pNewTrial);
				UpdateAllViews(NULL, &hint);
			}
			pTabView->SetCurTab(0);
		}
	}
	return bCreated;
}


void CAgilityBookDoc::SortDates()
{
	bool bDescending = !CAgilityBookOptions::GetNewestDatesFirst();
	for (ARBDogList::iterator iterDogs = m_Records.GetDogs().begin(); iterDogs != m_Records.GetDogs().end(); ++iterDogs)
	{
		ARBDogPtr pDog = *iterDogs;
		pDog->GetTrials().sort(bDescending);
	}
}


class CConfigActionCallback : public IConfigActionCallback
{
public:
	CConfigActionCallback() {}
	virtual void PreDelete(std::wstring const& inMsg);
	virtual void PostDelete(std::wstring const& inMsg) const;
};


void CConfigActionCallback::PreDelete(std::wstring const& inMsg)
{
	wxString msg(StringUtil::stringWX(inMsg));
	msg += L"\n\n";
	msg += _("IDS_ARE_YOU_SURE_CONTINUE");
	if (wxID_NO == wxMessageBox(msg, wxMessageBoxCaptionStr, wxYES_NO | wxCENTRE | wxICON_WARNING))
	{
		m_bContinue = false;
	}
}


void CConfigActionCallback::PostDelete(std::wstring const& msg) const
{
	wxMessageBox(StringUtil::stringWX(msg), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_WARNING);
}


void CAgilityBookDoc::ImportConfiguration(ARBConfig& update)
{
	std::wostringstream info;
	CConfigActionCallback callback;
	bool bOk = false;
	{
		wxBusyCursor wait;
		bOk = m_Records.Update(0, update, info, callback);
	}
	if (bOk)
	{
		CDlgMessage dlg(info.str(), wxGetApp().GetTopWindow());
		dlg.ShowModal();
		Modify(true);
		CUpdateHint hint(UPDATE_CONFIG);
		UpdateAllViews(NULL, &hint);
	}
	else
		wxMessageBox(_("IDS_CONFIG_NO_UPDATE"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
}


bool CAgilityBookDoc::ImportConfiguration(bool bUseDefault)
{
	bool bOk = false;
	bool bDoIt = false;
	CDlgConfigUpdate dlg(wxGetApp().GetTopWindow());
	if (bUseDefault)
	{
		bDoIt = true;
		dlg.LoadConfig(NULL);
	}
	else
	{
		if (wxID_OK == dlg.ShowModal())
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


bool CAgilityBookDoc::ImportARBCalEntry(
		ARBCalendarList const& listCal,
		long& nAdded,
		long& nUpdated,
		long& nDuplicate,
		long& nSkipped)
{
	wxBusyCursor wait;
	bool bModified = false;
	for (ARBCalendarList::const_iterator iter = listCal.begin(); iter != listCal.end(); ++iter)
	{
		ARBCalendarPtr pCal = *iter;
		ARBCalendarPtr calFound;
		if (!m_Records.GetCalendar().FindCalendar(pCal, false, &calFound))
		{
			if (!(CAgilityBookOptions::AutoDeleteCalendarEntries() && pCal->GetEndDate() < ARBDate::Today()))
			{
				m_Records.GetCalendar().AddCalendar(pCal);
				++nAdded;
			}
			else
				++nSkipped;
		}
		else
		{
			if (calFound->Update(pCal))
				++nUpdated;
			else
				++nDuplicate;
		}
	}
	return bModified;
}


bool CAgilityBookDoc::ImportARBTrainingEntry(
		ARBTrainingList const& listLog,
		long& nAdded,
		long& nUpdated,
		long& nDuplicate,
		long& nSkipped)
{
	wxBusyCursor wait;
	bool bModified = false;
	for (ARBTrainingList::const_iterator iter = listLog.begin(); iter != listLog.end(); ++iter)
	{
		ARBTrainingPtr pLog = *iter;
		if (!m_Records.GetTraining().FindTraining(pLog))
		{
			bModified = true;
			m_Records.GetTraining().AddTraining(pLog);
			++nAdded;
		}
		else
		{
			bModified = true;
			++nDuplicate;
		}
	}
	return bModified;
}


bool CAgilityBookDoc::ImportARBRunData(ElementNodePtr inTree, wxWindow* pParent)
{
	wxBusyCursor wait;
	bool bOk = false;
	CErrorCallback err;
	ARBAgilityRecordBook book;
	if (book.Load(inTree, false, false, true, true, true, err))
	{
		if (0 < err.m_ErrMsg.str().length())
			wxMessageBox(StringUtil::stringWX(err.m_ErrMsg.str()), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
		int countDog = 0;
		int countRegNumsAdded = 0;
		int countRegNumsUpdated = 0;
		int countExistingPts = 0;
		int countTitlesAdded = 0;
		int countTitlesUpdated = 0;
		int countTrials = 0;
		int countClubs = 0;
		int countJudges = 0;
		int countLocations = 0;
		for (ARBDogList::iterator iterDog = book.GetDogs().begin();
			iterDog != book.GetDogs().end();
			++iterDog)
		{
			ARBDogPtr pDog = *iterDog;
			ARBDogPtr pExisting;
			for (ARBDogList::iterator iterDog2 = m_Records.GetDogs().begin();
				iterDog2 != m_Records.GetDogs().end();
				++iterDog2)
			{
				if ((*iterDog2)->GetCallName() == pDog->GetCallName())
				{
					pExisting = *iterDog2;
					break;
				}
			}
			if (!pExisting)
			{
				++countDog;
				m_Records.GetDogs().AddDog(pDog);
			}
			else if (*pExisting != *pDog)
			{
				// If the dog exists, only update the existing points,
				// registration numbers, titles and trials.
				if (pExisting->GetRegNums() != pDog->GetRegNums())
				{
					for (ARBDogRegNumList::iterator iter = pDog->GetRegNums().begin();
						iter != pDog->GetRegNums().end();
						++iter)
					{
						ARBDogRegNumPtr pRegNum = *iter;
						ARBDogRegNumPtr pRegExist;
						if (pExisting->GetRegNums().FindRegNum(pRegNum->GetVenue(), &pRegExist))
						{
							if (*pRegExist != *pRegNum)
							{
								++countRegNumsUpdated;
								*pRegExist = *pRegNum;
							}
						}
						else
						{
							++countRegNumsAdded;
							pExisting->GetRegNums().AddRegNum(pRegNum);
						}
					}
				}
				if (pExisting->GetExistingPoints() != pDog->GetExistingPoints())
				{
					for (ARBDogExistingPointsList::iterator iter = pDog->GetExistingPoints().begin();
						iter != pDog->GetExistingPoints().end();
						++iter)
					{
						bool bFound = false;
						for (ARBDogExistingPointsList::iterator iter2 = pExisting->GetExistingPoints().begin();
							iter2 != pExisting->GetExistingPoints().end();
							++iter2)
						{
							if (*(*iter) == *(*iter2))
							{
								bFound = true;
								break;
							}
						}
						if (!bFound)
						{
							++countExistingPts;
							pExisting->GetExistingPoints().AddExistingPoints((*iter));
						}
					}
				}
				if (pExisting->GetTitles() != pDog->GetTitles())
				{
					for (ARBDogTitleList::iterator iter = pDog->GetTitles().begin();
						iter != pDog->GetTitles().end();
						++iter)
					{
						ARBDogTitlePtr pTitle = *iter;
						ARBDogTitlePtr pTitleExist;
						if (pExisting->GetTitles().FindTitle(pTitle->GetVenue(), pTitle->GetRawName(), &pTitleExist))
						{
							if (*pTitle != *pTitleExist)
							{
								++countTitlesUpdated;
								*pTitleExist = *pTitle;
							}
						}
						else
						{
							++countTitlesAdded;
							ARBDogTitlePtr pNewTitle = pTitle->Clone();
							pExisting->GetTitles().AddTitle(pNewTitle);
						}
					}
				}
				if (pExisting->GetTrials() != pDog->GetTrials())
				{
					for (ARBDogTrialList::iterator iter = pDog->GetTrials().begin();
						iter != pDog->GetTrials().end();
						++iter)
					{
						bool bFound = false;
						for (ARBDogTrialList::iterator iter2 = pExisting->GetTrials().begin();
							iter2 != pExisting->GetTrials().end();
							++iter2)
						{
							if (*(*iter) == *(*iter2))
							{
								bFound = true;
								break;
							}
						}
						if (!bFound)
						{
							++countTrials;
							pExisting->GetTrials().AddTrial((*iter));
						}
					}
				}
			}
			pDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
		}

		for (ARBInfoItemList::const_iterator iterClub = book.GetInfo().GetInfo(ARBInfo::eClubInfo).begin();
			iterClub != book.GetInfo().GetInfo(ARBInfo::eClubInfo).end();
			++iterClub)
		{
			ARBInfoItemPtr pClub = *iterClub;
			// If this fails, it already exists.
			if (m_Records.GetInfo().GetInfo(ARBInfo::eClubInfo).AddItem(pClub))
			{
				++countClubs;
			}
		}
		for (ARBInfoItemList::const_iterator iterJudge = book.GetInfo().GetInfo(ARBInfo::eJudgeInfo).begin();
			iterJudge != book.GetInfo().GetInfo(ARBInfo::eJudgeInfo).end();
			++iterJudge)
		{
			ARBInfoItemPtr pJudge = *iterJudge;
			// If this fails, it already exists.
			if (m_Records.GetInfo().GetInfo(ARBInfo::eJudgeInfo).AddItem(pJudge))
			{
				++countJudges;
			}
		}
		for (ARBInfoItemList::const_iterator iterLocation = book.GetInfo().GetInfo(ARBInfo::eLocationInfo).begin();
			iterLocation != book.GetInfo().GetInfo(ARBInfo::eLocationInfo).end();
			++iterLocation)
		{
			ARBInfoItemPtr pLocation = *iterLocation;
			// If this fails, it already exists.
			if (m_Records.GetInfo().GetInfo(ARBInfo::eLocationInfo).AddItem(pLocation))
			{
				++countLocations;
			}
		}
		if (0 < countDog
		|| 0 < countRegNumsAdded || 0 < countRegNumsUpdated
		|| 0 < countExistingPts
		|| 0 < countTitlesAdded || 0 < countTitlesUpdated
		|| 0 < countTrials)
		{
			CUpdateHint hint(UPDATE_ALL_VIEW);
			UpdateAllViews(NULL, &hint);
			Modify(true);
		}
		if (0 < countClubs)
		{
			std::set<std::wstring> namesInUse;
			m_Records.GetAllClubNames(namesInUse, false, false);
			m_Records.GetInfo().GetInfo(ARBInfo::eClubInfo).CondenseContent(namesInUse);
		}
		if (0 < countJudges)
		{
			std::set<std::wstring> namesInUse;
			m_Records.GetAllJudges(namesInUse, false, false);
			m_Records.GetInfo().GetInfo(ARBInfo::eJudgeInfo).CondenseContent(namesInUse);
		}
		if (0 < countLocations)
		{
			std::set<std::wstring> namesInUse;
			m_Records.GetAllTrialLocations(namesInUse, false, false);
			m_Records.GetInfo().GetInfo(ARBInfo::eLocationInfo).CondenseContent(namesInUse);
		}
		wxString str(_("IDS_ADDED"));
		bool bAdded = false;
		if (0 < countDog)
		{
			if (bAdded)
				str += L", ";
			bAdded = true;
			str += wxString::Format(_("IDS_ADDED_DOGS"), countDog);
		}
		if (0 < countRegNumsAdded)
		{
			if (bAdded)
				str += L", ";
			bAdded = true;
			str += wxString::Format(_("IDS_ADDED_REGNUMS"), countRegNumsAdded);
		}
		if (0 < countExistingPts)
		{
			if (bAdded)
				str += L", ";
			bAdded = true;
			str += wxString::Format(_("IDS_ADDED_EXISTINGPTS"), countExistingPts);
		}
		if (0 < countTitlesAdded)
		{
			if (bAdded)
				str += L", ";
			bAdded = true;
			str += wxString::Format(_("IDS_ADDED_TITLES"), countTitlesAdded);
		}
		if (0 < countTrials)
		{
			if (bAdded)
				str += L", ";
			bAdded = true;
			str += wxString::Format(_("IDS_ADDED_TRIALS"), countTrials);
		}
		if (0 < countClubs)
		{
			if (bAdded)
				str += L", ";
			bAdded = true;
			str += wxString::Format(_("IDS_ADDED_CLUBS"), countClubs);
		}
		if (0 < countJudges)
		{
			if (bAdded)
				str += L", ";
			bAdded = true;
			str += wxString::Format(_("IDS_ADDED_JUDGES"), countJudges);
		}
		if (0 < countLocations)
		{
			if (bAdded)
				str += L", ";
			bAdded = true;
			str += wxString::Format(_("IDS_ADDED_LOCATIONS"), countLocations);
		}
		bAdded = false;
		if (0 < countRegNumsUpdated)
		{
			if (bAdded)
				str += L", ";
			else
			{
				str += L"\n";
				str += _("IDS_UPDATED");
			}
			bAdded = true;
			str += wxString::Format(_("IDS_ADDED_REGNUMS"), countRegNumsUpdated);
		}
		if (0 < countTitlesUpdated)
		{
			if (bAdded)
				str += L", ";
			else
			{
				str += L"\n";
				str += _("IDS_UPDATED");
			}
			bAdded = true;
			str += wxString::Format(_("IDS_ADDED_TITLES"), countTitlesUpdated);
		}
		wxMessageBox(str, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
		bOk = true;
	}
	else if (0 < err.m_ErrMsg.str().length())
		wxMessageBox(StringUtil::stringWX(err.m_ErrMsg.str()), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_WARNING);
	return bOk;
}


bool CAgilityBookDoc::ImportARBCalData(ElementNodePtr inTree, wxWindow* pParent)
{
	wxBusyCursor wait;
	bool bOk = false;
	CErrorCallback err;
	ARBAgilityRecordBook book;
	if (book.Load(inTree, true, false, false, false, false, err))
	{
		if (0 < err.m_ErrMsg.str().length())
			wxMessageBox(StringUtil::stringWX(err.m_ErrMsg.str()), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);

		long nAdded = 0;
		long nUpdated = 0;
		long nDuplicate = 0;
		long nSkipped = 0;
		if (ImportARBCalEntry(book.GetCalendar(), nAdded, nUpdated, nDuplicate, nSkipped))
		{
			m_Records.GetCalendar().sort();
			CUpdateHint hint(UPDATE_CALENDAR_VIEW);
			UpdateAllViews(NULL, &hint);
			Modify(true);
		}

		wxString str = wxString::Format(_("IDS_UPDATED_CAL_ITEMS"), nAdded, nUpdated);
		wxMessageBox(str, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
		bOk = true;
	}
	else if (0 < err.m_ErrMsg.str().length())
		wxMessageBox(StringUtil::stringWX(err.m_ErrMsg.str()), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_WARNING);
	return bOk;
}


bool CAgilityBookDoc::ImportARBLogData(ElementNodePtr inTree, wxWindow* pParent)
{
	wxBusyCursor wait;
	bool bOk = false;
	CErrorCallback err;
	ARBAgilityRecordBook book;
	if (book.Load(inTree, false, true, false, false, false, err))
	{
		if (0 < err.m_ErrMsg.str().length())
			wxMessageBox(StringUtil::stringWX(err.m_ErrMsg.str()), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);

		long nAdded = 0;
		long nUpdated = 0;
		long nDuplicate = 0;
		long nSkipped = 0;
		if (ImportARBTrainingEntry(book.GetTraining(), nAdded, nUpdated, nDuplicate, nSkipped))
		{
			m_Records.GetTraining().sort();
			CUpdateHint hint(UPDATE_TRAINING_VIEW);
			UpdateAllViews(NULL, &hint);
			Modify(true);
		}

		wxString str = wxString::Format(_("IDS_UPDATED_TRAINING_ITEMS"), nAdded, nUpdated);
		wxMessageBox(str, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
		bOk = true;
	}
	else if (0 < err.m_ErrMsg.str().length())
		wxMessageBox(StringUtil::stringWX(err.m_ErrMsg.str()), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_WARNING);
	return bOk;
}


bool CAgilityBookDoc::ResetVisibility()
{
	bool bChanged = false;
	std::vector<CVenueFilter> venues;
	CFilterOptions::Options().GetFilterVenue(venues);
	std::set<std::wstring> names;
	CFilterOptions::Options().GetTrainingFilterNames(names);

	for (ARBDogList::iterator iterDogs = m_Records.GetDogs().begin(); iterDogs != m_Records.GetDogs().end(); ++iterDogs)
	{
		bChanged |= ResetVisibility(venues, *iterDogs);
	}

	for (ARBTrainingList::iterator iterTraining = m_Records.GetTraining().begin(); iterTraining != m_Records.GetTraining().end(); ++iterTraining)
	{
		bChanged |= ResetVisibility(names, *iterTraining);
	}

	for (ARBCalendarList::iterator iterCal = m_Records.GetCalendar().begin(); iterCal != m_Records.GetCalendar().end(); ++iterCal)
	{
		ARBCalendarPtr pCal = *iterCal;
		bool bVis = CFilterOptions::Options().IsCalendarVisible(venues, pCal);
		if (pCal->IsAnyFiltered() != !bVis)
		{
			bChanged = true;
			pCal->SetFiltered(!bVis);
		}
	}
	return bChanged;
}


bool CAgilityBookDoc::ResetVisibility(
		std::vector<CVenueFilter>& venues,
		ARBDogPtr pDog)
{
	bool bChanged = false;
	for (ARBDogTrialList::iterator iterTrial = pDog->GetTrials().begin(); iterTrial != pDog->GetTrials().end(); ++iterTrial)
		bChanged |= ResetVisibility(venues, *iterTrial);

	for (ARBDogTitleList::iterator iterTitle = pDog->GetTitles().begin(); iterTitle != pDog->GetTitles().end(); ++iterTitle)
		bChanged |= ResetVisibility(venues, *iterTitle);
	return bChanged;
}


bool CAgilityBookDoc::ResetVisibility(
		std::vector<CVenueFilter>& venues,
		ARBDogTrialPtr pTrial)
{
	bool bChanged = false;
	bool bVisTrial = CFilterOptions::Options().IsTrialVisible(venues, pTrial);
	pTrial->SetFiltered(!bVisTrial);
	int nVisible = 0;
	for (ARBDogRunList::iterator iterRun = pTrial->GetRuns().begin(); iterRun != pTrial->GetRuns().end(); ++iterRun)
	{
		bChanged |= ResetVisibility(venues, pTrial, *iterRun);
		if (!(*iterRun)->IsFiltered())
			++nVisible;
	}
	if (0 == nVisible && 0 < pTrial->GetRuns().size())
	{
		if (!pTrial->IsAnyFiltered())
		{
			bChanged = true;
			pTrial->SetFiltered(true);
		}
	}
	return bChanged;
}


bool CAgilityBookDoc::ResetVisibility(
		std::vector<CVenueFilter>& venues,
		ARBDogTrialPtr pTrial,
		ARBDogRunPtr pRun)
{
	bool bChanged = false;
	unsigned short nVisRun = CFilterOptions::Options().IsRunVisible(venues, pTrial, pRun);
	bool bFilter = (nVisRun & (0x1 << ARBBase::eFilter)) ? false : true;
	bool bIgnore = (nVisRun & (0x1 << ARBBase::eIgnoreQ)) ? false : true;
	if (pRun->IsFiltered(ARBBase::eFilter) != bFilter)
	{
		bChanged = true;
		pRun->SetFiltered(ARBBase::eFilter, bFilter);
	}
	if (pRun->IsFiltered(ARBBase::eIgnoreQ) != bIgnore)
	{
		bChanged = true;
		pRun->SetFiltered(ARBBase::eIgnoreQ, bIgnore);
	}
	return bChanged;
}


bool CAgilityBookDoc::ResetVisibility(
		std::vector<CVenueFilter>& venues,
		ARBDogTitlePtr pTitle)
{
	bool bChanged = false;
	bool bVisTitle = CFilterOptions::Options().IsTitleVisible(venues, pTitle);
	if (pTitle->IsAnyFiltered() != !bVisTitle)
	{
		bChanged = true;
		pTitle->SetFiltered(!bVisTitle);
	}
	return bChanged;
}


bool CAgilityBookDoc::ResetVisibility(
		std::set<std::wstring>& names,
		ARBTrainingPtr pTraining)
{
	bool bChanged = false;
	bool bVisTraining = CFilterOptions::Options().IsTrainingLogVisible(names, pTraining);
	if (pTraining->IsAnyFiltered() != !bVisTraining)
	{
		bChanged = true;
		pTraining->SetFiltered(!bVisTraining);
	}
	return bChanged;
}


bool CAgilityBookDoc::ShowPointsAsHtml(bool bHtml)
{
	CTabView* pView = GetTabView();
	if (pView)
		return pView->ShowPointsAsHtml(bHtml);
	return false;
}


CTabView* CAgilityBookDoc::GetTabView() const
{
	for (wxList::const_iterator iView = GetViews().begin(); iView != GetViews().end(); ++iView)
	{
		CTabView* pView = wxDynamicCast(*iView, CTabView);
		if (pView)
			return pView;
	}
	return NULL;
}


/**
 * Function to get the tree view. This is used internally and by the runs view.
 */
CAgilityBookTreeModel* CAgilityBookDoc::GetTreeModel() const
{
#pragma PRAGMA_TODO("Fix me")
#if 0
	for (wxList::const_iterator iView = GetViews().begin(); iView != GetViews().end(); ++iView)
	{
		CAgilityBookTreeView* pView = wxDynamicCast(*iView, CAgilityBookTreeView);
		if (pView)
			return pView;
	}
#endif
	return NULL;
}


/**
 * Function to get the calendar list view. This is used by the calendar view
 * and by the export wizard.
 */
CAgilityBookCalendarListView* CAgilityBookDoc::GetCalendarListView() const
{
	for (wxList::const_iterator iView = GetViews().begin(); iView != GetViews().end(); ++iView)
	{
		CAgilityBookCalendarListView* pView = wxDynamicCast(*iView, CAgilityBookCalendarListView);
		if (pView)
			return pView;
	}
	assert(0);
	return NULL;
}


/**
 * Function to get the calendar view. This is used by the calendar list view.
 */
CAgilityBookCalendarView* CAgilityBookDoc::GetCalendarView() const
{
	for (wxList::const_iterator iView = GetViews().begin(); iView != GetViews().end(); ++iView)
	{
		CAgilityBookCalendarView* pView = wxDynamicCast(*iView, CAgilityBookCalendarView);
		if (pView)
			return pView;
	}
	assert(0);
	return NULL;
}


/**
 * Function to get the trainging view. This is used by the export wizard.
 */
CAgilityBookTrainingView* CAgilityBookDoc::GetTrainingView() const
{
	for (wxList::const_iterator iView = GetViews().begin(); iView != GetViews().end(); ++iView)
	{
		CAgilityBookTrainingView* pView = wxDynamicCast(*iView, CAgilityBookTrainingView);
		if (pView)
			return pView;
	}
	assert(0);
	return NULL;
}


void CAgilityBookDoc::BackupFile(wxString const& lpszPathName)
{
	CreateBackupFile(lpszPathName, CAgilityBookOptions::GetNumBackupFiles());
}


bool CAgilityBookDoc::DeleteContents()
{
	if (!wxDocument::DeleteContents())
		return false;
	std::wstring msg(_("IDS_INDICATOR_BLANK"));
	wxGetApp().SetMessageText(msg, CFilterOptions::Options().IsFilterEnabled());
	wxGetApp().SetMessageText2(msg);
	m_Records.clear();
	return wxDocument::DeleteContents();
}


bool CAgilityBookDoc::OnNewDocument()
{
	if (!wxDocument::OnNewDocument())
		return false;
	wxConfig::Get()->Write(CFG_SETTINGS_LASTFILE, wxEmptyString);
	CConfigHandler handler;
	m_Records.Default(&handler);
	m_Records.GetConfig().GetActions().clear();
	if (0 == m_Records.GetDogs().size())
	{
// The old MFC code would prompt to create a new dog if this was a new file.
// I'm not going to implement that in wxWidgets.
//		if (AfxGetMainWnd() && IsWindow(AfxGetMainWnd()->GetSafeHwnd()))
//			AfxGetMainWnd()->PostMessage(PM_DELAY_MESSAGE, CREATE_NEWDOG);
// Instead, just create a "fake" dog.
		ARBDogPtr dog(ARBDog::New());
		dog->SetCallName(StringUtil::stringW(_("IDS_COL_DOG")));
		dog->SetDOB(ARBDate::Today());
		m_Records.GetDogs().AddDog(dog);
		Modify(false);
		// Kick the LoadData in every view
		UpdateAllViews();
		// Finally, force a status update (currently, the last loaddata is the winner)
		if (GetDocumentManager())
		{
			CAgilityBookBaseExtraView* pView = wxDynamicCast(GetDocumentManager()->GetCurrentView(), CAgilityBookBaseExtraView);
			if (pView)
				pView->UpdateMessages();
		}
	}
	return true;
}


// We override this instead of DoOpenDocument because we may need to modify
// the document.
bool CAgilityBookDoc::OnOpenDocument(const wxString& filename)
{
	if (!OnSaveModified())
		return false;

	//
	// DoOpenDocument stuff
	//

	if (!wxFile::Exists(filename))
	{
		wxConfig::Get()->Write(CFG_SETTINGS_LASTFILE, wxEmptyString);
		wxMessageBox(_("Sorry, could not open this file."),
			wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
		return false;
	}

	{
		wxBusyCursor wait;

		std::wostringstream err;
		ElementNodePtr tree(ElementNode::New());
		// Translate the XML to a tree form.
		if (!tree->LoadXML(filename, err))
		{
			wxConfig::Get()->Write(CFG_SETTINGS_LASTFILE, wxEmptyString);
			// This string is in fr/fr.po
			wxString msg = wxString::Format(_("Cannot open file '%s'."), filename.c_str());
			if (0 < err.str().length())
			{
				msg << L"\n\n" << StringUtil::stringWX(err.str());
			}
			wxMessageBox(msg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
			return false;
		}
		// Translate the tree to a class structure.
		CErrorCallback callback;
		if (!m_Records.Load(tree, callback))
		{
			wxConfig::Get()->Write(CFG_SETTINGS_LASTFILE, wxEmptyString);
			wxString msg = wxString::Format(_("Cannot open file '%s'."), filename.c_str());
			if (0 < callback.m_ErrMsg.str().length())
			{
				msg << L"\n\n" << StringUtil::stringWX(callback.m_ErrMsg.str());
			}
			wxMessageBox(msg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
			return false;
		}
		else if (0 < callback.m_ErrMsg.str().length())
		{
			wxString msg(_("IDS_NONFATAL_MSGS"));
			msg << L"\n\n" << StringUtil::stringWX(callback.m_ErrMsg.str());
			wxMessageBox(msg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
		}
		SortDates();

		ResetVisibility();

		//
		// End DoOpenDocument stuff
		//

		SetFilename(filename, true);
		Modify(false);
		m_savedYet = true;

		if (CAgilityBookOptions::AutoDeleteCalendarEntries())
		{
			ARBDate today(ARBDate::Today());
			today -= CAgilityBookOptions::DaysTillEntryIsPast();
			if (0 < m_Records.GetCalendar().TrimEntries(today))
				Modify(true);
		}

		wxConfig::Get()->Write(CFG_SETTINGS_LASTFILE, filename);
	}

	// Check our internal config.
	if (GetCurrentConfigVersion() > m_Records.GetConfig().GetVersion()
	&& m_Records.GetConfig().GetUpdateStatus())
	{
		if (CUpdateInfo::UpdateConfig(this))
		{
			if (ImportConfiguration(true))
				Modify(true);
		}
	}
	// Then check the external config.
	else
	{
		wxGetApp().AutoCheckConfiguration(this);
	}

// The old MFC code would prompt to create a new dog if this was a new file.
// I'm not going to implement that in wxWidgets.
	/*
	if (0 == m_Records.GetDogs().size() && AfxGetMainWnd() && ::IsWindow(AfxGetMainWnd()->GetSafeHwnd()))
	{
		AfxGetMainWnd()->PostMessage(PM_DELAY_MESSAGE, CREATE_NEWDOG);
	}
	*/

	// Kick the LoadData in every view
	{
		wxBusyCursor wait;
		UpdateAllViews();
	}
	// Finally, force a status update (currently, the last loaddata is the winner)
	if (GetDocumentManager())
	{
		CAgilityBookBaseExtraView* pView = wxDynamicCast(GetDocumentManager()->GetCurrentView(), CAgilityBookBaseExtraView);
		if (pView)
			pView->UpdateMessages();
	}

	return true;
}


bool CAgilityBookDoc::DoSaveDocument(const wxString& filename)
{
	wxBusyCursor wait;

	CVersionNum ver(true);
	std::wstring verstr = ver.GetVersionString();
	bool bAlreadyWarned = false;
	bool bOk = false;
	ElementNodePtr tree(ElementNode::New());
	// First, we have to push all the class data into a tree.
	if (m_Records.Save(tree, verstr, true, true, true, true, true))
	{
		BackupFile(filename);
		// Then we can stream that tree out as XML.
		if (tree->SaveXML(StringUtil::stringW(filename)))
		{
			wxConfig::Get()->Write(CFG_SETTINGS_LASTFILE, filename);
			bOk = true;
		}
		else
		{
			bAlreadyWarned = true;
			wxString errMsg = wxString::Format(_("IDS_CANNOT_OPEN"), filename.c_str());
			wxMessageBox(errMsg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
		}
	}
	if (!bOk && !bAlreadyWarned)
	{
		wxMessageBox(_("IDS_INTERNAL_ERROR"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
	}
	return bOk;
}


bool CAgilityBookDoc::OnCloseDocument()
{
	ARBDogPtr pDog = GetCurrentDog();
	if (pDog)
		wxConfig::Get()->Write(CFG_SETTINGS_LASTDOG, pDog->GetCallName().c_str());
	else
		wxConfig::Get()->Write(CFG_SETTINGS_LASTDOG, wxEmptyString);
	return wxDocument::OnCloseDocument();
}


class CFindInfo : public IFindCallback
{
public:
	CFindInfo(CAgilityBookDoc* pDoc)
		: m_pDoc(pDoc)
	{
		m_strCaption = _("IDS_SEARCH_NOTES");
		m_bEnableSearch = false;
		m_bSearchAll = true;
		m_bEnableDirection = false;
	}
	virtual bool Search(CDlgFind* pDlg) const;
	mutable std::vector<CFindItemInfo> m_Items;
private:
	CAgilityBookDoc* m_pDoc;
	void Search(
			std::wstring const& search,
			ARBInfo::eInfoType inType,
			std::set<std::wstring> const& inUse,
			ARBInfo const& info) const;
};


bool CFindInfo::Search(CDlgFind* pDlg) const
{
	m_Items.clear();
	std::wstring search = Text();
	if (!MatchCase())
		search = StringUtil::ToLower(search);
	ARBInfo& info = m_pDoc->Book().GetInfo();
	std::set<std::wstring> inUse;
	m_pDoc->Book().GetAllClubNames(inUse, false, false);
	Search(search, ARBInfo::eClubInfo, inUse, info);
	m_pDoc->Book().GetAllJudges(inUse, false, false);
	Search(search, ARBInfo::eJudgeInfo, inUse, info);
	m_pDoc->Book().GetAllTrialLocations(inUse, false, false);
	Search(search, ARBInfo::eLocationInfo, inUse, info);
	if (0 < m_Items.size())
	{
		pDlg->EndModal(wxID_OK);
		return true;
	}
	else
	{
		wxString msg = wxString::Format(_("IDS_CANNOT_FIND"), m_strSearch.c_str());
		wxMessageBox(msg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
		return false;
	}
}


void CFindInfo::Search(
		std::wstring const& search,
		ARBInfo::eInfoType inType,
		std::set<std::wstring> const& inUse,
		ARBInfo const& info) const
{
	for (std::set<std::wstring>::const_iterator iter = inUse.begin(); iter != inUse.end(); ++iter)
	{
		std::wstring str((*iter));
		if (!MatchCase())
			str = StringUtil::ToLower(str);
		if (std::wstring::npos != str.find(search))
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
		std::set<std::wstring> strings;
		if (0 < (*iterItem)->GetSearchStrings(strings))
		{
			for (std::set<std::wstring>::iterator iter = strings.begin(); iter != strings.end(); ++iter)
			{
				std::wstring str((*iter));
				if (!MatchCase())
					str = StringUtil::ToLower(str);
				if (std::wstring::npos != str.find(search))
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


void CAgilityBookDoc::OnUpdateCmd(wxUpdateUIEvent& evt)
{
	switch (evt.GetId())
	{
	case ID_FILE_EXPORT_WIZARD:
	case ID_FILE_LINKED:
		evt.Enable(true);
		break;
	case ID_COPY_TITLES_LIST:
		// As long as a dog is current, enable. This means the copy may have
		// only the dog's name.
		evt.Enable(GetCurrentDog() ? true : false);
		break;
	case ID_EDIT_CONFIGURATION:
	case ID_AGILITY_NEW_DOG:
	case ID_AGILITY_NEW_CALENDAR:
	case ID_AGILITY_UPDATE_CALENDAR:
	case ID_AGILITY_NEW_TRAINING:
	case ID_NOTES_CLUBS:
	case ID_NOTES_JUDGES:
	case ID_NOTES_LOCATIONS:
	case ID_NOTES_SEARCH:
	case wxID_PREFERENCES:
		evt.Enable(true);
		break;
	case ID_VIEW_SORTRUNS:
		evt.Check(CAgilityBookOptions::GetNewestDatesFirst() ? 1 : 0);
		evt.Enable(false);
		evt.Skip();
		break;
	case ID_VIEW_RUNS_BY_TRIAL:
		evt.Check(CAgilityBookOptions::GetViewRunsByTrial() ? 1 : 0);
		evt.Enable(false);
		evt.Skip();
		break;
	case ID_VIEW_HIDDEN:
		evt.Check(CAgilityBookOptions::GetViewHiddenTitles() ? 1 : 0);
		evt.Enable(false);
		evt.Skip();
		break;
	case ID_VIEW_TABLE_IN_YPS:
		evt.Check(CAgilityBookOptions::GetTableInYPS() ? 1 : 0);
		evt.Enable(false);
		evt.Skip();
		break;
	case ID_VIEW_RUNTIME_IN_OPS:
		evt.Check(CAgilityBookOptions::GetRunTimeInOPS() ? 1 : 0);
		evt.Enable(false);
		evt.Skip();
		break;
	case ID_VIEW_LIFETIME_EVENTS:
		evt.Check(CAgilityBookOptions::GetViewLifetimePointsByEvent() ? 1 : 0);
		evt.Enable(false);
		evt.Skip();
		break;
	case wxID_ABOUT:
	case ID_HELP_UPDATE:
		evt.Enable(true);
		break;
	}
}


void CAgilityBookDoc::OnCmd(wxCommandEvent& evt)
{
	switch (evt.GetId())
	{
	case ID_FILE_EXPORT_WIZARD:
		{
			new CWizard(this); // There's magic here.
		}
		break;

	case ID_FILE_LINKED:
		{
			CDlgFindLinks dlg(m_Records.GetDogs(), wxGetApp().GetTopWindow());
			if (0 == dlg.GetNumLinks())
				wxMessageBox(_("IDS_NO_LINKED_FILES"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
			else
				dlg.ShowModal();
		}
		break;

	case ID_COPY_TITLES_LIST:
		{
			ARBDogPtr pDog = GetCurrentDog();
			if (pDog)
			{
				std::vector<CVenueFilter> venues;
				CFilterOptions::Options().GetFilterVenue(venues);

				std::wstring preTitles, postTitles;
				for (ARBConfigVenueList::const_iterator iVenue = m_Records.GetConfig().GetVenues().begin();
					iVenue != m_Records.GetConfig().GetVenues().end();
					++iVenue)
				{
					if (!CFilterOptions::Options().IsVenueVisible(venues, (*iVenue)->GetName()))
						continue;
					std::wstring preTitles2, postTitles2;
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
										preTitles2 += L" ";
									preTitles2 += pTitle->GetGenericName();
								}
								else
								{
									if (!postTitles2.empty())
										postTitles2 += L" ";
									postTitles2 += pTitle->GetGenericName();
								}
							}
						}
					}
					if (!preTitles2.empty())
					{
						if (!preTitles.empty())
							preTitles += L" ";
						preTitles += preTitles2;
					}
					if (!postTitles2.empty())
					{
						if (!postTitles.empty())
							postTitles += L"; ";
						postTitles += postTitles2;
					}
				}

				std::wstring data(preTitles);
				if (!data.empty())
					data += L" ";
				data += pDog->GetCallName();
				if (!postTitles.empty())
				{
					data += L": ";
					data += postTitles;
				}
				CClipboardDataWriter clpData;
				if (clpData.isOkay())
				{
					clpData.AddData(data);
					clpData.CommitData();
				}
			}
		}
		break;

	case ID_EDIT_CONFIGURATION:
		{
			CDlgConfigure config(this, m_Records, wxGetApp().GetTopWindow());
			config.ShowModal();
			// Don't need to update calsite info - done during OnOK.
		}
		break;

	case ID_AGILITY_NEW_DOG:
		{
			ARBDogPtr dog(ARBDog::New());
			CDlgDog dlg(this, dog, wxGetApp().GetTopWindow());
			if (wxID_OK == dlg.ShowModal())
			{
				CTabView* pTab = GetTabView();
				if (pTab)
					pTab->SetCurTab(IDX_PANE_RUNS);
				if (m_Records.GetDogs().AddDog(dog))
				{
#pragma PRAGMA_TODO("Fix me")
#if 0
					CAgilityBookTreeView* pTree = GetTreeView();
					if (pTree)
						pTree->InsertDog(dog, true);
#endif
				}
			}
		}
		break;

	case ID_AGILITY_NEW_CALENDAR:
		{
			ARBCalendarPtr cal(ARBCalendar::New());
			CDlgCalendar dlg(cal, this, wxGetApp().GetTopWindow());
			if (wxID_OK == dlg.ShowModal())
			{
				if (!(CAgilityBookOptions::AutoDeleteCalendarEntries() && cal->GetEndDate() < ARBDate::Today()))
				{
					CTabView* pTab = GetTabView();
					if (pTab)
						pTab->SetCurTab(IDX_PANE_CALENDAR);
					m_Records.GetCalendar().AddCalendar(cal);
					m_Records.GetCalendar().sort();
					Modify(true);
					CUpdateHint hint(UPDATE_CALENDAR_VIEW);
					UpdateAllViews(NULL, &hint);
					CAgilityBookCalendarView* pCal = GetCalendarView();
					if (pCal)
						pCal->SetCurrentDate(cal->GetStartDate(), true);
				}
			}
		}
		break;

	case ID_AGILITY_UPDATE_CALENDAR:
		m_CalSites.FindEntries(this, m_Records.GetCalendar());
		break;

	case ID_AGILITY_NEW_TRAINING:
		{
			ARBTrainingPtr training(ARBTraining::New());
			CDlgTraining dlg(training, this, wxGetApp().GetTopWindow());
			if (wxID_OK == dlg.ShowModal())
			{
				CTabView* pTab = GetTabView();
				if (pTab)
					pTab->SetCurTab(IDX_PANE_LOG);
				m_Records.GetTraining().AddTraining(training);
				m_Records.GetTraining().sort();
				Modify(true);
				CUpdateHint hint(UPDATE_TRAINING_VIEW);
				UpdateAllViews(NULL, &hint);
				CAgilityBookTrainingView* pView = GetTrainingView();
				if (pView)
					pView->SetCurrentDate(training->GetDate());
			}
		}
		break;

	case ID_NOTES_CLUBS:
		{
			std::wstring select;
			ARBDogTrialPtr pTrial = GetCurrentTrial();
			if (pTrial)
				select = pTrial->GetClubs().GetPrimaryClubName();
			CDlgInfoNote dlg(this, ARBInfo::eClubInfo, select, wxGetApp().GetTopWindow());
			dlg.ShowModal();
		}
		break;

	case ID_NOTES_JUDGES:
		{
			std::wstring select;
			ARBDogRunPtr pRun = GetCurrentRun();
			if (pRun)
				select = pRun->GetJudge();
			CDlgInfoNote dlg(this, ARBInfo::eJudgeInfo, select, wxGetApp().GetTopWindow());
			dlg.ShowModal();
		}
		break;

	case ID_NOTES_LOCATIONS:
		{
			std::wstring select;
			ARBDogTrialPtr pTrial = GetCurrentTrial();
			if (pTrial)
				select = pTrial->GetLocation();
			CDlgInfoNote dlg(this, ARBInfo::eLocationInfo, select, wxGetApp().GetTopWindow());
			dlg.ShowModal();
		}
		break;

	case ID_NOTES_SEARCH:
		{
			CFindInfo callback(this);
			CDlgFind dlg(callback, wxGetApp().GetTopWindow());
			if (wxID_OK == dlg.ShowModal())
			{
				CDlgListViewer dlgList(this, StringUtil::stringW(_("IDS_COL_NOTES")), callback.m_Items, wxGetApp().GetTopWindow());
				dlgList.ShowModal();
			}
		}
		break;

	case wxID_PREFERENCES:
		{
			int nPage;
			CTabView* pTab = GetTabView();
			if (!pTab)
				return;
			switch (pTab->GetCurTab())
			{
			default:
				nPage = CDlgOptions::GetProgramPage();
				break;
			case IDX_PANE_RUNS:
			case IDX_PANE_POINTS:
				nPage = CDlgOptions::GetFilterPage();
				break;
			case IDX_PANE_CALENDAR:
				nPage = CDlgOptions::GetCalendarPage();
				break;
			case IDX_PANE_LOG:
				nPage = CDlgOptions::GetFilterPage();
				break;
			}
			CDlgOptions options(this, wxGetApp().GetTopWindow(), nPage);
			options.ShowModal();
		}
		break;

	case wxID_ABOUT:
		{
			CDlgAbout dlg(this, wxGetApp().GetTopWindow());
			dlg.ShowModal();
		}
		break;

	case ID_HELP_UPDATE:
		{
			bool close = false;
			wxGetApp().UpdateConfiguration(this, close);
			if (close)
			{
				wxGetApp().GetTopWindow()->Close(true);
			}
		}
		break;
	}
}
