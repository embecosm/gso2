#ifndef __CANONICALITERATORLIVENESS_H__
#define __CANONICALITERATORLIVENESS_H__


#include <vector>
#include <unordered_map>
#include "../slots.hpp"

#include "canonicalIteratorGeneric.hpp"

class canonicalIteratorLiveness : public canonicalIteratorGeneric
{
public:
    canonicalIteratorLiveness(std::vector<Slot*> &slotlist_, unsigned live_registers);

    bool next();
private:
    unsigned n_live;
};

std::vector<unsigned> possibleRegisters(std::vector<RegisterSlot*> &slotlist, std::vector<unsigned> &values, unsigned loc);

std::pair<std::vector<unsigned>,bool> canonicalMappingLive(std::vector<RegisterSlot*> &slotlist, std::vector<unsigned> values,
    unsigned     live_registers,
    std::vector<std::vector<unsigned>> *class_intersections=nullptr);

#endif
