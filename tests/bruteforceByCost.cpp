#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <set>

#include "algorithms/bruteforceByCost.hpp"

using namespace std;

BOOST_AUTO_TEST_SUITE(canonical_test);

template<typename T, typename C=int>
void testBFBC(vector<vector<T>> values, vector<typename vector<T>::iterator> iters,
    function< C(vector<typename vector<T>::iterator>&)> costFn)
{
    bruteforceByCost<vector<T>,C> bfbc(values, iters, costFn);

    uint64_t totalsize=1;
    uint64_t count=0;

    set<vector<typename vector<T>::iterator>> results;

    for(auto &c: values)
        totalsize *= c.size();

     do {
        count++;
        results.insert(iters);
    } while(bfbc.next());

    BOOST_REQUIRE(count == totalsize);
    BOOST_REQUIRE(results.size() == totalsize);
}

BOOST_AUTO_TEST_CASE( general_test_1 )
{
    vector<vector<int>> vals = {{0,1,2,3},{0,1,2,3}};
    vector<vector<int>::iterator> iters;

    testBFBC<int,int>(vals, iters,
        [] (vector<vector<int>::iterator> &its) -> int
        {
            int i = 1;

            for(auto &k: its)
                i *= *k;
            return i;
        });
}


BOOST_AUTO_TEST_CASE( general_test_2)
{
    vector<vector<int>> vals = {{0,1,2,3},{0,1,2,3},{0,1,2,3},{0,1,2,3}};
    vector<vector<int>::iterator> iters;

    testBFBC<int,int>(vals, iters,
        [] (vector<vector<int>::iterator> &its) -> int
        {
            int i = 1;

            for(auto &k: its)
                i *= *k;
            return i;
        });
}

BOOST_AUTO_TEST_CASE( general_test_3)
{
    vector<vector<int>> vals = {{0,1,2,3,4,5},{0,1,2,3,4,5}};
    vector<vector<int>::iterator> iters;


    testBFBC<int,int>(vals, iters,
        [] (vector<vector<int>::iterator> &its) -> int
        {
            int i = (*its[0]-3) * (*its[0]-3) + (*its[1]-3)*(*its[1]-3);
            return i;
        });
}

BOOST_AUTO_TEST_SUITE_END()
