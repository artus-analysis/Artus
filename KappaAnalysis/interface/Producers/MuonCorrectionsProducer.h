
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"

#include "Artus/Utility/interface/RoccoR.h"
#include "Artus/Utility/interface/rochcor2015.h"
#include "Artus/Utility/interface/rochcor2016.h"

/**
   \brief Producer for muon four momentum corrections.
   \Rochester Corrections are included
*/
class MuonCorrectionsProducer: public KappaProducerBase
{

public:
	std::string GetProducerId() const override;

	void Init(setting_type const& settings) override;

	void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override;

	public:
	enum class MuonEnergyCorrection : int
	{
		NONE  = -1,
		FALL2015 = 0,
		ROCHCORR2015 = 1,
		ROCHCORR2016 = 2
	};
	static MuonEnergyCorrection ToMuonEnergyCorrection(std::string const& muonEnergyCorrection)
	{
		if (muonEnergyCorrection == "fall2015") return MuonEnergyCorrection::FALL2015;
		else if (muonEnergyCorrection == "rochcorr2015") return MuonEnergyCorrection::ROCHCORR2015;
		else if (muonEnergyCorrection == "rochcorr2016") return MuonEnergyCorrection::ROCHCORR2016;
		else return MuonEnergyCorrection::NONE;
	}

protected:

	// Can be overwritten for analysis-specific use cases
	virtual void AdditionalCorrections(KMuon* muon, KappaEvent const& event,
	                                   KappaProduct& product, KappaSettings const& settings) const;
private:
	MuonEnergyCorrection muonEnergyCorrection;
	rochcor2015 *rmcor2015;
	rochcor2016 *rmcor2016;
};
