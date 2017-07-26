
#pragma once

#include "Artus/Utility/interface/ArtusLogging.h"

#include <iostream>
#include <boost/format.hpp>

#include <TMatrixTBase.h>


template<class T>
std::ostream& operator<<(std::ostream& os, TMatrixTBase<T> const& matrix)
{
	for (int row = 0; row < matrix.GetNrows(); ++row)
	{
		for (int column = 0; column < matrix.GetNcols(); ++column)
		{
			if (column > 0)
			{
				os << " ";
			}
			os << ((column > 0) ? " " : "") << boost::format{"%+5e"} % matrix(row, column);
		}
		os << std::endl;
	}
	return os;
}

