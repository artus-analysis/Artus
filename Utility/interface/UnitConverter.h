#pragma once

#include "Artus/Utility/interface/Utility.h"

/*
	A simple unit converter with constants that are used to convert units from one system into another one.
	This ensures that once a conversion is implemented, it can be used without worries about
	forgetting small details.

	Every Function takes 3 arguments. The value to be converted, its metric prefix and the desired
	metric prefix of the output.
	Default arguments are used to make it more user friendly and adhere to the standard way the
	quantities are stored in the root files (e.g. Momentum is assumed to be stored as GeV).
*/

class UnitConverter {
public:
	typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float> > RMFLV;
	typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > CartesianRMFLV;

	// values set in .cc file
	static const double eQ;
	static const double c;
	static const double hbar;

	// defined in .cc file
	static double MomentumNU2SI(double, double MetricPrefixIn = 1e9, double MetricPrefixOut = 1);
	static double EnergyNU2SI(double, double MetricPrefixIn = 1e9, double MetricPrefixOut = 1);
	static double LengthNU2SI(double, double MetricPrefixIn = 1e-9, double MetricPrefixOut = 1e-2);
	static double TimeNU2SI(double, double MetricPrefixIn = 1e9, double MetricPrefixOut = 1);
	static double VelocityNU2SI(double, double MetricPrefixIn = 1, double MetricPrefixOut = 1);
	static double MassNU2SI(double, double MetricPrefixIn = 1e9, double MetricPrefixOut = 1);
	static double MagneticFieldNU2SI(double , double MetricPrefixIn=1, double MetricPrefixOut=1);

	static double MomentumSI2NU(double, double MetricPrefixIn = 1, double MetricPrefixOut = 1e9);
	static double EnergySI2NU(double, double MetricPrefixIn = 1, double MetricPrefixOut = 1e9);
	static double LengthSI2NU(double, double MetricPrefixIn = 1e-2, double MetricPrefixOut = 1e-9);
	static double TimeSI2NU(double, double MetricPrefixIn = 1, double MetricPrefixOut = 1e9);
	static double VelocitySI2NU(double, double MetricPrefixIn = 1, double MetricPrefixOut = 1);
	static double MassSI2NU(double, double MetricPrefixIn = 1, double MetricPrefixOut = 1e9);
	static double MagneticFieldSI2NU(double , double MetricPrefixIn=1, double MetricPrefixOut=1);
};
