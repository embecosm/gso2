#include <algorithm>
#include <vector>
#include <unordered_set>
#include <iostream>

#include "canonical.hpp"

using namespace std;

bool nextCanonical(std::vector<Slot*> slotlist)
{
    // Generate classes skip list
    vector<unordered_set<unsigned>> skips;

    for(unsigned i = 0; i < slotlist.size(); ++i)
    {
        if(i == 0)
        {
            skips.push_back(unordered_set<unsigned>());
            continue;
        }

        auto rs_i   = (RegisterSlot *) slotlist[i];
        auto rs_im1 = (RegisterSlot *) slotlist[i-1];

        auto va_i   = rs_i->getValidArguments();
        auto va_im1 = rs_im1->getValidArguments();

        unordered_set<unsigned> current_skip(skips[i-1]);

        for(auto valid_register: va_i)
        {
            if(va_im1.find(valid_register) == va_im1.end())
            {
                // A register in the current class was not found in the
                // previous class, so add it to the skip list.
                current_skip.insert(valid_register);
            }
        }

        skips.push_back(current_skip);
    }

    for(int i = slotlist.size()-1; i > 0;)
    {
        auto rs_i   = (RegisterSlot *) slotlist[i];
        auto va_i   = rs_i->getValidArguments();

        // Get next highest valid argument
        unsigned current = rs_i->getValue();
        unsigned next    = *max_element(va_i.begin(), va_i.end())+1;
        bool found = false;
        for(auto reg: va_i)
        {
            if(reg < next && reg > current)
            {
                next = reg;
                found = true;
            }
        }

        if(!found)
        {
            rs_i->setValue(*min_element(va_i.begin(), va_i.end()));
            i--;
            continue;
        }

        rs_i->setValue(next);

        bool found_r = false;
        bool found_next = false;

        // PROBLEMS
        for(unsigned r = 0; r < next; ++r)
        {
            bool found = false;
            for(int j = 0; j < i; ++j)
            {
                auto rs_j = (RegisterSlot *) slotlist[j];
                if(rs_j->getValue() == r)
                {
                    found = true;
                }
                if(rs_j->getValue() == next)
                    found_next = true;
            }
            if(!found && va_i.find(r) != va_i.end())
            {
                found_r = true;
            }
        }

        if(found_r && !found_next) // && next not in prev slots
        {
            if(skips[i].find(next) != skips[i].end())
            {
                continue;
            }
            else
            {
                rs_i->setValue(*min_element(va_i.begin(), va_i.end()));
                i--;
                continue;
            }
        }
        else
            return true;
    }

    return false;
}


bool nextCanonicalBasic(std::vector<Slot*> slotlist)
{
    for(int i = slotlist.size()-1; i > 0; i--)
    {
        auto rs_i   = (RegisterSlot *) slotlist[i];
        auto va_i   = rs_i->getValidArguments();

        unsigned next = rs_i->getValue() + 1;

        unsigned max = 0;
        for(int j = 0; j < i; ++j)
            if(slotlist[j]->getValue() > max)
                max = slotlist[j]->getValue();

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
