#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgConfigure class
 * @author David Connet
 *
 * Revision History
 * 2009-02-10 Ported to wxWidgets.
 * 2007-08-19 Simplified UI layout into a single tree.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 */

#include "IconList.h"
#include "ARB/ARBConfig.h"
#include "ARB/ARBTypes2.h"
class ARBAgilityRecordBook;
class CAgilityBookDoc;
class CDlgConfigureDataBase;
class CTreeCtrl;


class CDlgConfigure : public wxDialog
{
	DECLARE_NO_COPY_IMPLEMENTED(CDlgConfigure)
	friend class CDlgConfigureDataRoot; // For 'Action' access
public:
	CDlgConfigure(
			CAgilityBookDoc* pDoc,
			ARBAgilityRecordBook& book,
			wxWindow* pParent = nullptr);
	~CDlgConfigure();

private:
	// Only public so internal data classes can access.
	enum class Action
	{
		None = 0,
		Venues,
		Faults,
		OtherPoints
	};

	Action GetAction() const;
	CDlgConfigureDataBase* GetData(wxTreeItemId hItem) const;
	void UpdateButtons();
	void LoadData(Action dataToLoad);
	void DoEdit();
	wxTreeItemId FindCurrentVenue(
			ARBConfigVenuePtr const& inVenue,
			bool bSet);
	wxTreeItemId FindCurrentFault(
			ARBConfigFaultPtr const& inFault,
			bool bSet);
	wxTreeItemId FindCurrentOtherPoints(
			ARBConfigOtherPointsPtr const& inOther,
			bool bSet);

	CAgilityBookDoc* m_pDoc;
	ARBAgilityRecordBook& m_Book;
	ARBConfig m_Config;
	CIconList m_ImageList;
	int m_idxFaults;
	CTreeCtrl* m_ctrlItems;
	wxButton* m_ctrlNew;
	wxButton* m_ctrlEdit;
	wxButton* m_ctrlDelete;
	wxButton* m_ctrlCopy;
	wxTreeItemId m_hItemVenues;
	wxTreeItemId m_hItemFaults;
	wxTreeItemId m_hItemOtherPts;

	DECLARE_EVENT_TABLE()
	void OnItemActivated(wxTreeEvent& evt);
	void OnSelectionChanged(wxTreeEvent& evt);
	void OnNew(wxCommandEvent& evt);
	void OnDelete(wxCommandEvent& evt);
	void OnEdit(wxCommandEvent& evt);
	void OnCopy(wxCommandEvent& evt);
	void OnUpdate(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
