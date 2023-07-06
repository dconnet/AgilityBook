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
 * 2020-01-18 USDAA Top10 changed.
 * 2018-06-23 Added USDAA Top10.
 * 2011-07-31 Created
 */

#include "ARBCommon/ARBDate.h"
#include "ARBTypes2.h"
#include "LibwxARB.h"


namespace dconSoft
{
namespace ARB
{

ARB_TYPEDEF(ARBCalcPoints)
ARB_TYPEDEF(ARBCalcPointsNormal)
ARB_TYPEDEF(ARBCalcPointsT2B)
ARB_TYPEDEF(ARBCalcPointsTop10USDAA)
ARB_TYPEDEF(ARBCalcPointsUKI)


enum class ARBPointsType
{
	Normal,     ///< Original points computation.
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
	virtual bool AllowConfiguration() const
	{
		return false;
	}

	/**
	 * Get the number of points, assumes run is a Q.
	 */
	virtual double GetPoints(
		double inPoints,
		double inTime,
		double inSCT,
		short inPlace,
		short inClass,
		ARBCommon::ARBDate date, // For USDAA
		bool isTourney,          // For USDAA
		bool isAtHome            // @Home accumulates points differently
	) const
		= 0;
};


/**
 * Calculate points based on faults.
 */
class ARB_API ARBCalcPointsNormal : public ARBCalcPoints
{
public:
	static ARBCalcPointsNormalPtr New();
	ARBCalcPointsNormal();
	ARBPointsType GetType() const override
	{
		return ARBPointsType::Normal;
	}
	std::wstring GetGenericName(double points, double faults) const override;
	bool AllowConfiguration() const override
	{
		return true;
	}
	double GetPoints(
		double inPoints,
		double inTime,
		double inSCT,
		short inPlace,
		short inClass,
		ARBCommon::ARBDate date,
		bool isTourney,
		bool isAtHome) const override;
};


/**
 * Calculate points based on percentage of SCT (AKC-T2B).
 */
class ARB_API ARBCalcPointsT2B : public ARBCalcPoints
{
public:
	static ARBCalcPointsT2BPtr New();
	ARBCalcPointsT2B();
	ARBPointsType GetType() const override
	{
		return ARBPointsType::T2B;
	}
	std::wstring GetGenericName(double points, double faults) const override;
	double GetPoints(
		double inPoints,
		double inTime,
		double inSCT,
		short inPlace,
		short inClass,
		ARBCommon::ARBDate date,
		bool isTourney,
		bool isAtHome) const override;
};


/**
 * Calculate points based on placement (UKI).
 */
class ARB_API ARBCalcPointsUKI : public ARBCalcPoints
{
public:
	static ARBCalcPointsUKIPtr New();
	ARBCalcPointsUKI();
	ARBPointsType GetType() const override
	{
		return ARBPointsType::UKI;
	}
	std::wstring GetGenericName(double points, double faults) const override;
	double GetPoints(
		double inPoints,
		double inTime,
		double inSCT,
		short inPlace,
		short inClass,
		ARBCommon::ARBDate date,
		bool isTourney,
		bool isAtHome) const override;
};


/**
 * Calculate Top Ten points based on placement (USDAA).
 */
class ARB_API ARBCalcPointsTop10USDAA : public ARBCalcPoints
{
public:
	static ARBCalcPointsTop10USDAAPtr New();
	ARBCalcPointsTop10USDAA();
	ARBPointsType GetType() const override
	{
		return ARBPointsType::Top10USDAA;
	}
	std::wstring GetGenericName(double points, double faults) const override;
	double GetPoints(
		double inPoints,
		double inTime,
		double inSCT,
		short inPlace,
		short inClass,
		ARBCommon::ARBDate date,
		bool isTourney,
		bool isAtHome) const override;
};

} // namespace ARB
} // namespace dconSoft
