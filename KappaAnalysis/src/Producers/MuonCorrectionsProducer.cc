
#include "Artus/KappaAnalysis/interface/Producers/MuonCorrectionsProducer.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "Artus/Utility/interface/SafeMap.h"
#include "Artus/Utility/interface/Utility.h"
#include "TLorentzVector.h"

std::string MuonCorrectionsProducer::GetProducerId() const {
	return "MuonCorrectionsProducer";
}

void MuonCorrectionsProducer::Init(KappaSettings const& settings) 
{
	KappaProducerBase::Init(settings);
	muonEnergyCorrection = ToMuonEnergyCorrection(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetMuonEnergyCorrection())));
	if (muonEnergyCorrection == MuonEnergyCorrection::ROCHCORR2015)
	{
		rmcor2015 = new rochcor2015(settings.GetMuonRochesterCorrectionsFile());
		
	}
	if (muonEnergyCorrection == MuonEnergyCorrection::ROCHCORR2016)
	{
		rmcor2016 = new rochcor2016(settings.GetMuonRochesterCorrectionsFile());
	}
}

void MuonCorrectionsProducer::Produce(KappaEvent const& event, KappaProduct& product,
                     KappaSettings const& settings) const
{
	assert(event.m_muons);

	// create a copy of all muons in the event
	product.m_correctedMuons.clear();
	product.m_correctedMuons.resize(event.m_muons->size());
	size_t muonIndex = 0;
	for (KMuons::const_iterator muon = event.m_muons->begin();
		 muon != event.m_muons->end(); ++muon)
	{
		product.m_correctedMuons[muonIndex] = std::shared_ptr<KMuon>(new KMuon(*muon));
		product.m_originalLeptons[product.m_correctedMuons[muonIndex].get()] = &(*muon);
		++muonIndex;
	}
	
	// perform corrections on copied muons
	for (std::vector<std::shared_ptr<KMuon> >::iterator muon = product.m_correctedMuons.begin();
		 muon != product.m_correctedMuons.end(); ++muon)
	{
		// No general correction available
	
		// perform possible analysis-specific corrections
		if (muonEnergyCorrection == MuonEnergyCorrection::FALL2015)
	{
		muon->get()->p4 = muon->get()->p4 * (1.0);
	}
		else if (muonEnergyCorrection == MuonEnergyCorrection::ROCHCORR2015)
		{
			TLorentzVector mu;
			mu.SetPtEtaPhiM(muon->get()->p4.Pt(),muon->get()->p4.Eta(),muon->get()->p4.Phi(),muon->get()->p4.mass());
	
			float q = muon->get()->charge();
			float qter = 1.0;
	
			if (settings.GetInputIsData())
			{
				rmcor2015->momcor_data(mu, q, 0, qter);
				muon->get()->p4.SetPxPyPzE(mu.Px(),mu.Py(),mu.Pz(),mu.E());
			}
			else
			{
			int ntrk = muon->get()->track.nPixelLayers + muon->get()->track.nStripLayers; // TODO: this corresponds to reco::HitPattern::trackerLayersWithMeasurementOld(). update to "new" impleme	ntation also in Kappa
				rmcor2015->momcor_mc(mu, q, ntrk, qter);
				muon->get()->p4.SetPxPyPzE(mu.Px(),mu.Py(),mu.Pz(),mu.E());
			}
		}
		else if (muonEnergyCorrection == MuonEnergyCorrection::ROCHCORR2016)
		{
			TLorentzVector mu;
			mu.SetPtEtaPhiM(muon->get()->p4.Pt(),muon->get()->p4.Eta(),muon->get()->p4.Phi(),muon->get()->p4.mass());
	
			float q = muon->get()->charge();
			float qter = 1.0;

			if (settings.GetInputIsData())
			{
				rmcor2016->momcor_data(mu, q, 0, qter);
				muon->get()->p4.SetPxPyPzE(mu.Px(),mu.Py(),mu.Pz(),mu.E());
			}
			else
			{
				int ntrk = muon->get()->track.nPixelLayers + muon->get()->track.nStripLayers; // TODO: this corresponds to reco::HitPattern::trackerLayersWithMeasurementOld(). update to "new" implementation also in Kappa
				rmcor2016->momcor_mc(mu, q, ntrk, qter);
				muon->get()->p4.SetPxPyPzE(mu.Px(),mu.Py(),mu.Pz(),mu.E());
			}
		}
		else if (muonEnergyCorrection != MuonEnergyCorrection::NONE)
		{
			LOG(FATAL) << "Muon energy correction of type " << Utility::ToUnderlyingValue(muonEnergyCorrection) << " not yet implemented!";
		}
		AdditionalCorrections(muon->get(), event, product, settings);
		
	}
	
	// sort vectors of corrected muons by pt
	std::sort(product.m_correctedMuons.begin(), product.m_correctedMuons.end(),
	          [](std::shared_ptr<KMuon> muon1, std::shared_ptr<KMuon> muon2) -> bool
	          { return muon1->p4.Pt() > muon2->p4.Pt(); });
}


// Can be overwritten for analysis-specific use cases
void MuonCorrectionsProducer::AdditionalCorrections(KMuon* muon, KappaEvent const& event,
                                   KappaProduct& product, KappaSettings const& settings) const
{
	
}


