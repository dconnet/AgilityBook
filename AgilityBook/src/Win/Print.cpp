/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Support for direct printing of runs.
 * @author David Connet
 *
 * Revision History
 * @li 2009-12-19 DRC Fix blank page layout in landscape.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-08-19 DRC Fixed printing when page size isn't specified.
 * @li 2009-05-31 DRC Added support for creating pages of a specific size.
 * @li 2009-01-27 DRC Ported to wxWidgets.
 * @li 2009-01-21 DRC Leave gray separator line in when text in run is empty.
 * @li 2007-07-13 DRC Created
 */

#include "stdafx.h"
#include "Print.h"

#include "AgilityBook.h"
#include "AgilityBookOptions.h"
#include "Globals.h"
#include "ListCtrl.h"
#include "PointsData.h"

#include "ARB/ARBConfig.h"
#include "ARB/ARBDog.h"
#include "ARB/ARBDogRun.h"
#include "ARB/ARBDogRunOtherPoints.h"
#include "ARB/ARBDogTrial.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

CPrintPreview::CPrintPreview(
		wxPrintout* printout,
		wxPrintout* printoutForPrinting,
		wxPrintDialogData* data)
	: wxPrintPreview(printout, printoutForPrinting, data)
{
}


bool CPrintPreview::Print(bool interactive)
{
	bool rc = false;
	if (GetPrintoutForPrinting())
	{
		wxPrinter printer(&GetPrintDialogData());
		rc = printer.Print(GetFrame(), GetPrintoutForPrinting(), interactive);
		if (rc)
		{
			wxGetApp().SavePrintData(printer.GetPrintDialogData());
			GetFrame()->Close(true);
		}
	}
	return rc;
}

/////////////////////////////////////////////////////////////////////////////

class CPrintRuns : public wxPrintout
{
public:
	CPrintRuns(
			ARBConfig const* inConfig,
			ARBDogPtr inDog,
			std::vector<RunInfo> const& inRuns);

	// Sets SetUserScale and m_OneInch (returns oneinch)
	double ComputeScaling();

	virtual bool HasPage(int page);
	virtual void GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo);
	virtual bool OnPrintPage(int pageNum);

private:
	std::wstring GetFieldText(ARBDogTrialPtr trial, ARBDogRunPtr run, int code);
	void PrintPage(int nCurPage, size_t curRun, wxDC* pDC, wxRect inRect);

	double m_OneInch;
	ARBConfig const* m_config;
	ARBDogPtr m_dog;
	std::vector<RunInfo> m_runs; // can't be const& since we're previewing now and actual vector goes out-of-scope
	int m_maxPage;
	int m_nPerSheet;
	int m_nPagesX; // Number of pages we can fit across
	int m_nPagesY;
};


CPrintRuns::CPrintRuns(
		ARBConfig const* inConfig,
		ARBDogPtr inDog,
		std::vector<RunInfo> const& inRuns)
	: wxPrintout(_("IDS_RUNS"))
	, m_OneInch(0.0)
	, m_config(inConfig)
	, m_dog(inDog)
	, m_runs(inRuns)
	, m_maxPage(0)
	, m_nPerSheet(0)
	, m_nPagesX(1)
	, m_nPagesY(1)
{
}


double CPrintRuns::ComputeScaling()
{
	// Scaling code was copied from wxWidget print sample
	wxDC* dc = GetDC();

	int ppiScreenX, ppiScreenY;
	GetPPIScreen(&ppiScreenX, &ppiScreenY);
	int ppiPrinterX, ppiPrinterY;
	GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);

	double scale = (double)ppiPrinterX / (double)ppiScreenX;

	int w, h;
	dc->GetSize(&w, &h);
	int pageWidth, pageHeight;
	GetPageSizePixels(&pageWidth, &pageHeight);

	double overallScale = scale * ((double)w / (double)pageWidth);
	dc->SetUserScale(overallScale, overallScale);

	m_OneInch = ppiPrinterX / scale;

	return m_OneInch;
}


bool CPrintRuns::HasPage(int page)
{
	return 1 <= page && page <= m_maxPage;
}


void CPrintRuns::GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo)
{
	int nRuns = static_cast<int>(m_runs.size());
	int nPages = 1;
	m_nPagesX = 1;
	m_nPagesY = 1;
	int pageW, pageH;
	GetPageSizeMM(&pageW, &pageH);
	long width, height;
	CAgilityBookOptions::GetRunPageSize(true, width, height, NULL);
	// This logic should match OnPrintPage
	if (0 < width && 0 < height)
	{
		m_nPagesX = pageW / width;
		if (1 > m_nPagesX)
			m_nPagesX = 1;
		m_nPagesY = pageH / height;
		if (1 > m_nPagesY)
			m_nPagesY = 1;
	}
	else
	{
		if (abs(pageW) > abs(pageH)) // landscape
			m_nPagesX = 2;
	}
	m_nPerSheet = (m_nPagesX * m_nPagesY) * 2;

	*minPage = 1;
	if (0 == nRuns)
	{
		*maxPage = 2;
	}
	else
	{
		nPages = nRuns / m_nPerSheet;
		if (0 != nRuns % m_nPerSheet)
			++nPages;
		*maxPage = nPages;
	}
	*pageFrom = *minPage;
	*pageTo = *maxPage;
	m_maxPage = *maxPage;
}


#define CODE_DOG		0
#define CODE_DATE		1
#define CODE_VENUE		2
#define CODE_CLUB		3
#define CODE_DIV		4
#define CODE_LOCATION	5
#define CODE_HEIGHT		6
#define CODE_JUDGE		7
#define CODE_HANDLER	8
#define CODE_CONDITIONS	9
#define CODE_Q			10
#define CODE_SCT		11
#define CODE_YARDS		12
#define CODE_OPEN		13
#define CODE_TIME		14
#define CODE_OBSTACLES	15
#define CODE_FAULTS		16
#define CODE_SCORE		17
#define CODE_PLACE		18
#define CODE_INCLASS	19
#define CODE_QD			20
#define CODE_OTHER		21
#define CODE_COMMENTS	22
#define CODE_REFPLACE1	23
#define CODE_REFQ1		24
#define CODE_REFTIME1	25
#define CODE_REFSCORE1	26
#define CODE_REFHT1		27
#define CODE_REF1		28
#define CODE_REFPLACE2	29
#define CODE_REFQ2		30
#define CODE_REFTIME2	31
#define CODE_REFSCORE2	32
#define CODE_REFHT2		33
#define CODE_REF2		34
#define CODE_REFPLACE3	35
#define CODE_REFQ3		36
#define CODE_REFTIME3	37
#define CODE_REFSCORE3	38
#define CODE_REFHT3		39
#define CODE_REF3		40
#define CODE_REFPLACE4	41
#define CODE_REFQ4		42
#define CODE_REFTIME4	43
#define CODE_REFSCORE4	44
#define CODE_REFHT4		45
#define CODE_REF4		46

// Make sure these are ordered by the aboves codes
static const wchar_t* sc_codes[] =
{
	arbT("IDS_COL_DOG"),
	arbT("IDS_COL_DATE"),
	arbT("IDS_COL_VENUE"),
	arbT("IDS_COL_CLUB"),
	arbT("IDS_COL_DIV_LVL_EVT"),
	arbT("IDS_COL_LOCATION"),
	arbT("IDS_COL_HEIGHT"),
	arbT("IDS_COL_JUDGE"),
	arbT("IDS_COL_HANDLER"),
	arbT("IDS_COL_CONDITIONS"),
	arbT("IDS_COL_Q"),
	arbT("IDS_COL_SCT"),
	arbT("IDS_COL_YARDS"),
	arbT("IDS_COL_OPEN_CLOSE"),
	arbT("IDS_COL_TIME"),
	arbT("IDS_COL_OBSTACLES"),
	arbT("IDS_COL_FAULTS"),
	arbT("IDS_COL_SCORE"),
	arbT("IDS_COL_PLACE"),
	arbT("IDS_COL_IN_CLASS"),
	arbT("IDS_COL_Q_D"),
	arbT("IDS_COL_OTHERPOINTS"),
	arbT("IDS_COL_COMMENTS"),
	arbT("IDS_COL_PLACE"),
	arbT("IDS_COL_Q"),
	arbT("IDS_COL_TIME"),
	arbT("IDS_COL_SCORE"),
	arbT("IDS_COL_HEIGHT"),
	arbT("IDS_COL_NAME_BREED_NOTE"),
	arbT("IDS_COL_PLACE"),
	arbT("IDS_COL_Q"),
	arbT("IDS_COL_TIME"),
	arbT("IDS_COL_SCORE"),
	arbT("IDS_COL_HEIGHT"),
	arbT("IDS_COL_NAME_BREED_NOTE"),
	arbT("IDS_COL_PLACE"),
	arbT("IDS_COL_Q"),
	arbT("IDS_COL_TIME"),
	arbT("IDS_COL_SCORE"),
	arbT("IDS_COL_HEIGHT"),
	arbT("IDS_COL_NAME_BREED_NOTE"),
	arbT("IDS_COL_PLACE"),
	arbT("IDS_COL_Q"),
	arbT("IDS_COL_TIME"),
	arbT("IDS_COL_SCORE"),
	arbT("IDS_COL_HEIGHT"),
	arbT("IDS_COL_NAME_BREED_NOTE"),
};
#define FOR_TIME	0x1
#define FOR_PTS		0x2
#define FOR_BOTH	0x3
static const struct
{
	unsigned char type;
	int line;
	int box;
	int colspan; // number of boxes to span
	int rowspan;
	bool bContinuation;
	int code;
	bool bWorkBreak;
	unsigned int fmt;
} sc_lines[] =
{
	{FOR_BOTH,  0, 0, 2, 1, false, CODE_DOG,        false, wxALIGN_LEFT | wxALIGN_BOTTOM},
	{FOR_BOTH,  0, 2, 2, 1, false, CODE_DATE,       false, wxALIGN_LEFT | wxALIGN_BOTTOM},
	{FOR_BOTH,  0, 4, 2, 1, false, CODE_VENUE,      false, wxALIGN_LEFT | wxALIGN_BOTTOM},
	{FOR_BOTH,  0, 6, 2, 1, false, CODE_LOCATION,   false, wxALIGN_LEFT | wxALIGN_BOTTOM},
	{FOR_BOTH,  1, 0, 3, 1, false, CODE_DIV,        false, wxALIGN_LEFT | wxALIGN_BOTTOM},
	{FOR_BOTH,  1, 3, 1, 1, false, CODE_HEIGHT,     false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH,  1, 4, 4, 2, false, CODE_CLUB,       true,  wxALIGN_LEFT},
	{FOR_BOTH,  2, 4, 4, 1, true,  CODE_CLUB, false, 0},
	{FOR_BOTH,  2, 0, 2, 1, false, CODE_JUDGE,      false, wxALIGN_LEFT | wxALIGN_BOTTOM},
	{FOR_BOTH,  2, 2, 2, 1, false, CODE_HANDLER,    false, wxALIGN_LEFT | wxALIGN_BOTTOM},
	{FOR_BOTH,  3, 0, 7, 1, false, CODE_CONDITIONS, true,  wxALIGN_LEFT},
	{FOR_BOTH,  3, 7, 1, 1, false, CODE_Q,          false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH,  4, 0, 1, 1, false, CODE_SCT,        false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_TIME,  4, 1, 1, 1, false, CODE_YARDS,      false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_PTS,   4, 1, 1, 1, false, CODE_OPEN,       false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH,  4, 2, 1, 1, false, CODE_OBSTACLES,  false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH,  4, 3, 1, 1, false, CODE_TIME,       false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_TIME,  4, 4, 1, 1, false, CODE_FAULTS,     false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_PTS,   4, 4, 1, 1, false, CODE_SCORE,      false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH,  4, 5, 1, 1, false, CODE_PLACE,      false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH,  4, 6, 1, 1, false, CODE_INCLASS,    false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH,  4, 7, 1, 1, false, CODE_QD,         false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH,  5, 0, 8, 5, false, CODE_COMMENTS,   true,  wxALIGN_LEFT},
	{FOR_BOTH,  6, 0, 8, 4, true,  CODE_COMMENTS, false, 0},
	{FOR_BOTH,  7, 0, 8, 3, true,  CODE_COMMENTS, false, 0},
	{FOR_BOTH,  8, 0, 8, 2, true,  CODE_COMMENTS, false, 0},
	{FOR_BOTH,  9, 0, 6, 1, true,  CODE_COMMENTS, false, 0},
	{FOR_BOTH,  9, 6, 2, 1, false, CODE_OTHER,      true,  wxALIGN_LEFT},
	{FOR_BOTH, 10, 0, 1, 1, false, CODE_REFPLACE1,  false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH, 10, 1, 1, 1, false, CODE_REFQ1,      false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH, 10, 2, 1, 1, false, CODE_REFTIME1,   false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH, 10, 3, 1, 1, false, CODE_REFSCORE1,  false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH, 10, 4, 1, 1, false, CODE_REFHT1,     false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH, 10, 5, 3, 1, false, CODE_REF1,       true,  wxALIGN_LEFT},
	{FOR_BOTH, 11, 0, 1, 1, false, CODE_REFPLACE2,  false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH, 11, 1, 1, 1, false, CODE_REFQ2,      false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH, 11, 2, 1, 1, false, CODE_REFTIME2,   false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH, 11, 3, 1, 1, false, CODE_REFSCORE2,  false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH, 11, 4, 1, 1, false, CODE_REFHT2,     false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH, 11, 5, 3, 1, false, CODE_REF2,       true,  wxALIGN_LEFT},
	{FOR_BOTH, 12, 0, 1, 1, false, CODE_REFPLACE3,  false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH, 12, 1, 1, 1, false, CODE_REFQ3,      false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH, 12, 2, 1, 1, false, CODE_REFTIME3,   false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH, 12, 3, 1, 1, false, CODE_REFSCORE3,  false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH, 12, 4, 1, 1, false, CODE_REFHT3,     false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH, 12, 5, 3, 1, false, CODE_REF3,       true,  wxALIGN_LEFT},
	{FOR_BOTH, 13, 0, 1, 1, false, CODE_REFPLACE4,  false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH, 13, 1, 1, 1, false, CODE_REFQ4,      false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH, 13, 2, 1, 1, false, CODE_REFTIME4,   false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH, 13, 3, 1, 1, false, CODE_REFSCORE4,  false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH, 13, 4, 1, 1, false, CODE_REFHT4,     false, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM},
	{FOR_BOTH, 13, 5, 3, 1, false, CODE_REF4,       true,  wxALIGN_LEFT},
};
static const int sc_nLines = sizeof(sc_lines) / sizeof(sc_lines[0]);


static void RefRunHelper(std::wostringstream& text, ARBDogReferenceRunPtr ref, int code)
{
	switch (code)
	{
	case CODE_REFPLACE1:
	case CODE_REFPLACE2:
	case CODE_REFPLACE3:
	case CODE_REFPLACE4:
		if (0 < ref->GetPlace())
			text << ref->GetPlace();
		break;
	case CODE_REFQ1:
	case CODE_REFQ2:
	case CODE_REFQ3:
	case CODE_REFQ4:
		if ((ARB_Q::eQ)ref->GetQ() != ARB_Q::eQ_NA)
			text << ref->GetQ().str();
		break;
	case CODE_REFTIME1:
	case CODE_REFTIME2:
	case CODE_REFTIME3:
	case CODE_REFTIME4:
		{
			double val = ref->GetTime();
			if (0.0 < val)
				text << ARBDouble::ToString(val);
		}
		break;
	case CODE_REFSCORE1:
	case CODE_REFSCORE2:
	case CODE_REFSCORE3:
	case CODE_REFSCORE4:
		text << ref->GetScore();
		break;
	case CODE_REFHT1:
	case CODE_REFHT2:
	case CODE_REFHT3:
	case CODE_REFHT4:
		text << ref->GetHeight();
		break;
	case CODE_REF1:
	case CODE_REF2:
	case CODE_REF3:
	case CODE_REF4:
		text << ref->GetName();
		if (!ref->GetBreed().empty())
			text << L"/" << ref->GetBreed();
		if (!ref->GetNote().empty())
			text << L"/" << ref->GetNote();
		break;
	}
}


std::wstring CPrintRuns::GetFieldText(ARBDogTrialPtr trial, ARBDogRunPtr run, int code)
{
	std::wostringstream text;
	switch (code)
	{
	default:
		break;
	case CODE_DOG:
		if (m_dog)
			text << m_dog->GetCallName();
		break;
	case CODE_DATE:
		if (run)
			text << run->GetDate().GetString();
		break;
	case CODE_VENUE:
		if (trial)
		{
			int i = 0;
			for (ARBDogClubList::iterator iter = trial->GetClubs().begin();
				iter != trial->GetClubs().end();
				++iter, ++i)
			{
				if (0 < i)
					text << L"/";
				text << (*iter)->GetVenue();
			}
			break;
		}
	case CODE_CLUB:
		if (trial)
		{
			int i = 0;
			for (ARBDogClubList::iterator iter = trial->GetClubs().begin();
				iter != trial->GetClubs().end();
				++iter, ++i)
			{
				if (0 < i)
					text << L"/";
				text << (*iter)->GetName();
			}
			break;
		}
	case CODE_DIV:
		if (run)
		{
			text << run->GetDivision()
				<< L"/" << run->GetLevel()
				<< L"/" << run->GetEvent();
		}
		break;
	case CODE_LOCATION:
		if (trial)
			text << trial->GetLocation();
		break;
	case CODE_HEIGHT:
		if (run)
			text << run->GetHeight();
		break;
	case CODE_JUDGE:
		if (run)
			text << run->GetJudge();
		break;
	case CODE_HANDLER:
		if (run)
			text << run->GetHandler();
		break;
	case CODE_CONDITIONS:
		if (run)
			text << run->GetConditions();
		break;
	case CODE_Q:
		if (run)
		{
			if ((ARB_Q::eQ)run->GetQ() != ARB_Q::eQ_NA)
				text << run->GetQ().str();
		}
		break;
	case CODE_SCT:
		if (run)
		{
			double val = run->GetScoring().GetSCT();
			if (0.0 < val)
				text << ARBDouble::ToString(val);
		}
		break;
	case CODE_YARDS:
		if (run)
		{
			double val = run->GetScoring().GetYards();
			if (0.0 < val)
				text << ARBDouble::ToString(val, 0);
		}
		break;
	case CODE_OPEN:
		if (run)
		{
			switch (run->GetScoring().GetType())
			{
			default:
				break;
			case ARBDogRunScoring::eTypeByOpenClose:
				if (0 < run->GetScoring().GetNeedOpenPts())
					text << run->GetScoring().GetNeedOpenPts();
				text << L" / ";
				if (0 < run->GetScoring().GetNeedClosePts())
					text << run->GetScoring().GetNeedClosePts();
				break;
			case ARBDogRunScoring::eTypeByPoints:
				if (0 < run->GetScoring().GetNeedOpenPts())
					text << run->GetScoring().GetNeedOpenPts();
				break;
			}
		}
		break;
	case CODE_TIME:
		if (run)
		{
			double val = run->GetScoring().GetTime();
			if (0.0 < val)
				text << ARBDouble::ToString(val);
		}
		break;
	case CODE_FAULTS:
		if (trial && run)
		{
			ARBConfigScoringPtr pScoring;
			if (m_config && trial->GetClubs().GetPrimaryClub())
				m_config->GetVenues().FindEvent(
					trial->GetClubs().GetPrimaryClubVenue(),
					run->GetEvent(),
					run->GetDivision(),
					run->GetLevel(),
					run->GetDate(),
					NULL,
					&pScoring);
			double timeFaults = run->GetScoring().GetTimeFaults(pScoring);
			if (run->GetQ().AllowTally()
			|| (0 < run->GetScoring().GetCourseFaults() || 0.0 < timeFaults))
			{
				text << run->GetScoring().GetCourseFaults();
				if (0.0 < timeFaults)
					text << L"+" << ARBDouble::ToString(timeFaults, 0);
			}
		}
		break;
	case CODE_SCORE:
		if (run)
		{
			switch (run->GetScoring().GetType())
			{
			default:
				break;
			case ARBDogRunScoring::eTypeByOpenClose:
				if (0 < run->GetScoring().GetOpenPts())
					text << run->GetScoring().GetOpenPts();
				text << L" / ";
				if (0 < run->GetScoring().GetClosePts())
					text << run->GetScoring().GetClosePts();
				break;
			case ARBDogRunScoring::eTypeByPoints:
				if (0 < run->GetScoring().GetOpenPts())
					text << run->GetScoring().GetOpenPts();
				break;
			}
		}
		break;
	case CODE_PLACE:
		if (run && 0 < run->GetPlace())
			text << run->GetPlace();
		break;
	case CODE_INCLASS:
		if (run && 0 < run->GetInClass())
			text << run->GetInClass();
		break;
	case CODE_QD:
		if (run && 0 <= run->GetDogsQd())
			text << run->GetDogsQd();
		break;
	case CODE_COMMENTS:
		if (run)
			text << run->GetNote();
		break;
	case CODE_OTHER:
		if (run && 0 < run->GetOtherPoints().size())
		{
			int i = 0;
			for (ARBDogRunOtherPointsList::iterator iter = run->GetOtherPoints().begin();
				iter != run->GetOtherPoints().end();
				++iter, ++i)
			{
				if (0 < i)
					text << L" ";
				text << (*iter)->GetName() << L":" << (*iter)->GetPoints();
			}
		}
		break;
	case CODE_REFPLACE1:
	case CODE_REFQ1:
	case CODE_REFTIME1:
	case CODE_REFSCORE1:
	case CODE_REFHT1:
	case CODE_REF1:
		if (run && 0 < run->GetReferenceRuns().size())
			RefRunHelper(text, run->GetReferenceRuns()[0], code);
		break;
	case CODE_REFPLACE2:
	case CODE_REFQ2:
	case CODE_REFTIME2:
	case CODE_REFSCORE2:
	case CODE_REFHT2:
	case CODE_REF2:
		if (run && 1 < run->GetReferenceRuns().size())
			RefRunHelper(text, run->GetReferenceRuns()[1], code);
		break;
	case CODE_REFPLACE3:
	case CODE_REFQ3:
	case CODE_REFTIME3:
	case CODE_REFSCORE3:
	case CODE_REFHT3:
	case CODE_REF3:
		if (run && 2 < run->GetReferenceRuns().size())
			RefRunHelper(text, run->GetReferenceRuns()[2], code);
		break;
	case CODE_REFPLACE4:
	case CODE_REFQ4:
	case CODE_REFTIME4:
	case CODE_REFSCORE4:
	case CODE_REFHT4:
	case CODE_REF4:
		if (run && 3 < run->GetReferenceRuns().size())
			RefRunHelper(text, run->GetReferenceRuns()[3], code);
		break;
	}
	return text.str();
}


void CPrintRuns::PrintPage(int nCurPage, size_t curRun, wxDC* pDC, wxRect inRect)
{
	wxBrush brNull(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW), wxTRANSPARENT);
	pDC->SetBrush(brNull);
	pDC->SetBackgroundMode(wxTRANSPARENT);

	wxFont fontText(5, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxFont fontData(8, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

	pDC->SetFont(fontText);
	pDC->SetPen(*wxBLACK_PEN);

	int h = inRect.height / 28;
	int w = inRect.width / 8;
	wxRect r[2];
	r[0] = inRect;
	r[0].height = h * 14;
	r[1] = r[0];
	r[1].y = inRect.GetBottom() - r[1].height;

	for (int iItem = 0; iItem < 2; ++iItem, ++curRun)
	{
		ARBDogTrialPtr trial;
		ARBDogRunPtr run;
		bool bPoints = false;
		if (m_runs.size() == 0)
		{
			bPoints = (0 == nCurPage % 2);
		}
		else if (curRun >= m_runs.size())
		{
			break;
		}
		else
		{
			trial = m_runs[curRun].first;
			run = m_runs[curRun].second;
			if (!run)
				continue;
			switch (run->GetScoring().GetType())
			{
			default:
			case ARBDogRunScoring::eTypeByTime:
				break;
			case ARBDogRunScoring::eTypeByOpenClose:
			case ARBDogRunScoring::eTypeByPoints:
				bPoints = true;
				break;
			}
		}
		// Frame the whole thing
		pDC->DrawRectangle(r[iItem]);

		// Now go thru each box.
		for (int j = 0; j < sc_nLines; ++j)
		{
			if ((bPoints && (sc_lines[j].type & FOR_PTS))
			|| (!bPoints && (sc_lines[j].type & FOR_TIME)))
			{
				wxRect rect;
				rect.x = r[iItem].x + w * sc_lines[j].box;
				rect.width = w * sc_lines[j].colspan + 1;
				if (8 == sc_lines[j].box + sc_lines[j].colspan)
					rect.SetRight(r[iItem].GetRight());
				rect.y = r[iItem].y + h * sc_lines[j].line;
				rect.height = + h;
				// Draw vertical separator lines (on left)
				if (0 < sc_lines[j].box)
				{
					pDC->DrawLine(rect.x, rect.y, rect.x, rect.GetBottom());
				}

				std::wstring str = GetFieldText(trial, run, sc_lines[j].code);

				// Draw horizontal separator lines (on top)
				if (0 < sc_lines[j].line && (str.empty() || !sc_lines[j].bContinuation))
				{
					bool bResetPen = false;
					if (sc_lines[j].bContinuation)
					{
						bResetPen = true;
						pDC->SetPen(*wxGREY_PEN);
					}
					pDC->DrawLine(rect.x, rect.y, rect.GetRight(), rect.y);
					if (bResetPen)
						pDC->SetPen(*wxBLACK_PEN);
				}
				if (sc_lines[j].bContinuation)
					str.clear();

				rect.Inflate(-1, -1);
				int textHeight = 0;
				if (!sc_lines[j].bContinuation)
				{
					wxString caption = wxGetTranslation(sc_codes[sc_lines[j].code]);
					wxDCClipper clip(*pDC, rect);
					pDC->DrawLabel(caption, rect, wxALIGN_LEFT | wxALIGN_TOP);
					textHeight = pDC->GetTextExtent(caption).y;
				}
				if (!str.empty())
				{
					if (1 < sc_lines[j].rowspan)
						rect.SetBottom(rect.GetBottom() + rect.height * sc_lines[j].rowspan);
					pDC->SetFont(fontData);
					if (sc_lines[j].bWorkBreak)
					{
						rect.y += textHeight;
						rect.height -= textHeight;
						DrawBetterLabel(pDC, StringUtil::stringWX(str), rect, sc_lines[j].fmt, false);
					}
					else
					{
						wxDCClipper clip(*pDC, rect);
						pDC->DrawLabel(StringUtil::stringWX(str), rect, sc_lines[j].fmt);
					}
					pDC->SetFont(fontText);
				}
			}
		}
	}

	pDC->SetPen(wxNullPen);
	pDC->SetFont(wxNullFont);
	pDC->SetBrush(wxNullBrush);
}


enum RingBinder
{
	eSmall3Ring,
	eLarge3Ring,
	eLarge4Ring
};


static void PrintMark(wxDC* pDC, wxCoord x, wxCoord y, double oneInch)
{
	wxCoord halfLine = static_cast<wxCoord>(oneInch / 10);
	pDC->DrawLine(x - halfLine, y - halfLine, x + halfLine, y + halfLine);
	pDC->DrawLine(x + halfLine, y - halfLine, x - halfLine, y + halfLine);
}

// Binder rings (based on US 8.5x11 paper)
// Rings are inset 3/8 inch
// sm 3ring: 2.75
// lg 3ring: 4.25
// lg 4ring: 1.375/4.25/1.375
static void PrintBinderMarkings(
		RingBinder style,
		wxDC* pDC,
		wxRect rPrinted,
		int margin,
		double oneInch)
{
	pDC->SetPen(*wxGREY_PEN);
	wxCoord x = static_cast<wxCoord>(rPrinted.x - margin + 0.375 * oneInch);
	wxCoord y = rPrinted.y + rPrinted.height / 2; // centered
	switch (style)
	{
	default:
	case eSmall3Ring:
		PrintMark(pDC, x, y, oneInch);
		PrintMark(pDC, x, static_cast<wxCoord>(y - 2.75 * oneInch), oneInch);
		PrintMark(pDC, x, static_cast<wxCoord>(y + 2.75 * oneInch), oneInch);
		break;
	case eLarge3Ring:
		PrintMark(pDC, x, y, oneInch);
		PrintMark(pDC, x, static_cast<wxCoord>(y - 4.25 * oneInch), oneInch);
		PrintMark(pDC, x, static_cast<wxCoord>(y + 4.25 * oneInch), oneInch);
		break;
	case eLarge4Ring:
		PrintMark(pDC, x, static_cast<wxCoord>(y - 3.5 * oneInch), oneInch); // 1 3/8 + 2 1/8
		PrintMark(pDC, x, static_cast<wxCoord>(y - 2.125 * oneInch), oneInch);
		PrintMark(pDC, x, static_cast<wxCoord>(y + 2.125 * oneInch), oneInch);
		PrintMark(pDC, x, static_cast<wxCoord>(y + 3.5 * oneInch), oneInch);
		break;
	}
	pDC->SetPen(wxNullPen);
}


bool CPrintRuns::OnPrintPage(int pageNum)
{
	ComputeScaling();
	wxRect r = GetLogicalPageRect(); // 0-normalized
	wxRect rSheet = GetLogicalPaperRect(); // negative x

	long marginL, marginR, marginT, marginB;
	CAgilityBookOptions::GetPrinterMargins(false, marginL, marginR, marginT, marginB, NULL);
	marginL = static_cast<long>((marginL * m_OneInch) / 100.0);
	marginR = static_cast<long>((marginR * m_OneInch) / 100.0);
	marginT = static_cast<long>((marginT * m_OneInch) / 100.0);
	marginB = static_cast<long>((marginB * m_OneInch) / 100.0);

	bool bPrintBox = false;
	long width, height;
	CAgilityBookOptions::GetRunPageSize(false, width, height, NULL);
	// This logic should match GetPageInfo
	if (0 < width && 0 < height)
	{
		bPrintBox = true;
		width = static_cast<long>((width * m_OneInch) / 100.0);
		height = static_cast<long>((height * m_OneInch) / 100.0);
		if (1 > r.width / width)
			width = rSheet.width;
		if (1 > r.height / height)
			height = rSheet.height;
	}
	else
	{
		width = rSheet.width;
		height = rSheet.height;
		if (abs(width) > abs(height)) // landscape
			width /= 2;
	}

	size_t curRun = (pageNum - 1) * m_nPerSheet;

	// Make sure our margins are at least as large as the physical printer margins.
	if (marginL < abs(rSheet.x))
		marginL = abs(rSheet.x);
	if (marginR < rSheet.width - r.width + rSheet.x)
		marginR = rSheet.width - r.width + rSheet.x;
	if (marginT < abs(rSheet.y))
		marginT = abs(rSheet.y);
	if (marginB < rSheet.height - r.height + rSheet.y)
		marginB = rSheet.height - r.height + rSheet.y;

	for (int iRow = 0; iRow < m_nPagesY; ++iRow)
	{
		for (int iCol = 0; iCol < m_nPagesX; ++iCol)
		{
			wxRect rPage;
			rPage.x = rSheet.x + iCol * width;
			rPage.y = rSheet.y + iRow * height;
			rPage.width = width;
			rPage.height = height;
			if (bPrintBox)
			{
				GetDC()->DrawLine(
					rSheet.x + (iCol + 1) * width,
					rPage.y + marginT,
					rSheet.x + (iCol + 1) * width,
					rPage.y + height - marginB);
			}
			rPage.x += marginL;
			rPage.y += marginT;
			rPage.width -= (marginL + marginR);
			rPage.height -= (marginT + marginB);
			PrintPage(pageNum, curRun, GetDC(), rPage);
			curRun += 2;
			if (1 == m_nPagesX && 1 == m_nPagesY)
			{
				if (!bPrintBox)
				{
					PrintBinderMarkings(eLarge3Ring, GetDC(), rPage, marginL, m_OneInch);
					PrintBinderMarkings(eLarge4Ring, GetDC(), rPage, marginL, m_OneInch);
				}
			}
			else
			{
				PrintBinderMarkings(eSmall3Ring, GetDC(), rPage, marginL, m_OneInch);
			}
		}
		if (bPrintBox)
			GetDC()->DrawLine(
				rSheet.x + marginL,
				rSheet.y + (iRow + 1) * height,
				rSheet.x + marginL + m_nPagesX * width - (marginL + marginR),
				rSheet.y + (iRow + 1) * height);
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////

CHtmlEasyPrinting::CHtmlEasyPrinting(wxWindow* parent)
	: wxHtmlEasyPrinting(wxEmptyString, parent)
{
	//SetFooter(L"<hr/><p align=\"right\">@TITLE@ (@PAGENUM@/@PAGESCNT@)</p>", wxPAGE_ALL);

	long leftMargin, rightMargin, topMargin, bottomMargin;
	CAgilityBookOptions::GetPrinterMargins(true, leftMargin, rightMargin, topMargin, bottomMargin, NULL);
	wxPageSetupDialogData* pSetup = GetPageSetupData();
	pSetup->SetMarginTopLeft(wxPoint(leftMargin, topMargin));
	pSetup->SetMarginBottomRight(wxPoint(rightMargin, bottomMargin));

	CFontInfo info;
	CAgilityBookOptions::GetPrinterFontInfo(info);
	SetStandardFonts(info.size, StringUtil::stringWX(info.name));
}


bool CHtmlEasyPrinting::DoPreview(
		wxHtmlPrintout* printout1,
		wxHtmlPrintout* printout2)
{
	// Pass two printout objects: for preview, and possible printing.
	CPrintPreview *preview = new CPrintPreview(printout1, printout2, wxGetApp().GetPrintData());
	if (!preview->Ok())
	{
		delete preview;
		wxMessageBox(_("Sorry, print preview needs a printer to be installed."));
		return false;
	}
	wxPreviewFrame *frame = new wxPreviewFrame(preview, wxGetApp().GetTopWindow(), _("Print Preview"), wxDefaultPosition, wxGetApp().GetTopWindow()->GetSize()); 
	frame->Centre(wxBOTH);
	frame->Initialize();
	frame->Show(true);
	return true;
}


bool CHtmlEasyPrinting::DoPrint(wxHtmlPrintout* printout)
{
	wxPrinter printer(wxGetApp().GetPrintData());
	if (!printer.Print(wxGetApp().GetTopWindow(), printout, true))
		return false;
	wxGetApp().SavePrintData(printer.GetPrintDialogData());
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool PrintRuns(
		ARBConfig const* inConfig,
		ARBDogPtr inDog,
		std::vector<RunInfo> const& inRuns)
{
	wxPrintPreviewBase *preview = new wxPrintPreview(
		new CPrintRuns(inConfig, inDog, inRuns), // preview
		new CPrintRuns(inConfig, inDog, inRuns), // printer
		wxGetApp().GetPrintData());
	if (!preview->Ok())
	{
		delete preview;
		wxMessageBox(_("Sorry, print preview needs a printer to be installed."));
		return false;
	}
	wxPreviewFrame* frame = new wxPreviewFrame(preview, wxGetApp().GetTopWindow(), _("IDS_RUNS"), wxDefaultPosition, wxGetApp().GetTopWindow()->GetSize());
	frame->Centre(wxBOTH);
	frame->Initialize();
	frame->Show(true);
	return true;
}
