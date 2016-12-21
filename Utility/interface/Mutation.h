
#pragma once

#include <utility>
#include <list>
#include <map>
#include <algorithm>

struct Mutation {

	typedef std::pair<float, float> FloatTupleType;
	typedef std::vector<FloatTupleType> FloatTuples;

	Mutation(std::vector<FloatTupleType> const& tuple,
			std::string const & prefix);

	Mutation();

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

	void getFolderName(MutationMap const& mm);

	void recursiveMutation(MutationMap const& mm, MutContainerIter itFirst,
	                       MutContainerIter itRest);
};

