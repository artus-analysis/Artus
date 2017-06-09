#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"
#include "Artus/Utility/interface/Utility.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"

/** Producer class for Z boson reconstruction from muons/electrons.
 *
 *	Needs to run after the valid object producers.
 * 	Needs at least 2 valid leptons to construct a Z.
 */

template <class TLepton1, class TLepton2>
class ZProducerBase : public KappaProducerBase
{
	public:
	ZProducerBase(std::vector<TLepton1*> KappaProduct::*validLeptons1,
	              std::vector<TLepton2*> KappaProduct::*validLeptons2,
	              bool check_same_collection=true,
	              bool check_cross_collection=false) :
		KappaProducerBase(),
		m_validLeptonsMember1(validLeptons1),
		m_validLeptonsMember2(validLeptons2),
		check_first_ll_collection(check_same_collection),
		check_second_ll_collection(check_same_collection),
		check_cross_ll_collection(check_cross_collection)
	{
	}

	ZProducerBase(std::vector<TLepton1*> KappaProduct::*validLeptons1) :
		KappaProducerBase(),
		m_validLeptonsMember1(validLeptons1),
		m_validLeptonsMember2(validLeptons1),
		check_first_ll_collection(true),
		check_second_ll_collection(false),
		check_cross_ll_collection(false)
	{
	}

	void Init(KappaSettings const& settings) override
	{
		KappaProducerBase::Init(settings);

		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("ZMass", [](KappaEvent const & event, KappaProduct const & product)
		{
			return product.m_z.p4.M();
		});

		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingLeptonFromZPt", [](KappaEvent const & event, KappaProduct const & product)
		{
			return product.m_zValid ? product.m_zLeptons.first->p4.Pt() : DefaultValues::UndefinedFloat;
		});

		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingLeptonFromZEta", [](KappaEvent const & event, KappaProduct const & product)
		{
			return product.m_zValid ? product.m_zLeptons.first->p4.Eta() : DefaultValues::UndefinedFloat;
		});

		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingLeptonFromZPhi", [](KappaEvent const & event, KappaProduct const & product)
		{
			return product.m_zValid ? product.m_zLeptons.first->p4.Phi() : DefaultValues::UndefinedFloat;
		});

		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingLeptonFromZPt", [](KappaEvent const & event, KappaProduct const & product)
		{
			return product.m_zValid ? product.m_zLeptons.second->p4.Pt() : DefaultValues::UndefinedFloat;
		});

		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingLeptonFromZEta", [](KappaEvent const & event, KappaProduct const & product)
		{
			return product.m_zValid ? product.m_zLeptons.second->p4.Eta() : DefaultValues::UndefinedFloat;
		});

		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingLeptonFromZPhi", [](KappaEvent const & event, KappaProduct const & product)
		{
			return product.m_zValid ? product.m_zLeptons.second->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("thetaZLepMinus", [](KappaEvent const & event, KappaProduct const & product)
		{
			return product.m_theta_Z_LepMinus;
		});
		LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("validZ", [](KappaEvent const & event, KappaProduct const & product)
		{
			return product.m_zValid;
		});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nZCandidates", [](KappaEvent const & event, KappaProduct const & product)
		{
			return product.m_found_zs;
		});
	}

	void Produce(KappaEvent const& event,
	             KappaProduct& product,
	             KappaSettings const& settings) const override
{
	/*
	Try all lepton combinations to find a valid Z

	Exactly one Z must be reconstructed. Events with any more or less are
	deemed ambigious, not having *a* valid Z.
	*/
	product.m_found_zs = 0;
	resetZ(product);

	if (check_first_ll_collection){
		for (unsigned int i = 0; i < (product.*m_validLeptonsMember1).size(); ++i) {
			for (unsigned int j = 0; j < i; ++j){
				KLepton* const lep1 = (product.*m_validLeptonsMember1).at(i);
				KLepton* const lep2 = (product.*m_validLeptonsMember1).at(j);
				if (lepton_pair_onZ(lep1,lep2,settings)){
					product.m_found_zs++;
					if (is_closer_to_Z((lep1->p4+lep2->p4).M(),product,settings))
						setZ(product,lep1,lep2,settings);
				}
			}
		}
	}

	// OK not the most elegant way, but at least it is understandable.
	// If you have time you can also bring this double loop into a function which takes carre of differnt types of m_validLeptonsMember1/2
	if (check_second_ll_collection){
		for (unsigned int i = 0; i < (product.*m_validLeptonsMember2).size(); ++i) {
			for (unsigned int j = 0; j < i; ++j){
				KLepton* const lep1 = (product.*m_validLeptonsMember2).at(i);
				KLepton* const lep2 = (product.*m_validLeptonsMember2).at(j);
				if (lepton_pair_onZ(lep1,lep2,settings)){
					product.m_found_zs++;
					if (is_closer_to_Z((lep1->p4+lep2->p4).M(),product,settings))
						setZ(product,lep1,lep2,settings);
				}
			}
		}
	}

	if (check_cross_ll_collection){
		for (unsigned int i = 0; i < (product.*m_validLeptonsMember1).size(); ++i) {
			for (unsigned int j = 0; j < (product.*m_validLeptonsMember2).size(); ++j){
				KLepton* const lep1 = (product.*m_validLeptonsMember1).at(i);
				KLepton* const lep2 = (product.*m_validLeptonsMember2).at(j);
				if (lepton_pair_onZ(lep1,lep2,settings)){
					product.m_found_zs++;
					if (is_closer_to_Z((lep1->p4+lep2->p4).M(),product,settings))
						setZ(product,lep1,lep2,settings);
				}
			}
		}
	}

	if (product.m_found_zs >1 && settings.GetVetoMultipleZs()) resetZ(product);
	if(product.m_zValid)
	{
		product.m_zPFLeptonsMatched =
			std::make_pair(determine_pfCandidate_for_lepton(product, settings, true),
			               determine_pfCandidate_for_lepton(product, settings, false) );
		product.m_theta_Z_LepMinus = calculate_theta_Z_LepMinus(product);

		if (product.m_zPFLeptonsMatched.first == nullptr or product.m_zPFLeptonsMatched.second == nullptr) {
			//std::cout << "Failed match" << std::endl;
			product.m_zValid = false;
		}
	}
	return;
}
	private:
		std::vector<TLepton1*> KappaProduct::*m_validLeptonsMember1;
		std::vector<TLepton2*> KappaProduct::*m_validLeptonsMember2;
		bool check_first_ll_collection;
		bool check_second_ll_collection;
		bool check_cross_ll_collection;
		bool lepton_pair_onZ(KLepton* const lep1, KLepton* const lep2, KappaSettings const& settings) const
		{
			if (lep1->charge() + lep2->charge() != 0) return false; // if charge not 0 can't be from Z
			KLV z_test;
			z_test.p4 = lep1->p4 + lep2->p4;
			double test_mass_diff = fabs(z_test.p4.M() - settings.GetZMass());
			return (test_mass_diff < settings.GetZMassRange()); // test if invM is in Z Range
		}
		bool is_closer_to_Z(double zCandidate_mass, KappaProduct& product, KappaSettings const& settings) const{
			if (!product.m_zValid) return true;
			return (fabs(zCandidate_mass-settings.GetZMass()) < fabs(product.m_z.p4.M()-settings.GetZMass()));
		}
		void resetZ(KappaProduct& product) const{
			product.m_z = KLV();
			product.m_z.p4.SetPt(0.0f);  // just to be sure
			product.m_zLeptons = std::make_pair(nullptr, nullptr);
			product.m_zValid = false;
		}
		void setZ(KappaProduct& product, KLepton* const lep1, KLepton* const lep2, KappaSettings const& settings) const
		{
			KLV zCandidate;
			zCandidate.p4 = lep1->p4 + lep2->p4;
			product.m_z = zCandidate;
			std::pair<KLepton*, KLepton*> zLeptons;
			if (lep1->p4.Pt() > lep2->p4.Pt())
				zLeptons = std::make_pair(lep1, lep2);
			else
				zLeptons = std::make_pair(lep2, lep1);
			product.m_zLeptons = zLeptons;
			product.m_zValid = (product.m_z.p4.M() > settings.GetLowerZMassCut() &&
				product.m_zLeptons.first->p4.Pt() > settings.GetLeadingLeptonFromZLowerPtCut());
		}
		const KPFCandidate* determine_pfCandidate_for_lepton(KappaProduct& product, KappaSettings const& settings,
			bool first_lepton) const
		{
			KLepton* reference_lepton;
			std::vector<const KPFCandidate*>* pf_candidate_collection = nullptr;
			if(first_lepton) reference_lepton = product.m_zLeptons.first;
			else reference_lepton = product.m_zLeptons.second;
			if(reference_lepton->flavour() == KLeptonFlavour::ELECTRON) pf_candidate_collection = &product.m_pfElectrons;
			else if(product.m_zLeptons.first->flavour() == KLeptonFlavour::MUON) pf_candidate_collection = &product.m_pfMuons;
			else return nullptr;

			if(pf_candidate_collection != nullptr)
			{
				for(unsigned int i = 0; i < pf_candidate_collection->size(); i++)
				{
					if(ROOT::Math::VectorUtil::DeltaR(reference_lepton->p4,pf_candidate_collection->at(i)->p4) < settings.GetdeltaRTolleranceForPF()
						&& std::abs(reference_lepton->p4.Pt() - pf_candidate_collection->at(i)->p4.Pt()) < settings.GetPtTolleranceForPF())
							return pf_candidate_collection->at(i);
				}
				return nullptr;
			}
			else return nullptr;
		}
		double calculate_theta_Z_LepMinus(KappaProduct& product) const
		{
			ROOT::Math::Boost boost_to_Z_restframe(product.m_z.p4.BoostToCM());
			KLepton* LepMinus = product.m_zLeptons.first->charge() < 0 ? product.m_zLeptons.first : product.m_zLeptons.second;
			RMFLV z4momentum =  product.m_z.p4;
			RMFLV LepMinus_4momentum_in_Z_restframe = boost_to_Z_restframe * LepMinus->p4;
			RMFLV::BetaVector z,lm;
			z.SetXYZ(z4momentum.Px(),z4momentum.Py(),z4momentum.Pz());
			lm.SetXYZ(LepMinus_4momentum_in_Z_restframe.Px(),LepMinus_4momentum_in_Z_restframe.Py(),LepMinus_4momentum_in_Z_restframe.Pz());
			z = z.Unit();
			lm = lm.Unit();
			return std::acos(z.Dot(lm));
		}
};

class ZmmProducer : public ZProducerBase<KMuon, KMuon>
{
public:
	std::string GetProducerId() const override;
	ZmmProducer();
};

class ZeeProducer : public ZProducerBase<KElectron, KElectron>
{
public:
	std::string GetProducerId() const override;
	ZeeProducer();
};

class ZemProducer : public ZProducerBase<KElectron, KMuon>
{
public:
	std::string GetProducerId() const override;
	ZemProducer();
};

class ZeemmProducer : public ZProducerBase<KElectron, KMuon>
{
public:
	std::string GetProducerId() const override;
	ZeemmProducer();
};

