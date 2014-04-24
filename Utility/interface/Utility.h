
#pragma once

#include <string>
#include <type_traits>

#include "Artus/Core/interface/GlobalInclude.h"
#include "Artus/Core/interface/Cpp11Support.h"


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
}

