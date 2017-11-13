/* Copyright (c) 2010 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 *   Manuel Zeise <zeise@cern.ch>
 *   Thomas Hauth <Thomas.Hauth@cern.ch>
 */

#ifndef KAPPA_MATCHING_H
#define KAPPA_MATCHING_H

#include <vector>
#include <Math/VectorUtil.h>
#include "Artus/KappaTools/interface/Toolbox/interface/IOHelper.h"

struct matchSort_deltaR
{
	matchSort_deltaR(const double dR = 0.3) : maxDeltaR(dR) {};

	template<typename T1, typename T2>
	double operator()(const T1 &obj1, const T2 &obj2) const
	{
		const double val = ROOT::Math::VectorUtil::DeltaR(obj1.p4, obj2.p4);
		return double(val) <= double(maxDeltaR) ? val : std::numeric_limits<double>::quiet_NaN();
	}

	const double maxDeltaR;
};

struct matchSort_deltaRdeltaPtRel
{
	matchSort_deltaRdeltaPtRel(const double dR = 0.3, const double dPtRel = 1e9) : maxDeltaR(dR), maxDeltaPtRel(dPtRel) {};

	template<typename T1, typename T2>
	double operator()(const T1 &obj1, const T2 &obj2) const
	{
		const double val1 = ROOT::Math::VectorUtil::DeltaR(obj1.p4, obj2.p4);
		const double val2 = std::abs(obj1.p4.pt() - obj2.p4.pt()) / obj2.p4.pt();
		return (val1 < maxDeltaR && val2 < maxDeltaPtRel) ? val1 : std::numeric_limits<double>::quiet_NaN();
	}

	const double maxDeltaR, maxDeltaPtRel;
};

template<typename T1, typename T2>
std::vector<int> matchSort_Matrix(const std::vector<T1> &base, const size_t base_size,
	const std::vector<T2> &target, const size_t target_size, const double dR = 0.3)
{
	static matchSort_deltaR metric(dR);
	return matchSort_Matrix(base, base_size, target, target_size, metric);
}

// return vector has the size of the target (retvec.size()==target.size())
// retvec[idx] gives the index for the object in base that matches target[idx]
//  ===>  target[idx] matches base[retvec[idx]]
// note: assuming above that target_size = target.size()
template<typename T1, typename T2, typename TMetricClass>
std::vector<int> matchSort_Matrix(const std::vector<T1> &base, const size_t base_size,
	const std::vector<T2> &target, const size_t target_size, const TMetricClass &metricFct)
{
	double **match_metric = nullptr;
	std::vector<int> result(target_size, -1);

	// Build m x n Matrix with dR
	match_metric = new double*[base_size];
	for (unsigned int i = 0; i < base_size; ++i)
	{
		const T1 &jet_i = base[i];
		match_metric[i] = new double[target_size];
		for (unsigned int j = 0; j < target_size; ++j)
		{
			const T2 &jet_j = target[j];
			match_metric[i][j] = metricFct(jet_i, jet_j);
		}
	}
	//printMatrix(match_metric, base_size, target_size);

	// Find matching index
	for (size_t t = 0; t < target_size; ++t)
	{
		// Find matrix entry with smallest metric
		int bestBase = -1;
		unsigned int bestTarget = -1u;
		double best_m = std::numeric_limits<double>::quiet_NaN();
		for (unsigned int idxBase = 0; idxBase < base_size; ++idxBase)
			for (unsigned int idxTarget = 0; idxTarget < target_size; ++idxTarget)
			{
				const double m = match_metric[idxBase][idxTarget];
				if ((!std::isnan(m)) && ((m < best_m) || std::isnan(best_m)))
				{
					bestBase = static_cast<int>(idxBase);
					bestTarget = idxTarget;
					best_m = m;
				}
			}

		// Add matched connection, if all objects are matched => EXIT
		if (std::isnan(best_m))
			break;
		// Invalidate metrics of matched objects
		for (unsigned int idxBase = 0; idxBase < base_size; ++idxBase)
			match_metric[idxBase][bestTarget] = std::numeric_limits<double>::quiet_NaN();
		for (unsigned int idxTarget = 0; idxTarget < target_size; ++idxTarget)
			match_metric[bestBase][idxTarget] = std::numeric_limits<double>::quiet_NaN();
		result[bestTarget] = static_cast<int>(bestBase);
	}

	for (unsigned int i = 0; i < base_size; ++i)
		delete [] match_metric[i];
	delete [] match_metric;

	return result;
}

#endif
