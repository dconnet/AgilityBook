#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBConfigOtherPoints and ARBConfigOtherPointsList class.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "ARB/ARBBase.h"
#include "ARB/ARBTypes2.h"


/**
 * A way to tally points that aren't inheritantly known.
 */
class ARBConfigOtherPoints : public ARBBase
{
public:
	/**
	 * How to accumulate points.
	 */
	typedef enum
	{
		eTallyAll,			///< Accumulate all runs together.
		eTallyAllByEvent,	///< Separate runs by event.
		eTallyLevel,		///< Separate runs by level.
		eTallyLevelByEvent	///< Separate runs by event and level.
	} eOtherPointsTally;

	static void GetTallyValidValues(std::vector<std::wstring>& outValues);

protected:
	ARBConfigOtherPoints();
	ARBConfigOtherPoints(ARBConfigOtherPoints const& rhs);

public:
	~ARBConfigOtherPoints();
	static ARBConfigOtherPointsPtr New();
	ARBConfigOtherPointsPtr Clone() const;

	ARBConfigOtherPoints& operator=(ARBConfigOtherPoints const& rhs);

	bool operator==(ARBConfigOtherPoints const& rhs) const;
	bool operator!=(ARBConfigOtherPoints const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Reset the contents of this object and all sub-objects.
	 */
	void clear();

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual std::wstring GetGenericName() const
	{
		return GetName();
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<std::wstring>& ioStrings) const
	{
		return 0;
	}

	/**
	 * Load an otherpoint configuration.
	 * @pre inTree is the actual ARBConfigOtherPoints element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigOtherPoints element will be created in ioTree.
	 */
	bool Save(ElementNodePtr ioTree) const;

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
	std::wstring const& GetDescription() const
	{
		return m_Desc;
	}
	void SetDescription(std::wstring const& inDesc)
	{
		m_Desc = inDesc;
	}
	eOtherPointsTally GetTally() const
	{
		return m_Tally;
	}
	void SetTally(eOtherPointsTally inTally)
	{
		m_Tally = inTally;
	}
	double GetDefault() const
	{
		return m_Default;
	}
	void SetDefault(double inDef)
	{
		m_Default = inDef;
	}

private:
	std::wstring m_Name;
	eOtherPointsTally m_Tally;
	std::wstring m_Desc;
	double m_Default;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigOtherPoints objects.
 */
class ARBConfigOtherPointsList : public ARBVector<ARBConfigOtherPointsPtr>
{
public:
	/**
	 * Load the information from XML (the tree).
	 * @pre inTree is the actual T element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Verify the name exists.
	 * @param inName Name to verify.
	 * @return Whether the OtherPoints exists.
	 */
	bool VerifyOtherPoints(std::wstring const& inName) const;

	/**
	 * Find an otherpoints object.
	 * @param inName Name of object to find.
	 * @param outPoints Pointer to object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindOtherPoints(
			std::wstring const& inName,
			ARBConfigOtherPointsPtr* outPoints = NULL) const;

	/**
	 * Add an otherpoints object.
	 * @param inOther Name of OtherPoints to add.
	 * @return Whether the object was added.
	 */
	bool AddOtherPoints(ARBConfigOtherPointsPtr inOther);

	/**
	 * Delete an object.
	 * @param inName Name of object to delete.
	 * @return Whether the object was deleted or not.
	 */
	bool DeleteOtherPoints(std::wstring const& inName);
};
