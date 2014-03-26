/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"

#include "DrawConsumerBase.h"
#include "Hist1D.h"
/*
 typedef std::function<
 std::vector<float>(MassRecoEvent const&, MassRecoProduct const&)> ValueExtractLambda;
 typedef std::pair<ValueExtractLambda, ValueModifiers> ValueDesc;
 */
template<class TTypes>
class DrawHist1dConsumerBase: public DrawConsumerBase<TTypes> {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	typedef std::function<
			std::vector<float>(event_type const&, product_type const&
                                )> ValueExtractLambda;
	typedef std::pair<ValueExtractLambda, ValueModifiers> ValueDesc;

	DrawHist1dConsumerBase(std::string const& histName, ValueDesc desc) :
			DrawConsumerBase<TTypes>(), m_histName(histName), m_desc(desc) {

	}

	virtual ~DrawHist1dConsumerBase() {
	}

	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE {
		return "hist1d";
	}

	virtual void Init(Pipeline<TTypes> * pset) ARTUS_CPP11_OVERRIDE {
		DrawConsumerBase<TTypes>::Init(pset);

		// init called
		const std::string rootFolder = pset->GetSettings().GetRootFileFolder();
		Hist1D * hist = new Hist1D(m_histName, rootFolder, m_desc.second);
		setHist(hist);
		assert(m_hist);
		m_hist->Init();
	}

	virtual void Finish() ARTUS_CPP11_OVERRIDE {
		// store hist
		// + modifiers
		//ARTUS_LOG( "Storing Hist for " << this->GetProductName() )
		m_hist->Store(this->GetPipelineSettings().GetRootOutFile());
	}

	virtual void ProcessFilteredEvent(event_type const& event,
			product_type const& product ) ARTUS_CPP11_OVERRIDE {

		DrawConsumerBase<TTypes>::ProcessFilteredEvent(event, product );

		auto res = m_desc.first(event, product);

		for (auto const& v : res) {
			getHist()->Fill(v, 1.0f);
		}
	}

	Hist1D* getHist() {
		return m_hist;
	}

	void setHist(Hist1D * hh) {
		m_hist = hh;
	}
private:
	std::string m_histName;
	ValueDesc m_desc;
	// already configured histogramm
	Hist1D* m_hist;
};
