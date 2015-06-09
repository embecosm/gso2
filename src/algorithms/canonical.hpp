#ifndef __CANONICAL_H__
#define __CANONICAL_H__

#include <vector>
#include "../slots.hpp"

std::vector<std::vector<unsigned>> initialiseCanonical(std::vector<Slot*> slotlist);
bool nextCanonical(std::vector<Slot*> slotlist, const std::vector<std::vector<unsigned>> skips);
bool nextCanonicalBasic(std::vector<Slot*> slotlist);

#endif
