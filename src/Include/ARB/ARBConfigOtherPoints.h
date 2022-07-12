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
 * How to accumulate points.
 */
enum class ARBOtherPointsTally
{
	All,         ///< Accumulate all runs together.
	AllByEvent,  ///< Separate runs by event.
	Level,       ///< Separate runs by level.
	LevelByEvent ///< Separate runs by event and level.
};


/**
 * A way to tally points that aren't inheritantly known.
 */
class ARB_API ARBConfigOtherPoints : public ARBBase
{
public:
	static void GetTallyValidValues(std::vector<std::wstring>& outValues);

protected:
	ARBConfigOtherPoints();
	ARBConfigOtherPoints(ARBConfigOtherPoints const& rhs);
	ARBConfigOtherPoints(ARBConfigOtherPoints&& rhs);

public:
	~ARBConfigOtherPoints();
	static ARBConfigOtherPointsPtr New();
	ARBConfigOtherPointsPtr Clone() const;

	ARBConfigOtherPoints& operator=(ARBConfigOtherPoints const& rhs);
	ARBConfigOtherPoints& operator=(ARBConfigOtherPoints&& rhs);

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
	std::wstring GetGenericName() const override
	{
		return GetName();
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	size_t GetSearchStrings(std::set<std::wstring>& ioStrings) const override
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
		ARBCommon::ElementNodePtr const& inTree,
		ARBCommon::ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigOtherPoints element will be created in ioTree.
	 */
	bool Save(ARBCommon::ElementNodePtr const& ioTree) const;

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
	ARBOtherPointsTally GetTally() const
	{
		return m_Tally;
	}
	void SetTally(ARBOtherPointsTally inTally)
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
	ARBOtherPointsTally m_Tally;
	std::wstring m_Desc;
	double m_Default;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigOtherPoints objects.
 */
class ARB_API ARBConfigOtherPointsList : public ARBVector<ARBConfigOtherPointsPtr>
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
		ARBCommon::ElementNodePtr const& inTree,
		ARBCommon::ARBVersion const& inVersion,
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
	bool FindOtherPoints(std::wstring const& inName, ARBConfigOtherPointsPtr* outPoints = nullptr) const;

	/**
	 * Add an otherpoints object.
	 * @param inOther Name of OtherPoints to add.
	 * @return Whether the object was added.
	 */
	bool AddOtherPoints(ARBConfigOtherPointsPtr const& inOther);

	/**
	 * Delete an object.
	 * @param inName Name of object to delete.
	 * @return Whether the object was deleted or not.
	 */
	bool DeleteOtherPoints(std::wstring const& inName);
};

} // namespace ARB
} // namespace dconSoft
