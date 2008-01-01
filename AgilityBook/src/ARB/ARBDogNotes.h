#pragma once

/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
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
 * @brief ARBDogNotes class.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-03-03 DRC Added CRCDMetaFile, string of Windows Enhanced Metafile.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include <set>
#include <vector>
#include "ARBTypes.h"

typedef std::vector<tstring> ARBDogFaultList;

class ARBMetaData;
typedef boost::shared_ptr<ARBMetaData> ARBMetaDataPtr;
class ARBMetaData
{
	friend class ARBDogNotes;
	ARBMetaData();
public:
	static ARBMetaDataPtr MetaData();
	~ARBMetaData();

	size_t length() const		{return m_Bytes;}
	char const* data() const	{return m_Data;}
private:
	size_t m_Bytes;
	char* m_Data;
};

/**
 * Various notes about a run.
 */
class ARBDogNotes
{
public:
	ARBDogNotes();
	~ARBDogNotes();
	ARBDogNotes(ARBDogNotes const& rhs);
	ARBDogNotes& operator=(ARBDogNotes const& rhs);

	bool operator==(ARBDogNotes const& rhs) const;
	bool operator!=(ARBDogNotes const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<tstring>& ioStrings) const;

	/**
	 * Load an existing point.
	 * @pre inTree is the actual ARBDogNotes element.
	 * @param inConfig Configuration for looking up information.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ARBConfig const& inConfig,
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBDogNotes element will be created in ioTree.
	 */
	bool Save(ElementNodePtr ioTree) const;

	/*
	 * Getters/setters.
	 */
	ARBDogFaultList const& GetFaults() const
	{
		return m_Faults;
	}
	ARBDogFaultList& GetFaults()
	{
		return m_Faults;
	}
	tstring const& GetCRCD() const
	{
		return m_CRCD;
	}
	void SetCRCD(tstring const& inCRCD)
	{
		m_CRCD = inCRCD;
	}
	tstring const& GetCRCDRawMetaData() const
	{
		return m_CRCDMeta;
	}
	ARBMetaDataPtr GetCRCDMetaData() const;
	void SetCRCDMetaData(char const* inCRCDMeta, size_t inBytes);
	tstring const& GetNote() const
	{
		return m_Note;
	}
	void SetNote(tstring const& inNote)
	{
		m_Note = inNote;
	}

private:
	ARBDogFaultList m_Faults;
	tstring m_CRCD;
	tstring m_CRCDMeta;
	tstring m_Note;
};
