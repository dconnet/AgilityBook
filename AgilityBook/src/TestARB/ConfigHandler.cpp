/*
 * Copyright (c) 2008-2009 David Connet. All Rights Reserved.
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
 * @brief Implementation of IARBConfigHandler
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-03-04 DRC Ported to wxWidgets.
 * @li 2008-10-31 DRC Created
 */

#include "stdafx.h"
#include "ConfigHandler.h"

#include "Element.h"
#include <wx/filesys.h>
#include <wx/mstream.h>
#include <wx/stdpaths.h>


bool CConfigHandler::LoadWxFile(
		wxString const& zipFile,
		wxString const& archiveFile,
		std::string& outData)
{
	outData.erase();
	wxString zipfile = wxFileSystem::FileNameToURL(zipFile);
	zipfile += wxT("#zip:") + archiveFile;
	wxFileSystem filesys;
	wxFSFile* file = filesys.OpenFile(zipfile);
	if (file)
	{
		size_t size = 0;
		wxInputStream* input = file->GetStream();
		while (input->CanRead())
		{
			char buffer[1024];
			size_t num = 1024;
			input->Read(buffer, num);
			outData.append(buffer, input->LastRead());
			size += input->LastRead();
		}
		delete file;
		return true;
	}
	else
	{
		wxLogMessage(zipfile);
		assert(file);
	}
	return false;
}


CConfigHandler::CConfigHandler()
{
}


ElementNodePtr CConfigHandler::LoadDefaultConfig() const
{
	bool bOk = false;
	wxString errMsg;
	ARBErrorCallback err(errMsg);
	ElementNodePtr tree(ElementNode::New());

#ifdef __WXMAC__
	// Command line programs on Mac are acting like unix. GetResourcesDir
	// returns /usr/local/share. And GetExecutablePath is returning nothing.
	wxString datafile = wxT("./testarb.dat");
#else
	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	wxString datafile = wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + fileName.GetName() + wxT(".dat");;
#endif
	std::string data;
	if (LoadWxFile(datafile, wxT("DefaultConfig.xml"), data))
		bOk = tree->LoadXMLBuffer(data.c_str(), data.length(), errMsg);

	return bOk ? tree : ElementNodePtr();
}


std::string CConfigHandler::LoadDTD(bool bNormalizeCRNL) const
{
#ifdef __WXMAC__
	// Command line programs on Mac are acting like unix. GetResourcesDir
	// returns /usr/local/share. And GetExecutablePath is returning nothing.
	wxString datafile = wxT("./testarb.dat");
#else
	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	wxString datafile = wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + fileName.GetName() + wxT(".dat");;
#endif

	std::string dtd;
	LoadWxFile(datafile, wxT("AgilityRecordBook.dtd"), dtd);

	if (bNormalizeCRNL)
		dtd = tstringUtil::Replace(dtd, "\r\n", "\n");
	return dtd;
}
