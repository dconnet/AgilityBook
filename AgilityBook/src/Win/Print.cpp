/*
 * Copyright © 2007-2009 David Connet. All Rights Reserved.
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
 * @brief Support for direct printing of runs.
 * @author David Connet
 *
 * Revision History
 * @li 2009-01-27 DRC Ported to wxWidgets.
 * @li 2009-01-21 DRC Leave gray separator line in when text in run is empty.
 * @li 2007-07-13 DRC Created
 */

#include "stdafx.h"
#include "Globals.h"

#include "AgilityBook.h"
#include "ARBConfig.h"
#include "ARBDog.h"
#include "ARBDogRun.h"
#include "ARBDogRunOtherPoints.h"
#include "ARBDogTrial.h"
#include "PointsData.h"
#include <wx/print.h>

/////////////////////////////////////////////////////////////////////////////

class CPrintRuns : public wxPrintout
{
public:
	CPrintRuns(
			ARBConfig const* inConfig,
			ARBDogPtr inDog,
			std::vector<RunInfo> const& inRuns);

	virtual bool HasPage(int page);
	virtual void GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo);
	virtual bool OnPrintPage(int pageNum);

private:
	tstring GetFieldText(ARBDogTrialPtr trial, ARBDogRunPtr run, int code);
	void PrintPage(int nCurPage, size_t curRun, wxDC* pDC, wxRect inRect);

	ARBConfig const* m_config;
	ARBDogPtr m_dog;
	std::vector<RunInfo> m_runs; // can't be const& since we're previewing now and actual vector goes out-of-scope
	int m_maxPage;
	int m_nPerSheet;
};


CPrintRuns::CPrintRuns(
		ARBConfig const* inConfig,
		ARBDogPtr inDog,
		std::vector<RunInfo> const& inRuns)
	: wxPrintout(_("IDS_RUNS"))
	, m_config(inConfig)
	, m_dog(inDog)
	, m_runs(inRuns)
	, m_maxPage(0)
	, m_nPerSheet(0)
{
}


bool CPrintRuns::HasPage(int page)
{
	return 1 <= page && page <= m_maxPage;
}


void CPrintRuns::GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo)
{
	*minPage = 1;
	if (0 == m_runs.size())
		*maxPage = 2;
	else
	{
		int nRuns = static_cast<int>(m_runs.size());
		int nPages = 1;
		wxSize sz = GetDC()->GetSize();
		if (abs(sz.x) > abs(sz.y))
			m_nPerSheet = 4;
		else
			m_nPerSheet = 2;
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
static const wxChar* sc_codes[] =
{
	wxT("IDS_COL_DOG"),
	wxT("IDS_COL_DATE"),
	wxT("IDS_COL_VENUE"),
	wxT("IDS_COL_CLUB"),
	wxT("IDS_COL_DIV_LVL_EVT"),
	wxT("IDS_COL_LOCATION"),
	wxT("IDS_COL_HEIGHT"),
	wxT("IDS_COL_JUDGE"),
	wxT("IDS_COL_HANDLER"),
	wxT("IDS_COL_CONDITIONS"),
	wxT("IDS_COL_Q"),
	wxT("IDS_COL_SCT"),
	wxT("IDS_COL_YARDS"),
	wxT("IDS_COL_OPEN_CLOSE"),
	wxT("IDS_COL_TIME"),
	wxT("IDS_COL_OBSTACLES"),
	wxT("IDS_COL_FAULTS"),
	wxT("IDS_COL_SCORE"),
	wxT("IDS_COL_PLACE"),
	wxT("IDS_COL_IN_CLASS"),
	wxT("IDS_COL_Q_D"),
	wxT("IDS_COL_OTHERPOINTS"),
	wxT("IDS_COL_COMMENTS"),
	wxT("IDS_COL_PLACE"),
	wxT("IDS_COL_Q"),
	wxT("IDS_COL_TIME"),
	wxT("IDS_COL_SCORE"),
	wxT("IDS_COL_HEIGHT"),
	wxT("IDS_COL_NAME_BREED_NOTE"),
	wxT("IDS_COL_PLACE"),
	wxT("IDS_COL_Q"),
	wxT("IDS_COL_TIME"),
	wxT("IDS_COL_SCORE"),
	wxT("IDS_COL_HEIGHT"),
	wxT("IDS_COL_NAME_BREED_NOTE"),
	wxT("IDS_COL_PLACE"),
	wxT("IDS_COL_Q"),
	wxT("IDS_COL_TIME"),
	wxT("IDS_COL_SCORE"),
	wxT("IDS_COL_HEIGHT"),
	wxT("IDS_COL_NAME_BREED_NOTE"),
	wxT("IDS_COL_PLACE"),
	wxT("IDS_COL_Q"),
	wxT("IDS_COL_TIME"),
	wxT("IDS_COL_SCORE"),
	wxT("IDS_COL_HEIGHT"),
	wxT("IDS_COL_NAME_BREED_NOTE"),
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
	UINT fmt;
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


static void RefRunHelper(otstringstream& text, ARBDogReferenceRunPtr ref, int code)
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
				text << ARBDouble::str(val);
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
			text << "/" << ref->GetBreed();
		if (!ref->GetNote().empty())
			text << "/" << ref->GetNote();
		break;
	}
}


tstring CPrintRuns::GetFieldText(ARBDogTrialPtr trial, ARBDogRunPtr run, int code)
{
	otstringstream text;
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
			text << run->GetDate().GetString(ARBDate::eDashYYYYMMDD);
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
					text << "/";
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
					text << "/";
				text << (*iter)->GetName();
			}
			break;
		}
	case CODE_DIV:
		if (run)
		{
			text << run->GetDivision()
				<< "/" << run->GetLevel()
				<< "/" << run->GetEvent();
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
				text << ARBDouble::str(val);
		}
		break;
	case CODE_YARDS:
		if (run)
		{
			double val = run->GetScoring().GetYards();
			if (0.0 < val)
				text << ARBDouble::str(val, 0);
		}
		break;
	case CODE_OPEN:
		if (run)
		{
			switch (run->GetScoring().GetType())
			{
			case ARBDogRunScoring::eTypeByOpenClose:
				if (0 < run->GetScoring().GetNeedOpenPts())
					text << run->GetScoring().GetNeedOpenPts();
				text << " / ";
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
				text << ARBDouble::str(val);
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
			if ((ARB_Q::eQ)run->GetQ() != ARB_Q::eQ_NA
			|| (0 < run->GetScoring().GetCourseFaults() || 0.0 < timeFaults))
			{
				text << run->GetScoring().GetCourseFaults();
				if (0.0 < timeFaults)
					text << "+" << ARBDouble::str(timeFaults, 0);
			}
		}
		break;
	case CODE_SCORE:
		if (run)
		{
			switch (run->GetScoring().GetType())
			{
			case ARBDogRunScoring::eTypeByOpenClose:
				if (0 < run->GetScoring().GetOpenPts())
					text << run->GetScoring().GetOpenPts();
				text << " / ";
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
					text << " ";
				text << (*iter)->GetName() << ":" << (*iter)->GetPoints();
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

				wxString str = GetFieldText(trial, run, sc_lines[j].code).c_str();

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

				rect.Inflate(-1, 1);
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
						DrawBetterLabel(pDC, str, rect, sc_lines[j].fmt, false);
					}
					else
					{
						wxDCClipper clip(*pDC, rect);
						pDC->DrawLabel(str, rect, sc_lines[j].fmt);
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


static enum RingBinder
{
	eSmall3Ring,
	eLarge3Ring,
	eLarge4Ring
};

static void PrintMark(wxDC* pDC, int x, int y)
{
	pDC->DrawLine(x - 5, y - 5, x + 5, y + 5);
	pDC->DrawLine(x + 5, y - 5, x - 5, y + 5);
}

// Binder rings (based on US 8.5x11 paper)
// Rings are inset 3/8 inch [27 twips]
// sm 3ring: 2.75 [198]
// lg 3ring: 4.25 [306]
// lg 4ring: 1.375/4.25/1.375 [99/306/99]
static void PrintBinderMarkings(
		RingBinder style,
		wxDC* pDC,
		wxRect rPrinted,
		int margin)
{
	pDC->SetPen(*wxGREY_PEN);
	int x = rPrinted.x - margin + 27;
	int y = rPrinted.y + rPrinted.height / 2; // centered
	switch (style)
	{
	default:
	case eSmall3Ring:
		PrintMark(pDC, x, y);
		PrintMark(pDC, x, y - 198);
		PrintMark(pDC, x, y + 198);
		break;
	case eLarge3Ring:
		PrintMark(pDC, x, y);
		PrintMark(pDC, x, y - 306);
		PrintMark(pDC, x, y + 306);
		break;
	case eLarge4Ring:
		PrintMark(pDC, x, y - 252); // 1 3/8 + 2 1/8 [99 + 153]
		PrintMark(pDC, x, y - 153); // 2 1/8 [153]
		PrintMark(pDC, x, y + 153);
		PrintMark(pDC, x, y + 252);
		break;
	}
	pDC->SetPen(wxNullPen);
}


bool CPrintRuns::OnPrintPage(int pageNum)
{
	GetDC()->SetMapMode(wxMM_POINTS);

	wxRect r(0,0,0,0);
	wxSize sz = GetDC()->GetSize();

	//float logUnitsFactor = 1.0; // factor to convert points into logical units
	if (IsPreview())
	{
		//TODO: This isn't right
		int ppiScreenX, ppiScreenY;
		GetPPIScreen(&ppiScreenX, &ppiScreenY);
		int ppiPrinterX, ppiPrinterY;
		GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);
		float scale = static_cast<float>(ppiPrinterX) / static_cast<float>(ppiScreenX);

		int w, h;
		GetDC()->GetSize(&w, &h);
		int pageWidth, pageHeight;
		GetPageSizePixels(&pageWidth, &pageHeight);

		float overallScale = scale * (static_cast<float>(w) / static_cast<float>(pageWidth));
		GetDC()->SetUserScale(overallScale, overallScale);

		//logUnitsFactor = static_cast<float>(ppiPrinterX) / (scale*72);

		sz = GetDC()->GetSize();
		r.width = GetDC()->DeviceToLogicalXRel(sz.x);
		r.height = GetDC()->DeviceToLogicalYRel(sz.y);
	}
	else
	{
		r.width = GetDC()->DeviceToLogicalXRel(sz.x);
		r.height = GetDC()->DeviceToLogicalYRel(sz.y);
	}

	// (old MFC note, not investigated further in wxWidgets)
	// Note, we could get the physical size of the page and try and get our
	// margin at exactly 1/2 inch. But observation has revealed that what the
	// printer reports as the offset, isn't what the printer prints as the
	// offset. So pretent the drawing area and physical area are the same.
	// (if printing to a pdf "printer", they are!)

	// Indent 1/2 inch
	int margin = 36;
	size_t curRun = (pageNum - 1) * m_nPerSheet;

	// 'r' is full print area
	//GetDC()->DrawRectangle(r);
	// Landscape
	if (abs(r.width) > abs(r.height))
	{
		//510x817 (old mfc pixels)
		wxRect r1(r);
		r1.x += margin;
		r1.y += margin;
		r1.width = r.width / 2 - 2 * margin;
		r1.height -= 2 * margin;
		PrintPage(pageNum, curRun, GetDC(), r1);
		PrintBinderMarkings(eSmall3Ring, GetDC(), r1, margin);

		r1.x += r1.width + margin;
		GetDC()->DrawLine(r1.x, r1.y, r1.x, r1.GetBottom());
		r1.x += margin;
		PrintPage(pageNum, curRun + 2, GetDC(), r1);
		PrintBinderMarkings(eSmall3Ring, GetDC(), r1, margin);
	}
	// Portrait
	else
	{
		wxRect r1(r);
		r1.x += margin;
		r1.y += margin;
		r1.width -= 2 * margin;
		r1.height -= 2 * margin;
		PrintPage(pageNum, curRun, GetDC(), r1);
		PrintBinderMarkings(eLarge3Ring, GetDC(), r1, margin);
		PrintBinderMarkings(eLarge4Ring, GetDC(), r1, margin);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool PrintRuns(
		ARBConfig const* inConfig,
		ARBDogPtr inDog,
		std::vector<RunInfo> const& inRuns)
{
	wxPrinter printer(wxGetApp().GetPrintData());
	CPrintRuns printout(inConfig, inDog, inRuns);
	if (!printer.Print(wxGetApp().GetTopWindow(), &printout, true))
	{
		if (wxPRINTER_ERROR == wxPrinter::GetLastError())
		{
			wxMessageBox(wxT("Problem!"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_ERROR);
		}
		else
		{
			// Cancelled
		}
		return false;
	}
	(*wxGetApp().GetPrintData()) = printer.GetPrintDialogData();

	/*
	 * TODO: Implement preview - as is, it works. But the drawing isn't right.
	 * Have to figure out the scaling.
	wxPrintPreview *preview = new wxPrintPreview(
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
	*/
	return true;
}
