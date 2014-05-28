
#pragma once

#include "Artus/Filter/interface/CutFilterBase.h"
#include "Artus/Utility/interface/Utility.h"


/** Electron Eta Filter
 */
template<class TTypes>
class ElectronUpperAbsEtaCutsFilter: public CutRangeFilterBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "electron_upper_abseta_cuts";
	}
	virtual void InitGlobal(global_setting_type const& globalSettings) ARTUS_CPP11_OVERRIDE {
		Init(Utility::ParseVectorToMap(globalSettings.GetElectronUpperAbsEtaCuts()));
	}
	virtual void InitLocal(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		Init(Utility::ParseVectorToMap(settings.GetElectronUpperAbsEtaCuts()));
	}

private:
	void Init(std::map<std::string, std::vector<std::string> > const& leptonUpperAbsEtaCuts) {
		for (std::map<std::string, std::vector<std::string> >::const_iterator leptonUpperAbsEtaCut = leptonUpperAbsEtaCuts.begin();
		     leptonUpperAbsEtaCut != leptonUpperAbsEtaCuts.end(); ++leptonUpperAbsEtaCut)
		{
			std::vector<int> indices;
			if (leptonUpperAbsEtaCut->first == "default") {
				indices = {0, 1, 2};
				LOG(WARNING) << "No lepton index for the Filter \"" << GetFilterId() << "\" specified. Check the possible 3 hardest leptons.";
			}
			else {
				indices.push_back(std::stoi(leptonUpperAbsEtaCut->first));
			}
			
			for (std::vector<std::string>::const_iterator ptCut = leptonUpperAbsEtaCut->second.begin();
			     ptCut != leptonUpperAbsEtaCut->second.end(); ++ptCut)
			{
				for (std::vector<int>::iterator index = indices.begin(); index != indices.end(); ++index)
				{
					size_t tmpIndex(*index);
					this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
							[tmpIndex](event_type const& event, product_type const& product) {
								return ((product.m_validElectrons.size() > tmpIndex) ? std::abs(product.m_validElectrons.at(tmpIndex)->p4.Eta()) : 100.0);
							},
							CutRange::UpperThresholdCut(std::stod(*ptCut))
					));
				}
			}
		}
		
	}
};


/** Muon Eta Filter
 */
template<class TTypes>
class MuonUpperAbsEtaCutsFilter: public CutRangeFilterBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "muon_upper_abseta_cuts";
	}
	virtual void InitGlobal(global_setting_type const& globalSettings) ARTUS_CPP11_OVERRIDE {
		Init(Utility::ParseVectorToMap(globalSettings.GetMuonUpperAbsEtaCuts()));
	}
	virtual void InitLocal(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		Init(Utility::ParseVectorToMap(settings.GetMuonUpperAbsEtaCuts()));
	}

private:
	void Init(std::map<std::string, std::vector<std::string> > const& leptonUpperAbsEtaCuts) {
		for (std::map<std::string, std::vector<std::string> >::const_iterator leptonUpperAbsEtaCut = leptonUpperAbsEtaCuts.begin();
		     leptonUpperAbsEtaCut != leptonUpperAbsEtaCuts.end(); ++leptonUpperAbsEtaCut)
		{
			std::vector<int> indices;
			if (leptonUpperAbsEtaCut->first == "default") {
				indices = {0, 1, 2};
				LOG(WARNING) << "No lepton index for the Filter \"" << GetFilterId() << "\" specified. Check the possible 3 hardest leptons.";
			}
			else {
				indices.push_back(std::stoi(leptonUpperAbsEtaCut->first));
			}
			
			for (std::vector<std::string>::const_iterator ptCut = leptonUpperAbsEtaCut->second.begin();
			     ptCut != leptonUpperAbsEtaCut->second.end(); ++ptCut)
			{
				for (std::vector<int>::iterator index = indices.begin(); index != indices.end(); ++index)
				{
					size_t tmpIndex(*index);
					this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
							[tmpIndex](event_type const& event, product_type const& product) {
								return ((product.m_validMuons.size() > tmpIndex) ? std::abs(product.m_validMuons.at(tmpIndex)->p4.Eta()) : 100.0);
							},
							CutRange::UpperThresholdCut(std::stod(*ptCut))
					));
				}
			}
		}
		
	}
};


/** Tau Eta Filter
 */
template<class TTypes>
class TauUpperAbsEtaCutsFilter: public CutRangeFilterBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "tau_upper_abseta_cuts";
	}
	virtual void InitGlobal(global_setting_type const& globalSettings) ARTUS_CPP11_OVERRIDE {
		Init(Utility::ParseVectorToMap(globalSettings.GetTauUpperAbsEtaCuts()));
	}
	virtual void InitLocal(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		Init(Utility::ParseVectorToMap(settings.GetTauUpperAbsEtaCuts()));
	}

private:
	void Init(std::map<std::string, std::vector<std::string> > const& leptonUpperAbsEtaCuts) {
		for (std::map<std::string, std::vector<std::string> >::const_iterator leptonUpperAbsEtaCut = leptonUpperAbsEtaCuts.begin();
		     leptonUpperAbsEtaCut != leptonUpperAbsEtaCuts.end(); ++leptonUpperAbsEtaCut)
		{
			std::vector<int> indices;
			if (leptonUpperAbsEtaCut->first == "default") {
				indices = {0, 1, 2};
				LOG(WARNING) << "No lepton index for the Filter \"" << GetFilterId() << "\" specified. Check the possible 3 hardest leptons.";
			}
			else {
				indices.push_back(std::stoi(leptonUpperAbsEtaCut->first));
			}
			
			for (std::vector<std::string>::const_iterator ptCut = leptonUpperAbsEtaCut->second.begin();
			     ptCut != leptonUpperAbsEtaCut->second.end(); ++ptCut)
			{
				for (std::vector<int>::iterator index = indices.begin(); index != indices.end(); ++index)
				{
					size_t tmpIndex(*index);
					this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
							[tmpIndex](event_type const& event, product_type const& product) {
								return ((product.m_validTaus.size() > tmpIndex) ? std::abs(product.m_validTaus.at(tmpIndex)->p4.Eta()) : 100.0);
							},
							CutRange::UpperThresholdCut(std::stod(*ptCut))
					));
				}
			}
		}
		
	}
};


/** Jets Eta Filter
 */
template<class TTypes>
class JetUpperAbsEtaCutsFilter: public CutRangeFilterBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "jet_upper_abseta_cuts";
	}
	virtual void InitGlobal(global_setting_type const& globalSettings) ARTUS_CPP11_OVERRIDE {
		Init(Utility::ParseVectorToMap(globalSettings.GetJetUpperAbsEtaCuts()));
	}
	virtual void InitLocal(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		Init(Utility::ParseVectorToMap(settings.GetJetUpperAbsEtaCuts()));
	}

private:
	void Init(std::map<std::string, std::vector<std::string> > const& leptonUpperAbsEtaCuts) {
		for (std::map<std::string, std::vector<std::string> >::const_iterator leptonUpperAbsEtaCut = leptonUpperAbsEtaCuts.begin();
		     leptonUpperAbsEtaCut != leptonUpperAbsEtaCuts.end(); ++leptonUpperAbsEtaCut)
		{
			std::vector<int> indices;
			if (leptonUpperAbsEtaCut->first == "default") {
				indices = {0, 1, 2};
				LOG(WARNING) << "No lepton index for the Filter \"" << GetFilterId() << "\" specified. Check the possible 3 hardest leptons.";
			}
			else {
				indices.push_back(std::stoi(leptonUpperAbsEtaCut->first));
			}
			
			for (std::vector<std::string>::const_iterator ptCut = leptonUpperAbsEtaCut->second.begin();
			     ptCut != leptonUpperAbsEtaCut->second.end(); ++ptCut)
			{
				for (std::vector<int>::iterator index = indices.begin(); index != indices.end(); ++index)
				{
					size_t tmpIndex(*index);
					this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
							[tmpIndex](event_type const& event, product_type const& product) {
								return ((product.m_validJets.size() > tmpIndex) ? std::abs(product.m_validJets.at(tmpIndex)->p4.Eta()) : 100.0);
							},
							CutRange::UpperThresholdCut(std::stod(*ptCut))
					));
				}
			}
		}
		
	}
};

