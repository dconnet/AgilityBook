#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBConfigTitlePoints and ARBConfigTitlePointsList class.
 * @author David Connet
 *
 * Revision History
 * 2011-07-31 Add support for different title point accumulation.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-03-31 Started adding auto-lifetime point accumulation.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-07-12 Added as part of file version 5.
 */

#include "ARBBase.h"
#include "ARBCalcPoints.h"
#include "ARBTypes2.h"
#include "LibwxARB.h"


namespace dconSoft
{
namespace ARB
{

/**
 * Number of title points that can be earned.
 */
class ARB_API ARBConfigTitlePoints : public ARBBase
{
protected:
	ARBConfigTitlePoints();
	ARBConfigTitlePoints(double inPoints, double inFaults, ARBPointsType inType);
	ARBConfigTitlePoints(ARBConfigTitlePoints const& rhs);
	ARBConfigTitlePoints(ARBConfigTitlePoints&& rhs);

public:
	~ARBConfigTitlePoints();
	static ARBConfigTitlePointsPtr New();
	static ARBConfigTitlePointsPtr New(double inPoints, double inFaults, ARBPointsType inType);
	ARBConfigTitlePointsPtr Clone() const;

	ARBConfigTitlePoints& operator=(ARBConfigTitlePoints const& rhs);
	ARBConfigTitlePoints& operator=(ARBConfigTitlePoints&& rhs);

	bool operator==(ARBConfigTitlePoints const& rhs) const;
	bool operator!=(ARBConfigTitlePoints const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	std::wstring GetGenericName() const override;

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
	 * Load a title configuration.
	 * @pre inTree is the actual ARBConfigTitlePoints element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @param ioLifetimePoints Used for migrating older files.
	 * @return Success
	 */
	bool Load(
		ARBCommon::ElementNodePtr const& inTree,
		ARBCommon::ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback,
		ARBConfigLifetimePointsList& ioLifetimePoints);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigTitlePoints element will be created in ioTree.
	 */
	bool Save(ARBCommon::ElementNodePtr const& ioTree) const;

	/*
	 * Setters/getters.
	 */
	double GetPoints() const
	{
		return m_Points;
	}
	bool SetPoints(double inPoints);
	double GetFaults() const
	{
		return m_Faults;
	}
	// There is no SetFaults since this needs to be a unique key in the list.
	ARBCalcPointsPtr GetCalc() const
	{
		return m_Calc;
	}

private:
	double m_Points;
	double m_Faults;
	ARBCalcPointsPtr m_Calc;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigTitlePoints objects.
 */
class ARB_API ARBConfigTitlePointsList : public ARBVector<ARBConfigTitlePointsPtr>
{
public:
	/**
	 * Load a title configuration.
	 * @pre inTree is the actual ARBConfigTitlePoints element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @param ioLifetimePoints Used for migrating older files.
	 * @return Success
	 */
	bool Load(
		ARBCommon::ElementNodePtr const& inTree,
		ARBCommon::ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback,
		ARBConfigLifetimePointsList& ioLifetimePoints);

	/**
	 * Sort the title point objects by faults.
	 */
	void sort();

	/**
	 * Get the type of points in the list.
	 */
	ARBCalcPointsPtr GetCalc() const;

	/**
	 * Get the type of points in the list.
	 */
	ARBPointsType GetType() const;

	/**
	 * Set the type of points in the list.
	 * Changing from Faults will clear the list.
	 * @param inType Set the type of title points.
	 */
	void SetType(ARBPointsType inType);

	/**
	 * Get the number of title points earned.
	 * @param inFaults Number of faults in the run.
	 * @param inTime Time of the run.
	 * @param inSCT SCT of run.
	 * @param inPlace Place of dog.
	 * @param inClass Number of dogs in the event.
	 * @param inDate Date of run.
	 * @param isTourney Is this a USDAA tournament event?
	 * @return Number of titling points.
	 */
	double GetTitlePoints(
		double inFaults,
		double inTime,
		double inSCT,
		short inPlace,
		short inClass,
		ARBCommon::ARBDate inDate,
		bool isTourney) const;

	/**
	 * Find a title points object.
	 * @param inFaults Number of faults to find.
	 * @param outPoints Pointer to found object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindTitlePoints(double inFaults, ARBConfigTitlePointsPtr* outPoints = nullptr) const;

	/**
	 * Add an object.
	 * @param inPoints Number of title points.
	 * @param inFaults Number of faults.
	 * @param outPoints Pointer to new object, NULL if it already exists.
	 * @return Whether the object was added.
	 */
	bool AddTitlePoints(double inPoints, double inFaults, ARBConfigTitlePointsPtr* outPoints = nullptr);

	/**
	 * Delete an object.
	 * @param inType Delete object with the given type.
	 * @param inFaults Delete object with the given faults.
	 * @return Whether object was deleted.
	 */
	bool DeleteTitlePoints(ARBPointsType inType, double inFaults);
};

} // namespace ARB
} // namespace dconSoft
