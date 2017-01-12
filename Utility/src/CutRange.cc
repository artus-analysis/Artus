
#include "Artus/Utility/interface/CutRange.h"
#include <cmath> 

CutRange::CutRange(double min, double max, double epsilon) :
	m_min(min),
	m_max(max),
	m_epsilon(epsilon)
{
	m_pointRange = Equals(min, max);
}
	
CutRange CutRange::LowerThresholdCut(double min)
{
	return CutRange(min);
}

CutRange CutRange::UpperThresholdCut(double max)
{
	return CutRange(std::numeric_limits<double>::lowest(), max);
}

CutRange CutRange::EqualsCut(double cut)
{
	return CutRange(cut, cut);
}

bool CutRange::IsInRange(double value)
{
	bool isInRange = false;
	if (m_pointRange) {
		isInRange = Equals(value, m_min);
	}
	else {
		isInRange = ((m_min <= value) && (value <= m_max));
	}
	return isInRange;
}

bool CutRange::Equals(double valueA, double valueB)
{
	double deltaOverMean = 2.0 * std::abs(valueA-valueB) / std::abs(valueA+valueB);
	return (deltaOverMean < m_epsilon);
}

