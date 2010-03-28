/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Main propertysheet
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-08-26 DRC Fixed streaming wxString to otstringstream.
 *                    Fixed reading binary files. Added some timestamps.
 * @li 2009-03-01 DRC Ported to wxWidgets.
 */

#include "stdafx.h"
#include "DlgARBHelp.h"

#include "ARBHelp.h"
#include "BinaryData.h"
#include "DlgPageEncode.h"
#include "DlgPageEncodeFiles.h"
#include "DlgPageEncodeFinish.h"
#include <wx/ffile.h>
#include <wx/filename.h>

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


void CDlgARBHelp::AddSysInfo(wxString const& inData)
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
	wxString rawdata;

	// System information.
	{
		wxString data;
		BinaryData::EncodeString(m_SysInfo, data);
		rawdata << wxT("\n") << STREAM_SYSTEM_BEGIN << wxT("\n")
			<< data
			<< wxT("\n") << STREAM_SYSTEM_END << wxT("\n");
	}

	// Registry information.
	{
		wxString data;
		BinaryData::EncodeString(m_RegInfo, data);
		rawdata << wxT("\n") << STREAM_REGISTRY_BEGIN << wxT("\n")
			<< data
			<< wxT("\n") << STREAM_REGISTRY_END << wxT("\n");
	}

	// Data files.
	for (FileMap::iterator iFile = m_IncFile.begin(); iFile != m_IncFile.end(); ++iFile)
	{
		rawdata << wxT("\n") << (*iFile).first;
		if ((*iFile).second)
		{
			wxFFile file;
			if (file.Open((*iFile).first, wxT("rb")))
			{
				wxString data;
				wxFileName fileName((*iFile).first);
				wxDateTime dtMod, dtCreate;
				if (fileName.GetTimes(NULL, &dtMod, &dtCreate))
				{
					rawdata << wxT("\nCreated: ") << dtCreate.Format()
						<< wxT("\nModified: ") << dtMod.Format();
				}
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

	wxString data;
	BinaryData::EncodeString(rawdata, data);
	rawdata.clear();

	rawdata << wxT("\n") << STREAM_DATA_BEGIN << wxT("\n")
		<< data
		<< wxT("\n") << STREAM_DATA_END << wxT("\n");

	return rawdata;
}