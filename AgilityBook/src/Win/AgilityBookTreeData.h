#pragma once

/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
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
 * @brief Data classes to support AgilityBookTree operations.
 * @author David Connet
 *
 * Revision History
 */

#include <list>
#include "AgilityBookDoc.h"
class ARBDog;
class ARBDogRun;
class ARBDogTrial;
class CAgilityBookTree;
class CAgilityBookTreeDataDog;
class CAgilityBookTreeDataTrial;
class CAgilityBookTreeDataRun;
class CAgilityBookViewDataCalendar;

class CAgilityBookTreeData
{
public:
	CAgilityBookTreeData(CAgilityBookTree* pTree)
		: m_pTree(pTree)
		, m_hItem(NULL)
	{}
	virtual ~CAgilityBookTreeData() {}

	HTREEITEM GetHTreeItem() const									{return m_hItem;}
	void SetHTreeItem(HTREEITEM hItem)								{m_hItem = hItem;}
	virtual const CAgilityBookTreeData* GetParent() const = 0;
	virtual const ARBBase* GetARBBase() const						{return NULL;}
	virtual const ARBDog* GetDog() const							{return NULL;}
	virtual const ARBDogTrial* GetTrial() const						{return NULL;}
	virtual const ARBDogRun* GetRun() const							{return NULL;}
	virtual ARBDog* GetDog()										{return NULL;}
	virtual ARBDogTrial* GetTrial()									{return NULL;}
	virtual ARBDogRun* GetRun()										{return NULL;}
	virtual const CAgilityBookTreeDataDog* GetDataDog() const		{return NULL;}
	virtual const CAgilityBookTreeDataTrial* GetDataTrial() const	{return NULL;}
	virtual const CAgilityBookTreeDataRun* GetDataRun() const		{return NULL;}
	virtual CAgilityBookTreeDataDog* GetDataDog()					{return NULL;}
	virtual CAgilityBookTreeDataTrial* GetDataTrial()				{return NULL;}
	virtual CAgilityBookTreeDataRun* GetDataRun()					{return NULL;}

	virtual UINT GetMenuID() const = 0;
	virtual bool OnUpdateCmd(UINT id) const = 0;
	virtual bool OnCmd(UINT id, bool* bTreeSelectionSet) = 0; // Returns true if data modified
	virtual CString OnNeedText() const = 0;

	virtual void Properties() = 0;

	bool CanPaste() const;
	bool DoPaste();

protected:
	CAgilityBookTree* m_pTree;
	HTREEITEM m_hItem;
};

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookTreeDataDog : public CAgilityBookTreeData
{
public:
	CAgilityBookTreeDataDog(CAgilityBookTree* pTree, ARBDog* pDog);
	~CAgilityBookTreeDataDog();

	virtual const CAgilityBookTreeData* GetParent() const		{return NULL;}
	virtual const ARBBase* GetARBBase() const					{return m_pDog;}
	virtual const ARBDog* GetDog() const						{return m_pDog;}
	virtual ARBDog* GetDog()									{return m_pDog;}
	virtual const CAgilityBookTreeDataDog* GetDataDog() const	{return this;}
	virtual CAgilityBookTreeDataDog* GetDataDog()				{return this;}

	virtual UINT GetMenuID() const								{return IDR_DOG;}
	virtual bool OnUpdateCmd(UINT id) const;
	virtual bool OnCmd(UINT id, bool* bTreeSelectionSet);
	virtual CString OnNeedText() const;

	virtual void Properties();

private:
	ARBDog* m_pDog;
};

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookTreeDataTrial : public CAgilityBookTreeData
{
public:
	CAgilityBookTreeDataTrial(CAgilityBookTree* pTree, ARBDogTrial* pTrial);
	~CAgilityBookTreeDataTrial();

	virtual const CAgilityBookTreeData* GetParent() const			{return GetDataDog();}
	virtual const ARBBase* GetARBBase() const						{return m_pTrial;}
	virtual const ARBDog* GetDog() const;
	virtual const ARBDogTrial* GetTrial() const						{return m_pTrial;}
	virtual ARBDog* GetDog();
	virtual ARBDogTrial* GetTrial()									{return m_pTrial;}
	virtual const CAgilityBookTreeDataDog* GetDataDog() const;
	virtual const CAgilityBookTreeDataTrial* GetDataTrial() const	{return this;}
	virtual CAgilityBookTreeDataDog* GetDataDog();
	virtual CAgilityBookTreeDataTrial* GetDataTrial()				{return this;}

	virtual UINT GetMenuID() const									{return IDR_TRIAL;}
	virtual bool OnUpdateCmd(UINT id) const;
	virtual bool OnCmd(UINT id, bool* bTreeSelectionSet);
	virtual CString OnNeedText() const;

	virtual void Properties();

private:
	ARBDogTrial* m_pTrial;
};

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookTreeDataRun : public CAgilityBookTreeData
{
public:
	CAgilityBookTreeDataRun(CAgilityBookTree* pTree, ARBDogRun* pRun);
	~CAgilityBookTreeDataRun();

	virtual const CAgilityBookTreeData* GetParent() const			{return GetDataTrial();}
	virtual const ARBBase* GetARBBase() const						{return m_pRun;}
	virtual const ARBDog* GetDog() const;
	virtual const ARBDogTrial* GetTrial() const;
	virtual const ARBDogRun* GetRun() const							{return m_pRun;}
	virtual ARBDog* GetDog();
	virtual ARBDogTrial* GetTrial();
	virtual ARBDogRun* GetRun()										{return m_pRun;}
	virtual const CAgilityBookTreeDataDog* GetDataDog() const;
	virtual const CAgilityBookTreeDataTrial* GetDataTrial() const;
	virtual const CAgilityBookTreeDataRun* GetDataRun() const		{return this;}
	virtual CAgilityBookTreeDataDog* GetDataDog();
	virtual CAgilityBookTreeDataTrial* GetDataTrial();
	virtual CAgilityBookTreeDataRun* GetDataRun()					{return this;}

	virtual UINT GetMenuID() const									{return IDR_RUN;}
	virtual bool OnUpdateCmd(UINT id) const;
	virtual bool OnCmd(UINT id, bool* bTreeSelectionSet);
	virtual CString OnNeedText() const;

	virtual void Properties();

private:
	ARBDogRun* m_pRun;
};
