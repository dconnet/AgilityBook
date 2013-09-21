#pragma once
#if USE_TREELIST

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Data classes to support AgilityBookTreeList operations.
 * @author David Connet
 *
 * Revision History
 * @li 2013-04-22 DRC Converted tree+list into single control.
 * @li 2009-07-11 DRC Added GetType()
 * @li 2009-02-02 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2003-12-30 DRC Implemented customized text in tree.
 */

#include "AgilityBookMenu.h"
#include "AgilityBookTreeListModel.h"
#include "ListData.h"
#include "ARB/ARBDog.h"
#include "ARB/ARBDogTrial.h"
#include <vector>
class CAgilityBookTreeListDataDog;
class CAgilityBookTreeListDataRun;
class CAgilityBookTreeListDataTrial;
class CAgilityBookTreeListView;


class CAgilityBookTreeListData
{
public:
	CAgilityBookTreeListData(CAgilityBookTreeListModel* pModel);
	virtual ~CAgilityBookTreeListData();

	// This removes a need for a dynamic cast
	virtual CTreeListDataType Type() const = 0;

	virtual bool IsContainer() const = 0;
	virtual wxIcon GetIcon(unsigned int col) const	{return 0 == col ? m_icon : wxIcon();}
	virtual wxVariant GetColumn(
			ARBConfig const& config,
			std::vector<long> const& columns,
			unsigned int col,
			bool bInCompare) const = 0;

	CAgilityBookTreeListData* GetParent() const			{return m_parent;}
	//virtual CAgilityBookTreeListData const* GetParent() const {return m_parent;}
	unsigned int GetChildren(wxDataViewItemArray& array) const;
	void Append(CAgilityBookTreeListData* child);
	void Remove(CAgilityBookTreeListData* child);

	virtual ARBBasePtr GetARBBase() const			{return ARBBasePtr();}
	virtual ARBDogPtr GetDog() const				{return ARBDogPtr();}
	virtual ARBDogTrialPtr GetTrial() const			{return ARBDogTrialPtr();}
	virtual ARBDogRunPtr GetRun() const				{return ARBDogRunPtr();}
	virtual ARBDogPtr GetDog()						{return ARBDogPtr();}
	virtual ARBDogTrialPtr GetTrial()				{return ARBDogTrialPtr();}
	virtual ARBDogRunPtr GetRun()					{return ARBDogRunPtr();}

	virtual MenuIdentityPopup GetMenuID() const = 0;

protected:
	CAgilityBookTreeListModel* m_pModel;
	wxIcon m_icon;
private:
	CAgilityBookTreeListData* m_parent;
	std::vector<CAgilityBookTreeListData*> m_children;
};

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookTreeListDataDog : public CAgilityBookTreeListData
{
public:
	CAgilityBookTreeListDataDog(
			CAgilityBookTreeListModel* pModel,
			ARBDogPtr dog);
	~CAgilityBookTreeListDataDog();
	virtual CTreeListDataType Type() const
	{
		return eTreeDog;
	}

	virtual bool IsContainer() const		{return true;}
	virtual wxVariant GetColumn(
			ARBConfig const& config,
			std::vector<long> const& columns,
			unsigned int col,
			bool bInCompare) const;

	virtual ARBBasePtr GetARBBase() const			{return m_pDog;}
	virtual ARBDogPtr GetDog() const				{return m_pDog;}
	virtual ARBDogPtr GetDog()						{return m_pDog;}

	virtual MenuIdentityPopup GetMenuID() const
	{
		return IdMenuDog;
	}

private:
	ARBDogPtr m_pDog;
};

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookTreeListDataTrial : public CAgilityBookTreeListData
{
public:
	CAgilityBookTreeListDataTrial(
			CAgilityBookTreeListModel* pModel,
			ARBDogPtr dog,
			ARBDogTrialPtr trial);
	~CAgilityBookTreeListDataTrial();
	virtual CTreeListDataType Type() const
	{
		return eTreeTrial;
	}

	virtual bool IsContainer() const		{return true;}
	virtual wxVariant GetColumn(
			ARBConfig const& config,
			std::vector<long> const& columns,
			unsigned int col,
			bool bInCompare) const;

	virtual ARBBasePtr GetARBBase() const			{return m_pTrial;}
	virtual ARBDogPtr GetDog() const				{return m_pDog;}
	virtual ARBDogTrialPtr GetTrial() const			{return m_pTrial;}
	virtual ARBDogPtr GetDog()						{return m_pDog;}
	virtual ARBDogTrialPtr GetTrial()				{return m_pTrial;}

	virtual MenuIdentityPopup GetMenuID() const
	{
		return IdMenuTrial;
	}

private:
	ARBDogPtr m_pDog;
	ARBDogTrialPtr m_pTrial;
};

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookTreeListDataRun : public CAgilityBookTreeListData
{
public:
	CAgilityBookTreeListDataRun(
			CAgilityBookTreeListModel* pModel,
			ARBDogPtr dog,
			ARBDogTrialPtr trial,
			ARBDogRunPtr run);
	~CAgilityBookTreeListDataRun();
	virtual CTreeListDataType Type() const
	{
		return eTreeRun;
	}

	virtual bool IsContainer() const		{return false;}
	virtual wxVariant GetColumn(
			ARBConfig const& config,
			std::vector<long> const& columns,
			unsigned int col,
			bool bInCompare) const;

	virtual ARBBasePtr GetARBBase() const			{return m_pRun;}
	virtual ARBDogPtr GetDog() const				{return m_pDog;}
	virtual ARBDogTrialPtr GetTrial() const			{return m_pTrial;}
	virtual ARBDogRunPtr GetRun() const				{return m_pRun;}
	virtual ARBDogPtr GetDog()						{return m_pDog;}
	virtual ARBDogTrialPtr GetTrial()				{return m_pTrial;}
	virtual ARBDogRunPtr GetRun()					{return m_pRun;}

	virtual MenuIdentityPopup GetMenuID() const
	{
		return IdMenuRun;
	}

private:
	ARBDogPtr m_pDog;
	ARBDogTrialPtr m_pTrial;
	ARBDogRunPtr m_pRun;
};

#endif // USE_TREELIST
