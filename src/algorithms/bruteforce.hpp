#ifndef __BRUTEFORCE_H__
#define __BRUTEFORCE_H__

#include <assert.h>
#include <vector>
#include <iostream>

template <typename Container>
bool bruteforceIterate(std::vector<Container> &values,
    std::vector<typename Container::iterator> &iterators)
{
    assert(values.size() == iterators.size());

    for(int i = iterators.size()-1; i >= 0; --i)
    {
        iterators[i]++;

        if(iterators[i] == values[i].end())
        {
            iterators[i] = values[i].begin();
        }
        else
        {
            return true;
        }
    }

    return false;
}

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

#endif
