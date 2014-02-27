
#pragma once

#include <type_traits>

namespace EnumHelper {

	template<typename E>
	using enableEnumType = typename std::enable_if< std::is_enum<E>::value, typename std::underlying_type<E>::type>::type;

	template<typename E>
	constexpr inline enableEnumType<E> toUnderlyingValue(E e) noexcept
	{
		return static_cast<typename std::underlying_type<E>::type>(e);
	}   

	template< typename E , typename T>
	constexpr inline typename std::enable_if<std::is_enum<E>::value && std::is_integral<T>::value, E>::type toEnum(T value) noexcept 
	{
		return static_cast<E>( value );
	}

}

