#pragma once

#include "TRandom3.h"
#include <TMath.h>
#include <iostream>

class BtagSF
{
  public:
	explicit BtagSF(int seed = 0);
	~BtagSF();

	bool isbtagged(double pt, float eta, float csv, Int_t jetflavor, bool isdata,
	               unsigned int btagsys, unsigned int mistagsys, bool is2012);
	double getSFb(double pt, unsigned int btagsys, bool is2012);
	double getSFc(double pt, unsigned int btagsys, bool is2012);
	double getSFl(double pt, float eta, unsigned int mistagsys, bool is2012);
	double getMistag(double pt, float eta);

	enum { kNo,	kDown, kUp }; // systematic variations

  private:
	TRandom3* randm;
};
