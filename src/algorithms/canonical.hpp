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
    const std::vector<Slot*> &slotlist;
    std::vector<std::vector<unsigned>> skips;
};

class canonicalIteratorBasic
{
public:
    canonicalIteratorBasic(std::vector<Slot*> &slotlist_);

    bool next();
private:
    const std::vector<Slot*> &slotlist;
    std::vector<std::vector<unsigned>> skips;
};

class canonicalIteratorCommutative
{
public:
    canonicalIteratorCommutative(std::vector<Slot*> &slotlist_);

    bool next();
private:
    const std::vector<Slot*> &slotlist;
    std::vector<std::vector<unsigned>> skips;
    std::vector<std::pair<unsigned,unsigned>> commute_list;
};

class canonicalIteratorLiveness
{
public:
    canonicalIteratorLiveness(std::vector<Slot*> &slotlist_);

    bool next();
private:
    const std::vector<Slot*> &slotlist;
    std::vector<std::vector<unsigned>> skips;
    std::vector<std::pair<unsigned,unsigned>> commute_list;
    std::vector<unsigned> live_in, live_out;
};

#endif
