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
 * @brief interface of the CDlgFindLinks class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-10 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-06-02 DRC Added 'Open' button.
 * @li 2004-03-31 DRC Created.
 */

#include "ARBTypes.h"
#include "ColumnOrder.h"
#include <vector>
class CReportListCtrl;
class wxListEvent;

class CDlgFindLinksData;
typedef tr1::shared_ptr<CDlgFindLinksData> CDlgFindLinksDataPtr;


class CDlgFindLinks : public wxDialog
{
	friend int wxCALLBACK CompareLinks(long item1, long item2, long sortData);
public:
	CDlgFindLinks(
			ARBDogList& inDogs,
			wxWindow* pParent = NULL);

	size_t GetNumLinks() const		{return m_Data.size();}

private:
	CDlgFindLinksDataPtr GetItemLinkDataByData(long data);
	int GetImageIndex(tstring const& inLink);
	void SetColumnHeaders();
	void UpdateButtons();
	void Edit();

	CReportListCtrl* m_ctrlLinks;
	wxButton* m_ctrlEdit;
	wxButton* m_ctrlOpen;

	CColumnOrder m_sortLinks;
	int m_imgEmpty;
	int m_imgOk;
	int m_imgMissing;
	std::vector<CDlgFindLinksDataPtr> m_Data;

	DECLARE_EVENT_TABLE()
	void OnColumnClick(wxListEvent& evt);
	void OnItemSelected(wxListEvent& evt);
	void OnDoubleClick(wxMouseEvent& evt);
	void OnKeyDown(wxKeyEvent& evt);
	void OnCopy(wxCommandEvent& evt);
	void OnEdit(wxCommandEvent& evt);
	void OnOpen(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
