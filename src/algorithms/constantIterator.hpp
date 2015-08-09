#ifndef __CONSTANTITERATOR_H__
#define __CONSTANTITERATOR_H__

#include <vector>
#include "../slots.hpp"

class constantIterator
{
public:
    constantIterator(std::vector<Slot*> &slotlist_);

    void setLossy(bool lossy);

    bool next();
private:
    std::vector<ConstantSlot*> slotlist;
};

#endif
