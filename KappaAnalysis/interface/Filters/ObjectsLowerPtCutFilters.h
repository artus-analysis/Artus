
#pragma once

#include <limits>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include "Artus/Filter/interface/CutFilterBase.h"
#include "Artus/Utility/interface/Utility.h"


/** Abstract Lepton Pt Filter
 */
template<class TTypes, class TLepton>
class LeptonLowerPtCutsFilter: public CutRangeFilterBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	LeptonLowerPtCutsFilter(std::vector<TLepton*> product_type::*validLeptons) :
		CutRangeFilterBase<TTypes>(),
		m_validLeptonsMember(validLeptons)
	{
	}


protected:

	void Initialise(std::vector<std::string> const& leptonLowerPtCutsVector) {
		std::map<std::string, std::vector<std::string> > leptonLowerPtCuts = Utility::ParseVectorToMap(leptonLowerPtCutsVector);
	
		std::vector<int> defaultIndices = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
		for (std::map<std::string, std::vector<std::string> >::const_iterator leptonLowerPtCut = leptonLowerPtCuts.begin();
		     leptonLowerPtCut != leptonLowerPtCuts.end(); ++leptonLowerPtCut)
		{
			std::vector<int> indices;
			std::vector<std::string> hltNames;
			if (leptonLowerPtCut->first == "default") {
				indices = defaultIndices;
				LOG(WARNING) << "No lepton index for the Filter \"" << this->GetFilterId() << "\" specified. Check the possible 3 hardest leptons.";
			}
			else {
				try {
					indices.push_back(boost::lexical_cast<int>(leptonLowerPtCut->first));
				}
				catch (boost::bad_lexical_cast const& exception) {
					hltNames.push_back(leptonLowerPtCut->first);
				}
			}
			
			for (std::vector<std::string>::const_iterator ptCut = leptonLowerPtCut->second.begin();
			     ptCut != leptonLowerPtCut->second.end(); ++ptCut)
			{
				double ptCutValue = std::stod(*ptCut);
				
				for (std::vector<int>::iterator index = indices.begin(); index != indices.end(); ++index)
				{
					size_t tmpIndex(*index); // TODO
					this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
							[this, tmpIndex](event_type const& event, product_type const& product) -> double {
								return (((product.*m_validLeptonsMember).size() > tmpIndex) ?
								        (product.*m_validLeptonsMember).at(tmpIndex)->p4.Pt() :
								        0.99*std::numeric_limits<double>::max());
							},
							CutRange::LowerThresholdCut(ptCutValue)
					));
				}
				
				for (std::vector<std::string>::iterator hltName = hltNames.begin(); hltName != hltNames.end(); ++hltName)
				{
					std::string tmpHltName(*hltName); // TODO
					boost::regex pattern(tmpHltName, boost::regex::icase | boost::regex::extended);
					for (std::vector<int>::iterator index = defaultIndices.begin(); index != defaultIndices.end(); ++index)
					{
						size_t tmpIndex(*index);
						this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
								[this, tmpHltName, pattern, tmpIndex](event_type const& event, product_type const& product) -> double {
									return (((product.*m_validLeptonsMember).size() > tmpIndex && boost::regex_search(product.m_selectedHltName, pattern)) ?
									        (product.*m_validLeptonsMember).at(tmpIndex)->p4.Pt() :
									        0.99*std::numeric_limits<double>::max());
								},
								CutRange::LowerThresholdCut(ptCutValue)
						));
					}
				}
			}
		}	
	}


private:
	std::vector<TLepton*> product_type::*m_validLeptonsMember;
};


/** Electron Pt Filter
 */
template<class TTypes>
class ElectronLowerPtCutsFilter: public LeptonLowerPtCutsFilter<TTypes, KDataElectron> {
public:
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "electron_lower_pt_cuts";
	}
	
	ElectronLowerPtCutsFilter() : LeptonLowerPtCutsFilter<TTypes, KDataElectron>(&TTypes::product_type::m_validElectrons) {}
	
	
	virtual void Init(typename TTypes::setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		this->Initialise(settings.GetElectronLowerPtCuts());
	}
};


/** Muon Pt Filter
 */
template<class TTypes>
class MuonLowerPtCutsFilter: public LeptonLowerPtCutsFilter<TTypes, KDataMuon> {
public:
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "muon_lower_pt_cuts";
	}
	
	MuonLowerPtCutsFilter() : LeptonLowerPtCutsFilter<TTypes, KDataMuon>(&TTypes::product_type::m_validMuons) {}
	
	virtual void Init(typename TTypes::setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		this->Initialise(settings.GetMuonLowerPtCuts());
	}
};


/** Tau Pt Filter
 */
template<class TTypes>
class TauLowerPtCutsFilter: public LeptonLowerPtCutsFilter<TTypes, KDataPFTau> {
public:
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "tau_lower_pt_cuts";
	}
	
	TauLowerPtCutsFilter() : LeptonLowerPtCutsFilter<TTypes, KDataPFTau>(&TTypes::product_type::m_validTaus) {}
	
	virtual void Init(typename TTypes::setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		this->Initialise(settings.GetTauLowerPtCuts());
	}
};


/** Jet Pt Filter
 */
template<class TTypes>
class JetLowerPtCutsFilter: public LeptonLowerPtCutsFilter<TTypes, KDataPFJet> {
public:
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "jet_lower_pt_cuts";
	}
	
	JetLowerPtCutsFilter() : LeptonLowerPtCutsFilter<TTypes, KDataPFJet>(&TTypes::product_type::m_validJets) {}
	
	virtual void Init(typename TTypes::setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		this->Initialise(settings.GetJetLowerPtCuts());
	}
};

