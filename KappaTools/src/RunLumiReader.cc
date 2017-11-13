/* Copyright (c) 2010 - All Rights Reserved
 *   Andreas Oehler <andreasoehler@gmx.net>
 *   Fred Stober <stober@cern.ch>
 *   Manuel Zeise <zeise@cern.ch>
 */

#include "Artus/KappaTools/interface/RunLumiReader.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "Artus/KappaTools/interface/IOHelper.h"
#include "Artus/KappaTools/interface/StringTools.h"

void readLumiFilter(const std::string json, std::map<run_id, std::set<std::pair<lumi_id, lumi_id> > > &lumifilter)
{
	using boost::property_tree::ptree;
	ptree pt;
	read_json(json, pt);
	for (ptree::const_iterator itRun = pt.begin(); itRun != pt.end(); ++itRun)
	{
		run_id run = KappaTools::parse<run_id>(itRun->first);
		for (ptree::const_iterator itLumiList = itRun->second.begin(); itLumiList != itRun->second.end(); ++itLumiList)
		{
			lumi_id lumi_low = 0, lumi_high = 0;
			for (ptree::const_iterator itLumiRange = itLumiList->second.begin(); itLumiRange != itLumiList->second.end(); ++itLumiRange)
			{
				if (lumi_low == 0)
					lumi_low = KappaTools::parse<lumi_id>(itLumiRange->second.data());
				else if (lumi_high == 0)
					lumi_high = KappaTools::parse<lumi_id>(itLumiRange->second.data());
			}
			lumifilter[run].insert(std::make_pair(lumi_low, lumi_high));
		}
	}
}

RunLumiSelector::RunLumiSelector(const std::string json, const run_id _passRunLow, const run_id _passRunHigh)
	: passRunLow(_passRunLow), passRunHigh(_passRunHigh)
{
	if (json != "")
		readLumiFilter(json, lumifilter);
}

RunLumiSelector::RunLumiSelector(const std::vector<std::string> &json, const run_id _passRunLow, const run_id _passRunHigh)
	: passRunLow(_passRunLow), passRunHigh(_passRunHigh)
{
	for (size_t i = 0; i < json.size(); ++i)
		readLumiFilter(json[i], lumifilter);
}

void RunLumiSelector::addJSONFile(const std::string json)
{
	readLumiFilter(json, lumifilter);
}

bool RunLumiSelector::isCompatible(const FileInterface &fi) const
{
	if (lumifilter.empty())
		return true;
	std::vector<std::pair<run_id, lumi_id> > lumiList = fi.GetRunLumis();
	for (std::vector<std::pair<run_id, lumi_id> >::const_iterator it = lumiList.begin(); it != lumiList.end(); ++it)
		if (accept(it->first, it->second))
			return true;
	return false;
}

std::pair<run_id, run_id> RunLumiSelector::getBoundaries() const
{
	run_id min = std::numeric_limits<run_id>::max(), max = std::numeric_limits<run_id>::min();
	if (lumifilter.size() == 0)
		return std::make_pair(max, min);
	for (std::map<run_id, std::set<std::pair<lumi_id, lumi_id> > >::const_iterator it = lumifilter.begin(); it != lumifilter.end(); ++it)
	{
		if (it->first < min)
			min = it->first;
		if (it->first > max)
			max = it->first;
	}
	return std::make_pair(min, max);
}

std::pair<run_id, lumi_id> RunLumiSelector::getMaxRunLumiPair() const
{
	typedef std::set<std::pair<lumi_id, lumi_id> > lumirange;
	std::map<run_id, lumirange>::const_reverse_iterator iter = lumifilter.rbegin();
	return std::make_pair(iter->first, iter->second.rbegin()->second);
}

std::map<run_id, std::set<std::pair<lumi_id, lumi_id> > > RunLumiSelector::getMinimalJSON(const std::map<run_id, std::set<std::pair<lumi_id, lumi_id> > > &lumiSrc)
{
	std::map<run_id, std::set<std::pair<lumi_id, lumi_id> > > lumiRet;
	for (std::map<run_id, std::set<std::pair<lumi_id, lumi_id> > >::const_iterator it1 = lumiSrc.begin(); it1 != lumiSrc.end(); )
	{
		lumi_id firstLumi = 0;
		lumi_id lastLumi = 0;
		for (std::set<std::pair<lumi_id, lumi_id> >::const_iterator it2 = it1->second.begin(); it2 != it1->second.end(); )
		{
			if (firstLumi == 0)
			{
				firstLumi = it2->first;
				lastLumi = it2->second;
			}
			else
			{
				if (lastLumi + 1 == it2->first)
					lastLumi = it2->second;
				else
				{
					lumiRet[it1->first].insert(std::make_pair(firstLumi, lastLumi));
					firstLumi = it2->first;
					lastLumi = it2->second;
				}
			}
			it2++;
			if (it2 == it1->second.end())
				lumiRet[it1->first].insert(std::make_pair(firstLumi, lastLumi));
		}
		it1++;
	}
	return lumiRet;
}

void RunLumiSelector::printJSON(std::ostream &os) const
{
	RunLumiSelector::printJSON(os, lumifilter);
}

void RunLumiSelector::printJSON(std::ostream &os,
	const std::map<run_id, std::set<std::pair<lumi_id, lumi_id> > > &lumiSrc)
{
	os << std::endl << "{";
	for (std::map<run_id, std::set<std::pair<lumi_id, lumi_id> > >::const_iterator it1 = lumiSrc.begin(); it1 != lumiSrc.end(); )
	{
		os << "\"" << it1->first << "\": [";
		for (std::set<std::pair<lumi_id, lumi_id> >::const_iterator it2 = it1->second.begin(); it2 != it1->second.end(); )
		{
			os << "[" << it2->first << "," << it2->second << "]";
			it2++;
			if (it2 != it1->second.end())
				os << ", ";
		}
		os << "]";
		it1++;
		if (it1 != lumiSrc.end())
			os << ", ";
	}
	os << "}" << std::endl << std::endl;
}

const std::map<run_id, std::set<std::pair<lumi_id, lumi_id> > > & RunLumiSelector::getRunLumiMap() const
{
	return lumifilter;
}

std::ostream &operator<<(std::ostream &os, const std::pair<lumi_id, lumi_id> &p)
{
	return os << p.first << "-" << p.second;
}

std::ostream &operator<<(std::ostream &os, const RunLumiSelector &m)
{
	os << "Accepted runs / lumis:" << std::endl << m.lumifilter << std::endl;
	if ((m.passRunLow > 0) || (m.passRunHigh > 0))
	{
		os << "All runs with ";
		if (m.passRunLow > 0)
			os << "run <= " << m.passRunLow << "";
		if ((m.passRunLow > 0) && (m.passRunHigh > 0))
			os << " and ";
		if (m.passRunHigh > 0)
			os << "run >= " << m.passRunHigh << "";
		os << " are automatically accepted" << std::endl;
	}
	return os;
}
