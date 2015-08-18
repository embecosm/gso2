#ifndef __CANONICALITERATORBASIC_H__
#define __CANONICALITERATORBASIC_H__

#include <vector>
#include <unordered_map>
#include "../slots.hpp"

/*! \class canonicalIteratorBasic

    Iterate over canonical forms of registers in the given slots. This is a
    simplified form, which does not abide by the register classes specified in
    each register slot. The values in the register slots are such that all
    possible renamings have been iterated during the course of the iterator.

    Example of canonical form:
        3 register slots
            0 0 0
            0 0 1
            0 1 0
            0 1 1
            0 1 2

        4 register slots
            0 0 0 0
            0 0 0 1
            0 0 1 0
            0 0 1 1
            0 0 1 2
            ...

    The algorithm for computing the next set of values is simple. Each slot is
    considered consecutively and incremented. If the slot's value becomes
    greater than the max of the sequence + 1, the value is reset to 0 and the
    next slot considered. Otherwise the next set of values has been found. See
    the design document distributed with the toolkit for more details.

    The class also accepts a pre_maximum parameter. This parameter tells the
    iterator to assume there were pre_maximum number of additional slots
    before the slots given. For example, 3 register slots, passed with
    pre_maximum=1 gives the following sequence:
        0 0 0
        0 0 1
        0 1 0
        0 1 1
        0 1 2
        1 0 0
        1 0 1
        1 0 2
        ...
    This parameter allows the canonicalIteratorBasic class to be used to just
    iterate over functional register renamings, narrowing the search space
    before trying to map a matching sequence to a correct register assignment.
    See the design document for more details.

*/
class canonicalIteratorBasic
{
public:
    /*! Initialise the canonical iterator.

        Pick the RegisterSlots from the slotslist_ and store the pointers
        internally.

        @param slotlist_        A list of slots. The RegisterSlots out of this
                                list are selected.
        @param pre_maximum_     Assume there are this number of extra slots
                                before the given slots.
    */
    canonicalIteratorBasic(std::vector<Slot*> &slotlist_, int pre_maximum_=-1);

    /*! Compute the next set of values in the canonical sequence.

        @return  True if there are further values to iterate, false if the last
                 value has already been iterated.
    */
    bool next();
protected:
    std::vector<RegisterSlot*> slotlist;

    int pre_maximum;
};

#endif
