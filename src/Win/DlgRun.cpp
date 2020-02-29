/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgRun class
 * @author David Connet
 *
 * Revision History
 * 2019-09-17 Fix last-entered on hidden fields. Fix run's club on creation.
 * 2019-08-18 Fix Bonus points (couldn't enter double).
 *            Auto-select event when there's only one.
 * 2018-12-16 Convert to fmt.
 * 2017-11-21 Update title points when InClass changes.
 * 2015-11-01 Compute score for NA runs also.
 * 2015-01-01 Changed pixels to dialog units.
 * 2013-05-19 Make last div/level/height/handler dog-aware.
 * 2012-12-29 Fix pasting metafiles.
 * 2012-11-17 Allow judge to be empty.
 * 2012-07-04 Add option to use run time or opening time in gamble OPS.
 * 2012-05-07 Added autocompletion to combo boxes.
 * 2012-02-16 Fix initial focus.
 * 2012-01-03 Fix field updating (some fields didn't have validator
 *            associated variables tied to them)
 * 2012-01-02 Change validator to support default value on empty field.
 * 2012-01-01 Add validation dialogs, page change vetoing.
 * 2011-12-30 Fixed CGenericValidator.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2011-02-12 Add DnD support for linked files.
 * 2010-01-02 Fix setting of required points with level changes.
 * 2009-10-18 Fix prepending of '0' to title points.
 * 2009-10-14 Add dog's name to dialog caption.
 * 2009-10-14 Fix initialization of opening pts in eScoreThenTime.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-08-12 Fixed division/level initialization.
 * 2009-03-16 Merged DlgRun* into here.
 * 2009-02-09 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2003-10-13 Make ref run dlg default to perfect score.
 *
 * DlgRunCRCD
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-05-04 Added ability to suppress metafile.
 * 2004-04-27 Added some error recovery.
 *
 * DlgRunComments
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 *
 * DlgRunLink
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-06-02 Moved ShellExecute code to AgilityBook.cpp, added icons.
 * 2004-03-30 Created
 *
 * DlgRunReference
 * 2015-11-01 Added YPS.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-09-20 Added yourself was not getting up-to-date scoring info.
 * 2005-07-10 Add button to add yourself to ref-runs.
 *            Make default ref-run a 'Q'.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-09-28 Accumulate all heights for refrun dlg.
 * 2003-12-27 Changed FindEvent to take a date.
 * 2003-10-13 Make ref run dlg default to perfect score.
 *
 * DlgRunScore
 * 2008-02-01 Make 'Notes' button change selection.
 * 2008-11-21 Enable tallying runs that have only lifetime points.
 * 2007-12-03 Refresh judge list after invoking 'notes' button.
 * 2007-07-01 Fixed a problem with table flag on a run.
 * 2006-11-05 Trim Divisions/Levels if no events are available on date.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-13 Added direct access to Notes dialog.
 * 2005-12-04 Added support for NADAC bonus titling points.
 * 2005-11-20 Allow 'E's on non-titling runs.
 * 2005-08-11 Removed QQ checkbox.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2005-01-02 Added subnames to events.
 * 2005-01-01 Renamed MachPts to SpeedPts.
 * 2004-11-13 Also compute score for NA runs that have no titling pts.
 * 2004-09-07 Time+Fault scoring shouldn't include time faults.
 * 2004-03-20 The date never got set if the initial entry had no date
 *            and we didn't change it (first run in a trial).
 *            Plus, the table-in-yps flag was backwards and didn't
 *            properly initialize when the event type changed.
 * 2004-02-14 Added Table-in-YPS flag.
 * 2004-02-09 Update YPS when the time is changed.
 *            When date changes, update controls.
 * 2004-01-19 Total faults weren't updated when course faults changed.
 * 2004-01-18 Calling UpdateData during data entry causes way too much
 *            validation. Only call during OnOK (from dlgsheet)
 * 2003-12-27 Changed FindEvent to take a date. Also, update the controls
 *            when the date changes as the scoring config may change.
 * 2003-12-09 Fixed a bug where the QQ checkbox didn't get set right.
 * 2003-10-13 Made Time/CourseFaults common to all scoring methods.
 *            This allows faults for things like language!
 * 2003-09-29 Required pts were being overwriten with default values
 *            during dialog initialization.
 * 2003-09-01 Total faults weren't being shown when there was no SCT.
 * 2003-08-17 Title points were being computed on 'NQ' and the score was
 *            always being computed. Fixed both.
 * 2003-07-14 Changed 'Score' to show data on 'Q' and 'NQ'.
 */

#include "stdafx.h"
#include "DlgRun.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ClipBoard.h"
#include "ComboBoxQ.h"
#include "DlgCRCDViewer.h"
#include "DlgInfoNote.h"
#include "DlgListCtrl.h"
#include "DlgReferenceRun.h"
#include "DlgSelectURL.h"
#include "ImageHelper.h"
#include "NoteButton.h"

#include "ARB/ARBConfig.h"
#include "ARB/ARBConfigOtherPoints.h"
#include "ARB/ARBConfigVenue.h"
#include "ARB/ARBDogClub.h"
#include "ARB/ARBDogRun.h"
#include "ARB/ARBDogTrial.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ARBWinUtilities.h"
#include "LibARBWin/CheckLink.h"
#include "LibARBWin/DPI.h"
#include "LibARBWin/ListCtrl.h"
#include "LibARBWin/ListData.h"
#include "LibARBWin/RichEditCtrl2.h"
#include "LibARBWin/Validators.h"
#include <algorithm>
#include <set>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/dcbuffer.h>
#include <wx/dnd.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

#ifdef __WXMSW__
// You can currently only enable this on Win32. I directly use the win32
// apis to play the metafile into a dc. In theory, I could use wxWidgets
// to play them - but I still need the direct win32 access to copy a
// metafile from the clipboard - wxWidgets only has support to copy >to<
// the clipboard.
#define HAS_ENHMETAFILE
#endif


/////////////////////////////////////////////////////////////////////////////

#if wxUSE_DRAG_AND_DROP

class CLinkDropTarget : public wxFileDropTarget
{
public:
	CLinkDropTarget(CDlgRun* dlg)
		: m_dlg(dlg)
	{
	}
	bool OnDropFiles(
			wxCoord x,
			wxCoord y,
			wxArrayString const& filenames) override;
private:
	CDlgRun* m_dlg;
};


bool CLinkDropTarget::OnDropFiles(
		wxCoord x,
		wxCoord y,
		wxArrayString const& filenames)
{
	for (size_t n = 0; n < filenames.size(); ++n)
	{
		m_dlg->m_Run->AddLink(StringUtil::stringW(filenames[n]));
		m_dlg->ListLinkFiles(StringUtil::stringW(filenames[n]).c_str());
	}
	return true;
}

#endif

/////////////////////////////////////////////////////////////////////////////

namespace
{
	static struct
	{
		int fmt;
		wchar_t const* idText;
	} const scRefRunColumns[] = {
		{wxLIST_FORMAT_CENTRE, arbT("IDS_COL_Q")},
		{wxLIST_FORMAT_CENTRE, arbT("IDS_COL_PLACE")},
		{wxLIST_FORMAT_CENTRE, arbT("IDS_COL_SCORE")},
		{wxLIST_FORMAT_LEFT, arbT("IDS_COL_TIME")},
		{wxLIST_FORMAT_LEFT, arbT("IDS_COL_YPS")},
		{wxLIST_FORMAT_LEFT, arbT("IDS_COL_NAME")},
		{wxLIST_FORMAT_LEFT, arbT("IDS_COL_HEIGHT")},
		{wxLIST_FORMAT_LEFT, arbT("IDS_COL_BREED")},
		{wxLIST_FORMAT_LEFT, arbT("IDS_COL_NOTE")},
	};
	constexpr int scNumRefRunColumns = sizeof(scRefRunColumns) / sizeof(scRefRunColumns[0]);
}

struct RunSortInfo : public SortInfo
{
	CDlgRun* pThis;
	CColumnOrder* pCols;

	RunSortInfo(CDlgRun* This, CColumnOrder* cols)
		: pThis(This)
		, pCols(cols)
	{
	}
};

/////////////////////////////////////////////////////////////////////////////

class CDlgDogVenueData : public wxClientData
{
public:
	CDlgDogVenueData(
			ARBDogClubPtr const& inClub,
			ARBConfigVenuePtr const& inVenue)
		: m_Club(inClub)
		, m_Venue(inVenue)
	{
	}
	ARBDogClubPtr m_Club;
	ARBConfigVenuePtr m_Venue;
};


class CDlgDogDivData : public wxClientData
{
public:
	CDlgDogDivData(ARBConfigDivisionPtr const& inDiv)
		: m_Div(inDiv)
	{
	}
	ARBConfigDivisionPtr m_Div;
};


class CDlgDogLevelData : public wxClientData
{
public:
	CDlgDogLevelData(ARBConfigLevelPtr const& inLevel)
		: m_pLevel(inLevel)
		, m_pSubLevel()
	{
	}
	CDlgDogLevelData(
			ARBConfigLevelPtr const& inLevel,
			ARBConfigSubLevelPtr const& inSubLevel)
		: m_pLevel(inLevel)
		, m_pSubLevel(inSubLevel)
	{
	}
	ARBConfigLevelPtr m_pLevel;
	ARBConfigSubLevelPtr m_pSubLevel;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgDogRefRunData : public CListData
{
	DECLARE_NO_COPY_IMPLEMENTED(CDlgDogRefRunData)
public:
	CDlgDogRefRunData(ARBDogRunPtr const& inRun, ARBDogReferenceRunPtr const& inRefRun)
		: m_Run(inRun)
		, m_RefRun(inRefRun)
	{
	}
	std::wstring OnNeedText(long iCol) const override;
	ARBDogReferenceRunPtr GetData() const		{return m_RefRun;}
private:
	ARBDogRunPtr m_Run;
	ARBDogReferenceRunPtr m_RefRun;
};


std::wstring CDlgDogRefRunData::OnNeedText(long iCol) const
{
	std::wstring str;
	switch (iCol)
	{
	default:
		break;
	case 0: // Q
		str = fmt::format(L"{}", m_RefRun->GetQ().str());
		break;
	case 1: // Place
		str = fmt::format(L"{}", m_RefRun->GetPlace());
		break;
	case 2: // Score
		str = m_RefRun->GetScore();
		break;
	case 3: // Time
		str = ARBDouble::ToString(m_RefRun->GetTime());
		break;
	case 4: // YPS
		{
			double time = m_RefRun->GetTime();
			double yps;
			if (m_Run->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), time, yps))
			{
				str = ARBDouble::ToString(yps, 3);
			}
		}
		break;
	case 5: // Name
		str = m_RefRun->GetName();
		break;
	case 6: // Height
		str = m_RefRun->GetHeight();
		break;
	case 7: // Breed
		str = m_RefRun->GetBreed();
		break;
	case 8: // Note
		str = StringUtil::Replace(m_RefRun->GetNote(), L"\n", L" ");
		break;
	}
	return str;
}


int wxCALLBACK CompareRefRuns(CListDataPtr const& item1, CListDataPtr const& item2, SortInfo const* pSortInfo)
{
	RunSortInfo const* pInfo = dynamic_cast<RunSortInfo const*>(pSortInfo);
	assert(pInfo);

	CDlgDogRefRunDataPtr pData1 = std::dynamic_pointer_cast<CDlgDogRefRunData, CListData>(item1);
	CDlgDogRefRunDataPtr pData2 = std::dynamic_pointer_cast<CDlgDogRefRunData, CListData>(item2);

	ARBDogReferenceRunPtr pTitle1 = pData1->GetData();
	ARBDogReferenceRunPtr pTitle2 = pData2->GetData();
	int rc = 0;
	for (int i = 0; i < pInfo->pCols->GetSize(); ++i)
	{
		int col = pInfo->pCols->GetColumnAt(i);
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
			if (pInfo->pCols->IsDescending(col))
				rc *= -1;
			break;
		}
	}
	return rc;
}

/////////////////////////////////////////////////////////////////////////////

class CMetaDataDisplay : public CTextCtrl
{
	DECLARE_NO_COPY_IMPLEMENTED(CMetaDataDisplay)
public:
	CMetaDataDisplay(
			wxWindow* parent,
			ARBDogRunPtr const& inRun);
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
		return !!m_metaFile;
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


wxBEGIN_EVENT_TABLE(CMetaDataDisplay, CTextCtrl)
	EVT_PAINT(CMetaDataDisplay::OnPaint)
wxEND_EVENT_TABLE()


CMetaDataDisplay::CMetaDataDisplay(
		wxWindow* parent,
		ARBDogRunPtr const& inRun)
	: m_Run(inRun)
#ifdef HAS_ENHMETAFILE
	, m_metaFile(nullptr)
	, m_metaFileBack(nullptr)
#endif
	, m_ViewText(true)
	, m_Insert(inRun->GetCRCD().empty())
{
	CTextCtrl::Create(parent, wxID_ANY,
		StringUtil::stringWX(m_Run->GetCRCD()),
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY);
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	AllowMultilineTabstop(true);

#ifdef HAS_ENHMETAFILE
	if (0 < m_Run->GetCRCDRawMetaData().length())
	{
		ARBMetaDataPtr metaData = m_Run->GetCRCDMetaData();
		m_metaFile = SetEnhMetaFileBits(static_cast<UINT>(metaData->data().size()), metaData->data().data());
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
		m_metaFile = nullptr;
		m_metaFileBack = nullptr;
	}
#endif
}


void CMetaDataDisplay::SetCRCDData()
{
	std::vector<unsigned char> bits;
#ifdef HAS_ENHMETAFILE
	if (m_metaFile)
	{
		ENHMETAHEADER header;
		GetEnhMetaFileHeader(m_metaFile, sizeof(header), &header);
		UINT nSize = GetEnhMetaFileBits(m_metaFile, 0, nullptr);
		bits = std::vector<unsigned char>(nSize+1);
		GetEnhMetaFileBits(m_metaFile, nSize, bits.data());
		assert(sizeof(BYTE) == sizeof(char));
		m_Run->SetCRCDMetaData(bits);
	}
	else
#endif
		m_Run->SetCRCDMetaData(bits);
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
		if (wxTheClipboard->Open())
		{
			bText = wxTheClipboard->IsSupported(wxDF_TEXT);
			wxTheClipboard->Close();
		}
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
				SetLabel(L"");
				DeleteMetaFile();
				if (bText)
				{
					m_ViewText = true;
					std::wstring str;
					clpData.GetData(str);
					str = StringUtil::Trim(str);
					// We do the replace since CRCD3 has "\n\nhdrs\r\netc"
					// Standardize to \n.
					str = StringUtil::Replace(str, L"\r\n", L"\n");
					m_Run->SetCRCD(str);
					SetLabel(StringUtil::stringWX(str));
					if (0 < str.length())
						m_Insert = false;
					// Only create the metafile if we pasted text. Otherwise
					// we could end up with a non-CRCD metafile. It's still
					// possible this may not be CRCD data - the user can
					// just clear it then.
					clpData.Close();
#ifdef HAS_ENHMETAFILE
					if (bMeta)
					{
						// Note, wxClipboard may be using the OLE clipboard
						// which doesn't actually open the windows one.
						wxOpenClipboard();
						HENHMETAFILE hData = static_cast<HENHMETAFILE>(GetClipboardData(CF_ENHMETAFILE));
						m_metaFile = CopyEnhMetaFile(hData, nullptr);
						m_metaFileBack = m_metaFile;
						SetCRCDData();
						if (m_metaFile)
						{
							m_ViewText = false;
							m_Insert = false;
						}
						wxCloseClipboard();
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
		SetLabel(L"");
		DeleteMetaFile();
		m_Run->SetCRCD(L"");
		m_Run->SetCRCDMetaData(std::vector<unsigned char>());
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
			m_metaFile = nullptr;
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
		wxAutoBufferedPaintDC dc(this);
		dc.SetBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
		dc.Clear();
#ifdef HAS_ENHMETAFILE
		wxSize sz = GetClientSize();
		RECT r;
		r.left = 0;
		r.top = 0;
		r.right = std::min(sz.x, sz.y);
		r.bottom = std::min(sz.x, sz.y);
		WXHDC hdc = dc.GetHDC();
		PlayEnhMetaFile(hdc, m_metaFile, &r);
#endif
	}
}

/////////////////////////////////////////////////////////////////////////////

// This is just to get the text in a sunken static control to look better
static std::wstring Pad(std::wstring const& val)
{
	return fmt::format(L" {} ", val);
}

/////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(CDlgRun, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgRun::OnOk)
wxEND_EVENT_TABLE()


CDlgRun::CDlgRun(
		CAgilityBookDoc* pDoc,
		ARBDogPtr const& inDog,
		ARBDogTrialPtr const& inTrial,
		ARBDogRunPtr const& inRun,
		wxWindow* pParent,
		int iSelectPage)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_pDog(inDog)
	, m_pTrial(inTrial)
	, m_pRealRun(inRun)
	, m_Run(inRun->Clone())
	, m_pRefRunMe()
	, m_panelScore(nullptr)
	, m_Date(inRun->GetDate())
	, m_ctrlVenues(nullptr)
	, m_ctrlDivisions(nullptr)
	, m_ctrlLevels(nullptr)
	, m_ctrlEvents(nullptr)
	, m_ctrlSubNamesText(nullptr)
	, m_ctrlSubNames(nullptr)
	, m_SubName(StringUtil::stringWX(inRun->GetSubName()))
	, m_ctrlTable(nullptr)
	, m_Table(false)
	, m_textHeight(nullptr)
	, m_ctrlHeight(nullptr)
	, m_Height(StringUtil::stringWX(inRun->GetHeight()))
	, m_textJudge(nullptr)
	, m_ctrlJudge(nullptr)
	, m_ctrlJudgeNote(nullptr)
	, m_Judge(StringUtil::stringWX(inRun->GetJudge()))
	, m_ctrlHandler(nullptr)
	, m_Handler(StringUtil::stringWX(inRun->GetHandler()))
	, m_ctrlConditions(nullptr)
	, m_Conditions(StringUtil::stringWX(inRun->GetConditions()))
	, m_ctrlDesc(nullptr)
	, m_ctrlPartnerEdit(nullptr)
	, m_ctrlPartner(nullptr)
	, m_ctrlSCTText(nullptr)
	, m_ctrlSCT(nullptr)
	, m_SCT(0.0)
	, m_textYardsReqOpeningPts(nullptr)
	, m_ctrlYardsReqOpeningPts(nullptr)
	, m_Opening(0)
	, m_Yards(0.0)
	, m_textMinYPSClosingTime(nullptr)
	, m_ctrlMinYPSClosingTime(nullptr)
	, m_SCT2(0.0)
	, m_ctrlClosingText(nullptr)
	, m_ctrlClosing(nullptr)
	, m_Closing(0)
	, m_ctrlObstaclesText(nullptr)
	, m_ctrlObstacles(nullptr)
	, m_Obstacles(inRun->GetScoring().GetObstacles())
	, m_ctrlTimeText(nullptr)
	, m_ctrlTime(nullptr)
	, m_Time(0.0)
	, m_ctrlFaultsText(nullptr)
	, m_ctrlFaults(nullptr)
	, m_Faults(0)
	, m_textYPSOpeningPts(nullptr)
	, m_ctrlYPSOpeningPts(nullptr)
	, m_Open(0)
	, m_textClosingPtsTotalFaults(nullptr)
	, m_ctrlClosingPtsTotalFaults(nullptr)
	, m_Close(0)
	, m_ctrlObstaclesPSText(nullptr)
	, m_ctrlObstaclesPS(nullptr)
	, m_textPlace(nullptr)
	, m_ctrlPlace(nullptr)
	, m_textPlaceOf(nullptr)
	, m_Place(inRun->GetPlace())
	, m_ctrlInClass(nullptr)
	, m_InClass(inRun->GetInClass())
	, m_textDogsQd(nullptr)
	, m_ctrlDogsQd(nullptr)
	, m_DogsQd(inRun->GetDogsQd())
	, m_ctrlQ(nullptr)
	, m_ctrlBonusPtsText(nullptr)
	, m_ctrlBonusTitlePts(nullptr)
	, m_BonusTitlePts(inRun->GetScoring().GetBonusTitlePts())
	, m_ctrlSpeedPtsText(nullptr)
	, m_ctrlSpeedPts(nullptr)
	, m_ctrlTitlePointsText(nullptr)
	, m_ctrlTitlePoints(nullptr)
	, m_ctrlScore(nullptr)
	, m_Comments(StringUtil::stringWX(inRun->GetNote()))
	, m_sortRefRuns(L"RefRuns")
	, m_idxRefRunPage(-1)
	, m_ctrlFaultsList(nullptr)
	, m_ctrlRefRuns(nullptr)
	, m_ctrlRefAddMe(nullptr)
	, m_ctrlRefEdit(nullptr)
	, m_ctrlRefDelete(nullptr)
	, m_CRCDDisplay(nullptr)
	, m_ctrlCourse(nullptr)
	, m_ctrlCRCDView(nullptr)
	, m_ctrlCRCDCopy(nullptr)
	, m_ctrlIncImage(nullptr)
	, m_ctrlLinks(nullptr)
	, m_ImageList()
	, m_imgEmpty(-1)
	, m_imgOk(-1)
	, m_imgMissing(-1)
	, m_ctrlLinkEdit(nullptr)
	, m_ctrlLinkDelete(nullptr)
	, m_ctrlLinkOpen(nullptr)
{
	SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY | GetExtraStyle());
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY,
			StringUtil::stringWX(pDoc->AddDogToCaption(StringUtil::stringW(_("IDS_RUN_PROPERTIES")))),
			wxDefaultPosition, wxDefaultSize,
			wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	m_ImageList.Create(DPI::Scale(this, 16), DPI::Scale(this, 16));

	m_clrBack = GetBackgroundColour();

	m_sortRefRuns.Initialize(scNumRefRunColumns);

	if (!m_Run->GetDate().IsValid())
		m_Date.SetToday();

	if (ARBScoringType::BySpeed != m_Run->GetScoring().GetType())
	{
		if (m_Judge.empty())
		{
			std::wstring last = CAgilityBookOptions::GetLastEnteredJudge();
			if (!last.empty())
				m_Judge = StringUtil::stringWX(last);
		}
	}

	if (m_Handler.empty())
	{
		std::wstring last = CAgilityBookOptions::GetLastEnteredHandler(m_pDog);
		if (!last.empty())
			m_Handler = StringUtil::stringWX(last);
	}

	switch (m_Run->GetScoring().GetType())
	{
	case ARBScoringType::Unknown:
		break;
	case ARBScoringType::ByTime:
		m_Faults = m_Run->GetScoring().GetCourseFaults();
		m_Time = m_Run->GetScoring().GetTime();
		m_Table = m_Run->GetScoring().HasTable();
		m_Yards = m_Run->GetScoring().GetYards();
		m_SCT = m_Run->GetScoring().GetSCT();
		break;
	case ARBScoringType::ByOpenClose:
		m_Faults = m_Run->GetScoring().GetCourseFaults();
		m_Time = m_Run->GetScoring().GetTime();
		m_Opening = m_Run->GetScoring().GetNeedOpenPts();
		m_Closing = m_Run->GetScoring().GetNeedClosePts();
		m_Open = m_Run->GetScoring().GetOpenPts();
		m_Close = m_Run->GetScoring().GetClosePts();
		m_SCT = m_Run->GetScoring().GetSCT();
		m_SCT2 = m_Run->GetScoring().GetSCT2();
		break;
	case ARBScoringType::ByPoints:
		m_Faults = m_Run->GetScoring().GetCourseFaults();
		m_Time = m_Run->GetScoring().GetTime();
		m_Opening = m_Run->GetScoring().GetNeedOpenPts();
		m_Open = m_Run->GetScoring().GetOpenPts();
		m_SCT = m_Run->GetScoring().GetSCT();
		break;
	case ARBScoringType::BySpeed:
		m_Faults = m_Run->GetScoring().GetCourseFaults();
		m_Time = m_Run->GetScoring().GetTime();
		m_Yards = m_Run->GetScoring().GetYards();
		break;
	}

	// Controls (these are done first to control tab order)

	wxNotebook* notebook = new wxNotebook(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, 0);
	notebook->Bind(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, &CDlgRun::OnPageChanging, this);
	notebook->Bind(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, &CDlgRun::OnPageChanged, this);

	// Score

	m_panelScore = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxStaticText* textDate = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_DATE"), wxDefaultPosition, wxDefaultSize, 0);
	textDate->Wrap(-1);

	wxDatePickerCtrl* ctrlDate = new wxDatePickerCtrl(m_panelScore, wxID_ANY, wxDefaultDateTime,
		wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN|wxDP_SHOWCENTURY,
		CGenericValidator(&m_Date));
	ctrlDate->Bind(wxEVT_DATE_CHANGED, &CDlgRun::OnScoreDateChanged, this);
	ctrlDate->SetHelpText(_("HIDC_RUNSCORE_DATE"));
	ctrlDate->SetToolTip(_("HIDC_RUNSCORE_DATE"));

	m_ctrlVenues = new wxComboBox(m_panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		0, nullptr, wxCB_DROPDOWN|wxCB_READONLY);
	m_ctrlVenues->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgRun::OnSelchangeVenue, this);
	m_ctrlVenues->SetHelpText(_("HIDC_RUNSCORE_VENUE"));
	m_ctrlVenues->SetToolTip(_("HIDC_RUNSCORE_VENUE"));
	for (auto pClub : inTrial->GetClubs())
	{
		if (pClub->GetPrimaryClub())
			continue;
		ARBConfigVenuePtr pVenue;
		m_pDoc->Book().GetConfig().GetVenues().FindVenue(pClub->GetVenue(), &pVenue);
		int index = m_ctrlVenues->Append(fmt::format(L"[{}] {}", pClub->GetVenue(), pClub->GetName()));
		m_ctrlVenues->SetClientObject(index, new CDlgDogVenueData(pClub, pVenue));
		if (m_Run->GetClub() && *(m_Run->GetClub()) == *pClub)
			m_ctrlVenues->SetSelection(index);
	}
	if (wxNOT_FOUND == m_ctrlVenues->GetSelection())
	{
		m_ctrlVenues->SetSelection(0);
		m_Run->SetClub(GetVenueData(0)->m_Club);
	}

	wxStaticText* textLocation = new wxStaticText(m_panelScore, wxID_ANY,
		Pad(m_pTrial->GetLocation()),
		wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	textLocation->Wrap(-1);

	wxStaticText* textDiv = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_DIVISION"),
		wxDefaultPosition, wxDefaultSize, 0);
	textDiv->Wrap(-1);

	m_ctrlDivisions = new wxComboBox(m_panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		0, nullptr, wxCB_DROPDOWN|wxCB_READONLY);
	m_ctrlDivisions->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgRun::OnSelchangeDivision, this);
	m_ctrlDivisions->SetHelpText(_("HIDC_RUNSCORE_DIVISION"));
	m_ctrlDivisions->SetToolTip(_("HIDC_RUNSCORE_DIVISION"));

	wxStaticText* textLevel = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_LEVEL"),
		wxDefaultPosition, wxDefaultSize, 0);
	textLevel->Wrap(-1);

	m_ctrlLevels = new wxComboBox(m_panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		0, nullptr, wxCB_DROPDOWN|wxCB_READONLY);
	m_ctrlLevels->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgRun::OnSelchangeLevel, this);
	m_ctrlLevels->SetHelpText(_("HIDC_RUNSCORE_LEVEL"));
	m_ctrlLevels->SetToolTip(_("HIDC_RUNSCORE_LEVEL"));

	wxStaticText* textEvent = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_EVENT"),
		wxDefaultPosition, wxDefaultSize, 0);
	textEvent->Wrap(-1);

	m_ctrlEvents = new wxComboBox(m_panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		0, nullptr, wxCB_DROPDOWN|wxCB_READONLY);
	m_ctrlEvents->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgRun::OnSelchangeEvent, this);
	m_ctrlEvents->SetHelpText(_("HIDC_RUNSCORE_EVENT"));
	m_ctrlEvents->SetToolTip(_("HIDC_RUNSCORE_EVENT"));

	m_ctrlSubNamesText = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_SUBNAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlSubNamesText->Wrap(-1);

	m_ctrlSubNames = new wxComboBox(m_panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		0, nullptr, wxCB_DROPDOWN,
		CTrimValidator(&m_SubName, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlSubNames->SetHelpText(_("HIDC_RUNSCORE_SUBNAME"));
	m_ctrlSubNames->SetToolTip(_("HIDC_RUNSCORE_SUBNAME"));

	m_ctrlTable = new wxCheckBox(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_TABLE"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_Table));
	m_ctrlTable->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgRun::OnBnClickedTableYps, this);
	m_ctrlTable->SetHelpText(_("HIDC_RUNSCORE_TABLE"));
	m_ctrlTable->SetToolTip(_("HIDC_RUNSCORE_TABLE"));

	m_textHeight = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_HEIGHT"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textHeight->Wrap(-1);

	m_ctrlHeight = new wxComboBox(m_panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition,
#ifdef __WXMAC__
		wxSize(wxDLG_UNIT_X(this, 50), -1), // Just not wide enough on a Mac...
#else
		wxSize(wxDLG_UNIT_X(this, 25), -1),
#endif
		0, nullptr, wxCB_DROPDOWN,
		CTrimValidator(&m_Height, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlHeight->SetHelpText(_("HIDC_RUNSCORE_HEIGHT"));
	m_ctrlHeight->SetToolTip(_("HIDC_RUNSCORE_HEIGHT"));
	std::set<std::wstring> names;
	m_pDoc->Book().GetAllHeights(names);
	std::set<std::wstring>::const_iterator iter;
	wxArrayString choices;
	for (iter = names.begin(); iter != names.end(); ++iter)
	{
		wxString wxName(StringUtil::stringWX(*iter));
		m_ctrlHeight->Append(wxName);
		choices.Add(wxName);
	}
	m_ctrlHeight->AutoComplete(choices);

	m_textJudge = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_JUDGE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textJudge->Wrap(-1);

	m_ctrlJudge = new wxComboBox(m_panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		0, nullptr, wxCB_DROPDOWN,
		CTrimValidator(&m_Judge, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlJudge->SetHelpText(_("HIDC_RUNSCORE_JUDGE"));
	m_ctrlJudge->SetToolTip(_("HIDC_RUNSCORE_JUDGE"));

	m_ctrlJudgeNote = new CNoteButton(m_panelScore);
	m_ctrlJudgeNote->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRun::OnJudgeNotes, this);
	m_ctrlJudgeNote->SetHelpText(_("HIDC_RUNSCORE_JUDGE_NOTES"));
	m_ctrlJudgeNote->SetToolTip(_("HIDC_RUNSCORE_JUDGE_NOTES"));

	wxStaticText* textHandler = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_HANDLER"),
		wxDefaultPosition, wxDefaultSize, 0);
	textHandler->Wrap(-1);

	m_ctrlHandler = new wxComboBox(m_panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		0, nullptr, wxCB_DROPDOWN,
		CTrimValidator(&m_Handler, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlHandler->SetHelpText(_("HIDC_RUNSCORE_HANDLER"));
	m_ctrlHandler->SetToolTip(_("HIDC_RUNSCORE_HANDLER"));
	m_pDoc->Book().GetAllHandlers(names);
	choices.Clear();
	for (iter = names.begin(); iter != names.end(); ++iter)
	{
		wxString wxName(StringUtil::stringWX(*iter));
		m_ctrlHandler->Append(wxName);
		choices.Add(wxName);
	}
	m_ctrlHandler->AutoComplete(choices);

	wxStaticText* textConditions = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_CONDITIONS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textConditions->Wrap(-1);

	m_ctrlConditions = new CSpellCheckCtrl(m_panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxDLG_UNIT(this, wxSize(200, 25)),
		wxTE_MULTILINE|wxTE_WORDWRAP,
		CTrimValidator(&m_Conditions, TRIMVALIDATOR_TRIM_BOTH));
	m_ctrlConditions->SetHelpText(_("HIDC_RUNSCORE_CONDITIONS"));
	m_ctrlConditions->SetToolTip(_("HIDC_RUNSCORE_CONDITIONS"));

	m_ctrlDesc = new CRichEditCtrl2(m_panelScore, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxSize(-1, wxDLG_UNIT_Y(this, 35)), true);
	m_ctrlDesc->SetHelpText(_("HIDC_RUNSCORE_DESC"));
	m_ctrlDesc->SetToolTip(_("HIDC_RUNSCORE_DESC"));

	m_ctrlPartnerEdit = new wxButton(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_PARTNERS_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlPartnerEdit->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRun::OnPartnersEdit, this);
	m_ctrlPartnerEdit->SetHelpText(_("HIDC_RUNSCORE_PARTNERS_EDIT"));
	m_ctrlPartnerEdit->SetToolTip(_("HIDC_RUNSCORE_PARTNERS_EDIT"));

	m_ctrlPartner = new CTextCtrl(m_panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 115), -1), wxTE_READONLY);
	m_ctrlPartner->SetBackgroundColour(m_clrBack);
	m_ctrlPartner->SetHelpText(_("HIDC_RUNSCORE_PARTNER"));
	m_ctrlPartner->SetToolTip(_("HIDC_RUNSCORE_PARTNER"));

	m_ctrlSCTText = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_SCT"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlSCTText->Wrap(-1);

	m_ctrlSCT = new CTextCtrl(m_panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 30), -1), 0,
		CGenericValidator(&m_SCT));
	m_ctrlSCT->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRun::OnSCTChange, this);
	m_ctrlSCT->SetHelpText(_("HIDC_RUNSCORE_SCT"));
	m_ctrlSCT->SetToolTip(_("HIDC_RUNSCORE_SCT"));

	m_textYardsReqOpeningPts = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_OPENING_PTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textYardsReqOpeningPts->Wrap(-1);

	m_ctrlYardsReqOpeningPts = new CTextCtrl(m_panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 30), -1), 0);
	m_ctrlYardsReqOpeningPts->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRun::OnReqOpeningYPSChange, this);
	m_ctrlYardsReqOpeningPts->SetHelpText(_("HIDC_RUNSCORE_OPENING_PTS"));
	m_ctrlYardsReqOpeningPts->SetToolTip(_("HIDC_RUNSCORE_OPENING_PTS"));

	m_textMinYPSClosingTime = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_MIN_YPS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textMinYPSClosingTime->Wrap(-1);

	m_ctrlMinYPSClosingTime = new CTextCtrl(m_panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 30), -1), 0);
	m_ctrlMinYPSClosingTime->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRun::OnClosingTimeChange, this);
	m_ctrlMinYPSClosingTime->SetHelpText(_("HIDC_RUNSCORE_SCT2"));
	m_ctrlMinYPSClosingTime->SetToolTip(_("HIDC_RUNSCORE_SCT2"));

	m_ctrlClosingText = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_CLOSING_PTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlClosingText->Wrap(-1);

	m_ctrlClosing = new CTextCtrl(m_panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 30), -1), 0,
		CGenericValidator(&m_Closing));
	m_ctrlClosing->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRun::OnReqClosingChange, this);
	m_ctrlClosing->SetHelpText(_("HIDC_RUNSCORE_CLOSING_PTS"));
	m_ctrlClosing->SetToolTip(_("HIDC_RUNSCORE_CLOSING_PTS"));

	m_ctrlObstaclesText = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_OBSTACLES"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlObstaclesText->Wrap(-1);

	m_ctrlObstacles = new CTextCtrl(m_panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 30), -1), 0,
		CGenericValidator(&m_Obstacles));
	m_ctrlObstacles->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRun::OnNumObsChange, this);
	m_ctrlObstacles->SetHelpText(_("HIDC_RUNSCORE_OBSTACLES"));
	m_ctrlObstacles->SetToolTip(_("HIDC_RUNSCORE_OBSTACLES"));

	m_ctrlTimeText = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_TIME"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlTimeText->Wrap(-1);

	m_ctrlTime = new CTextCtrl(m_panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 30), -1), 0,
		CGenericValidator(&m_Time));
	m_ctrlTime->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRun::OnTimeChange, this);
	m_ctrlTime->SetHelpText(_("HIDC_RUNSCORE_TIME"));
	m_ctrlTime->SetToolTip(_("HIDC_RUNSCORE_TIME"));

	m_ctrlFaultsText = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_FAULTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlFaultsText->Wrap(-1);

	m_ctrlFaults = new CTextCtrl(m_panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 30), -1), 0,
		CGenericValidator(&m_Faults));
	m_ctrlFaults->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRun::OnFaultsChange, this);
	m_ctrlFaults->SetHelpText(_("HIDC_RUNSCORE_FAULTS"));
	m_ctrlFaults->SetToolTip(_("HIDC_RUNSCORE_FAULTS"));

	m_textYPSOpeningPts = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_OPEN_PTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textYPSOpeningPts->Wrap(-1);

	m_ctrlYPSOpeningPts = new CTextCtrl(m_panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 30), -1), 0);
	m_ctrlYPSOpeningPts->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRun::OnOpenChange, this);
	m_ctrlYPSOpeningPts->SetHelpText(_("HIDC_RUNSCORE_OPEN_PTS"));
	m_ctrlYPSOpeningPts->SetToolTip(_("HIDC_RUNSCORE_OPEN_PTS"));

	m_textClosingPtsTotalFaults = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_CLOSE_PTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textClosingPtsTotalFaults->Wrap(-1);

	m_ctrlClosingPtsTotalFaults = new CTextCtrl(m_panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 30), -1), 0);
	m_ctrlClosingPtsTotalFaults->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRun::OnCloseChange, this);
	m_ctrlClosingPtsTotalFaults->SetHelpText(_("HIDC_RUNSCORE_CLOSE_PTS"));
	m_ctrlClosingPtsTotalFaults->SetToolTip(_("HIDC_RUNSCORE_CLOSE_PTS"));

	m_ctrlObstaclesPSText = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_OBSTACLES_PER_SEC"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlObstaclesPSText->Wrap(-1);

	m_ctrlObstaclesPS = new CTextCtrl(m_panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 30), -1), wxTE_READONLY);
	m_ctrlObstaclesPS->SetBackgroundColour(m_clrBack);
	m_ctrlObstaclesPS->SetHelpText(_("HIDC_RUNSCORE_OBSTACLES_PER_SEC"));
	m_ctrlObstaclesPS->SetToolTip(_("HIDC_RUNSCORE_OBSTACLES_PER_SEC"));

	m_textPlace = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_PLACE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textPlace->Wrap(-1);

	m_ctrlPlace = new CTextCtrl(m_panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 20), -1), 0,
		CGenericValidator(&m_Place));
	m_ctrlPlace->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRun::OnPlaceChange, this);
	m_ctrlPlace->SetHelpText(_("HIDC_RUNSCORE_PLACE"));
	m_ctrlPlace->SetToolTip(_("HIDC_RUNSCORE_PLACE"));

	m_textPlaceOf = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_IN_CLASS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textPlaceOf->Wrap(-1);

	m_ctrlInClass = new CTextCtrl(m_panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 20), -1), 0,
		CGenericValidator(&m_InClass));
	m_ctrlInClass->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRun::OnInClassChange, this);
	m_ctrlInClass->SetHelpText(_("HIDC_RUNSCORE_IN_CLASS"));
	m_ctrlInClass->SetToolTip(_("HIDC_RUNSCORE_IN_CLASS"));

	m_textDogsQd = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_DOGS_QD"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_textDogsQd->Wrap(-1);

	m_ctrlDogsQd = new CTextCtrl(m_panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 20), -1), 0,
		CGenericValidator(&m_DogsQd, -1));
	m_ctrlDogsQd->SetHelpText(_("HIDC_RUNSCORE_DOGS_QD"));
	m_ctrlDogsQd->SetToolTip(_("HIDC_RUNSCORE_DOGS_QD"));

	wxStaticText* textQ = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_Q"),
		wxDefaultPosition, wxDefaultSize, 0);
	textQ->Wrap(-1);

	m_ctrlQ = new CQualifyingComboBox(m_panelScore, m_Run);
	m_ctrlQ->Bind(wxEVT_COMMAND_CHOICE_SELECTED, &CDlgRun::OnSelchangeQ, this);
	m_ctrlQ->SetHelpText(_("HIDC_RUNSCORE_Q"));
	m_ctrlQ->SetToolTip(_("HIDC_RUNSCORE_Q"));

	m_ctrlBonusPtsText = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_BONUSPTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlBonusPtsText->Wrap(-1);

	m_ctrlBonusTitlePts = new CTextCtrl(m_panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 30), -1), 0,
		CGenericValidator(&m_BonusTitlePts, -1));
	m_ctrlBonusTitlePts->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgRun::OnBonusChange, this);
	m_ctrlBonusTitlePts->SetHelpText(_("HIDC_RUNSCORE_BONUSTITLEPTS"));
	m_ctrlBonusTitlePts->SetToolTip(_("HIDC_RUNSCORE_BONUSTITLEPTS"));

	m_ctrlSpeedPtsText = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_SPEEDPTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlSpeedPtsText->Wrap(-1);

	m_ctrlSpeedPts = new CTextCtrl(m_panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 30), -1), wxTE_READONLY);
	m_ctrlSpeedPts->SetBackgroundColour(m_clrBack);

	m_ctrlTitlePointsText = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_TITLE_POINTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlTitlePointsText->Wrap(-1);

	m_ctrlTitlePoints = new CTextCtrl(m_panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 30), -1), wxTE_READONLY);
	m_ctrlTitlePoints->SetBackgroundColour(m_clrBack);

	wxStaticText* textScore = new wxStaticText(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_SCORE"),
		wxDefaultPosition, wxDefaultSize, 0);
	textScore->Wrap(-1);

	m_ctrlScore = new CTextCtrl(m_panelScore, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 30), -1), wxTE_READONLY);
	m_ctrlScore->SetBackgroundColour(m_clrBack);

	wxButton* btnOtherPoints = new wxButton(m_panelScore, wxID_ANY,
		_("IDC_RUNSCORE_OTHERPOINTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnOtherPoints->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRun::OnOtherpoints, this);
	btnOtherPoints->SetHelpText(_("HIDC_RUNSCORE_OTHERPOINTS"));
	btnOtherPoints->SetToolTip(_("HIDC_RUNSCORE_OTHERPOINTS"));
	if (0 < m_pDoc->Book().GetConfig().GetOtherPoints().size())
		btnOtherPoints->Enable(true);
	else
		btnOtherPoints->Enable(false);

	// Comments

	wxPanel* panelComments = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	CSpellCheckCtrl* ctrlComments = new CSpellCheckCtrl(panelComments, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_WORDWRAP,
		CTrimValidator(&m_Comments, TRIMVALIDATOR_TRIM_BOTH));
	ctrlComments->SetHelpText(_("HIDC_RUNCOMMENT_COMMENTS"));
	ctrlComments->SetToolTip(_("HIDC_RUNCOMMENT_COMMENTS"));

	m_ctrlFaultsList = new wxListBox(panelComments, wxID_ANY,
		wxDefaultPosition, wxDefaultSize,
		0, nullptr, 0);
	m_ctrlFaultsList->SetHelpText(_("HIDC_RUNCOMMENT_FAULTS_LIST"));
	m_ctrlFaultsList->SetToolTip(_("HIDC_RUNCOMMENT_FAULTS_LIST"));

	wxButton* btnFaults = new wxButton(panelComments, wxID_ANY,
		_("IDC_RUNCOMMENT_COMMENTS_FAULTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnFaults->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRun::OnCommentsFaults, this);
	btnFaults->SetHelpText(_("HIDC_RUNCOMMENT_COMMENTS_FAULTS"));
	btnFaults->SetToolTip(_("HIDC_RUNCOMMENT_COMMENTS_FAULTS"));

	// RefRuns

	wxPanel* panelRefRuns = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	m_ctrlRefRuns = new CReportListCtrl(panelRefRuns,
		true, CReportListCtrl::SortHeader::Sort, true);
	m_ctrlRefRuns->Bind(wxEVT_COMMAND_LIST_COL_CLICK, &CDlgRun::OnRefRunColumnClick, this);
	m_ctrlRefRuns->Bind(wxEVT_COMMAND_LIST_ITEM_SELECTED, &CDlgRun::OnRefRunItemSelected, this);
	m_ctrlRefRuns->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &CDlgRun::OnRefRunItemActivated, this);
	m_ctrlRefRuns->Bind(wxEVT_KEY_DOWN, &CDlgRun::OnRefRunKeyDown, this);
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
	btnRefNew->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRun::OnRefRunNew, this);
	btnRefNew->SetHelpText(_("HIDC_RUNREF_NEW"));
	btnRefNew->SetToolTip(_("HIDC_RUNREF_NEW"));

	m_ctrlRefAddMe = new wxButton(panelRefRuns, wxID_ANY,
		_("IDC_RUNREF_ADDDOG"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlRefAddMe->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRun::OnRefRunAddMe, this);
	m_ctrlRefAddMe->SetHelpText(_("HIDC_RUNREF_ADDDOG"));
	m_ctrlRefAddMe->SetToolTip(_("HIDC_RUNREF_ADDDOG"));

	m_ctrlRefEdit = new wxButton(panelRefRuns, wxID_ANY,
		_("IDC_RUNREF_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlRefEdit->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRun::OnRefRunEdit, this);
	m_ctrlRefEdit->SetHelpText(_("HIDC_RUNREF_EDIT"));
	m_ctrlRefEdit->SetToolTip(_("HIDC_RUNREF_EDIT"));

	m_ctrlRefDelete = new wxButton(panelRefRuns, wxID_ANY,
		_("IDC_RUNREF_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlRefDelete->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRun::OnRefRunDelete, this);
	m_ctrlRefDelete->SetHelpText(_("HIDC_RUNREF_DELETE"));
	m_ctrlRefDelete->SetToolTip(_("HIDC_RUNREF_DELETE"));

	// CRCD

	wxPanel* panelCRCD = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxStaticText* textCRCD = new wxStaticText(panelCRCD, wxID_ANY,
		_("IDC_RUN_CRCD"),
		wxDefaultPosition, wxDefaultSize, 0);
	textCRCD->Wrap(wxDLG_UNIT_X(this, 300));

	m_CRCDDisplay = new CMetaDataDisplay(panelCRCD, m_Run);

	if (m_CRCDDisplay->HasMetafileSupport())
	{
		m_ctrlCourse = new wxButton(panelCRCD, wxID_ANY,
			_("IDC_RUNCRCD_EDIT"),
			wxDefaultPosition, wxDefaultSize, 0);
		m_ctrlCourse->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRun::OnCRCDEdit, this);
		m_ctrlCourse->SetHelpText(_("HIDC_RUNCRCD_EDIT"));
		m_ctrlCourse->SetToolTip(_("HIDC_RUNCRCD_EDIT"));

		m_ctrlCRCDView = new wxButton(panelCRCD, wxID_ANY,
			_("IDC_RUNCRCD_VIEW"),
			wxDefaultPosition, wxDefaultSize, 0);
		m_ctrlCRCDView->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRun::OnCRCDView, this);
		m_ctrlCRCDView->SetHelpText(_("HIDC_RUNCRCD_VIEW"));
		m_ctrlCRCDView->SetToolTip(_("HIDC_RUNCRCD_VIEW"));
	}

	m_ctrlCRCDCopy = new wxButton(panelCRCD, wxID_ANY,
		_("IDC_RUNCRCD_COPY"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlCRCDCopy->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRun::OnCRCDCopy, this);
	m_ctrlCRCDCopy->SetHelpText(_("HIDC_RUNCRCD_COPY"));
	m_ctrlCRCDCopy->SetToolTip(_("HIDC_RUNCRCD_COPY"));

	wxStaticText* textImageDesc = nullptr;
	if (m_CRCDDisplay->HasMetafileSupport())
	{
		m_ctrlIncImage = new wxCheckBox(panelCRCD, wxID_ANY,
			_("IDC_RUNCRCD_IMAGE"),
			wxDefaultPosition, wxDefaultSize, 0);
		m_ctrlIncImage->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgRun::OnCRCDImage, this);
		m_ctrlIncImage->SetHelpText(_("HIDC_RUNCRCD_IMAGE"));
		m_ctrlIncImage->SetToolTip(_("HIDC_RUNCRCD_IMAGE"));
		bool setCheck = CAgilityBookOptions::GetIncludeCRCDImage();
		if (m_CRCDDisplay->HasMetafile())
			setCheck = true;
		m_ctrlIncImage->SetValue(setCheck);

		textImageDesc = new wxStaticText(panelCRCD, wxID_ANY,
			_("IDC_RUNCRCD_IMAGE_TEXT"),
			wxDefaultPosition, wxDefaultSize, 0);
		textImageDesc->Wrap(wxDLG_UNIT_X(this, 50));
	}

	// Links

	wxPanel* panelLinks = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	m_ctrlLinks = new CListCtrl(panelLinks, wxID_ANY,
		wxDefaultPosition, wxDefaultSize,
		wxLC_NO_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL);
	m_ctrlLinks->SetImageList(&m_ImageList, wxIMAGE_LIST_SMALL);
	m_imgEmpty = m_ImageList.Add(CImageManager::Get()->GetIcon(ImageMgrBlank));
	m_imgOk = m_ImageList.Add(CImageManager::Get()->GetIcon(ImageMgrCheck));
	m_imgMissing = m_ImageList.Add(CImageManager::Get()->GetIcon(ImageMgrQuestion));
	m_ctrlLinks->Bind(wxEVT_COMMAND_LIST_ITEM_SELECTED, &CDlgRun::OnLinksItemSelected, this);
	m_ctrlLinks->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &CDlgRun::OnLinksItemActivated, this);
	m_ctrlLinks->Bind(wxEVT_KEY_DOWN, &CDlgRun::OnLinksKeyDown, this);
	m_ctrlLinks->SetHelpText(_("HIDC_RUNLINK_LIST"));
	m_ctrlLinks->SetToolTip(_("HIDC_RUNLINK_LIST"));
	m_ctrlLinks->InsertColumn(0, L"");
#if wxUSE_DRAG_AND_DROP
	m_ctrlLinks->SetDropTarget(new CLinkDropTarget(this));
#endif

	wxButton* btnLinkNew = new wxButton(panelLinks, wxID_ANY,
		_("IDC_RUNLINK_NEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnLinkNew->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRun::OnLinksNew, this);
	btnLinkNew->SetHelpText(_("HIDC_RUNLINK_NEW"));
	btnLinkNew->SetToolTip(_("HIDC_RUNLINK_NEW"));

	m_ctrlLinkEdit = new wxButton(panelLinks, wxID_ANY,
		_("IDC_RUNLINK_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlLinkEdit->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRun::OnLinksEdit, this);
	m_ctrlLinkEdit->SetHelpText(_("HIDC_RUNLINK_EDIT"));
	m_ctrlLinkEdit->SetToolTip(_("HIDC_RUNLINK_EDIT"));

	m_ctrlLinkDelete = new wxButton(panelLinks, wxID_ANY,
		_("IDC_RUNLINK_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlLinkDelete->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRun::OnLinksDelete, this);
	m_ctrlLinkDelete->SetHelpText(_("HIDC_RUNLINK_DELETE"));
	m_ctrlLinkDelete->SetToolTip(_("HIDC_RUNLINK_DELETE"));

	m_ctrlLinkOpen = new wxButton(panelLinks, wxID_ANY,
		_("IDC_RUNLINK_OPEN"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlLinkOpen->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRun::OnLinksOpen, this);
	m_ctrlLinkOpen->SetHelpText(_("HIDC_RUNLINK_OPEN"));
	m_ctrlLinkOpen->SetToolTip(_("HIDC_RUNLINK_OPEN"));

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerScorePanel = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerDate = new wxBoxSizer(wxHORIZONTAL);
	sizerDate->Add(textDate, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerDate->Add(ctrlDate, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerDate->Add(m_ctrlVenues, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerDate->Add(textLocation, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerScorePanel->Add(sizerDate, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerDivHt = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerDivHtTable = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* sizerEvent = new wxFlexGridSizer(4, 2, 0, 0);
	sizerEvent->SetFlexibleDirection(wxBOTH);
	sizerEvent->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerEvent->Add(textDiv, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerEvent->Add(m_ctrlDivisions, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0);
	sizerEvent->Add(textLevel, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	sizerEvent->Add(m_ctrlLevels, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxTOP, wxDLG_UNIT_X(this, 5));
	sizerEvent->Add(textEvent, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	sizerEvent->Add(m_ctrlEvents, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxTOP, wxDLG_UNIT_X(this, 5));
	sizerEvent->Add(m_ctrlSubNamesText, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	sizerEvent->Add(m_ctrlSubNames, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxTOP, wxDLG_UNIT_X(this, 5));

	sizerDivHtTable->Add(sizerEvent, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 0);
	sizerDivHtTable->Add(m_ctrlTable, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	sizerDivHt->Add(sizerDivHtTable, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerHtCond = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerHt = new wxBoxSizer(wxHORIZONTAL);
	sizerHt->Add(m_textHeight, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerHt->Add(m_ctrlHeight, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerHt->Add(m_textJudge, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerHt->Add(m_ctrlJudge, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerHt->Add(m_ctrlJudgeNote, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerHt->Add(textHandler, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerHt->Add(m_ctrlHandler, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 0);

	sizerHtCond->Add(sizerHt, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 0);

	wxBoxSizer* sizerCond = new wxBoxSizer(wxHORIZONTAL);
	sizerCond->Add(textConditions, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerCond->Add(m_ctrlConditions, 1, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 0);

	sizerHtCond->Add(sizerCond, 1, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerComments = new wxBoxSizer(wxHORIZONTAL);
	sizerComments->Add(m_ctrlDesc, 1, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 0);

	wxBoxSizer* sizerPartner = new wxBoxSizer(wxVERTICAL);
	sizerPartner->Add(m_ctrlPartnerEdit, 0, 0, 0);
	sizerPartner->Add(m_ctrlPartner, 0, wxTOP, wxDLG_UNIT_X(this, 3));

	sizerComments->Add(sizerPartner, 0, wxEXPAND | wxLEFT, wxDLG_UNIT_X(this, 5));

	sizerHtCond->Add(sizerComments, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND | wxTOP, wxDLG_UNIT_X(this, 5));

	sizerDivHt->Add(sizerHtCond, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 0);

	sizerScorePanel->Add(sizerDivHt, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxFlexGridSizer* sizerResults = new wxFlexGridSizer(5, 4, 0, 0);
	sizerResults->SetFlexibleDirection(wxBOTH);
	sizerResults->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxBoxSizer* sizerSCT = new wxBoxSizer(wxHORIZONTAL);
	sizerSCT->Add(m_ctrlSCTText, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerSCT->Add(m_ctrlSCT, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerSCT, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerTime = new wxBoxSizer(wxHORIZONTAL);
	sizerTime->Add(m_ctrlTimeText, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerTime->Add(m_ctrlTime, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerTime, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerPlace = new wxBoxSizer(wxHORIZONTAL);
	sizerPlace->Add(m_textPlace, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerPlace->Add(m_ctrlPlace, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerPlace->Add(m_textPlaceOf, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerPlace->Add(m_ctrlInClass, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerPlace, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerBonus = new wxBoxSizer(wxHORIZONTAL);
	sizerBonus->Add(m_ctrlBonusPtsText, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerBonus->Add(m_ctrlBonusTitlePts, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerBonus, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT, 0);

	wxBoxSizer* sizerYardsReqOpeningPts = new wxBoxSizer(wxHORIZONTAL);
	sizerYardsReqOpeningPts->Add(m_textYardsReqOpeningPts, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerYardsReqOpeningPts->Add(m_ctrlYardsReqOpeningPts, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerYardsReqOpeningPts, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerFaults = new wxBoxSizer(wxHORIZONTAL);
	sizerFaults->Add(m_ctrlFaultsText, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerFaults->Add(m_ctrlFaults, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerFaults, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerDogsQd = new wxBoxSizer(wxHORIZONTAL);
	sizerDogsQd->Add(m_textDogsQd, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerDogsQd->Add(m_ctrlDogsQd, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerDogsQd, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerSpeed = new wxBoxSizer(wxHORIZONTAL);
	sizerSpeed->Add(m_ctrlSpeedPtsText, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerSpeed->Add(m_ctrlSpeedPts, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerSpeed, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerMinYPSClosingTime = new wxBoxSizer(wxHORIZONTAL);
	sizerMinYPSClosingTime->Add(m_textMinYPSClosingTime, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerMinYPSClosingTime->Add(m_ctrlMinYPSClosingTime, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerMinYPSClosingTime, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerYPSOpeningPts = new wxBoxSizer(wxHORIZONTAL);
	sizerYPSOpeningPts->Add(m_textYPSOpeningPts, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerYPSOpeningPts->Add(m_ctrlYPSOpeningPts, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerYPSOpeningPts, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerQ = new wxBoxSizer(wxHORIZONTAL);
	sizerQ->Add(textQ, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerQ->Add(m_ctrlQ, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerQ, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerTitlePts = new wxBoxSizer(wxHORIZONTAL);
	sizerTitlePts->Add(m_ctrlTitlePointsText, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerTitlePts->Add(m_ctrlTitlePoints, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerTitlePts, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerReqClosingPts = new wxBoxSizer(wxHORIZONTAL);
	sizerReqClosingPts->Add(m_ctrlClosingText, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerReqClosingPts->Add(m_ctrlClosing, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerReqClosingPts, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerClosingPtsTotalFaults = new wxBoxSizer(wxHORIZONTAL);
	sizerClosingPtsTotalFaults->Add(m_textClosingPtsTotalFaults, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerClosingPtsTotalFaults->Add(m_ctrlClosingPtsTotalFaults, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerClosingPtsTotalFaults, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	sizerResults->Add(0, 0, 1, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 0);

	wxBoxSizer* sizerScore = new wxBoxSizer(wxHORIZONTAL);
	sizerScore->Add(textScore, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerScore->Add(m_ctrlScore, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerScore, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerNumObs = new wxBoxSizer(wxHORIZONTAL);
	sizerNumObs->Add(m_ctrlObstaclesText, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerNumObs->Add(m_ctrlObstacles, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerNumObs, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerObsSec = new wxBoxSizer(wxHORIZONTAL);
	sizerObsSec->Add(m_ctrlObstaclesPSText, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerObsSec->Add(m_ctrlObstaclesPS, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL, 0);

	sizerResults->Add(sizerObsSec, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_RIGHT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	sizerResults->Add(0, 0, 1, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 0);
	sizerResults->Add(btnOtherPoints, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	sizerScorePanel->Add(sizerResults, 1, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, wxDLG_UNIT_X(this, 5));

	m_panelScore->SetSizer(sizerScorePanel);
	m_panelScore->Layout();
	sizerScorePanel->Fit(m_panelScore);
	notebook->AddPage(m_panelScore, _("IDD_RUN_SCORE"), false);

	wxBoxSizer* sizerCommentsFaults = new wxBoxSizer(wxHORIZONTAL);
	sizerCommentsFaults->Add(ctrlComments, 1, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerFaultsList = new wxBoxSizer(wxVERTICAL);
	sizerFaultsList->Add(m_ctrlFaultsList, 1, wxEXPAND, 0);
	sizerFaultsList->Add(btnFaults, 0, wxTOP, wxDLG_UNIT_X(this, 5));

	sizerCommentsFaults->Add(sizerFaultsList, 0, wxEXPAND | wxRIGHT | wxTOP | wxBOTTOM, wxDLG_UNIT_X(this, 5));

	panelComments->SetSizer(sizerCommentsFaults);
	panelComments->Layout();
	sizerCommentsFaults->Fit(panelComments);
	notebook->AddPage(panelComments, _("IDD_RUN_COMMENTS"), false);

	wxBoxSizer* sizerRefRuns = new wxBoxSizer(wxVERTICAL);
	sizerRefRuns->Add(m_ctrlRefRuns, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerRefBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerRefBtns->Add(btnRefNew, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerRefBtns->Add(m_ctrlRefAddMe, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerRefBtns->Add(m_ctrlRefEdit, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerRefBtns->Add(m_ctrlRefDelete, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerRefRuns->Add(sizerRefBtns, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	panelRefRuns->SetSizer(sizerRefRuns);
	panelRefRuns->Layout();
	sizerRefRuns->Fit(panelRefRuns);
	notebook->AddPage(panelRefRuns, _("IDD_RUN_REFERENCE"), false);
	m_idxRefRunPage = static_cast<int>(notebook->GetPageCount()) - 1;

	wxBoxSizer* sizerCRCD = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerDisplay = new wxBoxSizer(wxVERTICAL);
	sizerDisplay->Add(textCRCD, 0, wxEXPAND, 0);
	sizerDisplay->Add(m_CRCDDisplay, 1, wxEXPAND | wxTOP, wxDLG_UNIT_X(this, 5));

	sizerCRCD->Add(sizerDisplay, 1, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerBtnsCRCD = new wxBoxSizer(wxVERTICAL);
	if (m_ctrlCourse)
		sizerBtnsCRCD->Add(m_ctrlCourse, 0, wxBOTTOM, wxDLG_UNIT_X(this, 3));
	if (m_ctrlCRCDView)
		sizerBtnsCRCD->Add(m_ctrlCRCDView, 0, wxBOTTOM, wxDLG_UNIT_X(this, 3));
	sizerBtnsCRCD->Add(m_ctrlCRCDCopy, 0, wxBOTTOM, wxDLG_UNIT_X(this, 3));
	if (m_ctrlIncImage)
		sizerBtnsCRCD->Add(m_ctrlIncImage, 0, wxBOTTOM, 3);
	if (textImageDesc)
		sizerBtnsCRCD->Add(textImageDesc, 0, wxBOTTOM, wxDLG_UNIT_X(this, 3));

	sizerCRCD->Add(sizerBtnsCRCD, 0, wxEXPAND | wxRIGHT | wxTOP | wxBOTTOM, wxDLG_UNIT_X(this, 5));

	panelCRCD->SetSizer(sizerCRCD);
	panelCRCD->Layout();
	sizerCRCD->Fit(panelCRCD);
	notebook->AddPage(panelCRCD, _("IDD_RUN_CRCD"), false);

	wxBoxSizer* sizerLinks = new wxBoxSizer(wxVERTICAL);
	sizerLinks->Add(m_ctrlLinks, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerLinkBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerLinkBtns->Add(btnLinkNew, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerLinkBtns->Add(m_ctrlLinkEdit, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerLinkBtns->Add(m_ctrlLinkDelete, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerLinkBtns->Add(0, 0, 1, wxEXPAND, 0);
	sizerLinkBtns->Add(m_ctrlLinkOpen, 0, 0, 0);

	sizerLinks->Add(sizerLinkBtns, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	panelLinks->SetSizer(sizerLinks);
	panelLinks->Layout();
	sizerLinks->Fit(panelLinks);
	notebook->AddPage(panelLinks, _("IDD_RUN_LINK"), false);

	bSizer->Add(notebook, 1, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	wxSizer* sdbSizer = CreateButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	UpdateVenue(false); // This will call UpdateControls();
	FillJudges();
	SetPartnerText();
	SetFaultsText();
	ListRefRuns();
	UpdateCRCDButtons();
	ListLinkFiles(nullptr);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	std::vector<wxWindow*> ctrls;
	ctrls.push_back(ctrlDate);
	ctrls.push_back(ctrlComments);
	ctrls.push_back(m_ctrlRefRuns);
	ctrls.push_back(m_CRCDDisplay);
	ctrls.push_back(m_ctrlLinks);
	IMPLEMENT_ON_INIT(CDlgRun, ctrls[iSelectPage])

	notebook->ChangeSelection(iSelectPage);
}


DEFINE_ON_INIT(CDlgRun)


CDlgDogVenueData* CDlgRun::GetVenueData(int index) const
{
	return dynamic_cast<CDlgDogVenueData*>(m_ctrlVenues->GetClientObject(index));
}


CDlgDogDivData* CDlgRun::GetDivisionData(int index) const
{
	return dynamic_cast<CDlgDogDivData*>(m_ctrlDivisions->GetClientObject(index));
}


CDlgDogLevelData* CDlgRun::GetLevelData(int index) const
{
	return dynamic_cast<CDlgDogLevelData*>(m_ctrlLevels->GetClientObject(index));
}


bool CDlgRun::GetEvent(ARBConfigEventPtr* outEvent) const
{
	if (outEvent)
		outEvent->reset();
	int idxVenue = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND == idxVenue)
		return false;
	int idxEvent = m_ctrlEvents->GetSelection();
	if (wxNOT_FOUND == idxEvent)
		return false;
	std::wstring str = StringUtil::stringW(m_ctrlEvents->GetString(idxEvent));
	if (str.empty())
		return false;
	ARBConfigVenuePtr pVenue = GetVenueData(idxVenue)->m_Venue;
	return pVenue->GetEvents().FindEvent(str, outEvent);
}


bool CDlgRun::GetScoring(ARBConfigScoringPtr* outScoring) const
{
	bool bFound = false;
	if (outScoring)
		outScoring->reset();
	std::wstring div, level;
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


void CDlgRun::UpdateVenue(bool bOnEventChange)
{
	int index = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND == index)
		return;
	CDlgDogVenueData* pData = GetVenueData(index);

	if (m_ctrlHeight->IsShown()
	&& ARBScoringType::BySpeed != m_Run->GetScoring().GetType())
	{
		if (bOnEventChange || m_Height.empty())
		{
			std::wstring last;
			last = CAgilityBookOptions::GetLastEnteredHeight(m_pDog, pData->m_Venue);
			if (!last.empty())
			{
				m_Height = StringUtil::stringWX(last);
				if (bOnEventChange)
					m_ctrlHeight->SetLabel(m_Height);
			}
		}
	}

	FillDivisions(bOnEventChange);
}


void CDlgRun::FillDivisions(bool bOnEventChange)
{
	int index = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND == index)
        return;
	ARBConfigVenuePtr pVenue = GetVenueData(index)->m_Venue;

	std::wstring div;
	index = m_ctrlDivisions->GetSelection();
	if (wxNOT_FOUND != index)
	{
		div = m_ctrlDivisions->GetString(index);
	}
	if (div.empty())
		div = m_Run->GetDivision();
	m_ctrlDivisions->Clear();

	for (ARBConfigDivisionList::const_iterator iterDiv = pVenue->GetDivisions().begin();
		iterDiv != pVenue->GetDivisions().end();
		++iterDiv)
	{
		ARBConfigDivisionPtr pDiv = (*iterDiv);
		// Does this division have any events possible on this date?
		for (ARBConfigEventList::const_iterator iterEvent = pVenue->GetEvents().begin();
			iterEvent != pVenue->GetEvents().end();
			++iterEvent)
		{
			if ((*iterEvent)->VerifyEvent(pDiv->GetName(), WILDCARD_LEVEL, m_Run->GetDate()))
			{
				index = m_ctrlDivisions->Append(StringUtil::stringWX(pDiv->GetName()));
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
		index = m_ctrlDivisions->FindString(StringUtil::stringWX(div), true);
		if (0 <= index)
			m_ctrlDivisions->SetSelection(index);
	}
	// Then try to find the last entered
	if (wxNOT_FOUND == m_ctrlDivisions->GetSelection())
	{
		std::wstring last = CAgilityBookOptions::GetLastEnteredDivision(m_pDog, pVenue);
		if (0 < last.length())
		{
			index = m_ctrlDivisions->FindString(StringUtil::stringWX(last), true);
			if (0 <= index)
				m_ctrlDivisions->SetSelection(index);
		}
	}
	if (wxNOT_FOUND == m_ctrlDivisions->GetSelection())
	{
		if (1 == m_ctrlDivisions->GetCount())
			m_ctrlDivisions->SetSelection(0);
	}
	// Force the division into the run. SetSelection will not cause an update.
	int idxDiv = m_ctrlDivisions->GetSelection();
	if (wxNOT_FOUND != idxDiv)
		m_Run->SetDivision(StringUtil::stringW(m_ctrlDivisions->GetString(idxDiv)));
	FillLevels(bOnEventChange);
}


void CDlgRun::FillLevels(bool bOnEventChange)
{
	int index = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND == index)
		return;
	ARBConfigVenuePtr pVenue = GetVenueData(index)->m_Venue;

	std::wstring level;
	index = m_ctrlLevels->GetSelection();
	if (wxNOT_FOUND != index)
	{
		level = m_ctrlLevels->GetString(index);
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
			for (ARBConfigEventList::const_iterator iterEvent = pVenue->GetEvents().begin();
				iterEvent != pVenue->GetEvents().end();
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
							int idx = m_ctrlLevels->Append(StringUtil::stringWX(pSubLevel->GetName()));
							m_ctrlLevels->SetClientObject(idx, new CDlgDogLevelData(pLevel, pSubLevel));
							if (level == pSubLevel->GetName())
								m_ctrlLevels->SetSelection(idx);
						}
					}
					else
					{
						int idx = m_ctrlLevels->Append(StringUtil::stringWX(pLevel->GetName()));
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
			std::wstring last = CAgilityBookOptions::GetLastEnteredLevel(m_pDog, pVenue);
			if (0 < last.length())
			{
				int idx = m_ctrlLevels->FindString(StringUtil::stringWX(last), true);
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
	int idxLvl = m_ctrlLevels->GetSelection();
	if (wxNOT_FOUND != idxLvl)
		m_Run->SetLevel(StringUtil::stringW(m_ctrlLevels->GetString(idxLvl)));
	FillEvents(bOnEventChange);
	SetTitlePoints();
}


void CDlgRun::FillEvents(bool bOnEventChange)
{
	int index = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND == index)
		return;
	ARBConfigVenuePtr pVenue = GetVenueData(index)->m_Venue;

	std::wstring evt;
	index = m_ctrlEvents->GetSelection();
	if (wxNOT_FOUND != index)
	{
		evt = m_ctrlEvents->GetString(index);
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
			ARBConfigEventPtr pLastAddedEvent;
			CDlgDogLevelData* pData = GetLevelData(idxLevel);
			for (ARBConfigEventList::const_iterator iter = pVenue->GetEvents().begin();
				iter != pVenue->GetEvents().end();
				++iter)
			{
				ARBConfigEventPtr pEvent = (*iter);
				if (pEvent->FindEvent(pDiv->GetName(), pData->m_pLevel->GetName(), m_Run->GetDate()))
				{
					int idx = m_ctrlEvents->Append(StringUtil::stringWX(pEvent->GetName()));
					if (evt == pEvent->GetName())
					{
						m_ctrlEvents->SetSelection(idx);
						SetEventDesc(pEvent);
					}
					else
					{
						// If we're not looking for an event (and haven't found one!),
						// remember this.
						pLastAddedEvent = pEvent;
					}
				}
			}
			// If there's only one event, auto-select it.
			if (pLastAddedEvent && 1 == m_ctrlEvents->GetCount() && 0 > m_ctrlEvents->GetSelection())
			{
				m_ctrlEvents->SetSelection(0);
				SetEventDesc(pLastAddedEvent);
				m_Run->SetEvent(pLastAddedEvent->GetName());
			}
		}
	}
	FillSubNames();
	UpdateControls(bOnEventChange);
}


void CDlgRun::FillSubNames()
{
	ARBConfigEventPtr pEvent;
	ARBConfigScoringPtr pScoring;
	if (GetEvent(&pEvent) && GetScoring(&pScoring))
	{
		if (pScoring->HasSubNames())
		{
            int index = m_ctrlVenues->GetSelection();
            if (wxNOT_FOUND == index)
                return;
            ARBConfigVenuePtr pVenue = GetVenueData(index)->m_Venue;

			std::set<std::wstring> names;
			m_pDoc->Book().GetAllEventSubNames(pVenue->GetName(), pEvent, names);
			m_ctrlSubNames->Clear();
			wxArrayString choices;
			for (std::set<std::wstring>::const_iterator iter = names.begin();
				iter != names.end();
				++iter)
			{
				wxString wxName(StringUtil::stringWX(*iter));
				m_ctrlSubNames->Append(wxName);
				choices.Add(wxName);
			}
			m_ctrlSubNames->AutoComplete(choices);
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
	std::set<std::wstring> names;
	m_pDoc->Book().GetAllJudges(names, true, true);
	if (!m_Run->GetJudge().empty())
		names.insert(m_Run->GetJudge());
	m_ctrlJudge->Clear();
	wxArrayString choices;
	for (std::set<std::wstring>::const_iterator iter = names.begin(); iter != names.end(); ++iter)
	{
		wxString wxName(StringUtil::stringWX(*iter));
		m_ctrlJudge->Append(wxName);
		choices.Add(wxName);
	}
	m_ctrlJudge->AutoComplete(choices);
	m_ctrlJudge->SetValue(m_Judge);
}


void CDlgRun::SetEventDesc(ARBConfigEventPtr const& inEvent)
{
	wxString desc;
	if (inEvent)
		desc += StringUtil::stringWX(inEvent->GetDesc());
	ARBConfigScoringPtr pScoring;
	if (GetScoring(&pScoring))
	{
		std::wstring const& note = pScoring->GetNote();
		if (!desc.empty() && 0 < note.length())
			desc += L"\n==========\n";
		desc += StringUtil::stringWX(note);
	}
	m_ctrlDesc->ChangeValue(desc);
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
					partners += L", ";
				partners += StringUtil::stringWX((*iter)->GetHandler());
				partners += L"/";
				partners += StringUtil::stringWX((*iter)->GetDog());
			}
		}
	}
	m_ctrlPartner->ChangeValue(partners);
}


void CDlgRun::SetMinYPS()
{
	if (ARBScoringType::ByTime == m_Run->GetScoring().GetType())
	{
		wxString str;
		double yps;
		if (m_Run->GetScoring().GetMinYPS(CAgilityBookOptions::GetTableInYPS(), yps))
		{
			str = StringUtil::stringWX(ARBDouble::ToString(yps, 3));
		}
		m_ctrlMinYPSClosingTime->ChangeValue(str);
	}
}


void CDlgRun::SetYPS()
{
	if (ARBScoringType::ByTime == m_Run->GetScoring().GetType()
	|| ARBScoringType::BySpeed == m_Run->GetScoring().GetType())
	{
		wxString str;
		double yps;
		if (m_Run->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), yps))
		{
			str = StringUtil::stringWX(ARBDouble::ToString(yps, 3));
		}
		m_ctrlYPSOpeningPts->ChangeValue(str);
	}
}


void CDlgRun::SetObstacles()
{
	wxString str;
	double ops;
	int prec;
	if (m_Run->GetScoring().GetObstaclesPS(CAgilityBookOptions::GetTableInYPS(), CAgilityBookOptions::GetRunTimeInOPS(), ops, prec))
	{
		str = StringUtil::stringWX(ARBDouble::ToString(ops, prec));
	}
	m_ctrlObstaclesPS->ChangeValue(str);
}


void CDlgRun::SetTotalFaults()
{
	if (ARBScoringType::ByTime == m_Run->GetScoring().GetType())
	{
		wxString total;
		ARBConfigScoringPtr pScoring;
		GetScoring(&pScoring);
		double faults = m_Run->GetScoring().GetCourseFaults() + m_Run->GetScoring().GetTimeFaults(pScoring);
		total = StringUtil::stringWX(ARBDouble::ToString(faults, 3));
		m_ctrlClosingPtsTotalFaults->ChangeValue(total);
	}
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

	std::wstring strSpeed(L"0");
	std::wstring strTitle(L"0");
	std::wstring strScore(L"");
	ARBConfigScoringPtr pScoring;
	if (GetScoring(&pScoring))
	{
		// 8/17/03: Only compute title points on Q runs.
		if (q.Qualified())
		{
			if (pScoring->HasSpeedPts())
			{
				strSpeed = fmt::format(L"{}", m_Run->GetSpeedPoints(pScoring));
			}
			strTitle = fmt::format(L"{}", m_Run->GetTitlePoints(pScoring));
		}
		if (q.ShouldComputeScore())
			strScore = ARBDouble::ToString(m_Run->GetScore(pScoring));
	}
	// Doesn't matter if they're hidden,..
	m_ctrlSpeedPts->ChangeValue(strSpeed);
	m_ctrlTitlePoints->ChangeValue(strTitle);
	m_ctrlScore->ChangeValue(strScore);
}


void CDlgRun::SetReadOnlyFlag(CTextCtrl* ctrl, bool bReadOnly)
{
	if (!ctrl)
		return;
	if (bReadOnly)
	{
		ctrl->SetEditable(false);
		ctrl->SetBackgroundColour(m_clrBack);
	}
	else
	{
		ctrl->SetEditable(true);
		ctrl->SetBackgroundColour(wxNullColour);
	}
}


void CDlgRun::FixScoreLayout()
{
	// If not set, we're in the constructor still and that will do the layout.
	if (!GetSizer())
		return;
	m_panelScore->Layout();
	m_panelScore->GetSizer()->Fit(m_panelScore);
	Layout();
	GetSizer()->Fit(this);
}


void CDlgRun::UpdateControls(bool bOnEventChange)
{
	m_textHeight->Show(false);
	m_ctrlHeight->Show(false);
	m_textJudge->Show(false);
	m_ctrlJudge->Show(false);
	m_ctrlJudgeNote->Show(false);
	m_ctrlHandler->Enable(false);
	m_ctrlConditions->Enable(false);
	m_ctrlPartnerEdit->Show(false);
	m_ctrlPartner->Show(false);
	m_ctrlTable->Show(false);
	m_ctrlSCTText->Show(false);
	m_ctrlSCT->Show(false);
	m_textYardsReqOpeningPts->Show(false);
	m_ctrlYardsReqOpeningPts->Show(false);
	m_textMinYPSClosingTime->Show(false);
	m_ctrlMinYPSClosingTime->Show(false);
	m_ctrlClosingText->Show(false);
	m_ctrlClosing->Show(false);
	m_ctrlObstaclesText->Show(false);
	m_ctrlObstacles->Show(false);
	m_ctrlTimeText->Show(false);
	m_ctrlTime->Show(false);
	m_ctrlFaultsText->Show(false);
	m_ctrlFaults->Show(false);
	m_textYPSOpeningPts->Show(false);
	m_ctrlYPSOpeningPts->Show(false);
	m_textClosingPtsTotalFaults->Show(false);
	m_ctrlClosingPtsTotalFaults->Show(false);
	m_ctrlObstaclesPS->Enable(false);
	m_textPlace->Show(false);
	m_ctrlPlace->Show(false);
	m_textPlaceOf->Show(false);
	m_ctrlInClass->Show(false);
	m_textDogsQd->Show(false);
	m_ctrlDogsQd->Show(false);
	m_ctrlQ->Enable(false);
	m_ctrlBonusPtsText->Show(false);
	m_ctrlBonusTitlePts->Show(false);
	m_ctrlSpeedPtsText->Show(false);
	m_ctrlSpeedPts->Show(false);
	m_ctrlTitlePointsText->Show(false);
	m_ctrlTitlePoints->Show(false);

	ARBConfigScoringPtr pScoring;
	if (!GetScoring(&pScoring))
	{
		m_Run->GetScoring().SetType(ARBScoringType::Unknown, false);
		FixScoreLayout();
		return;
	}

	ARBConfigEventPtr pEvent;
	if (!GetEvent(&pEvent))
	{
		FixScoreLayout();
		return;
	}

	m_ctrlHandler->Enable(true);
	m_ctrlConditions->Enable(true);

	if (pEvent->HasPartner())
	{
		m_ctrlPartnerEdit->Show(true);
		m_ctrlPartner->Show(true);
	}

	// 7/1/07: Resetting the table status must always be done - not just when
	// the new event has a table. (which is what was done before)
	if (bOnEventChange)
	{
		if (m_Run->GetScoring().HasTable() != pScoring->HasTable())
		{
			m_Run->GetScoring().SetHasTable(pScoring->HasTable());
			// Plus, we need to recompute the YPS.
			SetMinYPS();
			SetYPS();
			SetObstacles();
		}
	}
	if (pScoring->HasTable())
	{
		m_ctrlTable->Show(true);
		m_Table = m_Run->GetScoring().HasTable();
		m_ctrlTable->SetValue(m_Table);
	}

	wxString str;
	switch (pScoring->GetScoringStyle())
	{
	case ARBScoringStyle::Unknown:
		assert(0);
		break;
	case ARBScoringStyle::FaultsThenTime:
	case ARBScoringStyle::Faults100ThenTime:
	case ARBScoringStyle::Faults200ThenTime:
	case ARBScoringStyle::TimePlusFaults:
		m_Run->GetScoring().SetType(ARBScoringType::ByTime, pScoring->DropFractions());
		m_ctrlSCTText->Show(true);
		m_ctrlSCT->Show(true);
		m_textYardsReqOpeningPts->SetLabel(_("IDC_RUNSCORE_YARDS"));
		m_textYardsReqOpeningPts->Show(true);
		m_ctrlYardsReqOpeningPts->SetHelpText(_("HIDC_RUNSCORE_YARDS"));
		m_ctrlYardsReqOpeningPts->SetToolTip(_("HIDC_RUNSCORE_YARDS"));
		m_ctrlYardsReqOpeningPts->ChangeValue(StringUtil::stringWX(ARBDouble::ToString(m_Yards)));
		m_ctrlYardsReqOpeningPts->Show(true);
		m_textMinYPSClosingTime->SetLabel(_("IDC_RUNSCORE_MIN_YPS"));
		m_textMinYPSClosingTime->Show(true);
		m_ctrlMinYPSClosingTime->Show(true);
		m_ctrlMinYPSClosingTime->SetHelpText(wxEmptyString);
		m_ctrlMinYPSClosingTime->SetToolTip(wxEmptyString);
		SetReadOnlyFlag(m_ctrlMinYPSClosingTime, true);
		m_ctrlTimeText->Show(true);
		m_ctrlTime->Show(true);
		m_ctrlFaultsText->Show(true);
		m_ctrlFaults->Show(true);
		m_textYPSOpeningPts->SetLabel(_("IDC_RUNSCORE_YPS"));
		m_textYPSOpeningPts->Show(true);
		m_ctrlYPSOpeningPts->SetHelpText(wxEmptyString);
		m_ctrlYPSOpeningPts->SetToolTip(wxEmptyString);
		m_ctrlYPSOpeningPts->Show(true);
		SetReadOnlyFlag(m_ctrlYPSOpeningPts, true);
		m_textClosingPtsTotalFaults->SetLabel(_("IDC_RUNSCORE_TOTAL_FAULTS"));
		m_textClosingPtsTotalFaults->Show(true);
		m_ctrlClosingPtsTotalFaults->SetHelpText(wxEmptyString);
		m_ctrlClosingPtsTotalFaults->SetToolTip(wxEmptyString);
		m_ctrlClosingPtsTotalFaults->Show(true);
		SetReadOnlyFlag(m_ctrlClosingPtsTotalFaults, true);
		break;
	case ARBScoringStyle::OCScoreThenTime:
		m_Run->GetScoring().SetType(ARBScoringType::ByOpenClose, pScoring->DropFractions());
		if (bOnEventChange)
		{
			m_Opening = pScoring->GetRequiredOpeningPoints();
			if (0 != m_Opening)
				m_Run->GetScoring().SetNeedOpenPts(m_Opening);
			m_Closing = pScoring->GetRequiredClosingPoints();
			if (0 != m_Closing)
				m_Run->GetScoring().SetNeedClosePts(m_Closing);
		}
		// Do not push these (above) back into the run.
		// Otherwise this will overwrite valid values during OnInit.
		m_ctrlSCTText->Show(true);
		m_ctrlSCT->Show(true);
		m_textYardsReqOpeningPts->SetLabel(_("IDC_RUNSCORE_OPENING_PTS"));
		m_textYardsReqOpeningPts->Show(true);
		m_ctrlYardsReqOpeningPts->SetHelpText(_("HIDC_RUNSCORE_OPENING_PTS"));
		m_ctrlYardsReqOpeningPts->SetToolTip(_("HIDC_RUNSCORE_OPENING_PTS"));
		str.Printf(L"%hd", m_Opening);
		m_ctrlYardsReqOpeningPts->ChangeValue(str);
		m_ctrlYardsReqOpeningPts->Show(true);
		m_textMinYPSClosingTime->SetLabel(_("IDC_RUNSCORE_SCT2"));
		m_textMinYPSClosingTime->Show(true);
		m_ctrlMinYPSClosingTime->Show(true);
		m_ctrlMinYPSClosingTime->SetHelpText(_("HIDC_RUNSCORE_SCT2"));
		m_ctrlMinYPSClosingTime->SetToolTip(_("HIDC_RUNSCORE_SCT2"));
		m_ctrlMinYPSClosingTime->ChangeValue(StringUtil::stringWX(ARBDouble::ToString(m_SCT2)));
		SetReadOnlyFlag(m_ctrlMinYPSClosingTime, false);
		m_ctrlClosingText->Show(true);
		str.Printf(L"%hd", m_Closing);
		m_ctrlClosing->ChangeValue(str);
		m_ctrlClosing->Show(true);
		m_ctrlTimeText->Show(true);
		m_ctrlTime->Show(true);
		m_ctrlFaultsText->Show(true);
		m_ctrlFaults->Show(true);
		m_textYPSOpeningPts->SetLabel(_("IDC_RUNSCORE_OPEN_PTS"));
		m_textYPSOpeningPts->Show(true);
		m_ctrlYPSOpeningPts->SetHelpText(_("HIDC_RUNSCORE_OPEN_PTS"));
		m_ctrlYPSOpeningPts->SetToolTip(_("HIDC_RUNSCORE_OPEN_PTS"));
		str.Printf(L"%hd", m_Open);
		m_ctrlYPSOpeningPts->ChangeValue(str);
		m_ctrlYPSOpeningPts->Show(true);
		SetReadOnlyFlag(m_ctrlYPSOpeningPts, false);
		m_textClosingPtsTotalFaults->SetLabel(_("IDC_RUNSCORE_CLOSE_PTS"));
		m_textClosingPtsTotalFaults->Show(true);
		m_ctrlClosingPtsTotalFaults->SetHelpText(_("HIDC_RUNSCORE_CLOSE_PTS"));
		m_ctrlClosingPtsTotalFaults->SetToolTip(_("HIDC_RUNSCORE_CLOSE_PTS"));
		str.Printf(L"%hd", m_Close);
		m_ctrlClosingPtsTotalFaults->ChangeValue(str);
		m_ctrlClosingPtsTotalFaults->Show(true);
		SetReadOnlyFlag(m_ctrlClosingPtsTotalFaults, false);
		break;
	case ARBScoringStyle::ScoreThenTime:
		m_Run->GetScoring().SetType(ARBScoringType::ByPoints, pScoring->DropFractions());
		if (bOnEventChange)
		{
			m_Opening = pScoring->GetRequiredOpeningPoints();
			if (0 != m_Opening)
				m_Run->GetScoring().SetNeedOpenPts(m_Opening);
		}
		// Do not push this back into the run.
		// Otherwise this will overwrite valid values during OnInit.
		m_ctrlSCTText->Show(true);
		m_ctrlSCT->Show(true);
		m_textYardsReqOpeningPts->SetLabel(_("IDS_SCORING_REQUIRED_POINTS"));
		m_textYardsReqOpeningPts->Show(true);
		m_ctrlYardsReqOpeningPts->SetHelpText(_("HIDC_RUNSCORE_OPENING_PTS"));
		m_ctrlYardsReqOpeningPts->SetToolTip(_("HIDC_RUNSCORE_OPENING_PTS"));
		str.Printf(L"%hd", m_Opening);
		m_ctrlYardsReqOpeningPts->ChangeValue(str);
		m_ctrlYardsReqOpeningPts->Show(true);
		m_ctrlTimeText->Show(true);
		m_ctrlTime->Show(true);
		m_ctrlFaultsText->Show(true);
		m_ctrlFaults->Show(true);
		m_textYPSOpeningPts->SetLabel(_("IDS_POINTS"));
		m_textYPSOpeningPts->Show(true);
		m_ctrlYPSOpeningPts->SetHelpText(_("HIDC_RUNSCORE_OPEN_PTS"));
		m_ctrlYPSOpeningPts->SetToolTip(_("HIDC_RUNSCORE_OPEN_PTS"));
		str.Printf(L"%hd", m_Open);
		m_ctrlYPSOpeningPts->ChangeValue(str);
		m_ctrlYPSOpeningPts->Show(true);
		SetReadOnlyFlag(m_ctrlYPSOpeningPts, false);
		break;
	case ARBScoringStyle::TimeNoPlaces:
		m_Run->GetScoring().SetType(ARBScoringType::BySpeed, pScoring->DropFractions());
		m_ctrlSCTText->Show(false);
		m_ctrlSCT->Show(false);
		m_textYardsReqOpeningPts->SetLabel(_("IDC_RUNSCORE_YARDS"));
		m_textYardsReqOpeningPts->Show(true);
		m_ctrlYardsReqOpeningPts->SetHelpText(_("HIDC_RUNSCORE_YARDS"));
		m_ctrlYardsReqOpeningPts->SetToolTip(_("HIDC_RUNSCORE_YARDS"));
		m_ctrlYardsReqOpeningPts->ChangeValue(StringUtil::stringWX(ARBDouble::ToString(m_Yards)));
		m_ctrlYardsReqOpeningPts->Show(true);
		m_textMinYPSClosingTime->Show(false);
		m_ctrlMinYPSClosingTime->Show(false);
		m_ctrlTimeText->Show(true);
		m_ctrlTime->Show(true);
		m_ctrlFaultsText->Show(false);
		m_ctrlFaults->Show(false);
		m_textYPSOpeningPts->SetLabel(_("IDC_RUNSCORE_YPS"));
		m_textYPSOpeningPts->Show(true);
		m_ctrlYPSOpeningPts->SetHelpText(wxEmptyString);
		m_ctrlYPSOpeningPts->SetToolTip(wxEmptyString);
		m_ctrlYPSOpeningPts->Show(true);
		SetReadOnlyFlag(m_ctrlYPSOpeningPts, true);
		m_textClosingPtsTotalFaults->Show(false);
		m_ctrlClosingPtsTotalFaults->Show(false);
		break;
	}

	if (ARBScoringStyle::TimeNoPlaces != pScoring->GetScoringStyle())
	{
		m_textHeight->Show(true);
		m_ctrlHeight->Show(true);
		m_textJudge->Show(true);
		m_ctrlJudge->Show(true);
		m_ctrlJudgeNote->Show(true);
		m_ctrlObstaclesText->Show(true);
		m_ctrlObstacles->Show(true);
		m_textPlace->Show(true);
		m_ctrlPlace->Show(true);
		m_textPlaceOf->Show(true);
		m_ctrlInClass->Show(true);
		m_textDogsQd->Show(true);
		m_ctrlDogsQd->Show(true);
		m_ctrlDogsQd->Enable(0 < pScoring->GetTitlePoints().size() || 0 < pScoring->GetLifetimePoints().size());
		m_ctrlObstaclesPSText->SetLabel(_("IDC_RUNSCORE_OBSTACLES_PER_SEC"));
		m_ctrlObstaclesPSText->Show(true);
		m_ctrlObstaclesPS->Enable(true);
		m_ctrlObstaclesPS->SetHelpText(_("HIDC_RUNSCORE_OBSTACLES_PER_SEC"));
		m_ctrlObstaclesPS->SetToolTip(_("HIDC_RUNSCORE_OBSTACLES_PER_SEC"));
	}
	else
	{
		m_ctrlObstaclesPSText->SetLabel(_("IDC_RUNSCORE_MILESPERHOUR"));
		m_ctrlObstaclesPSText->Show(true);
		m_ctrlObstaclesPS->Enable(true);
		m_ctrlObstaclesPS->SetHelpText(_("HIDC_RUNSCORE_MILESPERHOUR"));
		m_ctrlObstaclesPS->SetToolTip(_("HIDC_RUNSCORE_MILESPERHOUR"));
	}
	m_ctrlQ->Enable(true);
	m_ctrlQ->ResetContent(pScoring);
	if (0 < pScoring->GetTitlePoints().size())
	{
		m_ctrlTitlePointsText->Show(true);
		m_ctrlTitlePoints->Show(true);
	}
	if (pScoring->HasBonusTitlePts())
	{
		m_ctrlBonusPtsText->Show(true);
		m_ctrlBonusTitlePts->Show(true);
	}
	if (pScoring->HasSpeedPts())
	{
		m_ctrlSpeedPtsText->Show(true);
		m_ctrlSpeedPts->Show(true);
	}
	SetTitlePoints();
	SetObstacles();
	if (ARBScoringType::ByTime == m_Run->GetScoring().GetType()
	|| ARBScoringType::BySpeed == m_Run->GetScoring().GetType())
	{
		SetMinYPS();
		SetYPS();
		SetTotalFaults();
	}
	FixScoreLayout();
}


void CDlgRun::SetFaultsText()
{
	m_ctrlFaultsList->Clear();
	for (ARBDogFaultList::const_iterator iter = m_Run->GetFaults().begin(); iter != m_Run->GetFaults().end(); ++iter)
	{
		m_ctrlFaultsList->Append(StringUtil::stringWX(*iter));
	}
}


CDlgDogRefRunDataPtr CDlgRun::GetReferenceData(long index) const
{
	return std::dynamic_pointer_cast<CDlgDogRefRunData, CListData>(m_ctrlRefRuns->GetData(index));
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
    std::wstring venue;
    if (m_Run->GetClub())
        venue = m_Run->GetClub()->GetVenue();
    else
    {
        int index = m_ctrlVenues->GetSelection();
        if (wxNOT_FOUND != index)
            venue = GetVenueData(index)->m_Venue->GetName();
    }

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
		venue,
		m_Run->GetEvent(),
		m_Run->GetDivision(),
		m_Run->GetLevel(),
		m_Run->GetDate(),
		nullptr,
		&pScoring))
	{
		m_pRefRunMe->SetScore(ARBDouble::ToString(m_Run->GetScore(pScoring)));
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
	for (int i = 0; i < scNumRefRunColumns; ++i)
	{
		std::wstring str = fmt::format(L"{} ({})",
			wxGetTranslation(scRefRunColumns[i].idText).wx_str(),
			m_sortRefRuns.FindColumnOrder(i) + 1);
		wxListItem item;
		item.SetMask(wxLIST_MASK_TEXT);
		item.SetText(str);
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
		CDlgDogRefRunDataPtr data(std::make_shared<CDlgDogRefRunData>(m_Run, *iterRef));
		m_ctrlRefRuns->InsertItem(data);
	}
	for (index = 0; index < scNumRefRunColumns; ++index)
		m_ctrlRefRuns->SetColumnWidth(index, wxLIST_AUTOSIZE_USEHEADER);
	RunSortInfo sortInfo(this, &m_sortRefRuns);
	m_ctrlRefRuns->SortItems(CompareRefRuns, &sortInfo);

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


void CDlgRun::GetAllHeights(std::set<std::wstring>& outNames)
{
	m_pDoc->Book().GetAllHeights(outNames);
	for (long index = 0; index < m_ctrlRefRuns->GetItemCount(); ++index)
	{
		ARBDogReferenceRunPtr pRefRun = GetReferenceData(index)->GetData();
		if (pRefRun)
		{
			std::wstring const& ht = pRefRun->GetHeight();
			if (0 < ht.length())
				outNames.insert(ht);
		}
	}
}


void CDlgRun::GetAllCallNames(std::set<std::wstring>& outNames)
{
	m_pDoc->Book().GetAllCallNames(outNames);
	for (int index = 0; index < m_ctrlRefRuns->GetItemCount(); ++index)
	{
		ARBDogReferenceRunPtr pRefRun = GetReferenceData(index)->GetData();
		if (pRefRun)
		{
			std::wstring const& ht = pRefRun->GetName();
			if (0 < ht.length())
				outNames.insert(ht);
		}
	}
}


void CDlgRun::GetAllBreeds(std::set<std::wstring>& outNames)
{
	m_pDoc->Book().GetAllBreeds(outNames);
	for (int index = 0; index < m_ctrlRefRuns->GetItemCount(); ++index)
	{
		ARBDogReferenceRunPtr pRefRun = GetReferenceData(index)->GetData();
		if (pRefRun)
		{
			std::wstring const& ht = pRefRun->GetBreed();
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
		std::set<std::wstring> heights, names, breeds;
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


void CDlgRun::ListLinkFiles(wchar_t const* pItem)
{
	wxBusyCursor wait;
	m_ctrlLinks->DeleteAllItems();
	std::set<std::wstring> links;
	m_Run->GetLinks(links);
	long i = 0;
	for (std::set<std::wstring>::iterator iter = links.begin();
		iter != links.end();
		++iter)
	{
		int idx = m_ctrlLinks->InsertItem(i++, StringUtil::stringWX(*iter), GetImageIndex(*iter));
		if (pItem && *iter == pItem)
			m_ctrlLinks->Select(idx);
	}
	m_ctrlLinks->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
	UpdateLinksButtons();
}


int CDlgRun::GetImageIndex(std::wstring const& inLink)
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
		std::wstring name = GetListColumnText(m_ctrlLinks, nItem, 0);
		CDlgSelectURL dlg(name, this);
		if (wxID_OK == dlg.ShowModal())
		{
			std::wstring newName = dlg.Name();
			if (name != newName)
			{
				m_Run->RemoveLink(name);
				if (0 < newName.length())
					m_Run->AddLink(newName);
				ListLinkFiles(newName.c_str());
			}
		}
	}
}


void CDlgRun::OnScoreDateChanged(wxDateEvent& evt)
{
	m_Run->SetDate(ARBDate(evt.GetDate().GetYear(), evt.GetDate().GetMonth() + 1, evt.GetDate().GetDay()));
	FillDivisions(true);
}


void CDlgRun::OnSelchangeVenue(wxCommandEvent& evt)
{
	bool bChanged = true;
	int index = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND != index)
		m_Run->SetClub(GetVenueData(index)->m_Club);
	// We could check if the venue changed. But I'm finding it convenient when
	// co-sanctioning changes and changes don't necessarily update the club,
	// re-selecting the venue "fixes" it.
	if (bChanged)
		UpdateVenue(true);
	else
		FillDivisions(true);
}


void CDlgRun::OnSelchangeDivision(wxCommandEvent& evt)
{
	int index = m_ctrlDivisions->GetSelection();
	if (wxNOT_FOUND != index)
		m_Run->SetDivision(StringUtil::stringW(m_ctrlDivisions->GetString(index)));
	FillLevels(true);
}


void CDlgRun::OnSelchangeLevel(wxCommandEvent& evt)
{
	int index = m_ctrlLevels->GetSelection();
	if (wxNOT_FOUND != index)
		m_Run->SetLevel(StringUtil::stringW(m_ctrlLevels->GetString(index)));
	FillEvents(true);
}


void CDlgRun::OnSelchangeEvent(wxCommandEvent& evt)
{
	int index = m_ctrlEvents->GetSelection();
	if (wxNOT_FOUND != index)
		m_Run->SetEvent(StringUtil::stringW(m_ctrlEvents->GetString(index)));
	FillSubNames();
	UpdateControls(true);
	ARBConfigEventPtr pEvent;
	GetEvent(&pEvent);
	SetEventDesc(pEvent);
}


void CDlgRun::OnJudgeNotes(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	CDlgInfoNote dlg(m_pDoc, ARBInfoType::Judge, StringUtil::stringW(m_Judge), this);
	if (wxID_OK == dlg.ShowModal())
	{
		m_Judge = StringUtil::stringWX(dlg.CurrentSelection());
		FillJudges();
	}
}


void CDlgRun::OnPartnersEdit(wxCommandEvent& evt)
{
	CDlgListCtrl dlg(ARBWhatToList::Partners, m_pDoc, m_Run, this);
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
		bSetTable = m_ctrlTable->GetValue();
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


void CDlgRun::OnSCTChange(wxCommandEvent& evt)
{
	// See DlgCalendar.cpp on why TransferDataFromWindow is bad here!
	StringUtil::ToDouble(StringUtil::stringW(m_ctrlSCT->GetValue()), m_SCT);
	m_Run->GetScoring().SetSCT(m_SCT);
	SetMinYPS();
	SetObstacles();
	SetTotalFaults();
	SetTitlePoints();
}


void CDlgRun::OnClosingTimeChange(wxCommandEvent& evt)
{
	StringUtil::ToDouble(StringUtil::stringW(m_ctrlMinYPSClosingTime->GetValue()), m_SCT2);
	m_Run->GetScoring().SetSCT2(m_SCT2);
	SetObstacles();
}


void CDlgRun::OnNumObsChange(wxCommandEvent& evt)
{
	m_Obstacles = static_cast<short>(wxAtol(m_ctrlObstacles->GetValue()));
	m_Run->GetScoring().SetObstacles(m_Obstacles);
	SetObstacles();
}


void CDlgRun::OnReqOpeningYPSChange(wxCommandEvent& evt)
{
	ARBConfigScoringPtr pScoring;
	if (GetScoring(&pScoring))
	{
		short val = static_cast<short>(wxAtol(m_ctrlYardsReqOpeningPts->GetValue()));
		switch (pScoring->GetScoringStyle())
		{
		case ARBScoringStyle::Unknown:
		case ARBScoringStyle::FaultsThenTime:
		case ARBScoringStyle::Faults100ThenTime:
		case ARBScoringStyle::Faults200ThenTime:
		case ARBScoringStyle::TimePlusFaults:
			m_Yards = val;
			m_Run->GetScoring().SetYards(m_Yards);
			SetMinYPS();
			SetYPS();
			SetTotalFaults();
			break;
		case ARBScoringStyle::ScoreThenTime:
		case ARBScoringStyle::OCScoreThenTime:
			m_Opening = val;
			m_Run->GetScoring().SetNeedOpenPts(m_Opening);
			SetTitlePoints();
			break;
		case ARBScoringStyle::TimeNoPlaces:
			m_Yards = val;
			m_Run->GetScoring().SetYards(m_Yards);
			SetYPS();
			break;
		}
	}
}


void CDlgRun::OnReqClosingChange(wxCommandEvent& evt)
{
	m_Closing = static_cast<short>(wxAtol(m_ctrlClosing->GetValue()));
	m_Run->GetScoring().SetNeedClosePts(m_Closing);
	SetTitlePoints();
}


void CDlgRun::OnTimeChange(wxCommandEvent& evt)
{
	StringUtil::ToDouble(StringUtil::stringW(m_ctrlTime->GetValue()), m_Time);
	m_Run->GetScoring().SetTime(m_Time);
	SetYPS();
	SetObstacles();
	SetTotalFaults();
	SetTitlePoints();
}


void CDlgRun::OnFaultsChange(wxCommandEvent& evt)
{
	m_Faults = static_cast<short>(wxAtol(m_ctrlFaults->GetValue()));
	m_Run->GetScoring().SetCourseFaults(m_Faults);
	SetTotalFaults();
	SetTitlePoints();
}


void CDlgRun::OnOpenChange(wxCommandEvent& evt)
{
	m_Open = static_cast<short>(wxAtol(m_ctrlYPSOpeningPts->GetValue()));
	m_Run->GetScoring().SetOpenPts(m_Open);
	SetTitlePoints();
}


void CDlgRun::OnCloseChange(wxCommandEvent& evt)
{
	m_Close = static_cast<short>(wxAtol(m_ctrlClosingPtsTotalFaults->GetValue()));
	m_Run->GetScoring().SetClosePts(m_Close);
	SetTitlePoints();
}


void CDlgRun::OnPlaceChange(wxCommandEvent& evt)
{
	m_Place = static_cast<short>(wxAtol(m_ctrlPlace->GetValue()));
	m_Run->SetPlace(m_Place);
	SetTitlePoints();
}


void CDlgRun::OnInClassChange(wxCommandEvent& evt)
{
	m_InClass = static_cast<short>(wxAtol(m_ctrlInClass->GetValue()));
	m_Run->SetInClass(m_InClass);
	SetTitlePoints();
}


void CDlgRun::OnBonusChange(wxCommandEvent& evt)
{
	StringUtil::ToDouble(StringUtil::stringW(m_ctrlBonusTitlePts->GetValue()), m_BonusTitlePts);
	m_Run->GetScoring().SetBonusTitlePts(m_BonusTitlePts);
	SetTitlePoints();
}


void CDlgRun::OnCommentsFaults(wxCommandEvent& evt)
{
	CDlgListCtrl dlg(ARBWhatToList::Faults, m_pDoc, m_Run, this);
	if (wxID_OK == dlg.ShowModal())
		SetFaultsText();
}


void CDlgRun::OnRefRunColumnClick(wxListEvent& evt)
{
	m_sortRefRuns.SetColumnOrder(evt.GetColumn());
	SetRefRunColumnHeaders();
	RunSortInfo sortInfo(this, &m_sortRefRuns);
	m_ctrlRefRuns->SortItems(CompareRefRuns, &sortInfo);
	m_sortRefRuns.Save();
}


void CDlgRun::OnRefRunItemSelected(wxListEvent& evt)
{
	UpdateRefRunButtons();
}


void CDlgRun::OnRefRunItemActivated(wxListEvent& evt)
{
	EditRefRun();
}


void CDlgRun::OnRefRunKeyDown(wxKeyEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		evt.Skip();
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
		EditRefRun();
		break;
	}
}


void CDlgRun::OnRefRunNew(wxCommandEvent& evt)
{
	ARBDogReferenceRunPtr ref(ARBDogReferenceRun::New());
	if (ARBScoringType::ByTime == m_Run->GetScoring().GetType())
	{
        std::wstring venue;
        if (m_Run->GetClub())
            venue = m_Run->GetClub()->GetVenue();
        else
        {
            int index = m_ctrlVenues->GetSelection();
            if (wxNOT_FOUND != index)
                venue = GetVenueData(index)->m_Venue->GetName();
        }

		ARBConfigScoringPtr pScoring;
		if (m_pDoc->Book().GetConfig().GetVenues().FindEvent(
			venue,
			m_Run->GetEvent(),
			m_Run->GetDivision(),
			m_Run->GetLevel(),
			m_Run->GetDate(),
			nullptr,
			&pScoring))
		{
			std::wstring nScore;
			switch (pScoring->GetScoringStyle())
			{
			case ARBScoringStyle::Unknown:
			case ARBScoringStyle::FaultsThenTime:
			case ARBScoringStyle::OCScoreThenTime:
			case ARBScoringStyle::ScoreThenTime:
			case ARBScoringStyle::TimePlusFaults:
			case ARBScoringStyle::TimeNoPlaces:
				nScore = L"0";
				break;
			case ARBScoringStyle::Faults100ThenTime:
				nScore = L"100";
				break;
			case ARBScoringStyle::Faults200ThenTime:
				nScore = L"200";
				break;
			}
			ref->SetScore(nScore);
		}
	}
	ref->SetQ(Q::Q);
	std::set<std::wstring> heights, names, breeds;
	GetAllHeights(heights);
	GetAllCallNames(names);
	GetAllBreeds(breeds);
	CDlgReferenceRun dlg(m_pDoc, m_Run, heights, names, breeds, ref, this);
	if (wxID_OK == dlg.ShowModal())
	{
		if (m_Run->GetReferenceRuns().AddReferenceRun(ref))
		{
			CDlgDogRefRunDataPtr data(std::make_shared<CDlgDogRefRunData>(m_Run, ref));
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
			CDlgDogRefRunDataPtr data(std::make_shared<CDlgDogRefRunData>(m_Run, m_pRefRunMe));
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


void CDlgRun::OnLinksItemActivated(wxListEvent& evt)
{
	EditLink();
}


void CDlgRun::OnLinksKeyDown(wxKeyEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		evt.Skip();
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
		EditLink();
		break;
	}
}


void CDlgRun::OnLinksNew(wxCommandEvent& evt)
{
	CDlgSelectURL dlg(L"", this);
	if (wxID_OK == dlg.ShowModal())
	{
		std::wstring newName = dlg.Name();
		if (0 < newName.length())
		{
			m_Run->AddLink(newName);
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
		std::wstring name = GetListColumnText(m_ctrlLinks, nItem, 0);
		m_Run->RemoveLink(name);
		ListLinkFiles(nullptr);
	}
}


void CDlgRun::OnLinksOpen(wxCommandEvent& evt)
{
	int nItem = m_ctrlLinks->GetFirstSelected();
	if (0 <= nItem)
	{
		std::wstring name = GetListColumnText(m_ctrlLinks, nItem, 0);
		wxLaunchDefaultBrowser(StringUtil::stringWX(name));
	}
}


void CDlgRun::OnPageChanging(wxBookCtrlEvent& evt)
{
	if (wxNOT_FOUND != evt.GetOldSelection())
	{
		if (!Validate() || !TransferDataFromWindow())
		{
			evt.Veto();
			return;
		}
	}
	evt.Skip();
}


void CDlgRun::OnPageChanged(wxBookCtrlEvent& evt)
{
	if (wxNOT_FOUND != evt.GetOldSelection())
		TransferDataFromWindow();
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

	int index = m_ctrlVenues->GetSelection();
	if (wxNOT_FOUND == index)
    {
		wxMessageBox(_("IDS_SELECT_VENUE"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
		m_ctrlVenues->SetFocus();
		return;
    }
	ARBConfigVenuePtr pVenue = GetVenueData(index)->m_Venue;

	index = m_ctrlDivisions->GetSelection();
	if (wxNOT_FOUND == index)
	{
		wxMessageBox(_("IDS_SELECT_DIVISION"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
		m_ctrlDivisions->SetFocus();
		return;
	}
	std::wstring curDiv = StringUtil::stringW(m_ctrlDivisions->GetString(index));

	index = m_ctrlLevels->GetSelection();
	if (wxNOT_FOUND == index)
	{
		wxMessageBox(_("IDS_SELECT_LEVEL"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
		m_ctrlLevels->SetFocus();
		return;
	}
	CDlgDogLevelData* pLevel = GetLevelData(index);
	assert(pLevel);

	index = m_ctrlEvents->GetSelection();
	if (wxNOT_FOUND == index)
	{
		wxMessageBox(_("IDS_SELECT_EVENT"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
		m_ctrlEvents->SetFocus();
		return;
	}
	std::wstring curEvent = StringUtil::stringW(m_ctrlEvents->GetString(index));

	ARBConfigEventPtr pEvent;
	pVenue->GetEvents().FindEvent(curEvent, &pEvent);
	if (!pEvent)
	{
		wxMessageBox(_("IDS_BAD_EVENT"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
		m_ctrlEvents->SetFocus();
		return;
	}

	if (!pEvent->FindEvent(curDiv, pLevel->m_pLevel->GetName(), m_Run->GetDate()))
	{
		wxMessageBox(_("IDS_BAD_SCORINGMETHOD"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
		m_ctrlLevels->SetFocus();
		return;
	}

	if (wxNOT_FOUND == m_ctrlQ->GetSelection())
	{
		wxMessageBox(_("IDS_SELECT_Q"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
		m_ctrlQ->SetFocus();
		return;
	}

	//@todo: Add integrity checks - things like snooker score >=37? is Q set?

	m_Run->SetDate(m_Date);
	if (m_ctrlSubNames->IsShown())
		m_Run->SetSubName(StringUtil::stringW(m_SubName));
	if (m_ctrlHeight->IsShown())
		m_Run->SetHeight(StringUtil::stringW(m_Height));
	if (m_ctrlJudge->IsShown())
		m_Run->SetJudge(StringUtil::stringW(m_Judge));
	if (m_ctrlHandler->IsShown())
		m_Run->SetHandler(StringUtil::stringW(m_Handler));
	m_Run->SetConditions(StringUtil::stringW(m_Conditions));
	m_Run->SetNote(StringUtil::stringW(m_Comments));
	if (m_ctrlInClass->IsShown())
		m_Run->SetInClass(m_InClass);
	if (m_ctrlDogsQd->IsShown())
		m_Run->SetDogsQd(m_DogsQd);

	//TODO: Remove debugging code
#ifdef _DEBUG
	{
		assert(!!pEvent.get());
		ARBConfigScoringPtr scoring;
		pEvent->FindEvent(curDiv, pLevel->m_pLevel->GetName(), m_Run->GetDate(), &scoring);
		assert(!!scoring.get());
		if (!scoring->HasTable())
			if (m_Run->GetScoring().HasTable())
				wxMessageBox(L"Poof!", wxMessageBoxCaptionStr, wxOK | wxCENTRE);
	}
#endif
	//End TODO

	*m_pRealRun = *m_Run;
	m_pTrial->SetMultiQs(m_pDoc->Book().GetConfig()); // Note, when adding a new run, this is actually too soon to call - the run isn't in the trial yet
	CAgilityBookOptions::SetLastEnteredDivision(m_pDog, pVenue, m_Run->GetDivision().c_str());
	CAgilityBookOptions::SetLastEnteredLevel(m_pDog, pVenue, m_Run->GetLevel().c_str());
	if (m_ctrlHeight->IsShown())
		CAgilityBookOptions::SetLastEnteredHeight(m_pDog, pVenue, m_Run->GetHeight().c_str());
	if (m_ctrlJudge->IsShown())
		CAgilityBookOptions::SetLastEnteredJudge(m_Run->GetJudge().c_str());
	if (m_ctrlHandler->IsShown())
		CAgilityBookOptions::SetLastEnteredHandler(m_pDog, m_Run->GetHandler().c_str());

	m_pDoc->Modify(true);

	EndDialog(wxID_OK);
}
