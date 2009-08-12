#pragma once

/*
 * Copyright (c) 2002-2009 David Connet. All Rights Reserved.
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
 * @brief interface of the CAgilityBookDoc class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-05 DRC Ported to wxWidgets.
 * @li 2008-11-19 DRC Added context menus to status bar.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-01-02 DRC Added subnames to events.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-10 DRC Moved import/export into a wizard.
 * @li 2003-10-31 DRC Added import/export calendar, export config.
 * @li 2003-10-22 DRC Added export dtd/xml menu options.
 * @li 2003-10-09 DRC Added option to not filter runs by selected trial.
 * @li 2003-08-27 DRC Added view accessors for calendar, made them public so
 *                    I don't have to use UpdateAllViews. Added methods to allow
 *                    creating titles/trials/runs from the Run view.
 * @li 2003-08-25 DRC Added GetCurrentRun().
 * @li 2003-08-24 DRC Optimized filtering by adding boolean into ARBBase to
 *                    prevent constant re-evaluation.
 */

#include "ARBAgilityRecordBook.h"
#include "ARBTypes.h"
#include "CalendarSites.h"
#include <set>
#include <wx/docview.h>
class CAgilityBookCalendarListView;
class CAgilityBookCalendarView;
class CAgilityBookRunsView;
class CAgilityBookTrainingView;
class CAgilityBookTreeView;
class CStatusHandler;
class CTabView;
struct CVenueFilter;


// UpdateAllViews() hints: Only the view bits can be or'd. Each item is
// considered a single event (consider all views as one)
#define UPDATE_VIEW_MASK			0x00ff
#define UPDATE_CALENDAR_VIEW		0x0001
#define UPDATE_TRAINING_VIEW		0x0002
#define UPDATE_POINTS_VIEW			0x0004
#define UPDATE_RUNS_VIEW			0x0008
#define UPDATE_RUNS_SELECTION_VIEW	0x0010
#define UPDATE_TREE_VIEW			0x0020
#define UPDATE_ALL_VIEW				(UPDATE_CALENDAR_VIEW|UPDATE_TRAINING_VIEW|UPDATE_POINTS_VIEW|UPDATE_RUNS_VIEW|UPDATE_TREE_VIEW)
#define UPDATE_CONFIG				0x0100
#define UPDATE_OPTIONS				0x0200
#define UPDATE_NEW_TRIAL			0x0300
#define UPDATE_LANG_CHANGE			0x0400
#define UPDATE_CUSTOMIZE			0x0500

class CUpdateHint : public wxObject
{
public:
	CUpdateHint(unsigned int hint, ARBBasePtr pObj = ARBBasePtr())
		: m_Hint(hint)
		, m_pObj(pObj)
	{
	}
	bool IsSet(unsigned int bit) const
	{
		// '?' to avoid the bool optimization warning
		return (m_Hint & bit) ? true : false;
	}
	bool IsEqual(unsigned int val) const
	{
		return m_Hint == val;
	}
	ARBBasePtr GetObj() const	{return m_pObj;}
private:
	unsigned int m_Hint;
	ARBBasePtr m_pObj;
};


class CErrorCallback : public ARBErrorCallback
{
public:
	CErrorCallback();
	virtual bool OnError(wxChar const* const pMsg);
	tstring m_ErrMsg;
};


class CAgilityBookDoc : public wxDocument
{
	DECLARE_DYNAMIC_CLASS(CAgilityBookDoc)
public:
	CAgilityBookDoc();
	~CAgilityBookDoc();

	bool StatusBarContextMenu(wxWindow* parent, int id, wxPoint const& point);

	// Data
	ARBDogPtr GetCurrentDog();
	ARBDogTrialPtr GetCurrentTrial();
	ARBDogRunPtr GetCurrentRun();
	ARBAgilityRecordBook& Book()			{return m_Records;}

	// These are called from the Runs view so the tree view can do the add.
	void AddTitle(ARBDogRunPtr pSelectedRun);
	void AddTrial(ARBDogRunPtr pSelectedRun);
	void AddRun(ARBDogRunPtr pSelectedRun);
	void EditRun(ARBDogRunPtr pRun);
	void DeleteRun(ARBDogRunPtr pRun);

	bool CreateTrialFromCalendar(
			ARBCalendar const& cal,
			CTabView* pTabView);
	void SortDates();
	void ImportConfiguration(ARBConfig& update);
	bool ImportConfiguration(bool bUseDefault);
	bool ImportARBRunData(ElementNodePtr inTree, wxWindow* pParent);
	bool ImportARBCalData(ElementNodePtr inTree, wxWindow* pParent);
	bool ImportARBLogData(ElementNodePtr inTree, wxWindow* pParent);

	/**
	 * Reset the visibility of all objects.
	 * @post All UI objects will be reset, all pointers to data objects will
	 *       be deleted. This should not be called from any AgilityBookTreeData
	 *       object!
	 */
	void ResetVisibility();
	void ResetVisibility(
			std::vector<CVenueFilter>& venues,
			ARBDogPtr pDog);
	void ResetVisibility(
			std::vector<CVenueFilter>& venues,
			ARBDogTrialPtr pTrial);
	void ResetVisibility(
			std::vector<CVenueFilter>& venues,
			ARBDogTrialPtr pTrial,
			ARBDogRunPtr pRun);
	void ResetVisibility(
			std::vector<CVenueFilter>& venues,
			ARBDogTitlePtr pTitle);
	void ResetVisibility(
			std::set<tstring>& names,
			ARBTrainingPtr pTraining);

	bool ShowPointsAsHtml(bool bHtml);
	CTabView* GetTabView() const;
	CAgilityBookTreeView* GetTreeView() const;
	CAgilityBookRunsView* GetRunsView() const;
	CAgilityBookCalendarListView* GetCalendarListView() const;
	CAgilityBookCalendarView* GetCalendarView() const;
	CAgilityBookTrainingView* GetTrainingView() const;

	void BackupFile(wxString lpszPathName);

private:
	bool m_SuppressUpdates;
	ARBAgilityRecordBook m_Records; ///< The real records.
	CCalendarSites m_CalSites;
	CStatusHandler* m_StatusData;

protected:
	virtual bool DeleteContents();
	virtual bool OnNewDocument();
	virtual bool OnOpenDocument(const wxString& file);
	virtual bool DoSaveDocument(const wxString& file);
	virtual bool OnCloseDocument();

private:
	DECLARE_EVENT_TABLE()
	void OnStatusDog(wxCommandEvent& evt);
	void OnStatusFilter(wxCommandEvent& evt);
	void OnUpdateCmd(wxUpdateUIEvent& evt);
	void OnCmd(wxCommandEvent& evt);
};
