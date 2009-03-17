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
 * @brief implementation of the CDlgRun class
 * @author David Connet
 *
 * Revision History
 * @li 2009-03-16 DRC Merged DlgRun* into here.
 * @li 2009-02-09 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2003-10-13 DRC Make ref run dlg default to perfect score.
 * DlgRunCRCD
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-05-04 DRC Added ability to suppress metafile.
 * @li 2004-04-27 DRC Added some error recovery.
 * DlgRunComments
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * DlgRunLink
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-06-02 DRC Moved ShellExecute code to AgilityBook.cpp, added icons.
 * @li 2004-03-30 DRC Created
 * DlgRunReference
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-09-20 DRC Added yourself was not getting up-to-date scoring info.
 * @li 2005-07-10 DRC Add button to add yourself to ref-runs.
 *                    Make default ref-run a 'Q'.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Accumulate all heights for refrun dlg.
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 * @li 2003-10-13 DRC Make ref run dlg default to perfect score.
 * DlgRunScore
 * @li 2008-02-01 DRC Make 'Notes' button change selection.
 * @li 2008-11-21 DRC Enable tallying runs that have only lifetime points.
 * @li 2007-12-03 DRC Refresh judge list after invoking 'notes' button.
 * @li 2007-07-01 DRC Fixed a problem with table flag on a run.
 * @li 2006-11-05 DRC Trim Divisions/Levels if no events are available on date.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-13 DRC Added direct access to Notes dialog.
 * @li 2005-12-04 DRC Added support for NADAC bonus titling points.
 * @li 2005-11-20 DRC Allow 'E's on non-titling runs.
 * @li 2005-08-11 DRC Removed QQ checkbox.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-02 DRC Added subnames to events.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-11-13 DRC Also compute score for NA runs that have no titling pts.
 * @li 2004-09-07 DRC Time+Fault scoring shouldn't include time faults.
 * @li 2004-03-20 DRC The date never got set if the initial entry had no date
 *                    and we didn't change it (first run in a trial).
 *                    Plus, the table-in-yps flag was backwards and didn't
 *                    properly initialize when the event type changed.
 * @li 2004-02-14 DRC Added Table-in-YPS flag.
 * @li 2004-02-09 DRC Update YPS when the time is changed.
 *                    When date changes, update controls.
 * @li 2004-01-19 DRC Total faults weren't updated when course faults changed.
 * @li 2004-01-18 DRC Calling UpdateData during data entry causes way too much
 *                    validation. Only call during OnOK (from dlgsheet)
 * @li 2003-12-27 DRC Changed FindEvent to take a date. Also, update the
 *                    controls when the date changes as the scoring config may
 *                    change.
 * @li 2003-12-09 DRC Fixed a bug where the QQ checkbox didn't get set right.
 * @li 2003-10-13 DRC Made Time/CourseFaults common to all scoring methods.
 *                    This allows faults for things like language!
 * @li 2003-09-29 DRC Required pts were being overwriten with default values
 *                    during dialog initialization.
 * @li 2003-09-01 DRC Total faults weren't being shown when there was no SCT.
 * @li 2003-08-17 DRC Title points were being computed on 'NQ' and the score was
 *                    always being computed. Fixed both.
 * @li 2003-07-14 DRC Changed 'Score' to show data on 'Q' and 'NQ'.
 */

#include "stdafx.h"
#include "DlgRun.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ARBConfig.h"
#include "ARBConfigOtherPoints.h"
#include "ARBConfigVenue.h"
#include "ARBDogClub.h"
#include "ARBDogRun.h"
#include "ARBDogTrial.h"
#include "CheckLink.h"
#include "ClipBoard.h"
#include "ComboBoxes.h"
#include "DlgCRCDViewer.h"
#include "DlgInfoNote.h"
#include "DlgListCtrl.h"
#include "DlgReferenceRun.h"
#include "DlgSelectURL.h"
#include "Globals.h"
#include "ListCtrl.h"
#include "ListData.h"
#include "NoteButton.h"
#include "RichEditCtrl2.h"
#include "Validators.h"
#include <set>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/dcbuffer.h>

#include "res/CalEmpty.xpm"
#include "res/CalPlan.xpm"
#include "res/CalTentative.xpm"

#ifdef WIN32
// You can currently only enable this on Win32. I directly use the win32
// apis to play the metafile into a dc. In theory, I could use wxWidgets
// to play them - but I still need the direct win32 access to copy a
// metafile from the clipboard - wxWidgets only has support to copy >to<
// the clipboard.
#define HAS_ENHMETAFILE
#endif

/////////////////////////////////////////////////////////////////////////////

static struct
{
	int fmt;
	wxChar const* idText;
} const scRefRunColumns[] = {
	{wxLIST_FORMAT_CENTRE, _("IDS_COL_Q")},
	{wxLIST_FORMAT_CENTRE, _("IDS_COL_PLACE")},
	{wxLIST_FORMAT_CENTRE, _("IDS_COL_SCORE")},
	{wxLIST_FORMAT_LEFT, _("IDS_COL_TIME")},
	{wxLIST_FORMAT_LEFT, _("IDS_COL_NAME")},
	{wxLIST_FORMAT_LEFT, _("IDS_COL_HEIGHT")},
	{wxLIST_FORMAT_LEFT, _("IDS_COL_BREED")},
	{wxLIST_FORMAT_LEFT, _("IDS_COL_NOTE")},
};
static int const scNumRefRunColumns = sizeof(scRefRunColumns) / sizeof(scRefRunColumns[0]);

static struct SORTINFO
{
	CDlgRun* pThis;
	CColumnOrder* pCols;
} s_SortInfo;

/////////////////////////////////////////////////////////////////////////////

class CDlgDogDivData : public wxClientData
{
public:
	CDlgDogDivData(ARBConfigDivisionPtr div)
		: m_Div(div)
	{
	}
	ARBConfigDivisionPtr m_Div;
};


class CDlgDogLevelData : public wxClientData
{
public:
	CDlgDogLevelData(ARBConfigLevelPtr pLevel)
		: m_pLevel(pLevel)
		, m_pSubLevel()
	{
	}
	CDlgDogLevelData(
			ARBConfigLevelPtr pLevel,
			ARBConfigSubLevelPtr pSubLevel)
		: m_pLevel(pLevel)
		, m_pSubLevel(pSubLevel)
	{
	}
	ARBConfigLevelPtr m_pLevel;
	ARBConfigSubLevelPtr m_pSubLevel;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgDogRefRunData : public CListData
{
public:
	CDlgDogRefRunData(ARBDogReferenceRunPtr refRun)
		: m_RefRun(refRun)
	{
	}
	virtual wxString OnNeedText(long iCol) const;
	ARBDogReferenceRunPtr GetData() const		{return m_RefRun;}
	void SetData(ARBDogReferenceRunPtr data)	{m_RefRun = data;}
private:
	ARBDogReferenceRunPtr m_RefRun;
};


wxString CDlgDogRefRunData::OnNeedText(long iCol) const
{
	wxString str;
	switch (iCol)
	{
	default:
		break;
	case 0: // Q
		str = m_RefRun->GetQ().str().c_str();
		break;
	case 1: // Place
		{
			otstringstream tmp;
			tmp << m_RefRun->GetPlace();
			str = tmp.str().c_str();
		}
		break;
	case 2: // Score
		str = m_RefRun->GetScore().c_str();
		break;
	case 3: // Time
		str = ARBDouble::str(m_RefRun->GetTime()).c_str();
		break;
	case 4: // Name
		str = m_RefRun->GetName().c_str();
		break;
	case 5: // Height
		str = m_RefRun->GetHeight().c_str();
		break;
	case 6: // Breed
		str = m_RefRun->GetBreed().c_str();
		break;
	case 7: // Note
		str = m_RefRun->GetNote().c_str();
		str.Replace(wxT("\n"), wxT(" "));
		break;
	}
	return str;
}


int wxCALLBACK CompareRefRuns(long item1, long item2, long sortData)
{
	CDlgDogRefRunDataPtr pData1 = s_SortInfo.pThis->GetReferenceDataByData(item1);
	CDlgDogRefRunDataPtr pData2 = s_SortInfo.pThis->GetReferenceDataByData(item2);
	ARBDogReferenceRunPtr pTitle1 = pData1->GetData();
	ARBDogReferenceRunPtr pTitle2 = pData2->GetData();
	int rc = 0;
	for (int i = 0; i < s_SortInfo.pCols->GetSize(); ++i)
	{
		int col = s_SortInfo.pCols->GetColumnAt(i);
		switch (col)
		{
		default:
		case 3: // Time
			if (pTitle1->GetTime() < pTitle2->GetTime())
				rc = -1;
			else if (pTitle1->GetTime() > pTitle2->GetTime())
				rc = 1;
			break;
		case 0: // Q
			if (pTitle1->GetQ() < pTitle2->GetQ())
				rc = -1;
			else if (pTitle1->GetQ() > pTitle2->GetQ())
				rc = 1;
			break;
		case 1: // Place
			if (pTitle1->GetPlace() < pTitle2->GetPlace())
				rc = -1;
			else if (pTitle1->GetPlace() > pTitle2->GetPlace())
				rc = 1;
			break;
		case 2: // Score
			if (pTitle1->GetScore() < pTitle2->GetScore())
				rc = -1;
			else if (pTitle1->GetScore() > pTitle2->GetScore())
				rc = 1;
			break;
		case 4: // Name
			if (pTitle1->GetName() < pTitle2->GetName())
				rc = -1;
			else if (pTitle1->GetName() > pTitle2->GetName())
				rc = 1;
			break;
		case 5: // Height
			if (pTitle1->GetHeight() < pTitle2->GetHeight())
				rc = -1;
			else if (pTitle1->GetHeight() > pTitle2->GetHeight())
				rc = 1;
			break;
		case 6: // Breed
			if (pTitle1->GetBreed() < pTitle2->GetBreed())
				rc = -1;
			else if (pTitle1->GetBreed() > pTitle2->GetBreed())
				rc = 1;
			break;
		case 7: // Notes
			if (pTitle1->GetNote() < pTitle2->GetNote())
				rc = -1;
			else if (pTitle1->GetNote() > pTitle2->GetNote())
				rc = 1;
			break;
		}
		if (rc)
		{
			if (s_SortInfo.pCols->IsDescending(col))
				rc *= -1;
			break;
		}
	}
	return rc;
}

/////////////////////////////////////////////////////////////////////////////

class CMetaDataDisplay : public wxTextCtrl
{
public:
	CMetaDataDisplay(
			wxWindow* parent,
			ARBDogRunPtr pRun);
	~CMetaDataDisplay();

	bool HasMetafileSupport() const
	{
#ifdef HAS_ENHMETAFILE
		return true;
#else
		return false;
#endif
	}
	bool HasMetafile()
	{
#ifdef HAS_ENHMETAFILE
		return NULL != m_metaFile;
#else
		return false;
#endif
	}
	bool ViewText() const		{return m_ViewText;}
	bool HasCourse() const		{return !m_Insert;}
	void SetCRCDData();
	void ToggleView();
	void OnCopy();
	void OnCRCDImage(bool checked);
	void Display();

private:
	void DeleteMetaFile();

	ARBDogRunPtr m_Run;
#ifdef HAS_ENHMETAFILE
	HENHMETAFILE m_metaFile;
	HENHMETAFILE m_metaFileBack;
#endif
	bool m_ViewText;
	bool m_Insert;

	DECLARE_EVENT_TABLE()
	void OnPaint(wxPaintEvent& evt);
};


BEGIN_EVENT_TABLE(CMetaDataDisplay, wxTextCtrl)
	EVT_PAINT(CMetaDataDisplay::OnPaint)
END_EVENT_TABLE()


CMetaDataDisplay::CMetaDataDisplay(
		wxWindow* parent,
		ARBDogRunPtr pRun)
	: m_Run(pRun)
#ifdef HAS_ENHMETAFILE
	, m_metaFile(NULL)
	, m_metaFileBack(NULL)
#endif
	, m_ViewText(true)
	, m_Insert(pRun->GetCRCD().empty())
{
	wxTextCtrl::Create(parent, wxID_ANY,
		m_Run->GetCRCD().c_str(),
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY);

#ifdef HAS_ENHMETAFILE
	if (0 < m_Run->GetCRCDRawMetaData().length())
	{
		ARBMetaDataPtr metaData = m_Run->GetCRCDMetaData();
		m_metaFile = SetEnhMetaFileBits(static_cast<UINT>(metaData->length()), reinterpret_cast<BYTE const*>(metaData->data()));
		m_metaFileBack = m_metaFile;
		if (m_metaFile)
		{
			m_ViewText = false;
			Enable(m_ViewText);
		}
	}
#endif
}


CMetaDataDisplay::~CMetaDataDisplay()
{
	DeleteMetaFile();
}


void CMetaDataDisplay::DeleteMetaFile()
{
#ifdef HAS_ENHMETAFILE
	if (!m_metaFile && m_metaFileBack)
		m_metaFile = m_metaFileBack;
	if (m_metaFile)
	{
		DeleteEnhMetaFile(m_metaFile);
		m_metaFile = NULL;
		m_metaFileBack = NULL;
	}
#endif
}


void CMetaDataDisplay::SetCRCDData()
{
#ifdef HAS_ENHMETAFILE
	if (m_metaFile)
	{
		ENHMETAHEADER header;
		GetEnhMetaFileHeader(m_metaFile, sizeof(header), &header);
		UINT nSize = GetEnhMetaFileBits(m_metaFile, 0, NULL);
		LPBYTE bits = new BYTE[nSize+1];
		GetEnhMetaFileBits(m_metaFile, nSize, bits);
		assert(sizeof(BYTE) == sizeof(char));
		m_Run->SetCRCDMetaData(bits, nSize);
		delete [] bits;
	}
	else
#endif
		m_Run->SetCRCDMetaData(NULL, 0);
}


void CMetaDataDisplay::ToggleView()
{
	m_ViewText = !m_ViewText;
	Enable(m_ViewText);
	Refresh();
}


void CMetaDataDisplay::OnCopy()
{
	if (m_Insert)
	{
		bool bText = false;
		bText = ::IsClipboardFormatAvailable(CF_TEXT) ? true : false;
		bool bMeta = false;
#ifdef HAS_ENHMETAFILE
		bMeta = ::IsClipboardFormatAvailable(CF_ENHMETAFILE) ? true : false;
		if (!CAgilityBookOptions::GetIncludeCRCDImage())
			bMeta = FALSE;
#endif
		if (bText || bMeta)
		{
			CClipboardDataReader clpData;
			if (clpData.Open())
			{
				SetLabel(wxT(""));
				DeleteMetaFile();
				if (bText)
				{
					m_ViewText = true;
					wxString str;
					clpData.GetData(str);
					str.Trim(true);
					str.Trim(false);
					// We do the replace since CRCD3 has "\n\nhdrs\r\netc"
					// Standardize to \n.
					str.Replace(wxT("\r\n"), wxT("\n"));
					m_Run->SetCRCD(str.c_str());
					SetLabel(str);
					if (0 < str.length())
						m_Insert = false;
					// Only create the metafile if we pasted text. Otherwise
					// we could end up with a non-CRCD metafile. It's still
					// possible this may not be CRCD data - the user can
					// just clear it then.
#ifdef HAS_ENHMETAFILE
					if (bMeta)
					{
						HENHMETAFILE hData = reinterpret_cast<HENHMETAFILE>(GetClipboardData(CF_ENHMETAFILE));
						m_metaFile = CopyEnhMetaFile(hData, NULL);
						m_metaFileBack = m_metaFile;
						SetCRCDData();
						if (m_metaFile)
						{
							m_ViewText = false;
							m_Insert = false;
						}
					}
#endif
				}
			}
		}
	}
	else
	{
		m_Insert = true;
		m_ViewText = true;
		SetLabel(wxT(""));
		DeleteMetaFile();
		m_Run->SetCRCD(wxT(""));
		m_Run->SetCRCDMetaData(NULL, 0);
	}
	Enable(m_ViewText);
	Refresh();
}


void CMetaDataDisplay::OnCRCDImage(bool checked)
{
#ifdef HAS_ENHMETAFILE
	CAgilityBookOptions::SetIncludeCRCDImage(checked);
	if (checked)
	{
		if (!m_metaFile && m_metaFileBack)
			m_metaFile = m_metaFileBack;
	}
	else
	{
		if (m_metaFile)
		{
			m_metaFileBack = m_metaFile;
			m_metaFile = NULL;
		}
	}
	SetCRCDData();
	if (m_metaFile)
	{
		m_ViewText = false;
		m_Insert = false;
	}
	else
		m_ViewText = true;
	Enable(m_ViewText);
	Refresh();
#endif
}


void CMetaDataDisplay::Display()
{
#ifdef HAS_ENHMETAFILE
	if (m_metaFile)
	{
		CDlgCRCDViewer viewer(m_metaFile, this);
		viewer.ShowModal();
	}
#endif
}


void CMetaDataDisplay::OnPaint(wxPaintEvent& evt)
{
	if (m_ViewText)
		evt.Skip(true);
	else
	{
		wxBufferedPaintDC dc(this);
		wxBrush br(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
		dc.SetBrush(br);
		dc.Clear();
		dc.SetBrush(wxNullBrush);
#ifdef HAS_ENHMETAFILE
		wxSize sz = GetClientSize();
		RECT r;
		r.left = 0;
		r.top = 0;
		r.right = std::min(sz.x, sz.y);
		r.bottom = std::min(sz.x, sz.y);
		HDC hdc = (HDC)dc.GetHDC();
		PlayEnhMetaFile(hdc, m_metaFile, &r);
#endif
	}
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(CDlgRun, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgRun::OnOk)
END_EVENT_TABLE()


CDlgRun::CDlgRun(
		CAgilityBookDoc* pDoc,
		ARBDogTrialPtr pTrial,
		ARBDogRunPtr pRun,
		wxWindow* pParent,
		int iSelectPage)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_pTrial(pTrial)
	, m_pRealRun(pRun)
	, m_Run(pRun->Clone())
	, m_pRefRunMe()
	, m_Club()
	, m_pVenue()

	, m_Date(pRun->GetDate())

	, m_ctrlDivisions(NULL)
	, m_ctrlLevels(NULL)
	, m_ctrlEvents(NULL)
	, m_ctrlSubNamesText(NULL)
	, m_ctrlSubNames(NULL)
	, m_SubName(pRun->GetSubName().c_str())
	, m_Height(pRun->GetHeight().c_str())
	, m_Judge(pRun->GetJudge().c_str())
	, m_Handler(pRun->GetHandler().c_str())
	, m_Conditions(pRun->GetConditions().c_str())
	
	, m_Comments(pRun->GetNote().c_str())
	, m_sortRefRuns(wxT("RefRuns"))

	, m_idxRefRunPage(-1)
	, m_ctrlFaultsList(NULL)
	, m_ctrlRefRuns(NULL)
	, m_ctrlRefAddMe(NULL)
	, m_ctrlRefEdit(NULL)
	, m_ctrlRefDelete(NULL)

	, m_CRCDDisplay(NULL)
	, m_ctrlCourse(NULL)
	, m_ctrlCRCDView(NULL)
	, m_ctrlCRCDCopy(NULL)
	, m_ctrlIncImage(NULL)

	, m_ctrlLinks(NULL)
	, m_ImageList(16,16)
	, m_imgEmpty(-1)
	, m_imgOk(-1)
	, m_imgMissing(-1)
	, m_ctrlLinkEdit(NULL)
	, m_ctrlLinkDelete(NULL)
	, m_ctrlLinkOpen(NULL)
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP|wxWS_EX_VALIDATE_RECURSIVELY);
	Create(pParent, wxID_ANY, _("IDS_RUN_PROPERTIES"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	pTrial->GetClubs().GetPrimaryClub(&m_Club);
	assert(NULL != m_Club.get());
	pDoc->Book().GetConfig().GetVenues().FindVenue(m_Club->GetVenue(), &m_pVenue);
	assert(NULL != m_pVenue.get());

	m_sortRefRuns.Initialize(scNumRefRunColumns);

	if (!m_Run->GetDate().IsValid())
		m_Date.SetToday();
	if (m_Height.empty())
	{
		wxString last = CAgilityBookOptions::GetLastEnteredHeight();
		if (!last.empty())
			m_Height = last;
	}
	if (m_Judge.empty())
	{
		wxString last = CAgilityBookOptions::GetLastEnteredJudge();
		if (!last.empty())
			m_Judge = last;
	}
	if (m_Handler.empty())
	{
		wxString last = CAgilityBookOptions::GetLastEnteredHandler();
		if (!last.empty())
			m_Handler = last;
	}

	//m_pageScore = new CDlgRunScore(pDoc, m_pVenue, pTrial, m_pRealRun, m_Run);
	//m_pageComments = new CDlgRunComments(pDoc, m_Run);
	//m_pageReference = new CDlgRunReference(pDoc, m_pVenue, m_Run);
	//m_pageCRCD = new CDlgRunCRCD(m_Run);
	//m_pageLink = new CDlgRunLink(pDoc, m_Run);

	// Controls (these are done first to control tab order)

	wxNotebook* notebook = new wxNotebook(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, 0);
	notebook->Connect(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler(CDlgRun::OnPageChanged), NULL, this);

	// Score

	wxPanel* panelScore = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxStaticText* textDate = new wxStaticText(panelScore, wxID_ANY,
		wxT("Date"), wxDefaultPosition, wxDefaultSize, 0);
	textDate->Wrap(-1);

	wxDatePickerCtrl* ctrlDate = new wxDatePickerCtrl(panelScore, wxID_ANY, wxDefaultDateTime,
		wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN|wxDP_SHOWCENTURY,
		CGenericValidator(&m_Date));
	ctrlDate->Connect(wxEVT_DATE_CHANGED, wxDateEventHandler(CDlgRun::OnScoreDateChanged), NULL, this);
	ctrlDate->SetHelpText(_("HIDC_RUNSCORE_DATE"));
	ctrlDate->SetToolTip(_("HIDC_RUNSCORE_DATE"));

	wxStaticText* textVenue = new wxStaticText(panelScore, wxID_ANY,
		m_pVenue->GetName().c_str(),
		wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	textVenue->Wrap(-1);

	wxStaticText* textClub = new wxStaticText(panelScore, wxID_ANY,
		m_Club->GetName().c_str(),
		wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	textClub->Wrap(-1);

	wxStaticText* textLocation = new wxStaticText(panelScore, wxID_ANY,
		pTrial->GetLocation().c_str(),
		wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	textLocation->Wrap(-1);

	wxStaticText* textDiv = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_DIVISION"),
		wxDefaultPosition, wxDefaultSize, 0);
	textDiv->Wrap(-1);

	m_ctrlDivisions = new wxComboBox(panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		0, NULL, wxCB_DROPDOWN);
	m_ctrlDivisions->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(CDlgRun::OnSelchangeDivision), NULL, this);
	m_ctrlDivisions->SetHelpText(_("HIDC_RUNSCORE_DIVISION"));
	m_ctrlDivisions->SetToolTip(_("HIDC_RUNSCORE_DIVISION"));

	wxStaticText* textLevel = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_LEVEL"),
		wxDefaultPosition, wxDefaultSize, 0);
	textLevel->Wrap(-1);

	m_ctrlLevels = new wxComboBox(panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		0, NULL, wxCB_DROPDOWN);
	m_ctrlLevels->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(CDlgRun::OnSelchangeLevel), NULL, this);
	m_ctrlLevels->SetHelpText(_("HIDC_RUNSCORE_LEVEL"));
	m_ctrlLevels->SetToolTip(_("HIDC_RUNSCORE_LEVEL"));

	wxStaticText* textEvent = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_EVENT"),
		wxDefaultPosition, wxDefaultSize, 0);
	textEvent->Wrap(-1);

	m_ctrlEvents = new wxComboBox(panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		0, NULL, wxCB_DROPDOWN);
	m_ctrlEvents->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(CDlgRun::OnSelchangeEvent), NULL, this);
	m_ctrlEvents->SetHelpText(_("HIDC_RUNSCORE_EVENT"));
	m_ctrlEvents->SetToolTip(_("HIDC_RUNSCORE_EVENT"));

	m_ctrlSubNamesText = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_SUBNAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlSubNamesText->Wrap(-1);

	m_ctrlSubNames = new wxComboBox(panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		0, NULL, wxCB_DROPDOWN,
		CTrimValidator(&m_SubName, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlSubNames->SetHelpText(_("HIDC_RUNSCORE_SUBNAME"));
	m_ctrlSubNames->SetToolTip(_("HIDC_RUNSCORE_SUBNAME"));

	m_ctrlTable = new wxCheckBox(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_TABLE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlTable->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CDlgRun::OnBnClickedTableYps), NULL, this);
	m_ctrlTable->SetHelpText(_("HIDC_RUNSCORE_TABLE"));
	m_ctrlTable->SetToolTip(_("HIDC_RUNSCORE_TABLE"));

	wxStaticText* textHeight = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_HEIGHT"),
		wxDefaultPosition, wxDefaultSize, 0);
	textHeight->Wrap(-1);

	wxComboBox* ctrlHeight = new wxComboBox(panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxSize(50, -1),
		0, NULL, wxCB_DROPDOWN,
		CTrimValidator(&m_Height, TRIMVALIDATOR_TRIM_BOTH));
	ctrlHeight->SetHelpText(_("HIDC_RUNSCORE_HEIGHT"));
	ctrlHeight->SetToolTip(_("HIDC_RUNSCORE_HEIGHT"));
	std::set<tstring> names;
	m_pDoc->Book().GetAllHeights(names);
	std::set<tstring>::const_iterator iter;
	for (iter = names.begin(); iter != names.end(); ++iter)
	{
		ctrlHeight->Append((*iter).c_str());
	}

	wxStaticText* textJudge = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_JUDGE"),
		wxDefaultPosition, wxDefaultSize, 0);
	textJudge->Wrap(-1);

	m_ctrlJudge = new wxComboBox(panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		0, NULL, wxCB_DROPDOWN,
		CTrimValidator(&m_Judge, _("IDS_SELECT_JUDGE")));
	m_ctrlJudge->SetHelpText(_("HIDC_RUNSCORE_JUDGE"));
	m_ctrlJudge->SetToolTip(_("HIDC_RUNSCORE_JUDGE"));

	CNoteButton* ctrlJudgeNote = new CNoteButton(panelScore);
	ctrlJudgeNote->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgRun::OnJudgeNotes), NULL, this);
	ctrlJudgeNote->SetHelpText(_("HIDC_RUNSCORE_JUDGE_NOTES"));
	ctrlJudgeNote->SetToolTip(_("HIDC_RUNSCORE_JUDGE_NOTES"));

	wxStaticText* textHandler = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_HANDLER"),
		wxDefaultPosition, wxDefaultSize, 0);
	textHandler->Wrap(-1);

	wxComboBox* ctrlHandler = new wxComboBox(panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		0, NULL, wxCB_DROPDOWN,
		CTrimValidator(&m_Handler, TRIMVALIDATOR_TRIM_BOTH));
	ctrlHandler->SetHelpText(_("HIDC_RUNSCORE_HANDLER"));
	ctrlHandler->SetToolTip(_("HIDC_RUNSCORE_HANDLER"));
	m_pDoc->Book().GetAllHandlers(names);
	for (iter = names.begin(); iter != names.end(); ++iter)
	{
		ctrlHandler->Append((*iter).c_str());
	}

	wxStaticText* textConditions = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_CONDITIONS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textConditions->Wrap(-1);

	wxTextCtrl* ctrlConditions = new wxTextCtrl(panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxSize(-1, 50),
		wxTE_MULTILINE|wxTE_WORDWRAP,
		CTrimValidator(&m_Conditions, TRIMVALIDATOR_TRIM_BOTH));
	ctrlConditions->SetHelpText(_("HIDC_RUNSCORE_CONDITIONS"));
	ctrlConditions->SetToolTip(_("HIDC_RUNSCORE_CONDITIONS"));

	m_ctrlDesc = new CRichEditCtrl2(panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxSize(-1, 70),
		wxTE_READONLY);
	m_ctrlDesc->SetHelpText(_("HIDC_RUNSCORE_DESC"));
	m_ctrlDesc->SetToolTip(_("HIDC_RUNSCORE_DESC"));

	wxButton* btnPartner = new wxButton(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_PARTNERS_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnPartner->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgRun::OnPartnersEdit), NULL, this);
	btnPartner->SetHelpText(_("HIDC_RUNSCORE_PARTNERS_EDIT"));
	btnPartner->SetToolTip(_("HIDC_RUNSCORE_PARTNERS_EDIT"));

	m_ctrlPartner = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(200, -1), wxTE_READONLY);
	m_ctrlPartner->SetHelpText(_("HIDC_RUNSCORE_PARTNER"));
	m_ctrlPartner->SetToolTip(_("HIDC_RUNSCORE_PARTNER"));

	wxStaticText* m_textSCT = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_SCT"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textSCT->Wrap(-1);

	m_ctrlSCT = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0);
	m_ctrlSCT->SetHelpText(_("HIDC_RUNSCORE_SCT"));
	m_ctrlSCT->SetToolTip(_("HIDC_RUNSCORE_SCT"));

	wxStaticText* textTime = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_TIME"),
		wxDefaultPosition, wxDefaultSize, 0);
	textTime->Wrap(-1);

	wxTextCtrl* ctrlTime = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0);
	ctrlTime->SetHelpText(_("HIDC_RUNSCORE_TIME"));
	ctrlTime->SetToolTip(_("HIDC_RUNSCORE_TIME"));

	wxStaticText* textPlace = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_PLACE"),
		wxDefaultPosition, wxDefaultSize, 0);
	textPlace->Wrap(-1);

	wxTextCtrl* ctrlPlace = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(30, -1), 0);
	ctrlPlace->SetHelpText(_("HIDC_RUNSCORE_PLACE"));
	ctrlPlace->SetToolTip(_("HIDC_RUNSCORE_PLACE"));

	wxStaticText* textPlaceOf = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_IN_CLASS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textPlaceOf->Wrap(-1);

	wxTextCtrl* ctrlPlaceTotal = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(30, -1), 0);
	ctrlPlaceTotal->SetHelpText(_("HIDC_RUNSCORE_IN_CLASS"));
	ctrlPlaceTotal->SetToolTip(_("HIDC_RUNSCORE_IN_CLASS"));

	wxStaticText* m_textBonus = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_BONUSPTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textBonus->Wrap(-1);

	m_ctrlBonusPts = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0);
	m_ctrlBonusPts->SetHelpText(_("HIDC_RUNSCORE_BONUSPTS"));
	m_ctrlBonusPts->SetToolTip(_("HIDC_RUNSCORE_BONUSPTS"));

	wxStaticText* m_textYardsReqOpeningPts = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_OPENING_PTS"),
		//_("IDC_RUNSCORE_YARDS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textYardsReqOpeningPts->Wrap(-1);

	wxTextCtrl* m_ctrlYardsReqOpeningPts = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0);
	m_ctrlYardsReqOpeningPts->SetHelpText(_("HIDC_RUNSCORE_OPENING_PTS"));
	m_ctrlYardsReqOpeningPts->SetToolTip(_("HIDC_RUNSCORE_OPENING_PTS"));
	//m_ctrlYardsReqOpeningPts->SetHelpText(_("HIDC_RUNSCORE_YARDS"));
	//m_ctrlYardsReqOpeningPts->SetToolTip(_("HIDC_RUNSCORE_YARDS"));

	wxStaticText* textFaults = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_FAULTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textFaults->Wrap(-1);

	wxTextCtrl* ctrlFaults = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0);
	ctrlFaults->SetHelpText(_("HIDC_RUNSCORE_FAULTS"));
	ctrlFaults->SetToolTip(_("HIDC_RUNSCORE_FAULTS"));

	wxStaticText* textDogsQd = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_DOGS_QD"),
		wxDefaultPosition, wxDefaultSize, 0);
	textDogsQd->Wrap(-1);

	wxTextCtrl* ctrlDogsQd = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0);
	ctrlDogsQd->SetHelpText(_("HIDC_RUNSCORE_DOGS_QD"));
	ctrlDogsQd->SetToolTip(_("HIDC_RUNSCORE_DOGS_QD"));

	wxStaticText* m_textSpeed = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_SPEEDPTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textSpeed->Wrap(-1);

	m_ctrlSpeedPts = new wxStaticText(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), wxALIGN_CENTRE|wxST_NO_AUTORESIZE|wxSUNKEN_BORDER);

	wxStaticText* m_textMinYPSClosingTime = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_MIN_YPS"),
		//_("IDC_RUNSCORE_SCT2"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textMinYPSClosingTime->Wrap(-1);

	m_ctrlMinYPSClosingTime = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0);
	m_ctrlMinYPSClosingTime->SetHelpText(_("HIDC_RUNSCORE_SCT2"));
	m_ctrlMinYPSClosingTime->SetToolTip(_("HIDC_RUNSCORE_SCT2"));

	wxStaticText* m_textYPSOpeningPts = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_OPEN_PTS"),
		//_("IDC_RUNSCORE_YPS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textYPSOpeningPts->Wrap(-1);

	m_ctrlYPSOpeningPts = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0);
	m_ctrlYPSOpeningPts->SetHelpText(_("HIDC_RUNSCORE_OPEN_PTS"));
	m_ctrlYPSOpeningPts->SetToolTip(_("HIDC_RUNSCORE_OPEN_PTS"));

	wxStaticText* textQ = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_Q"),
		wxDefaultPosition, wxDefaultSize, 0);
	textQ->Wrap(-1);

	m_ctrlQ = new CQualifyingComboBox(panelScore, m_Run);
	m_ctrlQ->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(CDlgRun::OnSelchangeQ), NULL, this);
	m_ctrlQ->SetHelpText(_("HIDC_RUNSCORE_Q"));
	m_ctrlQ->SetToolTip(_("HIDC_RUNSCORE_Q"));

	wxStaticText* textTitlePts = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_TITLE_POINTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textTitlePts->Wrap(-1);

	m_ctrlTitlePoints = new wxStaticText(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), wxALIGN_CENTRE|wxST_NO_AUTORESIZE|wxSUNKEN_BORDER);

	wxStaticText* m_textReqClosingPts = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_CLOSING_PTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textReqClosingPts->Wrap(-1);

	wxTextCtrl* m_ctrlReqClosingPts = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0);
	m_ctrlReqClosingPts->SetHelpText(_("HIDC_RUNSCORE_CLOSING_PTS"));
	m_ctrlReqClosingPts->SetToolTip(_("HIDC_RUNSCORE_CLOSING_PTS"));

	wxStaticText* m_textClosingPtsTotalFaults = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_CLOSE_PTS"),
		//_("IDC_RUNSCORE_TOTAL_FAULTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textClosingPtsTotalFaults->Wrap(-1);

	wxTextCtrl* m_ctrlClosingPtsTotalFaults = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0);
	m_ctrlClosingPtsTotalFaults->SetHelpText(_("HIDC_RUNSCORE_CLOSE_PTS"));
	m_ctrlClosingPtsTotalFaults->SetToolTip(_("HIDC_RUNSCORE_CLOSE_PTS"));

	wxStaticText* textScore = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_SCORE"),
		wxDefaultPosition, wxDefaultSize, 0);
	textScore->Wrap(-1);

	m_ctrlScore = new wxStaticText(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), wxALIGN_CENTRE|wxST_NO_AUTORESIZE|wxSUNKEN_BORDER);

	wxStaticText* textNumObs = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_OBSTACLES"),
		wxDefaultPosition, wxDefaultSize, 0);
	textNumObs->Wrap(-1);

	wxTextCtrl* ctrlNumObs = new wxTextCtrl(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0);
	ctrlNumObs->SetHelpText(_("HIDC_RUNSCORE_OBSTACLES"));
	ctrlNumObs->SetToolTip(_("HIDC_RUNSCORE_OBSTACLES"));

	wxStaticText* textObsSec = new wxStaticText(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_OBSTACLES_PER_SEC"),
		wxDefaultPosition, wxDefaultSize, 0);
	textObsSec->Wrap(-1);

	m_ctrlObstaclesPS = new wxStaticText(panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), wxALIGN_CENTRE|wxST_NO_AUTORESIZE|wxSUNKEN_BORDER);
	m_ctrlObstaclesPS->SetHelpText(_("HIDC_RUNSCORE_OBSTACLES_PER_SEC"));
	m_ctrlObstaclesPS->SetToolTip(_("HIDC_RUNSCORE_OBSTACLES_PER_SEC"));

	wxButton* btnOtherPoints = new wxButton(panelScore, wxID_ANY,
		_("IDC_RUNSCORE_OTHERPOINTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnOtherPoints->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgRun::OnOtherpoints), NULL, this);
	btnOtherPoints->SetHelpText(_("HIDC_RUNSCORE_OTHERPOINTS"));
	btnOtherPoints->SetToolTip(_("HIDC_RUNSCORE_OTHERPOINTS"));
	if (0 < m_pDoc->Book().GetConfig().GetOtherPoints().size())
		btnOtherPoints->Enable(true);
	else
		btnOtherPoints->Enable(false);

	// Comments

	wxPanel* panelComments = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxTextCtrl* ctrlComments = new wxTextCtrl(panelComments, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_WORDWRAP,
		CTrimValidator(&m_Comments, TRIMVALIDATOR_TRIM_BOTH));
	ctrlComments->SetHelpText(_("HIDC_RUNCOMMENT_COMMENTS"));
	ctrlComments->SetToolTip(_("HIDC_RUNCOMMENT_COMMENTS"));

	m_ctrlFaultsList = new wxListBox(panelComments, wxID_ANY,
		wxDefaultPosition, wxDefaultSize,
		0, NULL, 0);
	m_ctrlFaultsList->SetHelpText(_("HIDC_RUNCOMMENT_FAULTS_LIST"));
	m_ctrlFaultsList->SetToolTip(_("HIDC_RUNCOMMENT_FAULTS_LIST"));

	wxButton* btnFaults = new wxButton(panelComments, wxID_ANY,
		_("IDC_RUNCOMMENT_COMMENTS_FAULTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnFaults->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgRun::OnCommentsFaults), NULL, this);
	btnFaults->SetHelpText(_("HIDC_RUNCOMMENT_COMMENTS_FAULTS"));
	btnFaults->SetToolTip(_("HIDC_RUNCOMMENT_COMMENTS_FAULTS"));

	// RefRuns

	wxPanel* panelRefRuns = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	m_ctrlRefRuns = new CReportListCtrl(panelRefRuns,
		true, CReportListCtrl::eSortHeader, true);
	m_ctrlRefRuns->Connect(wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler(CDlgRun::OnRefRunColumnClick), NULL, this);
	m_ctrlRefRuns->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(CDlgRun::OnRefRunItemSelected), NULL, this);
	m_ctrlRefRuns->Connect(wxEVT_COMMAND_LEFT_DCLICK, wxMouseEventHandler(CDlgRun::OnRefRunDoubleClick), NULL, this);
	m_ctrlRefRuns->Connect(wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler(CDlgRun::OnRefRunKeyDown), NULL, this);
	m_ctrlRefRuns->SetHelpText(_("HIDC_RUNREF_REF_RUNS"));
	m_ctrlRefRuns->SetToolTip(_("HIDC_RUNREF_REF_RUNS"));
	int index;
	for (index = 0; index < scNumRefRunColumns; ++index)
	{
		m_ctrlRefRuns->InsertColumn(index, wxGetTranslation(scRefRunColumns[index].idText), scRefRunColumns[index].fmt);
	}
	SetRefRunColumnHeaders();
	for (index = 0; index < scNumRefRunColumns; ++index)
		m_ctrlRefRuns->SetColumnWidth(index, wxLIST_AUTOSIZE_USEHEADER);

	wxButton* btnRefNew = new wxButton(panelRefRuns, wxID_ANY,
		_("IDC_RUNREF_NEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnRefNew->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgRun::OnRefRunNew), NULL, this);
	btnRefNew->SetHelpText(_("HIDC_RUNREF_NEW"));
	btnRefNew->SetToolTip(_("HIDC_RUNREF_NEW"));

	m_ctrlRefAddMe = new wxButton(panelRefRuns, wxID_ANY,
		_("IDC_RUNREF_ADDDOG"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlRefAddMe->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgRun::OnRefRunAddMe), NULL, this);
	m_ctrlRefAddMe->SetHelpText(_("HIDC_RUNREF_ADDDOG"));
	m_ctrlRefAddMe->SetToolTip(_("HIDC_RUNREF_ADDDOG"));

	m_ctrlRefEdit = new wxButton(panelRefRuns, wxID_ANY,
		_("IDC_RUNREF_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlRefEdit->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgRun::OnRefRunEdit), NULL, this);
	m_ctrlRefEdit->SetHelpText(_("HIDC_RUNREF_EDIT"));
	m_ctrlRefEdit->SetToolTip(_("HIDC_RUNREF_EDIT"));

	m_ctrlRefDelete = new wxButton(panelRefRuns, wxID_ANY,
		_("IDC_RUNREF_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlRefDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgRun::OnRefRunDelete), NULL, this);
	m_ctrlRefDelete->SetHelpText(_("HIDC_RUNREF_DELETE"));
	m_ctrlRefDelete->SetToolTip(_("HIDC_RUNREF_DELETE"));

	// CRCD

	wxPanel* panelCRCD = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxStaticText* textCRCD = new wxStaticText(panelCRCD, wxID_ANY,
		_("IDC_RUN_CRCD"),
		wxDefaultPosition, wxDefaultSize, 0);
	textCRCD->Wrap(500);

	m_CRCDDisplay = new CMetaDataDisplay(panelCRCD, m_Run);

	if (m_CRCDDisplay->HasMetafileSupport())
	{
		m_ctrlCourse = new wxButton(panelCRCD, wxID_ANY,
			_("IDC_RUNCRCD_EDIT"),
			wxDefaultPosition, wxDefaultSize, 0);
		m_ctrlCourse->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgRun::OnCRCDEdit), NULL, this);
		m_ctrlCourse->SetHelpText(_("HIDC_RUNCRCD_EDIT"));
		m_ctrlCourse->SetToolTip(_("HIDC_RUNCRCD_EDIT"));

		m_ctrlCRCDView = new wxButton(panelCRCD, wxID_ANY,
			_("IDC_RUNCRCD_VIEW"),
			wxDefaultPosition, wxDefaultSize, 0);
		m_ctrlCRCDView->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgRun::OnCRCDView), NULL, this);
		m_ctrlCRCDView->SetHelpText(_("HIDC_RUNCRCD_VIEW"));
		m_ctrlCRCDView->SetToolTip(_("HIDC_RUNCRCD_VIEW"));
	}

	m_ctrlCRCDCopy = new wxButton(panelCRCD, wxID_ANY,
		_("IDC_RUNCRCD_COPY"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlCRCDCopy->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgRun::OnCRCDCopy), NULL, this);
	m_ctrlCRCDCopy->SetHelpText(_("HIDC_RUNCRCD_COPY"));
	m_ctrlCRCDCopy->SetToolTip(_("HIDC_RUNCRCD_COPY"));

	wxStaticText* textImageDesc = NULL;
	if (m_CRCDDisplay->HasMetafileSupport())
	{
		m_ctrlIncImage = new wxCheckBox(panelCRCD, wxID_ANY,
			_("IDC_RUNCRCD_IMAGE"),
			wxDefaultPosition, wxDefaultSize, 0);
		m_ctrlIncImage->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CDlgRun::OnCRCDImage), NULL, this);
		m_ctrlIncImage->SetHelpText(_("HIDC_RUNCRCD_IMAGE"));
		m_ctrlIncImage->SetToolTip(_("HIDC_RUNCRCD_IMAGE"));
		bool setCheck = CAgilityBookOptions::GetIncludeCRCDImage();
		if (m_CRCDDisplay->HasMetafile())
			setCheck = true;
		m_ctrlIncImage->SetValue(setCheck);

		textImageDesc = new wxStaticText(panelCRCD, wxID_ANY,
			_("IDC_RUNCRCD_IMAGE_TEXT"),
			wxDefaultPosition, wxDefaultSize, 0);
		textImageDesc->Wrap(80);
	}

	// Links

	wxPanel* panelLinks = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	m_ctrlLinks = new wxListView(panelLinks, wxID_ANY,
		wxDefaultPosition, wxDefaultSize,
		wxLC_NO_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL);
	m_ctrlLinks->SetImageList(&m_ImageList, wxIMAGE_LIST_SMALL);
	m_imgEmpty = m_ImageList.Add(wxIcon(CalEmpty_xpm));
	m_imgOk = m_ImageList.Add(wxIcon(CalPlan_xpm));
	m_imgMissing = m_ImageList.Add(wxIcon(CalTentative_xpm));
	m_ctrlLinks->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(CDlgRun::OnLinksItemSelected), NULL, this);
	m_ctrlLinks->Connect(wxEVT_COMMAND_LEFT_DCLICK, wxMouseEventHandler(CDlgRun::OnLinksDoubleClick), NULL, this);
	m_ctrlLinks->Connect(wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler(CDlgRun::OnLinksKeyDown), NULL, this);
	m_ctrlLinks->SetHelpText(_("HIDC_RUNLINK_LIST"));
	m_ctrlLinks->SetToolTip(_("HIDC_RUNLINK_LIST"));
	m_ctrlLinks->InsertColumn(0, wxT(""));

	wxButton* btnLinkNew = new wxButton(panelLinks, wxID_ANY,
		_("IDC_RUNLINK_NEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnLinkNew->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgRun::OnLinksNew), NULL, this);
	btnLinkNew->SetHelpText(_("HIDC_RUNLINK_NEW"));
	btnLinkNew->SetToolTip(_("HIDC_RUNLINK_NEW"));

	m_ctrlLinkEdit = new wxButton(panelLinks, wxID_ANY,
		_("IDC_RUNLINK_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlLinkEdit->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgRun::OnLinksEdit), NULL, this);
	m_ctrlLinkEdit->SetHelpText(_("HIDC_RUNLINK_EDIT"));
	m_ctrlLinkEdit->SetToolTip(_("HIDC_RUNLINK_EDIT"));

	m_ctrlLinkDelete = new wxButton(panelLinks, wxID_ANY,
		_("IDC_RUNLINK_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlLinkDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgRun::OnLinksDelete), NULL, this);
	m_ctrlLinkDelete->SetHelpText(_("HIDC_RUNLINK_DELETE"));
	m_ctrlLinkDelete->SetToolTip(_("HIDC_RUNLINK_DELETE"));

	m_ctrlLinkOpen = new wxButton(panelLinks, wxID_ANY,
		_("IDC_RUNLINK_OPEN"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlLinkOpen->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgRun::OnLinksOpen), NULL, this);
	m_ctrlLinkOpen->SetHelpText(_("HIDC_RUNLINK_OPEN"));
	m_ctrlLinkOpen->SetToolTip(_("HIDC_RUNLINK_OPEN"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerScorePanel = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerDate = new wxBoxSizer(wxHORIZONTAL);
	sizerDate->Add(textDate, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerDate->Add(ctrlDate, 0, wxALL, 5);
	sizerDate->Add(textVenue, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerDate->Add(textClub, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerDate->Add(textLocation, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	sizerScorePanel->Add(sizerDate, 0, wxEXPAND, 5);

	wxBoxSizer* sizerDivHt = new wxBoxSizer(wxHORIZONTAL);

	wxFlexGridSizer* sizerEvent = new wxFlexGridSizer(5, 2, 0, 0);
	sizerEvent->SetFlexibleDirection(wxBOTH);
	sizerEvent->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerEvent->Add(textDiv, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerEvent->Add(m_ctrlDivisions, 0, wxALL, 5);
	sizerEvent->Add(textLevel, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerEvent->Add(m_ctrlLevels, 0, wxALL, 5);
	sizerEvent->Add(textEvent, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerEvent->Add(m_ctrlEvents, 0, wxALL, 5);
	sizerEvent->Add(m_ctrlSubNamesText, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerEvent->Add(m_ctrlSubNames, 0, wxALL, 5);
	sizerEvent->Add(0, 0, 1, wxEXPAND, 5);
	sizerEvent->Add(m_ctrlTable, 0, wxALL, 5);

	sizerDivHt->Add(sizerEvent, 0, wxEXPAND, 5);

	wxBoxSizer* sizerHtCond = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerHt = new wxBoxSizer(wxHORIZONTAL);
	sizerHt->Add(textHeight, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerHt->Add(ctrlHeight, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerHt->Add(textJudge, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerHt->Add(m_ctrlJudge, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerHt->Add(ctrlJudgeNote, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT|wxTOP, 5);
	sizerHt->Add(textHandler, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerHt->Add(ctrlHandler, 0, wxALL, 5);

	sizerHtCond->Add(sizerHt, 0, wxEXPAND, 5);

	wxBoxSizer* sizerCond = new wxBoxSizer(wxHORIZONTAL);
	sizerCond->Add(textConditions, 0, wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerCond->Add(ctrlConditions, 1, wxALL|wxEXPAND, 5);

	sizerHtCond->Add(sizerCond, 1, wxEXPAND, 5);

	wxBoxSizer* sizerComments = new wxBoxSizer(wxHORIZONTAL);
	sizerComments->Add(m_ctrlDesc, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerPartner = new wxBoxSizer(wxVERTICAL);
	sizerPartner->Add(btnPartner, 0, wxALL, 5);
	sizerPartner->Add(m_ctrlPartner, 0, wxALL, 5);

	sizerComments->Add(sizerPartner, 0, wxEXPAND, 5);

	sizerHtCond->Add(sizerComments, 0, wxEXPAND, 5);

	sizerDivHt->Add(sizerHtCond, 0, wxEXPAND, 5);

	sizerScorePanel->Add(sizerDivHt, 0, wxEXPAND, 5);

	wxFlexGridSizer* sizerResults = new wxFlexGridSizer(5, 4, 0, 0);
	sizerResults->SetFlexibleDirection(wxBOTH);
	sizerResults->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxBoxSizer* sizerSCT = new wxBoxSizer(wxHORIZONTAL);
	sizerSCT->Add(m_textSCT, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerSCT->Add(m_ctrlSCT, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	sizerResults->Add(sizerSCT, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerTime = new wxBoxSizer(wxHORIZONTAL);
	sizerTime->Add(textTime, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerTime->Add(ctrlTime, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	sizerResults->Add(sizerTime, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerPlace = new wxBoxSizer(wxHORIZONTAL);
	sizerPlace->Add(textPlace, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerPlace->Add(ctrlPlace, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerPlace->Add(textPlaceOf, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxTOP, 5);
	sizerPlace->Add(ctrlPlaceTotal, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	sizerResults->Add(sizerPlace, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerBonus = new wxBoxSizer(wxHORIZONTAL);
	sizerBonus->Add(m_textBonus, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxTOP, 5);
	sizerBonus->Add(m_ctrlBonusPts, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	sizerResults->Add(sizerBonus, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerYardsReqOpeningPts = new wxBoxSizer(wxHORIZONTAL);
	sizerYardsReqOpeningPts->Add(m_textYardsReqOpeningPts, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerYardsReqOpeningPts->Add(m_ctrlYardsReqOpeningPts, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerYardsReqOpeningPts, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerFaults = new wxBoxSizer(wxHORIZONTAL);
	sizerFaults->Add(textFaults, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerFaults->Add(ctrlFaults, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerFaults, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerDogsQd = new wxBoxSizer(wxHORIZONTAL);
	sizerDogsQd->Add(textDogsQd, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerDogsQd->Add(ctrlDogsQd, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerDogsQd, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerSpeed = new wxBoxSizer(wxHORIZONTAL);
	sizerSpeed->Add(m_textSpeed, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerSpeed->Add(m_ctrlSpeedPts, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerSpeed, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerMinYPSClosingTime = new wxBoxSizer(wxHORIZONTAL);
	sizerMinYPSClosingTime->Add(m_textMinYPSClosingTime, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerMinYPSClosingTime->Add(m_ctrlMinYPSClosingTime, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerMinYPSClosingTime, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerYPSOpeningPts = new wxBoxSizer(wxHORIZONTAL);
	sizerYPSOpeningPts->Add(m_textYPSOpeningPts, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerYPSOpeningPts->Add(m_ctrlYPSOpeningPts, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerYPSOpeningPts, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerQ = new wxBoxSizer(wxHORIZONTAL);
	sizerQ->Add(textQ, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerQ->Add(m_ctrlQ, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerQ, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerTitlePts = new wxBoxSizer(wxHORIZONTAL);
	sizerTitlePts->Add(textTitlePts, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerTitlePts->Add(m_ctrlTitlePoints, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerTitlePts, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerReqClosingPts = new wxBoxSizer(wxHORIZONTAL);
	sizerReqClosingPts->Add(m_textReqClosingPts, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerReqClosingPts->Add(m_ctrlReqClosingPts, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerReqClosingPts, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerClosingPtsTotalFaults = new wxBoxSizer(wxHORIZONTAL);
	sizerClosingPtsTotalFaults->Add(m_textClosingPtsTotalFaults, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerClosingPtsTotalFaults->Add(m_ctrlClosingPtsTotalFaults, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerClosingPtsTotalFaults, 0, wxALIGN_RIGHT, 5);

	sizerResults->Add(0, 0, 1, wxEXPAND, 5);

	wxBoxSizer* sizerScore = new wxBoxSizer(wxHORIZONTAL);
	sizerScore->Add(textScore, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerScore->Add(m_ctrlScore, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerScore, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerNumObs = new wxBoxSizer(wxHORIZONTAL);
	sizerNumObs->Add(textNumObs, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerNumObs->Add(ctrlNumObs, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerNumObs, 0, wxALIGN_RIGHT, 5);

	wxBoxSizer* sizerObsSec = new wxBoxSizer(wxHORIZONTAL);
	sizerObsSec->Add(textObsSec, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5);
	sizerObsSec->Add(m_ctrlObstaclesPS, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerResults->Add(sizerObsSec, 0, wxALIGN_RIGHT, 5);
	sizerResults->Add(0, 0, 1, wxEXPAND, 5);
	sizerResults->Add(btnOtherPoints, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);

	sizerScorePanel->Add(sizerResults, 1, wxEXPAND, 5);

	panelScore->SetSizer(sizerScorePanel);
	panelScore->Layout();
	sizerScorePanel->Fit(panelScore);
	notebook->AddPage(panelScore, _("IDD_RUN_SCORE"), true);

	wxBoxSizer* sizerCommentsFaults = new wxBoxSizer(wxHORIZONTAL);
	sizerCommentsFaults->Add(ctrlComments, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerFaultsList = new wxBoxSizer(wxVERTICAL);
	sizerFaultsList->Add(m_ctrlFaultsList, 1, wxALL, 5);
	sizerFaultsList->Add(btnFaults, 0, wxALL, 5);

	sizerCommentsFaults->Add(sizerFaultsList, 0, wxEXPAND, 5);

	panelComments->SetSizer(sizerCommentsFaults);
	panelComments->Layout();
	sizerCommentsFaults->Fit(panelComments);
	notebook->AddPage(panelComments, _("IDD_RUN_COMMENTS"), false);

	wxBoxSizer* sizerRefRuns = new wxBoxSizer(wxVERTICAL);
	sizerRefRuns->Add(m_ctrlRefRuns, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerRefBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerRefBtns->Add(btnRefNew, 0, wxALL, 5);
	sizerRefBtns->Add(m_ctrlRefAddMe, 0, wxALL, 5);
	sizerRefBtns->Add(m_ctrlRefEdit, 0, wxALL, 5);
	sizerRefBtns->Add(m_ctrlRefDelete, 0, wxALL, 5);
	sizerRefRuns->Add(sizerRefBtns, 0, wxEXPAND, 5);

	panelRefRuns->SetSizer(sizerRefRuns);
	panelRefRuns->Layout();
	sizerRefRuns->Fit(panelRefRuns);
	notebook->AddPage(panelRefRuns, _("IDD_RUN_REFERENCE"), false);
	m_idxRefRunPage = notebook->GetPageCount() - 1;

	wxBoxSizer* sizerCRCD = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerDisplay = new wxBoxSizer(wxVERTICAL);
	sizerDisplay->Add(textCRCD, 0, wxALL, 5);
	sizerDisplay->Add(m_CRCDDisplay, 1, wxALL|wxEXPAND, 5);

	sizerCRCD->Add(sizerDisplay, 1, wxEXPAND, 5);

	wxBoxSizer* sizerBtnsCRCD = new wxBoxSizer(wxVERTICAL);
	if (m_ctrlCourse)
		sizerBtnsCRCD->Add(m_ctrlCourse, 0, wxALL, 5);
	if (m_ctrlCRCDView)
		sizerBtnsCRCD->Add(m_ctrlCRCDView, 0, wxALL, 5);
	sizerBtnsCRCD->Add(m_ctrlCRCDCopy, 0, wxALL, 5);
	if (m_ctrlIncImage)
		sizerBtnsCRCD->Add(m_ctrlIncImage, 0, wxALL, 5);
	if (textImageDesc)
		sizerBtnsCRCD->Add(textImageDesc, 0, wxALL, 5);

	sizerCRCD->Add(sizerBtnsCRCD, 0, wxEXPAND, 5);

	panelCRCD->SetSizer(sizerCRCD);
	panelCRCD->Layout();
	sizerCRCD->Fit(panelCRCD);
	notebook->AddPage(panelCRCD, _("IDD_RUN_CRCD"), false);

	wxBoxSizer* sizerLinks = new wxBoxSizer(wxVERTICAL);
	sizerLinks->Add(m_ctrlLinks, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerLinkBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerLinkBtns->Add(btnLinkNew, 0, wxALL, 5);
	sizerLinkBtns->Add(m_ctrlLinkEdit, 0, wxALL, 5);
	sizerLinkBtns->Add(m_ctrlLinkDelete, 0, wxALL, 5);
	sizerLinkBtns->Add(0, 0, 1, wxEXPAND, 5);
	sizerLinkBtns->Add(m_ctrlLinkOpen, 0, wxALL, 5);

	sizerLinks->Add(sizerLinkBtns, 0, wxEXPAND, 5);

	panelLinks->SetSizer(sizerLinks);
	panelLinks->Layout();
	sizerLinks->Fit(panelLinks);
	notebook->AddPage(panelLinks, _("IDD_RUN_LINK"), false);

	bSizer->Add(notebook, 1, wxEXPAND | wxALL, 5);

	wxSizer* sdbSizer = CreateButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	FillDivisions(); // This will call UpdateControls();
	FillJudges();
	SetPartnerText();
	SetObstacles();
	SetFaultsText();
	ListRefRuns();
	UpdateCRCDButtons();
	ListLinkFiles(NULL);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	notebook->SetSelection(iSelectPage);
}


CDlgDogDivData* CDlgRun::GetDivisionData(int index) const
{
	return dynamic_cast<CDlgDogDivData*>(m_ctrlDivisions->GetClientObject(index));
}


CDlgDogLevelData* CDlgRun::GetLevelData(int index) const
{
	return dynamic_cast<CDlgDogLevelData*>(m_ctrlLevels->GetClientObject(index));
}


/*
bool CDlgRun::GetText(
		CEdit* pEdit,
		short& val) const
{
{
}


bool CDlgRun::GetText(
		CEdit* pEdit,
		double& val) const
{
{
}
*/


bool CDlgRun::GetEvent(ARBConfigEventPtr* outEvent) const
{
	if (outEvent)
		outEvent->reset();
	int index = m_ctrlEvents->GetSelection();
	if (wxNOT_FOUND == index)
		return false;
	wxString str = m_ctrlEvents->GetString(index);
	if (str.empty())
		return false;
	tstring evt = str.c_str();
	return m_pVenue->GetEvents().FindEvent(evt, outEvent);
}


bool CDlgRun::GetScoring(ARBConfigScoringPtr* outScoring) const
{
	bool bFound = false;
	if (outScoring)
		outScoring->reset();
	tstring div, level;
	int index = m_ctrlDivisions->GetSelection();
	if (wxNOT_FOUND != index)
	{
		div = m_ctrlDivisions->GetString(index);
	}
	index = m_ctrlLevels->GetSelection();
	if (wxNOT_FOUND != index)
	{
		level = m_ctrlLevels->GetString(index);
		CDlgDogLevelData* pLevel = GetLevelData(index);
		assert(pLevel);
		ARBConfigEventPtr pEvent;
		if (GetEvent(&pEvent))
		{
			if (0 < div.length() && 0 < level.length())
				bFound = pEvent->FindEvent(div, pLevel->m_pLevel->GetName(), m_Run->GetDate(), outScoring);
		}
	}
	return bFound;
}


void CDlgRun::FillDivisions()
{
	tstring div;
	long index = m_ctrlDivisions->GetSelection();
	if (wxNOT_FOUND != index)
	{
		div = m_ctrlDivisions->GetString(index).c_str();
	}
	if (div.empty())
		div = m_Run->GetDivision();
	m_ctrlDivisions->Clear();

	for (ARBConfigDivisionList::const_iterator iterDiv = m_pVenue->GetDivisions().begin();
		iterDiv != m_pVenue->GetDivisions().end();
		++iterDiv)
	{
		ARBConfigDivisionPtr pDiv = (*iterDiv);
		// Does this division have any events possible on this date?
		for (ARBConfigEventList::const_iterator iterEvent = m_pVenue->GetEvents().begin();
			iterEvent != m_pVenue->GetEvents().end();
			++iterEvent)
		{
			if ((*iterEvent)->VerifyEvent(pDiv->GetName(), WILDCARD_LEVEL, m_Run->GetDate()))
			{
				index = m_ctrlDivisions->Append(pDiv->GetName().c_str());
				m_ctrlDivisions->SetClientObject(index, new CDlgDogDivData(pDiv));
				if (pDiv->GetName() == div)
					m_ctrlDivisions->SetSelection(index);
				break;
			}
		}
	}
	// First try to find 'div' (in case the divisions changed)
	if (wxNOT_FOUND == m_ctrlDivisions->GetSelection() && !div.empty())
	{
		index = m_ctrlDivisions->FindString(div.c_str(), true);
		if (0 <= index)
			m_ctrlDivisions->SetSelection(index);
	}
	// Then try to find the last entered
	if (wxNOT_FOUND == m_ctrlDivisions->GetSelection())
	{
		wxString last = CAgilityBookOptions::GetLastEnteredDivision();
		if (0 < last.length())
		{
			index = m_ctrlDivisions->FindString(last, true);
			if (0 <= index)
				m_ctrlDivisions->SetSelection(index);
		}
	}
	if (wxNOT_FOUND == m_ctrlDivisions->GetSelection())
	{
		if (1 == m_ctrlDivisions->GetCount())
			m_ctrlDivisions->SetSelection(0);
	}
	FillLevels();
}


void CDlgRun::FillLevels()
{
	tstring level;
	int index = m_ctrlLevels->GetSelection();
	if (wxNOT_FOUND != index)
	{
		level = m_ctrlLevels->GetString(index).c_str();
	}
	if (level.empty())
		level = m_Run->GetLevel();
	m_ctrlLevels->Clear();
	index = m_ctrlDivisions->GetSelection();
	if (wxNOT_FOUND != index)
	{
		ARBConfigDivisionPtr pDiv = GetDivisionData(index)->m_Div;
		for (ARBConfigLevelList::const_iterator iter = pDiv->GetLevels().begin();
			iter != pDiv->GetLevels().end();
			++iter)
		{
			ARBConfigLevelPtr pLevel = (*iter);
			// Does this level have any events possible on this date?
			for (ARBConfigEventList::const_iterator iterEvent = m_pVenue->GetEvents().begin();
				iterEvent != m_pVenue->GetEvents().end();
				++iterEvent)
			{
				if ((*iterEvent)->VerifyEvent(pDiv->GetName(), pLevel->GetName(), m_Run->GetDate()))
				{
					if (0 < pLevel->GetSubLevels().size())
					{
						for (ARBConfigSubLevelList::const_iterator iterSub = pLevel->GetSubLevels().begin();
							iterSub != pLevel->GetSubLevels().end();
							++iterSub)
						{
							ARBConfigSubLevelPtr pSubLevel = (*iterSub);
							int idx = m_ctrlLevels->Append(pSubLevel->GetName().c_str());
							m_ctrlLevels->SetClientObject(idx, new CDlgDogLevelData(pLevel, pSubLevel));
							if (level == pSubLevel->GetName())
								m_ctrlLevels->SetSelection(idx);
						}
					}
					else
					{
						int idx = m_ctrlLevels->Append(pLevel->GetName().c_str());
						m_ctrlLevels->SetClientObject(idx, new CDlgDogLevelData(pLevel));
						if (level == pLevel->GetName())
							m_ctrlLevels->SetSelection(idx);
					}
					break;
				}
			}
		}
		if (wxNOT_FOUND == m_ctrlLevels->GetSelection())
		{
			wxString last = CAgilityBookOptions::GetLastEnteredLevel();
			if (0 < last.length())
			{
				int idx = m_ctrlLevels->FindString(last, true);
				if (0 <= idx)
					m_ctrlLevels->SetSelection(idx);
			}
		}
	}
	if (wxNOT_FOUND == m_ctrlLevels->GetSelection())
	{
		if (1 == m_ctrlLevels->GetCount())
			m_ctrlLevels->SetSelection(0);
	}
	FillEvents();
	SetTitlePoints();
}


void CDlgRun::FillEvents()
{
	tstring evt;
	int index = m_ctrlEvents->GetSelection();
	if (wxNOT_FOUND != index)
	{
		evt = m_ctrlEvents->GetString(index).c_str();
	}
	if (evt.empty())
		evt = m_Run->GetEvent();
	m_ctrlEvents->Clear();
	int idxDiv = m_ctrlDivisions->GetSelection();
	if (wxNOT_FOUND != idxDiv)
	{
		ARBConfigDivisionPtr pDiv = GetDivisionData(idxDiv)->m_Div;
		int idxLevel = m_ctrlLevels->GetSelection();
		if (wxNOT_FOUND != idxLevel)
		{
			CDlgDogLevelData* pData = GetLevelData(idxLevel);
			for (ARBConfigEventList::const_iterator iter = m_pVenue->GetEvents().begin();
				iter != m_pVenue->GetEvents().end();
				++iter)
			{
				ARBConfigEventPtr pEvent = (*iter);
				if (pEvent->FindEvent(pDiv->GetName(), pData->m_pLevel->GetName(), m_Run->GetDate()))
				{
					int idx = m_ctrlEvents->Append(pEvent->GetName().c_str());
					if (evt == pEvent->GetName())
					{
						m_ctrlEvents->SetSelection(idx);
						SetEventDesc(pEvent);
					}
				}
			}
		}
	}
	FillSubNames();
	UpdateControls();
}


void CDlgRun::FillSubNames()
{
	ARBConfigEventPtr pEvent;
	if (GetEvent(&pEvent))
	{
		if (pEvent->HasSubNames())
		{
			std::set<tstring> names;
			m_pDoc->Book().GetAllEventSubNames(m_pVenue->GetName(), pEvent, names);
			m_ctrlSubNames->Clear();
			for (std::set<tstring>::const_iterator iter = names.begin();
				iter != names.end();
				++iter)
			{
				m_ctrlSubNames->Append(iter->c_str());
			}
			m_ctrlSubNamesText->Show(true);
			m_ctrlSubNames->Show(true);
		}
		else
		{
			m_ctrlSubNamesText->Show(false);
			m_ctrlSubNames->Show(false);
		}
	}
	else
	{
		m_ctrlSubNamesText->Show(false);
		m_ctrlSubNames->Show(false);
	}
}


void CDlgRun::FillJudges()
{
	std::set<tstring> names;
	m_pDoc->Book().GetAllJudges(names, true, true);
	if (!m_Run->GetJudge().empty())
		names.insert(m_Run->GetJudge());
	m_ctrlJudge->Clear();
	for (std::set<tstring>::const_iterator iter = names.begin(); iter != names.end(); ++iter)
	{
		m_ctrlJudge->Append((*iter).c_str());
	}
	m_ctrlJudge->SetValue(m_Judge);
}


void CDlgRun::SetEventDesc(ARBConfigEventPtr inEvent)
{
	wxString desc;
	if (inEvent)
		desc += inEvent->GetDesc().c_str();
	ARBConfigScoringPtr pScoring;
	if (GetScoring(&pScoring))
	{
		tstring const& note = pScoring->GetNote();
		if (!desc.empty() && 0 < note.length())
			desc += wxT("\n==========\n");
		desc += note.c_str();
	}
	m_ctrlDesc->SetValue(desc);
}


void CDlgRun::SetPartnerText()
{
	wxString partners;
	ARBConfigEventPtr pEvent;
	if (GetEvent(&pEvent))
	{
		if (pEvent->HasPartner())
		{
			for (ARBDogRunPartnerList::const_iterator iter = m_Run->GetPartners().begin(); iter != m_Run->GetPartners().end(); ++iter)
			{
				if (!partners.empty())
					partners += wxT(", ");
				partners += (*iter)->GetHandler().c_str();
				partners += wxT("/");
				partners += (*iter)->GetDog().c_str();
			}
		}
	}
	m_ctrlPartner->SetValue(partners);
}


void CDlgRun::SetMinYPS()
{
	wxString str;
	double yps;
	if (m_Run->GetScoring().GetMinYPS(CAgilityBookOptions::GetTableInYPS(), yps))
	{
		str = ARBDouble::str(yps, 3).c_str();
	}
	m_ctrlMinYPSClosingTime->SetValue(str);
}


void CDlgRun::SetYPS()
{
	wxString str;
	double yps;
	if (m_Run->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), yps))
	{
		str = ARBDouble::str(yps, 3).c_str();
	}
	m_ctrlYPSOpeningPts->SetValue(str);
}


void CDlgRun::SetObstacles()
{
	wxString str;
	double ops;
	if (m_Run->GetScoring().GetObstaclesPS(CAgilityBookOptions::GetTableInYPS(), ops))
	{
		str = ARBDouble::str(ops, 3).c_str();
	}
	m_ctrlObstaclesPS->SetLabel(str);
}


void CDlgRun::SetTotalFaults()
{
	wxString total;
	if (ARBDogRunScoring::eTypeByTime == m_Run->GetScoring().GetType())
	{
		ARBConfigScoringPtr pScoring;
		GetScoring(&pScoring);
		double faults = m_Run->GetScoring().GetCourseFaults() + m_Run->GetScoring().GetTimeFaults(pScoring);
		total = ARBDouble::str(faults, 3).c_str();
	}
	m_ctrlClosingPtsTotalFaults->SetValue(total);
}


void CDlgRun::SetTitlePoints()
{
	int index = m_ctrlQ->GetSelection();
	if (wxNOT_FOUND == index)
	{
		m_ctrlQ->SetFocus();
		return;
	}
	ARB_Q q = m_ctrlQ->GetQ(index);

	wxString strBonus(wxT("0"));
	wxString strSpeed(wxT("0"));
	wxString strTitle(wxT("0"));
	wxString strScore(wxT(""));
	ARBConfigScoringPtr pScoring;
	if (GetScoring(&pScoring))
	{
		// 8/17/03: Only compute title points on Q runs.
		if (q.Qualified())
		{
			if (pScoring->HasBonusPts())
			{
				otstringstream str;
				str << m_Run->GetScoring().GetBonusPts();
				strBonus = str.str().c_str();
			}
			if (pScoring->HasSpeedPts())
			{
				otstringstream str;
				str << m_Run->GetSpeedPoints(pScoring);
				strSpeed = str.str().c_str();
			}
			{
				otstringstream str;
				str << m_Run->GetTitlePoints(pScoring);
				strTitle = str.str().c_str();
			}
		}
		// 8/17/03: Only compute score on Q and NQ runs.
		// 11/13/04: Also compute score for NA runs that have no titling pts.
		if (q.Qualified()
		|| ARB_Q::eQ_NQ == q
		|| (ARB_Q::eQ_NA == q && ARBDouble::equal(0.0, static_cast<double>(pScoring->GetTitlePoints().size()))))
			strScore = ARBDouble::str(m_Run->GetScore(pScoring)).c_str();
	}
	// Doesn't matter if they're hidden,..
	m_ctrlBonusPts->SetValue(strBonus);
	m_ctrlSpeedPts->SetLabel(strSpeed);
	m_ctrlTitlePoints->SetLabel(strTitle);
	m_ctrlScore->SetLabel(strScore);
}


void CDlgRun::UpdateControls(bool bOnEventChange)
{
}


void CDlgRun::SetFaultsText()
{
	m_ctrlFaultsList->Clear();
	for (ARBDogFaultList::const_iterator iter = m_Run->GetFaults().begin(); iter != m_Run->GetFaults().end(); ++iter)
	{
		m_ctrlFaultsList->Append((*iter).c_str());
	}
}


CDlgDogRefRunDataPtr CDlgRun::GetReferenceDataByData(long index) const
{
	return tr1::dynamic_pointer_cast<CDlgDogRefRunData, CListData>(m_ctrlRefRuns->GetDataByData(index));
}


CDlgDogRefRunDataPtr CDlgRun::GetReferenceData(long index) const
{
	return tr1::dynamic_pointer_cast<CDlgDogRefRunData, CListData>(m_ctrlRefRuns->GetData(index));
}


bool CDlgRun::IsRefRunMe()
{
	if (m_pRefRunMe)
	{
		for (ARBDogReferenceRunList::const_iterator iterRef = m_Run->GetReferenceRuns().begin();
			iterRef != m_Run->GetReferenceRuns().end();
			++iterRef)
		{
			if (*iterRef == m_pRefRunMe)
			{
				return true;
			}
		}
	}
	return false;
}


void CDlgRun::CreateRefRunMe()
{
	// Create the 'me' reference run.
	m_pRefRunMe = ARBDogReferenceRunPtr(ARBDogReferenceRun::New());
	m_pRefRunMe->SetQ(m_Run->GetQ());
	m_pRefRunMe->SetPlace(m_Run->GetPlace());
	m_pRefRunMe->SetName(m_pDoc->GetCurrentDog()->GetCallName());
	m_pRefRunMe->SetHeight(m_Run->GetHeight());
	m_pRefRunMe->SetBreed(m_pDoc->GetCurrentDog()->GetBreed());
	m_pRefRunMe->SetTime(m_Run->GetScoring().GetTime());
	ARBConfigScoringPtr pScoring;
	if (m_pDoc->Book().GetConfig().GetVenues().FindEvent(
		m_pVenue->GetName(),
		m_Run->GetEvent(),
		m_Run->GetDivision(),
		m_Run->GetLevel(),
		m_Run->GetDate(),
		NULL,
		&pScoring))
	{
		m_pRefRunMe->SetScore(ARBDouble::str(m_Run->GetScore(pScoring)));
	}

	// Now see if I'm already in there.
	// Only compare: Q/Place/Name/Time/Score.
	for (ARBDogReferenceRunList::const_iterator iterRef = m_Run->GetReferenceRuns().begin();
		iterRef != m_Run->GetReferenceRuns().end();
		++iterRef)
	{
		ARBDogReferenceRunPtr pRef = *iterRef;
		if (pRef->GetQ() == m_pRefRunMe->GetQ()
		&& pRef->GetPlace() == m_pRefRunMe->GetPlace()
		&& pRef->GetName() == m_pRefRunMe->GetName()
		&& ARBDouble::equal(pRef->GetTime(), m_pRefRunMe->GetTime())
		&& pRef->GetScore() == m_pRefRunMe->GetScore())
		{
			m_pRefRunMe = pRef;
			break;
		}
	}
	// Note: This means if we create 'me', modify it, switch tabs
	// and come back, 'addme' is enabled again.
}


void CDlgRun::UpdateRefRunButtons()
{
	m_ctrlRefAddMe->Enable(!IsRefRunMe());
	bool bSelected = (0 <= m_ctrlRefRuns->GetFirstSelected());
	m_ctrlRefEdit->Enable(bSelected);
	m_ctrlRefDelete->Enable(bSelected);
}


void CDlgRun::SetRefRunColumnHeaders()
{
	LV_COLUMN col;
	col.mask = LVCF_TEXT | LVCF_SUBITEM;
	for (int i = 0; i < scNumRefRunColumns; ++i)
	{
		otstringstream str;
		str << wxGetTranslation(scRefRunColumns[i].idText)
			<< wxT(" (" << m_sortRefRuns.FindColumnOrder(i) + 1) << ')';
		wxListItem item;
		item.SetMask(wxLIST_MASK_TEXT);
		item.SetText(str.str().c_str());
		m_ctrlRefRuns->SetColumn(i, item);
		m_ctrlRefRuns->SetColumnSort(i, m_sortRefRuns.IsDescending(i) ? -1 : 1);
	}
}


void CDlgRun::ListRefRuns()
{
	ARBDogReferenceRunPtr pSelected;
	long index = m_ctrlRefRuns->GetFirstSelected();
	if (0 <= index)
	{
		ARBDogReferenceRunPtr pData = GetReferenceData(index)->GetData();
		if (pData)
			pSelected = pData;
	}
	m_ctrlRefRuns->DeleteAllItems();

	index = 0;
	for (ARBDogReferenceRunList::const_iterator iterRef = m_Run->GetReferenceRuns().begin();
	iterRef != m_Run->GetReferenceRuns().end();
	++index, ++iterRef)
	{
		CDlgDogRefRunDataPtr data(new CDlgDogRefRunData(*iterRef));
		m_ctrlRefRuns->InsertItem(data);
	}
	for (index = 0; index < scNumRefRunColumns; ++index)
		m_ctrlRefRuns->SetColumnWidth(index, wxLIST_AUTOSIZE_USEHEADER);
	s_SortInfo.pThis = this;
	s_SortInfo.pCols = &m_sortRefRuns;
	m_ctrlRefRuns->SortItems(CompareRefRuns, 0);

	if (pSelected)
	{
		for (index = 0; index < m_ctrlRefRuns->GetItemCount(); ++index)
		{
			ARBDogReferenceRunPtr pRefRun = GetReferenceData(index)->GetData();
			if (pRefRun == pSelected) // compare by ptr is fine.
			{
				m_ctrlRefRuns->SetSelection(index, true);
				break;
			}
		}
	}
	UpdateRefRunButtons();
}


void CDlgRun::GetAllHeights(std::set<tstring>& outNames)
{
	m_pDoc->Book().GetAllHeights(outNames);
	for (long index = 0; index < m_ctrlRefRuns->GetItemCount(); ++index)
	{
		ARBDogReferenceRunPtr pRefRun = GetReferenceData(index)->GetData();
		if (pRefRun)
		{
			tstring const& ht = pRefRun->GetHeight();
			if (0 < ht.length())
				outNames.insert(ht);
		}
	}
}


void CDlgRun::GetAllCallNames(std::set<tstring>& outNames)
{
	m_pDoc->Book().GetAllCallNames(outNames);
	for (int index = 0; index < m_ctrlRefRuns->GetItemCount(); ++index)
	{
		ARBDogReferenceRunPtr pRefRun = GetReferenceData(index)->GetData();
		if (pRefRun)
		{
			tstring const& ht = pRefRun->GetName();
			if (0 < ht.length())
				outNames.insert(ht);
		}
	}
}


void CDlgRun::GetAllBreeds(std::set<tstring>& outNames)
{
	m_pDoc->Book().GetAllBreeds(outNames);
	for (int index = 0; index < m_ctrlRefRuns->GetItemCount(); ++index)
	{
		ARBDogReferenceRunPtr pRefRun = GetReferenceData(index)->GetData();
		if (pRefRun)
		{
			tstring const& ht = pRefRun->GetBreed();
			if (0 < ht.length())
				outNames.insert(ht);
		}
	}
}


void CDlgRun::EditRefRun()
{
	long nItem = m_ctrlRefRuns->GetFirstSelected();
	if (0 <= nItem)
	{
		std::set<tstring> heights, names, breeds;
		GetAllHeights(heights);
		GetAllCallNames(names);
		GetAllBreeds(breeds);
		ARBDogReferenceRunPtr pRef = GetReferenceData(nItem)->GetData();
		CDlgReferenceRun dlg(m_pDoc, m_Run, heights, names, breeds, pRef, this);
		if (wxID_OK == dlg.ShowModal())
			ListRefRuns();
	}
}


void CDlgRun::UpdateCRCDButtons()
{
	if (m_ctrlCRCDView)
	{
		if (m_CRCDDisplay->ViewText())
			m_ctrlCRCDView->SetLabel(_("IDS_CRCD_BTN_VIEWIMAGE"));
		else
			m_ctrlCRCDView->SetLabel(_("IDS_CRCD_BTN_VIEWTEXT"));
	}
	if (m_CRCDDisplay->HasCourse())
		m_ctrlCRCDCopy->SetLabel(_("IDS_CRCD_BTN_CLEARCOURSE"));
	else
		m_ctrlCRCDCopy->SetLabel(_("IDS_CRCD_BTN_INSERTCOURSE"));

	if (m_ctrlCourse)
		m_ctrlCourse->Enable(m_CRCDDisplay->HasMetafile());
	if (m_ctrlCRCDView)
		m_ctrlCRCDView->Enable(m_CRCDDisplay->HasMetafile());
}


void CDlgRun::UpdateLinksButtons()
{
	bool bEnable = (0 <= m_ctrlLinks->GetFirstSelected());
	m_ctrlLinkEdit->Enable(bEnable);
	m_ctrlLinkDelete->Enable(bEnable);
	m_ctrlLinkOpen->Enable(bEnable);
}


void CDlgRun::ListLinkFiles(wxChar const* pItem)
{
	wxBusyCursor wait;
	m_ctrlLinks->DeleteAllItems();
	std::set<tstring> links;
	m_Run->GetLinks(links);
	long i = 0;
	for (std::set<tstring>::iterator iter = links.begin();
		iter != links.end();
		++iter)
	{
		int idx = m_ctrlLinks->InsertItem(i++, (*iter).c_str(), GetImageIndex((*iter).c_str()));
		if (pItem && *iter == pItem)
			m_ctrlLinks->Select(idx);
	}
	m_ctrlLinks->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
	UpdateLinksButtons();
}


int CDlgRun::GetImageIndex(tstring const& inLink)
{
	wxBusyCursor wait;
	int img = m_imgEmpty;
	if (0 < inLink.length())
	{
		if (CheckLink(inLink, this))
			img = m_imgOk;
		else
			img = m_imgMissing;
	}
	return img;
}


void CDlgRun::EditLink()
{
	int nItem = m_ctrlLinks->GetFirstSelected();
	if (0 <= nItem)
	{
		wxString name = GetListColumnText(m_ctrlLinks, nItem, 0);
		CDlgSelectURL dlg(name, this);
		if (wxID_OK == dlg.ShowModal())
		{
			wxString newName = dlg.GetName();
			if (name != newName)
			{
				m_Run->RemoveLink(name.c_str());
				if (0 < newName.length())
					m_Run->AddLink(newName.c_str());
				ListLinkFiles(newName.c_str());
			}
		}
	}
}


void CDlgRun::OnScoreDateChanged(wxDateEvent& evt)
{
	m_Run->SetDate(ARBDate(evt.GetDate().GetYear(), evt.GetDate().GetMonth(), evt.GetDate().GetDay()));
	FillDivisions();
}


void CDlgRun::OnSelchangeDivision(wxCommandEvent& evt)
{
	FillLevels();
}


void CDlgRun::OnSelchangeLevel(wxCommandEvent& evt)
{
	FillEvents();
}


void CDlgRun::OnSelchangeEvent(wxCommandEvent& evt)
{
	FillSubNames();
	UpdateControls(true);
	ARBConfigEventPtr pEvent;
	GetEvent(&pEvent);
	SetEventDesc(pEvent);
}


void CDlgRun::OnJudgeNotes(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	CDlgInfoNote dlg(m_pDoc, ARBInfo::eJudgeInfo, m_Judge, this);
	if (wxID_OK == dlg.ShowModal())
	{
		m_Judge = dlg.CurrentSelection();
		FillJudges();
	}
}


void CDlgRun::OnPartnersEdit(wxCommandEvent& evt)
{
	CDlgListCtrl dlg(CDlgListCtrl::ePartners, m_pDoc, m_Run, this);
	if (wxID_OK == dlg.ShowModal())
		SetPartnerText();
}


void CDlgRun::OnOtherpoints(wxCommandEvent& evt)
{
	CDlgListCtrl dlg(m_pDoc->Book().GetConfig(), m_Run, this);
	dlg.ShowModal();
}


void CDlgRun::OnBnClickedTableYps(wxCommandEvent& evt)
{
	bool bSetTable = false;
	if (m_ctrlTable->IsShown())
		bSetTable  = m_ctrlTable->GetValue();
	m_Run->GetScoring().SetHasTable(bSetTable);
	SetMinYPS();
	SetYPS();
	SetObstacles();
}


void CDlgRun::OnSelchangeQ(wxCommandEvent& evt)
{
	ARB_Q q;
	int index = m_ctrlQ->GetSelection();
	if (wxNOT_FOUND != index)
		q = m_ctrlQ->GetQ(index);
	m_Run->SetQ(q);
	SetTitlePoints();
}


void CDlgRun::OnCommentsFaults(wxCommandEvent& evt)
{
	CDlgListCtrl dlg(CDlgListCtrl::eFaults, m_pDoc, m_Run, this);
	if (wxID_OK == dlg.ShowModal())
		SetFaultsText();
}


void CDlgRun::OnRefRunColumnClick(wxListEvent& evt)
{
	s_SortInfo.pThis = this;
	m_sortRefRuns.SetColumnOrder(evt.GetColumn());
	SetRefRunColumnHeaders();
	s_SortInfo.pCols = &m_sortRefRuns;
	m_ctrlRefRuns->SortItems(CompareRefRuns, 0);
	m_sortRefRuns.Save();
}


void CDlgRun::OnRefRunItemSelected(wxListEvent& evt)
{
	UpdateRefRunButtons();
}


void CDlgRun::OnRefRunDoubleClick(wxMouseEvent& evt)
{
	EditRefRun();
}


void CDlgRun::OnRefRunKeyDown(wxListEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
		EditRefRun();
		break;
	}
	evt.Skip();
}


void CDlgRun::OnRefRunNew(wxCommandEvent& evt)
{
	ARBDogReferenceRunPtr ref(ARBDogReferenceRun::New());
	if (ARBDogRunScoring::eTypeByTime == m_Run->GetScoring().GetType())
	{
		ARBConfigScoringPtr pScoring;
		if (m_pDoc->Book().GetConfig().GetVenues().FindEvent(
			m_pVenue->GetName(),
			m_Run->GetEvent(),
			m_Run->GetDivision(),
			m_Run->GetLevel(),
			m_Run->GetDate(),
			NULL,
			&pScoring))
		{
			tstring nScore;
			switch (pScoring->GetScoringStyle())
			{
			default:
				nScore = wxT("0");
				break;
			case ARBConfigScoring::eFaults100ThenTime:
				nScore = wxT("100");
				break;
			case ARBConfigScoring::eFaults200ThenTime:
				nScore = wxT("200");
				break;
			}
			ref->SetScore(nScore);
		}
	}
	ref->SetQ(ARB_Q::eQ_Q);
	std::set<tstring> heights, names, breeds;
	GetAllHeights(heights);
	GetAllCallNames(names);
	GetAllBreeds(breeds);
	CDlgReferenceRun dlg(m_pDoc, m_Run, heights, names, breeds, ref, this);
	if (wxID_OK == dlg.ShowModal())
	{
		if (m_Run->GetReferenceRuns().AddReferenceRun(ref))
		{
			CDlgDogRefRunDataPtr data(new CDlgDogRefRunData(ref));
			long index = m_ctrlRefRuns->InsertItem(data);
			m_ctrlRefRuns->Select(index);
			// Insert item first to set selection.
			// ListRefRuns then 'fixes' order.
			ListRefRuns();
		}
	}
}


void CDlgRun::OnRefRunAddMe(wxCommandEvent& evt)
{
	if (!IsRefRunMe())
	{
		if (m_Run->GetReferenceRuns().AddReferenceRun(m_pRefRunMe))
		{
			CDlgDogRefRunDataPtr data(new CDlgDogRefRunData(m_pRefRunMe));
			long index = m_ctrlRefRuns->InsertItem(data);
			m_ctrlRefRuns->Select(index);
			// Insert item first to set selection.
			// ListRefRuns then 'fixes' order.
			ListRefRuns();
		}
	}
}


void CDlgRun::OnRefRunEdit(wxCommandEvent& evt)
{
	EditRefRun();
}


void CDlgRun::OnRefRunDelete(wxCommandEvent& evt)
{
	long nItem = m_ctrlRefRuns->GetFirstSelected();
	if (0 <= nItem)
	{
		ARBDogReferenceRunPtr pRef = GetReferenceData(nItem)->GetData();
		if (pRef)
		{
			if (m_Run->GetReferenceRuns().DeleteReferenceRun(pRef))
				m_ctrlRefRuns->DeleteItem(nItem);
			if (m_pRefRunMe && m_pRefRunMe == pRef)
				CreateRefRunMe();
		}
		UpdateRefRunButtons();
	}
}


void CDlgRun::OnCRCDEdit(wxCommandEvent& evt)
{
	m_CRCDDisplay->Display();
}


void CDlgRun::OnCRCDView(wxCommandEvent& evt)
{
	m_CRCDDisplay->ToggleView();
	UpdateCRCDButtons();
}


void CDlgRun::OnCRCDCopy(wxCommandEvent& evt)
{
	m_CRCDDisplay->OnCopy();
	UpdateCRCDButtons();
}


void CDlgRun::OnCRCDImage(wxCommandEvent& evt)
{
	m_CRCDDisplay->OnCRCDImage(m_ctrlIncImage->GetValue());
	UpdateCRCDButtons();
}


void CDlgRun::OnLinksItemSelected(wxListEvent& evt)
{
	UpdateLinksButtons();
}


void CDlgRun::OnLinksDoubleClick(wxMouseEvent& evt)
{
	EditLink();
}


void CDlgRun::OnLinksKeyDown(wxListEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
		EditLink();
		break;
	}
	evt.Skip();
}


void CDlgRun::OnLinksNew(wxCommandEvent& evt)
{
	CDlgSelectURL dlg(wxT(""), this);
	if (wxID_OK == dlg.ShowModal())
	{
		wxString newName = dlg.GetName();
		if (0 < newName.length())
		{
			m_Run->AddLink(newName.c_str());
			ListLinkFiles(newName.c_str());
		}
	}
}


void CDlgRun::OnLinksEdit(wxCommandEvent& evt)
{
	EditLink();
}


void CDlgRun::OnLinksDelete(wxCommandEvent& evt)
{
	long nItem = m_ctrlLinks->GetFirstSelected();
	if (0 <= nItem)
	{
		wxString name = GetListColumnText(m_ctrlLinks, nItem, 0);
		m_Run->RemoveLink(name.c_str());
		ListLinkFiles(NULL);
	}
}


void CDlgRun::OnLinksOpen(wxCommandEvent& evt)
{
	int nItem = m_ctrlLinks->GetFirstSelected();
	if (0 <= nItem)
	{
		wxString name = GetListColumnText(m_ctrlLinks, nItem, 0);
		wxLaunchDefaultBrowser(name);
	}
}


void CDlgRun::OnPageChanged(wxNotebookEvent& evt)
{
	if (m_idxRefRunPage == evt.GetSelection())
	{
		// Recreate the 'me' reference run on page change. This will push in
		// any changes made in the scoring page.
		CreateRefRunMe();
		UpdateRefRunButtons();
	}
	evt.Skip();
}


void CDlgRun::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	m_Run->SetSubName(m_SubName.c_str());
	m_Run->SetHeight(m_Height.c_str());
	m_Run->SetJudge(m_Judge.c_str());
	m_Run->SetHandler(m_Handler.c_str());
	m_Run->SetConditions(m_Conditions.c_str());
	m_Run->SetNote(m_Comments.c_str());

	//TODO: Remove debugging code
#ifdef _DEBUG
	{
		ARBConfigEventPtr pEvent;
		m_pVenue->GetEvents().FindEvent(m_Run->GetEvent(), &pEvent);
		assert(NULL != pEvent.get());
		if (!pEvent->HasTable())
			if (m_Run->GetScoring().HasTable())
				wxMessageBox(wxT("Poof!"), wxMessageBoxCaptionStr, wxCENTRE);
	}
#endif
	//End TODO

	*m_pRealRun = *m_Run;
	m_pTrial->SetMultiQs(m_pDoc->Book().GetConfig()); // Note, when adding a new run, this is actually too soon to call - the run isn't in the trial yet
	CAgilityBookOptions::SetLastEnteredDivision(m_Run->GetDivision().c_str());
	CAgilityBookOptions::SetLastEnteredLevel(m_Run->GetLevel().c_str());
	CAgilityBookOptions::SetLastEnteredHeight(m_Run->GetHeight().c_str());
	CAgilityBookOptions::SetLastEnteredJudge(m_Run->GetJudge().c_str());
	CAgilityBookOptions::SetLastEnteredHandler(m_Run->GetHandler().c_str());
	EndDialog(wxID_OK);
}
