#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Element class.
 * @author David Connet
 *
 * Revision History
 * 2022-01-31 Add wxDateTime support.
 * 2018-12-16 Convert to fmt.
 * 2012-11-25 Add libxml support back in.
 * 2012-04-10 Based on wx-group thread, use std::string for internal use
 * 2012-03-16 Renamed LoadXML functions, added stream version.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-03-12 Converting all TCHAR stuff to wxWidgets
 * 2007-09-06 Added GetNthElementNode
 * 2007-08-15 Modified to support mixed text/nodes.
 * 2007-08-08 Moved initialization here, so all XML usage is contained.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-10-22 Added a DTD parameter to SaveXML.
 */

#include "ARBTypes.h"
#include "LibwxARBCommon.h"

#include "fmt/xchar.h"
#include <istream>
#include <map>
#include <vector>
class ARBDate;
class ARBVersion;


enum class ARBElementType
{
	Node,
	Text
};


/**
 * Result of getting an attribute.
 */
enum class ARBAttribLookup
{
	NotFound, ///< Attribute was not found.
	Invalid,  ///< Attribute's value is not valid for data type.
	Found     ///< Attribute was found.
};


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
class ARBCOMMON_API Element
{
protected:
	Element();
	Element(Element const&) = delete;
	Element(Element&&) = delete;
	Element& operator=(Element const&) = delete;
	Element& operator=(Element&&) = delete;

public:
	/**
	 * Call once (at program startup) to initialize XML subsystem.
	 * @param outMsg In case of failure, error message.
	 * @return Initialization success.
	 */
	static bool Initialize(std::wstring& outMsg);

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

	virtual ARBElementType GetType() const = 0;

	/**
	 * Get the name of this element.
	 */
	virtual std::wstring const& GetName() const = 0;

	/**
	 * Set the name of this element.
	 * @param inName New name for this element.
	 */
	virtual void SetName(std::wstring const& inName) = 0;

	/**
	 * Get the value of this element. This will concatenate all text nodes.
	 */
	virtual std::wstring GetValue() const = 0;

	/**
	 * Set the value of this element. If this element has text nodes, they
	 * will all be consolidated into one.
	 * @pre GetElementCount() must be 0.
	 * @param inValue New value for this element.
	 */
	virtual void SetValue(std::wstring const& inValue) = 0;
	virtual void SetValue(wchar_t const* const inValue) = 0;
	virtual void SetValue(short inValue) = 0;
	virtual void SetValue(unsigned short inValue) = 0;
	virtual void SetValue(long inValue) = 0;
	virtual void SetValue(unsigned long inValue) = 0;

	/**
	 * Set the value of this element.
	 * @pre GetElementCount() must be 0.
	 * @param inValue New value for this element.
	 * @param inPrec Precision, trailing zeros are trimmed unless prec=2, then they are only trimmed if all zero.
	 */
	virtual void SetValue(double inValue, int inPrec = 2) = 0;
};


class ARBCOMMON_API ElementNode : public Element
{
protected:
	ElementNode();
	explicit ElementNode(std::wstring const& inName);
	DECLARE_NO_COPY_IMPLEMENTED(ElementNode);

public:
	static ElementNodePtr New();
	static ElementNodePtr New(std::wstring const& inName);

	void Dump(int inLevel = 0) const override;
	ARBElementType GetType() const override;
	std::wstring const& GetName() const override;
	void SetName(std::wstring const& inName) override;
	std::wstring GetValue() const override;
	void SetValue(std::wstring const& inValue) override;
	void SetValue(wchar_t const* const inValue) override;
	void SetValue(short inValue) override;
	void SetValue(unsigned short inValue) override;
	void SetValue(long inValue) override;
	void SetValue(unsigned long inValue) override;
	void SetValue(double inValue, int inPrec = 2) override;

	/**
	 * Clear the name, value, attributes, and subelements (everything!).
	 */
	void clear();

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
	ARBAttribLookup GetNthAttrib(int inIndex, std::wstring& outName, std::wstring& outValue) const;

	/**
	 * Get the value of an attribute.
	 * @param inName Name of attribute to get.
	 * @param outValue Value of attribute
	 * @return Result of lookup.
	 */
	ARBAttribLookup GetAttrib(std::wstring const& inName, std::wstring& outValue) const;
	ARBAttribLookup GetAttrib(std::wstring const& inName, ARBVersion& outValue) const;
	ARBAttribLookup GetAttrib(std::wstring const& inName, ARBDate& outValue) const;
#if defined(__WXWINDOWS__)
	ARBAttribLookup GetAttrib(std::wstring const& inName, wxDateTime& outValue) const;
#endif
	ARBAttribLookup GetAttrib(std::wstring const& inName, bool& outValue) const;
	ARBAttribLookup GetAttrib(std::wstring const& inName, short& outValue) const;
	ARBAttribLookup GetAttrib(std::wstring const& inName, unsigned short& outValue) const;
	ARBAttribLookup GetAttrib(std::wstring const& inName, long& outValue) const;
	ARBAttribLookup GetAttrib(std::wstring const& inName, unsigned long& outValue) const;
	ARBAttribLookup GetAttrib(std::wstring const& inName, double& outValue) const;

	/**
	 * Add an attribute.
	 * @param inName Name of attribute to add.
	 * @param inValue Value of new attribute.
	 * @return This always returns true.
	 * @post If inName already exists, the previous value will be overwritten.
	 */
	bool AddAttrib(std::wstring const& inName, std::wstring const& inValue);
	bool AddAttrib(std::wstring const& inName, wchar_t const* const inValue);
	bool AddAttrib(std::wstring const& inName, ARBVersion const& inValue);
	bool AddAttrib(std::wstring const& inName, ARBDate const& inValue);
#if defined(__WXWINDOWS__)
	bool AddAttrib(std::wstring const& inName, wxDateTime const& inValue);
#endif
	bool AddAttrib(std::wstring const& inName, bool inValue);
	bool AddAttrib(std::wstring const& inName, short inValue);
	bool AddAttrib(std::wstring const& inName, unsigned short inValue);
	bool AddAttrib(std::wstring const& inName, long inValue);
	bool AddAttrib(std::wstring const& inName, unsigned long inValue);

	/**
	 * Add an attribute.
	 * @param inName Name of attribute to add.
	 * @param inValue Value of new attribute.
	 * @param inPrec Precision, trailing zeros are trimmed unless prec=2, then they are only trimmed if all zero.
	 * @return This always returns true.
	 * @post If inName already exists, the previous value will be overwritten.
	 */
	bool AddAttrib(std::wstring const& inName, double inValue, int inPrec = 2);

	/**
	 * Remove an attribute.
	 * @param inName Attribute to remove
	 * @return Whether or not the attribute was removed.
	 */
	bool RemoveAttrib(std::wstring const& inName);

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
	int GetNodeCount(ARBElementType type) const;

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
	ElementNodePtr AddElementNode(std::wstring const& inName, int inAt = -1);

	/**
	 * Add an element.
	 * If inAt is less than zero or greater than the number of items,
	 * the item will be appended.
	 * @pre GetValue() must be empty.
	 * @param inText Text of new text element.
	 * @param inAt Add the new element at the specified location.
	 * @return The new element.
	 */
	ElementTextPtr AddElementText(std::wstring const& inText, int inAt = -1);

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
	int FindElement(std::wstring const& inName, int inStartFrom = 0) const;

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
		ElementNode const*& outParentNode,
		int& outElementIndex,
		std::wstring const& inName,
		std::wstring const* inValue = nullptr) const;

	/**
	 * Populate this element from the given stream.
	 * @param inStream XML stream to load.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Whether file loaded successfully.
	 */
	bool LoadXML(std::istream& inStream, fmt::wmemory_buffer& ioErrMsg);

	/**
	 * Populate this element from the given buffer.
	 * @param inData XML data to load.
	 * @param nData Length of inData buffer.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Whether file loaded successfully.
	 */
	bool LoadXML(char const* inData, size_t nData, fmt::wmemory_buffer& ioErrMsg);

	/**
	 * Populate this element from the given file.
	 * @param inFileName XML file to load.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Whether file loaded successfully.
	 */
	bool LoadXML(wchar_t const* inFileName, fmt::wmemory_buffer& ioErrMsg);

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
	bool SaveXML(std::ostream& outStream, std::string const& inDTD) const;

	/**
	 * Save this element to the given file.
	 * @param outFile File to write tree to.
	 * @retval true Tree successfully written.
	 * @retval false Tree failed to save.
	 */
	bool SaveXML(std::wstring const& outFile) const;

	/**
	 * Save this element to the given file.
	 * @param outFile File to write tree to.
	 * @param inDTD DTD to include in generation of XML file.
	 * @retval true Tree successfully written.
	 * @retval false Tree failed to save.
	 */
	bool SaveXML(std::wstring const& outFile, std::string const& inDTD) const;

protected:
	void RemoveAllTextNodes();

	std::wstring m_Name;
	typedef std::map<std::wstring, std::wstring> MyAttributes;
	std::wstring m_Value;
	MyAttributes m_Attribs;
	std::vector<ElementPtr> m_Elements;
};


class ARBCOMMON_API ElementText : public Element
{
protected:
	ElementText();
	explicit ElementText(std::wstring const& inText);
	DECLARE_NO_COPY_IMPLEMENTED(ElementText);

public:
	static ElementTextPtr New();
	static ElementTextPtr New(std::wstring const& inText);

	void Dump(int inLevel = 0) const override;
	ARBElementType GetType() const override;
	std::wstring const& GetName() const override;
	void SetName(std::wstring const& inName) override;
	std::wstring GetValue() const override;
	void SetValue(std::wstring const& inValue) override;
	void SetValue(wchar_t const* const inValue) override;
	void SetValue(short inValue) override;
	void SetValue(unsigned short inValue) override;
	void SetValue(long inValue) override;
	void SetValue(unsigned long inValue) override;
	void SetValue(double inValue, int inPrec = 2) override;

protected:
	std::wstring m_Value;
};
