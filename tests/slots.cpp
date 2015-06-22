#define BOOST_TEST_MODULE slots test
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <vector>
#include <iostream>

#include "slots.hpp"
#include "algorithms/bruteforce.hpp"

using namespace std;

BOOST_AUTO_TEST_CASE( bruteforce_slot_tests )
{
    {
        vector<ConstantSlot> values;

        values.push_back(ConstantSlot({{0,10}}));
        values.push_back(ConstantSlot({{0,10}}));

        values[0].reset();
        values[1].reset();

        unsigned count = 0;

        do {
            count++;
        } while(bruteforceIterate(values));

        BOOST_REQUIRE(count == 11*11);
    }

    {
        vector<ConstantSlot> values;

        values.push_back(ConstantSlot({{0,1}, {100,102}}));
        values.push_back(ConstantSlot({{0,2}, {5,6}}));

        values[0].reset();
        values[1].reset();

        BOOST_REQUIRE(values[0].getValue() == 0);
        BOOST_REQUIRE(values[1].getValue() == 0);

        bruteforceIterate(values);
        BOOST_REQUIRE(values[0].getValue() == 0);
        BOOST_REQUIRE(values[1].getValue() == 1);

        bruteforceIterate(values);
        BOOST_REQUIRE(values[0].getValue() == 0);
        BOOST_REQUIRE(values[1].getValue() == 2);

        bruteforceIterate(values);
        BOOST_REQUIRE(values[0].getValue() == 0);
        BOOST_REQUIRE(values[1].getValue() == 5);

        bruteforceIterate(values);
        BOOST_REQUIRE(values[0].getValue() == 0);
        BOOST_REQUIRE(values[1].getValue() == 6);

        bruteforceIterate(values);
        BOOST_REQUIRE(values[0].getValue() == 1);
        BOOST_REQUIRE(values[1].getValue() == 0);

        bruteforceIterate(values);
        BOOST_REQUIRE(values[0].getValue() == 1);
        BOOST_REQUIRE(values[1].getValue() == 1);

        bruteforceIterate(values);
        BOOST_REQUIRE(values[0].getValue() == 1);
        BOOST_REQUIRE(values[1].getValue() == 2);

        bruteforceIterate(values);
        BOOST_REQUIRE(values[0].getValue() == 1);
        BOOST_REQUIRE(values[1].getValue() == 5);

        bruteforceIterate(values);
        BOOST_REQUIRE(values[0].getValue() == 1);
        BOOST_REQUIRE(values[1].getValue() == 6);

        bruteforceIterate(values);
        BOOST_REQUIRE(values[0].getValue() == 100);
        BOOST_REQUIRE(values[1].getValue() == 0);
    }
}
