/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Implementation of IARBConfigHandler
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-01-03 DRC Ported to wxWidgets.
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

	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	wxString datafile = wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + fileName.GetName() + wxT(".dat");
	std::string data;
	if (LoadWxFile(datafile, wxT("DefaultConfig.xml"), data))
		bOk = tree->LoadXMLBuffer(data.c_str(), data.length(), errMsg);

	return bOk ? tree : ElementNodePtr();
}


std::string CConfigHandler::LoadDTD(bool bNormalizeCRNL) const
{
	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	wxString datafile = wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + fileName.GetName() + wxT(".dat");

	std::string dtd;
	LoadWxFile(datafile, wxT("AgilityRecordBook.dtd"), dtd);

	if (bNormalizeCRNL)
		dtd = tstringUtil::Replace(dtd, "\r\n", "\n");
	return dtd;
}
