#ifndef __BRUTEFORCEBYCOST_HPP__
#define __BRUTEFORCEBYCOST_HPP__

#include <vector>
#include <set>
#include <map>
#include <functional>

/*! \class bruteforceByCost

    The class attempts to bruteforce a combination of containers, in ascending
    cost order. This class accepts a list of containers, a list of iterators
    to each container, and a cost function. The cost function gives the 'cost'
    of each list of iterators.

    Example:
        \code
            vector<vector<int>> list_of_values = {
                {0,1,2,3},
                {0,1,2,3},
                };
            vector<vector<int>::iterator> list_of_iterators;

            bruteforceByCost<vector<int>>  bfbc(list_of_values,
                list_of_iterators,
                [] (vector<vector<int>::iterator> &current)
                    {
                        int i = 0;

                        for(auto &it: current)
                            i += *it;

                        return i;
                    });

            // Initial set in list_of_iterators
            bfbc.next();
            // Next set is now in list_of_iterators
            bfbc.next();
            // Next set is now in list_of_iterators
            ...
        \endcode

        The above code specifies that there are two lists, to be bruteforced
        (i.e. cross product of both), therefore the following sequence would
        be obtained with a standard bruteforce.
        \code
            (0,0)
            (0,1)
            (0,2)
            (0,3)
            (1,0)
            (1,1)
            (1,2)
            ...
        \endcode

        However, the cost function given to bruteforceByCost specifies that
        the 'cost' of a sequence is given by the sum of its values. Therefore
        the sequence of iteration is:
        \code
            (0,0)
            (0,1)
            (1,0)
            (2,0)
            (1,1)
            (0,2)
            ...
        \endcode

        Overall the order of iteration over these two lists is:
        \code
            0  2  5  9
            1  4  8 12
            3  7 11 14
            6 10 13 15
        \endcode

    The class supports arbitrary cost functions, but does not initially
    bruteforce all possible combinations to find the lowest to start with.
    Instead, the current iterator is used (or initialise to the first element
    of each array), and each direction explored (in a flood fill style). The
    lowest cost found so far is returned each time next is called.
*/
template<typename Container, typename CostType=int>
class bruteforceByCost
{
public:
    typedef std::vector<Container> CType;
    typedef std::vector<typename Container::iterator> IType;

    /*! Construct the iterator.

        This initialises the class, and puts the first valid value into the
        iterators reference passed in.

        @param containers_          A list of containers, for which the cross
                                    product should be bruteforced.
        @param iterators_           The vector in which the iterators should
                                    be placed. This will be initialised if
                                    initialiseIterator is true, otherwise it
                                    is assumed to contain the first tuple of
                                    elements from which to be iterated.
        @param costFunction_        A function which will return a cost, when
                                    passed a list of iterators.
        @param initialiseIterator   Whether or not the iterators_ parameter
                                    passed in is initialised.
    */
    bruteforceByCost(CType &containers_, IType &iterators_,
        std::function<CostType(IType&)> costFunction_,
            bool initialiseIterator=true) :
        containers(containers_), iterators(iterators_),
        costFunction(costFunction_)
    {

        if(initialiseIterator)
        {
            iterators.clear();
            for(auto &c: containers)
                iterators.push_back(c.begin());
        }

        past_edge.insert(iterators);
        add_next_edge();
    }

    /*! Display a grid of the explored and to-be-explored edges stored. For debugging.
    */
    void display()
    {
        for(auto x = containers[0].begin(); x != containers[0].end(); ++x)
        {
            for(auto y = containers[1].begin(); y != containers[1].end(); ++y)
            {
                IType iter = {x,y};

                if(past_edge.count(iter) == 1)
                    std::cout << "E";
                else if(next_edge.count(iter) == 1)
                    std::cout << "N";
                else
                    std::cout << ".";
            }
            std::cout << "\n";
        }
    }

    /*! Compute the next item to be iterated.

        This function advances the iterator to the next item.

        TODO: this function should reset when it wraps around.

        @return True if there are more values to iterate over.
    */
    bool next()
    {
        if(next_edge.size() == 0)
        {
            return false;
        }
        else
        {
            auto next_key = min_element(next_edge.begin(), next_edge.end(),
                ComparePairSecond());

            IType old_iterators = iterators;
            iterators = next_key->first;
            next_edge.erase(next_key);

            add_next_edge();

            // For each connected iterator, check whether we should remove it
            // from the past edges. We can remove an iterator if itself has no
            // adjacent iterators in the next_edge.
            for(unsigned i = 0; i < containers.size(); ++i)
                for(int offset = -1; offset < 2; offset+=2)
                {
                    IType current_connected = iterators;

                    if(offset == -1 && current_connected[i] == containers[i].begin())
                        continue;

                    current_connected[i] += offset;

                    if(current_connected[i] == containers[i].end())
                        continue;

                    bool is_in_next_edge = false;

                    for(unsigned j = 0; j < containers.size(); ++j)
                        for(int offset = -1; offset < 2; offset+=2)
                        {
                            IType test_edge = current_connected;

                            if(offset == -1 && test_edge[j] == containers[j].begin())
                                continue;

                            test_edge[j] += offset;

                            if(test_edge[j] == containers[j].end())
                                continue;

                            if(next_edge.count(test_edge) == 1)
                                is_in_next_edge = true;
                        }

                    if(!is_in_next_edge && past_edge.count(current_connected))
                    {
                        past_edge.erase(current_connected);
                    }
                }

            past_edge.insert(iterators);

            return true;
        }
    }

private:
    CType &containers;
    IType &iterators;
    std::function<CostType(IType&)> costFunction;

    std::map<IType,CostType> next_edge;
    std::set<IType> past_edge;

    // A class to compare the second value in a pair type. Used with the map
    // above to find the minimum value.
    struct ComparePairSecond
    {
        typedef typename std::map<IType,CostType>::value_type PType;
        bool operator()(PType a, PType b) const
        {
            return a.second < b.second;
        }
    };

    /*! Add the adjacent iterators to the current iterator to the list of iterators
        to be explored. This is equivalent to four-way linkage in two
        dimensions. Each time called, it adds up to 2*containers.size()
        iterators to the list.
    */
    void add_next_edge()
    {
        for(unsigned i = 0; i < containers.size(); ++i)
            for(int offset = -1; offset < 2; offset+=2)
            {
                IType current = iterators;

                if(offset == -1 && current[i] == containers[i].begin())
                    continue;

                current[i] += offset;

                if(current[i] == containers[i].end())
                    continue;

                if(past_edge.count(current) == 1)
                    continue;

                next_edge[current] = costFunction(current);
            }
    }
};

#endif
