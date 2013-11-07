/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <utility>
#include <list>
#include <map>
#include <algorithm>

struct Mutation {

	typedef std::pair<float, float> FloatTupleType;
	typedef std::vector<FloatTupleType> FloatTuples;

	Mutation(std::vector<FloatTupleType> const& tuple,
			std::string const & prefix) :
			MutationType(TypeEnum::FloatTuple), FloatTuple(tuple), NamePrefix(
					prefix) {

	}

	Mutation() {
	}

	size_t Index;
	enum class TypeEnum {
		FloatTuple, String
	};

	TypeEnum MutationType;

	FloatTuples FloatTuple;
	std::vector<std::string> String;
	std::string NamePrefix;
};

//typedef std::vector < Mutation > MutationList;
typedef std::map<std::string, Mutation> MutationMap;

class MutationCombiner {
public:
	typedef std::vector<std::string> MutContainer;
	typedef MutContainer::iterator MutContainerIter;

	void getFolderName(MutationMap const& mm) {
		std::vector<std::string> mutNames;

		for (auto const& m : mm) {
			mutNames.push_back(m.first);
		}
		// sort, so the mutation will always be the same
		std::sort(mutNames.begin(), mutNames.end());

		// combine first with the rest ...

		recursiveMutation(mm, mutNames.begin(), mutNames.begin() + 1);
	}

	void recursiveMutation(MutationMap const& mm, MutContainerIter itFirst,
			MutContainerIter itRest) {
		// still working on this ...
		//auto thisMut = mm[*itFirst];

		//for ( auto thisElem : thisMut.FloatTuple)
	}
};

