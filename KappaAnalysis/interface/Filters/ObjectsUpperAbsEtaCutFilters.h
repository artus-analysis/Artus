
#pragma once

#include <limits>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include "Artus/Filter/interface/CutFilterBase.h"
#include "Artus/Utility/interface/Utility.h"


/** Abstract Lepton Eta Filter
 */
template<class TTypes, class TLepton>
class LeptonUpperAbsEtaCutsFilter: public CutRangeFilterBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	LeptonUpperAbsEtaCutsFilter(std::vector<TLepton*> product_type::*validLeptons) :
		CutRangeFilterBase<TTypes>(),
		m_validLeptonsMember(validLeptons)
	{
	}


protected:

	void Initialise(std::vector<std::string> const& leptonUpperAbsEtaCutsVector) {
		std::map<std::string, std::vector<std::string> > leptonUpperAbsEtaCuts = Utility::ParseVectorToMap(leptonUpperAbsEtaCutsVector);
	
		std::vector<int> defaultIndices = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
		for (std::map<std::string, std::vector<std::string> >::const_iterator leptonUpperAbsEtaCut = leptonUpperAbsEtaCuts.begin();
		     leptonUpperAbsEtaCut != leptonUpperAbsEtaCuts.end(); ++leptonUpperAbsEtaCut)
		{
			std::vector<int> indices;
			std::vector<std::string> hltNames;
			if (leptonUpperAbsEtaCut->first == "default") {
				indices = defaultIndices;
				LOG(WARNING) << "No lepton index for the Filter \"" << this->GetFilterId() << "\" specified. Check the possible 3 hardest leptons.";
			}
			else {
				try {
					indices.push_back(boost::lexical_cast<int>(leptonUpperAbsEtaCut->first));
				}
				catch (boost::bad_lexical_cast const& exception) {
					hltNames.push_back(leptonUpperAbsEtaCut->first);
				}
			}
			
			for (std::vector<std::string>::const_iterator absEtaCut = leptonUpperAbsEtaCut->second.begin();
			     absEtaCut != leptonUpperAbsEtaCut->second.end(); ++absEtaCut)
			{
				double absEtaCutValue = std::stod(*absEtaCut);
				
				for (std::vector<int>::iterator index = indices.begin(); index != indices.end(); ++index)
				{
					size_t tmpIndex(*index); // TODO
					this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
							[this, tmpIndex](event_type const& event, product_type const& product) -> double {
								return (((product.*m_validLeptonsMember).size() > tmpIndex) ?
								        std::abs((product.*m_validLeptonsMember).at(tmpIndex)->p4.Eta()) :
								        -1.0);
							},
							CutRange::UpperThresholdCut(absEtaCutValue)
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
									        std::abs((product.*m_validLeptonsMember).at(tmpIndex)->p4.Eta()) :
									        -1.0);
								},
								CutRange::UpperThresholdCut(absEtaCutValue)
						));
					}
				}
			}
		}	
	}


private:
	std::vector<TLepton*> product_type::*m_validLeptonsMember;
};


/** Electron Eta Filter
 */
template<class TTypes>
class ElectronUpperAbsEtaCutsFilter: public LeptonUpperAbsEtaCutsFilter<TTypes, KDataElectron> {
public:
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "ElectronUpperAbsEtaCutsFilter";
	}
	
	ElectronUpperAbsEtaCutsFilter() : LeptonUpperAbsEtaCutsFilter<TTypes, KDataElectron>(&TTypes::product_type::m_validElectrons) {}
	
	virtual void Init(typename TTypes::setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		this->Initialise(settings.GetElectronUpperAbsEtaCuts());
	}
};


/** Muon Eta Filter
 */
template<class TTypes>
class MuonUpperAbsEtaCutsFilter: public LeptonUpperAbsEtaCutsFilter<TTypes, KDataMuon> {
public:
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "MuonUpperAbsEtaCutsFilter";
	}
	
	MuonUpperAbsEtaCutsFilter() : LeptonUpperAbsEtaCutsFilter<TTypes, KDataMuon>(&TTypes::product_type::m_validMuons) {}
	
	virtual void Init(typename TTypes::setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		this->Initialise(settings.GetMuonUpperAbsEtaCuts());
	}
};


/** Tau Eta Filter
 */
template<class TTypes>
class TauUpperAbsEtaCutsFilter: public LeptonUpperAbsEtaCutsFilter<TTypes, KDataPFTau> {
public:
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "TauUpperAbsEtaCutsFilter";
	}
	
	TauUpperAbsEtaCutsFilter() : LeptonUpperAbsEtaCutsFilter<TTypes, KDataPFTau>(&TTypes::product_type::m_validTaus) {}
	
	virtual void Init(typename TTypes::setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		this->Initialise(settings.GetTauUpperAbsEtaCuts());
	}
};


/** Jet Eta Filter
 */
template<class TTypes>
class JetUpperAbsEtaCutsFilter: public LeptonUpperAbsEtaCutsFilter<TTypes, KDataPFJet> {
public:
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "JetUpperAbsEtaCutsFilter";
	}
	
	JetUpperAbsEtaCutsFilter() : LeptonUpperAbsEtaCutsFilter<TTypes, KDataPFJet>(&TTypes::product_type::m_validJets) {}
	
	virtual void Init(typename TTypes::setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		this->Initialise(settings.GetJetUpperAbsEtaCuts());
	}
};

