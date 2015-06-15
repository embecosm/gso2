#ifndef __CANONICAL_H__
#define __CANONICAL_H__

#include <vector>
#include "../slots.hpp"

class canonicalIterator
{
public:
    canonicalIterator(std::vector<Slot*> &slotlist_);

    bool next();
private:
    std::vector<Slot*> &slotlist;
    std::vector<std::vector<unsigned>> skips;
};

class canonicalIteratorBasic
{
public:
    canonicalIteratorBasic(std::vector<Slot*> &slotlist_);

    bool next();
private:
    std::vector<Slot*> &slotlist;
    std::vector<std::vector<unsigned>> skips;
};


#endif
