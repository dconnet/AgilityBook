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

#include "ARB/ARBBase.h"
#include "ARB/ARBTypes2.h"


/**
 * Other points are accumulated on a per-run basis.
 */
class ARBDogRunOtherPoints : public ARBBase
{
protected:
	ARBDogRunOtherPoints();
	ARBDogRunOtherPoints(ARBDogRunOtherPoints const& rhs);

public:
	~ARBDogRunOtherPoints();
	static ARBDogRunOtherPointsPtr New();
	ARBDogRunOtherPointsPtr Clone() const;

	ARBDogRunOtherPoints& operator=(ARBDogRunOtherPoints const& rhs);

	bool operator==(ARBDogRunOtherPoints const& rhs) const;
	bool operator!=(ARBDogRunOtherPoints const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual std::wstring GetGenericName() const
	{
		return m_Name;
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<std::wstring>& ioStrings) const;

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
			ElementNodePtr const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBDogOtherPoints element will be created in ioTree.
	 */
	bool Save(ElementNodePtr const& ioTree) const;

	/*
	 * Getters/setters.
	 */
	std::wstring const& GetName() const
	{
		return m_Name;
	}
	void SetName(std::wstring const& inName)
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
	std::wstring m_Name;
	double m_Points;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogRunOtherPoints objects.
 */
class ARBDogRunOtherPointsList : public ARBVector<ARBDogRunOtherPointsPtr>
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
			ElementNodePtr const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Add an other point.
	 * @param inOther OtherPoints to add.
	 * @return Whether the object was added.
	 */
	bool AddOtherPoints(ARBDogRunOtherPointsPtr const& inOther);
};
