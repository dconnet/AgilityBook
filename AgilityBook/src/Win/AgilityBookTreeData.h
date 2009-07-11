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
 *
 * @brief Data classes to support AgilityBookTree operations.
 * @author David Connet
 *
 * Revision History
 * @li 2009-07-11 DRC Added GetType()
 * @li 2009-02-02 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2003-12-30 DRC Implemented customized text in tree.
 */

#include "AgilityBookMenu.h"
#include "ARBDog.h"
#include "ARBDogTrial.h"
#include "ListData.h"
#include <vector>
class CAgilityBookTreeView;
class CAgilityBookTreeDataDog;
class CAgilityBookTreeDataTrial;
class CAgilityBookTreeDataRun;


class CAgilityBookTreeData : public CTreeData
{
public:
	CAgilityBookTreeData(CAgilityBookTreeView* pTree)
		: m_pTree(pTree)
	{}

	// This removes a need for a dynamic cast
	enum CTreeDataType { eTreeDog, eTreeTrial, eTreeRun };
	virtual CTreeDataType GetType() const = 0;

	virtual CAgilityBookTreeData const* GetParent() const = 0;
	virtual ARBBasePtr GetARBBase() const
	{
		return ARBBasePtr();
	}
	virtual ARBDogPtr GetDog() const
	{
		return ARBDogPtr();
	}
	virtual ARBDogTrialPtr GetTrial() const
	{
		return ARBDogTrialPtr();
	}
	virtual ARBDogRunPtr GetRun() const
	{
		return ARBDogRunPtr();
	}
	virtual ARBDogPtr GetDog()
	{
		return ARBDogPtr();
	}
	virtual ARBDogTrialPtr GetTrial()
	{
		return ARBDogTrialPtr();
	}
	virtual ARBDogRunPtr GetRun()
	{
		return ARBDogRunPtr();
	}
	virtual CAgilityBookTreeDataDog const* GetDataDog() const
	{
		return NULL;
	}
	virtual CAgilityBookTreeDataTrial const* GetDataTrial() const
	{
		return NULL;
	}
	virtual CAgilityBookTreeDataRun const* GetDataRun() const
	{
		return NULL;
	}
	virtual CAgilityBookTreeDataDog* GetDataDog()
	{
		return NULL;
	}
	virtual CAgilityBookTreeDataTrial* GetDataTrial()
	{
		return NULL;
	}
	virtual CAgilityBookTreeDataRun* GetDataRun()
	{
		return NULL;
	}

	virtual MenuIdentityPopup GetMenuID() const = 0;
	virtual bool OnUpdateCmd(int id, bool& ioEnable) const = 0;
	virtual bool OnCmd(
			int id,
			bool& bModified,
			bool* bTreeSelectionSet) = 0; // Returns true if data modified
	virtual wxString OnNeedText() const = 0;
	virtual int OnNeedIcon() const = 0;

	virtual void Properties() = 0;

	bool CanPaste() const;
	bool DoPaste(bool* bTreeSelectionSet);

protected:
	CAgilityBookTreeView* m_pTree;
	std::vector<long> const& GetDogColumns() const;
	std::vector<long> const& GetTrialColumns() const;
	std::vector<long> const& GetRunColumns() const;
};

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookTreeDataDog : public CAgilityBookTreeData
{
public:
	CAgilityBookTreeDataDog(
			CAgilityBookTreeView* pTree,
			ARBDogPtr pDog);
	~CAgilityBookTreeDataDog();

	virtual CTreeDataType GetType() const
	{
		return CAgilityBookTreeData::eTreeDog;
	}

	virtual CAgilityBookTreeData const* GetParent() const
	{
		return NULL;
	}
	virtual ARBBasePtr GetARBBase() const
	{
		return m_pDog;
	}
	virtual ARBDogPtr GetDog() const
	{
		return m_pDog;
	}
	virtual ARBDogPtr GetDog()
	{
		return m_pDog;
	}
	virtual CAgilityBookTreeDataDog const* GetDataDog() const
	{
		return this;
	}
	virtual CAgilityBookTreeDataDog* GetDataDog()
	{
		return this;
	}

	virtual MenuIdentityPopup GetMenuID() const
	{
		return IdMenuDog;
	}
	virtual bool OnUpdateCmd(int id, bool& ioEnable) const;
	virtual bool OnCmd(
			int id,
			bool& bModified,
			bool* bTreeSelectionSet);
	virtual wxString OnNeedText() const;
	virtual int OnNeedIcon() const;

	virtual void Properties();

private:
	ARBDogPtr m_pDog;
};

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookTreeDataTrial : public CAgilityBookTreeData
{
public:
	CAgilityBookTreeDataTrial(
			CAgilityBookTreeView* pTree,
			ARBDogTrialPtr pTrial);

	virtual CTreeDataType GetType() const
	{
		return CAgilityBookTreeData::eTreeTrial;
	}

	virtual CAgilityBookTreeData const* GetParent() const
	{
		return GetDataDog();
	}
	virtual ARBBasePtr GetARBBase() const
	{
		return m_pTrial;
	}
	virtual ARBDogPtr GetDog() const;
	virtual ARBDogTrialPtr GetTrial() const
	{
		return m_pTrial;
	}
	virtual ARBDogPtr GetDog();
	virtual ARBDogTrialPtr GetTrial()
	{
		return m_pTrial;
	}
	virtual CAgilityBookTreeDataDog const* GetDataDog() const;
	virtual CAgilityBookTreeDataTrial const* GetDataTrial() const
	{
		return this;
	}
	virtual CAgilityBookTreeDataDog* GetDataDog();
	virtual CAgilityBookTreeDataTrial* GetDataTrial()
	{
		return this;
	}

	virtual MenuIdentityPopup GetMenuID() const
	{
		return IdMenuTrial;
	}
	virtual bool OnUpdateCmd(int id, bool& ioEnable) const;
	virtual bool OnCmd(
			int id,
			bool& bModified,
			bool* bTreeSelectionSet);
	virtual wxString OnNeedText() const;
	virtual int OnNeedIcon() const;

	virtual void Properties();

private:
	ARBDogTrialPtr m_pTrial;
	int m_idxIcon;
};

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookTreeDataRun : public CAgilityBookTreeData
{
public:
	CAgilityBookTreeDataRun(
			CAgilityBookTreeView* pTree,
			ARBDogRunPtr pRun);
	~CAgilityBookTreeDataRun();

	virtual CTreeDataType GetType() const
	{
		return CAgilityBookTreeData::eTreeRun;
	}

	virtual CAgilityBookTreeData const* GetParent() const
	{
		return GetDataTrial();
	}
	virtual ARBBasePtr GetARBBase() const
	{
		return m_pRun;
	}
	virtual ARBDogPtr GetDog() const;
	virtual ARBDogTrialPtr GetTrial() const;
	virtual ARBDogRunPtr GetRun() const
	{
		return m_pRun;
	}
	virtual ARBDogPtr GetDog();
	virtual ARBDogTrialPtr GetTrial();
	virtual ARBDogRunPtr GetRun()
	{
		return m_pRun;
	}
	virtual CAgilityBookTreeDataDog const* GetDataDog() const;
	virtual CAgilityBookTreeDataTrial const* GetDataTrial() const;
	virtual CAgilityBookTreeDataRun const* GetDataRun() const
	{
		return this;
	}
	virtual CAgilityBookTreeDataDog* GetDataDog();
	virtual CAgilityBookTreeDataTrial* GetDataTrial();
	virtual CAgilityBookTreeDataRun* GetDataRun()
	{
		return this;
	}

	virtual MenuIdentityPopup GetMenuID() const
	{
		return IdMenuRun;
	}
	virtual bool OnUpdateCmd(int id, bool& ioEnable) const;
	virtual bool OnCmd(
			int id,
			bool& bModified,
			bool* bTreeSelectionSet);
	virtual wxString OnNeedText() const;
	virtual int OnNeedIcon() const;

	virtual void Properties();

private:
	ARBDogRunPtr m_pRun;
};
