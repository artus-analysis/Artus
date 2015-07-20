
#pragma once

#include <limits>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include "Artus/Filter/interface/CutFilterBase.h"
#include "Artus/Utility/interface/Utility.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


/** Abstract Lepton Eta Filter
 */
template<class TLepton>
class LeptonUpperAbsEtaCutsFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	LeptonUpperAbsEtaCutsFilter(std::vector<TLepton*> KappaProduct::*validLeptons) :
		CutRangeFilterBase<KappaTypes>(),
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
				LOG(WARNING) << "No lepton index for the Filter \"" << this->GetFilterId() << "\" specified. Check the possible " << defaultIndices.size() << " hardest leptons.";
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
							[this, tmpIndex](KappaEvent const& event, KappaProduct const& product) -> double {
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
								[this, tmpHltName, pattern, tmpIndex](KappaEvent const& event, KappaProduct const& product) -> double {
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
	std::vector<TLepton*> KappaProduct::*m_validLeptonsMember;
};


/** Electron Eta Filter
 */
class ElectronUpperAbsEtaCutsFilter: public LeptonUpperAbsEtaCutsFilter<KElectron> {
public:
	
	virtual std::string GetFilterId() const override;
	
	ElectronUpperAbsEtaCutsFilter() : LeptonUpperAbsEtaCutsFilter<KElectron>(&KappaProduct::m_validElectrons) {}
	
	virtual void Init(KappaSettings const& settings) override;
};


/** Muon Eta Filter
 */
class MuonUpperAbsEtaCutsFilter: public LeptonUpperAbsEtaCutsFilter<KMuon> {
public:
	
	virtual std::string GetFilterId() const override;
	
	MuonUpperAbsEtaCutsFilter() : LeptonUpperAbsEtaCutsFilter<KMuon>(&KappaProduct::m_validMuons) {}
	
	virtual void Init(KappaSettings const& settings) override;
};


/** Tau Eta Filter
 */
class TauUpperAbsEtaCutsFilter: public LeptonUpperAbsEtaCutsFilter<KTau> {
public:
	
	virtual std::string GetFilterId() const override;
	
	TauUpperAbsEtaCutsFilter() : LeptonUpperAbsEtaCutsFilter<KTau>(&KappaProduct::m_validTaus) {}
	
	virtual void Init(KappaSettings const& settings) override;
};


/** Jet Eta Filter
 */
class JetUpperAbsEtaCutsFilter: public LeptonUpperAbsEtaCutsFilter<KBasicJet> {
public:
	
	virtual std::string GetFilterId() const override;
	
	JetUpperAbsEtaCutsFilter() : LeptonUpperAbsEtaCutsFilter<KBasicJet>(&KappaProduct::m_validJets) {}
	
	virtual void Init(KappaSettings const& settings) override;
};

