#pragma once

/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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
 * @brief Element class.
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
class Element
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

	Element();
	Element(std::string const& inName);
	Element(Element const& rhs);
	Element& operator=(Element const& rhs);
	~Element();

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
	std::string const& GetName() const;

	/**
	 * Set the name of this element.
	 * @param inName New name for this element.
	 */
	void SetName(std::string const& inName);

	/**
	 * Get the value of this element.
	 */
	std::string const& GetValue() const;

	/**
	 * Set the value of this element.
	 * @pre GetElementCount() must be 0.
	 * @param inValue New value for this element.
	 */
	void SetValue(std::string const& inValue);
	void SetValue(char const* const inValue);
	void SetValue(short inValue);
	void SetValue(long inValue);

	/**
	 * Set the value of this element.
	 * @pre GetElementCount() must be 0.
	 * @param inValue New value for this element.
	 * @param inPrec Precision, trailing zeros are trimmed unless prec=2, then they are only trimmed if all zero.
	 */
	void SetValue(double inValue, int inPrec = 2);

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
	AttribLookup GetAttrib(std::string const& inName, std::string& outValue) const;
	AttribLookup GetAttrib(std::string const& inName, ARBVersion& outValue) const;
	AttribLookup GetAttrib(std::string const& inName, ARBDate& outValue) const;
	AttribLookup GetAttrib(std::string const& inName, bool& outValue) const;
	AttribLookup GetAttrib(std::string const& inName, short& outValue) const;
	AttribLookup GetAttrib(std::string const& inName, long& outValue) const;
	AttribLookup GetAttrib(std::string const& inName, double& outValue) const;

	/**
	 * Add an attribute.
	 * @param inName Name of attribute to add.
	 * @param inValue Value of new attribute.
	 * @return This always returns true.
	 * @post If inName already exists, the previous value will be overwritten.
	 */
	bool AddAttrib(std::string const& inName, std::string const& inValue);
	bool AddAttrib(std::string const& inName, char const* const inValue);
	bool AddAttrib(std::string const& inName, ARBVersion const& inValue);
	bool AddAttrib(std::string const& inName, ARBDate const& inValue);
	bool AddAttrib(std::string const& inName, bool inValue);
	bool AddAttrib(std::string const& inName, short inValue);
	bool AddAttrib(std::string const& inName, long inValue);

	/**
	 * Add an attribute.
	 * @param inName Name of attribute to add.
	 * @param inValue Value of new attribute.
	 * @param inPrec Precision, trailing zeros are trimmed unless prec=2, then they are only trimmed if all zero.
	 * @return This always returns true.
	 * @post If inName already exists, the previous value will be overwritten.
	 */
	bool AddAttrib(std::string const& inName, double inValue, int inPrec = 2);

	/**
	 * Remove an attribute.
	 * @param inName Attribute to remove
	 * @return Whether or not the attribute was removed.
	 */
	bool RemoveAttrib(std::string const& inName);

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
	Element const& GetElement(int inIndex) const;

	/**
	 * Get the specified element.
	 * @param inIndex Index of element to be obtained.
	 * @return The element.
	 */
	Element& GetElement(int inIndex);

	/**
	 * Add an element.
	 * If inAt is less than zero or greater than the number of items,
	 * the item will be appended.
	 * @pre GetValue() must be empty.
	 * @param inName Name of new element.
	 * @param inAt Add the new element at the specified location.
	 * @return The new element.
	 */
	Element& AddElement(std::string const& inName, int inAt = -1);

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
	int FindElement(std::string const& inName, int inStartFrom = 0) const;

	/**
	 * Populate this element from the given buffer.
	 * @param inData XML data to load.
	 * @param nData Length of inData buffer.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Whether file loaded successfully.
	 */
	bool LoadXMLBuffer(char const* inData, unsigned int nData, std::string& ioErrMsg);

	/**
	 * Populate this element from the given file.
	 * @param inFileName XML file to load.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Whether file loaded successfully.
	 */
	bool LoadXMLFile(char const* inFileName, std::string& ioErrMsg);

	/**
	 * Save this element to the given output stream.
	 * @param outStream Stream to write tree to.
	 * @param inDTD DTD to include in generation of XML file.
	 * @retval true Tree successfully written.
	 * @retval false Tree failed to save.
	 */
	bool SaveXML(std::ostream& outStream, std::string const* inDTD = NULL) const;

private:
	bool LoadXML(XERCES_CPP_NAMESPACE_QUALIFIER InputSource const& inSource,
		std::string& ioErrMsg);
	typedef std::map<std::string, std::string> MyAttributes;
	std::string m_Name;
	std::string m_Value;
	MyAttributes m_Attribs;
	std::vector<Element> m_Elements;
};
