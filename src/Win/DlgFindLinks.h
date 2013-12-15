#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgFindLinks class
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-10 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-06-02 Added 'Open' button.
 * 2004-03-31 Created.
 */

#include "ColumnOrder.h"
#include "ARB/ARBTypes2.h"
#include <vector>
class CReportListCtrl;
class wxListEvent;

class CDlgFindLinksData;
typedef std::tr1::shared_ptr<CDlgFindLinksData> CDlgFindLinksDataPtr;


class CDlgFindLinks : public wxDialog
{
#if wxCHECK_VERSION(3, 0, 0)
	friend int wxCALLBACK CompareLinks(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData);
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
	int GetImageIndex(std::wstring const& inLink);
	void SetColumnHeaders();
	void UpdateButtons();
	void Edit();

	CReportListCtrl* m_ctrlLinks;
	wxButton* m_ctrlEdit;
	wxButton* m_ctrlOpen;

	CColumnOrder m_sortLinks;
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
