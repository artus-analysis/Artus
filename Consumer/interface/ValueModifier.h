/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <cstddef>
#include <cassert>
#include <vector>

class Hist1D;
class Hist2D;
class Profile2d;

class ValueModifier {
public:
	virtual ~ValueModifier() {
	}

	virtual void applyHistBeforeCreation(Hist1D * h1, size_t index) {
		assert(false);
	}

	virtual void applyProfileBeforeCreation(Profile2d * h1, size_t index) {
		assert(false);
	}

	virtual void applyHist2DBeforeCreation(Hist2D * h1, size_t index) {
		assert(false);
	}

	/*	virtual void applyProfile(Profile2 * h1, size_t index) {
	 assert(false);
	 }
	 */
};

class ValueModifierRange: public ValueModifier {
public:
	ValueModifierRange(float l, float u) :
			m_binLower(l), m_binUpper(u) {

	}

	virtual ~ValueModifierRange() {
	}

	virtual void applyHistBeforeCreation(Hist1D * h1, size_t index)	override;

	virtual void applyProfileBeforeCreation(Profile2d * h1, size_t index) override;

	virtual void applyHist2DBeforeCreation(Hist2D * h1, size_t index) override;

private:
	const float m_binLower;
	const float m_binUpper;
};

class ValueModifierBinCount: public ValueModifier {
public:
	explicit ValueModifierBinCount(size_t bc) : m_binCount(bc) {
	}

	virtual ~ValueModifierBinCount() {
	}

	virtual void applyHistBeforeCreation(Hist1D * h1, size_t index) override;

	virtual void applyProfileBeforeCreation(Profile2d * h1, size_t index) override;

	virtual void applyHist2DBeforeCreation(Hist2D * h1, size_t index) override;

private:
	const size_t m_binCount;
};

typedef std::vector<ValueModifier *> ValueModifiers;

class DefaultModifiers {
public:
	static ValueModifiers getGenericModifier(float, float, size_t binCount = 500);
	static ValueModifiers getRModifier(size_t binCount = 500);
	static ValueModifiers getZModifier(size_t binCount = 500);
	static ValueModifiers getEtaModifier(size_t binCount = 100);
	static ValueModifiers getThetaModifier();
	static ValueModifiers getPtModifier(float);
	static ValueModifiers getPtModifier(float, float);
	static ValueModifiers getMassModifier(float, float);

	static ValueModifiers getRunModifier();

};
