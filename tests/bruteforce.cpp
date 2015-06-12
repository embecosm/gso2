#define BOOST_TEST_MODULE bruteforce test
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <vector>
#include <iostream>

#include "algorithms/bruteforce.hpp"

using namespace std;

BOOST_AUTO_TEST_CASE( standard_tests )
{
    {
        vector<unsigned> values = {0,1};
        vector<vector<unsigned>::iterator> indices;

        indices.push_back(values.begin());
        indices.push_back(values.begin());

        BOOST_REQUIRE(*indices[0] == 0 && *indices[1] == 0);
        bruteforceIterate(values, indices);
        BOOST_REQUIRE(*indices[0] == 0 && *indices[1] == 1);
        bruteforceIterate(values, indices);
        BOOST_REQUIRE(*indices[0] == 1 && *indices[1] == 0);
        bruteforceIterate(values, indices);
        BOOST_REQUIRE(*indices[0] == 1 && *indices[1] == 1);
    }

    {
        vector<unsigned> values = {5,2,6};
        vector<vector<unsigned>::iterator> indices;

        indices.push_back(values.begin());
        indices.push_back(values.begin());
        indices.push_back(values.begin());

        BOOST_REQUIRE(*indices[0] == 5 && *indices[1] == 5 && *indices[2] == 5);
        bruteforceIterate(values, indices);
        BOOST_REQUIRE(*indices[0] == 5 && *indices[1] == 5 && *indices[2] == 2);
        bruteforceIterate(values, indices);
        BOOST_REQUIRE(*indices[0] == 5 && *indices[1] == 5 && *indices[2] == 6);
        bruteforceIterate(values, indices);

        BOOST_REQUIRE(*indices[0] == 5 && *indices[1] == 2 && *indices[2] == 5);
        bruteforceIterate(values, indices);
        BOOST_REQUIRE(*indices[0] == 5 && *indices[1] == 2 && *indices[2] == 2);
        bruteforceIterate(values, indices);
        BOOST_REQUIRE(*indices[0] == 5 && *indices[1] == 2 && *indices[2] == 6);
        bruteforceIterate(values, indices);

        BOOST_REQUIRE(*indices[0] == 5 && *indices[1] == 6 && *indices[2] == 5);
        bruteforceIterate(values, indices);
        BOOST_REQUIRE(*indices[0] == 5 && *indices[1] == 6 && *indices[2] == 2);
        bruteforceIterate(values, indices);
        BOOST_REQUIRE(*indices[0] == 5 && *indices[1] == 6 && *indices[2] == 6);
        bruteforceIterate(values, indices);

        BOOST_REQUIRE(*indices[0] == 2 && *indices[1] == 5 && *indices[2] == 5);
        bruteforceIterate(values, indices);
        BOOST_REQUIRE(*indices[0] == 2 && *indices[1] == 5 && *indices[2] == 2);
        bruteforceIterate(values, indices);
        BOOST_REQUIRE(*indices[0] == 2 && *indices[1] == 5 && *indices[2] == 6);
        bruteforceIterate(values, indices);
    }

    {
        vector<unsigned> values = {0,1,2,3};
        vector<vector<unsigned>::iterator> indices;

        indices.push_back(values.begin());
        indices.push_back(values.begin());
        indices.push_back(values.begin());
        indices.push_back(values.begin());

        unsigned count = 0;

        do {
            count++;
        } while(bruteforceIterate(values, indices));

        BOOST_REQUIRE(count == 4*4*4*4);
    }

    {
        vector<unsigned> values = {3,2,6,4,7,1};
        vector<vector<unsigned>::iterator> indices;

        indices.push_back(values.begin());
        indices.push_back(values.begin());
        indices.push_back(values.begin());
        indices.push_back(values.begin());
        indices.push_back(values.begin());

        unsigned count = 0;

        do {
            count++;
        } while(bruteforceIterate(values, indices));

        BOOST_REQUIRE(count == 6*6*6*6*6);
    }
}


BOOST_AUTO_TEST_CASE( custom_tests )
{
    {
        vector<vector<unsigned>> values = {{0,1}, {0,1,2}};
        vector<vector<unsigned>::iterator> indices;

        indices.push_back(values[0].begin());
        indices.push_back(values[1].begin());

        BOOST_REQUIRE(*indices[0] == 0 && *indices[1] == 0);
        bruteforceIterate(values, indices);
        BOOST_REQUIRE(*indices[0] == 0 && *indices[1] == 1);
        bruteforceIterate(values, indices);
        BOOST_REQUIRE(*indices[0] == 0 && *indices[1] == 2);
        bruteforceIterate(values, indices);

        BOOST_REQUIRE(*indices[0] == 1 && *indices[1] == 0);
        bruteforceIterate(values, indices);
        BOOST_REQUIRE(*indices[0] == 1 && *indices[1] == 1);
        bruteforceIterate(values, indices);
        BOOST_REQUIRE(*indices[0] == 1 && *indices[1] == 2);
        bruteforceIterate(values, indices);
        BOOST_REQUIRE(*indices[0] == 0 && *indices[1] == 0);
    }

    {
        vector<vector<unsigned>> values = {{0,0}, {0,0,0}, {0,0,0,0}, {0}};
        vector<vector<unsigned>::iterator> indices;

        indices.push_back(values[0].begin());
        indices.push_back(values[1].begin());
        indices.push_back(values[2].begin());
        indices.push_back(values[3].begin());

        unsigned count = 0;

        do {
            count++;
        } while(bruteforceIterate(values, indices));

        BOOST_REQUIRE(count == 2*3*4*1);
    }
}
