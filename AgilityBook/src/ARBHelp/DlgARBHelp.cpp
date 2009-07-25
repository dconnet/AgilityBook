/*
 * Copyright (c) 2007-2009 David Connet. All Rights Reserved.
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
 * @brief Main propertysheet
 * @author David Connet
 *
 * Revision History
 * @li 2009-03-01 DRC Ported to wxWidgets.
 */

#include "stdafx.h"
#include "DlgARBHelp.h"

#include "ARBHelp.h"
#include "BinaryData.h"
#include "DlgPageEncode.h"
#include "DlgPageEncodeFiles.h"
#include "DlgPageEncodeFinish.h"
#include <sstream>
#include <wx/ffile.h>

#include "../Win/res/AgilityBook16.xpm"
#include "../Win/res/AgilityBook32.xpm"
#include "../Win/res/AgilityBook48.xpm"


CDlgARBHelp::CDlgARBHelp()
	: wxWizard(NULL, wxID_ANY, wxT("Agility Record Book Helper"), wxNullBitmap, wxDefaultPosition, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	wxIconBundle icons;
	icons.AddIcon(wxIcon(AgilityBook16_xpm));
	icons.AddIcon(wxIcon(AgilityBook32_xpm));
	icons.AddIcon(wxIcon(AgilityBook48_xpm));
	SetIcons(icons);

	CDlgPageEncode* m_pageEncode = new CDlgPageEncode(this);
	CDlgPageEncodeFiles* page2 = new CDlgPageEncodeFiles(this);
	CDlgPageEncodeFinish* page3 = new CDlgPageEncodeFinish(this);

	wxWizardPageSimple::Chain(m_pageEncode, page2);
	wxWizardPageSimple::Chain(page2, page3);

	GetPageAreaSizer()->Add(m_pageEncode);
	GetPageAreaSizer()->Add(page2);
	GetPageAreaSizer()->Add(page3);

	RunWizard(m_pageEncode);
	Destroy();
}


void CDlgARBHelp::AddSysInfo(tstring const& inData)
{
	m_SysInfo.clear();
	if (!inData.empty())
		m_SysInfo = inData;
}


void CDlgARBHelp::AddRegistryInfo(wxChar const* inData)
{
	m_RegInfo.clear();
	if (inData)
		m_RegInfo = inData;
}


void CDlgARBHelp::SetARBFileStatus(wxString const& inFileName, bool bInclude)
{
	m_IncFile[inFileName] = bInclude;
}


// Make sure this is synchronized (order of encoding) with
// DlgPageDecode (decoder)
wxString CDlgARBHelp::GetEncodedData()
{
	// Scoping done to minimize memory usage.
	wxString arbData;
	{
		otstringstream rawdata;

		{
			tstring data;
			BinaryData::EncodeString(m_SysInfo, data);
			rawdata << wxT("\n") << STREAM_SYSTEM_BEGIN << wxT("\n")
				<< data
				<< wxT("\n") << STREAM_SYSTEM_END << wxT("\n");
		}

		{
			tstring data;
			BinaryData::EncodeString(m_RegInfo, data);
			rawdata << wxT("\n") << STREAM_REGISTRY_BEGIN << wxT("\n")
				<< data
				<< wxT("\n") << STREAM_REGISTRY_END << wxT("\n");
		}

		for (FileMap::iterator iFile = m_IncFile.begin(); iFile != m_IncFile.end(); ++iFile)
		{
			rawdata << wxT("\n") << (*iFile).first.c_str();
			if ((*iFile).second)
			{
				wxFFile file;
				if (file.Open((*iFile).first))
				{
					tstring data;
					BinaryData::Encode(file, data);
					rawdata << wxT("\n") << STREAM_FILE_BEGIN << wxT("\n")
						<< data
						<< wxT("\n") << STREAM_FILE_END << wxT("\n");
				}
				else
					rawdata << wxT(": Error: Cannot read file\n");
			}
			else
			{
				rawdata << wxT(": Skipped\n");
			}
		}

		tstring data;
		BinaryData::EncodeString(rawdata.str(), data);
		otstringstream encodedData;
		encodedData << wxT("\n") << STREAM_DATA_BEGIN << wxT("\n")
			<< data
			<< wxT("\n") << STREAM_DATA_END << wxT("\n");
		arbData = encodedData.str().c_str();
	}

	return arbData;
}
