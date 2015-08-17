#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <stdint.h>
#include <vector>
#include <string>
#include <functional>

/*! \class Combinations

    Construction a list of N combinations from SIZE possibilities. This class
    iterates over all possible combinations, returning a list of length N,
    with the selections. It is intended for these selections to be indices
    into another list with objects to be selected.

    Note: if SIZE is greater than 63 this will break.


    Example:
        Combinations c(4,2);

        c.getSelection(); // {0, 1}
        c.next();
        c.getSelection(); // {0, 2}
        c.next();
        c.getSelection(); // {0, 3}
        c.next();
        c.getSelection(); // {1, 2}
        ...
        c.next();
        c.getSelection(); // {2,3}
        c.next();         // Return false
        c.getSelection(); // {0,1}

    TODO: perhaps getSelection should be renamed to getIndices.
*/
class Combinations
{
public:
    /*! Initialise the combinations generator.

        This initialise the internal variables, such that an immediate call to
        getSelection returns the correct values (i.e. before calling next);

        @param size_    The number of objects to choose combinations from.
        @param n_       The number of objects to choose.

    */
    Combinations(unsigned size_, unsigned n_);

    /*! Return the list of current combinations.

        This returns a sorted list of the current combination chosen. The
        values are returned in the variable passed in the arguments list
        (reference) to avoid the recreation of the object, since it is
        expected that this function may be in a hot loop.

        @param sel      The list in which to put the current combinations.
    */
    void getSelection(std::vector<unsigned> &sel);

    /*! Advance to the next combination.

        This function computes the next combination from the current one,
        returning true if the entire sequence is finished and has wrapped
        around.

        @return True if all combinations have been iterated.
    */
    bool next();

private:
    uint64_t current, max;
    unsigned n, size;
};

class Instruction;
class Slot;

/*! Format the instruction sequence into a string.

    This function takes a list of instruction, and their corresponding slots,
    formatting it into a string. Each instruction is separated by a newline,
    and otherwise formatted as specified by the instruction.

    E.g.
        mov r0, r1
        add r1, r2

    @param insns    The instruction to format.
    @param slots    The slots corresponding to the instructions.
    @return         A string representing the instructions
*/
std::string print(const std::vector<Instruction *> &insns,
    std::vector<Slot*> &slots);


/*! Create a list of instruction from string input.

    This function accepts a string, and a list of functions which can
    construct an Instruction, creating a list of correct instructions and
    slots from the string. The factories list must contain functions which can
    construct a function. The string is parsed by splitting on newlines, and
    attempting to match each line to an Instruction generated in the order of
    the factories list.

    E.g.
        input = "mov r0, r1\nadd r1, r2"
        factories = {functions to generate mov_Instruction and add_Instruction}

        // after parseInstructionList has been called:

        insns = {pointer to mov_Instruction, pointer to add_Instruction}
        slots = {pointer to RegisterSlot * 4}

    @param input        The input string to parse
    @param factories    A list of functions, which return a pointer to a newly
                        allocated Instruction.
    @param insns        The list in which the instructions will be returned.
    @param slots        The list in which the corresponding slots will be
                        returned.
    @return             Whether or not the input string could be parsed
                        without errors.

*/
bool parseInstructionList(std::string input, std::vector<std::function<Instruction *()>> factories,
    std::vector<Instruction*> &insns, std::vector<Slot*> &slots);

#endif
