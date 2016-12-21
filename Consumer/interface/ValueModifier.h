
#pragma once

#include <cstddef>
#include <cassert>
#include <vector>

class Hist1D;
class Hist2D;
class Profile2d;

class ValueModifier {
public:
	virtual ~ValueModifier();
	virtual void applyHistBeforeCreation(Hist1D * h1, size_t index);
	virtual void applyProfileBeforeCreation(Profile2d * h1, size_t index);
	virtual void applyHist2DBeforeCreation(Hist2D * h1, size_t index);
	// virtual void applyProfile(Profile2 * h1, size_t index);
};

class ValueModifierRange: public ValueModifier {
public:
	ValueModifierRange(float l, float u);
	virtual ~ValueModifierRange();
	void applyHistBeforeCreation(Hist1D * h1, size_t index)	override;
	void applyProfileBeforeCreation(Profile2d * h1, size_t index) override;
	void applyHist2DBeforeCreation(Hist2D * h1, size_t index) override;

private:
	const float m_binLower;
	const float m_binUpper;
};

class ValueModifierBinCount: public ValueModifier {
public:
	explicit ValueModifierBinCount(size_t bc);
	virtual ~ValueModifierBinCount();
	void applyHistBeforeCreation(Hist1D * h1, size_t index) override;
	void applyProfileBeforeCreation(Profile2d * h1, size_t index) override;
	void applyHist2DBeforeCreation(Hist2D * h1, size_t index) override;

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
	static ValueModifiers getPtModifier(float maxPt);
	static ValueModifiers getPtModifier(float minPt, float maxPt);
	static ValueModifiers getMassModifier(float lowMass, float upperMass);
	static ValueModifiers getRunModifier();

};
