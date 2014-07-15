
#pragma once

#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"
#include "Artus/Core/interface/ProductBase.h"
#include "Artus/Utility/interface/SafeMap.h"
#include "Artus/Utility/interface/Utility.h"


/**
   \brief Producer for tau energy scale corrections.
   
   (No correction implemented yet.)
   
   Required config tags
   - TauEnergyCorrection (possible value: summer2013)
*/
template<class TTypes>
class TauEnergyCorrectionProducer: public ProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	enum class TauEnergyCorrection : int
	{
		NONE  = -1,
		SUMMER2013 = 0,
	};
	static TauEnergyCorrection ToTauEnergyCorrection(std::string const& tauEnergyCorrection)
	{
		if (tauEnergyCorrection == "summer2013") return TauEnergyCorrection::SUMMER2013;
		else return TauEnergyCorrection::NONE;
	}

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "tau_energy_correction";
	}
	
	virtual void Init(setting_type const& settings)  ARTUS_CPP11_OVERRIDE
	{
		ProducerBase<TTypes>::Init(settings);
		
		tauEnergyCorrection = ToTauEnergyCorrection(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetTauEnergyCorrection())));
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		// create a copy of all taus in the event
		product.m_correctedTaus.clear();
		product.m_correctedTaus.resize(event.m_taus->size());
		size_t tauIndex = 0;
		for (KDataPFTaus::const_iterator tau = event.m_taus->begin();
			 tau != event.m_taus->end(); ++tau)
		{
			product.m_correctedTaus[tauIndex] = std::shared_ptr<KDataPFTau>(new KDataPFTau(*tau));
			++tauIndex;
		}
		
		// perform corrections on copied taus
		for (std::vector<std::shared_ptr<KDataPFTau> >::iterator tau = product.m_correctedTaus.begin();
			 tau != product.m_correctedTaus.end(); ++tau)
		{
			// No general correction available
		
			// perform possible analysis-specific corrections
			AdditionalCorrections(tau->get(), event, product, settings);
		}
		
		// sort vectors of corrected taus by pt
		std::sort(product.m_correctedTaus.begin(), product.m_correctedTaus.end(),
		          [](std::shared_ptr<KDataPFTau> tau1, std::shared_ptr<KDataPFTau> tau2) -> bool
		          { return tau1->p4.Pt() > tau2->p4.Pt(); });
	}


protected:

	// Can be overwritten for analysis-specific use cases
	virtual void AdditionalCorrections(KDataPFTau* tau, event_type const& event,
	                                   product_type& product, setting_type const& settings) const
	{
	}


private:
	TauEnergyCorrection tauEnergyCorrection;

};

