
#pragma once

#include "Artus/Filter/interface/CutFilterBase.h"
#include "Artus/Utility/interface/Utility.h"


/** Electron Pt Filter
 */
template<class TTypes>
class ElectronLowerPtCutsFilter: public CutRangeFilterBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "electron_lower_pt_cuts";
	}
	virtual void InitGlobal(global_setting_type const& globalSettings) ARTUS_CPP11_OVERRIDE {
		Init(Utility::ParseVectorToMap(globalSettings.GetElectronLowerPtCuts()));
	}
	virtual void InitLocal(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		Init(Utility::ParseVectorToMap(settings.GetElectronLowerPtCuts()));
	}

private:
	void Init(std::map<std::string, std::vector<std::string> > const& leptonLowerPtCuts) {
		for (std::map<std::string, std::vector<std::string> >::const_iterator leptonLowerPtCut = leptonLowerPtCuts.begin();
		     leptonLowerPtCut != leptonLowerPtCuts.end(); ++leptonLowerPtCut)
		{
			std::vector<int> indices;
			if (leptonLowerPtCut->first == "default") {
				indices = {0, 1, 2};
				LOG(WARNING) << "No lepton index for the Filter \"" << GetFilterId() << "\" specified. Check the possible 3 hardest leptons.";
			}
			else {
				indices.push_back(std::stoi(leptonLowerPtCut->first));
			}
			
			for (std::vector<std::string>::const_iterator ptCut = leptonLowerPtCut->second.begin();
			     ptCut != leptonLowerPtCut->second.end(); ++ptCut)
			{
				for (std::vector<int>::iterator index = indices.begin(); index != indices.end(); ++index)
				{
					size_t tmpIndex(*index);
					this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
							[tmpIndex](event_type const& event, product_type const& product) {
								return ((product.m_validElectrons.size() > tmpIndex) ? product.m_validElectrons.at(tmpIndex)->p4.Pt() : -1.0);
							},
							CutRange::LowerThresholdCut(std::stod(*ptCut))
					));
				}
			}
		}
		
	}
};


/** Muon Pt Filter
 */
template<class TTypes>
class MuonLowerPtCutsFilter: public CutRangeFilterBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "muon_lower_pt_cuts";
	}
	virtual void InitGlobal(global_setting_type const& globalSettings) ARTUS_CPP11_OVERRIDE {
		Init(Utility::ParseVectorToMap(globalSettings.GetMuonLowerPtCuts()));
	}
	virtual void InitLocal(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		Init(Utility::ParseVectorToMap(settings.GetMuonLowerPtCuts()));
	}

private:
	void Init(std::map<std::string, std::vector<std::string> > const& leptonLowerPtCuts) {
		for (std::map<std::string, std::vector<std::string> >::const_iterator leptonLowerPtCut = leptonLowerPtCuts.begin();
		     leptonLowerPtCut != leptonLowerPtCuts.end(); ++leptonLowerPtCut)
		{
			std::vector<int> indices;
			if (leptonLowerPtCut->first == "default") {
				indices = {0, 1, 2};
				LOG(WARNING) << "No lepton index for the Filter \"" << GetFilterId() << "\" specified. Check the possible 3 hardest leptons.";
			}
			else {
				indices.push_back(std::stoi(leptonLowerPtCut->first));
			}
			
			for (std::vector<std::string>::const_iterator ptCut = leptonLowerPtCut->second.begin();
			     ptCut != leptonLowerPtCut->second.end(); ++ptCut)
			{
				for (std::vector<int>::iterator index = indices.begin(); index != indices.end(); ++index)
				{
					size_t tmpIndex(*index);
					this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
							[tmpIndex](event_type const& event, product_type const& product) {
								return ((product.m_validMuons.size() > tmpIndex) ? product.m_validMuons.at(tmpIndex)->p4.Pt() : -1.0);
							},
							CutRange::LowerThresholdCut(std::stod(*ptCut))
					));
				}
			}
		}
		
	}
};


/** Tau Pt Filter
 */
template<class TTypes>
class TauLowerPtCutsFilter: public CutRangeFilterBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "tau_lower_pt_cuts";
	}
	virtual void InitGlobal(global_setting_type const& globalSettings) ARTUS_CPP11_OVERRIDE {
		Init(Utility::ParseVectorToMap(globalSettings.GetTauLowerPtCuts()));
	}
	virtual void InitLocal(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		Init(Utility::ParseVectorToMap(settings.GetTauLowerPtCuts()));
	}

private:
	void Init(std::map<std::string, std::vector<std::string> > const& leptonLowerPtCuts) {
		for (std::map<std::string, std::vector<std::string> >::const_iterator leptonLowerPtCut = leptonLowerPtCuts.begin();
		     leptonLowerPtCut != leptonLowerPtCuts.end(); ++leptonLowerPtCut)
		{
			std::vector<int> indices;
			if (leptonLowerPtCut->first == "default") {
				indices = {0, 1, 2};
				LOG(WARNING) << "No lepton index for the Filter \"" << GetFilterId() << "\" specified. Check the possible 3 hardest leptons.";
			}
			else {
				indices.push_back(std::stoi(leptonLowerPtCut->first));
			}
			
			for (std::vector<std::string>::const_iterator ptCut = leptonLowerPtCut->second.begin();
			     ptCut != leptonLowerPtCut->second.end(); ++ptCut)
			{
				for (std::vector<int>::iterator index = indices.begin(); index != indices.end(); ++index)
				{
					size_t tmpIndex(*index);
					this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
							[tmpIndex](event_type const& event, product_type const& product) {
								return ((product.m_validTaus.size() > tmpIndex) ? product.m_validTaus.at(tmpIndex)->p4.Pt() : -1.0);
							},
							CutRange::LowerThresholdCut(std::stod(*ptCut))
					));
				}
			}
		}
		
	}
};


/** Jets Pt Filter
 */
template<class TTypes>
class JetLowerPtCutsFilter: public CutRangeFilterBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "jet_lower_pt_cuts";
	}
	virtual void InitGlobal(global_setting_type const& globalSettings) ARTUS_CPP11_OVERRIDE {
		Init(Utility::ParseVectorToMap(globalSettings.GetJetLowerPtCuts()));
	}
	virtual void InitLocal(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		Init(Utility::ParseVectorToMap(settings.GetJetLowerPtCuts()));
	}

private:
	void Init(std::map<std::string, std::vector<std::string> > const& leptonLowerPtCuts) {
		for (std::map<std::string, std::vector<std::string> >::const_iterator leptonLowerPtCut = leptonLowerPtCuts.begin();
		     leptonLowerPtCut != leptonLowerPtCuts.end(); ++leptonLowerPtCut)
		{
			std::vector<int> indices;
			if (leptonLowerPtCut->first == "default") {
				indices = {0, 1, 2};
				LOG(WARNING) << "No lepton index for the Filter \"" << GetFilterId() << "\" specified. Check the possible 3 hardest leptons.";
			}
			else {
				indices.push_back(std::stoi(leptonLowerPtCut->first));
			}
			
			for (std::vector<std::string>::const_iterator ptCut = leptonLowerPtCut->second.begin();
			     ptCut != leptonLowerPtCut->second.end(); ++ptCut)
			{
				for (std::vector<int>::iterator index = indices.begin(); index != indices.end(); ++index)
				{
					size_t tmpIndex(*index);
					this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
							[tmpIndex](event_type const& event, product_type const& product) {
								return ((product.m_validJets.size() > tmpIndex) ? product.m_validJets.at(tmpIndex)->p4.Pt() : -1.0);
							},
							CutRange::LowerThresholdCut(std::stod(*ptCut))
					));
				}
			}
		}
		
	}
};

