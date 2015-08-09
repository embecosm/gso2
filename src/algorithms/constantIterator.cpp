#include <vector>
#include "constantIterator.hpp"
#include "bruteforce.hpp"

using namespace std;

constantIterator::constantIterator(vector<Slot*> &slotlist_)
{
    for(auto slot: slotlist_)
    {
        if(dynamic_cast<ConstantSlot*>(slot) != 0)
        {
            slotlist.push_back((ConstantSlot*)slot);
            slot->reset();
        }
    }
}

void constantIterator::setLossy(bool lossy)
{
    for(auto slot: slotlist)
    {
        slot->iteratorSkip(lossy);
        slot->reset();
    }
}

bool constantIterator::next()
{
    return bruteforceIterate(slotlist);
}
