#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief Dialog for adding notes to ARBInfo items.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-10 DRC Ported to wxWidgets.
 * @li 2008-02-01 DRC Add ability to see what was last selected.
 * @li 2008-01-01 DRC Added visible flag.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2003-12-07 DRC Created
 */

#include "ARBInfo.h"
#include <set>
#include <vector>
class CAgilityBookDoc;
class CTextCtrl;
class wxBitmapComboBox;


class CDlgInfoNote : public wxDialog
{
public:
	CDlgInfoNote(
			CAgilityBookDoc* pDoc,
			ARBInfo::eInfoType inType,
			wxString const& inSelect,
			wxWindow* pParent = NULL);

	wxString CurrentSelection() const	{return m_CurSel;}

private:
	void UpdateImage(int index);
	void UpdateData();

	CAgilityBookDoc* m_pDoc;
	ARBInfo::eInfoType m_Type;
	wxString m_Select;
	std::set<wxString> m_NamesInUse;
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
		NameInfo(wxString const& inName);
		NameInfo(NameInfo const& rhs);
		bool operator==(NameInfo const& rhs);
		wxString m_Name;
		eUsage m_eInUse;
		bool m_bHasData;
	};
	std::vector<NameInfo> m_Names;
	size_t m_nAdded;
	wxString m_CurSel;
	wxBitmapComboBox* m_ctrlNames;
	wxButton* m_ctrlDelete;
	wxCheckBox* m_ctrlVisible;
	CTextCtrl* m_ctrlNotes;
	wxBitmap m_None;
	wxBitmap m_Note;
	wxBitmap m_Added;
	wxBitmap m_NoteAdded;

	DECLARE_EVENT_TABLE()
	void OnSelchangeName(wxCommandEvent& evt);
	void OnNewItem(wxCommandEvent& evt);
	void OnDeleteItem(wxCommandEvent& evt);
	void OnClickedJudgeVisible(wxCommandEvent& evt);
	void OnKillfocusComments(wxFocusEvent& evt);
	void OnOk(wxCommandEvent& evt);
};