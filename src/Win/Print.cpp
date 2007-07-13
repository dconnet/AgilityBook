/*
 * Copyright © 2007 David Connet. All Rights Reserved.
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
 * @li 2007-07-13 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"

#include "ARBDog.h"
#include "ARBDogRun.h"
#include "ARBDogTrial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

// Added '2' to avoid dependencies on class defined (locally) in viewprnt.cpp.
// In fact, I suspect if I compiled using MFC DLLs, it wouldn't work otherwise
// Since I'm statically linking, the OnCancel/... actually resolve when linking
class CPrintingDialog2 : public CDialog
{
public:
	//{{AFX_DATA(CPrintingDialog2)
	enum { IDD = AFX_IDD_PRINTDLG };
	//}}AFX_DATA
	CPrintingDialog2::CPrintingDialog2(CWnd* pParent)
	{
		Create(CPrintingDialog2::IDD, pParent);      // modeless !
		_afxWinState->m_bUserAbort = FALSE;
	}
	virtual ~CPrintingDialog2() { }

	virtual BOOL OnInitDialog();
	virtual void OnCancel();
};

BOOL CPrintingDialog2::OnInitDialog()
{
	SetWindowText(AfxGetAppName());
	CenterWindow();
	return CDialog::OnInitDialog();
}

void CPrintingDialog2::OnCancel()
{
	_afxWinState->m_bUserAbort = TRUE;  // flag that user aborted print
	CDialog::OnCancel();
}

BOOL CALLBACK AbortProc(HDC, int)
{
	_AFX_WIN_STATE* pWinState = _afxWinState;
	MSG msg;
	while (!pWinState->m_bUserAbort &&
		::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
	{
		if (!AfxPumpMessage())
			return FALSE;   // terminate if WM_QUIT received
	}
	return !pWinState->m_bUserAbort;
}

/////////////////////////////////////////////////////////////////////////////

class CPrintRuns
{
public:
	CPrintRuns(
			ARBDogPtr inDog,
			std::vector<RunInfo> const& inRuns)
		: m_dog(inDog)
		, m_runs(inRuns)
		, m_cur(0)
	{
	}

	BOOL DoPreparePrinting(CPrintInfo* pInfo);
	BOOL OnPreparePrinting(CPrintInfo* pInfo);
	void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	void OnPrint(CDC* pDC, CPrintInfo* pInfo);

private:
	void PrintPage(UINT nCurPage, CDC* pDC, CRect inRect);

	ARBDogPtr m_dog;
	std::vector<RunInfo> const& m_runs;
	size_t m_cur;
};

BOOL CPrintRuns::DoPreparePrinting(CPrintInfo* pInfo)
{
	if (pInfo->m_pPD->m_pd.nMinPage > pInfo->m_pPD->m_pd.nMaxPage)
		pInfo->m_pPD->m_pd.nMaxPage = pInfo->m_pPD->m_pd.nMinPage;
	CWinApp* pApp = AfxGetApp();
	pInfo->m_pPD->m_pd.nFromPage = (WORD)pInfo->GetMinPage();
	pInfo->m_pPD->m_pd.nToPage = (WORD)pInfo->GetMaxPage();
	if (pApp->DoPrintDialog(pInfo->m_pPD) != IDOK)
		return FALSE;
	ASSERT(pInfo->m_pPD != NULL);
	ASSERT(pInfo->m_pPD->m_pd.hDC != NULL);
	if (pInfo->m_pPD->m_pd.hDC == NULL)
		return FALSE;
	pInfo->m_nNumPreviewPages = pApp->m_nNumPreviewPages;
	ENSURE(pInfo->m_strPageDesc.LoadString(AFX_IDS_PREVIEWPAGEDESC));
	return TRUE;
}

BOOL CPrintRuns::OnPreparePrinting(CPrintInfo* pInfo)
{
	pInfo->SetMinPage(1);
	if (0 == m_runs.size())
		pInfo->SetMaxPage(2);
	// Don't bother computing max page - we don't know
	// the paper orientation.
	return DoPreparePrinting(pInfo);
}

void CPrintRuns::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
}

void CPrintRuns::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	UINT nPages = static_cast<UINT>(m_runs.size());
	if (nPages > 0)
	{
		// Correct the max page based on paper orientation.
		UINT nRuns = static_cast<UINT>(m_runs.size());
		UINT nPages = 1;
		CRect r(pInfo->m_rectDraw);
		if (abs(r.Width()) > abs(r.Height()))
		{
			nPages = nRuns / 4;
			if (0 != nRuns % 4)
				++nPages;
		}
		else
		{
			nPages = nRuns / 2;
			if (0 != nRuns % 2)
				++nPages;
		}
		if (nPages < pInfo->GetMaxPage())
			pInfo->SetMaxPage(nPages);
	}
}

void CPrintRuns::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
}

#define CODE_Dog		0
#define CODE_Date		1
#define CODE_Venue		2
#define CODE_Club		3
#define CODE_Div		4
#define CODE_Location	5
#define CODE_Height		6
#define CODE_Judge		7
#define CODE_Handler	8
#define CODE_Conditions	9
#define CODE_Q			10
#define CODE_SCT		11
#define CODE_Yards		12
#define CODE_Open		13
#define CODE_Time		14
#define CODE_Faults		15
#define CODE_Score		16
#define CODE_Place		17
#define CODE_InClass	18
#define CODE_Qd			19
#define CODE_Comments	20
#define CODE_Other		21
#define CODE_RefPlace1	22
#define CODE_RefQ1		23
#define CODE_RefTime1	24
#define CODE_RefScore1	25
#define CODE_RefHt1		26
#define CODE_Ref1		27
#define CODE_RefPlace2	28
#define CODE_RefQ2		29
#define CODE_RefTime2	30
#define CODE_RefScore2	31
#define CODE_RefHt2		32
#define CODE_Ref2		33
#define CODE_RefPlace3	34
#define CODE_RefQ3		35
#define CODE_RefTime3	36
#define CODE_RefScore3	37
#define CODE_RefHt3		38
#define CODE_Ref3		39
#define CODE_RefPlace4	40
#define CODE_RefQ4		41
#define CODE_RefTime4	42
#define CODE_RefScore4	43
#define CODE_RefHt4		44
#define CODE_Ref4		45

// Make sure these are ordered by the aboves codes
static const struct { LPCTSTR caption; } codes[] = {
	_T("Dog"),
	_T("Date"),
	_T("Venue"),
	_T("Club"),
	_T("Div/level/event"),
	_T("Location"),
	_T("Height"),
	_T("Judge"),
	_T("Handler"),
	_T("Conditions"),
	_T("Q"),
	_T("SCT"),
	_T("Yards"),
	_T("Open/Close"),
	_T("Time"),
	_T("Faults"),
	_T("Score"),
	_T("Place"),
	_T("In Class"),
	_T("Qd"),
	_T("Comments"),
	_T("Other Points"),
	_T("Place"),
	_T("Q"),
	_T("Time"),
	_T("Score"),
	_T("Ht"),
	_T("Name/Breed/Notes"),
	_T("Place"),
	_T("Q"),
	_T("Time"),
	_T("Score"),
	_T("Ht"),
	_T("Name/Breed/Notes"),
	_T("Place"),
	_T("Q"),
	_T("Time"),
	_T("Score"),
	_T("Ht"),
	_T("Name/Breed/Notes"),
	_T("Place"),
	_T("Q"),
	_T("Time"),
	_T("Score"),
	_T("Ht"),
	_T("Name/Breed/Notes"),
};

static void RefRunHelper(ARBostringstream& text, ARBDogReferenceRunPtr ref, int code, bool& multiline)
{
	switch (code)
	{
	case CODE_RefPlace1:
	case CODE_RefPlace2:
	case CODE_RefPlace3:
	case CODE_RefPlace4:
		if (0 < ref->GetPlace())
			text << ref->GetPlace();
		break;
	case CODE_RefQ1:
	case CODE_RefQ2:
	case CODE_RefQ3:
	case CODE_RefQ4:
		if ((ARB_Q::eQ)ref->GetQ() != ARB_Q::eQ_NA)
			text << ref->GetQ().str();
		break;
	case CODE_RefTime1:
	case CODE_RefTime2:
	case CODE_RefTime3:
	case CODE_RefTime4:
		{
			double val = ref->GetTime();
			if (0.0 < val)
				text << ARBDouble::str(val);
		}
		break;
	case CODE_RefScore1:
	case CODE_RefScore2:
	case CODE_RefScore3:
	case CODE_RefScore4:
		text << ref->GetScore();
		break;
	case CODE_RefHt1:
	case CODE_RefHt2:
	case CODE_RefHt3:
	case CODE_RefHt4:
		text << ref->GetHeight();
		break;
	case CODE_Ref1:
	case CODE_Ref2:
	case CODE_Ref3:
	case CODE_Ref4:
		text << ref->GetName();
		if (!ref->GetBreed().empty())
			text << "/" << ref->GetBreed();
		if (!ref->GetNote().empty())
		{
			multiline = true;
			text << "/" << ref->GetNote();
		}
		break;
	}
}

void CPrintRuns::PrintPage(UINT nCurPage, CDC* pDC, CRect inRect)
{
	pDC->SelectStockObject(NULL_BRUSH);
	pDC->SetBkMode(TRANSPARENT);
	CFont fontText, fontData;
	fontText.CreatePointFont(70, _T("MS Sans Serif"), pDC);
	fontData.CreatePointFont(100, _T("Arial"), pDC);
	CFont* pOld = pDC->SelectObject(&fontText);

	int h = inRect.Height() / 29;
	int w = inRect.Width() / 8;
	CRect r[2];
	r[0] = inRect;
	r[1] = inRect;
	r[0].bottom = r[0].top + h * 14;
	r[1].top = r[1].bottom - h * 14;

	for (int i = 0; i < 2; ++i, ++m_cur)
	{
		ARBDogTrialPtr trial;
		ARBDogRunPtr run;
		bool bPoints = false;
		if (m_runs.size() == 0)
		{
			bPoints = (0 == nCurPage % 2);
		}
		else if (m_cur >= m_runs.size())
		{
			break;
		}
		else
		{
			trial = m_runs[m_cur].first;
			run = m_runs[m_cur].second;
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
		pDC->Rectangle(r[i]);
		// Individual lines
		for (int j = 1; j < 14; ++j)
		{
			pDC->MoveTo(r[i].left, r[i].top + h * j);
			pDC->LineTo(r[i].right, r[i].top + h * j);
		}
#define FOR_TIME	0x1
#define FOR_PTS		0x2
#define FOR_BOTH	0x3
		static const struct
		{
			unsigned char type;
			int line;
			int box;
			int span; // number of boxes to span
			int code;
		} lines[] =
		{
			{FOR_BOTH, 0, 0, 2, CODE_Dog},
			{FOR_BOTH, 0, 2, 2, CODE_Date},
			{FOR_BOTH, 0, 4, 2, CODE_Venue},
			{FOR_BOTH, 0, 6, 2, CODE_Club},
			{FOR_BOTH, 1, 0, 4, CODE_Div},
			{FOR_BOTH, 1, 4, 4, CODE_Location},
			{FOR_BOTH, 2, 0, 1, CODE_Height},
			{FOR_BOTH, 2, 1, 4, CODE_Judge},
			{FOR_BOTH, 2, 5, 3, CODE_Handler},
			{FOR_BOTH, 3, 0, 8, CODE_Conditions},
			{FOR_BOTH, 4, 0, 1, CODE_Q},
			{FOR_BOTH, 4, 1, 1, CODE_SCT},
			{FOR_TIME, 4, 2, 1, CODE_Yards},
			{FOR_PTS, 4, 2, 1, CODE_Open},
			{FOR_BOTH, 4, 3, 1, CODE_Time},
			{FOR_TIME, 4, 4, 1, CODE_Faults},
			{FOR_PTS, 4, 4, 1, CODE_Score},
			{FOR_BOTH, 4, 5, 1, CODE_Place},
			{FOR_BOTH, 4, 6, 1, CODE_InClass},
			{FOR_BOTH, 4, 7, 1, CODE_Qd},
			{FOR_BOTH, 5, 0, 6, CODE_Comments},
			{FOR_BOTH, 9, 6, 2, CODE_Other},
			{FOR_BOTH, 10, 0, 1, CODE_RefPlace1},
			{FOR_BOTH, 10, 1, 1, CODE_RefQ1},
			{FOR_BOTH, 10, 2, 1, CODE_RefTime1},
			{FOR_BOTH, 10, 3, 1, CODE_RefScore1},
			{FOR_BOTH, 10, 4, 1, CODE_RefHt1},
			{FOR_BOTH, 10, 5, 3, CODE_Ref1},
			{FOR_BOTH, 11, 0, 1, CODE_RefPlace2},
			{FOR_BOTH, 11, 1, 1, CODE_RefQ2},
			{FOR_BOTH, 11, 2, 1, CODE_RefTime2},
			{FOR_BOTH, 11, 3, 1, CODE_RefScore2},
			{FOR_BOTH, 11, 4, 1, CODE_RefHt2},
			{FOR_BOTH, 11, 5, 3, CODE_Ref2},
			{FOR_BOTH, 12, 0, 1, CODE_RefPlace3},
			{FOR_BOTH, 12, 1, 1, CODE_RefQ3},
			{FOR_BOTH, 12, 2, 1, CODE_RefTime3},
			{FOR_BOTH, 12, 3, 1, CODE_RefScore3},
			{FOR_BOTH, 12, 4, 1, CODE_RefHt3},
			{FOR_BOTH, 12, 5, 3, CODE_Ref3},
			{FOR_BOTH, 13, 0, 1, CODE_RefPlace4},
			{FOR_BOTH, 13, 1, 1, CODE_RefQ4},
			{FOR_BOTH, 13, 2, 1, CODE_RefTime4},
			{FOR_BOTH, 13, 3, 1, CODE_RefScore4},
			{FOR_BOTH, 13, 4, 1, CODE_RefHt4},
			{FOR_BOTH, 13, 5, 3, CODE_Ref4},
		};
		static const int nLines = sizeof(lines) / sizeof(lines[0]);
		for (int j = 0; j < nLines; ++j)
		{
			if ((bPoints && (lines[j].type & FOR_PTS))
			|| (!bPoints && (lines[j].type & FOR_TIME)))
			{
				CRect rect;
				rect.left = r[i].left + w * lines[j].box;
				rect.right = rect.left + w * lines[j].span;
				rect.top = r[i].top + h * lines[j].line;
				rect.bottom = rect.top + h;
				if (0 < lines[j].box)
				{
					pDC->MoveTo(rect.left, rect.top);
					pDC->LineTo(rect.left, rect.bottom);
				}
				rect.InflateRect(-1, -1);
				CRect rText(rect);
				pDC->DrawText(codes[lines[j].code].caption, -1, rect, DT_LEFT | DT_NOPREFIX);
				pDC->DrawText(codes[lines[j].code].caption, -1, rText, DT_LEFT | DT_NOPREFIX | DT_CALCRECT);
				if (run)
				{
					bool multiline = false;
					ARBostringstream text;
					switch (lines[j].code)
					{
					case CODE_Dog:
						text << m_dog->GetCallName();
						break;
					case CODE_Date:
						text << run->GetDate().GetString(ARBDate::eDashYYYYMMDD);
						break;
					case CODE_Venue:
						{
							ARBDogClubPtr club;
							if (trial->GetClubs().GetPrimaryClub(&club))
								text << club->GetVenue();
							break;
						}
					case CODE_Club:
						{
							ARBDogClubPtr club;
							if (trial->GetClubs().GetPrimaryClub(&club))
								text << club->GetName();
							break;
						}
					case CODE_Div:
						text << run->GetDivision()
							<< "/" << run->GetLevel()
							<< "/" << run->GetEvent();
						break;
					case CODE_Location:
						text << trial->GetLocation();
						break;
					case CODE_Height:
						text << run->GetHeight();
						break;
					case CODE_Judge:
						text << run->GetJudge();
						break;
					case CODE_Handler:
						text << run->GetHandler();
						break;
					case CODE_Conditions:
						multiline = true;
						rect.left = rText.right;
						text << run->GetConditions();
						break;
					case CODE_Q:
						if ((ARB_Q::eQ)run->GetQ() != ARB_Q::eQ_NA)
							text << run->GetQ().str();
						break;
					case CODE_SCT:
						{
							double val = run->GetScoring().GetSCT();
							if (0.0 < val)
								text << ARBDouble::str(val);
						}
						break;
					case CODE_Yards:
						{
							double val = run->GetScoring().GetYards();
							if (0.0 < val)
								text << ARBDouble::str(val, 0);
						}
						break;
					case CODE_Open:
						switch (run->GetScoring().GetType())
						{
						case ARBDogRunScoring::eTypeByOpenClose:
							if (0 < run->GetScoring().GetNeedOpenPts() || 0 < run->GetScoring().GetNeedClosePts())
								text << run->GetScoring().GetNeedOpenPts()
									<< "/" << run->GetScoring().GetNeedClosePts();
							break;
						case ARBDogRunScoring::eTypeByPoints:
							if (0 < run->GetScoring().GetNeedOpenPts())
								text << run->GetScoring().GetNeedOpenPts();
							break;
						}
						break;
					case CODE_Time:
						{
							double val = run->GetScoring().GetTime();
							if (0.0 < val)
								text << ARBDouble::str(val);
						}
						break;
					case CODE_Faults:
						{
							if (0 < run->GetScoring().GetCourseFaults())
								text << run->GetScoring().GetCourseFaults();
							/*
							ARBConfigScoringPtr pScoring;
							if (trial->GetClubs().GetPrimaryClub())
								m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
									trial->GetClubs().GetPrimaryClubVenue(),
									run->GetEvent(),
									run->GetDivision(),
									run->GetLevel(),
									run->GetDate(),
									NULL,
									&pScoring);
							double faults = run->GetScoring().GetCourseFaults() + run->GetScoring().GetTimeFaults(pScoring);
							text << ARBDouble::str(faults, 0);
							*/
						}
						break;
					case CODE_Score:
						switch (run->GetScoring().GetType())
						{
						case ARBDogRunScoring::eTypeByOpenClose:
							if (0 < run->GetScoring().GetOpenPts() || 0 < run->GetScoring().GetClosePts())
								text << run->GetScoring().GetOpenPts()
									<< "/" << run->GetScoring().GetClosePts();
						case ARBDogRunScoring::eTypeByPoints:
							if (0 < run->GetScoring().GetOpenPts())
								text << run->GetScoring().GetOpenPts();
						}
						break;
					case CODE_Place:
						if (0 < run->GetPlace())
							text << run->GetPlace();
						break;
					case CODE_InClass:
						if (0 < run->GetInClass())
							text << run->GetInClass();
						break;
					case CODE_Qd:
						if (0 <= run->GetDogsQd())
							text << run->GetDogsQd();
						break;
					case CODE_Comments:
						multiline = true;
						rect.top = rText.bottom + 1;
						text << run->GetNote();
						break;
					case CODE_Other:
						if (0 < run->GetOtherPoints().size())
						{
						}
						break;
					case CODE_RefPlace1:
					case CODE_RefQ1:
					case CODE_RefTime1:
					case CODE_RefScore1:
					case CODE_RefHt1:
					case CODE_Ref1:
						if (0 < run->GetReferenceRuns().size())
							RefRunHelper(text, run->GetReferenceRuns()[0], lines[j].code, multiline);
						break;
					case CODE_RefPlace2:
					case CODE_RefQ2:
					case CODE_RefTime2:
					case CODE_RefScore2:
					case CODE_RefHt2:
					case CODE_Ref2:
						if (1 < run->GetReferenceRuns().size())
							RefRunHelper(text, run->GetReferenceRuns()[1], lines[j].code, multiline);
						break;
					case CODE_RefPlace3:
					case CODE_RefQ3:
					case CODE_RefTime3:
					case CODE_RefScore3:
					case CODE_RefHt3:
					case CODE_Ref3:
						if (2 < run->GetReferenceRuns().size())
							RefRunHelper(text, run->GetReferenceRuns()[2], lines[j].code, multiline);
						break;
					case CODE_RefPlace4:
					case CODE_RefQ4:
					case CODE_RefTime4:
					case CODE_RefScore4:
					case CODE_RefHt4:
					case CODE_Ref4:
						if (3 < run->GetReferenceRuns().size())
							RefRunHelper(text, run->GetReferenceRuns()[3], lines[j].code, multiline);
						break;
					}
					ARBString str = text.str();
					if (!str.empty())
					{
						pDC->SelectObject(&fontData);
						UINT flags = DT_LEFT | DT_NOPREFIX;
						if (multiline)
							flags |= DT_WORDBREAK;
						else
							flags |= DT_SINGLELINE | DT_VCENTER;
						pDC->DrawText(str.c_str(), -1, rect, flags);
						pDC->SelectObject(&fontText);
					}
				}
			}
		}
	}

	pDC->SelectObject(pOld);
}

void CPrintRuns::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	// Units are .01 inches.
	pDC->SetMapMode(MM_LOENGLISH);

	// Drawing area. Note, located at 0,0.
	CRect r(pInfo->m_rectDraw);
	TRACE("rectDraw: %d,%d %d,%d (%dx%d)\n", pInfo->m_rectDraw.left, pInfo->m_rectDraw.top, pInfo->m_rectDraw.right, pInfo->m_rectDraw.bottom, pInfo->m_rectDraw.Width(), pInfo->m_rectDraw.Height());
	pDC->DPtoLP(r);
	// Physical size of page, larger than printing area.
	CSize szPhysical(pDC->GetDeviceCaps(PHYSICALWIDTH), pDC->GetDeviceCaps(PHYSICALHEIGHT));
	pDC->DPtoLP(&szPhysical);
	// Top/Left offset to printable area.
	CSize szOffset(pDC->GetDeviceCaps(PHYSICALOFFSETX), pDC->GetDeviceCaps(PHYSICALOFFSETY));
	pDC->DPtoLP(&szOffset);
	// Bottom/right to printable
	CSize szInset(szPhysical.cx - r.Width() - szOffset.cx, szPhysical.cy + r.Height() - szOffset.cy);

	// Indent 1/2 inch
	int margin = 50;

	// 'r' is full print area
	//pDC->Rectangle(r);
	CPoint pt = r.TopLeft();
	// Landscape
	if (abs(r.Width()) > abs(r.Height()))
	{
		//510x817
		CRect r1(r);
		r1.left += margin - szOffset.cx;
		r1.top -= margin - szOffset.cy;
		r1.right = r.left + r.Width() / 2 - margin;
		r1.bottom += margin - szInset.cy;
		PrintPage(pInfo->m_nCurPage, pDC, r1);

		r1.left = r1.right + margin;
		pDC->MoveTo(r1.TopLeft());
		pDC->LineTo(r1.left, r1.bottom);
		r1.left += margin;
		r1.right = r.right - (margin - szInset.cx);
		PrintPage(pInfo->m_nCurPage, pDC, r1);
	}
	// Portrait
	else
	{
		CRect r1(r);
		r1.left += margin - szOffset.cx;
		r1.top -= margin - szOffset.cy;
		r1.right -= margin - szInset.cx;
		r1.bottom += margin - szInset.cy;
		PrintPage(pInfo->m_nCurPage, pDC, r1);
	}
}

/////////////////////////////////////////////////////////////////////////////

bool PrintRuns(ARBDogPtr inDog, std::vector<RunInfo> const& inRuns)
{
	CPrintRuns runs(inDog, inRuns);

	// Style the printing after CView (code is based on CView::OnFilePrint)

	CPrintInfo printInfo;
	ASSERT(printInfo.m_pPD != NULL);    // must be set
	if (!runs.OnPreparePrinting(&printInfo))
		return false;
	// hDC must be set (did you remember to call DoPreparePrinting?)
	ASSERT(printInfo.m_pPD->m_pd.hDC != NULL);

	CString strTitle;
	strTitle = "Runs";

	DOCINFO docInfo;
	memset(&docInfo, 0, sizeof(DOCINFO));
	docInfo.cbSize = sizeof(DOCINFO);
	docInfo.lpszDocName = strTitle;
	docInfo.lpszOutput = NULL;
	CString strPortName = printInfo.m_pPD->GetPortName();

	CDC dcPrint;
	dcPrint.Attach(printInfo.m_pPD->m_pd.hDC);  // attach printer dc
	dcPrint.m_bPrinting = TRUE;

	runs.OnBeginPrinting(&dcPrint, &printInfo);

	dcPrint.SetAbortProc(AbortProc);

	CWnd* hwndTemp = AfxGetMainWnd();
	hwndTemp->EnableWindow(FALSE);
	CPrintingDialog2 dlgPrintStatus(AfxGetMainWnd());

	CString strTemp;
	dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_DOCNAME, strTitle);
	dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PRINTERNAME, printInfo.m_pPD->GetDeviceName());
	dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PORTNAME, strPortName);
	dlgPrintStatus.ShowWindow(SW_SHOW);
	dlgPrintStatus.UpdateWindow();

	// Initialise print document details
	// Begin a new print job
	printInfo.m_nJobNumber = dcPrint.StartDoc(&docInfo);
	if (printInfo.m_nJobNumber == SP_ERROR)
	{
		// enable main window before proceeding
		hwndTemp->EnableWindow(TRUE);
		// cleanup and show error message
		runs.OnEndPrinting(&dcPrint, &printInfo);
		dlgPrintStatus.DestroyWindow();
		dcPrint.Detach();   // will be cleaned up by CPrintInfo destructor
		AfxMessageBox(AFX_IDP_FAILED_TO_START_PRINT);
		return false;
	}

	// Guarantee values are in the valid range
	UINT nEndPage = printInfo.GetToPage();
	UINT nStartPage = printInfo.GetFromPage();

	if (nEndPage < printInfo.GetMinPage())
		nEndPage = printInfo.GetMinPage();
	if (nEndPage > printInfo.GetMaxPage())
		nEndPage = printInfo.GetMaxPage();

	if (nStartPage < printInfo.GetMinPage())
		nStartPage = printInfo.GetMinPage();
	if (nStartPage > printInfo.GetMaxPage())
		nStartPage = printInfo.GetMaxPage();

	int nStep = (nEndPage >= nStartPage) ? 1 : -1;
	nEndPage = (nEndPage == 0xffff) ? 0xffff : nEndPage + nStep;

	VERIFY(strTemp.LoadString(AFX_IDS_PRINTPAGENUM));

	bool bError = false;

	for (printInfo.m_nCurPage = nStartPage;
		printInfo.m_nCurPage != nEndPage;
		printInfo.m_nCurPage += nStep)
	{
		runs.OnPrepareDC(&dcPrint, &printInfo);
		// check for end of print
		if (!printInfo.m_bContinuePrinting)
			break;
		// write current page
		TCHAR szBuf[80];
		ATL_CRT_ERRORCHECK_SPRINTF(_sntprintf_s(szBuf, _countof(szBuf), _countof(szBuf) - 1, strTemp, printInfo.m_nCurPage));
		dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PAGENUM, szBuf);

		// set up drawing rect to entire page (in logical coordinates)
		printInfo.m_rectDraw.SetRect(0, 0,
			dcPrint.GetDeviceCaps(HORZRES),
			dcPrint.GetDeviceCaps(VERTRES));
		dcPrint.DPtoLP(&printInfo.m_rectDraw);

		// attempt to start the current page
		if (dcPrint.StartPage() < 0)
		{
			bError = true;
			break;
		}

		// must call OnPrepareDC on newer versions of Windows because
		// StartPage now resets the device attributes.
		runs.OnPrepareDC(&dcPrint, &printInfo);

		ASSERT(printInfo.m_bContinuePrinting);

		// page successfully started, so now render the page
		runs.OnPrint(&dcPrint, &printInfo);
		if (nStep > 0) // pages are printed in ascending order
		{
			// OnPrint may have set the last page
			// because the end of the document was reached.
			// The loop must not continue with the next iteration.
		// Is this a bug in CView? Leaving this here causes it to print all pages.
		// If there are 5 pages available, and I specified 1-2, this changes it
		// to print all 5.
			//nEndPage = printInfo.GetMaxPage() + nStep; 
		}

		// If the user restarts the job when it's spooling, all 
		// subsequent calls to EndPage returns < 0. The first time
		// GetLastError returns ERROR_PRINT_CANCELLED
		if (dcPrint.EndPage() < 0 && (GetLastError()!= ERROR_SUCCESS))
		{
			// Don't bother. CView does a bunch of stuff. We die.
			bError = true;
			break;
		}
		if (!AbortProc(dcPrint.m_hDC, 0))
		{		
			bError = true;
			break;
		}
	}
	if (!bError)
		dcPrint.EndDoc();
	else
		dcPrint.AbortDoc();

	hwndTemp->EnableWindow();    // enable main window

	runs.OnEndPrinting(&dcPrint, &printInfo);              // Call your "Clean up" function

	dlgPrintStatus.DestroyWindow();
	dcPrint.Detach();   // will be cleaned up by CPrintInfo destructor

	return bError;
}
