#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Dialog for adding notes to ARBInfo items.
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-10 Ported to wxWidgets.
 * 2008-02-01 Add ability to see what was last selected.
 * 2008-01-01 Added visible flag.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2003-12-07 Created
 */

#include "ARB/ARBInfo.h"
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
			std::wstring const& inSelect,
			wxWindow* pParent = NULL);

	std::wstring CurrentSelection() const;

private:
	void UpdateImage(int index);
	void UpdateData();

	CAgilityBookDoc* m_pDoc;
	ARBInfo::eInfoType m_Type;
	wxString m_Select;
	std::set<std::wstring> m_NamesInUse;
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
		NameInfo(std::wstring const& inName);
		NameInfo(NameInfo const& rhs);
		bool operator==(NameInfo const& rhs);
		std::wstring m_Name;
		eUsage m_eInUse;
		bool m_bHasData;
	};
	std::vector<NameInfo> m_Names;
	size_t m_nAdded;
	std::wstring m_CurSel;
	wxBitmapComboBox* m_ctrlNames;
	wxButton* m_ctrlDelete;
	wxCheckBox* m_ctrlVisible;
	CTextCtrl* m_ctrlNotes;
	wxBitmap m_None;
	wxBitmap m_Note;
	wxBitmap m_Added;
	wxBitmap m_NoteAdded;

	DECLARE_ON_INIT()
	DECLARE_EVENT_TABLE()
	void OnSelchangeName(wxCommandEvent& evt);
	void OnNewItem(wxCommandEvent& evt);
	void OnDeleteItem(wxCommandEvent& evt);
	void OnClickedJudgeVisible(wxCommandEvent& evt);
	void OnEnChangeComments(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
