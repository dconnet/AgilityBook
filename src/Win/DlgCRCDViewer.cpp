/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
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
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}


void CMetaDataStatic::OnPaint(wxPaintEvent& evt)
{
	wxBufferedPaintDC dc(this);
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
