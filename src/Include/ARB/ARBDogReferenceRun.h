#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBDogReferenceRun and ARBDogReferenceRunList classes.
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
#include "ARB_Q.h"
#include "LibwxARB.h"


namespace dconSoft
{
namespace ARB
{

/**
 * How did other dogs perform on the run we did?
 */
class ARB_API ARBDogReferenceRun : public ARBBase
{
protected:
	ARBDogReferenceRun();
	ARBDogReferenceRun(ARBDogReferenceRun const& rhs);
	ARBDogReferenceRun(ARBDogReferenceRun&& rhs);

public:
	~ARBDogReferenceRun();
	static ARBDogReferenceRunPtr New();
	ARBDogReferenceRunPtr Clone() const;

	ARBDogReferenceRun& operator=(ARBDogReferenceRun const& rhs);
	ARBDogReferenceRun& operator=(ARBDogReferenceRun&& rhs);

	bool operator==(ARBDogReferenceRun const& rhs) const;
	bool operator!=(ARBDogReferenceRun const& rhs) const
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
	 * Load an existing point.
	 * @pre inTree is the actual ARBDogReferenceRun element.
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
	 * @post The ARBDogReferenceRun element will be created in ioTree.
	 */
	bool Save(ARBCommon::ElementNodePtr const& ioTree) const;

	/**
	 * Getters/setters.
	 */
	ARB_Q GetQ() const
	{
		return m_Q;
	}
	void SetQ(ARB_Q inQ)
	{
		m_Q = inQ;
	}
	short GetPlace() const
	{
		return m_Place;
	}
	void SetPlace(short inPlace)
	{
		m_Place = inPlace;
	}
	wxString const& GetName() const
	{
		return m_Name;
	}
	void SetName(wxString const& inName)
	{
		m_Name = inName;
	}
	wxString const& GetHeight() const
	{
		return m_Height;
	}
	void SetHeight(wxString const& inHeight)
	{
		m_Height = inHeight;
	}
	wxString const& GetBreed() const
	{
		return m_Breed;
	}
	void SetBreed(wxString const& inBreed)
	{
		m_Breed = inBreed;
	}
	double GetTime() const
	{
		return m_Time;
	}
	void SetTime(double inTime)
	{
		m_Time = inTime;
	}
	wxString const& GetScore() const
	{
		return m_Score;
	}
	void SetScore(wxString const& inScore)
	{
		m_Score = inScore;
	}
	wxString const& GetNote() const
	{
		return m_Note;
	}
	void SetNote(wxString const& inNote)
	{
		m_Note = inNote;
	}

private:
	ARB_Q m_Q;
	short m_Place;
	wxString m_Name;
	wxString m_Height;
	wxString m_Breed;
	double m_Time;
	wxString m_Score;
	wxString m_Note;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogReferenceRun objects.
 */
class ARB_API ARBDogReferenceRunList : public ARBVector<ARBDogReferenceRunPtr>
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
	 * Add a reference run.
	 * @param inRef Object to add.
	 * @return Whether the object was added.
	 */
	bool AddReferenceRun(ARBDogReferenceRunPtr const& inRef);

	/**
	 * Delete a reference run.
	 * @param inRef Object to delete.
	 * @return Whether object was deleted.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteReferenceRun(ARBDogReferenceRunPtr const& inRef);
};

} // namespace ARB
} // namespace dconSoft
