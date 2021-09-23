#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgInfoNoteEdit class
 * @author David Connet
 *
 * Revision History
 * 2021-09-19 Created
 */

#include "ARB/ARBTypes2.h"
#include <set>
class CDlgInfoNote;
class CSpellCheckCtrl;


class CDlgInfoNoteEdit : public wxDialog
{
public:
	CDlgInfoNoteEdit(std::wstring const& inName, std::wstring const& inComment, bool isVisible, CDlgInfoNote* parent);

	wxString const& Name() const
	{
		return m_Name;
	}
	wxString const& Comment() const
	{
		return m_Comment;
	}
	bool IsVisible() const
	{
		return m_isVisible;
	}

private:
	CDlgInfoNote* m_parent;
	std::wstring m_OrigName;
	wxString m_Name;
	wxString m_Comment;
	bool m_isVisible;
	CSpellCheckCtrl* m_ctrlNotes;

	DECLARE_ON_INIT()
	void OnOk(wxCommandEvent& evt);
};
