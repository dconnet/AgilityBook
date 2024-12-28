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


namespace dconSoft
{
namespace ARB
{

typedef std::vector<wxString> ARBDogFaultList;


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

	std::vector<unsigned char> const& data() const
	{
		return m_Data;
	}

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
		ARBCommon::ElementNodePtr const& inTree,
		ARBCommon::ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBDogNotes element will be created in ioTree.
	 */
	bool Save(ARBCommon::ElementNodePtr const& ioTree) const;

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
	std::string const& GetCRCDRawMetaData() const
	{
		return m_CRCDMeta;
	}
	ARBMetaDataPtr GetCRCDMetaData() const;
	void SetCRCDMetaData(std::vector<unsigned char> const& inCRCDMeta);
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
	std::string m_CRCDMeta;
	wxString m_Note;
};

} // namespace ARB
} // namespace dconSoft
