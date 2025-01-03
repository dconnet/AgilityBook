#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CAgilityBookDoc class
 * @author David Connet
 *
 * Revision History
 * 2015-10-29 Add Save override.
 * 2012-09-29 Strip the Runs View.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-05 Ported to wxWidgets.
 * 2008-11-19 Added context menus to status bar.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-01-02 Added subnames to events.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2003-12-10 Moved import/export into a wizard.
 * 2003-10-31 Added import/export calendar, export config.
 * 2003-10-22 Added export dtd/xml menu options.
 * 2003-10-09 Added option to not filter runs by selected trial.
 * 2003-08-27 Added view accessors for calendar, made them public so
 *            I don't have to use UpdateAllViews. Added methods to allow
 *            creating titles/trials/runs from the Run view.
 * 2003-08-25 Added GetCurrentRun().
 * 2003-08-24 Optimized filtering by adding boolean into ARBBase to
 *            prevent constant re-evaluation.
 */

#include "ARB/ARBAgilityRecordBook.h"
#include "ARBCommon/ARBTypes.h"
#include <wx/docview.h>
#include <set>


namespace dconSoft
{
class CAgilityBookCalendarListView;
class CAgilityBookCalendarView;
class CAgilityBookTrainingView;
class CAgilityBookRunsView;
class CAgilityBookTreeView;
class CStatusHandler;
class CTabView;
struct CVenueFilter;


// UpdateAllViews() hints: Only the view bits can be or'd. Each item is
// considered a single event (consider all views as one)
#define UPDATE_VIEW_MASK           0x00ff
#define UPDATE_CALENDAR_VIEW       0x0001
#define UPDATE_TRAINING_VIEW       0x0002
#define UPDATE_POINTS_VIEW         0x0004
#define UPDATE_RUNS_VIEW           0x0008
#define UPDATE_RUNS_SELECTION_VIEW 0x0010
#define UPDATE_TREE_VIEW           0x0020
#define UPDATE_CONFIG              0x0100
#define UPDATE_OPTIONS             0x0200
#define UPDATE_NEW_TRIAL           0x0300
#define UPDATE_LANG_CHANGE         0x0400
#define UPDATE_CUSTOMIZE           0x0500
#define UPDATE_ALL_VIEW \
	(UPDATE_CALENDAR_VIEW | UPDATE_TRAINING_VIEW | UPDATE_POINTS_VIEW | UPDATE_RUNS_VIEW | UPDATE_TREE_VIEW)

class CUpdateHint : public wxObject
{
public:
	CUpdateHint(unsigned int hint, ARB::ARBBasePtr const& inObj = ARB::ARBBasePtr())
		: m_Hint(hint)
		, m_pObj(inObj)
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
	ARB::ARBBasePtr GetObj() const
	{
		return m_pObj;
	}

private:
	unsigned int m_Hint;
	ARB::ARBBasePtr m_pObj;
};


class CErrorCallback : public ARB::ARBErrorCallback
{
public:
	CErrorCallback();
	bool OnError(wchar_t const* const pMsg) override;
	wxString m_ErrMsg;
};


class CAgilityBookDoc : public wxDocument
{
	DECLARE_DYNAMIC_CLASS(CAgilityBookDoc)
	DECLARE_NO_COPY_IMPLEMENTED(CAgilityBookDoc)
public:
	CAgilityBookDoc();
	~CAgilityBookDoc();

	bool StatusBarContextMenu(wxWindow* parent, int id, wxPoint const& point);

	wxString AddDogToCaption(wxString const& caption) const;

	// Data
	ARB::ARBDogPtr GetCurrentDog() const;
	void SetCurrentDog(ARB::ARBDogPtr const& inDog, bool bSuppressHints = false);
	ARB::ARBDogTrialPtr GetCurrentTrial() const;
	ARB::ARBDogRunPtr GetCurrentRun() const;
	ARB::ARBAgilityRecordBook& Book()
	{
		return m_Records;
	}

	bool EditDog(ARB::ARBDogPtr const& inDog, int nPage = 0);
	bool AddTitle(ARB::ARBDogPtr const& inDog);
	bool EditTrial(ARB::ARBDogPtr const& inDog, ARB::ARBDogTrialPtr const& inTrial);
	bool DeleteTrial(ARB::ARBDogPtr const& inDog, ARB::ARBDogTrialPtr const& inTrial, bool bSilent = false);
	bool EditRun(ARB::ARBDogPtr const& inDog, ARB::ARBDogTrialPtr const& inTrial, ARB::ARBDogRunPtr const& inRun);
	bool DeleteRuns(std::vector<ARB::ARBDogRunPtr> const& inRuns, bool bSilent = false);

	bool CreateTrialFromCalendar(ARB::ARBCalendar const& cal, CTabView* pTabView);
	bool SetCalenderDate(ARBCommon::ARBDate const& date);
	void SortDates();
	bool ImportConfiguration(ARB::ARBConfig const& update);
	bool ImportConfiguration(bool bUseDefault);

	bool ImportARBCalEntry(
		ARB::ARBCalendarList const& listCal,
		long& nAdded,
		long& nUpdated,
		long& nDuplicate,
		long& nSkipped);
	bool ImportARBTrainingEntry(
		ARB::ARBTrainingList const& listLog,
		long& nAdded,
		long& nUpdated,
		long& nDuplicate,
		long& nSkipped);

	bool ImportARBRunData(ARBCommon::ElementNodePtr const& inTree, wxWindow* pParent);
	bool ImportARBCalData(ARBCommon::ElementNodePtr const& inTree, wxWindow* pParent);
	bool ImportARBLogData(ARBCommon::ElementNodePtr const& inTree, wxWindow* pParent);

	/**
	 * Reset the visibility of all objects.
	 * @return Visibility was changed.
	 * @post All UI objects will be reset, all pointers to data objects will
	 *       be deleted. This should not be called from any AgilityBookTreeData
	 *       object!
	 */
	bool ResetVisibility();
	bool ResetVisibility(std::vector<CVenueFilter> const& venues, ARB::ARBDogPtr const& inDog);
	bool ResetVisibility(std::vector<CVenueFilter> const& venues, ARB::ARBDogTrialPtr const& inTrial);
	bool ResetVisibility(
		std::vector<CVenueFilter> const& venues,
		ARB::ARBDogTrialPtr const& inTrial,
		ARB::ARBDogRunPtr const& inRun);
	bool ResetVisibility(std::vector<CVenueFilter> const& venues, ARB::ARBDogTitlePtr const& inTitle);
	bool ResetVisibility(std::set<wxString> const& names, ARB::ARBTrainingPtr const& inTraining);

	bool ShowPointsAsHtml(bool bHtml);
	void BackupFile(wxString const& lpszPathName);

	CTabView* GetTabView() const;

	CAgilityBookTreeView* GetTreeView() const;
	CAgilityBookRunsView* GetRunsView() const;

private:
	CAgilityBookCalendarListView* GetCalendarListView() const;
	CAgilityBookCalendarView* GetCalendarView() const;
	CAgilityBookTrainingView* GetTrainingView() const;
	bool IsDocumentUpdatable(wxString const& filename) const;
	wxString GenerateHash(wxString const& filename) const;

	wxString m_fileHash;
	ARB::ARBAgilityRecordBook m_Records; ///< The real records.
	CStatusHandler* m_StatusData;
	ARB::ARBDogPtr m_pCurrentDog;

protected:
	bool DeleteContents() override;
	bool OnNewDocument() override;
	bool OnOpenDocument(const wxString& file) override;
	bool Save() override;
	bool DoSaveDocument(const wxString& file) override;
	bool OnCloseDocument() override;

private:
	DECLARE_EVENT_TABLE()
	void OnStatusDog(wxCommandEvent& evt);
	void OnStatusFilter(wxCommandEvent& evt);
	void OnUpdateCmdTrue(wxUpdateUIEvent& evt);
	void OnUpdateCmd(wxUpdateUIEvent& evt);
	void OnCmd(wxCommandEvent& evt);
	void OnUpdateFileProperties(wxUpdateUIEvent& evt);
	void OnFileProperties(wxCommandEvent& evt);
#if defined(_DEBUG) || defined(__WXDEBUG__)
	void OnHelpDebug(wxCommandEvent& evt);
#endif
};

} // namespace dconSoft
