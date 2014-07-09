
#pragma once

#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"

#include "Kappa/DataFormats/interface/Kappa.h"

#define USE_JEC
#include "KappaTools/RootTools/JECTools.h"

#include "Artus/Core/interface/ProducerBase.h"
#include "Artus/Utility/interface/Utility.h"

/**
   \brief Producer for Jet Energy Correction (JEC)
   
   Required config tags:
   - JetEnergyCorrectionParameters (files containing the correction parameters in the right order)
   - JetEnergyCorrectionUncertaintyParameters (default: empty)
   - JetEnergyCorrectionUncertaintyShift (default 0.0)
   
   Required packages (unfortunately, nobody knows a tag):
   git cms-addpkg CondFormats/JetMETObjects
   
   Documentation:
   https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#JetEnCorFWLite
*/


template<class TTypes, class TJet>
class JetEnergyCorrectionProducerBase: public ProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	JetEnergyCorrectionProducerBase(std::vector<TJet>* event_type::*jets, std::vector<TJet> product_type::*correctedJets) :
		ProducerBase<TTypes>(),
		m_jetsMember(jets),
		m_correctedJetsMember(correctedJets)
	{
	}

	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE
	{
		ProducerBase<TTypes>::Init(settings);
		
		// load correction parameters
		LOG(DEBUG) << "\tLoading JetCorrectorParameters from files...";
		std::vector<JetCorrectorParameters> jecParameters;
		for (std::vector<std::string>::const_iterator jecParametersFile = settings.GetJetEnergyCorrectionParameters().begin();
		     jecParametersFile != settings.GetJetEnergyCorrectionParameters().end(); ++jecParametersFile)
		{
			jecParameters.push_back(JetCorrectorParameters(*jecParametersFile));
			LOG(DEBUG) << "\t\t" << *jecParametersFile;
		}
		factorizedJetCorrector = new FactorizedJetCorrector(jecParameters);
		
		// initialise uncertainty calculation
		LOG(DEBUG) << "\tLoading JetCorrectionUncertainty from files...";
		if (! settings.GetJetEnergyCorrectionUncertaintyParameters().empty())
		{
			jetCorrectionUncertainty = new JetCorrectionUncertainty(settings.GetJetEnergyCorrectionUncertaintyParameters());
			LOG(DEBUG) << "\t\t" << settings.GetJetEnergyCorrectionUncertaintyParameters();
		}
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		// creates copies of jets in event
		(product.*m_correctedJetsMember).resize((event.*m_jetsMember)->size());
		for (size_t jetIndex = 0; jetIndex < (event.*m_jetsMember)->size(); ++jetIndex)
		{
			(product.*m_correctedJetsMember)[jetIndex] = (*(event.*m_jetsMember))[jetIndex];
		}
		
		// apply corrections and uncertainty shift
		correctJets(&(product.*m_correctedJetsMember), factorizedJetCorrector, jetCorrectionUncertainty,
		            event.m_jetArea->median, event.m_vertexSummary->nVertices, -1,
		            settings.GetJetEnergyCorrectionUncertaintyShift());
	}


private:
	std::vector<TJet>* event_type::*m_jetsMember;
	std::vector<TJet> product_type::*m_correctedJetsMember;
	
	FactorizedJetCorrector* factorizedJetCorrector = 0;
	JetCorrectionUncertainty* jetCorrectionUncertainty = 0;
};



/**
   \brief Producer for Jet Energy Correction (JEC)
   
   Operates on the vector event.m_jets and product::m_correctedJets.
*/
template<class TTypes>
class JetEnergyCorrectionProducer: public JetEnergyCorrectionProducerBase<TTypes, KDataPFJet>
{
public:
	JetEnergyCorrectionProducer() :
		JetEnergyCorrectionProducerBase<TTypes, KDataPFJet>(&TTypes::event_type::m_jets,
	                                                        &TTypes::product_type::m_correctedJets)
	{
	};
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "jet_energy_correction";
	}
};



/**
   \brief Producer for Jet Energy Correction (JEC)
   
   Operates on the vector event.m_tjets and product::m_correctedTaggedJets.
*/
template<class TTypes>
class TaggedJetEnergyCorrectionProducer: public JetEnergyCorrectionProducerBase<TTypes, KDataPFTaggedJet>
{
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	TaggedJetEnergyCorrectionProducer() :
		JetEnergyCorrectionProducerBase<TTypes, KDataPFTaggedJet>(&TTypes::event_type::m_tjets,
		                                                          &TTypes::product_type::m_correctedTaggedJets)
	{
	};
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "tagged_jet_energy_correction";
	}
};


