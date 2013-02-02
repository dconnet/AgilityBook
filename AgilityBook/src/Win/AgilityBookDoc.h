#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CAgilityBookDoc class
 * @author David Connet
 *
 * Revision History
 * @li 2012-09-29 DRC Strip the Runs View.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
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

#include "CalendarSites.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBTypes.h"
#include <set>
#include <wx/docview.h>
class CAgilityBookCalendarListView;
class CAgilityBookCalendarView;
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
#define UPDATE_TREE_VIEW			0x0008
#define UPDATE_ALL_VIEW				(UPDATE_CALENDAR_VIEW|UPDATE_TRAINING_VIEW|UPDATE_POINTS_VIEW|UPDATE_TREE_VIEW)
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
	virtual bool OnError(wchar_t const* const pMsg);
	std::wostringstream m_ErrMsg;
};


class CAgilityBookDoc : public wxDocument
{
	DECLARE_DYNAMIC_CLASS(CAgilityBookDoc)
public:
	CAgilityBookDoc();
	~CAgilityBookDoc();

	bool StatusBarContextMenu(wxWindow* parent, int id, wxPoint const& point);

	std::wstring AddDogToCaption(std::wstring const& caption) const;

	// Data
	ARBDogPtr GetCurrentDog() const;
	void SetCurrentDog(ARBDogPtr pDog);
	ARBDogTrialPtr GetCurrentTrial() const;
	ARBDogRunPtr GetCurrentRun() const;
	ARBAgilityRecordBook& Book()			{return m_Records;}

	bool AddTitle(ARBDogPtr pDog);
	// These are called from the Runs view so the tree view can do the add.
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

	bool ImportARBCalEntry(
			ARBCalendarList const& listCal,
			long& nAdded,
			long& nUpdated,
			long& nDuplicate,
			long& nSkipped);
	bool ImportARBTrainingEntry(
			ARBTrainingList const& listLog,
			long& nAdded,
			long& nUpdated,
			long& nDuplicate,
			long& nSkipped);

	bool ImportARBRunData(ElementNodePtr inTree, wxWindow* pParent);
	bool ImportARBCalData(ElementNodePtr inTree, wxWindow* pParent);
	bool ImportARBLogData(ElementNodePtr inTree, wxWindow* pParent);

	/**
	 * Reset the visibility of all objects.
	 * @return Visibility was changed.
	 * @post All UI objects will be reset, all pointers to data objects will
	 *       be deleted. This should not be called from any AgilityBookTreeData
	 *       object!
	 */
	bool ResetVisibility();
	bool ResetVisibility(
			std::vector<CVenueFilter>& venues,
			ARBDogPtr pDog);
	bool ResetVisibility(
			std::vector<CVenueFilter>& venues,
			ARBDogTrialPtr pTrial);
	bool ResetVisibility(
			std::vector<CVenueFilter>& venues,
			ARBDogTrialPtr pTrial,
			ARBDogRunPtr pRun);
	bool ResetVisibility(
			std::vector<CVenueFilter>& venues,
			ARBDogTitlePtr pTitle);
	bool ResetVisibility(
			std::set<std::wstring>& names,
			ARBTrainingPtr pTraining);

	bool ShowPointsAsHtml(bool bHtml);
	CTabView* GetTabView() const;
	CAgilityBookTreeView* GetTreeView() const;
	CAgilityBookCalendarListView* GetCalendarListView() const;
	CAgilityBookCalendarView* GetCalendarView() const;
	CAgilityBookTrainingView* GetTrainingView() const;

	void BackupFile(wxString const& lpszPathName);

private:
	ARBAgilityRecordBook m_Records; ///< The real records.
	CCalendarSites m_CalSites;
	CStatusHandler* m_StatusData;
	ARBDogPtr m_pCurrentDog;

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
