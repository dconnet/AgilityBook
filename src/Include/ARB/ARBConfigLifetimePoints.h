#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBConfigLifetimePoints and ARBConfigLifetimePointsList class.
 * @author David Connet
 *
 * Revision History
 * 2016-06-14 Add support for speed point based lifetime points.
 * 2016-01-06 Add support for named lifetime points.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-10-06 Added as part of file version 10.
 */

#include "ARBBase.h"
#include "ARBTypes2.h"
#include "LibwxARB.h"


namespace dconSoft
{
namespace ARB
{

/**
 * Number of lifetime points that can be earned based on number of faults.
 * Some venues have a sliding scale, for instance a clean run in NADAC
 * is worth 10 pts, a run with up to 5 faults earns 5 pts.
 *
 * This class works very much like ARBConfigTitlePoints. (In fact, lifetime
 * points used to be titling points until it was discovered that some venues
 * award different values [CPE].)
 */
class ARB_API ARBConfigLifetimePoints : public ARBBase
{
protected:
	ARBConfigLifetimePoints();
	ARBConfigLifetimePoints(wxString const& name, double inPoints, double inFaults);
	ARBConfigLifetimePoints(wxString const& name, double inFaults);
	ARBConfigLifetimePoints(ARBConfigLifetimePoints const& rhs);
	ARBConfigLifetimePoints(ARBConfigLifetimePoints&& rhs);

public:
	~ARBConfigLifetimePoints();
	static ARBConfigLifetimePointsPtr New();
	static ARBConfigLifetimePointsPtr New(wxString const& name, double inPoints, double inFaults);
	static ARBConfigLifetimePointsPtr New(wxString const& name, double inFaults);
	ARBConfigLifetimePointsPtr Clone() const;

	ARBConfigLifetimePoints& operator=(ARBConfigLifetimePoints const& rhs);
	ARBConfigLifetimePoints& operator=(ARBConfigLifetimePoints&& rhs);

	bool operator==(ARBConfigLifetimePoints const& rhs) const;
	bool operator!=(ARBConfigLifetimePoints const& rhs) const
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
	 * Load a lifetime configuration.
	 * @pre inTree is the actual ARBConfigLifetimePoints element.
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
	 * @post The ARBConfigLifetimePoints element will be created in ioTree.
	 */
	bool Save(ARBCommon::ElementNodePtr const& ioTree) const;

	/*
	 * Setters/getters.
	 */
	wxString GetName() const
	{
		return m_Name;
	}
	void SetName(wxString const& inName)
	{
		m_Name = inName;
	}
	bool UseSpeedPts() const
	{
		return m_UseSpeedPts;
	}
	void UseSpeedPts(bool inUseSpeedPts)
	{
		m_UseSpeedPts = inUseSpeedPts;
	}
	double GetPoints() const
	{
		return m_Points;
	}
	void SetPoints(double inPoints)
	{
		m_Points = inPoints;
	}
	double GetFaults() const
	{
		return m_Faults;
	}
	// There is no SetName/Faults since this needs to be a unique key in the list.

	// Fixup routine called from ARBConfigVenue only.
	void FixName(wxString const& inName)
	{
		m_Name = inName;
	}

private:
	wxString m_Name;
	bool m_UseSpeedPts;
	double m_Points;
	double m_Faults;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigLifetimePoints objects.
 */
class ARB_API ARBConfigLifetimePointsList : public ARBVector<ARBConfigLifetimePointsPtr>
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
	 * Sort the lifetime point objects by faults.
	 */
	void sort();

	/**
	 * Get the number of lifetime title points earned based on faults.
	 * @param inName Name of lifetime points.
	 * @param inFaults Number of faults in the run.
	 * @param inSpeedPts Number of speed points, if applicable.
	 * @return Number of lifetime titling points.
	 */
	double GetLifetimePoints(wxString const& inName, double inFaults, short inSpeedPts) const;

	/**
	 * Find a points object.
	 * @param inName Name of faults to find.
	 * @param inFaults Number of faults to find.
	 * @param outPoints Pointer to found object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindLifetimePoints(wxString const& inName, double inFaults, ARBConfigLifetimePointsPtr* outPoints = nullptr)
		const;

	/**
	 * Add an object.
	 * @param inName Name of faults to find.
	 * @param inUseSpeedPts If true, ignore inPoints
	 * @param inPoints Number of lifetime points.
	 * @param inFaults Number of faults.
	 * @param outPoints Pointer to new object, NULL if it already exists.
	 * @return Whether the object was added.
	 */
	bool AddLifetimePoints(
		wxString const& inName,
		bool inUseSpeedPts,
		double inPoints,
		double inFaults,
		ARBConfigLifetimePointsPtr* outPoints = nullptr);

	/**
	 * Delete a lifetime name object.
	 * @param inName Name of points to find.
	 * @param inFaults Delete object with the given faults.
	 * @return Whether object was deleted.
	 */
	bool DeleteLifetimePoints(wxString const& inName, double inFaults);
};

} // namespace ARB
} // namespace dconSoft
