#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"

/** Producer class for Z boson reconstruction from muons/electrons.
 *
 *	Needs to run after the valid object producers.
 * 	Needs at least 2 valid leptons to construct a Z.
 */

template <class TLepton1, class TLepton2, class TParticle>
class ZProducerBase : public KappaProducerBase
{
  public:
	ZProducerBase(std::vector<TLepton1*> KappaProduct::*validLeptons1,
				  std::vector<TLepton2*> KappaProduct::*validLeptons2, 
				  bool check_same_collection=true, 
				  bool check_cross_collection=false)
		: KappaProducerBase(),
		  m_validLeptonsMember1(validLeptons1),
		  m_validLeptonsMember2(validLeptons2),
		  check_first_ll_collection(check_same_collection),
		  check_second_ll_collection(check_same_collection),
		  check_cross_ll_collection(check_cross_collection)
	{
	}
	ZProducerBase(std::vector<TLepton1*> KappaProduct::*validLeptons1)
		: KappaProducerBase(),
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
                        return product.m_zLeptons.first->p4.Pt();
                });

		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingLeptonFromZEta", [](KappaEvent const & event, KappaProduct const & product)
                {
                        return product.m_zLeptons.first->p4.Eta();
                });

		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingLeptonFromZPhi", [](KappaEvent const & event, KappaProduct const & product)
                {
                        return product.m_zLeptons.first->p4.Phi();
                });

		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingLeptonFromZPt", [](KappaEvent const & event, KappaProduct const & product)
                {
                        return product.m_zLeptons.second->p4.Pt();
                });

		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingLeptonFromZEta", [](KappaEvent const & event, KappaProduct const & product)
                {
                        return product.m_zLeptons.second->p4.Eta();
                });

		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingLeptonFromZPhi", [](KappaEvent const & event, KappaProduct const & product)
                {
                        return product.m_zLeptons.second->p4.Phi();
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
	int found_zs = 0;
        resetZ(product);
        
	if (check_first_ll_collection){
	  for (unsigned int i = 0; i < (product.*m_validLeptonsMember1).size(); ++i) {	
	    for (unsigned int j = 0; j < i; ++j){
		TParticle* const lep1 = (product.*m_validLeptonsMember1).at(i);
		TParticle* const lep2 = (product.*m_validLeptonsMember1).at(j);
		if (lepton_pair_onZ(lep1,lep2,settings)){
		   found_zs++;
		   if (is_closer_to_Z((lep1->p4+lep2->p4).M(),product,settings))
		     setZ(product,lep1,lep2);
		}
	    }
	  }
	}
	
	// OK not the most elegant way, but at least it is understandable. 
	// If you have time you can also bring this double loop into a function which takes carre of differnt types of m_validLeptonsMember1/2
	if (check_second_ll_collection){
	  for (unsigned int i = 0; i < (product.*m_validLeptonsMember2).size(); ++i) {	
	    for (unsigned int j = 0; j < i; ++j){
		TParticle* const lep1 = (product.*m_validLeptonsMember2).at(i);
		TParticle* const lep2 = (product.*m_validLeptonsMember2).at(j);
		if (lepton_pair_onZ(lep1,lep2,settings)){
		   found_zs++;
		   if (is_closer_to_Z((lep1->p4+lep2->p4).M(),product,settings))
		     setZ(product,lep1,lep2);
		}
	    }
	  }
	}	
	
	if (check_cross_ll_collection){
	  for (unsigned int i = 0; i < (product.*m_validLeptonsMember1).size(); ++i) {	
	    for (unsigned int j = 0; j < (product.*m_validLeptonsMember2).size(); ++j){
		TParticle* const lep1 = (product.*m_validLeptonsMember1).at(i);
		TParticle* const lep2 = (product.*m_validLeptonsMember2).at(j);
		if (lepton_pair_onZ(lep1,lep2,settings)){
		   found_zs++;
		   if (is_closer_to_Z((lep1->p4+lep2->p4).M(),product,settings))
		     setZ(product,lep1,lep2);
		}
	    }
	  }
	}
	
	if (found_zs >1 && settings.GetVetoMultipleZs()) resetZ(product);
	
	
	return;
}
  protected:
	std::vector<TLepton1*> KappaProduct::*m_validLeptonsMember1;
	std::vector<TLepton2*> KappaProduct::*m_validLeptonsMember2;
	bool check_first_ll_collection;
	bool check_second_ll_collection;
	bool check_cross_ll_collection;
        bool lepton_pair_onZ(TParticle* const lep1, TParticle* const lep2, KappaSettings const& settings) const
	{
	     if(lep1->charge() + lep2->charge() != 0){
	     	return false; // if charge not 0 can't be from Z 
	     }
	     KLV z_test;
	     z_test.p4 = lep1->p4 + lep2->p4;
	     double test_mass_diff = fabs(z_test.p4.M() - settings.GetZMass());
             return (test_mass_diff < settings.GetZMassRange()); // test if invM is in Z Range
        }
       virtual bool is_closer_to_Z(double zCandidate_mass, KappaProduct& product, KappaSettings const& settings) const {
		return false;
        }
       virtual void resetZ(KappaProduct& product) const{
        }
       virtual void setZ(KappaProduct& product, TParticle* const lep1, TParticle* const lep2) const
        {
        }
        
        
        
};
//RecoZProducer
template <class TLepton1, class TLepton2>
class RecoZProducer : public ZProducerBase<TLepton1, TLepton2, KLepton>
{
    public:
	RecoZProducer(std::vector<TLepton1*> KappaProduct::*validLeptons1,
				  std::vector<TLepton2*> KappaProduct::*validLeptons2, 
				  bool check_same_collection=true, 
				  bool check_cross_collection=false)
		: ZProducerBase<TLepton1, TLepton2, KLepton>(validLeptons1, validLeptons2, check_same_collection, check_cross_collection)
	{
	}
	RecoZProducer(std::vector<TLepton1*> KappaProduct::*validLeptons1)
		: ZProducerBase<TLepton1, TLepton2, KLepton>(validLeptons1)
	{
	}
    protected:
	void resetZ(KappaProduct& product) const override{
            product.m_z = KLV();
            product.m_z.p4.SetPt(0.0f);  // just to be sure
            product.m_zLeptons = std::make_pair(nullptr, nullptr);
            product.m_zValid = false;
        }
	bool is_closer_to_Z(double zCandidate_mass, KappaProduct& product, KappaSettings const& settings) const override{
            if (!product.m_zValid) return true;
            return (fabs(zCandidate_mass-settings.GetZMass()) < fabs(product.m_z.p4.M()-settings.GetZMass()));
        }
        void setZ(KappaProduct& product, KLepton* const lep1, KLepton* const lep2) const override
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
            product.m_zValid = true;
        }    
};
class ZmmProducer : public RecoZProducer<KMuon, KMuon>
{
  public:
	std::string GetProducerId() const override { return "ZmmProducer"; };
	ZmmProducer()
		: RecoZProducer<KMuon, KMuon>(&KappaProduct::m_validMuons)
	{
	}
};
class ZeeProducer : public RecoZProducer<KElectron, KElectron>
{
  public:
	std::string GetProducerId() const override { return "ZeeProducer"; };
	ZeeProducer()
		: RecoZProducer<KElectron, KElectron>(&KappaProduct::m_validElectrons)
	{
	}
};

class ZemProducer : public RecoZProducer<KElectron, KMuon>
{
  public:
	std::string GetProducerId() const override { return "ZemProducer"; };
	ZemProducer()
		: RecoZProducer<KElectron, KMuon>(&KappaProduct::m_validElectrons, &KappaProduct::m_validMuons,false,true)
	{
	}
};


class ZeemmProducer : public RecoZProducer<KElectron, KMuon>
{
  public:
	std::string GetProducerId() const override { return "ZeemmProducer"; };
	ZeemmProducer()
		: RecoZProducer<KElectron, KMuon>(&KappaProduct::m_validElectrons, &KappaProduct::m_validMuons,true,false)
	{
	}
};

//--------------------------------------------------------------------------------------------------
//GenZProducer
class GenZProducer : public ZProducerBase<KGenParticle, KGenParticle, KGenParticle>
{
    public:
	GenZProducer(std::vector<KGenParticle*> KappaProduct::*validLeptons1,
				  std::vector<KGenParticle*> KappaProduct::*validLeptons2, 
				  bool check_same_collection=true, 
				  bool check_cross_collection=false)
		: ZProducerBase<KGenParticle, KGenParticle, KGenParticle>(validLeptons1, validLeptons2, check_same_collection, check_cross_collection)
	{
	}
	GenZProducer(std::vector<KGenParticle*> KappaProduct::*validLeptons1)
		: ZProducerBase<KGenParticle, KGenParticle, KGenParticle>(validLeptons1)
	{
	}
    protected:
	void resetZ(KappaProduct& product) const override{
            product.m_genz = KLV();
            product.m_genz.p4.SetPt(0.0f);  // just to be sure
            product.m_genzLeptons = std::make_pair(nullptr, nullptr);
            product.m_genzValid = false;
        }
	bool is_closer_to_Z(double zCandidate_mass, KappaProduct& product, KappaSettings const& settings) const override{
            if (!product.m_genzValid) return true;
            return (fabs(zCandidate_mass-settings.GetZMass()) < fabs(product.m_genz.p4.M()-settings.GetZMass()));
        }
        void setZ(KappaProduct& product, KGenParticle* const lep1, KGenParticle* const lep2) const override
        {
            KLV zCandidate;
            zCandidate.p4 = lep1->p4 + lep2->p4;
            product.m_genz = zCandidate;
	    std::pair<KGenParticle*, KGenParticle*> genzLeptons;
            if (lep1->p4.Pt() > lep2->p4.Pt())
               genzLeptons = std::make_pair(lep1, lep2);
            else
               genzLeptons = std::make_pair(lep2, lep1);
            product.m_genzLeptons = genzLeptons;
            product.m_genzValid = true;
        }    
};

class GenZmmProducer : public GenZProducer
{
  public:
	std::string GetProducerId() const override { return "GenZmmProducer"; };
	GenZmmProducer()
		: GenZProducer(&KappaProduct::m_genMuons)
	{
	}
};
class GenZeeProducer : public GenZProducer
{
  public:
	std::string GetProducerId() const override { return "GenZeeProducer"; };
	GenZeeProducer()
		: GenZProducer(&KappaProduct::m_genElectrons)
	{
	}
};

class GenZemProducer : public GenZProducer
{
  public:
	std::string GetProducerId() const override { return "GenZemProducer"; };
	GenZemProducer()
		: GenZProducer(&KappaProduct::m_genElectrons, &KappaProduct::m_genMuons,false,true)
	{
	}
};


class GenZeemmProducer : public GenZProducer
{
  public:
	std::string GetProducerId() const override { return "GenZeemmProducer"; };
	GenZeemmProducer()
		: GenZProducer(&KappaProduct::m_genElectrons, &KappaProduct::m_genMuons,true,false)
	{
	}
};
