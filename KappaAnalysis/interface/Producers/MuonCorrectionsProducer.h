
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"

#include "Artus/Utility/interface/RoccoR.h"
#include "Artus/Utility/interface/rochcor2015.h"
#include "Artus/Utility/interface/RoccoR2016.h"
#include "TRandom3.h"

/**
   \brief Producer for muon four momentum corrections.
   \Rochester Corrections are included
*/
class MuonCorrectionsProducer: public KappaProducerBase
{

public:
	std::string GetProducerId() const override;

	void Init(setting_type const& settings, metadata_type& metadata) override;

	void Produce(event_type const& event, product_type& product,
	             setting_type const& settings, metadata_type const& metadata) const override;

	public:
	enum class MuonEnergyCorrection : int
	{
		NONE  = -1,
		FALL2015 = 0,
		ROCHCORR2015 = 1,
		ROCHCORR2016 = 2
	};
	static MuonEnergyCorrection ToMuonEnergyCorrection(std::string const& muonEnergyCorrection);

protected:

	// Can be overwritten for analysis-specific use cases
	virtual void AdditionalCorrections(KMuon* muon, event_type const& event, product_type& product,
	                                   setting_type const& settings, metadata_type const& metadata) const;
private:
	MuonEnergyCorrection muonEnergyCorrection;
	rochcor2015 *rmcor2015;
	RoccoR2016 *rmcor2016;
	TRandom3 *random;
};
