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
#include "LibARBWin/Logger.h"
#include <set>


namespace dconSoft
{
namespace ARBWin
{
class CSpellCheckCtrl;
} // namespace ARBWin
class CDlgInfoNote;


class CDlgInfoNoteEdit : public wxDialog
{
public:
	CDlgInfoNoteEdit(wxString const& inName, wxString const& inComment, bool isVisible, CDlgInfoNote* parent);

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
	wxString m_OrigName;
	wxString m_Name;
	wxString m_Comment;
	bool m_isVisible;
	ARBWin::CSpellCheckCtrl* m_ctrlNotes;
	ARBWin::CStackLogger m_trace;

	void OnOk(wxCommandEvent& evt);
};

} // namespace dconSoft
