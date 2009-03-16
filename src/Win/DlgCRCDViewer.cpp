/*
 * Copyright © 2004-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgCRCDViewer class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2004-03-06 DRC Created
 */

#include "stdafx.h"
#include "DlgCRCDViewer.h"

#include <wx/dcbuffer.h>


#ifdef WIN32

class CMetaDataStatic : public wxStaticText
{
public:
	CMetaDataStatic(
			wxWindow* pParent,
			HENHMETAFILE inMetaFile);

private:
	HENHMETAFILE m_MetaFile;

	DECLARE_EVENT_TABLE()
	void OnPaint(wxPaintEvent& evt);
};


BEGIN_EVENT_TABLE(CMetaDataStatic, wxStaticText)
	EVT_PAINT(CMetaDataStatic::OnPaint)
END_EVENT_TABLE()


CMetaDataStatic::CMetaDataStatic(
		wxWindow* pParent,
		HENHMETAFILE inMetaFile)
	: m_MetaFile(inMetaFile)
{
	wxStaticText::Create(pParent, wxID_ANY,
		wxEmptyString,
		wxDefaultPosition, wxSize(450, 450), wxSUNKEN_BORDER);
}


void CMetaDataStatic::OnPaint(wxPaintEvent& evt)
{
	wxBufferedPaintDC dc(this);
	wxBrush br(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	dc.SetBrush(br);
	dc.Clear();
	dc.SetBrush(wxNullBrush);
	if (m_MetaFile)
	{
		wxSize sz = GetClientSize();
		RECT r;
		r.left = 0;
		r.top = 0;
		r.right = std::min(sz.x, sz.y);
		r.bottom = std::min(sz.x, sz.y);
		HDC hdc = (HDC)dc.GetHDC();
		PlayEnhMetaFile(hdc, m_MetaFile, &r);
	}
}

/////////////////////////////////////////////////////////////////////////////

CDlgCRCDViewer::CDlgCRCDViewer(
		HENHMETAFILE inMetaFile,
		wxWindow* pParent)
	: wxDialog(pParent, wxID_ANY, _("IDD_CRCD_VIEWER"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxMAXIMIZE_BOX | wxRESIZE_BORDER)
{
	// Controls (these are done first to control tab order)

	CMetaDataStatic* textCtrl = new CMetaDataStatic(this, inMetaFile);

	wxButton* btnClose = new wxButton(this, wxID_OK, _("IDC_VIEWER_CRCD_CLOSE"));
	btnClose->SetDefault();

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(textCtrl, 1, wxALL|wxEXPAND, 5);
	bSizer->Add(btnClose, 0, wxALIGN_RIGHT|wxALL, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	btnClose->SetFocus();
}

#endif
