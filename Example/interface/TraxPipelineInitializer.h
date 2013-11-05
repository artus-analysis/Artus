#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Draw/interface/ValueModifier.h"

#include "Artus/Draw/interface/DrawHist1dConsumer.h"
#include "Artus/Draw/interface/ProfileConsumerBase.h"

#include "TraxTypes.h"

#include "TraxPipelineSettings.h"
#include "TraxEventData.h"
#include "TraxMetaData.h"

// filter
#include "PtFilter.h"

// consumer
#include "MeanPtConsumer.h"

class TraxPipelineInitilizer: public PipelineInitilizerBase<TraxTypes> {
public:

	virtual void InitPipeline(TraxPipeline * pLine,
			TraxPipelineSettings const& pset) const ARTUS_CPP11_OVERRIDE
			{

		pLine->AddFilter(new PtFilter());
		pLine->AddConsumer(new MeanPtConsumer());

		typedef std::function<
				std::vector<float>(event_type const&, global_meta_type const&,
						local_meta_type const&)> ValueExtractLambda;
		typedef std::pair<ValueExtractLambda, ValueModifiers> ValueDesc;

		// define how to extract Pt and the range
		auto extractPtSim =
				[]( TraxEventData const& ev, TraxGlobalMetaData const & gm, TraxLocalMetaData const & lm )
				-> std::vector<float> {return {ev.m_floatPtSim};};

		auto PtSimValue = std::make_pair(extractPtSim,
				DefaultModifiers::getPtModifier(0.8, 1.2f));

		// define how to extract Theta and the range
		auto extractThetaSim =
				[]( TraxEventData const& ev, TraxGlobalMetaData const & gm, TraxLocalMetaData const & lm )
				-> std::vector<float> {return {ev.m_floatTheSim};};

		auto ThetaSimValue = std::make_pair(extractThetaSim,
				DefaultModifiers::getThetaModifier());

		// plot Pt
		pLine->AddConsumer(
				new DrawHist1dConsumerBase<TraxTypes>("pt", PtSimValue));

		// plot Theta
		pLine->AddConsumer(
				new DrawHist1dConsumerBase<TraxTypes>("theta", ThetaSimValue));

		// profile Pt over Theta
		pLine->AddConsumer(
				new ProfileConsumerBase<TraxTypes>("pt_over_theta",
						ThetaSimValue, PtSimValue));

	}
};
