#pragma once

/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
 */

/**
 * @file
 * @brief CElement class.
 * @author David Connet
 *
 * Revision History
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-10-22 DRC Added a DTD parameter to SaveXML.
 */

#include <map>
#include <string>
#include <vector>
#include <xercesc/sax/InputSource.hpp>

class ARBDate;
class ARBVersion;

/**
 * Tree-like structure to hold XML data.
 *
 * Create the XML tree structure. This allows easy translation of an XML file
 * into something we can work with. Since we are using SAX to do the reading
 * of the XML, it's easier to read the data into a generic structure and
 * post-process that than it is to take the SAX events and directly create our
 * final data structure. By using SAX, we use alot less memory than the DOM.
 *
 * Reminder, attributes are not ordered and are unique by name.
 * Elements are ordered and the same name may be repeated.
 * (Hence the different access methods.)
 *
 * @attention An element may not have both elements and a value. While XML
 * allows this, we do not. This greatly simplifies data representation.
 */
class CElement
{
public:
	/**
	 * Result of getting an attribute.
	 */
	typedef enum
	{
		eNotFound,		///< Attribute was not found.
		eInvalidValue,	///< Attribute's value is not valid for data type.
		eFound			///< Attribute was found.
	} AttribLookup;

	CElement();
	CElement(const std::string& inName);
	CElement(const CElement& rhs);
	CElement& operator=(const CElement& rhs);
	~CElement();

	/**
	 * Dump the element tree for debugging purposes.
	 * @param inLevel Indent level.
	 */
	void Dump(int inLevel = 0) const;

	/**
	 * Clear the name, value, attributes, and subelements (everything!).
	 */
	void clear();

	/**
	 * Get the name of this element.
	 */
	const std::string& GetName() const;

	/**
	 * Set the name of this element.
	 * @param inName New name for this element.
	 */
	void SetName(const std::string& inName);

	/**
	 * Get the value of this element.
	 */
	const std::string& GetValue() const;

	/**
	 * Set the value of this element.
	 * @pre GetElementCount() must be 0.
	 * @param inValue New value for this element.
	 */
	void SetValue(const std::string& inValue);
	void SetValue(const char* const inValue);
	void SetValue(const short inValue);
	void SetValue(const long inValue);

	/**
	 * Set the value of this element.
	 * @pre GetElementCount() must be 0.
	 * @param inValue New value for this element.
	 * @param inPrec Precision, trailing zeros are trimmed unless prec=2, then they are only trimmed if all zero.
	 */
	void SetValue(const double inValue, int inPrec = 2);

	/**
	 * The number of attributes. This should only be used when iterating over
	 * all attributes. Use in conjunction with GetNthAttrib. For getting the
	 * value of a given attribute, GetAttrib is more efficient.
	 * @return Number of attributes.
	 */
	int GetAttribCount() const;

	/**
	 * Get the Nth attribute's name and value.
	 * @param inIndex Index of attribute to get.
	 * @param outName Name of the attribute.
	 * @param outValue Value of the attribute.
	 * @return Result of lookup.
	 */
	AttribLookup GetNthAttrib(int inIndex, std::string& outName, std::string& outValue) const;

	/**
	 * Get the value of an attribute.
	 * @param inName Name of attribute to get.
	 * @param outValue Value of attribute
	 * @return Result of lookup.
	 */
	AttribLookup GetAttrib(const std::string& inName, std::string& outValue) const;
	AttribLookup GetAttrib(const std::string& inName, ARBVersion& outValue) const;
	AttribLookup GetAttrib(const std::string& inName, ARBDate& outValue) const;
	AttribLookup GetAttrib(const std::string& inName, bool& outValue) const;
	AttribLookup GetAttrib(const std::string& inName, short& outValue) const;
	AttribLookup GetAttrib(const std::string& inName, long& outValue) const;
	AttribLookup GetAttrib(const std::string& inName, double& outValue) const;

	/**
	 * Add an attribute.
	 * @param inName Name of attribute to add.
	 * @param inValue Value of new attribute.
	 * @return This always returns true.
	 * @post If inName already exists, the previous value will be overwritten.
	 */
	bool AddAttrib(const std::string& inName, const std::string& inValue);
	bool AddAttrib(const std::string& inName, const char* const inValue);
	bool AddAttrib(const std::string& inName, const ARBVersion& inValue);
	bool AddAttrib(const std::string& inName, const ARBDate& inValue);
	bool AddAttrib(const std::string& inName, const bool inValue);
	bool AddAttrib(const std::string& inName, const short inValue);
	bool AddAttrib(const std::string& inName, const long inValue);

	/**
	 * Add an attribute.
	 * @param inName Name of attribute to add.
	 * @param inValue Value of new attribute.
	 * @param inPrec Precision, trailing zeros are trimmed unless prec=2, then they are only trimmed if all zero.
	 * @return This always returns true.
	 * @post If inName already exists, the previous value will be overwritten.
	 */
	bool AddAttrib(const std::string& inName, const double inValue, int inPrec = 2);

	/**
	 * Remove an attribute.
	 * @param inName Attribute to remove
	 * @return Whether or not the attribute was removed.
	 */
	bool RemoveAttrib(const std::string& inName);

	/**
	 * Remove all attributes.
	 */
	void RemoveAllAttribs();

	/**
	 * Get the number of elements this element contains.
	 * @return The number of elements.
	 */
	int GetElementCount() const;

	/**
	 * Get the specified element.
	 * @param inIndex Index of element to be obtained.
	 * @return The element.
	 */
	const CElement& GetElement(int inIndex) const;

	/**
	 * Get the specified element.
	 * @param inIndex Index of element to be obtained.
	 * @return The element.
	 */
	CElement& GetElement(int inIndex);

	/**
	 * Add an element.
	 * If inAt is less than zero or greater than the number of items,
	 * the item will be appended.
	 * @pre GetValue() must be empty.
	 * @param inName Name of new element.
	 * @param inAt Add the new element at the specified location.
	 * @return The new element.
	 */
	CElement& AddElement(const std::string& inName, int inAt = -1);

	/**
	 * Remove the specified element.
	 * @param inIndex Index of element to be removed.
	 * @return Whether or not element was removed.
	 */
	bool RemoveElement(int inIndex);

	/**
	 * Remove all the elements.
	 */
	void RemoveAllElements();

	/**
	 * Find the specified element.
	 * @param inName Name of the element to find.
	 * @param inStartFrom Start the search from this location.
	 * @return Index of the first element to match the search.
	 */
	int FindElement(const std::string& inName, int inStartFrom = 0) const;

	/**
	 * Populate this element from the given buffer.
	 * @param inData XML data to load.
	 * @param nData Length of inData buffer.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Whether file loaded successfully.
	 */
	bool LoadXMLBuffer(const char* inData, const unsigned int nData, std::string& ioErrMsg);

	/**
	 * Populate this element from the given file.
	 * @param inFileName XML file to load.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Whether file loaded successfully.
	 */
	bool LoadXMLFile(const char* inFileName, std::string& ioErrMsg);

	/**
	 * Save this element to the given output stream.
	 * @param outStream Stream to write tree to.
	 * @param inDTD DTD to include in generation of XML file.
	 * @retval true Tree successfully written.
	 * @retval false Tree failed to save.
	 */
	bool SaveXML(std::ostream& outStream, const std::string* inDTD = NULL) const;

private:
	bool LoadXML(const XERCES_CPP_NAMESPACE_QUALIFIER InputSource &inSource,
		std::string& ioErrMsg);
	typedef std::map<std::string, std::string> CAttributes;
	std::string m_Name;
	std::string m_Value;
	CAttributes m_Attribs;
	std::vector<CElement> m_Elements;
};
