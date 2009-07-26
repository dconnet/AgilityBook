#pragma once

/*
 * Copyright (c) 2003-2009 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgFind class
 * @author David Connet
 *
 * Revision History
 * @li 2009-01-26 DRC Ported to wxWidgets.
 * @li 2003-12-27 DRC Created
 */

class CDlgFind;

class IFindCallback
{
public:
	IFindCallback()
		: m_strCaption()
		, m_strSearch()
		, m_bMatchCase(false)
		, m_bEnableSearch(true)
		, m_bSearchAll(false)
		, m_bEnableDirection(true)
		, m_bDown(true)
	{
	}

	virtual wxString const& GetCaption() const	{return m_strCaption;}
	virtual wxString const& Text() const		{return m_strSearch;}
	virtual void Text(wxString const& text)		{m_strSearch = text;}
	virtual bool MatchCase() const				{return m_bMatchCase;}
	virtual void MatchCase(bool bCase)			{m_bMatchCase = bCase;}
	virtual bool EnableSearch() const			{return m_bEnableSearch;}
	virtual bool SearchAll() const				{return m_bSearchAll;}
	virtual void SearchAll(bool bAll)			{m_bSearchAll = bAll;}
	virtual bool EnableDirection() const		{return m_bEnableDirection;}
	virtual bool SearchDown() const				{return m_bDown;}
	virtual void SearchDown(bool bDown)			{m_bDown = bDown;}

	virtual bool Search(CDlgFind* pDlg) const = 0;

protected:
	wxString m_strCaption;
	wxString m_strSearch;
	bool m_bMatchCase;
	bool m_bEnableSearch;
	bool m_bSearchAll;
	bool m_bEnableDirection;
	bool m_bDown;
};


class CDlgFind : public wxDialog
{
public:
	CDlgFind(
			IFindCallback& callback,
			wxWindow* pParent = NULL);

private:
	IFindCallback& m_Callback;
	wxTextCtrl* m_textCtrl;
	wxCheckBox* m_checkBox;
	wxRadioBox* m_radioBoxSearch;
	wxRadioBox* m_radioBoxDir;
	wxButton* m_btnFind;

	void OnChangeName(wxCommandEvent& evt);
	void OnFind(wxCommandEvent& evt);
};
