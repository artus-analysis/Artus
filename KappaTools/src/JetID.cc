/* Copyright (c) 2011 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 *   Manuel Zeise <zeise@cern.ch>
 */

#include "Artus/KappaTools/interface/JetID.h"

bool JetIDLoose<KCaloJet>::operator()(const struct KCaloJet &jet) const
{
	if (
		((fabs(jet.p4.eta()) > 2.6) || (jet.fEM > 0.01)) &&		// Remove HCAL noise for |eta|
		(jet.fHPD < 0.98) &&	// Remove defective HCAL cells
		(jet.n90Hits > 1)	// Remove ECAL spikes
		)
		return true;
	else
		return false;
}

bool JetIDLoose<KBasicJet>::operator()(const struct KBasicJet &jet) const
{
	if (
		(jet.photonFraction < 0.99) &&
		(jet.neutralHadronFraction < 0.99) &&
		(jet.nConstituents > 1)
		)
	{
		if (fabs(jet.p4.eta()) < 2.4)
		{
			if (
				(jet.electronFraction < 0.99) &&
				(jet.chargedHadronFraction > 0) &&
				(jet.nCharged > 0)
				)
				return true;
			return false;
		}
		return true;
	}
	return false;
}

bool JetIDTight<KBasicJet>::operator()(const class KBasicJet &jet) const
{
	if (
		(jet.photonFraction < 0.90) &&
		(jet.neutralHadronFraction < 0.90) &&
		(jet.nConstituents > 1)
		)
	{
		if (fabs(jet.p4.eta()) < 2.4)
		{
			if (
				(jet.electronFraction < 0.99) &&
				(jet.chargedHadronFraction > 0) &&
				(jet.nCharged > 0)
				)
				return true;
			return false;
		}
		return true;
	}
	return false;
}
