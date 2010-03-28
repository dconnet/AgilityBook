#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgFindLinks class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
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
typedef std::tr1::shared_ptr<CDlgFindLinksData> CDlgFindLinksDataPtr;


class CDlgFindLinks : public wxDialog
{
#if wxCHECK_VERSION(2,9,1)
	friend int wxCALLBACK CompareLinks(long item1, long item2, wxIntPtr sortData);
#else
	friend int wxCALLBACK CompareLinks(long item1, long item2, long sortData);
#endif
public:
	CDlgFindLinks(
			ARBDogList& inDogs,
			wxWindow* pParent = NULL);

	size_t GetNumLinks() const		{return m_Data.size();}

private:
	CDlgFindLinksDataPtr GetItemLinkData(long item);
	CDlgFindLinksDataPtr GetItemLinkDataByData(long data);
	int GetImageIndex(wxString const& inLink);
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
	void OnItemActivated(wxListEvent& evt);
	void OnKeyDown(wxKeyEvent& evt);
	void OnCopy(wxCommandEvent& evt);
	void OnEdit(wxCommandEvent& evt);
	void OnOpen(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
