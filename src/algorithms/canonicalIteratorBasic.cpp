#include <algorithm>
#include <vector>
#include <set>
#include <iostream>

#include "canonicalIteratorBasic.hpp"

using namespace std;

canonicalIteratorBasic::canonicalIteratorBasic(vector<Slot*> &slotlist_,
    int pre_maximum_)
{
    // We just want the slots which are RegisterSlots
    for(auto slot: slotlist_)
    {
        if(dynamic_cast<RegisterSlot*>(slot) != 0)
            slotlist.push_back((RegisterSlot*)slot);
    }
    pre_maximum = pre_maximum_;
}

bool canonicalIteratorBasic::next()
{
    // Iterate from the last slot in the sequence.
    for(int i = slotlist.size()-1; i >= 0; i--)
    {
        auto rs_i   = slotlist[i];
        auto va_i   = rs_i->getValidArguments();

        int next = rs_i->getValue() + 1;

        // Calculate what the maximum value currently in the sequence is.
        int max = pre_maximum;
        for(int j = 0; j < i; ++j)
        {
            int val = slotlist[j]->getValue();
            if(val > max)
                max = val;
        }

        // If the next value is too large, reset the current value and
        // continue with the next slot. Otherwise, we are done.
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
