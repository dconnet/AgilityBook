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
 * @brief interface of the CDlgExistingPoints class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-02-03 DRC Created
 */

#include "ARBDate.h"
#include "ARBDogExistingPoints.h"
class CAgilityBookDoc;
class CDlgPointsDivisionData;
class CDlgPointsEventData;
class CDlgPointsLevelData;
class CDlgPointsMultiQData;
class CDlgPointsOtherPtData;
class CDlgPointsVenueData;
class wxDateEvent;
class wxDatePickerCtrl;


class CDlgExistingPoints : public wxDialog
{
public:
	CDlgExistingPoints(
			CAgilityBookDoc* pDoc,
			ARBDogExistingPointsList& points,
			ARBDogExistingPointsPtr pExistingPoints,
			wxWindow* pParent = NULL);

private:
	ARBDogExistingPoints::PointType GetCurrentType() const;
	CDlgPointsVenueData* GetVenueData(int index) const;
	CDlgPointsDivisionData* GetDivisionData(int index) const;
	CDlgPointsLevelData* GetLevelData(int index) const;
	CDlgPointsEventData* GetEventData(int index) const;
	CDlgPointsOtherPtData* GetOtherPointData(int index) const;
	CDlgPointsMultiQData* GetMultiQData(int index) const;
	void SetEnableLists(
			bool& outVenue,
			bool& outDivMQ,
			bool& outLevel,
			bool& outEvent,
			bool& outSubName,
			bool& outOther,
			bool bSet);
	void UpdateControls();
	void FillVenues();
	void FillDivMultiQ();
	void FillLevels();
	void FillEvents();
	void FillSubNames();

	CAgilityBookDoc* m_pDoc;
	ARBDogExistingPointsList& m_PointsList;
	ARBDogExistingPointsPtr m_pExistingPoints;
	ARBDate m_Date;
	double m_Points;
	wxString m_Comments;
	wxDatePickerCtrl* m_ctrlDate;
	wxComboBox* m_ctrlType;
	wxComboBox* m_ctrlVenues;
	wxString m_TextVenue;
	wxStaticText* m_textDivMultiQs;
	wxComboBox* m_ctrlDivMultiQs;
	wxString m_TextDivMultiQ;
	wxComboBox* m_ctrlLevels;
	wxString m_TextLevel;
	wxComboBox* m_ctrlEvents;
	wxString m_TextEvent;
	wxComboBox* m_ctrlSubNames;
	wxString m_TextSubName;
	wxComboBox* m_ctrlOthers;
	wxString m_TextOther;
	wxSizer* m_sdbSizer;

	DECLARE_EVENT_TABLE()
	void OnDateChanged(wxDateEvent& evt);
	void OnSelchangeType(wxCommandEvent& evt);
	void OnSelchangeVenue(wxCommandEvent& evt);
	void OnSelchangeDivMultiQ(wxCommandEvent& evt);
	void OnSelchangeLevel(wxCommandEvent& evt);
	void OnSelchangeEvent(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
