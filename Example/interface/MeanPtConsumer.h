
#pragma once

#include "TraxTypes.h"

/*
 * IMPORTANT NOTE:
 * Not nummericaly stable. Don't use in real world applications
 * This is just to illustrate an easy consumer.
 */

class MeanPtConsumer: public TraxConsumerBase {
public:
	MeanPtConsumer();

	std::string GetConsumerId() const override;

	void ProcessFilteredEvent(event_type const& event, product_type const& product,
			setting_type const& settings, metadata_type const& metadata) override;

	void Finish(setting_type const& settings, metadata_type const& metadata) override;

private:
	long long m_itemCount;
	double m_mean;
};

