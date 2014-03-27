
#include "Artus/Core/interface/Mutation.h"

Mutation::Mutation(std::vector<FloatTupleType> const& tuple,
		std::string const & prefix) :
		MutationType(TypeEnum::FloatTuple), FloatTuple(tuple), NamePrefix(
				prefix) {

}

void MutationCombiner::getFolderName(MutationMap const& mm) {
	std::vector<std::string> mutNames;

	for (auto const& m : mm) {
		mutNames.push_back(m.first);
	}
	// sort, so the mutation will always be the same
	std::sort(mutNames.begin(), mutNames.end());

	// combine first with the rest ...

	recursiveMutation(mm, mutNames.begin(), mutNames.begin() + 1);
}

void MutationCombiner::recursiveMutation(MutationMap const& mm, MutContainerIter itFirst,
                                         MutContainerIter itRest) {
	// still working on this ...
	//auto thisMut = mm[*itFirst];

	//for ( auto thisElem : thisMut.FloatTuple)
}

