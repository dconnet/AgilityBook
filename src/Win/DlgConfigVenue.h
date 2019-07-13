#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgConfigVenue class
 * @author David Connet
 *
 * Revision History
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-01-24 Created
 */

#include "ARB/ARBConfigScoring.h"
class ARBAgilityRecordBook;
class CDlgConfigureDataBase;
class CTreeCtrl;
class wxTreeEvent;
class wxTreeItemId;


class CDlgConfigVenue : public wxDialog
{
	friend class CDlgConfigureDataDivision;
	friend class CDlgConfigureDataEvent;
	friend class CDlgConfigureDataLevel;
	friend class CDlgConfigureDataLifetimeName;
	friend class CDlgConfigureDataMultiQ;
	friend class CDlgConfigureDataSubLevel;
	friend class CDlgConfigureDataTitle;
	friend class CDlgConfigVenueDataRoot;
	DECLARE_NO_COPY_IMPLEMENTED(CDlgConfigVenue)

public:
	CDlgConfigVenue(
			ARBAgilityRecordBook const& book,
			ARBConfig const& config,
			ARBConfigVenuePtr const& inVenue,
			wxWindow* pParent = nullptr);
	~CDlgConfigVenue();

	void GetFixups(ARBConfigActionList& ioFixups);

private:
	enum class Action
	{
		None,
		Divisions,
		Events,
		LifetimeNames,
		MultiQ,
		Titles,
	};

	CDlgConfigureDataBase* GetData(wxTreeItemId item) const;
	CDlgConfigureDataBase* GetCurrentData(wxTreeItemId* pItem = nullptr) const;
	void UpdateButtons();
	/*
	int FindCurrentDivision(
			ARBConfigDivisionPtr const& inDiv,
			bool bSet);
	HTREEITEM FindCurrentLevel(
			ARBConfigLevelPtr const& inLevel,
			bool bSet);
	HTREEITEM FindCurrentSubLevel(
			ARBConfigSubLevelPtr const& inSubLevel,
			bool bSet);
	int FindCurrentTitle(
			ARBConfigTitlePtr const& nTitle,
			bool bSet);
	int FindCurrentEvent(
			ARBConfigEventPtr const& nEvent,
			bool bSet);
	int FindCurrentMultiQ(
			ARBConfigMultiQPtr const& inMultiQ,
			bool bSet);
	*/

	ARBAgilityRecordBook const& m_Book;
	ARBConfig const& m_Config;
	ARBConfigVenuePtr m_pVenueOrig;
	ARBConfigVenuePtr m_pVenue;
	std::vector<ARBConfigActionPtr> m_DlgFixup;
	wxString m_Name;
	wxString m_LongName;
	wxString m_URL;
	wxString m_Desc;
	CTreeCtrl* m_ctrlItems;
	wxButton* m_ctrlNew;
	wxButton* m_ctrlEdit;
	wxButton* m_ctrlDelete;
	wxButton* m_ctrlCopy;
	wxButton* m_ctrlMoveUp;
	wxButton* m_ctrlMoveDown;

	DECLARE_ON_INIT()
	DECLARE_EVENT_TABLE()
	void OnSelectionChanged(wxTreeEvent& evt);
	void OnItemActivated(wxTreeEvent& evt);
	void OnKeydown(wxKeyEvent& evt);
	void OnNew(wxCommandEvent& evt);
	void OnDelete(wxCommandEvent& evt);
	void OnEdit(wxCommandEvent& evt);
	void OnCopy(wxCommandEvent& evt);
	void OnMoveUp(wxCommandEvent& evt);
	void OnMoveDown(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
