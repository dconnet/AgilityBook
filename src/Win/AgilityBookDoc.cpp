/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CAgilityBookDoc class
 * @author David Connet
 *
 * Revision History
 * 2019-12-26 Fixed file size in properties for new file.
 * 2018-12-16 Convert to fmt.
 * 2018-09-15 Refactored how tree/list handle common actions.
 * 2015-12-31 Add wx version to file-written-on section in File Properties.
 * 2015-11-25 Oops, Mac doesn't have '__super'.
 * 2015-10-29 Add Save override. Check if file was externally modified.
 * 2014-12-04 If all dogs are deceased, don't prompt to update config.
 * 2014-04-25 Don't prompt to update config if file is readonly.
 * 2013-01-11 Reset filters on configuration import.
 * 2012-09-29 Strip the Runs View.
 * 2012-07-11 After importing an ARB file, sort it.
 * 2012-07-04 Add option to use run time or opening time in gamble OPS.
 * 2012-03-16 Renamed LoadXML functions, added stream version.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-08-25 Refresh properly after changing filter in context menu.
 * 2009-08-19 Create a 'fake' dog when creating a new file.
 * 2009-02-05 Ported to wxWidgets.
 * 2008-11-19 Added context menus to status bar.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2006-02-08 Added 'RenameEvent' action.
 * 2005-12-14 Moved 'Titles' to 'Venue'.
 * 2005-10-26 Added option to prevent auto-update user query.
 * 2005-10-19 Fixed a problem with CFile::GetStatus (see AgilityBook.cpp).
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-12-19 Changed sort-newest to only do trials, not runs.
 * 2004-12-18 Added an extra check before posting the new dog msg.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-07-23 Auto-check the config version on document open.
 * 2004-07-20 Moved the user-request updates here so it can check if
 *            a new configuration is available.
 * 2004-06-29 Set filtering on runs that are in hidden trials.
 * 2004-04-29 Use default config during auto-update (no file prompt).
 * 2004-03-31 Only prompt to merge config if config version number is
 *            greater (was simply checking for not-equal)
 * 2004-03-26 Added code to migrate runs to the new table-in-run form.
 *            Added menu handlers for 'Show Hidden Titles' (oops)
 * 2004-02-26 Moved config update here, test doc for current config.
 * 2004-01-26 Display errors on non-fatal load.
 * 2003-12-10 Moved import/export into a wizard.
 * 2003-12-07 Changed Load/Save api to support new info section.
 * 2003-10-31 Added import/export calendar, export config.
 * 2003-10-22 Added export dtd/xml menu options.
 * 2003-10-09 Added option to not filter runs by selected trial.
 * 2003-09-15 Fixed a bug where a trial created for more than one dog
 *            at the same time actually only created one linked entry.
 * 2003-08-27 Added view accessors for calendar, made them public so
 *            I don't have to use UpdateAllViews. Added methods to allow
 *            creating titles/trials/runs from the Run view.
 * 2003-08-25 Added GetCurrentRun().
 * 2003-08-24 Optimized filtering by adding boolean into ARBBase to
 *            prevent constant re-evaluation.
 */

#include "stdafx.h"
#include "AgilityBookDoc.h"

#include "AgilityBook.h"
#include "AgilityBookCalendarListView.h"
#include "AgilityBookCalendarView.h"
#include "AgilityBookMenu.h"
#include "AgilityBookOptions.h"
#include "AgilityBookRunsView.h"
#include "AgilityBookTrainingView.h"
#include "AgilityBookTreeData.h"
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
#include "DlgRun.h"
#include "DlgSelectDog.h"
#include "DlgTitle.h"
#include "DlgTraining.h"
#include "DlgTrial.h"
#include "FilterOptions.h"
#include "MainFrm.h"
#include "RegItems.h"
#include "TabView.h"
#include "UpdateInfo.h"
#include "VersionNumber.h"
#include "Wizard.h"

#include "ARBCommon/ARBMsgDigest.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/VersionNum.h"
#include <algorithm>
#include <wx/config.h>
#include <wx/file.h>
#include <wx/filefn.h>
#include <wx/wfstream.h>

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
		ARBDogPtr const& inDog,
		ARBDogTitlePtr const& inTitle)
{
	// Find all trials in the future in the same venue.
	std::vector<ARBDogTrialPtr> trials;
	for (ARBDogTrialList::iterator i = inDog->GetTrials().begin();
		i != inDog->GetTrials().end();
		++i)
	{
		if ((*i)->HasVenue(inTitle->GetVenue())
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

wxBEGIN_EVENT_TABLE(CAgilityBookDoc, wxDocument)
	EVT_UPDATE_UI(ID_FILE_EXPORT_WIZARD, CAgilityBookDoc::OnUpdateCmdTrue)
	EVT_MENU(ID_FILE_EXPORT_WIZARD, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_FILE_LINKED, CAgilityBookDoc::OnUpdateCmdTrue)
	EVT_MENU(ID_FILE_LINKED, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_FILE_PROPERTIES, CAgilityBookDoc::OnUpdateFileProperties)
	EVT_MENU(ID_FILE_PROPERTIES, CAgilityBookDoc::OnFileProperties)
	EVT_UPDATE_UI(ID_COPY_TITLES_LIST, CAgilityBookDoc::OnUpdateCmd)
	EVT_MENU(ID_COPY_TITLES_LIST, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_EDIT_CONFIGURATION, CAgilityBookDoc::OnUpdateCmdTrue)
	EVT_MENU(ID_EDIT_CONFIGURATION, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_AGILITY_NEW_DOG, CAgilityBookDoc::OnUpdateCmdTrue)
	EVT_MENU(ID_AGILITY_NEW_DOG, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_AGILITY_NEW_CALENDAR, CAgilityBookDoc::OnUpdateCmdTrue)
	EVT_MENU(ID_AGILITY_NEW_CALENDAR, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_AGILITY_UPDATE_CALENDAR, CAgilityBookDoc::OnUpdateCmdTrue)
	EVT_MENU(ID_AGILITY_UPDATE_CALENDAR, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_AGILITY_NEW_TRAINING, CAgilityBookDoc::OnUpdateCmdTrue)
	EVT_MENU(ID_AGILITY_NEW_TRAINING, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_NOTES_CLUBS, CAgilityBookDoc::OnUpdateCmdTrue)
	EVT_MENU(ID_NOTES_CLUBS, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_NOTES_JUDGES, CAgilityBookDoc::OnUpdateCmdTrue)
	EVT_MENU(ID_NOTES_JUDGES, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_NOTES_LOCATIONS, CAgilityBookDoc::OnUpdateCmdTrue)
	EVT_MENU(ID_NOTES_LOCATIONS, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_NOTES_SEARCH, CAgilityBookDoc::OnUpdateCmdTrue)
	EVT_MENU(ID_NOTES_SEARCH, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(wxID_PREFERENCES, CAgilityBookDoc::OnUpdateCmdTrue)
	EVT_MENU(wxID_PREFERENCES, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_UNSORT, CAgilityBookDoc::OnUpdateCmd)
	EVT_UPDATE_UI(ID_VIEW_SORTRUNS, CAgilityBookDoc::OnUpdateCmd)
	EVT_UPDATE_UI(ID_VIEW_RUNS_BY_TRIAL, CAgilityBookDoc::OnUpdateCmd)
	EVT_UPDATE_UI(ID_VIEW_HIDDEN, CAgilityBookDoc::OnUpdateCmd)
	EVT_UPDATE_UI(ID_VIEW_TABLE_IN_YPS, CAgilityBookDoc::OnUpdateCmd)
	EVT_UPDATE_UI(ID_VIEW_RUNTIME_IN_OPS, CAgilityBookDoc::OnUpdateCmd)
	EVT_UPDATE_UI(ID_VIEW_LIFETIME_EVENTS, CAgilityBookDoc::OnUpdateCmd)
	EVT_UPDATE_UI(wxID_ABOUT, CAgilityBookDoc::OnUpdateCmdTrue)
	EVT_MENU(wxID_ABOUT, CAgilityBookDoc::OnCmd)
	EVT_UPDATE_UI(ID_HELP_UPDATE, CAgilityBookDoc::OnUpdateCmdTrue)
	EVT_MENU(ID_HELP_UPDATE, CAgilityBookDoc::OnCmd)
#ifdef _DEBUG
	EVT_MENU(ID_HELP_DEBUG, CAgilityBookDoc::OnHelpDebug)
#endif
wxEND_EVENT_TABLE()


CAgilityBookDoc::CAgilityBookDoc()
	: m_Records()
	, m_CalSites()
	, m_StatusData(nullptr)
	, m_pCurrentDog()
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
		CAgilityBookTreeView* pTree = GetTreeView();
		if (pTree)
			pTree->SelectDog(m_StatusData->dogs[evt.GetId()-baseID]);
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
			UpdateAllViews(nullptr, &hint);
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
			if (GetTreeView() && 1 < m_Records.GetDogs().size())
			{
				ARBDogPtr curDog = GetCurrentDog();
				wxMenu menu;
				int menuId = baseID;
				CStatusHandler data;
				ARBDogList::const_iterator iDog;
				for (iDog = m_Records.GetDogs().begin(); iDog != m_Records.GetDogs().end(); ++iDog, ++menuId)
				{
					parent->Bind(wxEVT_COMMAND_MENU_SELECTED, &CAgilityBookDoc::OnStatusDog, this, menuId);
					std::wstring item = StringUtil::Replace((*iDog)->GetGenericName(), L"&", L"&&");
					wxMenuItem* menuitem = menu.AppendCheckItem(menuId, StringUtil::stringWX(item));
					if (curDog && *(*iDog) == *curDog)
						menuitem->Check(true);
					data.dogs.push_back(*iDog);
				}
				bHandled = true;
				m_StatusData = &data;
				parent->PopupMenu(&menu, point);
				menuId = baseID;
				for (iDog = m_Records.GetDogs().begin(); iDog != m_Records.GetDogs().end(); ++iDog, ++menuId)
				{
					parent->Unbind(wxEVT_COMMAND_MENU_SELECTED, &CAgilityBookDoc::OnStatusDog, this, menuId);
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
					wxMenu menu;
					int menuId = baseID;
					std::vector<std::wstring>::const_iterator iFilter;
					for (iFilter = data.filterNames.begin();
						iFilter != data.filterNames.end();
						++iFilter, ++menuId)
					{
						parent->Bind(wxEVT_COMMAND_MENU_SELECTED, &CAgilityBookDoc::OnStatusFilter, this, menuId);
						std::wstring item = StringUtil::Replace(*iFilter, L"&", L"&&");
						wxMenuItem* menuitem = menu.AppendCheckItem(menuId, StringUtil::stringWX(item));
						if (*iFilter == filterName)
							menuitem->Check(true);
					}
					bHandled = true;
					m_StatusData = &data;
					parent->PopupMenu(&menu, point);
					menuId = baseID;
					for (iFilter = data.filterNames.begin();
						iFilter != data.filterNames.end();
						++iFilter, ++menuId)
					{
						parent->Unbind(wxEVT_COMMAND_MENU_SELECTED, &CAgilityBookDoc::OnStatusFilter, this, menuId);
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
	return m_pCurrentDog;
}


void CAgilityBookDoc::SetCurrentDog(ARBDogPtr const& inDog, bool bSuppressHints)
{
	if (m_pCurrentDog != inDog)
	{
		m_pCurrentDog = inDog;

		if (!bSuppressHints)
		{
			CUpdateHint hint(UPDATE_POINTS_VIEW);
			UpdateAllViews(nullptr, &hint);
		}
	}
}


/**
 * Return the trial associated with the currently selected item in the tree.
 */
ARBDogTrialPtr CAgilityBookDoc::GetCurrentTrial() const
{
	ARBDogTrialPtr pTrial;
	CAgilityBookTreeView* pTree = GetTreeView();
	assert(pTree);
	if (pTree && pTree->GetCurrentTreeItem())
		pTrial = pTree->GetCurrentTreeItem()->GetTrial();
	return pTrial;
}


/**
 * Return the run associated with the currently selected item in the tree.
 */
ARBDogRunPtr CAgilityBookDoc::GetCurrentRun() const
{
	ARBDogRunPtr pRun;
	CAgilityBookTreeView* pTree = GetTreeView();
	assert(pTree);
	if (pTree && pTree->GetCurrentTreeItem())
		pRun = pTree->GetCurrentTreeItem()->GetRun();
	return pRun;
}


bool CAgilityBookDoc::EditDog(ARBDogPtr const& inDog, int nPage)
{
	bool bAdd = false;
	ARBDogPtr pDog = inDog;
	if (!pDog)
	{
		bAdd = true;
		pDog = ARBDogPtr(ARBDog::New());
	}
	bool bOk = false;
	CDlgDog dlg(this, pDog, wxGetApp().GetTopWindow(), nPage);
	if (wxID_OK == dlg.ShowModal())
	{
		bOk = true;
		if (bAdd)
		{
			if (!Book().GetDogs().AddDog(pDog))
			{
				bOk = false;
			}
			else
			{
				CAgilityBookTreeView* pTree = GetTreeView();
				assert(pTree);
				wxTreeItemId hItem = pTree->InsertDog(pDog);
				pTree->SelectItem(hItem);
			}
		}
		// No need to refresh item here - CDlgDog::OnOk will call the document
		// ResetVisibility which causes a tree load. Note: If the dog dlg gets
		// more intelligent (reset is only needed if changing titles/etc), then
		// we may need this. The problem is that pDogData may be deleted.
		// Need a way to track that pDogData is gone...
		//else
		//	pTree->RefreshItem(pDogData->GetId());
	}
	return bOk;
}


bool CAgilityBookDoc::AddTitle(ARBDogPtr const& inDog)
{
	if (inDog)
	{
		CDlgTitle dlgTitle(Book().GetConfig(), inDog->GetTitles(), ARBDogTitlePtr());
		if (wxID_OK == dlgTitle.ShowModal())
		{
			std::vector<CVenueFilter> venues;
			CFilterOptions::Options().GetFilterVenue(venues);
			ResetVisibility(venues, dlgTitle.GetNewTitle());
			UpdateFutureTrials(Book().GetConfig(), inDog, dlgTitle.GetNewTitle());
			CUpdateHint hint(UPDATE_POINTS_VIEW);
			UpdateAllViews(nullptr, &hint);
			Modify(true);
			if (CAgilityBookOptions::AutoShowPropertiesOnNewTitle())
			{
				CDlgDog dlgDog(this, inDog, wxGetApp().GetTopWindow(), 1);
				dlgDog.ShowModal();
			}
			return true;
		}
	}
	return false;
}


bool CAgilityBookDoc::EditTrial(ARBDogPtr const& inDog, ARBDogTrialPtr const& inTrial)
{
	assert(inDog);
	bool bAdd = false;
	ARBDogTrialPtr pTrial = inTrial;
	if (!pTrial)
	{
		bAdd = true;
		pTrial = ARBDogTrialPtr(ARBDogTrial::New());
	}
	bool bOk = false;
	CDlgTrial dlg(this, pTrial, wxGetApp().GetTopWindow());
	if (wxID_OK == dlg.ShowModal())
	{
		CAgilityBookTreeView* pTree = GetTreeView();
		assert(pTree);
		bOk = true;
		std::vector<CVenueFilter> venues;
		CFilterOptions::Options().GetFilterVenue(venues);
		if (bAdd)
		{
			if (!inDog->GetTrials().AddTrial(pTrial, !CAgilityBookOptions::GetNewestDatesFirst()))
			{
				bOk = false;
				wxMessageBox(_("IDS_CREATETRIAL_FAILED"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
			}
			else
			{
				ResetVisibility(venues, pTrial);
				// Even though we will reset the tree, go ahead and add/select
				// the item into the tree here. That will make sure when the
				// tree is reloaded, that the new item is selected.
				CAgilityBookTreeData* pDogData = pTree->FindData(inDog);
				assert(pDogData);
				wxTreeItemId hItem = pTree->InsertTrial(pTrial, pDogData->GetId());
				if (!hItem.IsOk())
				{
					wxMessageBox(_("IDS_CREATETRIAL_FILTERED"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
				}
				else
				{
					pTree->SelectItem(hItem);
					//if (bTreeSelectionSet)
					//	*bTreeSelectionSet = true;
				}
			}
		}
		else
		{
			inDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
			ResetVisibility(venues, pTrial);
			CAgilityBookTreeData* pTrialData = pTree->FindData(pTrial);
			if (pTrialData)
				pTree->RefreshItem(pTrialData->GetId());
			//if (dlg.RunsWereDeleted())
			//{
			//	if (bTreeSelectionSet)
			//		*bTreeSelectionSet = true;
			//}
		}
		// We have to update the tree even when we add above as it may have
		// caused the trial to be reordered.
		if (bOk)
		{
			CUpdateHint hint(UPDATE_POINTS_VIEW | UPDATE_RUNS_VIEW | UPDATE_TREE_VIEW);
			UpdateAllViews(nullptr, &hint);
		}
	}
	return bOk;
}


bool CAgilityBookDoc::DeleteTrial(ARBDogPtr const& inDog, ARBDogTrialPtr const& inTrial, bool bSilent)
{
	if (!inDog || !inTrial)
		return false;

	bool bDeleted = false;
	if (bSilent
	|| wxYES == wxMessageBox(_("IDS_DELETE_TRIAL_DATA"), wxMessageBoxCaptionStr, wxYES_NO | wxNO_DEFAULT | wxCENTRE | wxICON_QUESTION))
	{
		if (inDog->GetTrials().DeleteTrial(inTrial))
		{
			// Note: Do not send hint here. The caller must sent it.
			// (the tree needs to delete the item before sending the hint)
			Modify(true);
			bDeleted = true;
		}
	}
	return bDeleted;
}


bool CAgilityBookDoc::EditRun(ARBDogPtr const& inDog, ARBDogTrialPtr const& inTrial, ARBDogRunPtr const& inRun)
{
	assert(inDog);
	assert(inTrial);
	CAgilityBookTreeView* pTree = GetTreeView();
	assert(pTree);
	bool bOk = false;
	bool bAdd = false;
	ARBDogRunPtr pRun = inRun;
	if (!pRun)
	{
		if (0 == inTrial->GetClubs().size())
		{
			wxMessageBox(_("IDS_NEED_CLUB"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_WARNING);
			return false;
		}
		for (auto pClub : inTrial->GetClubs())
		{
			if (!Book().GetConfig().GetVenues().FindVenue(pClub->GetVenue()))
			{
				wxMessageBox(fmt::format(_("IDS_VENUE_CONFIG_MISSING").wx_str(), pClub->GetVenue()),
					wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
				return false;
			}
		}
		bAdd = true;
		pRun = ARBDogRunPtr(ARBDogRun::New());
		ARBDate date = inTrial->GetEndDate();
		// If this is the first run, the date won't be set.
		if (!date.IsValid())
			date.SetToday();
		pRun->SetDate(date);
	}
	CDlgRun dlg(this, inDog, inTrial, pRun);
	if (wxID_OK == dlg.ShowModal())
	{
		bOk = true;
		std::vector<CVenueFilter> venues;
		CFilterOptions::Options().GetFilterVenue(venues);
		if (bAdd)
		{
			if (!inTrial->GetRuns().AddRun(pRun))
			{
				bOk = false;
				wxMessageBox(_("IDS_CREATERUN_FAILED"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
			}
			else
			{
				// When adding a new trial, we need to reset the multiQs.
				// The edit dialog does it in OnOK, but we don't add the run
				// until after the dialog is done.
				inTrial->SetMultiQs(Book().GetConfig());
				inTrial->GetRuns().sort();
				inDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
				ResetVisibility(venues, inTrial, pRun);
				// Even though we will reset the tree, go ahead and add/select
				// the item into the tree here. That will make sure when the
				// tree is reloaded, that the new item is selected.
				CAgilityBookTreeData* pTrialData = pTree->FindData(inTrial);
				wxTreeItemId hItem = pTree->InsertRun(inTrial, pRun, pTrialData->GetId());
				if (!hItem.IsOk())
				{
					if (CFilterOptions::Options().IsFilterEnabled())
						wxMessageBox(_("IDS_CREATERUN_FILTERED"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
				}
				else
				{
					pTree->SelectItem(hItem);
					//if (bTreeSelectionSet)
					//	*bTreeSelectionSet = true;
				}
			}
		}
		else
		{
			inDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
			ResetVisibility(venues, inTrial, pRun);
		}
		// We have to update the tree even when we add above as it may have
		// caused the trial to be reordered.
		if (bOk)
		{
			CUpdateHint hint(UPDATE_POINTS_VIEW | UPDATE_RUNS_VIEW | UPDATE_TREE_VIEW);
			UpdateAllViews(nullptr, &hint);
		}
	}
	return bOk;
}


bool CAgilityBookDoc::DeleteRuns(std::vector<ARBDogRunPtr> const& inRuns, bool bSilent)
{
	if (inRuns.empty())
		return false;

	CAgilityBookTreeView* pTree = GetTreeView();
	assert(pTree);

	bool bUpdate = false;
	unsigned int updateHint = UPDATE_POINTS_VIEW | UPDATE_RUNS_VIEW;
	std::set<ARBDogPtr> sortTrials;
	std::set<wxTreeItemId> refreshItems;

	for (auto inRun : inRuns)
	{
		CAgilityBookTreeData* pData = pTree->FindData(inRun);
		if (!pData)
			continue;
		if (bSilent
		|| wxYES == wxMessageBox(_("IDS_DELETE_EVENT_DATA"), wxMessageBoxCaptionStr, wxYES_NO | wxNO_DEFAULT | wxCENTRE | wxICON_QUESTION))
		{
			ARBDogTrialPtr pTrial = pData->GetTrial();
			ARBDate startDate = pTrial->GetStartDate();
			if (pTrial->GetRuns().DeleteRun(pData->GetRun()))
			{
				bUpdate = true;
				refreshItems.insert(pTree->GetItemParent(pData->GetId()));
				ARBDogPtr pDog = pData->GetDog();
				// Delete() will cause 'pData' to be deleted.
				pTree->Delete(pData->GetId());
				pData = nullptr;
				pTrial->SetMultiQs(Book().GetConfig());
				if (pTrial->GetStartDate() != startDate)
				{
					updateHint |= UPDATE_TREE_VIEW;
					sortTrials.insert(pDog);
				}
				Modify(true);
			}
			bSilent = true; // Only prompt on the first run
		}
	}

	if (bUpdate)
	{
		for (auto id : refreshItems)
			pTree->RefreshItem(id);

		for (auto pDog : sortTrials)
			pDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());

		CUpdateHint hint(updateHint);
		UpdateAllViews(nullptr, &hint);
	}

	return bUpdate;
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
				pDog->GetTrials().AddTrial(pNewTrial, !CAgilityBookOptions::GetNewestDatesFirst());
				Modify(true);
				CUpdateHint hint(UPDATE_NEW_TRIAL, pNewTrial);
				UpdateAllViews(nullptr, &hint);
			}
			pTabView->SetCurTab(0);
		}
	}
	return bCreated;
}


bool CAgilityBookDoc::SetCalenderDate(ARBDate const& date)
{
	CAgilityBookCalendarView* pCalView = GetCalendarView();
	return pCalView->SetCurrentDate(date, true);
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
	void PreDelete(std::wstring const& inMsg) override;
	void PostDelete(std::wstring const& inMsg) const override;
};


void CConfigActionCallback::PreDelete(std::wstring const& inMsg)
{
	wxString msg(StringUtil::stringWX(inMsg));
	msg += L"\n\n";
	msg += _("IDS_ARE_YOU_SURE_CONTINUE");
	if (wxNO == wxMessageBox(msg, wxMessageBoxCaptionStr, wxYES_NO | wxCENTRE | wxICON_WARNING))
	{
		m_bContinue = false;
	}
}


void CConfigActionCallback::PostDelete(std::wstring const& msg) const
{
	wxMessageBox(StringUtil::stringWX(msg), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_WARNING);
}


bool CAgilityBookDoc::ImportConfiguration(ARBConfig& update)
{
	unsigned int iHint = 0;
	fmt::wmemory_buffer info;
	CConfigActionCallback callback;
	bool bChanges = false;
	{
		wxBusyCursor wait;
		short configVersionPreUpdate = m_Records.GetConfig().GetVersion();
		bChanges = m_Records.Update(0, update, info, callback);

		if (bChanges)
		{
			CFilterOptions filterOptions;
			if (filterOptions.Update(update, configVersionPreUpdate, m_Records.GetConfig()) && ResetVisibility())
			{
				iHint |= UPDATE_ALL_VIEW;
			}
		}
	}
	if (bChanges)
	{
		CAgilityBookOptions::CleanLastItems(m_Records.GetConfig());
		CDlgMessage dlg(fmt::to_string(info), wxGetApp().GetTopWindow());
		dlg.ShowModal();
		Modify(true);
		CUpdateHint hint(UPDATE_CONFIG | iHint);
		UpdateAllViews(nullptr, &hint);
	}
	else if (callback.CanContinue())
		wxMessageBox(_("IDS_CONFIG_NO_UPDATE"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
	return bChanges;
}


bool CAgilityBookDoc::ImportConfiguration(bool bUseDefault)
{
	bool bChanges = false;
	bool bDoIt = false;
	CDlgConfigUpdate dlg(wxGetApp().GetTopWindow());
	if (bUseDefault)
	{
		bDoIt = true;
		dlg.LoadConfig(nullptr);
	}
	else
	{
		if (wxID_OK == dlg.ShowModal())
			bDoIt = true;
	}
	if (bDoIt)
	{
		ARBConfig& update = dlg.GetConfig();
		bChanges = ImportConfiguration(update);
	}
	return bChanges;
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


bool CAgilityBookDoc::ImportARBRunData(ElementNodePtr const& inTree, wxWindow* pParent)
{
	wxBusyCursor wait;
	bool bOk = false;
	CErrorCallback err;
	ARBAgilityRecordBook book;
	if (book.Load(inTree, false, false, true, true, true, err))
	{
		if (0 < err.m_ErrMsg.size())
			wxMessageBox(StringUtil::stringWX(fmt::to_string(err.m_ErrMsg)), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
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

		for (ARBInfoItemList::const_iterator iterClub = book.GetInfo().GetInfo(ARBInfoType::Club).begin();
			iterClub != book.GetInfo().GetInfo(ARBInfoType::Club).end();
			++iterClub)
		{
			ARBInfoItemPtr pClub = *iterClub;
			// If this fails, it already exists.
			if (m_Records.GetInfo().GetInfo(ARBInfoType::Club).AddItem(pClub))
			{
				++countClubs;
			}
		}
		for (ARBInfoItemList::const_iterator iterJudge = book.GetInfo().GetInfo(ARBInfoType::Judge).begin();
			iterJudge != book.GetInfo().GetInfo(ARBInfoType::Judge).end();
			++iterJudge)
		{
			ARBInfoItemPtr pJudge = *iterJudge;
			// If this fails, it already exists.
			if (m_Records.GetInfo().GetInfo(ARBInfoType::Judge).AddItem(pJudge))
			{
				++countJudges;
			}
		}
		for (ARBInfoItemList::const_iterator iterLocation = book.GetInfo().GetInfo(ARBInfoType::Location).begin();
			iterLocation != book.GetInfo().GetInfo(ARBInfoType::Location).end();
			++iterLocation)
		{
			ARBInfoItemPtr pLocation = *iterLocation;
			// If this fails, it already exists.
			if (m_Records.GetInfo().GetInfo(ARBInfoType::Location).AddItem(pLocation))
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
			UpdateAllViews(nullptr, &hint);
			Modify(true);
		}
		if (0 < countClubs)
		{
			std::set<std::wstring> namesInUse;
			m_Records.GetAllClubNames(namesInUse, false, false);
			m_Records.GetInfo().GetInfo(ARBInfoType::Club).CondenseContent(namesInUse);
		}
		if (0 < countJudges)
		{
			std::set<std::wstring> namesInUse;
			m_Records.GetAllJudges(namesInUse, false, false);
			m_Records.GetInfo().GetInfo(ARBInfoType::Judge).CondenseContent(namesInUse);
		}
		if (0 < countLocations)
		{
			std::set<std::wstring> namesInUse;
			m_Records.GetAllTrialLocations(namesInUse, false, false);
			m_Records.GetInfo().GetInfo(ARBInfoType::Location).CondenseContent(namesInUse);
		}
		fmt::wmemory_buffer str;
		fmt::format_to(str, L"{}", _("IDS_ADDED").wx_str());
		bool bAdded = false;
		if (0 < countDog)
		{
			if (bAdded)
				fmt::format_to(str, L", ");
			bAdded = true;
			fmt::format_to(str, _("IDS_ADDED_DOGS").wx_str(), countDog);
		}
		if (0 < countRegNumsAdded)
		{
			if (bAdded)
				fmt::format_to(str, L", ");
			bAdded = true;
			fmt::format_to(str, _("IDS_ADDED_REGNUMS").wx_str(), countRegNumsAdded);
		}
		if (0 < countExistingPts)
		{
			if (bAdded)
				fmt::format_to(str, L", ");
			bAdded = true;
			fmt::format_to(str, _("IDS_ADDED_EXISTINGPTS").wx_str(), countExistingPts);
		}
		if (0 < countTitlesAdded)
		{
			if (bAdded)
				fmt::format_to(str, L", ");
			bAdded = true;
			fmt::format_to(str, _("IDS_ADDED_TITLES").wx_str(), countTitlesAdded);
		}
		if (0 < countTrials)
		{
			if (bAdded)
				fmt::format_to(str, L", ");
			bAdded = true;
			fmt::format_to(str, _("IDS_ADDED_TRIALS").wx_str(), countTrials);
		}
		if (0 < countClubs)
		{
			if (bAdded)
				fmt::format_to(str, L", ");
			bAdded = true;
			fmt::format_to(str, _("IDS_ADDED_CLUBS").wx_str(), countClubs);
		}
		if (0 < countJudges)
		{
			if (bAdded)
				fmt::format_to(str, L", ");
			bAdded = true;
			fmt::format_to(str, _("IDS_ADDED_JUDGES").wx_str(), countJudges);
		}
		if (0 < countLocations)
		{
			if (bAdded)
				fmt::format_to(str, L", ");
			bAdded = true;
			fmt::format_to(str, _("IDS_ADDED_LOCATIONS").wx_str(), countLocations);
		}
		bAdded = false;
		if (0 < countRegNumsUpdated)
		{
			if (bAdded)
				fmt::format_to(str, L", ");
			else
				fmt::format_to(str, L"\n{}", _("IDS_UPDATED").wx_str());
			bAdded = true;
			fmt::format_to(str, _("IDS_ADDED_REGNUMS").wx_str(), countRegNumsUpdated);
		}
		if (0 < countTitlesUpdated)
		{
			if (bAdded)
				fmt::format_to(str, L", ");
			else
				fmt::format_to(str, L"\n{}", _("IDS_UPDATED").wx_str());
			bAdded = true;
			fmt::format_to(str, _("IDS_ADDED_TITLES").wx_str(), countTitlesUpdated);
		}
		wxMessageBox(fmt::to_string(str), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
		bOk = true;
	}
	else if (0 < err.m_ErrMsg.size())
		wxMessageBox(StringUtil::stringWX(fmt::to_string(err.m_ErrMsg)), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_WARNING);
	return bOk;
}


bool CAgilityBookDoc::ImportARBCalData(ElementNodePtr const& inTree, wxWindow* pParent)
{
	wxBusyCursor wait;
	bool bOk = false;
	CErrorCallback err;
	ARBAgilityRecordBook book;
	if (book.Load(inTree, true, false, false, false, false, err))
	{
		if (0 < err.m_ErrMsg.size())
			wxMessageBox(StringUtil::stringWX(fmt::to_string(err.m_ErrMsg)), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);

		long nAdded = 0;
		long nUpdated = 0;
		long nDuplicate = 0;
		long nSkipped = 0;
		if (ImportARBCalEntry(book.GetCalendar(), nAdded, nUpdated, nDuplicate, nSkipped))
		{
			m_Records.GetCalendar().sort();
			CUpdateHint hint(UPDATE_CALENDAR_VIEW);
			UpdateAllViews(nullptr, &hint);
			Modify(true);
		}

		std::wstring str = fmt::format(_("IDS_UPDATED_CAL_ITEMS").wx_str(), nAdded, nUpdated);
		wxMessageBox(str, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
		bOk = true;
	}
	else if (0 < err.m_ErrMsg.size())
		wxMessageBox(StringUtil::stringWX(fmt::to_string(err.m_ErrMsg)), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_WARNING);
	return bOk;
}


bool CAgilityBookDoc::ImportARBLogData(ElementNodePtr const& inTree, wxWindow* pParent)
{
	wxBusyCursor wait;
	bool bOk = false;
	CErrorCallback err;
	ARBAgilityRecordBook book;
	if (book.Load(inTree, false, true, false, false, false, err))
	{
		if (0 < err.m_ErrMsg.size())
			wxMessageBox(StringUtil::stringWX(fmt::to_string(err.m_ErrMsg)), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);

		long nAdded = 0;
		long nUpdated = 0;
		long nDuplicate = 0;
		long nSkipped = 0;
		if (ImportARBTrainingEntry(book.GetTraining(), nAdded, nUpdated, nDuplicate, nSkipped))
		{
			m_Records.GetTraining().sort();
			CUpdateHint hint(UPDATE_TRAINING_VIEW);
			UpdateAllViews(nullptr, &hint);
			Modify(true);
		}

		std::wstring str = fmt::format(_("IDS_UPDATED_TRAINING_ITEMS").wx_str(), nAdded, nUpdated);
		wxMessageBox(str, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
		bOk = true;
	}
	else if (0 < err.m_ErrMsg.size())
		wxMessageBox(StringUtil::stringWX(fmt::to_string(err.m_ErrMsg)), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_WARNING);
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
		ARBDogPtr const& inDog)
{
	bool bChanged = false;
	for (ARBDogTrialList::iterator iterTrial = inDog->GetTrials().begin(); iterTrial != inDog->GetTrials().end(); ++iterTrial)
		bChanged |= ResetVisibility(venues, *iterTrial);

	for (ARBDogTitleList::iterator iterTitle = inDog->GetTitles().begin(); iterTitle != inDog->GetTitles().end(); ++iterTitle)
		bChanged |= ResetVisibility(venues, *iterTitle);
	return bChanged;
}


bool CAgilityBookDoc::ResetVisibility(
		std::vector<CVenueFilter>& venues,
		ARBDogTrialPtr const& inTrial)
{
	bool bChanged = false;
	bool bVisTrial = CFilterOptions::Options().IsTrialVisible(venues, inTrial);
	inTrial->SetFiltered(!bVisTrial);
	int nVisible = 0;
	for (ARBDogRunList::iterator iterRun = inTrial->GetRuns().begin(); iterRun != inTrial->GetRuns().end(); ++iterRun)
	{
		bChanged |= ResetVisibility(venues, inTrial, *iterRun);
		if (!(*iterRun)->IsFiltered())
			++nVisible;
	}
	if (0 == nVisible && 0 < inTrial->GetRuns().size())
	{
		if (!inTrial->IsAnyFiltered())
		{
			bChanged = true;
			inTrial->SetFiltered(true);
		}
	}
	return bChanged;
}


bool CAgilityBookDoc::ResetVisibility(
		std::vector<CVenueFilter>& venues,
		ARBDogTrialPtr const& inTrial,
		ARBDogRunPtr const& inRun)
{
	bool bChanged = false;
	unsigned short nVisRun = CFilterOptions::Options().IsRunVisible(venues, inTrial, inRun);
	bool bFilter = (nVisRun & GetFilterMask(ARBFilterType::Full)) ? false : true;
	bool bIgnore = (nVisRun & GetFilterMask(ARBFilterType::IgnoreQ)) ? false : true;
	if (inRun->IsFiltered(ARBFilterType::Full) != bFilter)
	{
		bChanged = true;
		inRun->SetFiltered(ARBFilterType::Full, bFilter);
	}
	if (inRun->IsFiltered(ARBFilterType::IgnoreQ) != bIgnore)
	{
		bChanged = true;
		inRun->SetFiltered(ARBFilterType::IgnoreQ, bIgnore);
	}
	return bChanged;
}


bool CAgilityBookDoc::ResetVisibility(
		std::vector<CVenueFilter>& venues,
		ARBDogTitlePtr const& inTitle)
{
	bool bChanged = false;
	bool bVisTitle = CFilterOptions::Options().IsTitleVisible(venues, inTitle);
	if (inTitle->IsAnyFiltered() != !bVisTitle)
	{
		bChanged = true;
		inTitle->SetFiltered(!bVisTitle);
	}
	return bChanged;
}


bool CAgilityBookDoc::ResetVisibility(
		std::set<std::wstring>& names,
		ARBTrainingPtr const& inTraining)
{
	bool bChanged = false;
	bool bVisTraining = CFilterOptions::Options().IsTrainingLogVisible(names, inTraining);
	if (inTraining->IsAnyFiltered() != !bVisTraining)
	{
		bChanged = true;
		inTraining->SetFiltered(!bVisTraining);
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
	return nullptr;
}


/**
 * Function to get the tree view. This is used internally and by the runs view.
 */
CAgilityBookTreeView* CAgilityBookDoc::GetTreeView() const
{
	for (wxList::const_iterator iView = GetViews().begin(); iView != GetViews().end(); ++iView)
	{
		CAgilityBookTreeView* pView = wxDynamicCast(*iView, CAgilityBookTreeView);
		if (pView)
			return pView;
	}
	return nullptr;
}


/**
 * Function to get the run view.
 */
CAgilityBookRunsView* CAgilityBookDoc::GetRunsView() const
{
	for (wxList::const_iterator iView = GetViews().begin(); iView != GetViews().end(); ++iView)
	{
		CAgilityBookRunsView* pView = wxDynamicCast(*iView, CAgilityBookRunsView);
		if (pView)
			return pView;
	}
	return nullptr;
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
	return nullptr;
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
	return nullptr;
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
	return nullptr;
}


void CAgilityBookDoc::BackupFile(wxString const& lpszPathName)
{
	CreateBackupFile(lpszPathName, CAgilityBookOptions::GetNumBackupFiles());
}


bool CAgilityBookDoc::DeleteContents()
{
	if (!wxDocument::DeleteContents())
		return false;
	m_fileHash.clear();
	std::wstring msg(_("IDS_INDICATOR_BLANK"));
	wxGetApp().SetMessageText(msg, CFilterOptions::Options().IsFilterEnabled());
	wxGetApp().SetMessageText2(msg);
	m_Records.clear();
	return true;
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


// Can we prompt to update the config?
bool CAgilityBookDoc::IsDocumentUpdatable(wxString const& filename) const
{
	// Readonly file will not update.
	if (!wxFileName::IsFileWritable(filename))
		return false;

	// Update allowed if no dogs in the file
	if (0 == m_Records.GetDogs().size())
		return true;

	// Update allowed if a dog is not deceased.
	for (ARBDogList::const_iterator iDog = m_Records.GetDogs().begin(); iDog != m_Records.GetDogs().end(); ++iDog)
	{
		if (!(*iDog)->GetDeceased().IsValid())
			return true;
	}

	// All dogs are deceased, so no update.
	return false;
}


std::wstring CAgilityBookDoc::GenerateHash(wxString const& filename) const
{
	wxFileInputStream file(StringUtil::stringW(filename));
	wxStdInputStream stdfile(file);
	return ARBMsgDigest::Compute(stdfile, ARBMsgDigest::ARBDigest::SHA1, nullptr);
}


// We override this instead of DoOpenDocument because we may need to modify
// the document.
bool CAgilityBookDoc::OnOpenDocument(const wxString& filename)
{
	if (!OnSaveModified())
		return false;

	STACK_TRACE(stack, L"CAgilityBookDoc");

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

		STACK_TICKLE(stack, L"PreLoadXML");
		fmt::wmemory_buffer err;
		ElementNodePtr tree(ElementNode::New());
		// Translate the XML to a tree form.
		if (!tree->LoadXML(filename, err))
		{
			wxConfig::Get()->Write(CFG_SETTINGS_LASTFILE, wxEmptyString);
			fmt::wmemory_buffer msg;
			fmt::format_to(msg, _("Cannot open file '{}'.").wx_str(), filename.wx_str());
			if (0 < err.size())
			{
				fmt::format_to(msg, L"\n\n{}", fmt::to_string(err));
			}
			wxMessageBox(fmt::to_string(msg), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
			return false;
		}
		STACK_TICKLE(stack, L"PostLoadXML");

		// Translate the tree to a class structure.
		CErrorCallback callback;
		if (!m_Records.Load(tree, callback))
		{
			wxConfig::Get()->Write(CFG_SETTINGS_LASTFILE, wxEmptyString);
			fmt::wmemory_buffer msg;
			fmt::format_to(msg, _("Cannot open file '{}'.").wx_str(), filename.wx_str());
			if (0 < callback.m_ErrMsg.size())
			{
				fmt::format_to(msg, L"\n\n{}", fmt::to_string(callback.m_ErrMsg));
			}
			wxMessageBox(fmt::to_string(msg), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
			return false;
		}
		else if (0 < callback.m_ErrMsg.size())
		{
			std::wstring msg = fmt::format(L"{}\n\n{}",
					_("IDS_NONFATAL_MSGS").wx_str(),
					fmt::to_string(callback.m_ErrMsg));
			wxMessageBox(msg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
		}
		STACK_TICKLE(stack, L"PostLoad");

		SortDates();
		STACK_TICKLE(stack, L"PostSort");

		ResetVisibility();
		STACK_TICKLE(stack, L"PostReset");

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

	STACK_TICKLE(stack, L"PreCurDog");
	if (1 == m_Records.GetDogs().size())
	{
		m_pCurrentDog = m_Records.GetDogs().front();
	}
	else
	{
		std::wstring strCallName = StringUtil::stringW(wxConfig::Get()->Read(CFG_SETTINGS_LASTDOG, wxString()));
		if (!strCallName.empty())
		{
			for (ARBDogList::const_iterator iDog = m_Records.GetDogs().begin(); iDog != m_Records.GetDogs().end(); ++iDog)
			{
				if ((*iDog)->GetCallName() == strCallName)
				{
					m_pCurrentDog = *iDog;
					break;
				}
			}
		}
	}
	STACK_TICKLE(stack, L"PostCurDog");

	// Check our internal config (if we're allowed to update)
	if (IsDocumentUpdatable(filename))
	{
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
	}

// The old MFC code would prompt to create a new dog if this was a new file.
// I'm not going to implement that in wxWidgets.
	/*
	if (0 == m_Records.GetDogs().size() && AfxGetMainWnd() && ::IsWindow(AfxGetMainWnd()->GetSafeHwnd()))
	{
		AfxGetMainWnd()->PostMessage(PM_DELAY_MESSAGE, CREATE_NEWDOG);
	}
	*/

	STACK_TICKLE(stack, L"PreKick (post isupdateable)");
	// Kick the LoadData in every view
	{
		wxBusyCursor wait;
		UpdateAllViews();
	}
	STACK_TICKLE(stack, L"PostKick");
	// Finally, force a status update (currently, the last loaddata is the winner)
	if (GetDocumentManager())
	{
		CAgilityBookBaseExtraView* pView = wxDynamicCast(GetDocumentManager()->GetCurrentView(), CAgilityBookBaseExtraView);
		if (pView)
			pView->UpdateMessages();
	}
	STACK_TICKLE(stack, L"PostUpdate");

	m_fileHash = GenerateHash(filename);

	return true;
}


bool CAgilityBookDoc::Save()
{
	// Check if file externally modified
	if (!m_fileHash.empty() && !GetFilename().empty())
	{
		std::wstring hash = GenerateHash(GetFilename());
		if (!hash.empty() && hash != m_fileHash)
		{
			if (wxYES != wxMessageBox(_("IDS_WARN_FILE_UPDATED"), wxMessageBoxCaptionStr, wxICON_WARNING | wxYES_NO))
				return false;
		}
	}
	return wxDocument::Save();
}


bool CAgilityBookDoc::DoSaveDocument(const wxString& filename)
{
	wxBusyCursor wait;

	STACK_TRACE(stack, L"CAgilityBookDoc::DoSave");

	CVersionNum ver(ARB_VER_MAJOR, ARB_VER_MINOR, ARB_VER_DOT, ARB_VER_BUILD);
	std::wstring verstr = ver.GetVersionString();
	bool bAlreadyWarned = false;
	bool bOk = false;
	ElementNodePtr tree(ElementNode::New());
	// First, we have to push all the class data into a tree.
	if (m_Records.Save(tree, verstr, true, true, true, true, true))
	{
		STACK_TICKLE(stack, L"PostTreeSave");

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
			std::wstring errMsg = fmt::format(_("IDS_CANNOT_OPEN").wx_str(), filename);
			wxMessageBox(errMsg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
		}
	}
	if (!bOk && !bAlreadyWarned)
	{
		wxMessageBox(_("IDS_INTERNAL_ERROR"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
	}

	m_fileHash = GenerateHash(filename);
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
	bool Search(CDlgFind* pDlg) const override;
	mutable std::vector<CFindItemInfo> m_Items;
private:
	CAgilityBookDoc* m_pDoc;
	void Search(
			std::wstring const& search,
			ARBInfoType inType,
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
	Search(search, ARBInfoType::Club, inUse, info);
	m_pDoc->Book().GetAllJudges(inUse, false, false);
	Search(search, ARBInfoType::Judge, inUse, info);
	m_pDoc->Book().GetAllTrialLocations(inUse, false, false);
	Search(search, ARBInfoType::Location, inUse, info);
	if (0 < m_Items.size())
	{
		pDlg->EndModal(wxID_OK);
		return true;
	}
	else
	{
		std::wstring msg = fmt::format(_("IDS_CANNOT_FIND").wx_str(), m_strSearch);
		wxMessageBox(msg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
		return false;
	}
}


void CFindInfo::Search(
		std::wstring const& search,
		ARBInfoType inType,
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


void CAgilityBookDoc::OnUpdateCmdTrue(wxUpdateUIEvent& evt)
{
	evt.Enable(true);
}


void CAgilityBookDoc::OnUpdateCmd(wxUpdateUIEvent& evt)
{
	switch (evt.GetId())
	{
	case ID_COPY_TITLES_LIST:
		// As long as a dog is current, enable. This means the copy may have
		// only the dog's name.
		evt.Enable(GetCurrentDog() ? true : false);
		break;
	case ID_UNSORT:
		evt.Enable(false);
		evt.Skip();
		break;
	case ID_VIEW_SORTRUNS:
		evt.Check(CAgilityBookOptions::GetNewestDatesFirst() ? 1 : 0);
		evt.Enable(false);
		evt.Skip();
		break;
	case ID_VIEW_RUNS_BY_TRIAL:
		evt.Check(ARBViewRuns::RunsByTrial == CAgilityBookOptions::GetViewRunsStyle() ? 1 : 0);
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
					CAgilityBookTreeView* pTree = GetTreeView();
					if (pTree)
						pTree->InsertDog(dog, true);
				}
			}
		}
		break;

	case ID_AGILITY_NEW_CALENDAR:
		{
			ARBCalendarPtr cal(ARBCalendar::New());
			wxView* pView = GetDocumentManager()->GetCurrentView();
			if (pView)
			{
				CAgilityBookCalendarView* pCalView = GetCalendarView();
				CAgilityBookCalendarListView* pCalListView = GetCalendarListView();
				if ((pCalView == pView || pCalListView == pView) &&
					pCalView->GetCurrentDate().IsValid())
				{
					cal->SetStartDate(pCalView->GetCurrentDate());
					cal->SetEndDate(pCalView->GetCurrentDate() + 1);
				}
			}
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
					UpdateAllViews(nullptr, &hint);
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
				UpdateAllViews(nullptr, &hint);
				CAgilityBookTrainingView* pView = GetTrainingView();
				if (pView)
					pView->SetCurrentDate(training->GetDate());
			}
		}
		break;

	case ID_NOTES_CLUBS:
		{
			std::wstring select;
			ARBDogRunPtr pRun = GetCurrentRun();
			if (pRun)
				select = pRun->GetClub()->GetName();
			CDlgInfoNote dlg(this, ARBInfoType::Club, select, wxGetApp().GetTopWindow());
			dlg.ShowModal();
		}
		break;

	case ID_NOTES_JUDGES:
		{
			std::wstring select;
			ARBDogRunPtr pRun = GetCurrentRun();
			if (pRun)
				select = pRun->GetJudge();
			CDlgInfoNote dlg(this, ARBInfoType::Judge, select, wxGetApp().GetTopWindow());
			dlg.ShowModal();
		}
		break;

	case ID_NOTES_LOCATIONS:
		{
			std::wstring select;
			ARBDogTrialPtr pTrial = GetCurrentTrial();
			if (pTrial)
				select = pTrial->GetLocation();
			CDlgInfoNote dlg(this, ARBInfoType::Location, select, wxGetApp().GetTopWindow());
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
			int nPage = 0;
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


void CAgilityBookDoc::OnUpdateFileProperties(wxUpdateUIEvent& evt)
{
	evt.Enable(!GetFilename().empty());
}


void CAgilityBookDoc::OnFileProperties(wxCommandEvent& evt)
{
	fmt::wmemory_buffer str;

	wxString filename = GetFilename();
	if (!filename.empty())
	{
		wxFileName file(filename);
		if (file.Exists())
		{
			fmt::format_to(str, _("IDS_FILEPROP_NAME").wx_str(), filename.wx_str());

			if (!wxFileName::IsFileWritable(filename))
				fmt::format_to(str, _("IDS_FILEPROP_NAME_READONLY").wx_str());

			if (IsModified())
				fmt::format_to(str, _("IDS_FILEPROP_NAME_MODIFIED").wx_str());

			fmt::format_to(str, L"\n");

			fmt::format_to(str, L"{}\n", fmt::format(_("IDS_FILEPROP_SIZE").wx_str(), file.GetSize().GetValue()));

			wxDateTime timeCreate, timeMod;
			if (GetFileTimes(file, nullptr, &timeMod, &timeCreate))
			{
				fmt::format_to(str, L"{}\n", fmt::format(_("IDS_FILEPROP_CREATED").wx_str(), timeCreate.FormatISOCombined().wx_str()));
				fmt::format_to(str, L"{}\n", fmt::format(_("IDS_FILEPROP_MODIFIED").wx_str(), timeMod.FormatISOCombined().wx_str()));
			}
		}
		else
		{
			fmt::format_to(str, filename.wx_str());
			if (IsModified())
				fmt::format_to(str, _("IDS_FILEPROP_NAME_MODIFIED").wx_str());
			fmt::format_to(str, L"\n");
		}
	}

	fmt::format_to(str, L"\n");

	if (Book().GetConfig().GetVersion() == GetCurrentConfigVersion())
		fmt::format_to(str, L"{}\n", fmt::format(_("IDS_FILEPROP_CONFIGURATION").wx_str(), Book().GetConfig().GetVersion()));
	else
		fmt::format_to(str, L"{}\n", fmt::format(_("IDS_FILEPROP_CONFIGURATION_CURRENT").wx_str(), Book().GetConfig().GetVersion(), GetCurrentConfigVersion()));

	if (!Book().GetFileInfo(ARBFileInfo::Book).empty())
	{
		ARBVersion ver(Book().GetFileInfo(ARBFileInfo::Book));
		if (ver == ARBAgilityRecordBook::GetCurrentDocVersion())
			fmt::format_to(str, L"{}\n", fmt::format(_("IDS_FILEPROP_VERSION").wx_str(), ver.str()));
		else
			fmt::format_to(str, L"{}\n", fmt::format(_("IDS_FILEPROP_VERSION_CURRENT").wx_str(), ver.str(), ARBAgilityRecordBook::GetCurrentDocVersion().str()));
	}

	if (!Book().GetFileInfo(ARBFileInfo::OS).empty()
	|| !Book().GetFileInfo(ARBFileInfo::Platform).empty()
	|| !Book().GetFileInfo(ARBFileInfo::TimeStamp).empty()
	|| !Book().GetFileInfo(ARBFileInfo::Version).empty())
	{
		fmt::format_to(str, L"\n{}\n{}\n", fmt::format(_("IDS_FILEPROP_FILEWRITTEN").wx_str(),
			Book().GetFileInfo(ARBFileInfo::OS),
			Book().GetFileInfo(ARBFileInfo::Platform),
			Book().GetFileInfo(ARBFileInfo::TimeStamp),
			Book().GetFileInfo(ARBFileInfo::Version)),
			wxVERSION_STRING);
	}

	CDlgMessage dlg(fmt::to_string(str), wxGetApp().GetTopWindow(), _("IDS_FILE_PROPERTIES").wc_str());
	dlg.ShowModal();
}


#ifdef _DEBUG
void CAgilityBookDoc::OnHelpDebug(wxCommandEvent& evt)
{
	wxWindow* parent = wxGetApp().GetTopWindow();

	wxSize x;
	x = parent->ConvertPixelsToDialog(wxSize(400, 0));
	std::wstring s = fmt::format(L"px->dlg {},{}\n", x.x, x.y);
	wxMessageBox(s);

//	dlg.ShowModal();
}
#endif
