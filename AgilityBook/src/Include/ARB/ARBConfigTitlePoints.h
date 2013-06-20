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
 * @li 2011-07-31 DRC Add support for different title point accumulation.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-03-31 DRC Started adding auto-lifetime point accumulation.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-07-12 DRC Added as part of file version 5.
 */

#include "ARB/ARBBase.h"
#include "ARB/ARBCalcPoints.h"
#include "ARB/ARBTypes2.h"


/**
 * Number of title points that can be earned.
 */
class ARBConfigTitlePoints : public ARBBase
{
protected:
	ARBConfigTitlePoints();
	ARBConfigTitlePoints(
			double inPoints,
			double inFaults,
			ARBPointsType inType);
	ARBConfigTitlePoints(ARBConfigTitlePoints const& rhs);

public:
	~ARBConfigTitlePoints();
	static ARBConfigTitlePointsPtr New();
	static ARBConfigTitlePointsPtr New(
			double inPoints,
			double inFaults,
			ARBPointsType inType);
	ARBConfigTitlePointsPtr Clone() const;

	ARBConfigTitlePoints& operator=(ARBConfigTitlePoints const& rhs);

	bool operator==(ARBConfigTitlePoints const& rhs) const;
	bool operator!=(ARBConfigTitlePoints const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual std::wstring GetGenericName() const;

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
	 * Load a title configuration.
	 * @pre inTree is the actual ARBConfigTitlePoints element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @param ioLifetimePoints Used for migrating older files.
	 * @return Success
	 */
	bool Load(
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback,
			ARBConfigLifetimePointsList& ioLifetimePoints);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigTitlePoints element will be created in ioTree.
	 */
	bool Save(ElementNodePtr ioTree) const;

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
class ARBConfigTitlePointsList : public ARBVector<ARBConfigTitlePointsPtr>
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
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
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
	 * @return Number of titling points.
	 */
	double GetTitlePoints(
			double inFaults,
			double inTime,
			double inSCT,
			short inPlace,
			short inClass) const;

	/**
	 * Find a title points object.
	 * @param inFaults Number of faults to find.
	 * @param outPoints Pointer to found object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindTitlePoints(
			double inFaults,
			ARBConfigTitlePointsPtr* outPoints = NULL) const;

	/**
	 * Add an object.
	 * @param inPoints Number of title points.
	 * @param inFaults Number of faults.
	 * @param outPoints Pointer to new object, NULL if it already exists.
	 * @return Whether the object was added.
	 */
	bool AddTitlePoints(
			double inPoints,
			double inFaults,
			ARBConfigTitlePointsPtr* outPoints = NULL);

	/**
	 * Delete an object.
	 * @param inType Delete object with the given type.
	 * @param inFaults Delete object with the given faults.
	 * @return Whether object was deleted.
	 */
	bool DeleteTitlePoints(
			ARBPointsType inType,
			double inFaults);
};
