#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBConfigTitle and ARBConfigTitleList class.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-08-05 DRC Added Roman numbers
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-10-14 DRC Added option to prefix a title.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-11 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-01-05 DRC Added LongName.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBDate.h"
#include "ARBTypes.h"


enum ARBTitleStyle
{
	eTitleNumber = 0,	/// "Title4"
	eTitleRoman = 1,	/// "Title IV"
};


class ARBTitleInstance
{
protected:
	virtual std::wstring TitleInstance(
			bool bShowInstanceOne,
			short instance,
			ARBTitleStyle style) const;
};


/**
 * Title configuration.
 * A title consists of a name (the abbreviation), a long name and a description.
 */
class ARBConfigTitle : public ARBBase, protected ARBTitleInstance
{
protected:
	ARBConfigTitle();
	ARBConfigTitle(ARBConfigTitle const& rhs);

public:
	~ARBConfigTitle();
	static ARBConfigTitlePtr New();
	ARBConfigTitlePtr Clone() const;

	ARBConfigTitle& operator=(ARBConfigTitle const& rhs);

	bool operator==(ARBConfigTitle const& rhs) const;
	bool operator!=(ARBConfigTitle const& rhs) const
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
		return GetNiceName();
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
	 * Load a title configuration.
	 * @pre inTree is the actual ARBConfigTitle element.
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
	 * @post The ARBConfigTitle element will be created in ioTree.
	 */
	bool Save(ElementNodePtr ioTree) const;

	/**
	 * Determine if this method is valid on the given date.
	 * @param inDate Date to check, if not valid, this method is valid.
	 */
	bool IsValidOn(ARBDate inDate) const
	{
		if (inDate.IsValid()
		&& ((m_ValidFrom.IsValid() && inDate < m_ValidFrom)
		|| (m_ValidTo.IsValid() && inDate > m_ValidTo)))
		{
			return false;
		}
		return true;
	}

	/**
	 * Get the nice (long) name.
	 * @return the nice (long) name.
	 */
	std::wstring const& GetNiceName() const
	{
		if (0 == m_LongName.length())
			return m_Name;
		else
			return m_LongName;
	}

	/**
	 * Get the complete name (name + nicename).
	 * @param inInstance Instance of the title to allow for multiple.
	 * @param bShowInstance Show '1' in title name for 1st title.
	 * @param bAbbrevFirst Name is before or after Longname.
	 * @param bAddDates Add the valid date ranges, if applicable.
	 * @return The complete name.
	 */
	std::wstring GetCompleteName(
			short inInstance = 0,
			bool bShowInstance = false,
			bool bAbbrevFirst = true,
			bool bAddDates = false) const;

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
	std::wstring const& GetLongName() const
	{
		return m_LongName;
	}
	void SetLongName(std::wstring const& inName)
	{
		m_LongName = inName;
	}
	short GetMultiple() const
	{
		return m_Multiple;
	}
	void SetMultiple(short inMultiple)
	{
		m_Multiple = inMultiple;
	}
	ARBTitleStyle GetMultipleStyle() const
	{
		return m_MultipleStyle;
	}
	void SetMultipleStyle(ARBTitleStyle inMultipleStyle)
	{
		m_MultipleStyle = inMultipleStyle;
	}
	bool GetPrefix() const
	{
		return m_Prefix;
	}
	void SetPrefix(bool inPrefix)
	{
		m_Prefix = inPrefix;
	}
	ARBDate const& GetValidFrom() const
	{
		return m_ValidFrom;
	}
	void SetValidFrom(ARBDate const& inDate)
	{
		m_ValidFrom = inDate;
	}
	ARBDate const& GetValidTo() const
	{
		return m_ValidTo;
	}
	void SetValidTo(ARBDate const& inDate)
	{
		m_ValidTo = inDate;
	}
	std::wstring const& GetDescription() const
	{
		return m_Desc;
	}
	void SetDescription(std::wstring const& inDesc)
	{
		m_Desc = inDesc;
	}

private:
	std::wstring m_Name;
	std::wstring m_LongName;
	short m_Multiple;
	ARBTitleStyle m_MultipleStyle;
	bool m_Prefix;
	ARBDate m_ValidFrom;
	ARBDate m_ValidTo;
	std::wstring m_Desc;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigTitle objects.
 */
class ARBConfigTitleList : public ARBVector<ARBConfigTitlePtr>
{
public:
	/**
	 * Load the information from XML (the tree).
	 * @pre inTree is the actual T element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @param inCheckDups Check for duplicate title names during upgrade.
	 * @return Success
	 */
	bool Load(
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback,
			bool inCheckDups = false);

	/**
	 * Reorder the list based on 'inList'
	 * @param inList List we want to match order on.
	 */
	void ReorderBy(ARBConfigTitleList const& inList);

	/**
	 * Find a title by the complete name.
	 * This api is used to fix a problem introduced in v1.0.0.8.
	 * @param inName Complete name of title to find.
	 * @param inInstance Instance of the title to allow for multiple.
	 * @param bShowInstance Show '1' in title name for 1st title.
	 * @param bAbbrevFirst Name is before or after Longname.
	 * @param outTitle Pointer to found object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindTitleCompleteName(
			std::wstring const& inName,
			short inInstance,
			bool bShowInstance,
			bool bAbbrevFirst = true,
			ARBConfigTitlePtr* outTitle = NULL) const;

	/**
	 * Find a title.
	 * @param inName Name of title to find.
	 * @param outTitle Pointer to found object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindTitle(
			std::wstring const& inName,
			ARBConfigTitlePtr* outTitle = NULL) const;

	/**
	 * Add a title.
	 * @param inName Name of title to add.
	 * @param outTitle Pointer to new object, NULL if name already exists or is empty.
	 * @return Whether the object was added.
	 */
	bool AddTitle(
			std::wstring const& inName,
			ARBConfigTitlePtr* outTitle = NULL);

	/**
	 * Add a title.
	 * @param inTitle Title to add.
	 * @return Whether the object was added.
	 */
	bool AddTitle(ARBConfigTitlePtr inTitle);

	/**
	 * Delete a title.
	 * @param inName Name of title to delete.
	 * @return Whether title was deleted.
	 */
	bool DeleteTitle(std::wstring const& inName);
};
