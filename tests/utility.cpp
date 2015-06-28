#define BOOST_TEST_MODULE utility test
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <vector>
#include <iostream>

#include "utility.hpp"

using namespace std;

BOOST_AUTO_TEST_CASE( combination_tests )
{
    {
        vector<unsigned> values = {0,1,2,3,4,5,6,7};

        Combinations<unsigned> comb(values, 4);

        unsigned count = 0;
        do {
            count++;
        } while(comb.next());

        // 8 choose 4
        BOOST_CHECK(count == 70);
    }

    {
        vector<unsigned> values = {0,1,2,3,4,5};

        Combinations<unsigned> comb(values, 2);

        unsigned count = 0;
        do {
            count++;
        } while(comb.next());

        // 6 choose 2
        BOOST_CHECK(count == 15);
    }

    {
        vector<unsigned> values = {0,1,2,3,4,5};

        Combinations<unsigned> comb(values, 2);

        BOOST_CHECK(comb.getSelection() == vector<unsigned>({0,1}));
        comb.next();
        BOOST_CHECK(comb.getSelection() == vector<unsigned>({0,2}));
        comb.next();
        BOOST_CHECK(comb.getSelection() == vector<unsigned>({1,2}));
        comb.next();
        BOOST_CHECK(comb.getSelection() == vector<unsigned>({0,3}));
        comb.next();
        BOOST_CHECK(comb.getSelection() == vector<unsigned>({1,3}));
        comb.next();
        BOOST_CHECK(comb.getSelection() == vector<unsigned>({2,3}));
        comb.next();
        BOOST_CHECK(comb.getSelection() == vector<unsigned>({0,4}));
    }
}
