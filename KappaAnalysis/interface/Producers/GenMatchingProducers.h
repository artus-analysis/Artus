
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"


/** Abstract Producer class for trigger matching valid objects
 *
 *	Needs to run after the valid object producers.
 */
template<class TTypes, class TValidObject>
class GenMatchingProducerBase: public ProducerBase<TTypes>
{

public:
	
	enum class TauDecayMode : int
	{
		NONE = -1,
		E   = 0,
		M   = 1,
		T   = 2
	};

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	GenMatchingProducerBase(std::map<TValidObject*, const KDataLV*> product_type::*genMatchedObjects,
	                            std::vector<TValidObject*> product_type::*validObjects,
	                            std::vector<TValidObject*> product_type::*invalidObjects,
	                            TauDecayMode tauDecayMode,
	                            float (setting_type::*GetDeltaRGenMatchingObjects)(void) const,
	                            bool (setting_type::*GetInvalidateNonGenMatchingObjects)(void) const
	                            ) :
		m_genMatchedObjects(genMatchedObjects),
		m_validObjects(validObjects),
		m_invalidObjects(invalidObjects),
		tauDecayMode(tauDecayMode),
		GetDeltaRGenMatchingObjects(GetDeltaRGenMatchingObjects),
		GetInvalidateNonGenMatchingObjects(GetInvalidateNonGenMatchingObjects)
	{
	}

	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE 
	{
		ProducerBase<TTypes>::Init(settings);
		LambdaNtupleConsumer<TTypes>::Quantities["ratioGenMatched"] = [](event_type const & event, product_type const & product)
		{
			return product.m_ratioGenMatched;
		};
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		assert(event.m_genTaus);
		float ratioGenMatched = 0;
		if ((settings.*GetDeltaRGenMatchingObjects)() > 0.0)
		{
			// loop over all valid objects to check
			for (typename std::vector<TValidObject*>::iterator validObject = (product.*m_validObjects).begin();
			     validObject != (product.*m_validObjects).end();)
			{
				bool objectMatched = false;
				float deltaR = 0;
				// loop over all genTaus
				for (typename std::vector<KDataGenTau>::const_iterator genTau = event.m_genTaus->begin();
			     !objectMatched && genTau != event.m_genTaus->end();++genTau) 
				{
					// only use genTaus that will decay into comparable particles
					if (this->MatchDecayMode(*genTau,tauDecayMode))
					{
						deltaR = ROOT::Math::VectorUtil::DeltaR((*validObject)->p4, genTau->p4_vis);
						if(deltaR<(settings.*GetDeltaRGenMatchingObjects)())
						{
							(product.*m_genMatchedObjects)[*validObject] = &(*genTau);
							ratioGenMatched += 1./(product.*m_validObjects).size();
							objectMatched = true;
						}
					}
				}
				// invalidate the object if the trigger has not matched
				if ((! objectMatched) && (settings.*GetInvalidateNonGenMatchingObjects)())
				{
					(product.*m_invalidObjects).push_back(*validObject);
					validObject = (product.*m_validObjects).erase(validObject);
				}
				else
				{
					++validObject;
				}
			}
			// preserve sorting of invalid objects
			if ((settings.*GetInvalidateNonGenMatchingObjects)())
			{
				std::sort((product.*m_invalidObjects).begin(), (product.*m_invalidObjects).end(),
				          [](TValidObject const* object1, TValidObject const* object2) -> bool
				          { return object1->p4.Pt() > object2->p4.Pt(); });
			}
		}
		product.m_ratioGenMatched = ratioGenMatched;
	}

	virtual bool MatchDecayMode(KDataGenTau const &genTau, TauDecayMode tauDecayMode) const
	{
		bool decayModeMatched = false;
		switch(tauDecayMode) 
  		{	
			case TauDecayMode::NONE: LOG(ERROR) << "tauDecayMode == NONE";
			case TauDecayMode::E: if (genTau.isElectronicDecay()) decayModeMatched = true;
    			case TauDecayMode::M: if (genTau.isMuonicDecay()) decayModeMatched = true;
			case TauDecayMode::T: if (genTau.isHadronicDecay()) decayModeMatched = true;
		};
		return decayModeMatched;
	}
	
private:
	std::map<TValidObject*,const KDataLV*> product_type::*m_genMatchedObjects;
	std::vector<TValidObject*> product_type::*m_validObjects;
	std::vector<TValidObject*> product_type::*m_invalidObjects;
	TauDecayMode tauDecayMode;
	float (setting_type::*GetDeltaRGenMatchingObjects)(void) const;
	bool (setting_type::*GetInvalidateNonGenMatchingObjects)(void) const;
	
	std::map<size_t, std::vector<std::string> > m_objectTriggerFiltersByIndex;
	std::map<std::string, std::vector<std::string> > m_objectTriggerFiltersByHltName;

};


/** Producer for gen matched electrons
 *  Required config tags:
 *  - DeltaRGenMatchingElectrons (default provided)
 *  - InvalidateNonMatchingElectrons (default provided)
 */
template<class TTypes>
class ElectronGenMatchingProducer: public GenMatchingProducerBase<TTypes, KDataElectron>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "ElectronGenMatchingProducer";
	}

	ElectronGenMatchingProducer() :
		GenMatchingProducerBase<TTypes, KDataElectron>(&product_type::m_genMatchedElectrons,
		                                                   &product_type::m_validElectrons,
		                                                   &product_type::m_invalidElectrons,
		                                                   ElectronGenMatchingProducer::TauDecayMode::E,
		                                                   &setting_type::GetDeltaRGenMatchingElectrons,
		                                                   &setting_type::GetInvalidateNonGenMatchingElectrons
		                                                   )
	{
	}

};


/** Producer for gen matched muons
 *  Required config tags:
 *  - DeltaRGenMatchingMuons (default provided)
 *  - InvalidateNonMatchingMuons (default provided)
 */
template<class TTypes>
class MuonGenMatchingProducer: public GenMatchingProducerBase<TTypes, KDataMuon>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "MuonGenMatchingProducer";
	}
	
	MuonGenMatchingProducer() :
		GenMatchingProducerBase<TTypes, KDataMuon>(&product_type::m_genMatchedMuons,
		                                               &product_type::m_validMuons,
		                                               &product_type::m_invalidMuons,
		                                               MuonGenMatchingProducer::TauDecayMode::M,
		                                               &setting_type::GetDeltaRGenMatchingMuons,
		                                               &setting_type::GetInvalidateNonGenMatchingMuons
		                                               )
	{
	}

};


/** Producer for gen matched taus
 *  Required config tags:
 *  - DeltaRGenMatchingTaus (default provided)
 *  - InvalidateNonMatchingTaus (default provided)
 */
template<class TTypes>
class TauGenMatchingProducer: public GenMatchingProducerBase<TTypes, KDataPFTau>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "TauGenMatchingProducer";
	}
	
	TauGenMatchingProducer() :
		GenMatchingProducerBase<TTypes, KDataPFTau>(&product_type::m_genMatchedTaus,
		                                                &product_type::m_validTaus,
		                                                &product_type::m_invalidTaus,
		                                                TauGenMatchingProducer::TauDecayMode::T,
		                                                &setting_type::GetDeltaRGenMatchingTaus,
		                                                &setting_type::GetInvalidateNonGenMatchingTaus)
	{
	}

};

