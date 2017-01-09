
#pragma once

#include <vector>

#include <boost/test/included/unit_test.hpp>
#include <boost/ptr_container/ptr_map.hpp>

#include "Artus/Utility/interface/SafeMap.h"

class SomeClass {};

BOOST_AUTO_TEST_CASE(test_safemap)
{
    {
        // test with std::map
        std::map<int, std::string> myMap;

        myMap[23] = "23";
        myMap[42] = "42";

        std::string rr1 = SafeMap::Get(myMap, 42);
        BOOST_CHECK(rr1 == "42");

        std::string rr2DefValue("42");
        std::string rr2 = SafeMap::GetWithDefault(myMap, 43, rr2DefValue);
        BOOST_CHECK(rr2 == std::string("42"));
        BOOST_CHECK(&rr2 != &rr2DefValue);

    }
    {
        // test with boost::ptr_map
        boost::ptr_map<int, std::string> myMapPtr;

        myMapPtr[23] = "23";
        myMapPtr[42] = "42";

        std::string rrPtr = SafeMap::GetPtrMap(myMapPtr, 23);
        BOOST_CHECK(rrPtr == "23");
    }
}

