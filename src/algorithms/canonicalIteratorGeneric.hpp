#ifndef __CANONICALITERATORGENERIC_H__
#define __CANONICALITERATORGENERIC_H__

#include <vector>
#include <unordered_map>
#include "../slots.hpp"

#include "canonicalIteratorBasic.hpp"

/*! \class canonicalIteratorGeneric

    Iteratively generate values for registers slots.

    This iterator is similar to canonicalIteratorGeneric, except it accounts
    for the register classes specified by each RegisterSlot.

*/

class canonicalIteratorGeneric : public canonicalIteratorBasic
{
public:
    /*! Constructor that takes the list of slots used in the instruction
        sequence.

        The constructor extracts the RegisterSlots from the entire
        list and stores the pointers.

        @param slotlist_  List of pointers to register slots
    */
    canonicalIteratorGeneric(std::vector<Slot*> &slotlist_);

    /*! Advance the stored RegisterSlots to the next sequence.

        @return  True if the set sequence is valid, false if the sequence has
                 wrapped around.
    */
    bool next();
protected:
    bool computed_intersections;
    std::vector<std::vector<unsigned>> class_intersections;
    std::vector<unsigned> canonical;
    unsigned max_class_size;

    /*! Perform a single standard canonical step.

        This performs a standard canonical step on the canonical variable.

        @return     True if there are further values to enumerate.
    */
    bool canonicalStep();

    /*! Precompute the register classes for possible combinations of classes.

        This function precomputes the intersection of all combinations of
        register classes passed in. The result is stored in the
        class_intersections variable. For the index, each bit represents
        whether a particular register class is in the intersection or not.

        @param classes  The list of register classes to compute intersections
                        for. The number of classes is large: 2^classes.size()
    */
    void precomputePossibleRegisters(std::vector<std::vector<unsigned>> classes);
};

/*! Attempt to map a set of values onto correct register classes.

    This function takes a set of values, which define the current registers of
    each slot. These need not be valid by the RegisterSlot::getValidArguments
    call, instead representing which slots must take the same register.

    The function uses this information and attempt to rename each register so
    that all registers satisfy the register class of each slot.

    The function also accepts a class_intersections parameter which allows
    some of the computation to be precomputed. This parameter must be a list
    of 2^N length, where N is the largest class ID given by a RegisterSlot.
    Each entry in the list must be the intersections of the classes identified
    by its index. The index has a bit set for each class ID. For example, the
    intersection of class IDs 0, 3 and 4 would be at index 0b00011001, or 25.

    @param slotlist             The list of RegisterSlots to map onto correct
                                values
    @param values               A list of slot values which should be used
                                with the slotlist. If this is not set, then
                                the list of values is taken from each slot,
                                via the standard Slot::getValue call.
    @param class_intersections  If specified, this parameter has the
                                precomputed intersections between all
                                combinations of classes. If it is not
                                specified, a slow method is used. However,
                                this slower method can cope with RegisterSlots
                                without a class ID assigned, or when the
                                memory tradeoff to precompute would be
                                extreme.
*/
std::pair<std::vector<unsigned>,bool> canonicalMapping(
    std::vector<RegisterSlot*> &slotlist,
    std::vector<unsigned> values = {},
    std::vector<std::vector<unsigned>> *class_intersections=nullptr);

/*! This function forwards to the RegisterSlot version of canonicalMapping.

    The RegisterSlots are extracted from the given slot list, and passed on to
    canonicalMapping. See canonicalMapping.

    @param slotlist             The list of slots to map onto correct values.
    @param values               A substitute list of values for the register
                                slots.
    @param class_intersections  A precomputed list of the register class
                                intersections. See the other form of
                                canonicalMapping for more details.

*/
std::pair<std::vector<unsigned>,bool> canonicalMapping(
    std::vector<Slot*> &slotlist,
    std::vector<unsigned> values = {},
    std::vector<std::vector<unsigned>> *class_intersections=nullptr);


#endif
