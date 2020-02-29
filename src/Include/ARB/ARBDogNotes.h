#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBDogNotes class.
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-03-03 Added CRCDMetaFile, string of Windows Enhanced Metafile.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 */

#include "ARBTypes2.h"
#include "LibwxARB.h"

#include <set>
#include <vector>

typedef std::vector<std::wstring> ARBDogFaultList;


class ARBMetaData;
typedef std::shared_ptr<ARBMetaData> ARBMetaDataPtr;
class ARB_API ARBMetaData
{
	friend class ARBDogNotes;
	DECLARE_NO_COPY_IMPLEMENTED(ARBMetaData)
protected:
	ARBMetaData();
public:
	static ARBMetaDataPtr MetaData();
	void Clear();
	~ARBMetaData();

	std::vector<unsigned char> const& data() const	{return m_Data;}

private:
	std::vector<unsigned char> m_Data;
};


/**
 * Various notes about a run.
 */
class ARB_API ARBDogNotes
{
public:
	ARBDogNotes();
	virtual ~ARBDogNotes();
	ARBDogNotes(ARBDogNotes const& rhs);
	ARBDogNotes(ARBDogNotes&& rhs);
	ARBDogNotes& operator=(ARBDogNotes const& rhs);
	ARBDogNotes& operator=(ARBDogNotes&& rhs);

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
	virtual size_t GetSearchStrings(std::set<std::wstring>& ioStrings) const;

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
			ElementNodePtr const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBDogNotes element will be created in ioTree.
	 */
	bool Save(ElementNodePtr const& ioTree) const;

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
	std::wstring const& GetCRCD() const
	{
		return m_CRCD;
	}
	void SetCRCD(std::wstring const& inCRCD)
	{
		m_CRCD = inCRCD;
	}
	std::wstring const& GetCRCDRawMetaData() const
	{
		return m_CRCDMeta;
	}
	ARBMetaDataPtr GetCRCDMetaData() const;
	void SetCRCDMetaData(std::vector<unsigned char> const& inCRCDMeta);
	std::wstring const& GetNote() const
	{
		return m_Note;
	}
	void SetNote(std::wstring const& inNote)
	{
		m_Note = inNote;
	}

private:
	ARBDogFaultList m_Faults;
	std::wstring m_CRCD;
	std::wstring m_CRCDMeta;
	std::wstring m_Note;
};
