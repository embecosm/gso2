#ifndef __CANONICALITERATORGENERIC_H__
#define __CANONICALITERATORGENERIC_H__

#include <vector>
#include <unordered_map>
#include "../slots.hpp"

#include "canonicalIteratorBasic.hpp"

/*! \class canonicalIterator
    Iteratively generate values for registers slots.

Each sequences of registers is generated in lexical order, taking into account
the set of valid registers for each slot. This can sometimes result in
sequences which appear not to be ordered correctly, but are when considering
the register classes of each slot.

*/

class canonicalIteratorGeneric : public canonicalIteratorBasic
{
public:
    /*!
       Constructor that takes the list of slots used in the instruction
       sequence. The constructor extracts the RegisterSlots from the entire
       list and stores the pointers.

       @param slotlist_  List of pointers to register slots
    */
    canonicalIteratorGeneric(std::vector<Slot*> &slotlist_);

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

std::pair<std::vector<unsigned>,bool> canonicalMapping(std::vector<RegisterSlot*> &slotlist, std::vector<unsigned> values = {},
    std::vector<std::vector<unsigned>> *class_intersections=nullptr);

std::pair<std::vector<unsigned>,bool> canonicalMapping(std::vector<Slot*> &slotlist, std::vector<unsigned> values = {},
    std::vector<std::vector<unsigned>> *class_intersections=nullptr);


#endif
