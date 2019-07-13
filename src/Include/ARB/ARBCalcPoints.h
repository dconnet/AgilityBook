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

#include "ARBTypes2.h"
#include "LibwxARB.h"
ARB_TYPEDEF(ARBCalcPoints)
ARB_TYPEDEF(ARBCalcPointsNormal)
ARB_TYPEDEF(ARBCalcPointsT2B)
ARB_TYPEDEF(ARBCalcPointsTop10USDAA)
ARB_TYPEDEF(ARBCalcPointsUKI)


enum class ARBPointsType
{
	// Note, these faults are used as indices in the UI
	Normal = 0, ///< Original points computation.
	T2B,        ///< AKC/T2B: based on percentage of SCT.
	UKI,        ///< UKI: points based on placement.
	Top10USDAA, ///< USDAA: Top10 points based on placement/inclass
	Max
};


/**
 * Calculate title (etc) points.
 */
class ARB_API ARBCalcPoints
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
class ARB_API ARBCalcPointsNormal : public ARBCalcPoints
{
protected:
	ARBCalcPointsNormal();
public:
	static ARBCalcPointsNormalPtr New();
	ARBPointsType GetType() const override {return ARBPointsType::Normal;}
	std::wstring GetGenericName(double points, double faults) const override;
	bool AllowConfiguration() const override {return true;}
	double GetPoints(
			double inPoints,
			double inTime,
			double inSCT,
			short inPlace,
			short inClass) const override;
};


/**
 * Calculate points based on percentage of SCT (AKC-T2B).
 */
class ARB_API ARBCalcPointsT2B : public ARBCalcPoints
{
protected:
	ARBCalcPointsT2B();
public:
	static ARBCalcPointsT2BPtr New();
	ARBPointsType GetType() const override {return ARBPointsType::T2B;}
	std::wstring GetGenericName(double points, double faults) const override;
	double GetPoints(
			double inPoints,
			double inTime,
			double inSCT,
			short inPlace,
			short inClass) const override;
};


/**
 * Calculate points based on placement (UKI).
 */
class ARB_API ARBCalcPointsUKI : public ARBCalcPoints
{
protected:
	ARBCalcPointsUKI();
public:
	static ARBCalcPointsUKIPtr New();
	ARBPointsType GetType() const override {return ARBPointsType::UKI;}
	std::wstring GetGenericName(double points, double faults) const override;
	double GetPoints(
			double inPoints,
			double inTime,
			double inSCT,
			short inPlace,
			short inClass) const override;
};


/**
 * Calculate Top Ten points based on placement (USDAA).
 */
class ARB_API ARBCalcPointsTop10USDAA : public ARBCalcPoints
{
protected:
	ARBCalcPointsTop10USDAA();
public:
	static ARBCalcPointsTop10USDAAPtr New();
	ARBPointsType GetType() const override {return ARBPointsType::Top10USDAA;}
	std::wstring GetGenericName(double points, double faults) const override;
	double GetPoints(
			double inPoints,
			double inTime,
			double inSCT,
			short inPlace,
			short inClass) const override;
};
