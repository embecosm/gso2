#ifndef __TEST_H__
#define __TEST_H__

#include <vector>
#include <functional>
#include "../frontend.hpp"
#include "../slots.hpp"

template <typename Machine>
void executeSequence(std::vector<Instruction *> &insns, std::vector<Slot *> &slots,
    Machine &state)
{
    auto current_slot = &slots[0];

    for(auto insn: insns)
    {
        auto n = insn->execute(&state, current_slot);
        current_slot += n;
    }
}

/*! Test the equivalence of an instruction sequence, when given an initial state
    and expected state. Permutations of the state will be checked, i.e. if the
    result was expected in r0, but actually appeared in r1, the state will be
    accepted. See TargetMachine::containsState for more details.

    This function is mostly a simple forwarder to executeSequence and
    containsState.

    TODO maybe this function should accept a register_mapping parameter.

    @param insns                The instruction sequence.
    @param slots                The list of slots.
    @param initial              The initial test vector.
    @param expected             The expected machine state.
    @param register_mapping     A mapping between registers for the sequence
                                under test. See TargetMachine::containsState
                                for more information.
    @return                     Is the state obtained when applying the
                                instructions to the initial parameter
                                contained within the expected state.
*/
template <typename Machine>
bool testEquivalence(std::vector<Instruction *> &insns, std::vector<Slot *> &slots,
    Machine &initial, Machine &expected,
    std::vector<std::pair<unsigned, unsigned>> *register_mapping=nullptr)
{
    Machine test_state;

    test_state = initial;

    executeSequence(insns, slots, test_state);

    return test_state.containsState(expected, register_mapping);
}

template <typename Machine>
bool testEquivalence(std::vector<Instruction *> &insns, std::vector<Slot *> &slots,
    Machine &initial, std::function<void(Machine&)> transform,
    std::vector<std::pair<unsigned, unsigned>> *register_mapping=nullptr)
{
    Machine transformed = initial;
    transform(transformed);

    return testEquivalence(insns, slots, initial, transformed, register_mapping);
}

template <typename Machine>
bool testEquivalence(std::vector<Instruction *> &insns, std::vector<Slot *> &slots,
    std::vector<Instruction *> &reference_insns, std::vector<Slot *> &reference_slots,
    std::vector<std::pair<unsigned, unsigned>> *register_mapping=nullptr)
{
    // First do the instruction sequence under test
    Machine test_state;
    Machine reference_state;

    test_state.initialiseRandom();
    reference_state = test_state;

    executeSequence(insns, slots, test_state);

    // Execute the reference sequence
    executeSequence(reference_insns, reference_slots, reference_state);

    return test_state.containsState(reference_state, register_mapping);
}


template <typename Machine>
bool testEquivalenceMultiple(std::vector<Instruction *> &insns,
    std::vector<Slot *> &slots, std::vector<Instruction *> reference_insns,
    std::vector<Slot *> reference_slots, unsigned number=100)
{
    std::vector<std::pair<unsigned,unsigned>> mapping;

    for(unsigned i = 0; i < number; ++i)
    {
        if(!testEquivalence<Machine>(insns, slots, reference_insns, reference_slots, &mapping))
            return false;
    }

    return true;
}

template <typename Machine>
bool testEquivalenceMultiple(std::vector<Instruction *> &insns,
    std::vector<Slot *> &slots, std::function<void(Machine&)> transform,
    unsigned number=100)
{
    std::vector<std::pair<unsigned,unsigned>> mapping;

    for(unsigned i = 0; i < number; ++i)
    {
        Machine initial;
        initial.initialiseRandom();
        if(!testEquivalence<Machine>(insns, slots, initial, transform, &mapping))
            return false;
    }

    return true;
}

#endif
