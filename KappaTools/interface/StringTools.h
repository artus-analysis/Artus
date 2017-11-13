/* Copyright (c) 2010 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 */

#ifndef TBSTRINGTOOLS_H
#define TBSTRINGTOOLS_H

#include <map>
#include <vector>
#include <iostream>
#include <sstream>

// This construct is needed to stringify -D defines:
#define QUOTE(x) XQUOTE(x)
#define XQUOTE(x) #x

namespace KappaTools{

	void reportParseError(const std::string &s);

	template <typename T>
	T parse(const std::string &s, bool fail = true);
	template <>
	std::string parse<std::string>(const std::string &s, bool);
	template <>
	bool parse<bool>(const std::string &s, bool);

	template <typename T>
	std::string str(const T &i);
	template <>
	std::string str<bool>(const bool &i);

	std::string tolower(std::string s);
	std::string toupper(std::string s);

	std::string replace(const std::string &input, const std::string &find, const std::string &replace);
	std::string replaceall(const std::string &input, const std::string &find, const std::string &replace);

	std::vector<std::string> split(const std::string &str, const std::string &delim, const size_t maxSize = 0);
	std::vector<std::string> tokenize(const std::string &str, const std::string &delim = " ", const bool escape = false);

	std::string lstrip(const std::string &input, const std::string rm = std::string(" \n\r\0", 4));
	std::string rstrip(const std::string &input, const std::string rm = std::string(" \n\r\0", 4));
	std::string strip(const std::string &input, const std::string rm = std::string(" \n\r\0", 4));

	template<typename T1, typename T2>
	bool in(const T1 x, const T2 y);
	template<>
	bool in(const char x, const std::string y);
	template<>
	bool in(const char x, const std::string &y);
	template<>
	bool in(const std::string x, const std::vector<std::string> &y);
	template<>
	bool in(const std::string &x, const std::vector<std::string> &y);

	bool startswith(const std::string &input, const std::string search);
	bool endswith(const std::string &input, const std::string search);

	std::string basename(const std::string &input);
	std::string dirname(const std::string &input);
	std::string substVars(std::string in, const std::vector<std::pair<std::string, std::string> > &vars);

	template<typename Tin>
	std::string join(const std::string delim, const Tin &cont);
	template<typename Tin, typename Tas>
	std::string joinAs(const std::string delim, const Tin &cont);

	class stlprintf
	{
	public:
		stlprintf(const char *format, ...);
		operator const char*() const;
		operator const std::string() const;
	private:
		std::string str;
	};
}

#include "StringTools.hxx"

#endif
