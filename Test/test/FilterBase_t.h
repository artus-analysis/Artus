/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <boost/test/included/unit_test.hpp>

#include "Artus/Core/interface/FilterBase.h"
#include "Artus/Core/interface/FilterResult.h"
#include "Artus/Core/interface/CutFlow.h"

BOOST_AUTO_TEST_CASE( test_cut_flow )
{
	FilterResult fres1;
	FilterResult fres2;
	FilterResult fres3;

	fres1.SetFilterDecision("filter1", false);
	fres1.SetFilterDecision("filter2", true);

	fres2.SetFilterDecision("filter1",true);
	fres2.SetFilterDecision("filter2", false);

	fres3.SetFilterDecision("filter1",true);
	fres3.SetFilterDecision("filter2", true);

	CutFlow cflow;

	cflow.AddFilterResult ( fres1 );
	cflow.AddFilterResult ( fres2 );
	cflow.AddFilterResult ( fres3 );

	BOOST_CHECK( cflow.GetEventCount() == 3 );
	BOOST_CHECK_EQUAL( cflow.GetCutEntry("filter1")->second, 2);
	BOOST_CHECK_EQUAL( cflow.GetCutEntry("filter2")->second, 2);

}

BOOST_AUTO_TEST_CASE( test_filter_result )
{
	FilterResult fres;

	fres.SetFilterDecision("filter1",true);
	fres.SetFilterDecision("filter_too", false);

	BOOST_CHECK( fres.GetFilterDecision("filter1") == FilterResult::Decision::Passed );
	BOOST_CHECK( fres.GetFilterDecision("filter_too") == FilterResult::Decision::NotPassed );

	BOOST_CHECK( fres.HasPassed() == false );
	BOOST_CHECK( fres.HasPassedIfExcludingFilter("filter_too") == true );
}

BOOST_AUTO_TEST_CASE( test_filter_result_set_after_cached_true )
{
	FilterResult fres;

	fres.SetFilterDecision("filter1",true);
	fres.SetFilterDecision("filter2", false);
	fres.SetFilterDecision("filter3", true);

	BOOST_CHECK( fres.HasPassed() == false );
	BOOST_CHECK( fres.HasPassedIfExcludingFilter("filter2") == true );

	fres.SetFilterDecision("filter2", true);

	BOOST_CHECK( fres.HasPassed() == true );
	BOOST_CHECK( fres.HasPassedIfExcludingFilter("filter2") == true );
}

BOOST_AUTO_TEST_CASE( test_filter_result_set_after_cached_false )
{
	FilterResult fres;

	fres.SetFilterDecision("filter1",true);
	fres.SetFilterDecision("filter2", true);
	fres.SetFilterDecision("filter3", true);

	BOOST_CHECK( fres.HasPassed() == true );
	BOOST_CHECK( fres.HasPassedIfExcludingFilter("filter2") == true );

	fres.SetFilterDecision("filter3", false);

	BOOST_CHECK( fres.HasPassed() == false );
	BOOST_CHECK( fres.HasPassedIfExcludingFilter("filter3") == true );
}


BOOST_AUTO_TEST_CASE( test_filter_result_intial_filter_names )
{
	FilterResult::FilterNames fnames;
	fnames.push_back( "filter1" );
	fnames.push_back( "filter2" );
	fnames.push_back( "filter3" );

	FilterResult fres (fnames);
	BOOST_CHECK( fres.GetFilterDecision("filter1") == FilterResult::Decision::Undefined );
	BOOST_CHECK( fres.GetFilterDecision("filter2") == FilterResult::Decision::Undefined );
	BOOST_CHECK( fres.GetFilterDecision("filter3") == FilterResult::Decision::Undefined );

	// no filter decision set, so the filters pass
	BOOST_CHECK( fres.HasPassed() == true );
	BOOST_CHECK( fres.GetFilterDecisions().size() == 3 );


	fres.SetFilterDecision("filter1",true);
	fres.SetFilterDecision("filter2", false);

	// one filter failed, so the filters don't pass
	BOOST_CHECK( fres.HasPassed() == false );
	BOOST_CHECK( fres.GetFilterDecisions().size() == 3 );
}


BOOST_AUTO_TEST_CASE( test_filter_result_copy_constr )
{
	FilterResult fres_global;

	fres_global.SetFilterDecision("global_filter1",true);
	fres_global.SetFilterDecision("global_filter2", false);

	FilterResult fres_local( fres_global );

	fres_local.SetFilterDecision("local1", true);

	BOOST_CHECK( fres_local.HasPassed() == false );
	BOOST_CHECK( fres_local.HasPassedIfExcludingFilter("global_filter2") == true );
	BOOST_CHECK( fres_local.GetFilterDecision("global_filter1") == FilterResult::Decision::Passed );
	BOOST_CHECK( fres_local.GetFilterDecision("global_filter2") == FilterResult::Decision::NotPassed );
	BOOST_CHECK( fres_local.GetFilterDecision("local1") == FilterResult::Decision::Passed );
}

