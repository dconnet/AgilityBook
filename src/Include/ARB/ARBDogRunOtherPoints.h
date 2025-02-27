#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBDogRunOtherPoints and ARBDogRunOtherPointsList classes.
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBTypes2.h"
#include "LibwxARB.h"


namespace dconSoft
{
namespace ARB
{

/**
 * Other points are accumulated on a per-run basis.
 */
class ARB_API ARBDogRunOtherPoints : public ARBBase
{
protected:
	ARBDogRunOtherPoints();
	ARBDogRunOtherPoints(ARBDogRunOtherPoints const& rhs);
	ARBDogRunOtherPoints(ARBDogRunOtherPoints&& rhs);

public:
	~ARBDogRunOtherPoints();
	static ARBDogRunOtherPointsPtr New();
	ARBDogRunOtherPointsPtr Clone() const;

	ARBDogRunOtherPoints& operator=(ARBDogRunOtherPoints const& rhs);
	ARBDogRunOtherPoints& operator=(ARBDogRunOtherPoints&& rhs);

	bool operator==(ARBDogRunOtherPoints const& rhs) const;
	bool operator!=(ARBDogRunOtherPoints const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	wxString GetGenericName() const override
	{
		return m_Name;
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	size_t GetSearchStrings(std::set<wxString>& ioStrings) const override;

	/**
	 * Load an other point.
	 * @pre inTree is the actual ARBDogOtherPoints element.
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
	 * @post The ARBDogOtherPoints element will be created in ioTree.
	 */
	bool Save(ARBCommon::ElementNodePtr const& ioTree) const;

	/*
	 * Getters/setters.
	 */
	wxString const& GetName() const
	{
		return m_Name;
	}
	void SetName(wxString const& inName)
	{
		m_Name = inName;
	}
	double GetPoints() const
	{
		return m_Points;
	}
	void SetPoints(double inPts)
	{
		m_Points = inPts;
	}

private:
	wxString m_Name;
	double m_Points;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogRunOtherPoints objects.
 */
class ARB_API ARBDogRunOtherPointsList : public ARBVector<ARBDogRunOtherPointsPtr>
{
public:
	/**
	 * Load the information from XML (the tree).
	 * @pre inTree is the actual T element.
	 * @param inConfig Configuration information to verify data to load against.
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
	 * Add an other point.
	 * @param inOther OtherPoints to add.
	 * @return Whether the object was added.
	 */
	bool AddOtherPoints(ARBDogRunOtherPointsPtr const& inOther);
};

} // namespace ARB
} // namespace dconSoft
