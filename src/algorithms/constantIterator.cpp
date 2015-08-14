#include <vector>
#include "constantIterator.hpp"
#include "bruteforce.hpp"

using namespace std;

constantIterator::constantIterator(vector<Slot*> &slotlist_)
{
    for(auto slot: slotlist_)
    {
        // See if any of the slots are ConstantSlots. If they are, reset them
        // and store the points in our own list.
        if(dynamic_cast<ConstantSlot*>(slot) != 0)
        {
            slotlist.push_back((ConstantSlot*)slot);
            slot->reset();
        }
    }
}

void constantIterator::setLossy(bool lossy)
{
    // Set the lossy bit on all slots. Reset them to their first value (not
    // strictly necessary, since iteratorSkip does this).
    for(auto slot: slotlist)
    {
        slot->iteratorSkip(lossy);
        slot->reset();
    }
}

bool constantIterator::next()
{
    // Iterate over all the slots we have
    return bruteforceIterate(slotlist);
}
