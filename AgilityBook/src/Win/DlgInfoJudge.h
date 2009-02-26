#pragma once

/*
 * Copyright � 2002-2009 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgInfoJudge class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-10 DRC Ported to wxWidgets.
 * @li 2008-02-01 DRC Add ability to see what was last selected.
 * @li 2008-01-01 DRC Added visible flag.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2003-12-07 DRC Created
 */

#include "ARBInfo.h"
//#include "ARBTypes.h"
//#include "ComboBox.h"
//#include "DlgBaseDialog.h"
//#include <set>
class CAgilityBookDoc;

class CDlgInfoJudge
{
public:
	CDlgInfoJudge(
			CAgilityBookDoc* pDoc,
			ARBInfo::eInfoType inType,
			tstring const& inSelect,
			wxWindow* pParent = NULL) {}
	wxString CurrentSelection() const	{return wxT("");}
	int ShowModal()
	{
		wxMessageBox(wxT("CDlgInfoJudge"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_INFORMATION);
		return wxID_CANCEL;
	}
};

#if 0
class CDlgInfoJudge : public CDlgBaseDialog
{
// Construction
public:
	CDlgInfoJudge(
			CAgilityBookDoc* pDoc,
			ARBInfo::eInfoType inType,
			tstring const& inSelect,
			CWnd* pParent = NULL);

	CString CurrentSelection() const	{return m_CurSel;}

private:
// Dialog Data
	//{{AFX_DATA(CDlgInfoJudge)
	enum { IDD = IDD_JUDGE_INFO };
	CButton	m_ctrlDelete;
	CComboBox2	m_ctrlNames;
	CButton		m_ctrlVisible;
	CEdit		m_ctrlComment;
	//}}AFX_DATA
	CAgilityBookDoc* m_pDoc;
	ARBInfo::eInfoType m_Type;
	tstring m_Select;
	std::set<tstring> m_NamesInUse;
	ARBInfoItemList const& m_InfoOrig;
	ARBInfoItemList m_Info;
	class NameInfo
	{
	public:
		typedef enum
		{
			eNotInUse,
			eInUse,
			eDeleted
		} eUsage;
		NameInfo();
		NameInfo(tstring const& inName);
		NameInfo(NameInfo const& rhs);
		bool operator==(NameInfo const& rhs);
		tstring m_Name;
		eUsage m_eInUse;
		bool m_bHasData;
	};
	std::vector<NameInfo> m_Names;
	size_t m_nAdded;
	CString m_CurSel;

// Overrides
	//{{AFX_VIRTUAL(CDlgInfoJudge)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDlgInfoJudge)
	virtual BOOL OnInitDialog();
	afx_msg int OnCompareItem(int nIDCtl, LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSelchangeName();
	afx_msg void OnBnClickedJudgeVisible();
	afx_msg void OnKillfocusComments();
	afx_msg void OnNew();
	afx_msg void OnDelete();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
