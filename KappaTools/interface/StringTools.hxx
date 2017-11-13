/* Copyright (c) 2010 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 */

template <typename T>
T KappaTools::parse(const std::string &s, bool fail)
{
	T x; char temp; std::istringstream str(s);
	if (!(str >> x) || (fail && str.get(temp)))
		KappaTools::reportParseError(s);
	return x;
}

template <typename T>
std::string KappaTools::str(const T &i)
{
	std::ostringstream str;
	str << i;
	return str.str();
}

template<typename Tin>
std::string KappaTools::join(const std::string delim, const Tin &cont)
{
	if (cont.empty())
		return "";
	std::string result;
	typename Tin::const_iterator last = cont.end();
	for (typename Tin::const_iterator it = cont.begin(); it != cont.end();)
	{
		result += str(*it);
		if (++it != last)
			result += delim;
	}
	return result;
}

template<typename Tin, typename Tas>
std::string KappaTools::joinAs(const std::string delim, const Tin &cont)
{
	if (cont.empty())
		return "";
	std::string result;
	typename Tin::const_iterator last = cont.end();
	for (typename Tin::const_iterator it = cont.begin(); it != cont.end();)
	{
		result += str(static_cast<Tas>(*it));
		if (++it != last)
			result += delim;
	}
	return result;
}
