#pragma once

#include "Artus/Core/interface/Cpp11Support.h"

#include "TraxConsumer.h"

/*
 * IMPORTANT NOTE:
 * Not numericaly stable. Don't use in real world applications
 * This is just to illustrate an easy consumer.
 */

class MeanPtConsumer: public TraxConsumerBase {
public:
	MeanPtConsumer() :
			m_itemCount(0), m_mean(0.0f) {

	}

	virtual void ProcessFilteredEvent(TraxEventData const& event,
			TraxGlobalMetaData const& globalMetaData,
			TraxLocalMetaData const& localMetaData) ARTUS_CPP11_OVERRIDE {

		m_mean = ((m_mean * m_itemCount) + event.m_floatPtSim)
				/ (m_itemCount + 1);

		m_itemCount++;

	}

	virtual void Finish() ARTUS_CPP11_OVERRIDE {
		// todo: print pipeline name here
		std::cout << "Track mean Pt is " << m_mean << " [GeV]" << std::endl;
	}

private:
	long long m_itemCount;
	double m_mean;
};
