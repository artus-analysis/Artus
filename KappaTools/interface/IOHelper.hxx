/* Copyright (c) 2010 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 *   Manuel Zeise <zeise@cern.ch>
 */

template<typename T>
void printMatrix(T **matrix, const size_t x, const size_t y, const size_t width)
{
	OStreamGuard guard(std::cout);
	for (size_t i = 0; i < x; ++i)
	{
		for (size_t j = 0; j < y; ++j)
			std::cout << std::setw(width) << matrix[i][j] << " ";
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

namespace __gnu_cxx
{
	template<typename T1, typename T2>
	std::ostream &operator<<(std::ostream &os, const std::map<T1, T2> &m)
	{
		struct MapStrClass
		{
			static std::string pstr(const std::pair<const T1, T2> &x) { return KappaTools::str<T1>(x.first) + ": " + KappaTools::str<T2>(x.second); }
		};
		return os << "{" << KappaTools::join(", ", strmap(MapStrClass::pstr, m)) << "}";
	}

	template<typename T>
	std::ostream &operator<<(std::ostream &os, const std::set<T> &s)
	{
		return os << "{" << KappaTools::join(", ", s) << "}";
	}

	template<typename T>
	std::ostream &operator<<(std::ostream &os, const std::vector<T> &v)
	{
		return os << "[" << KappaTools::join(", ", v) << "]";
	}

	template<typename T1, typename T2>
	std::ostream &operator<<(std::ostream &os, const std::pair<T1, T2> &p)
	{
		return os << "(" << p.first << ", " << p.second << ")";
	}
}
