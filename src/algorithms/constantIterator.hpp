#ifndef __CONSTANTITERATOR_H__
#define __CONSTANTITERATOR_H__

#include <vector>
#include "../slots.hpp"

/*! \class constantIterator

    A class for iterating over constants. Currently this doesn't do much more
    than just store a list of ConstantSlots, and setting the lossy bit in them
    all at the same time.

    TODO: Should the logic for constant slot lossiness be moved into this
          class, just as there are separate iterators for RegisterSlots?
*/
class constantIterator
{
public:
    /*! Initialise the constant iterator

        This constructor takes a list of slots, and copies the pointers to
        constant slots into an internal array, which is used for iteration.

        @param slotlist_   A list of slots which may or may not contain
                           ConstantSlots to iterate over.
    */
    constantIterator(std::vector<Slot*> &slotlist_);

    /*! Set the lossy bit on the constant slots.

        The lossy bit restricts the range of the constant slots to some
        constants which are thought to be used more frequently than others.
        See ConstantSlot::iteratorSkip for more details.

        @param lossy    Whether or not to enable the lossy mode.
    */
    void setLossy(bool lossy);

    /*! Advance to the next constant

        This function goes through all the constant slots, iterating over
        every combination of values. If the full sequence has been iterated,
        false is returned.

        @return     Whether the iterator still has values left to iterate
                    over.
    */
    bool next();
private:
    std::vector<ConstantSlot*> slotlist;
};

#endif
