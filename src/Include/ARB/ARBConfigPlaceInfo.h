#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBConfigPlaceInfo and ARBConfigPlaceInfoList class.
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-11-03 Created
 */

#include "ARBBase.h"
#include "ARBTypes2.h"
#include "LibwxARB.h"


namespace dconSoft
{
namespace ARB
{

/**
 * This class is used for 2 purposes:
 * - Speed point multiplier (AKC MACH)
 * - Points (Sweepstakes champion points)
 * - Placements (FCI, Level 2 placements)
 */
class ARB_API ARBConfigPlaceInfo : public ARBBase
{
protected:
	ARBConfigPlaceInfo();
	ARBConfigPlaceInfo(short inPlace, double inValue, bool bMustQ);
	ARBConfigPlaceInfo(ARBConfigPlaceInfo const& rhs);
	ARBConfigPlaceInfo(ARBConfigPlaceInfo&& rhs);

public:
	~ARBConfigPlaceInfo();
	static ARBConfigPlaceInfoPtr New();
	static ARBConfigPlaceInfoPtr New(short inPlace, double inValue, bool bMustQ);
	ARBConfigPlaceInfoPtr Clone() const;

	ARBConfigPlaceInfo& operator=(ARBConfigPlaceInfo const& rhs);
	ARBConfigPlaceInfo& operator=(ARBConfigPlaceInfo&& rhs);

	bool operator==(ARBConfigPlaceInfo const& rhs) const;
	bool operator!=(ARBConfigPlaceInfo const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	wxString GetGenericName() const override;

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	size_t GetSearchStrings(std::set<wxString>& ioStrings) const override
	{
		return 0;
	}

	/**
	 * Load a point info configuration.
	 * @pre inTree is the actual ARBConfigPlaceInfo element.
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
	 * @post The ARBConfigPlaceInfo element will be created in ioTree.
	 */
	bool Save(ARBCommon::ElementNodePtr const& ioTree) const;

	/*
	 * Setters/getters.
	 */
	short GetPlace() const
	{
		return m_Place;
	}
	// There is no SetPlace since this needs to be a unique key in the list.
	double GetValue() const
	{
		return m_Value;
	}
	void SetValue(double inValue)
	{
		m_Value = inValue;
	}
	bool MustQ() const
	{
		return m_MustQ;
	}
	void SetMustQ(bool inValue)
	{
		m_MustQ = inValue;
	}

private:
	short m_Place;
	double m_Value;
	bool m_MustQ;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigPlaceInfo objects.
 */
class ARB_API ARBConfigPlaceInfoList : public ARBVector<ARBConfigPlaceInfoPtr>
{
public:
	/**
	 * Load a place info configuration.
	 * @pre inTree is the actual ARBConfigPlaceInfo element.
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
	 * Sort the point objects by place.
	 */
	void sort();

	/**
	 * Get the value based on place.
	 * @param inPlace Placement.
	 * @param outValue Value of placement.
	 * @return inPlace has a value.`
	 */
	bool GetPlaceInfo(short inPlace, double& outValue) const;

	/**
	 * Find a points object.
	 * @param inPlace Placement to find.
	 * @param outPlace Pointer to found object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindPlaceInfo(short inPlace, ARBConfigPlaceInfoPtr* outPlace = nullptr) const;

	/**
	 * Add an object.
	 * @param inPlace Placement.
	 * @param inValue Value.
	 * @param inMustQ Whether the run must Q in order to earn placement points.
	 * @param outPlace Pointer to new object, NULL if it already exists.
	 * @return Whether the object was added.
	 */
	bool AddPlaceInfo(short inPlace, double inValue, bool inMustQ, ARBConfigPlaceInfoPtr* outPlace = nullptr);

	/**
	 * Delete an object.
	 * @param inPlace Delete object with the given placement.
	 * @return Whether object was deleted.
	 */
	bool DeletePlaceInfo(short inPlace);
};

} // namespace ARB
} // namespace dconSoft
