/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Consumer/interface/ValueModifier.h"

#include "Artus/Consumer/interface/DrawHist1dConsumer.h"
#include "Artus/Consumer/interface/ProfileConsumerBase.h"

#include "TraxTypes.h"

#include "TraxPipelineSettings.h"
#include "TraxEvent.h"
#include "TraxProduct.h"

// filter
#include "PtFilter.h"

// consumer
#include "MeanPtConsumer.h"
#include "TraxNtupleConsumer.h"

class TraxPipelineInitializer: public PipelineInitilizerBase<TraxTypes > {
public:

	virtual void InitPipeline(TraxPipeline * pLine,
			TraxSettings const& pset) const override
			{

		// define how to extract Pt and the range
		auto extractPtSim =
				[]( TraxEvent const& ev, TraxProduct const & prod )
				-> std::vector<float> {return {ev.m_floatPtSim};};
		auto PtSimValue = std::make_pair(extractPtSim,
				DefaultModifiers::getPtModifier(0.7, 1.3f));

		// extracts the value which has been corrected by a globalProducer
		auto extractPtSimCorrected =
				[]( TraxEvent const& ev, TraxProduct const & prod )
				-> std::vector<float> {return {prod.m_floatPtSim_corrected};};
		auto PtSimCorrectedValue = std::make_pair(extractPtSimCorrected,
				DefaultModifiers::getPtModifier(0.7, 1.3f));

		// define how to extract Theta and the range
		auto extractThetaSim =
				[]( TraxEvent const& ev, TraxProduct const & prod )
				-> std::vector<float> {return {ev.m_floatTheSim};};

		auto ThetaSimValue = std::make_pair(extractThetaSim,
				DefaultModifiers::getThetaModifier());


		BOOST_FOREACH(std::string id, pset.GetConsumers())
		{
			// the quantities_all serves as an alias which
			// will install custom producers to pipeline
			if (id == "quantities_all")
			{
				// plot Pt
				pLine->AddConsumer(
						new DrawHist1dConsumerBase<TraxTypes>("pt", PtSimValue));

				// plot Pt - corrected, from the global product
				pLine->AddConsumer(
						new DrawHist1dConsumerBase<TraxTypes>("pt_corr", PtSimCorrectedValue));


				// plot Theta
				pLine->AddConsumer(
						new DrawHist1dConsumerBase<TraxTypes>("theta", ThetaSimValue));

				// profile Pt over Theta
				pLine->AddConsumer(
						new ProfileConsumerBase<TraxTypes>("pt_over_theta",
							ThetaSimValue, PtSimValue));
			}
		}

	}
};
