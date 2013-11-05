#pragma once

#include "DrawConsumerBase.h"

template<class TTypes>
class DrawHist1dConsumerBase: public DrawConsumerBase<TTypes> {
public:

	DrawHist1dConsumerBase() :
			DrawConsumerBase<TTypes>(), m_hist(NULL) {
	}

	virtual ~DrawHist1dConsumerBase() {
	}

	virtual void Init(EventPipeline<TTypes> * pset) {
		DrawConsumerBase<TTypes>::Init(pset);

		assert(m_hist);
		//CALIB_LOG( "Initializing Hist for " << this->GetProductName() )
		//m_hist->SetNameAndCaption( this->GetProductName());
		m_hist->SetRootFileFolder(
				this->GetPipelineSettings().GetRootFileFolder());
		m_hist->Init(Hist1D::ModifierList());
	}

	virtual void Finish() {
		// store hist
		// + modifiers
		//CALIB_LOG( "Storing Hist for " << this->GetProductName() )
		std::cout << "STORING HIST" << std::endl;
		m_hist->Store(this->GetPipelineSettings().GetRootOutFile());
	}

	Hist1D* getHist() {
		return m_hist;
	}

	void setHist(Hist1D * hh) {
		m_hist = hh;
	}

	// already configured histogramm
	Hist1D* m_hist;
};
