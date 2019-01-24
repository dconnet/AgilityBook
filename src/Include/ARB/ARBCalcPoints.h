#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBCalcPoints class
 * @author David Connet
 *
 * Revision History
 * 2018-06-23 Added USDAA Top10.
 * 2011-07-31 Created
 */

#include "ARB/ARBTypes2.h"
ARB_TYPEDEF(ARBCalcPoints)
ARB_TYPEDEF(ARBCalcPointsNormal)
ARB_TYPEDEF(ARBCalcPointsT2B)
ARB_TYPEDEF(ARBCalcPointsUKI)
ARB_TYPEDEF(ARBCalcPointsTop10USDAA)


enum ARBPointsType
{
	// Note, these faults are used as indices in the UI
	ePointsTypeNormal = 0, ///< Original points computation.
	ePointsTypeT2B,        ///< AKC/T2B: based on percentage of SCT.
	ePointsTypeUKI,        ///< UKI: points based on placement.
	ePointsTypeTop10USDAA, ///< USDAA: Top10 points based on placement/inclass
	ePointsTypeMax
};


/**
 * Calculate title (etc) points.
 */
class ARBCalcPoints
{
protected:
	ARBCalcPoints();
	ARBCalcPoints(ARBCalcPoints const&) = delete;
	ARBCalcPoints(ARBCalcPoints&&) = delete;
	ARBCalcPoints& operator=(ARBCalcPoints const&) = delete;
	ARBCalcPoints& operator=(ARBCalcPoints&&) = delete;

public:
	virtual ~ARBCalcPoints();
	static ARBCalcPointsPtr New(ARBPointsType type);

	/**
	 * What is this?
	 */
	virtual ARBPointsType GetType() const = 0;

	virtual std::wstring GetGenericName(double points, double faults) const = 0;

	/**
	 * Allow ARBConfigTitlePoints to create items.
	 */
	virtual bool AllowConfiguration() const	{return false;}

	virtual double GetPoints(
			double inPoints,
			double inTime,
			double inSCT,
			short inPlace,
			short inClass) const = 0;
};


/**
 * Calculate points based on faults.
 */
class ARBCalcPointsNormal : public ARBCalcPoints
{
protected:
	ARBCalcPointsNormal();
public:
	static ARBCalcPointsNormalPtr New();
	virtual ARBPointsType GetType() const	{return ePointsTypeNormal;}
	virtual std::wstring GetGenericName(double points, double faults) const;
	virtual bool AllowConfiguration() const	{return true;}
	virtual double GetPoints(
			double inPoints,
			double inTime,
			double inSCT,
			short inPlace,
			short inClass) const;
};


/**
 * Calculate points based on percentage of SCT (AKC-T2B).
 */
class ARBCalcPointsT2B : public ARBCalcPoints
{
protected:
	ARBCalcPointsT2B();
public:
	static ARBCalcPointsT2BPtr New();
	virtual ARBPointsType GetType() const	{return ePointsTypeT2B;}
	virtual std::wstring GetGenericName(double points, double faults) const;
	virtual double GetPoints(
			double inPoints,
			double inTime,
			double inSCT,
			short inPlace,
			short inClass) const;
};


/**
 * Calculate points based on placement (UKI).
 */
class ARBCalcPointsUKI : public ARBCalcPoints
{
protected:
	ARBCalcPointsUKI();
public:
	static ARBCalcPointsUKIPtr New();
	virtual ARBPointsType GetType() const	{return ePointsTypeUKI;}
	virtual std::wstring GetGenericName(double points, double faults) const;
	virtual double GetPoints(
			double inPoints,
			double inTime,
			double inSCT,
			short inPlace,
			short inClass) const;
};


/**
 * Calculate Top Ten points based on placement (USDAA).
 */
class ARBCalcPointsTop10USDAA : public ARBCalcPoints
{
protected:
	ARBCalcPointsTop10USDAA();
public:
	static ARBCalcPointsTop10USDAAPtr New();
	virtual ARBPointsType GetType() const	{return ePointsTypeTop10USDAA;}
	virtual std::wstring GetGenericName(double points, double faults) const;
	virtual double GetPoints(
			double inPoints,
			double inTime,
			double inSCT,
			short inPlace,
			short inClass) const;
};
