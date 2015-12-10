#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"

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
				  bool same_ll_collection)
		: KappaProducerBase(),
		  m_validLeptonsMember1(validLeptons1),
		  m_validLeptonsMember2(validLeptons2),
		  m_same_ll_collection(same_ll_collection)
	{
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
	KLV zCandidate;
	std::pair<KLepton*, KLepton*> zLeptons;

	// do not double count when matching leptons from the same collection
	for (unsigned int i = 0; i < (product.*m_validLeptonsMember1).size(); ++i) {
		for (unsigned int j = (m_same_ll_collection ? i + 1 : 0);
		     j < (product.*m_validLeptonsMember2).size(); ++j) {
			KLepton* const m1 = (product.*m_validLeptonsMember1).at(i);
			KLepton* const m2 = (product.*m_validLeptonsMember2).at(j);
			// valid Z is neutral and close to Z mass
			if (m1->charge() + m2->charge() == 0) {
				KLV z;
				z.p4 = m1->p4 + m2->p4;
				if (z.p4.mass() > settings.GetZMass() - settings.GetZMassRange() &&
					z.p4.mass() < settings.GetZMass() + settings.GetZMassRange()) {
					// allow only 1 Z per event
					if (++found_zs > 1)
						break;
					zCandidate = z;
					if (m1->p4.Pt() > m2->p4.Pt())
						zLeptons = std::make_pair(m1, m2);
					else
						zLeptons = std::make_pair(m2, m1);
				}
			}
		}
		if (found_zs > 1)
			break;
	}
	if (found_zs == 1) {
		product.m_z = zCandidate;
		product.m_zLeptons = zLeptons;
		product.m_zValid = true;
	} else {
		product.m_z = KLV();
		product.m_z.p4.SetPt(0.0f);  // just to be sure
		product.m_zLeptons = std::make_pair(nullptr, nullptr);
		product.m_zValid = false;
	}
	return;
}
  private:
	std::vector<TLepton1*> KappaProduct::*m_validLeptonsMember1;
	std::vector<TLepton2*> KappaProduct::*m_validLeptonsMember2;
	bool m_same_ll_collection;
};

class ZmmProducer : public ZProducerBase<KMuon, KMuon>
{
  public:
	std::string GetProducerId() const override { return "ZmmProducer"; };
	ZmmProducer()
		: ZProducerBase<KMuon, KMuon>(&KappaProduct::m_validMuons, &KappaProduct::m_validMuons, true)
	{
	}
};

class ZeeProducer : public ZProducerBase<KElectron, KElectron>
{
  public:
	std::string GetProducerId() const override { return "ZeeProducer"; };
	ZeeProducer()
		: ZProducerBase<KElectron, KElectron>(&KappaProduct::m_validElectrons,
											  &KappaProduct::m_validElectrons,
											  true)
	{
	}
};

class ZemProducer : public ZProducerBase<KElectron, KMuon>
{
  public:
	std::string GetProducerId() const override { return "ZemProducer"; };
	ZemProducer()
		: ZProducerBase<KElectron, KMuon>(&KappaProduct::m_validElectrons,
										  &KappaProduct::m_validMuons,
										  false)
	{
	}
};
