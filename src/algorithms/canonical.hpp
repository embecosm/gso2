#ifndef __CANONICAL_H__
#define __CANONICAL_H__

#include <vector>
#include "../slots.hpp"

/*! \class canonicalIterator
    Iteratively generate values for registers slots.

Each sequences of registers is generated in lexical order, taking into account
the set of valid registers for each slot. This can sometimes result in
sequences which appear not to be ordered correctly, but are when considering
the register classes of each slot.

*/

class canonicalIterator
{
public:
    /*!
       Constructor that takes the list of slots used in the instruction
       sequence. The constructor extracts the RegisterSlots from the entire
       list and stores the pointers.

       @param slotlist_  List of pointers to register slots
    */
    canonicalIterator(std::vector<Slot*> &slotlist_);

    /*!
        Advance the stored RegisterSlots to the next sequence.

        @return True if the set sequence is valid, false if the sequence has
            wrapped around.
    */
    bool next();
private:
    std::vector<Slot*> slotlist;
    std::vector<std::vector<unsigned>> skips;
};

/// \class canonicalIteratorBasic

class canonicalIteratorBasic
{
public:
    canonicalIteratorBasic(std::vector<Slot*> &slotlist_);

    bool next();
private:
    std::vector<Slot*> slotlist;
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
