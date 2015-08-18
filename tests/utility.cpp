#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <vector>
#include <iostream>

#include "utility.hpp"

using namespace std;

BOOST_AUTO_TEST_SUITE(utility_test)

BOOST_AUTO_TEST_CASE( combination_tests )
{
    {
        vector<unsigned> values = {0,1,2,3,4,5,6,7};

        Combinations comb(values.size(), 4);

        unsigned count = 0;
        do {
            count++;
        } while(comb.next());

        // 8 choose 4
        BOOST_CHECK(count == 70);
    }

    {
        vector<unsigned> values = {0,1,2,3,4,5};

        Combinations comb(values.size(), 2);

        unsigned count = 0;
        do {
            count++;
        } while(comb.next());

        // 6 choose 2
        BOOST_CHECK(count == 15);
    }

    {
        vector<unsigned> values = {0,1,2,3,4,5};
        vector<unsigned> sel;

        Combinations comb(values.size(), 2);

        comb.getSelection(sel);
        BOOST_CHECK(sel == vector<unsigned>({0,1}));
        comb.next();
        comb.getSelection(sel);
        BOOST_CHECK(sel == vector<unsigned>({0,2}));
        comb.next();
        comb.getSelection(sel);
        BOOST_CHECK(sel == vector<unsigned>({1,2}));
        comb.next();
        comb.getSelection(sel);
        BOOST_CHECK(sel == vector<unsigned>({0,3}));
        comb.next();
        comb.getSelection(sel);
        BOOST_CHECK(sel == vector<unsigned>({1,3}));
        comb.next();
        comb.getSelection(sel);
        BOOST_CHECK(sel == vector<unsigned>({2,3}));
        comb.next();
        comb.getSelection(sel);
        BOOST_CHECK(sel == vector<unsigned>({0,4}));
    }
}

BOOST_AUTO_TEST_SUITE_END()
