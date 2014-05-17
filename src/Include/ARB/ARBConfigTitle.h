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
 * 2013-01-13 Added new recurring title suffix style.
 * 2012-07-30 Added new recurring title style (none).
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-08-05 Added Roman numbers
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-10-14 Added option to prefix a title.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2005-01-11 Allow titles to be optionally entered multiple times.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-01-05 Added LongName.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 */

#include "ARB/ARBBase.h"
#include "ARB/ARBTypes2.h"
#include "ARBCommon/ARBDate.h"
#include "ARBCommon/Element.h"


enum ARBTitleStyle
{
	eTitleStyleNone,	/// "Title"
	eTitleStyleNumber,	/// "Title4"
	eTitleStyleRoman,	/// "Title-IV"
};

ElementNode::AttribLookup LoadTitleStyle(
		ElementNodePtr inTree,
		wchar_t const* inAttrib,
		ARBVersion const& inVersion,
		ARBTitleStyle& ioStyle);
void SaveTitleStyle(
		ElementNodePtr ioTree,
		wchar_t const* inAttrib,
		ARBTitleStyle inStyle);


enum ARBTitleSeparator
{
	eTitleSeparatorNone,	/// "Title4"
	eTitleSeparatorSpace,	/// "Title 4"
	eTitleSeparatorHyphen,	/// "Title-4"
};

ElementNode::AttribLookup LoadTitleSeparator(
		ElementNodePtr inTree,
		wchar_t const* inAttrib,
		ARBVersion const& inVersion,
		ARBTitleStyle inStyle,
		ARBTitleSeparator& ioSep);
void SaveTitleSeparator(
		ElementNodePtr ioTree,
		wchar_t const* inAttrib,
		ARBTitleSeparator inSep);


class ARBTitleInstance
{
protected:
	virtual std::wstring TitleInstance(
			bool bShowInstanceOne,
			short instance,
			short startAt,
			short increment,
			ARBTitleStyle style,
			ARBTitleSeparator sep) const;
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
	 * Get the title name (name + instance).
	 * @param inInstance Instance of the title to allow for multiple.
	 * @return The title name.
	 */
	std::wstring GetTitleName(short inInstance = 0) const;

	/**
	 * Get the complete name (name + nicename).
	 * @param inInstance Instance of the title to allow for multiple.
	 * @param bAbbrevFirst Name is before or after Longname.
	 * @param bAddDates Add the valid date ranges, if applicable.
	 * @return The complete name.
	 */
	std::wstring GetCompleteName(
			short inInstance = 0,
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
	std::wstring const& GetDescription() const
	{
		return m_Desc;
	}
	void SetDescription(std::wstring const& inDesc)
	{
		m_Desc = inDesc;
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
	// Recurring properties
	bool IsRecurring() const
	{
		return 0 != m_MultipleStartAt;
	}
	short GetMultipleStartAt() const
	{
		return m_MultipleStartAt;
	}
	void SetMultipleStartAt(short startAt)
	{
		m_MultipleStartAt = startAt;
	}
	unsigned short GetMultipleIncrement() const
	{
		return m_MultipleIncrement;
	}
	void SetMultipleIncrement(unsigned short inc)
	{
		m_MultipleIncrement = inc;
	}
	bool ShowMultipleOnFirstInstance() const
	{
		return m_MultipleOnFirst;
	}
	void SetMultipleOnFirstInstance(bool onFirst)
	{
		m_MultipleOnFirst = onFirst;
	}
	ARBTitleStyle GetMultipleStyle() const
	{
		return m_MultipleStyle;
	}
	ARBTitleSeparator GetMultipleStyleSeparator() const
	{
		return m_MultipleSeparator;
	}
	void SetMultipleStyle(
			ARBTitleStyle inMultipleStyle,
			ARBTitleSeparator inMultipleSeparator)
	{
		m_MultipleStyle = inMultipleStyle;
		m_MultipleSeparator = inMultipleSeparator;
	}

private:
	std::wstring m_Name;
	std::wstring m_LongName;
	std::wstring m_Desc;
	bool m_Prefix;
	ARBDate m_ValidFrom;
	ARBDate m_ValidTo;
	// Recurring properties
	short m_MultipleStartAt;
	unsigned short m_MultipleIncrement;
	bool m_MultipleOnFirst;
	ARBTitleStyle m_MultipleStyle;
	ARBTitleSeparator m_MultipleSeparator;
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
	 * @param bAbbrevFirst Name is before or after Longname.
	 * @param outTitle Pointer to found object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindTitleCompleteName(
			std::wstring const& inName,
			short inInstance,
			bool bAbbrevFirst = true,
			ARBConfigTitlePtr* outTitle = nullptr) const;

	/**
	 * Find a title.
	 * @param inName Name of title to find.
	 * @param outTitle Pointer to found object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindTitle(
			std::wstring const& inName,
			ARBConfigTitlePtr* outTitle = nullptr) const;

	/**
	 * Add a title.
	 * @param inName Name of title to add.
	 * @param outTitle Pointer to new object, NULL if name already exists or is empty.
	 * @return Whether the object was added.
	 */
	bool AddTitle(
			std::wstring const& inName,
			ARBConfigTitlePtr* outTitle = nullptr);

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
