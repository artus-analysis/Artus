#pragma once

#include <limits>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include "Artus/Filter/interface/CutFilterBase.h"
#include "Artus/Utility/interface/Utility.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


/** Abstract Lepton Pt Filter
 */
template<class TLepton>
class LeptonLowerPtCutsFilter: public CutRangeFilterBase<KappaTypes> {
public:

	typedef typename std::function<double(event_type const&, product_type const&, setting_type const&, metadata_type const&)> double_extractor_lambda;

	explicit LeptonLowerPtCutsFilter(std::vector<TLepton*> product_type::*validLeptons) :
		CutRangeFilterBase<KappaTypes>(),
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
				LOG(WARNING) << "No lepton index for the Filter \"" << this->GetFilterId() << "\" specified. Check the possible " << defaultIndices.size() << " hardest leptons.";
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
							[this, tmpIndex](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata) -> double {
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
								[this, tmpHltName, pattern, tmpIndex](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata) -> double {
									bool hasMatch = false;
									for (unsigned int iHlt = 0; iHlt < product.m_selectedHltNames.size(); ++iHlt)
										hasMatch = hasMatch || boost::regex_search(product.m_selectedHltNames.at(iHlt), pattern);

									return (((product.*m_validLeptonsMember).size() > tmpIndex && hasMatch) ?
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
class ElectronLowerPtCutsFilter: public LeptonLowerPtCutsFilter<KElectron> {
public:
	
	std::string GetFilterId() const override;
	
	ElectronLowerPtCutsFilter();
	
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** Muon Pt Filter
 */
class MuonLowerPtCutsFilter: public LeptonLowerPtCutsFilter<KMuon> {
public:
	
	std::string GetFilterId() const override;
	
	MuonLowerPtCutsFilter();
	
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** Tau Pt Filter
 */
class TauLowerPtCutsFilter: public LeptonLowerPtCutsFilter<KTau> {
public:
	
	std::string GetFilterId() const override;
	
	TauLowerPtCutsFilter();
	
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** Jet Pt Filter
 */
class JetLowerPtCutsFilter: public LeptonLowerPtCutsFilter<KBasicJet> {
public:
	
	std::string GetFilterId() const override;
	
	JetLowerPtCutsFilter();
	
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** NonBTaggedJet Pt Filter
 */
class NonBTaggedJetLowerPtCutsFilter: public LeptonLowerPtCutsFilter<KJet> {
public:
	
	std::string GetFilterId() const override;
	
	NonBTaggedJetLowerPtCutsFilter();
	
	void Init(setting_type const& settings, metadata_type& metadata) override;
};
