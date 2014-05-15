
#pragma once

#include <limits.h>

#include "Artus/Utility/interface/ArtusLogging.h"


class CutRange {
public:
	CutRange(double min=std::numeric_limits<double>::lowest(),
	         double max=std::numeric_limits<double>::max(),
	         double epsilon=0.001);
	
	static CutRange LowerThresholdCut(double min);
	static CutRange UpperThresholdCut(double max);
	static CutRange EqualsCut(double cut);
	
	bool IsInRange(double value);

private:
	double m_min, m_max, m_epsilon;
	bool m_pointRange;

	bool Equals(double valueA, double valueB);
};

