/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <vector>

#include <boost/test/included/unit_test.hpp>
#include <boost/ptr_container/ptr_map.hpp>

#include "Artus/Utility/interface/SafeMap.h"

class SomeClass {};

BOOST_AUTO_TEST_CASE( test_safemap )
{
    {
        // test with std::map
        std::map < int, std::string > myMapa;

        myMapa [23] = "23";
        myMapa [42] = "42";

        std::string const & rr1 = SafeMap::Get ( myMapa, 42 );
        BOOST_CHECK( rr1 == "42" );
        std::string rr2 = SafeMap::GetWithDefault ( myMapa, 43, std::string("42") ); // removed "const &" argument, since in this CMSSW_7_4_X the conversion fails 
        BOOST_CHECK_MESSAGE( rr2 == std::string("42"), "the map has value:" << rr2 );
    }
    {
        // test with boost::ptr_map
        boost::ptr_map < int, std::string > myMapPtr;

        myMapPtr [ 23 ] = "23";
        myMapPtr [ 42 ] = "42";

        std::string const& rrPtr = SafeMap::GetPtrMap ( myMapPtr, 23 );
        BOOST_CHECK( rrPtr == "23" );
    }
}

