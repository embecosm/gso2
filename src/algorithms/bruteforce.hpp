#ifndef __BRUTEFORCE_H__
#define __BRUTEFORCE_H__

#include <assert.h>
#include <vector>
#include <iostream>

/*! Iterate over all possible combinations.

    This function iterates over every possible combination of the values in
    each entry of the values container.

    Example:
        vector<vector<int>> vals = {{0,1,2}, {3,4}};

        vector<vector<int>::iterator> current;
        current.push_back(vals[0].begin());
        current.push_back(vals[1].begin());

        do {
            ...
        } while(bruteforceIterate(values, current));

    The following values will be in current:
        0, 3
        0, 4
        1, 3
        1, 4
        2, 3
        2, 4

    @param containers   A list of containers, each containing a set of values.
    @param iterators    A list of iterators. Each iterator is derived from the
                        corresponding container.
    @return             True if there are further values to iterate.

*/
template <typename Container>
bool bruteforceIterate(std::vector<Container> &containers,
    std::vector<typename Container::iterator> &iterators)
{
    assert(containers.size() == iterators.size());

    for(int i = iterators.size()-1; i >= 0; --i)
    {
        iterators[i]++;

        if(iterators[i] == containers[i].end())
        {
            iterators[i] = containers[i].begin();
        }
        else
        {
            return true;
        }
    }

    return false;
}

/*! Iterate over all possible combinations.

    This function iterates over every possible combination of the values.
    There is no list of containers in this version, allowing just a single
    container to be passed, if all entries in the iterator list are derived
    from the same container.

    Example:
        vector<int> vals = {0,1,2};

        vector<vector<int>::iterator> current;

        current.push_back(vals.begin());
        current.push_back(vals.begin());

        do {
            ...
        } while(bruteforceIterate(values, current));

    The following values will be in current:
        0, 0
        0, 1
        0, 2
        1, 0
        1, 1
        ...

    @param values       A list of values.
    @param iterators    A list of iterators. Each iterator is derived from values.
    @return             True if there are further values to iterate.

*/
template <typename Container>
bool bruteforceIterate(Container &values,
    std::vector<typename Container::iterator> &iterators)
{
    for(int i = iterators.size()-1; i >= 0; --i)
    {
        iterators[i]++;

        if(iterators[i] == values.end())
        {
            iterators[i] = values.begin();
        }
        else
        {
            return true;
        }
    }

    return false;
}

/*! Iterate over all possible combinations.

    This function accepts a generic iterator class, as long as the class has a
    next function. The next function advances the iterator to the next value,
    reseting it to the first if the last value has been reached. If a reset
    occurs, false is returned. This allows bruteforce iterating over other
    iterators, such as canonicalIterators and constantIterators.

    @param iterators    A list of iterators
    @return             True if there are further values to iterate.
*/
template <typename Iterator>
bool bruteforceIterate(std::vector<Iterator> &iterators)
{
    for(int i = iterators.size()-1; i >= 0; --i)
    {
        if(iterators[i].next())
        {
            return true;
        }
    }

    return false;
}

/*! Iterate over all possible combinations.

    This function accepts a generic iterator class, as long as the class has a
    next function. The next function advances the iterator to the next value,
    reseting it to the first if the last value has been reached. If a reset
    occurs, false is returned. This allows bruteforce iterating over other
    iterators, such as canonicalIterators and constantIterators.

    @param iterators    A list of iterators
    @return             True if there are further values to iterate.
*/
template <typename Iterator>
bool bruteforceIterate(std::vector<Iterator*> &iterators)
{
    for(int i = iterators.size()-1; i >= 0; --i)
    {
        if(iterators[i]->next())
        {
            return true;
        }
    }

    return false;
}

#endif
