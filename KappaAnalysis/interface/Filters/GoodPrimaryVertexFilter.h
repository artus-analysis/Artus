
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/FilterBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaEnumTypes.h"


/** Filter events
 *  Config tags:
 *   - GenDiLeptonDecayMode
 */
class GoodPrimaryVertexFilter: public FilterBase<KappaTypes>
{

public:

	std::string GetFilterId() const override;
	void Init(setting_type const& settings) override;
	bool DoesEventPass(KappaEvent const& event, KappaProduct const& product,
	                           KappaSettings const& settings) const override;

private:

	float m_maxPrimaryVertexZ;
	float m_maxPrimaryVertexRho;
	float m_minPrimaryVertexFitnDOF;
};
