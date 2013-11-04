#pragma once

#include <boost/test/included/unit_test.hpp>

#include "Artus/Core/interface/FilterBase.h"

BOOST_AUTO_TEST_CASE( test_filter_result )
{
	FilterResult fres;

	fres.SetFilterDecisions("filter1",true);
	fres.SetFilterDecisions("filter_too", false);

	BOOST_CHECK( fres.GetFilterDecision("filter1") == true );
	BOOST_CHECK( fres.GetFilterDecision("filter_too") == false );

	BOOST_CHECK( fres.HasPassed() == false );
	BOOST_CHECK( fres.HasPassedIfExcludingFilter("filter_too") == true );
}

