/*
 * Copyright � 2002-2003 David Connet. All Rights Reserved.
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
 *
 * @brief The classes that make up the dog's information.
 * @author David Connet
 *
 * Revision History
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-09-08 DRC Changed AddPartner usage.
 */

#include "StdAfx.h"
#include "ARBDogRunPartner.h"

#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBDogRunPartner::ARBDogRunPartner()
	: m_Handler()
	, m_Dog()
	, m_RegNum()
{
}

ARBDogRunPartner::ARBDogRunPartner(const ARBDogRunPartner& rhs)
	: m_Handler(rhs.m_Handler)
	, m_Dog(rhs.m_Dog)
	, m_RegNum(rhs.m_RegNum)
{
}

ARBDogRunPartner::~ARBDogRunPartner()
{
}

ARBDogRunPartner& ARBDogRunPartner::operator=(const ARBDogRunPartner& rhs)
{
	if (this != &rhs)
	{
		m_Handler = rhs.m_Handler;
		m_Dog = rhs.m_Dog;
		m_RegNum = rhs.m_RegNum;
	}
	return *this;
}

bool ARBDogRunPartner::operator==(const ARBDogRunPartner& rhs) const
{
	return m_Handler == rhs.m_Handler
		&& m_Dog == rhs.m_Dog
		&& m_RegNum == rhs.m_RegNum;
}

bool ARBDogRunPartner::operator!=(const ARBDogRunPartner& rhs) const
{
	return !operator==(rhs);
}

bool ARBDogRunPartner::Load(
	const ARBConfig& inConfig,
	const CElement& inTree,
	const ARBVersion& inVersion)
{
	if (CElement::eFound != inTree.GetAttrib(ATTRIB_PARTNER_HANDLER, m_Handler)
	|| 0 == m_Handler.length())
	{
		ErrorMissingAttribute(TREE_PARTNER, ATTRIB_PARTNER_HANDLER);
		return false;
	}

	if (CElement::eFound != inTree.GetAttrib(ATTRIB_PARTNER_DOG, m_Dog)
	|| 0 == m_Dog.length())
	{
		ErrorMissingAttribute(TREE_PARTNER, ATTRIB_PARTNER_DOG);
		return false;
	}

	inTree.GetAttrib(ATTRIB_PARTNER_REGNUM, m_RegNum);

	return true;
}

bool ARBDogRunPartner::Save(CElement& ioTree) const
{
	CElement& partner = ioTree.AddElement(TREE_PARTNER);
	partner.AddAttrib(ATTRIB_PARTNER_HANDLER, m_Handler);
	partner.AddAttrib(ATTRIB_PARTNER_DOG, m_Dog);
	if (0 < m_RegNum.length())
		partner.AddAttrib(ATTRIB_PARTNER_REGNUM, m_RegNum);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

ARBDogRunPartner* ARBDogRunPartnerList::AddPartner(ARBDogRunPartner* inPartner)
{
	if (inPartner)
	{
		inPartner->AddRef();
		push_back(inPartner);
	}
	return inPartner;
}
