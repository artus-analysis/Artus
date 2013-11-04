#pragma once

#include <boost/test/included/unit_test.hpp>

#include "Artus/Core/interface/Mutation.h"

BOOST_AUTO_TEST_CASE( test_mutation_generate_name )
{
	Mutation::FloatTuples etaTuples;
	Mutation::FloatTuples phiTuples;

	etaTuples. push_back ( std::make_pair ( -1.0f, 0.0f) );
	etaTuples. push_back ( std::make_pair ( 0.0f, 1.0f) );

	Mutation mt_eta(etaTuples, "eta");
	Mutation mt_phi(phiTuples, "phi");

	MutationMap mutations;

	mutations[ "global-eta" ] = mt_eta;
	mutations["global-phi"] = mt_phi;

}

