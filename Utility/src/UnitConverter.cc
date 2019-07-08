
#include "Artus/Utility/interface/UnitConverter.h"

typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float> > RMFLV;
typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > CartesianRMFLV;

const double UnitConverter::eQ = 1.60217662e-19; // elementary charge
const double UnitConverter::c = 2.99792458*1e8; // speed of light in m/s
const double UnitConverter::hbar = 1.054571800e-34; // reduced Planck constant in J/s

double UnitConverter::MomentumNU2SI(double p, double MetricPrefixIn, double MetricPrefixOut){
	return p * UnitConverter::eQ / UnitConverter::c * MetricPrefixIn / MetricPrefixOut;
}

double UnitConverter::EnergyNU2SI(double E, double MetricPrefixIn, double MetricPrefixOut){
	return E * UnitConverter::eQ * MetricPrefixIn / MetricPrefixOut;
}

double UnitConverter::LengthNU2SI(double l, double MetricPrefixIn, double MetricPrefixOut){
	return l / UnitConverter::eQ * UnitConverter::hbar * UnitConverter::c * MetricPrefixIn / MetricPrefixOut;
}

double UnitConverter::TimeNU2SI(double t, double MetricPrefixIn, double MetricPrefixOut){
	return t / UnitConverter::eQ * UnitConverter::hbar * MetricPrefixIn / MetricPrefixOut;
}

double UnitConverter::VelocityNU2SI(double v, double MetricPrefixIn, double MetricPrefixOut){
	return v * UnitConverter::c * MetricPrefixIn / MetricPrefixOut;
}

double UnitConverter::MassNU2SI(double m, double MetricPrefixIn, double MetricPrefixOut){
	return m * UnitConverter::eQ / UnitConverter::c / UnitConverter::c * MetricPrefixIn / MetricPrefixOut;
}

double UnitConverter::MomentumSI2NU(double p, double MetricPrefixIn, double MetricPrefixOut){
	return p * UnitConverter::c / UnitConverter::eQ * MetricPrefixIn / MetricPrefixOut;
}

double UnitConverter::EnergySI2NU(double E, double MetricPrefixIn, double MetricPrefixOut){
	return E / UnitConverter::eQ * MetricPrefixIn / MetricPrefixOut;
}

double UnitConverter::LengthSI2NU(double l, double MetricPrefixIn, double MetricPrefixOut){
	return l * UnitConverter::eQ / UnitConverter::hbar / UnitConverter::c * MetricPrefixIn / MetricPrefixOut;
}

double UnitConverter::TimeSI2NU(double t, double MetricPrefixIn, double MetricPrefixOut){
	return t * UnitConverter::eQ / UnitConverter::hbar * MetricPrefixIn / MetricPrefixOut;
}

double UnitConverter::VelocitySI2NU(double v, double MetricPrefixIn, double MetricPrefixOut){
	return v / UnitConverter::c * MetricPrefixIn / MetricPrefixOut;
}

double UnitConverter::MassSI2NU(double m, double MetricPrefixIn, double MetricPrefixOut){
	return m / UnitConverter::eQ * UnitConverter::c * UnitConverter::c * MetricPrefixIn / MetricPrefixOut;
}

