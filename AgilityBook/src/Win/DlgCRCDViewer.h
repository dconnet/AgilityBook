#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgCRCDViewer class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-03-06 DRC Created
 */

#ifdef WIN32

// Metafiles are only available on Windows
class CDlgCRCDViewer : public wxDialog
{
public:
	CDlgCRCDViewer(
			HENHMETAFILE inMetaFile,
			wxWindow* pParent = NULL);
};

#endif
