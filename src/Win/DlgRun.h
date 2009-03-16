#pragma once

/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgRun class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-09 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "ARBTypes.h"
#include "ColumnOrder.h"
class CAgilityBookDoc;
class CMetaDataDisplay;
class CReportListCtrl;

class CDlgDogRefRunData;
typedef tr1::shared_ptr<CDlgDogRefRunData> CDlgDogRefRunDataPtr;


class CDlgRun : public wxDialog
{
public:
	CDlgRun(
			CAgilityBookDoc* pDoc,
			ARBDogTrialPtr pTrial,
			ARBDogRunPtr pRun,
			wxWindow* pParent = NULL,
			int iSelectPage = 0);

	CDlgDogRefRunDataPtr GetReferenceDataByData(long index) const;
private:
	void SetFaultsText();

	CDlgDogRefRunDataPtr GetReferenceData(long index) const;
	bool IsRefRunMe();
	void CreateRefRunMe();
	void UpdateRefRunButtons();
	void SetRefRunColumnHeaders();
	void ListRefRuns();
	void GetAllHeights(std::set<tstring>& outNames);
	void GetAllCallNames(std::set<tstring>& outNames);
	void GetAllBreeds(std::set<tstring>& outNames);
	void EditRefRun();

	void UpdateCRCDButtons();

	void UpdateLinksButtons();
	void ListLinkFiles(wxChar const* pItem);
	int GetImageIndex(tstring const& inLink);
	void EditLink();

	CAgilityBookDoc* m_pDoc;
	ARBDogTrialPtr m_pTrial;
	ARBDogRunPtr m_pRealRun;
	ARBDogRunPtr m_Run;
	ARBDogReferenceRunPtr m_pRefRunMe;
	ARBDogClubPtr m_Club;
	ARBConfigVenuePtr m_Venue;

	wxString m_Comments;
	CColumnOrder m_sortRefRuns;

	int m_idxRefRunPage;
	wxListBox* m_ctrlFaultsList;
	CReportListCtrl* m_ctrlRefRuns;
	wxButton* m_ctrlRefAddMe;
	wxButton* m_ctrlRefEdit;
	wxButton* m_ctrlRefDelete;

	CMetaDataDisplay* m_CRCDDisplay;
	wxButton* m_ctrlCourse;
	wxButton* m_ctrlCRCDView;
	wxButton* m_ctrlCRCDCopy;
	wxCheckBox* m_ctrlIncImage;

	wxListView* m_ctrlLinks;
	wxImageList m_ImageList;
	int m_imgEmpty;
	int m_imgOk;
	int m_imgMissing;
	wxButton* m_ctrlLinkEdit;
	wxButton* m_ctrlLinkDelete;
	wxButton* m_ctrlLinkOpen;

	DECLARE_EVENT_TABLE()
	void OnPageChanged(wxNotebookEvent& evt);
	void OnOk(wxCommandEvent& evt);
	// Score
	// Comments
	void OnCommentsFaults(wxCommandEvent& evt);
	// RefRuns
	void OnRefRunColumnClick(wxListEvent& evt);
	void OnRefRunItemSelected(wxListEvent& evt);
	void OnRefRunDoubleClick(wxMouseEvent& evt);
	void OnRefRunKeyDown(wxListEvent& evt);
	void OnRefRunNew(wxCommandEvent& evt);
	void OnRefRunAddMe(wxCommandEvent& evt);
	void OnRefRunEdit(wxCommandEvent& evt);
	void OnRefRunDelete(wxCommandEvent& evt);
	// CRCD
	void OnCRCDEdit(wxCommandEvent& evt);
	void OnCRCDView(wxCommandEvent& evt);
	void OnCRCDCopy(wxCommandEvent& evt);
	void OnCRCDImage(wxCommandEvent& evt);
	// Links
	void OnLinksItemSelected(wxListEvent& evt);
	void OnLinksDoubleClick(wxMouseEvent& evt);
	void OnLinksKeyDown(wxListEvent& evt);
	void OnLinksNew(wxCommandEvent& evt);
	void OnLinksEdit(wxCommandEvent& evt);
	void OnLinksDelete(wxCommandEvent& evt);
	void OnLinksOpen(wxCommandEvent& evt);
};
