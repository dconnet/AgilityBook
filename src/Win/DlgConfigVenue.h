#pragma once

/*
 * Copyright © 2004-2009 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgConfigVenue class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-01-24 DRC Created
 */

#include "ARBConfigScoring.h"
class ARBAgilityRecordBook;
class CDlgConfigureDataBase;
class wxTreeCtrl;
class wxTreeEvent;
class wxTreeItemId;


class CDlgConfigVenue : public wxDialog
{
	friend class CDlgConfigureDataDivision;
	friend class CDlgConfigureDataEvent;
	friend class CDlgConfigureDataLevel;
	friend class CDlgConfigureDataMultiQ;
	friend class CDlgConfigureDataSubLevel;
	friend class CDlgConfigureDataTitle;
	friend class CDlgConfigVenueDataRoot;

public:
	CDlgConfigVenue(
			ARBAgilityRecordBook const& book,
			ARBConfig const& config,
			ARBConfigVenuePtr pVenue,
			wxWindow* pParent = NULL);
	~CDlgConfigVenue();

	void GetFixups(ARBConfigActionList& ioFixups);

	typedef enum
	{
		eNone,
		eDivisions,
		eEvents,
		eMultiQ,
		eTitles,
	} eAction;

private:
	CDlgConfigureDataBase* GetData(wxTreeItemId item) const;
	CDlgConfigureDataBase* GetCurrentData(wxTreeItemId* pItem = NULL) const;
	void UpdateButtons();
	/*
	int FindCurrentDivision(
			ARBConfigDivisionPtr pDiv,
			bool bSet);
	HTREEITEM FindCurrentLevel(
			ARBConfigLevelPtr pLevel,
			bool bSet);
	HTREEITEM FindCurrentSubLevel(
			ARBConfigSubLevelPtr pSubLevel,
			bool bSet);
	int FindCurrentTitle(
			ARBConfigTitlePtr pTitle,
			bool bSet);
	int FindCurrentEvent(
			ARBConfigEventPtr pEvent,
			bool bSet);
	int FindCurrentMultiQ(
			ARBConfigMultiQPtr pMultiQ,
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
	wxString m_LifetimeName;
	wxString m_Desc;
	wxTreeCtrl* m_ctrlItems;
	wxButton* m_ctrlNew;
	wxButton* m_ctrlEdit;
	wxButton* m_ctrlDelete;
	wxButton* m_ctrlCopy;
	wxButton* m_ctrlMoveUp;
	wxButton* m_ctrlMoveDown;

	DECLARE_EVENT_TABLE()
	void OnSelectionChanged(wxTreeEvent& evt);
	void OnDblclk(wxMouseEvent& evt);
	void OnKeydown(wxTreeEvent& evt);
	void OnNew(wxCommandEvent& evt);
	void OnDelete(wxCommandEvent& evt);
	void OnEdit(wxCommandEvent& evt);
	void OnCopy(wxCommandEvent& evt);
	void OnMoveUp(wxCommandEvent& evt);
	void OnMoveDown(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
