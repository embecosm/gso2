#ifndef __TEST_H__
#define __TEST_H__

#include <vector>
#include <functional>
#include "../frontend.hpp"
#include "../slots.hpp"

template <typename Machine>
bool testEquivalence(std::vector<Instruction *> insns, std::vector<Slot *> slots,
    Machine initial, Machine expected)
{
    auto current_slot = &slots[0];
    Machine test_state;

    test_state = initial;

    for(auto insn: insns)
    {
        auto n = insn->execute(&test_state, current_slot);
        current_slot += n;
    }

    return test_state == expected;
}

template <typename Machine>
bool testEquivalence(std::vector<Instruction *> insns, std::vector<Slot *> slots,
    Machine initial, std::function<void(Machine&)> transform)
{
    Machine transformed = initial;
    transform(transformed);

    return testEquivalence(insns, slots, initial, transformed);
}

template <typename Machine>
bool testEquivalence(std::vector<Instruction *> insns, std::vector<Slot *> slots,
    std::vector<Instruction *> reference_insns, std::vector<Slot *> reference_slots)
{
    // First do the instruction sequence under test
    auto current_slot = &slots[0];

    Machine test_state;
    Machine reference_state;

    test_state.initialiseRandom();
    reference_state = test_state;

    for(auto insn: insns)
    {
        auto n = insn->execute(&test_state, current_slot);
        current_slot += n;
    }

    // Execute the reference sequence
    current_slot = &slots[0];

    for(auto insn: reference_insns)
    {
        auto n = insn->execute(&reference_state, current_slot);
        current_slot += n;
    }

    return test_state == reference_state;
}

template <typename Machine>
bool testEquivalenceMultiple(std::vector<Instruction *> insns,
    std::vector<Slot *> slots, std::vector<Instruction *> reference_insns,
    std::vector<Slot *> reference_slots, unsigned number=100)
{
    for(int i = 0; i < number; ++i)
    {
        if(!testEquivalence<Machine>(insns, slots, reference_insns, reference_slots))
            return false;
    }

    return true;
}

template <typename Machine>
bool testEquivalenceMultiple(std::vector<Instruction *> insns,
    std::vector<Slot *> slots, std::function<void(Machine&)> transform,
    unsigned number=100)
{
    for(int i = 0; i < number; ++i)
    {
        Machine initial;
        initial.initialiseRandom();
        if(!testEquivalence<Machine>(insns, slots, initial, transform))
            return false;
    }

    return true;
}

#endif
