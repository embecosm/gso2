#include <algorithm>
#include <vector>
#include <unordered_set>
#include <iostream>

#include "canonical.hpp"

using namespace std;

canonicalIterator::canonicalIterator(vector<Slot*> &slotlist_) :
    slotlist(slotlist_)
{
    for(unsigned i = 0; i < slotlist.size(); ++i)
    {
        if(i == 0)
        {
            skips.push_back(vector<unsigned>());
            continue;
        }

        auto rs_i   = (RegisterSlot *) slotlist[i];
        auto rs_im1 = (RegisterSlot *) slotlist[i-1];

        auto va_i   = rs_i->getValidArguments();
        auto va_im1 = rs_im1->getValidArguments();

        vector<unsigned> current_skip(skips[i-1]);

        for(auto valid_register: va_i)
        {
            if(!binary_search(va_im1.begin(), va_im1.end(), valid_register))
            {
                // A register in the current class was not found in the
                // previous class, so add it to the skip list.
                current_skip.push_back(valid_register);
            }
        }

        skips.push_back(current_skip);
    }
}

bool canonicalIterator::next()
{
    for(int i = slotlist.size()-1; i > 0;)
    {
        auto rs_i   = (RegisterSlot *) slotlist[i];
        auto va_i   = rs_i->getValidArguments();

        // Get next highest valid argument
        unsigned current = rs_i->getValue();
        unsigned next    = va_i.back()+1;
        bool found = false;
        for(auto reg: va_i)
        {
            if(reg < next && reg > current)
            {
                // Since va_i is sorted, we have found the lowest
                next = reg;
                found = true;
                break;
            }
        }

        if(!found)
        {
            rs_i->setValue(va_i.front());
            i--;
            continue;
        }

        rs_i->setValue(next);

        bool found_r = false;
        bool found_next = false;

        for(unsigned r = 0; r < next; ++r)
        {
            bool found = false;
            for(int j = 0; j < i; ++j)
            {
                auto rs_j_val = ((RegisterSlot *) slotlist[j])->getValue();
                if(rs_j_val == r)
                    found = true;
                if(rs_j_val == next)
                    found_next = true;
            }
            if(!found && binary_search(va_i.begin(), va_i.end(), r))
            {
                found_r = true;
            }
        }

        if(found_r && !found_next) // && next not in prev slots
        {
            if(binary_search(skips[i].begin(), skips[i].end(), next))
            {
                continue;
            }
            else
            {
                rs_i->setValue(va_i.front());
                i--;
                continue;
            }
        }
        else
            return true;
    }

    return false;
}

canonicalIteratorBasic::canonicalIteratorBasic(vector<Slot*> &slotlist_) :
    slotlist(slotlist_)
{}

bool canonicalIteratorBasic::next()
{
    for(int i = slotlist.size()-1; i > 0; i--)
    {
        auto rs_i   = (RegisterSlot *) slotlist[i];
        auto va_i   = rs_i->getValidArguments();

        unsigned next = rs_i->getValue() + 1;

        unsigned max = 0;
        for(int j = 0; j < i; ++j)
        {
            auto val = slotlist[j]->getValue();
            if(val > max)
                max = val;
        }

        if(next > max + 1)
        {
            rs_i->setValue(0);
            continue;
        }
        else
        {
            rs_i->setValue(next);
            return true;
        }
    }

    return false;
}
