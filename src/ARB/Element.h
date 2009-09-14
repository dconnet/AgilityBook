#pragma once

/*
 * Copyright (c) 2002-2009 David Connet. All Rights Reserved.
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
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-03-12 DRC Converting all TCHAR stuff to wxWidgets
 * @li 2007-09-06 DRC Added GetNthElementNode
 * @li 2007-08-15 DRC Modified to support mixed text/nodes.
 * @li 2007-08-08 DRC Moved initialization here, so all XML usage is contained.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-10-22 DRC Added a DTD parameter to SaveXML.
 */

#include <map>
#include <vector>
#include "ARBTypes.h"
class ARBDate;

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
 */
class Element
{
protected:
	Element();

public:
	/**
	 * Call once (at program startup) to initialize XML subsystem.
	 * @param outMsg In case of failure, error message.
	 * @return Initialization success.
	 */
	static bool Initialize(wxString& outMsg);

	/**
	 * Call once (at program termination) to cleanup XML subsystem.
	 */
	static void Terminate();

	virtual ~Element() = 0;

	/**
	 * Dump the element tree for debugging purposes.
	 * @param inLevel Indent level.
	 */
	virtual void Dump(int inLevel = 0) const = 0;

	typedef enum
	{
		Element_Node,
		Element_Text
	} ElementType;
	virtual ElementType GetType() const = 0;

	/**
	 * Get the name of this element.
	 */
	virtual wxString const& GetName() const = 0;

	/**
	 * Set the name of this element.
	 * @param inName New name for this element.
	 */
	virtual void SetName(wxString const& inName) = 0;

	/**
	 * Get the value of this element. This will concatenate all text nodes.
	 */
	virtual wxString GetValue() const = 0;

	/**
	 * Set the value of this element. If this element has text nodes, they
	 * will all be consolidated into one.
	 * @pre GetElementCount() must be 0.
	 * @param inValue New value for this element.
	 */
	virtual void SetValue(wxString const& inValue) = 0;
	virtual void SetValue(wxChar const* const inValue) = 0;
	virtual void SetValue(short inValue) = 0;
	virtual void SetValue(long inValue) = 0;

	/**
	 * Set the value of this element.
	 * @pre GetElementCount() must be 0.
	 * @param inValue New value for this element.
	 * @param inPrec Precision, trailing zeros are trimmed unless prec=2, then they are only trimmed if all zero.
	 */
	virtual void SetValue(
			double inValue,
			int inPrec = 2) = 0;
};

class ElementNode : public Element
{
private:
	ElementNode();
	ElementNode(wxString const& inName);
	ElementNode(ElementNode const&); // Not implemented
	ElementNode& operator=(ElementNode const&); // Not implemented

public:
	static ElementNodePtr New();
	static ElementNodePtr New(wxString const& inName);

	virtual void Dump(int inLevel = 0) const;
	virtual Element::ElementType GetType() const;
	virtual wxString const& GetName() const;
	virtual void SetName(wxString const& inName);
	virtual wxString GetValue() const;
	virtual void SetValue(wxString const& inValue);
	virtual void SetValue(wxChar const* const inValue);
	virtual void SetValue(short inValue);
	virtual void SetValue(long inValue);
	virtual void SetValue(double inValue, int inPrec = 2);

	/**
	 * Clear the name, value, attributes, and subelements (everything!).
	 */
	void clear();

	/**
	 * Result of getting an attribute.
	 */
	typedef enum
	{
		eNotFound,		///< Attribute was not found.
		eInvalidValue,	///< Attribute's value is not valid for data type.
		eFound			///< Attribute was found.
	} AttribLookup;

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
	AttribLookup GetNthAttrib(
			int inIndex,
			wxString& outName,
			wxString& outValue) const;

	/**
	 * Get the value of an attribute.
	 * @param inName Name of attribute to get.
	 * @param outValue Value of attribute
	 * @return Result of lookup.
	 */
	AttribLookup GetAttrib(
			wxString const& inName,
			wxString& outValue) const;
	AttribLookup GetAttrib(
			wxString const& inName,
			ARBVersion& outValue) const;
	AttribLookup GetAttrib(
			wxString const& inName,
			ARBDate& outValue) const;
	AttribLookup GetAttrib(
			wxString const& inName,
			bool& outValue) const;
	AttribLookup GetAttrib(
			wxString const& inName,
			short& outValue) const;
	AttribLookup GetAttrib(
			wxString const& inName,
			long& outValue) const;
	AttribLookup GetAttrib(
			wxString const& inName,
			double& outValue) const;

	/**
	 * Add an attribute.
	 * @param inName Name of attribute to add.
	 * @param inValue Value of new attribute.
	 * @return This always returns true.
	 * @post If inName already exists, the previous value will be overwritten.
	 */
	bool AddAttrib(
			wxString const& inName,
			wxString const& inValue);
	bool AddAttrib(
			wxString const& inName,
			wxChar const* const inValue);
	bool AddAttrib(
			wxString const& inName,
			ARBVersion const& inValue);
	bool AddAttrib(
			wxString const& inName,
			ARBDate const& inValue);
	bool AddAttrib(
			wxString const& inName,
			bool inValue);
	bool AddAttrib(
			wxString const& inName,
			short inValue);
	bool AddAttrib(
			wxString const& inName,
			long inValue);

	/**
	 * Add an attribute.
	 * @param inName Name of attribute to add.
	 * @param inValue Value of new attribute.
	 * @param inPrec Precision, trailing zeros are trimmed unless prec=2, then they are only trimmed if all zero.
	 * @return This always returns true.
	 * @post If inName already exists, the previous value will be overwritten.
	 */
	bool AddAttrib(
			wxString const& inName,
			double inValue,
			int inPrec = 2);

	/**
	 * Remove an attribute.
	 * @param inName Attribute to remove
	 * @return Whether or not the attribute was removed.
	 */
	bool RemoveAttrib(wxString const& inName);

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
	 * Get the number of typed elements.
	 * @param type Type to count.
	 */
	int GetNodeCount(ElementType type) const;

	/**
	 * Does this node have any text? (a value)
	 */
	bool HasTextNodes() const;

	/**
	 * Get the specified element.
	 * @param inIndex Index of element to be obtained.
	 * @return The element.
	 */
	ElementPtr GetElement(int inIndex) const;

	/**
	 * Get the specified element.
	 * @param inIndex Index of element to be obtained.
	 * @return The element.
	 */
	ElementPtr GetElement(int inIndex);

	ElementNodePtr GetElementNode(int inIndex) const;
	ElementNodePtr GetElementNode(int inIndex);

	/**
	 * Get the specified element node. This will not return the Nth item,
	 * rather it will get the Nth node, skipping text nodes.
	 */
	ElementNodePtr GetNthElementNode(int inIndex) const;
	ElementNodePtr GetNthElementNode(int inIndex);

	/**
	 * Add an element.
	 * If inAt is less than zero or greater than the number of items,
	 * the item will be appended.
	 * @pre GetValue() must be empty.
	 * @param inName Name of new element.
	 * @param inAt Add the new element at the specified location.
	 * @return The new element.
	 */
	ElementNodePtr AddElementNode(
			wxString const& inName,
			int inAt = -1);

	/**
	 * Add an element.
	 * If inAt is less than zero or greater than the number of items,
	 * the item will be appended.
	 * @pre GetValue() must be empty.
	 * @param inText Text of new text element.
	 * @param inAt Add the new element at the specified location.
	 * @return The new element.
	 */
	ElementTextPtr AddElementText(
			wxString const& inText,
			int inAt = -1);

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
	int FindElement(
			wxString const& inName,
			int inStartFrom = 0) const;

	/**
	 * Search for the specified element, depth.
	 * @param outParentNode Pointer to parent of found node
	 * @param outElementIndex Index of found element.
	 * @param inName Name of the element to find.
	 * @param inValue Optionally search for node's content also
	 * @return whether node was found
	 * @note This will not test the calling element, only it's children.
	 */
	bool FindElementDeep(
			ElementNodePtr& outParentNode,
			int& outElementIndex,
			wxString const& inName,
			wxString const* inValue = NULL) const;

	/**
	 * Populate this element from the given buffer.
	 * @param inData XML data to load.
	 * @param nData Length of inData buffer.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Whether file loaded successfully.
	 */
	bool LoadXMLBuffer(
			char const* inData,
			size_t nData,
			wxString& ioErrMsg);

	/**
	 * Populate this element from the given file.
	 * @param inFileName XML file to load.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Whether file loaded successfully.
	 */
	bool LoadXMLFile(
			wxChar const* inFileName,
			wxString& ioErrMsg);

	/**
	 * Save this element to the given output stream.
	 * @param outStream Stream to write tree to.
	 * @retval true Tree successfully written.
	 * @retval false Tree failed to save.
	 */
	bool SaveXML(std::ostream& outStream) const;

	/**
	 * Save this element to the given output stream.
	 * @param outStream Stream to write tree to.
	 * @param inDTD DTD to include in generation of XML file.
	 * @retval true Tree successfully written.
	 * @retval false Tree failed to save.
	 */
	bool SaveXML(
			std::ostream& outStream,
			std::string const& inDTD) const;

	/**
	 * Save this element to the given file.
	 * @param outFile File to write tree to.
	 * @retval true Tree successfully written.
	 * @retval false Tree failed to save.
	 */
	bool SaveXML(wxChar const* outFile) const;

	/**
	 * Save this element to the given file.
	 * @param outFile File to write tree to.
	 * @param inDTD DTD to include in generation of XML file.
	 * @retval true Tree successfully written.
	 * @retval false Tree failed to save.
	 */
	bool SaveXML(
			wxChar const* outFile,
			std::string const& inDTD) const;

protected:
	void RemoveAllTextNodes();

	ElementNodeWPtr m_Me;
	wxString m_Name;
	typedef std::map<wxString, wxString> MyAttributes;
	wxString m_Value;
	MyAttributes m_Attribs;
	std::vector<ElementPtr> m_Elements;
};

class ElementText : public Element
{
private:
	ElementText();
	ElementText(wxString const& inText);
	ElementText(ElementText const&); // Not implemented
	ElementText& operator=(ElementText const&); // Not implemented

public:
	static ElementTextPtr New();
	static ElementTextPtr New(wxString const& inText);

	virtual void Dump(int inLevel = 0) const;
	virtual Element::ElementType GetType() const;
	virtual wxString const& GetName() const;
	virtual void SetName(wxString const& inName);
	virtual wxString GetValue() const;
	virtual void SetValue(wxString const& inValue);
	virtual void SetValue(wxChar const* const inValue);
	virtual void SetValue(short inValue);
	virtual void SetValue(long inValue);
	virtual void SetValue(double inValue, int inPrec = 2);

protected:
	ElementTextWPtr m_Me;
	wxString m_Value;
};
