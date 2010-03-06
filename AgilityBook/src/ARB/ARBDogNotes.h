#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 * @brief ARBDogNotes class.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-03-03 DRC Added CRCDMetaFile, string of Windows Enhanced Metafile.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include <set>
#include <vector>
#include "ARBTypes.h"

typedef std::vector<wxString> ARBDogFaultList;

class ARBMetaData;
typedef std::tr1::shared_ptr<ARBMetaData> ARBMetaDataPtr;
class ARBMetaData
{
	friend class ARBDogNotes;
	ARBMetaData();
public:
	static ARBMetaDataPtr MetaData();
	void Clear();
	~ARBMetaData();

	size_t length() const				{return m_Bytes;}
	unsigned char const* data() const	{return m_Data;}
private:
	size_t m_Bytes;
	unsigned char* m_Data;
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
	virtual size_t GetSearchStrings(std::set<wxString>& ioStrings) const;

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
	wxString const& GetCRCD() const
	{
		return m_CRCD;
	}
	void SetCRCD(wxString const& inCRCD)
	{
		m_CRCD = inCRCD;
	}
	wxString const& GetCRCDRawMetaData() const
	{
		return m_CRCDMeta;
	}
	ARBMetaDataPtr GetCRCDMetaData() const;
	void SetCRCDMetaData(unsigned char const* inCRCDMeta, size_t inBytes);
	wxString const& GetNote() const
	{
		return m_Note;
	}
	void SetNote(wxString const& inNote)
	{
		m_Note = inNote;
	}

private:
	ARBDogFaultList m_Faults;
	wxString m_CRCD;
	wxString m_CRCDMeta;
	wxString m_Note;
};