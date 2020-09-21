#pragma once

#include <algorithm>
#include <memory>
#include <random>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#include "JetMETCorrections/Modules/interface/JetResolution.h"

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/Utility/interface/Utility.h"


/**
   \brief Produce collection of "smeared" jets.
 
   The aim of this smearing is to account for the difference in jet energy 
   resolution (JER) between Monte Carlo simulation and Data.

   Required config tags:
   - JEREnabled (default: false)
   - JERFile (file containing jet energy resolution (JER) information)
   - JERScaleFactorFile (file containing JER scale factors)
   - JERVariation (default: 0; 1: up, 1: down)
   - JERUncertaintySource (default: "")
   - JERSeed (default: 37428479)
   - JERUseDeterministicSeed (default: true)
   - JERGenJetdRMax (max dR to match with genJet; default: 0.2)
   - JERdPtMaxFactor (max dPt (in resolution units) to match with genJet; default: 3)

   Documentation:
   https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyResolution
   https://github.com/cms-sw/cmssw/blob/CMSSW_10_2_X/PhysicsTools/PatUtils/interface/SmearedJetProducerT.h

   Input files:
   https://github.com/cms-jet/JRDatabase

   TODO: Is templated version with code in header file necessary?

*/

template<class TJet>
class SmearedJetProducerBase: public KappaProducerBase
{

public:
	SmearedJetProducerBase(std::vector<TJet>* KappaTypes::event_type::*jets,
	                       std::vector<std::shared_ptr<TJet> > KappaTypes::product_type::*correctedJets) :
		KappaProducerBase(),
		m_basicJetsMember(jets),
		m_correctedJetsMember(correctedJets),
		m_random_generator(nullptr),
		m_verbose(false) {}

	~SmearedJetProducerBase() {
	  
	  if (m_random_generator != nullptr) 
	    delete m_random_generator;

	}

	void Init(setting_type const& settings, metadata_type& metadata) override {

	  KappaProducerBase::Init(settings, metadata);

	  validJetsInput = KappaEnumTypes::ToValidJetsInput(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetValidJetsInput())));
	  m_enabled = settings.GetJEREnabled();

	  // load correction parameters
	  std::string resolutionFile = settings.GetJERFile();
	  LOG(DEBUG) << "Loading JetEnergyResolution from file "
		     << resolutionFile;
	  if (!resolutionFile.empty())
	    m_resolution_from_file.reset(new JME::JetResolution(resolutionFile));
	  else {
	    m_enabled = false;
	    LOG(DEBUG) << "Disabling JetEnergyResolution corrections due to unspecified file ";
	  }
	  std::string scaleFactorFile = settings.GetJERScaleFactorFile();
	  LOG(DEBUG) << "\tLoading JetEnergyResolution scale factor from file "
		     << scaleFactorFile;
	  if (!scaleFactorFile.empty())
	    m_scale_factor_from_file.reset(new JME::JetResolutionScaleFactor(scaleFactorFile));
	  else {
	    m_enabled = false;
	    LOG(DEBUG) << "Disabling JetEnergyResolution corrections due to unspecified file ";
	  }

	  std::uint32_t seed = settings.GetJERSeed();
	  if (m_random_generator == nullptr)
	    m_random_generator = new std::mt19937(seed);
	  else
	    m_random_generator->seed(seed);
	  m_useDeterministicSeed = settings.GetJERUseDeterministicSeed();

	  std::int32_t variation = settings.GetJERVariation();
	  m_nomVar = 1;
	  if (variation == 0)
	    m_systematic_variation = Variation::NOMINAL;
	  else if (variation == 1)
	    m_systematic_variation = Variation::UP;
	  else if (variation == -1)
	    m_systematic_variation = Variation::DOWN;
	  else if (variation == 101) {
	    m_systematic_variation = Variation::NOMINAL;
	    m_nomVar = 1;
	  } else if (variation == -101) {
	    m_systematic_variation = Variation::NOMINAL;
	    m_nomVar = -1;
	  } else {
	    LOG(DEBUG) << "Invalid value for 'JERVariation' parameter. Only -1, 0, 1 or 101, -101 are supported. Using 0, i.e. no variation";
	    m_systematic_variation = Variation::NOMINAL;
	  }

	  m_uncertaintySource = settings.GetJERUncertaintySource();

	  m_dR_max = settings.GetJERdRMax();
	  m_dPt_max_factor = settings.GetJERdPtMaxFactor();

	  if (m_verbose) {
	    std::cout<<"[SmearedJetProducer::Init]: params: "<<std::endl
		     <<"\tenable JER: "<<m_enabled<<std::endl
		     <<"\tresolution file: "<<resolutionFile<<std::endl
		     <<"\tSF file: "<<scaleFactorFile<<std::endl
		     <<"\tseed: "<<seed<<std::endl
		     <<"\tuse deterministic seed: "<<m_useDeterministicSeed<<std::endl
		     <<"\tdR_max: "<<m_dR_max<<std::endl
		     <<"\tdPt_max_factor: "<<m_dPt_max_factor<<std::endl
		     <<std::endl;
	  }
	}

	void Produce(event_type const& event, product_type& product,
	             setting_type const& settings, metadata_type const& metadata) const override {

	  assert((!m_enabled||(event.m_genJets)));
	  assert(event.m_pileupDensity);

	  // copy jets from m_basicJetsMember into m_correctedJetsMember, if m_correctedJetsMember is empty.
	  if (validJetsInput == KappaEnumTypes::ValidJetsInput::AUTO && ((product.*m_correctedJetsMember).size() == 0))
	  {
	    // LOG(DEBUG) << "Jets not corrected yet. Copying basic jets.";
	    (product.*m_correctedJetsMember).clear();
	    (product.*m_correctedJetsMember).resize((event.*m_basicJetsMember)->size());
	    size_t jetIndex = 0;
	    for (typename std::vector<TJet>::iterator jet = (event.*m_basicJetsMember)->begin(); jet != (event.*m_basicJetsMember)->end(); ++jet)
	    {
	      (product.*m_correctedJetsMember)[jetIndex] = std::make_shared<TJet> (*jet);
	      ++jetIndex;
	    }
	  }

	  JME::JetResolution resolution;
	  JME::JetResolutionScaleFactor resolution_sf;
	  if (m_enabled) {
	    resolution = *m_resolution_from_file;
	    resolution_sf = *m_scale_factor_from_file;

	    if (m_useDeterministicSeed) {
	      unsigned int runNum_uint = static_cast<unsigned int>(event.m_eventInfo->nRun);
	      unsigned int lumiNum_uint = static_cast<unsigned int>(event.m_eventInfo->nLumi);
	      unsigned int evNum_uint = static_cast<unsigned int>(event.m_eventInfo->nEvent);
	      //it assumes that jets are sorted in pt.
	      unsigned int jet0eta = uint32_t((product.*m_correctedJetsMember).empty() ? 0 : (product.*m_correctedJetsMember)[0]->p4.eta() / 0.01);
	      std::uint32_t seed = jet0eta + m_nomVar + (lumiNum_uint<<10) + (runNum_uint<<20) + evNum_uint;
	      m_random_generator->seed(seed);
	      if (m_verbose) {
		std::cout<<"[SmearedJetProducer::Produce]: deterministic seed: "
			 <<seed<<std::endl;
	      }
	    }
	  }


	  // smear each jet in the event
	  for (typename std::vector<std::shared_ptr<TJet> >::iterator jet = (product.*m_correctedJetsMember).begin();
	       jet != (product.*m_correctedJetsMember).end(); ++jet) {
	    if ((!m_enabled) || ((*jet)->p4.pt() == 0)) {
	      // Module disabled or invalid p4: Simply preserve input jet.
	      if (m_verbose) {
		std::cout<<"[SmearedJetProducer::Produce]: no smearing: "
			 <<"original Pt = "<<(*jet)->p4.pt()
			 <<std::endl;
	      }
	      continue;
	    }
	    double jet_resolution = resolution.getResolution(
	      {{JME::Binning::JetPt, (*jet)->p4.pt()}, {JME::Binning::JetEta, (*jet)->p4.eta()}, {JME::Binning::Rho, event.m_pileupDensity->rho}});
	    double jer_sf = resolution_sf.getScaleFactor(
	       {{JME::Binning::JetPt, (*jet)->p4.pt()}, {JME::Binning::JetEta, (*jet)->p4.eta()}},
	       m_systematic_variation,
	       m_uncertaintySource);
	    if (m_verbose) {
	      std::cout<<"[SmearedJetProducer::Produce]: smearing params: "
		       <<"jet_resolution = "<<jet_resolution
		       <<", jer_sf = "<<jer_sf
		       <<std::endl;
	    }
	    //match genJet
	    int genJetIndex = matchedGenJetIndex(event, jet->get(), (*jet)->p4.pt() * jet_resolution);
	    double smearFactor = 1.;
	    if (genJetIndex>-1) {
	      /*
	       * Case 1: we have a "good" gen jet matched to the reco jet
	       */
	      double dPt = (*jet)->p4.pt() - event.m_genJets->at(genJetIndex).p4.pt();
	      smearFactor = 1 + m_nomVar * (jer_sf - 1.) * dPt / (*jet)->p4.pt();
	      if (m_verbose) {
		std::cout<<"[SmearedJetProducer::Produce]: matched to gen: "
			 <<"original Pt = "<<(*jet)->p4.pt()
			 <<", gen Pt = "<<event.m_genJets->at(genJetIndex).p4.pt()
			 <<std::endl;
	      }
	    } else if (jer_sf > 1) {
	      /*
	       * Case 2: we don't have a gen jet. Smear jet pt using a random gaussian variation
	       */
	      double sigma = jet_resolution * std::sqrt(jer_sf * jer_sf - 1);
	      std::normal_distribution<> d(0, sigma);
	      smearFactor = 1. + m_nomVar * d(*m_random_generator);
	      if (m_verbose) {
		std::cout<<"[SmearedJetProducer::Produce]: no matched to gen: "
			 <<"gaussian width: "<<sigma
			 <<std::endl;
	      }
	    }
	    if ((*jet)->p4.energy() * smearFactor < MIN_JET_ENERGY) {
	      /* 
	       * Negative or too small smearFactor. We would change direction of the jet
	       * and this is not what we want.
	       * Recompute the smearing factor in order to have jet.energy() == MIN_JET_ENERGY
	       */
	      double newSmearFactor = MIN_JET_ENERGY / (*jet)->p4.energy();
	      smearFactor = newSmearFactor;
	    }

	    float origPt = (*jet)->p4.pt();
	    (*jet)->p4 = smearFactor * (*jet)->p4;
	    if (m_verbose) {
	      std::cout<<"[SmearedJetProducer::Produce]: smearing: "
		       <<"original Pt = "<<origPt
		       <<", smeared Pt = "<<(*jet)->p4.pt()
		       <<std::endl;
	    }
	  }

	  // perform corrections on copied jets
	  for (typename std::vector<std::shared_ptr<TJet> >::iterator jet = (product.*m_correctedJetsMember).begin();
	       jet != (product.*m_correctedJetsMember).end(); ++jet) {
	    // No general correction implemented

	    // perform possible analysis-specific corrections
	    AdditionalCorrections(jet->get(), event, product, settings, metadata);
	  }

	  // sort vectors of corrected jets by pt
	  std::sort((product.*m_correctedJetsMember).begin(), (product.*m_correctedJetsMember).end(),
		    [](std::shared_ptr<TJet> jet1, std::shared_ptr<TJet> jet2) -> bool
		    { return jet1->p4.pt() > jet2->p4.pt(); });
	}

protected:
	// Can be overwritten for analysis-specific use cases
	virtual void AdditionalCorrections(TJet* jet, event_type const& event, product_type& product,
	                                   setting_type const& settings, metadata_type const& metadata) const {}

	// Return index of matched genJet
	int matchedGenJetIndex(event_type const& event, const TJet* jet, double resolution) const {
	  int matchedIndex = -1;
	  double min_dR = std::numeric_limits<double>::infinity();
	  int genJetIndex = 0;
	  for (std::vector<KGenJet>::const_iterator genJet = event.m_genJets->begin();
	       genJet != event.m_genJets->end(); ++genJet) {
	    double dR = ROOT::Math::VectorUtil::DeltaR(genJet->p4, jet->p4);
	    if (dR > min_dR) {
	      ++genJetIndex;
	      continue;	      
	    }
	    if (dR < m_dR_max) {
	      double dPt = std::abs(genJet->p4.pt() - jet->p4.pt());
	      if (dPt > m_dPt_max_factor * resolution) {
		++genJetIndex;
		continue;
	      }	      
	      min_dR = dR;
	      matchedIndex = genJetIndex;
	    }
	    ++genJetIndex;
	  }

	  return matchedIndex;
	}

private:

	std::vector<TJet>* KappaTypes::event_type::*m_basicJetsMember;
	std::vector<std::shared_ptr<TJet> > KappaTypes::product_type::*m_correctedJetsMember;

	KappaEnumTypes::ValidJetsInput validJetsInput;

	static constexpr const double MIN_JET_ENERGY = 1e-2;
	bool m_enabled;
	Variation m_systematic_variation;
	std::string m_uncertaintySource;
	bool m_useDeterministicSeed;
	std::unique_ptr<JME::JetResolution> m_resolution_from_file;
	std::unique_ptr<JME::JetResolutionScaleFactor> m_scale_factor_from_file;

	std::mt19937 * m_random_generator;

	int m_nomVar;
	
	double m_dR_max;
	double m_dPt_max_factor;

	bool m_verbose;
};



/**
   \brief Producer of smeared jets for Jet Energy Resolution (JER) correction
   
   Operates on the vector product::m_correctedJets to smear them accordingly to JER.
*/
class SmearedJetProducer: public SmearedJetProducerBase<KBasicJet>
{
public:
	SmearedJetProducer();

	std::string GetProducerId() const override;
};



/**
   \brief Producer of smeared jets for Jet Energy Resolution (JER) correction
   
   Operates on the vector product::m_correctedTaggedjets to smear them accordingly to.
*/
class SmearedTaggedJetProducer: public SmearedJetProducerBase<KJet>
{
public:

	SmearedTaggedJetProducer();
	
	std::string GetProducerId() const override;
};
