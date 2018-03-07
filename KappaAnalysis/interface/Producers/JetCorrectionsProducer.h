
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
#include "Artus/KappaTools/interface/JECTools.h"

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/Utility/interface/Utility.h"

/**
   \brief Producer for jet corrections (mainly JEC)
   
   Required config tags:
   - JetEnergyCorrectionParameters (files containing the correction parameters in the right order)
   - JetEnergyCorrectionUncertaintyParameters (default: empty)
   - JetEnergyCorrectionUncertaintySource (default "")
   - JetEnergyCorrectionUncertaintyShift (default 0.0)
   
   Required packages (unfortunately, nobody knows a tag):
   git cms-addpkg CondFormats/JetMETObjects
   
   Documentation:
   https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#JetEnCorFWLite

   TODO: the code can be moved to a .cc file, if there are no external users using the TJet templated 
         version
*/


template<class TJet>
class JetCorrectionsProducerBase: public KappaProducerBase
{

public:
	
	JetCorrectionsProducerBase(std::vector<TJet>* event_type::*jets,
	                           std::vector<std::shared_ptr<TJet> > product_type::*correctedJets) :
		KappaProducerBase(),
		m_basicJetsMember(jets),
		m_correctedJetsMember(correctedJets)
	{
	}
	
	~JetCorrectionsProducerBase()
	{
		if (factorizedJetCorrector != nullptr)
		{
			delete factorizedJetCorrector;
		}
		if (jetCorrectionUncertainty != nullptr)
		{
			delete jetCorrectionUncertainty;
		}
	}

	void Init(setting_type const& settings, metadata_type& metadata) override
	{
		KappaProducerBase::Init(settings, metadata);
		
		// load correction parameters
		LOG(DEBUG) << "\tLoading JetCorrectorParameters from files...";
		std::vector<JetCorrectorParameters> jecParameters;
		for (std::vector<std::string>::const_iterator jecParametersFile = settings.GetJetEnergyCorrectionParameters().begin();
		     jecParametersFile != settings.GetJetEnergyCorrectionParameters().end(); ++jecParametersFile)
		{
			LOG(DEBUG) << "\t\t" << *jecParametersFile;
			jecParameters.push_back(JetCorrectorParameters(*jecParametersFile));
		}
		if (jecParameters.size() > 0)
		{
			factorizedJetCorrector = new FactorizedJetCorrector(jecParameters);
		}
		
		// initialise uncertainty calculation
		LOG(DEBUG) << "\tLoading JetCorrectionUncertainty from files...";
		if ((! settings.GetJetEnergyCorrectionUncertaintyParameters().empty()) &&
		    (settings.GetJetEnergyCorrectionUncertaintyShift() != 0.0))
		{
			JetCorrectorParameters jecUncertaintyParameters;
			if (!settings.GetJetEnergyCorrectionUncertaintySource().empty()) {
				LOG(DEBUG) << "\t\t" << settings.GetJetEnergyCorrectionUncertaintyParameters() << " (" << settings.GetJetEnergyCorrectionUncertaintySource() << ")";
				jecUncertaintyParameters = JetCorrectorParameters(
						settings.GetJetEnergyCorrectionUncertaintyParameters(),
						settings.GetJetEnergyCorrectionUncertaintySource()
				);
				jetCorrectionUncertainty = new JetCorrectionUncertainty(jecUncertaintyParameters);
			}
			else {
				LOG(DEBUG) << "\t\t" << settings.GetJetEnergyCorrectionUncertaintyParameters();
				jecUncertaintyParameters = JetCorrectorParameters(settings.GetJetEnergyCorrectionUncertaintyParameters());
				jetCorrectionUncertainty = new JetCorrectionUncertainty(jecUncertaintyParameters);
			}
			if ((!jecUncertaintyParameters.isValid()) || (jecUncertaintyParameters.size() == 0))
			{
				LOG(FATAL) << "Invalid definition " << settings.GetJetEnergyCorrectionUncertaintySource() 
				           << " in file " << settings.GetJetEnergyCorrectionUncertaintyParameters();
			}
		}
	}

	void Produce(event_type const& event, product_type& product,
	             setting_type const& settings, metadata_type const& metadata) const override
	{
		assert((event.*m_basicJetsMember));
		assert(event.m_pileupDensity);
		assert(event.m_vertexSummary);
		
		// create a copy of all jets in the event (first temporarily for the JEC)
		(product.*m_correctedJetsMember).clear();
		std::vector<TJet> correctJetsForJecTools((event.*m_basicJetsMember)->size());
		size_t jetIndex = 0;
		for (typename std::vector<TJet>::const_iterator jet = (event.*m_basicJetsMember)->begin();
			 jet != (event.*m_basicJetsMember)->end(); ++jet)
		{
			correctJetsForJecTools[jetIndex] = *jet;
			++jetIndex;
		}
		
		// apply jet energy corrections and uncertainty shift (if uncertainties are not to be splitted into individual contributions)
		float shift = settings.GetJetEnergyCorrectionSplitUncertainty() ? 0.0 : settings.GetJetEnergyCorrectionUncertaintyShift();
		correctJets(&correctJetsForJecTools, factorizedJetCorrector, jetCorrectionUncertainty,
		            event.m_pileupDensity->rho, event.m_vertexSummary->nVertices, -1,
		            shift);
		
		// create the shared pointers to store in the product
		(product.*m_correctedJetsMember).clear();
		(product.*m_correctedJetsMember).resize(correctJetsForJecTools.size());
		jetIndex = 0;
		for (typename std::vector<TJet>::const_iterator jet = correctJetsForJecTools.begin();
			 jet != correctJetsForJecTools.end(); ++jet)
		{
			(product.*m_correctedJetsMember)[jetIndex] = std::shared_ptr<TJet>(new TJet(*jet));
			product.m_originalJets[(product.*m_correctedJetsMember)[jetIndex].get()] = &(*jet);
			++jetIndex;
		}
		
		// perform corrections on copied jets
		for (typename std::vector<std::shared_ptr<TJet> >::iterator jet = (product.*m_correctedJetsMember).begin();
			 jet != (product.*m_correctedJetsMember).end(); ++jet)
		{
			// No general correction implemented
			
			// total uncertainty shifts
			if (jetCorrectionUncertainty != nullptr)
			{
				double uncertainty = 0.0;
				if ((std::abs((*jet)->p4.Eta()) < 5.2) && ((*jet)->p4.Pt() > 9.0))
				{
					jetCorrectionUncertainty->setJetEta((*jet)->p4.Eta());
					jetCorrectionUncertainty->setJetPt((*jet)->p4.Pt());
					uncertainty = jetCorrectionUncertainty->getUncertainty(true);
				}
				(*jet)->p4 = (*jet)->p4 * (1.0 + uncertainty * settings.GetJetEnergyCorrectionUncertaintyShift());
			}
			
			// perform possible analysis-specific corrections
			AdditionalCorrections(jet->get(), event, product, settings, metadata);
		}
		
		// sort vectors of corrected jets by pt
		std::sort((product.*m_correctedJetsMember).begin(), (product.*m_correctedJetsMember).end(),
		          [](std::shared_ptr<TJet> jet1, std::shared_ptr<TJet> jet2) -> bool
		          { return jet1->p4.Pt() > jet2->p4.Pt(); });
	}


protected:
	// Can be overwritten for analysis-specific use cases
	virtual void AdditionalCorrections(TJet* jet, event_type const& event, product_type& product,
	                                   setting_type const& settings, metadata_type const& metadata) const
	{
	}


private:
	std::vector<TJet>* event_type::*m_basicJetsMember;
	std::vector<std::shared_ptr<TJet> > product_type::*m_correctedJetsMember;

	FactorizedJetCorrector* factorizedJetCorrector = nullptr;
	JetCorrectionUncertainty* jetCorrectionUncertainty = nullptr;
};



/**
   \brief Producer for Jet Energy Correction (JEC)
   
   Operates on the vector event.m_basicJets and product::m_correctedJets.
*/
class JetCorrectionsProducer: public JetCorrectionsProducerBase<KBasicJet>
{
public:
	JetCorrectionsProducer();

	std::string GetProducerId() const override;
};



/**
   \brief Producer for Jet Energy Correction (JEC)
   
   Operates on the vector event.m_tjets and product::m_correctedTaggedJets.
*/
class TaggedJetCorrectionsProducer: public JetCorrectionsProducerBase<KJet>
{
public:

	TaggedJetCorrectionsProducer();
	
	std::string GetProducerId() const override;
};


