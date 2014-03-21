
#pragma once

#include <string>
#include <type_traits>


namespace Utility {

	// get value from generic associative containers with default value
	template <template<class, class, class...> class Container, typename Key, typename Value, typename... Args>
	Value GetWithDefault(const Container<Key, Value, Args...>& m, Key const& key, const Value & defaultValue)
	{
		typename Container<Key, Value, Args...>::const_iterator it = m.find(key);
		if (it == m.end())
		    return defaultValue;
		return it->second;
	}

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

