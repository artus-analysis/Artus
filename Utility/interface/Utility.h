
#pragma once

#include "Artus/Utility/interface/ArtusLogging.h"

#include <map>
#include <type_traits>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

#include <Math/LorentzVector.h>
#include <Math/PtEtaPhiM4D.h>
#include <Math/DisplacementVector3D.h>
#include <Math/Cartesian3D.h>
#include <Math/SMatrix.h>
#include <Math/VectorUtil.h>


typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float> > RMFLV;
typedef ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<float>,ROOT::Math::DefaultCoordinateSystemTag> RMDataV;
typedef ROOT::Math::SMatrix<double, 2, 2, ROOT::Math::MatRepSym<double, 2> > RMSM2x2;

namespace Utility {

	template<typename Enum>
	using enableEnumType = typename std::enable_if< std::is_enum<Enum>::value, typename std::underlying_type<Enum>::type>::type;

	// convert enum class type to underlying (int) value
	template<typename Enum>
	constexpr inline enableEnumType<Enum> ToUnderlyingValue(Enum e) noexcept
	{
		return static_cast<typename std::underlying_type<Enum>::type>(e);
	}

	// convert underlying (int) value to enum class type
	template< typename Enum , typename T>
	constexpr inline typename std::enable_if<std::is_enum<Enum>::value && std::is_integral<T>::value, Enum>::type ToEnum(T value) noexcept
	{
		return static_cast<Enum>( value );
	}

	std::string ReadStringFromFile(std::string fileName);

	// parse vector of strings into map of strings by splitting keys and values by the delimiters
	std::map<std::string, std::vector<std::string> > ParseVectorToMap(std::vector<std::string> const& vectorToParse,
	                                                                  std::string const& delimiters=":");
	
	// convert string map into other type by using boost::lexical_cast
	template<class TKey, class TValue>
	std::map<TKey, std::vector<TValue> > ParseMapTypes(std::map<std::string, std::vector<std::string> > const& inputMap,
	                                                   std::map<std::string, std::vector<TValue> > & outputMapWrongTKey,
	                                                   std::map<TKey, std::vector<std::string> > & outputMapWrongTValue,
	                                                   std::map<std::string, std::vector<std::string> > & outputMapWrongTKeyWrongTValue)
	{
		std::map<TKey, std::vector<TValue> > outputMap;
		
		
		for (std::map<std::string, std::vector<std::string> >::const_iterator inputItem = inputMap.begin();
		     inputItem != inputMap.end(); ++inputItem)
		{
			// parse key
			TKey key;
			bool keyParsed = true;
			try {
				key = boost::lexical_cast<TKey>(inputItem->first);
			}
			catch (boost::bad_lexical_cast const& exception) {
				keyParsed = false;
			}
			
			for (std::vector<std::string>::const_iterator inputValueItem = inputItem->second.begin();
			     inputValueItem != inputItem->second.end(); ++inputValueItem)
			{
				// parse value
				TValue valueItem;
				bool valueParsed = true;
				try {
					valueItem = boost::lexical_cast<TValue>(*inputValueItem);
				}
				catch (boost::bad_lexical_cast const& exception) {
					valueParsed = false;
				}
				
				// fill output maps
				if (keyParsed)
				{
					if (valueParsed)
					{
						if (outputMap.count(key) == 0)
						{
							outputMap[key] = std::vector<TValue>();
						}
						outputMap[key].push_back(valueItem);
					}
					else
					{
						if (outputMapWrongTValue.count(key) == 0)
						{
							outputMapWrongTValue[key] = std::vector<std::string>();
						}
						outputMapWrongTValue[key].push_back(*inputValueItem);
					}
				}
				else
				{
					if (valueParsed)
					{
						if (outputMapWrongTKey.count(inputItem->first) == 0)
						{
							outputMapWrongTKey[inputItem->first] = std::vector<TValue>();
						}
						outputMapWrongTKey[inputItem->first].push_back(valueItem);
					}
					else
					{
						if (outputMapWrongTKeyWrongTValue.count(inputItem->first) == 0)
						{
							outputMapWrongTKeyWrongTValue[inputItem->first] = std::vector<std::string>();
						}
						outputMapWrongTKeyWrongTValue[inputItem->first].push_back(*inputValueItem);
					}
				}
			}
		}
		
		return outputMap;
	}
	
	template<class TKey, class TValue>
	std::map<TKey, std::vector<TValue> > ParseMapTypes(std::map<std::string, std::vector<std::string> > const& inputMap)
	{
		std::map<std::string, std::vector<TValue> > outputMapWrongTKey;
		std::map<TKey, std::vector<std::string> > outputMapWrongTValue;
		std::map<std::string, std::vector<std::string> > outputMapWrongTKeyWrongTValue;
		
		return ParseMapTypes(inputMap, outputMapWrongTKey, outputMapWrongTValue, outputMapWrongTKeyWrongTValue);
	}
	
	template<class TKey, class TValue>
	std::map<TKey, std::vector<TValue> > ParseMapTypes(std::map<std::string, std::vector<std::string> > const& inputMap,
	                                                   std::map<std::string, std::vector<TValue> > & outputMapWrongTKey)
	{
		std::map<TKey, std::vector<std::string> > outputMapWrongTValue;
		std::map<std::string, std::vector<std::string> > outputMapWrongTKeyWrongTValue;
		
		return ParseMapTypes(inputMap, outputMapWrongTKey, outputMapWrongTValue, outputMapWrongTKeyWrongTValue);
	}
	
	template<class TKey, class TValue>
	std::map<TKey, std::vector<TValue> > ParseMapTypes(std::map<std::string, std::vector<std::string> > const& inputMap,
	                                                   std::map<std::string, std::vector<TValue> > & outputMapWrongTKey,
	                                                   std::map<TKey, std::vector<std::string> > & outputMapWrongTValue)
	{
		std::map<std::string, std::vector<std::string> > outputMapWrongTKeyWrongTValue;
		
		return ParseMapTypes(inputMap, outputMapWrongTKey, outputMapWrongTValue, outputMapWrongTKeyWrongTValue);
	}
	
	// Check if container contains a certain item
	template <template<class, class...> class Container, typename Item, typename... Args>
	static bool Contains(Container<Item, Args...> const& container, Item const& item)
	{
		typename Container<Item, Args...>::const_iterator it = std::find(container.begin(), container.end(), item);
		return (it != container.end());
	}
	
	template<class TNumber>
	bool ApproxEqual(TNumber value1, TNumber value2, double maxDelta=1e-5)
	{
		if (value1 == value2)
		{
			return true;
		}
		else
		{
			double delta = std::abs(value1 - value2);
			if ((value1 + value2) != 0)
			{
				delta *= (2.0 / std::abs(value1 + value2));
			}
			return (delta < maxDelta);
		}
	}
	
	template<>
	bool ApproxEqual<RMFLV>(RMFLV value1, RMFLV value2, double maxDelta);
	
	template<>
	bool ApproxEqual<RMDataV>(RMDataV value1, RMDataV value2, double maxDelta);
	
	template<>
	bool ApproxEqual<RMSM2x2>(RMSM2x2 value1, RMSM2x2 value2, double maxDelta);
	
	template<class T>
	std::vector<T> Sorted(std::vector<T> const& vectorToSort)
	{
		std::vector<T> sortedVector = vectorToSort;
		std::sort(sortedVector.begin(), sortedVector.end());
		return sortedVector;
	}
	
	template<class T1, class T2>
	std::map<T1*, T2*> MatchDeltaR(std::vector<T1> const& lorentsVectors1, std::vector<T2> const& lorentsVectors2, float maxDeltaR)
	{
		std::map<T1*, T2*> matches;
		for (typename std::vector<T1>::const_iterator v1 = lorentsVectors1.begin(); v1 != lorentsVectors1.end(); ++v1)
		{
			for (typename std::vector<T2>::const_iterator v2 = lorentsVectors2.begin(); v2 != lorentsVectors2.end(); ++v2)
			{
				if (ROOT::Math::VectorUtil::DeltaR(v1->p4, v2->p4) < maxDeltaR)
				{
					matches[&(*v1)] = &(*v2);
					break;
				}
			}
		}
		return matches;
	}
}

