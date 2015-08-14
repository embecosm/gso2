#ifndef __CANONICALITERATORBASIC_H__
#define __CANONICALITERATORBASIC_H__

#include <vector>
#include <unordered_map>
#include "../slots.hpp"

class canonicalIteratorBasic
{
public:
    canonicalIteratorBasic(std::vector<Slot*> &slotlist_, int pre_maximum_=-1);

    void initialise();
    bool next();
protected:
    std::vector<RegisterSlot*> slotlist;

    int pre_maximum;
};

#endif
