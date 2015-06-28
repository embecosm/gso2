#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <stdint.h>
#include <vector>

template <typename T>
class Combinations
{
public:
    Combinations(std::vector<T> &values_, unsigned n_) : values(values_)
    {
        // TODO check values.size() <= 64
        n = n_;
        current = (1 << n) - 1;
        max = (current << (values.size() - n));
    }

    // TODO: should we return the actual values here, rather than the index of 
    // the value?
    std::vector<unsigned> getSelection()
    {
        uint64_t val = current;
        std::vector<unsigned> sel;

        for(unsigned i = 0; i < values.size(); ++i, val >>= 1)
        {
            if(val & 1)
                sel.push_back(i);
        }

        return sel;
    }

    bool next()
    {
        uint64_t smallest, ripple, ones, x = current;

        // snoob function. Next unsigned integer with the same number of one
        // bits set.
        smallest = x & -x;
        ripple = x + smallest;
        ones = x ^ ripple;
        ones = (ones >> 2) /smallest;

        current = ripple | ones;
        // End snoob function.

        if(current > max || (max == 1 && current == 1))
        {
            current = (1<<n)-1;
            return false;
        }
        else
            return true;
    }

private:
    uint64_t current, max;
    unsigned n;
    std::vector<T> &values;
};

#endif
