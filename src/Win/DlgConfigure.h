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
 * @brief interface of the CDlgConfigure class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-10 DRC Ported to wxWidgets.
 * @li 2007-08-19 DRC Simplified UI layout into a single tree.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "ARBConfig.h"
#include "ARBTypes.h"
#include "IconList.h"
#include <wx/treectrl.h>
class ARBAgilityRecordBook;
class CAgilityBookDoc;
class CDlgConfigureDataBase;


class CDlgConfigure : public wxDialog
{
public:
	CDlgConfigure(
			CAgilityBookDoc* pDoc,
			ARBAgilityRecordBook& book,
			wxWindow* pParent = NULL);
	~CDlgConfigure();

	typedef enum
	{
		eNone = 0,
		eVenues,
		eFaults,
		eOtherPoints
	} eAction;

private:
	eAction GetAction() const;
	CDlgConfigureDataBase* GetData(wxTreeItemId hItem) const;
	void UpdateButtons();
	void LoadData(eAction dataToLoad);
	void DoEdit();
	wxTreeItemId FindCurrentVenue(
			ARBConfigVenuePtr pVenue,
			bool bSet);
	wxTreeItemId FindCurrentFault(
			ARBConfigFaultPtr pFault,
			bool bSet);
	wxTreeItemId FindCurrentOtherPoints(
			ARBConfigOtherPointsPtr pOther,
			bool bSet);

	CAgilityBookDoc* m_pDoc;
	ARBAgilityRecordBook& m_Book;
	ARBConfig m_Config;
	CIconList m_ImageList;
	int m_idxFaults;
	wxTreeCtrl* m_ctrlItems;
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
