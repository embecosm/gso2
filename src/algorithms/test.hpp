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

template <typename Machine>
bool testEquivalence(std::vector<Instruction *> &insns, std::vector<Slot *> &slots,
    Machine &initial, Machine &expected)
{
    Machine test_state;

    test_state = initial;

    executeSequence(insns, slots, test_state);

    return test_state.containsState(expected);
}

template <typename Machine>
bool testEquivalence(std::vector<Instruction *> &insns, std::vector<Slot *> &slots,
    Machine &initial, std::function<void(Machine&)> transform)
{
    Machine transformed = initial;
    transform(transformed);

    return testEquivalence(insns, slots, initial, transformed);
}

template <typename Machine>
bool testEquivalence(std::vector<Instruction *> &insns, std::vector<Slot *> &slots,
    std::vector<Instruction *> &reference_insns, std::vector<Slot *> &reference_slots,
    std::vector<std::pair<unsigned, unsigned>> &register_mapping)
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
bool testEquivalence(std::vector<Instruction *> &insns, std::vector<Slot *> &slots,
    std::vector<Instruction *> &reference_insns, std::vector<Slot *> &reference_slots)
{
    std::vector<std::pair<unsigned,unsigned>> temporary;
    return testEquivalence<Machine>(insns, slots, reference_insns, reference_slots, temporary);
}

template <typename Machine>
bool testEquivalenceMultiple(std::vector<Instruction *> &insns,
    std::vector<Slot *> &slots, std::vector<Instruction *> reference_insns,
    std::vector<Slot *> reference_slots, unsigned number=100)
{
    std::vector<std::pair<unsigned,unsigned>> mapping;

    for(unsigned i = 0; i < number; ++i)
    {
        if(!testEquivalence<Machine>(insns, slots, reference_insns, reference_slots, mapping))
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
        if(!testEquivalence<Machine>(insns, slots, initial, transform, mapping))
            return false;
    }

    return true;
}

#endif
