/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgCRCDViewer class
 * @author David Connet
 *
 * Revision History
 * 2014-12-31 Changed pixels to dialog units.
 * 2009-02-11 Ported to wxWidgets.
 * 2004-03-06 Created
 */

#include "stdafx.h"
#include "DlgCRCDViewer.h"

#include <wx/dcbuffer.h>
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


#ifdef __WXMSW__

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


wxBEGIN_EVENT_TABLE(CMetaDataStatic, wxStaticText)
	EVT_PAINT(CMetaDataStatic::OnPaint)
wxEND_EVENT_TABLE()


CMetaDataStatic::CMetaDataStatic(
		wxWindow* pParent,
		HENHMETAFILE inMetaFile)
	: m_MetaFile(inMetaFile)
{
	wxStaticText::Create(pParent, wxID_ANY,
		wxEmptyString,
		wxDefaultPosition, wxDLG_UNIT(pParent, wxSize(260, 220)), wxSUNKEN_BORDER);
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}


void CMetaDataStatic::OnPaint(wxPaintEvent& evt)
{
	wxAutoBufferedPaintDC dc(this);
	dc.SetBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	dc.Clear();
	if (m_MetaFile)
	{
		wxSize sz = GetClientSize();
		RECT r;
		r.left = 0;
		r.top = 0;
		r.right = std::min(sz.x, sz.y);
		r.bottom = std::min(sz.x, sz.y);
		WXHDC hdc = dc.GetHDC();
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

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(textCtrl, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	bSizer->Add(btnClose, 0, wxALIGN_RIGHT | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	btnClose->SetFocus();
}

#endif
