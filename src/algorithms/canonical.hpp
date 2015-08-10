#ifndef __CANONICAL_H__
#define __CANONICAL_H__

#include <vector>
#include <unordered_map>
#include "../slots.hpp"

class canonicalIteratorBasic
{
public:
    canonicalIteratorBasic(std::vector<Slot*> &slotlist_);

    void initialise();
    bool next();
protected:
    std::vector<RegisterSlot*> slotlist;
    std::vector<std::vector<unsigned>> skips;
};

/*! \class canonicalIterator
    Iteratively generate values for registers slots.

Each sequences of registers is generated in lexical order, taking into account
the set of valid registers for each slot. This can sometimes result in
sequences which appear not to be ordered correctly, but are when considering
the register classes of each slot.

*/

class canonicalIterator : public canonicalIteratorBasic
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
protected:
    bool computed_intersections;
    std::vector<std::vector<unsigned>> class_intersections;
    std::vector<unsigned> canonical;
    unsigned max_class_size;

    bool canonicalStep();
    void precomputePossibleRegisters(std::vector<std::vector<unsigned>> classes);
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

class canonicalIteratorLiveness : public canonicalIterator
{
public:
    canonicalIteratorLiveness(std::vector<Slot*> &slotlist_, unsigned live_registers);

    bool next();
private:
    unsigned n_live;
};

std::vector<unsigned> possibleRegisters(std::vector<RegisterSlot*> &slotlist, std::vector<unsigned> &values, unsigned loc);

std::pair<std::vector<unsigned>,bool> canonicalMapping(std::vector<RegisterSlot*> &slotlist, std::vector<unsigned> values = {},
    std::vector<std::vector<unsigned>> *class_intersections=nullptr);

std::pair<std::vector<unsigned>,bool> canonicalMapping(std::vector<Slot*> &slotlist, std::vector<unsigned> values = {},
    std::vector<std::vector<unsigned>> *class_intersections=nullptr);

std::pair<std::vector<unsigned>,bool> canonicalMappingLive(std::vector<RegisterSlot*> &slotlist, std::vector<unsigned> values,
    unsigned     live_registers,
    std::vector<std::vector<unsigned>> *class_intersections=nullptr);

#endif
