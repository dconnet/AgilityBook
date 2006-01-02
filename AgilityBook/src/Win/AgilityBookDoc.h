#pragma once

/*
 * Copyright © 2002-2006 David Connet. All Rights Reserved.
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

#include <set>
#include "ARBAgilityRecordBook.h"
#include "ARBTypes.h"
class ARBCalendar;
class ARBCalendarList;
class ARBConfig;
class ARBDog;
class ARBDogList;
class ARBDogRun;
class ARBDogTrial;
class ARBTraining;
class CAgilityBookTree;
class CAgilityBookViewCalendar;
class CAgilityBookViewCalendarList;
class CAgilityBookViewTraining;
class CTabView;
struct CVenueFilter;

//
// Application specific message
//
#define PM_DELAY_MESSAGE		(WM_APP+1)
	// Only sent if there are no dogs and we want to auto-create one to start
	#define	CREATE_NEWDOG				1 // LPARAM unused


// UpdateAllViews() hints
#define UPDATE_CONFIG			0x0001
#define UPDATE_CALENDAR_VIEW	0x0010
#define UPDATE_TRAINING_VIEW	0x0020
#define UPDATE_POINTS_VIEW		0x0040
#define UPDATE_RUNS_VIEW		0x0080
#define UPDATE_TREE_VIEW		0x0100
#define UPDATE_ALL_VIEW			(UPDATE_CALENDAR_VIEW|UPDATE_TRAINING_VIEW|UPDATE_POINTS_VIEW|UPDATE_RUNS_VIEW|UPDATE_TREE_VIEW)
#define UPDATE_OPTIONS			0x1000
#define UPDATE_NEW_TRIAL		0x2000

class CErrorCallback : public ARBErrorCallback
{
public:
	CErrorCallback();
	virtual bool OnError(TCHAR const* const pMsg);
	ARBString m_ErrMsg;
};

class CAgilityBookDoc : public CDocument
{
protected: // create from serialization only
	CAgilityBookDoc();
	DECLARE_DYNCREATE(CAgilityBookDoc)

public:
	// Data
	ARBDog* GetCurrentDog();
	ARBDogTrial* GetCurrentTrial();
	ARBDogRun* GetCurrentRun();
	ARBAgilityRecordBook& GetARB()			{return m_Records;}
	ARBCalendarList& GetCalendar()			{return m_Records.GetCalendar();}
	ARBTrainingList& GetTraining()			{return m_Records.GetTraining();}
	ARBConfig& GetConfig()					{return m_Records.GetConfig();}
	ARBInfo& GetInfo()						{return m_Records.GetInfo();}
	ARBDogList& GetDogs()					{return m_Records.GetDogs();}

	size_t GetAllClubNames(
			std::set<ARBString>& outClubs,
			bool bInfo = true) const
	{
		return m_Records.GetAllClubNames(outClubs, bInfo);
	}
	size_t GetAllTrialLocations(
			std::set<ARBString>& outLocations,
			bool bInfo = true) const
	{
		return m_Records.GetAllTrialLocations(outLocations, bInfo);
	}
	size_t GetAllEventSubNames(
			ARBString const& inVenue,
			ARBConfigEvent const* inEvent,
			std::set<ARBString>& outNames) const
	{
		return m_Records.GetAllEventSubNames(inVenue, inEvent, outNames);
	}
	size_t GetAllHeights(std::set<ARBString>& ouHeights) const
	{
		return m_Records.GetAllHeights(ouHeights);
	}
	size_t GetAllCallNames(std::set<ARBString>& outNames) const
	{
		return m_Records.GetAllCallNames(outNames);
	}
	size_t GetAllBreeds(std::set<ARBString>& outBreeds) const
	{
		return m_Records.GetAllBreeds(outBreeds);
	}
	size_t GetAllJudges(
			std::set<ARBString>& outJudges,
			bool bInfo = true) const
	{
		return m_Records.GetAllJudges(outJudges, bInfo);
	}
	size_t GetAllHandlers(std::set<ARBString>& outHandlers) const
	{
		return m_Records.GetAllHandlers(outHandlers);
	}
	size_t GetAllFaultTypes(std::set<ARBString>& outFaults) const
	{
		return m_Records.GetAllFaultTypes(outFaults);
	}
	size_t GetAllTrainingLogNames(std::set<ARBString>& outNames) const
	{
		return m_Records.GetTraining().GetAllNames(outNames);
	}

	// These are called from the Runs view so the tree view can do the add.
	void AddTitle(ARBDogRun* pSelectedRun);
	void AddTrial(ARBDogRun* pSelectedRun);
	void AddRun(ARBDogRun* pSelectedRun);
	void EditRun(ARBDogRun* pRun);
	void DeleteRun(ARBDogRun* pRun);

	bool CreateTrialFromCalendar(
			ARBCalendar const& cal,
			CTabView* pTabView);
	void SortDates();
	void ImportConfiguration(ARBConfig& update);
	bool ImportConfiguration(bool bUseDefault);

	/**
	 * Reset the visibility of all objects.
	 * @post All UI objects will be reset, all pointers to data objects will
	 *       be deleted. This should not be called from any AgilityBookTreeData
	 *       object!
	 */
	void ResetVisibility();
	void ResetVisibility(
			std::vector<CVenueFilter>& venues,
			ARBDog* pDog);
	void ResetVisibility(
			std::vector<CVenueFilter>& venues,
			ARBDogTrial* pTrial);
	void ResetVisibility(
			std::vector<CVenueFilter>& venues,
			ARBDogTrial* pTrial,
			ARBDogRun* pRun);
	void ResetVisibility(
			std::vector<CVenueFilter>& venues,
			ARBDogTitle* pTitle);
	void ResetVisibility(
			std::set<ARBString>& names,
			ARBTraining* pTraining);

	CAgilityBookTree* GetTreeView() const;
	CAgilityBookViewCalendarList* GetCalendarListView() const;
	CAgilityBookViewCalendar* GetCalendarView() const;
	CAgilityBookViewTraining* GetTrainingView() const;

	void BackupFile(LPCTSTR lpszPathName);

// Overrides
public:
	//{{AFX_VIRTUAL(CAgilityBookDoc)
	virtual void DeleteContents();
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAgilityBookDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
private:
	bool m_SuppressUpdates;
	ARBAgilityRecordBook m_Records; ///< The real records.

// Generated message map functions
protected:
	//{{AFX_MSG(CAgilityBookDoc)
	afx_msg void OnAppAbout();
	afx_msg void OnHelpUpdate();
	afx_msg void OnFileExportWizard();
	afx_msg void OnFileLinked();
	afx_msg void OnEditConfiguration();
	afx_msg void OnAgilityNewDog();
	afx_msg void OnAgilityNewCalendar();
	afx_msg void OnAgilityNewTraining();
	afx_msg void OnNotesClubs();
	afx_msg void OnNotesJudges();
	afx_msg void OnNotesLocations();
	afx_msg void OnNotesSearch();
	afx_msg void OnViewOptions();
	afx_msg void OnUpdateViewSortruns(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewRunsByTrial(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewHiddenTitles(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTableInYPS(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
