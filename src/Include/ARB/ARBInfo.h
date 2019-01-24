#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBInfo class
 * @author David Connet
 *
 * Revision History
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-11-18 Added Club/Location info.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2003-12-07 Created
 */

#include "ARB/ARBInfoItem.h"
#include "ARB/ARBTypes2.h"


/**
 * General information to be tracked.
 *
 * This class is contained in ARBAgilityRecordBook.
 */
class ARBInfo
{
public:
	typedef enum
	{
		eClubInfo,
		eJudgeInfo,
		eLocationInfo
	} eInfoType;

	ARBInfo();
	~ARBInfo();
	ARBInfo(ARBInfo const& rhs);
	ARBInfo(ARBInfo&& rhs);
	ARBInfo& operator=(ARBInfo const& rhs);
	ARBInfo& operator=(ARBInfo&& rhs);

	bool operator==(ARBInfo const& rhs) const;
	bool operator!=(ARBInfo const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Reset the contents of this object and all sub-objects.
	 */
	void clear();

	/**
	 * Load the info.
	 * @pre inTree is the actual ARBInfo element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ElementNodePtr const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBInfo element will be created in ioTree.
	 */
	bool Save(ElementNodePtr const& ioTree) const;

	/*
	 * Getters.
	 */
	ARBInfoItemList const& GetInfo(ARBInfo::eInfoType inType) const
	{
		switch (inType)
		{
		default:
		case ARBInfo::eClubInfo:
			return m_ClubInfo;
		case ARBInfo::eJudgeInfo:
			return m_JudgeInfo;
		case ARBInfo::eLocationInfo:
			return m_LocationInfo;
		}
	}
	ARBInfoItemList& GetInfo(ARBInfo::eInfoType inType)
	{
		switch (inType)
		{
		default:
		case ARBInfo::eClubInfo:
			return m_ClubInfo;
		case ARBInfo::eJudgeInfo:
			return m_JudgeInfo;
		case ARBInfo::eLocationInfo:
			return m_LocationInfo;
		}
	}

private:
	ARBInfoItemList m_ClubInfo;
	ARBInfoItemList m_JudgeInfo;
	ARBInfoItemList m_LocationInfo;
};
