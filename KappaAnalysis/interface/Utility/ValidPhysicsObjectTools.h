
#pragma once

#include "Artus/Utility/interface/ArtusLogging.h"

#include <algorithm>

#include <boost/regex.hpp>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Utility/interface/Utility.h"


/**
   \brief GlobalProducer, for valid electrons.
   
   
*/
template<class TTypes, class TPhysicsObject>
class ValidPhysicsObjectTools
{
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	
	ValidPhysicsObjectTools(std::vector<TPhysicsObject*> product_type::*validPhysicsObjects) :
		m_validPhysicsObjectsMember(validPhysicsObjects)
	{
	}


protected:
	std::map<size_t, std::vector<float> > lowerPtCutsByIndex;
	std::map<std::string, std::vector<float> > lowerPtCutsByHltName;
	std::map<size_t, std::vector<float> > upperAbsEtaCutsByIndex;
	std::map<std::string, std::vector<float> > upperAbsEtaCutsByHltName;
	
	bool PassKinematicCuts(TPhysicsObject* physicsObject, event_type const& event, product_type& product) const
	{
		bool validObject = true;
		
		for (std::map<size_t, std::vector<float> >::const_iterator lowerPtCutByIndex = lowerPtCutsByIndex.begin();
		     lowerPtCutByIndex != lowerPtCutsByIndex.end() && validObject; ++lowerPtCutByIndex)
		{
			if ((physicsObject->p4.Pt() < *std::max_element(lowerPtCutByIndex->second.begin(), lowerPtCutByIndex->second.end()))
			    && (lowerPtCutByIndex->first == (product.*m_validPhysicsObjectsMember).size()))
			{
				validObject = false;
			}
		}
		
		for (std::map<size_t, std::vector<float> >::const_iterator upperAbsEtaCutByIndex = upperAbsEtaCutsByIndex.begin();
		     upperAbsEtaCutByIndex != upperAbsEtaCutsByIndex.end() && validObject; ++upperAbsEtaCutByIndex)
		{
			if ((std::abs(physicsObject->p4.Eta()) > *std::min_element(upperAbsEtaCutByIndex->second.begin(), upperAbsEtaCutByIndex->second.end()))
			    && (upperAbsEtaCutByIndex->first == (product.*m_validPhysicsObjectsMember).size()))
			{
				validObject = false;
			}
		}
		
		for (std::map<std::string, std::vector<float> >::const_iterator lowerPtCutByHltName = lowerPtCutsByHltName.begin();
		     lowerPtCutByHltName != lowerPtCutsByHltName.end() && validObject; ++lowerPtCutByHltName)
		{
			if ((physicsObject->p4.Pt() < *std::max_element(lowerPtCutByHltName->second.begin(), lowerPtCutByHltName->second.end()))
			    &&
			    (
			    	(lowerPtCutByHltName->first == "default")
			    	||
			    	boost::regex_search(product.m_selectedHltName, boost::regex(lowerPtCutByHltName->first, boost::regex::icase | boost::regex::extended))
			    )
			   )
			{
				validObject = false;
			}
		}
		
		for (std::map<std::string, std::vector<float> >::const_iterator upperAbsEtaCutByHltName = upperAbsEtaCutsByHltName.begin();
		     upperAbsEtaCutByHltName != upperAbsEtaCutsByHltName.end() && validObject; ++upperAbsEtaCutByHltName)
		{
			if ((std::abs(physicsObject->p4.Eta()) > *std::min_element(upperAbsEtaCutByHltName->second.begin(), upperAbsEtaCutByHltName->second.end()))
			    &&
			    (
			    	(upperAbsEtaCutByHltName->first == "default")
			    	||
			    	boost::regex_search(product.m_selectedHltName, boost::regex(upperAbsEtaCutByHltName->first, boost::regex::icase | boost::regex::extended))
			    )
			   )
			{
				validObject = false;
			}
		}

		return validObject;
	}

private:
	std::vector<TPhysicsObject*> product_type::*m_validPhysicsObjectsMember;

};

