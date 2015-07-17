/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#include "Artus/Consumer/interface/ValueModifier.h"

#include "Artus/Consumer/interface/Hist1D.h"

void ValueModifierRange::applyHistBeforeCreation(Hist1D * h1, size_t index) {
	h1->m_dBinLower = this->m_binLower;
	h1->m_dBinUpper = this->m_binUpper;
}

void ValueModifierBinCount::applyHistBeforeCreation(Hist1D * h1, size_t index) {
	h1->m_iBinCount = (int) m_binCount;
}

void ValueModifierRange::applyHist2DBeforeCreation(Hist2D * h1, size_t index) {
	/*assert((index == 0) || (index == 1));
	 if (index == 0) {
	 h1->m_dBinXLower = this->m_binLower;
	 h1->m_dBinXUpper = this->m_binUpper;
	 }
	 if (index == 1) {
	 h1->m_dBinYLower = this->m_binLower;
	 h1->m_dBinYUpper = this->m_binUpper;
	 }*/
}

void ValueModifierRange::applyProfileBeforeCreation(Profile2d * h1,
		size_t index) {/*
		 assert((index == 0) || (index == 1));
		 if (index == 0) {
		 h1->m_dBinLowerX = this->m_binLower;
		 h1->m_dBinUpperX = this->m_binUpper;
		 }
		 if (index == 1) {
		 //ignore Y bin settings, because there is no binning in y direction
		 //for 2d profiles
		 }*/
}

void ValueModifierBinCount::applyProfileBeforeCreation(Profile2d * h1,
		size_t index) {
	/*
	 assert((index == 0) || (index == 1));
	 if (index == 0) {
	 h1->m_iBinCountX = this->m_binCount;
	 }
	 if (index == 1) {
	 //ignore Y bin settings, because there is no binning in y direction
	 //for 2d profiles
	 }*/
}

void ValueModifierBinCount::applyHist2DBeforeCreation(Hist2D * h1,
		size_t index) {
	/*assert((index == 0) || (index == 1));
	 if (index == 0) {
	 h1->m_iBinXCount = this->m_binCount;
	 }
	 if (index == 1) {
	 h1->m_iBinYCount = this->m_binCount;
	 }*/
}

ValueModifiers DefaultModifiers::getRModifier(size_t binCount) {
	ValueModifiers modList;

	modList.push_back(new ValueModifierRange(0.0f, 100.f));
	modList.push_back(new ValueModifierBinCount(binCount));

	return modList;
}

ValueModifiers DefaultModifiers::getZModifier(size_t binCount) {
	ValueModifiers modList;

	modList.push_back(new ValueModifierRange(-100.0f, 100.0f));
	modList.push_back(new ValueModifierBinCount(binCount));

	return modList;
}

ValueModifiers DefaultModifiers::getEtaModifier(size_t binCount) {
	ValueModifiers modList;

	modList.push_back(new ValueModifierRange(-2.5f, 2.5f));
	modList.push_back(new ValueModifierBinCount(binCount));

	return modList;
}

ValueModifiers DefaultModifiers::getPtModifier(float maxPt = 200.0f) {
	return DefaultModifiers::getPtModifier(0.0, maxPt);
}

ValueModifiers DefaultModifiers::getPtModifier(float minPt, float maxPt) {
	ValueModifiers modList;

	modList.push_back(new ValueModifierRange(minPt, maxPt));
	modList.push_back(new ValueModifierBinCount(200));

	return modList;
}

ValueModifiers DefaultModifiers::getThetaModifier() {
	ValueModifiers modList;

	modList.push_back(new ValueModifierRange(0.0, 3.2f));
	modList.push_back(new ValueModifierBinCount(200));

	return modList;
}

ValueModifiers DefaultModifiers::getMassModifier(float lowMass,
		float upperMass) {
	ValueModifiers modList;

	modList.push_back(new ValueModifierRange(lowMass, upperMass));
	modList.push_back(new ValueModifierBinCount(200));

	return modList;
}

ValueModifiers  DefaultModifiers::getGenericModifier(float up, float down, size_t binCount ) {
        ValueModifiers modList;

        modList.push_back(new ValueModifierRange(up, down));
        modList.push_back(new ValueModifierBinCount(binCount));

        return modList;
}

ValueModifiers DefaultModifiers::getRunModifier() {
	ValueModifiers modList;

	modList.push_back(new ValueModifierRange(190000, 220000));
	modList.push_back(new ValueModifierBinCount(500));

	return modList;
}

